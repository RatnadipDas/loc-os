#include "virtio_disk.h"

#include "alloc.h"
#include "arg.h"
#include "utils.h"
#include "virtio.h"

/**
 * @brief Pointer to the Virtqueue used for sending block device requests.
 *
 * This queue is used by the driver to enqueue read/write requests which the VirtIO block device processes.
 */
struct virtio_virtq *blk_request_vq;

/**
 * @brief Pointer to the current VirtIO block request structure.
 *
 * Contains information such as the operation type (read/write), target sector,
 * data buffer, and status field for the request.
 */
struct virtio_blk_req *blk_req;

/**
 * @brief Physical address of the block request structure.
 *
 * This is the physical memory address of `blk_req` and is shared with the device
 * so it can access the memory for processing I/O.
 */
paddr_t blk_req_paddr;

/**
 * @brief Capacity of the VirtIO block device in sectors.
 *
 * Represents the total number of 512-byte sectors that the block device can store.
 */
unsigned blk_capacity;

/**
 * @brief Reads a 32-bit value from a VirtIO device register.
 *
 * @param offset Offset (in bytes) from the base physical address of the VirtIO block device.
 * @return The 32-bit value read from the specified register.
 */
uint32_t virtio_reg_read32(unsigned offset) {
    return *(volatile uint32_t *)(VIRTIO_BLK_PADDR + offset);
}

/**
 * @brief Reads a 64-bit value from a VirtIO device register.
 *
 * @param offset Offset (in bytes) from the base physical address of the VirtIO block device.
 * @return The 64-bit value read from the specified register.
 */

uint64_t virtio_reg_read64(unsigned offset) {
    return *(volatile uint64_t *)(VIRTIO_BLK_PADDR + offset);
}

/**
 * @brief Writes a 32-bit value to a VirtIO device register.
 *
 * @param offset Offset (in bytes) from the base physical address of the VirtIO block device.
 * @param value The 32-bit value to write.
 */
void virtio_reg_write32(unsigned offset, uint32_t value) {
    *(volatile uint32_t *)(VIRTIO_BLK_PADDR + offset) = value;
}

/**
 * @brief Performs a read-modify-write on a 32-bit VirtIO register with a bitwise OR operation.
 *
 * This function reads the current value of the register, performs a bitwise OR with the provided value,
 * and writes the result back to the register.
 *
 * @param offset Offset (in bytes) from the base physical address of the VirtIO block device.
 * @param value The value to OR with the existing register value.
 */
void virtio_reg_fetch_and_or32(unsigned offset, uint32_t value) {
    virtio_reg_write32(offset, virtio_reg_read32(offset) | value);
}

/**
 * @brief Checks if the Virtqueue has pending requests still being processed by the device.
 *
 * Compares the driver's `last_seen_used_index` with the device-maintained `used.index`.
 * If they are unequal, it means the device has not yet completed all submitted requests.
 *
 * @param vq Pointer to the VirtIO virtqueue structure to check.
 * @return `true` if there are unprocessed requests, `false` otherwise.
 */
bool virtq_is_busy(struct virtio_virtq *vq) {
    // last_used_index tracks curent position where used.index should be
    // after completing all the operations, if they are not equal that
    // means Device has not processed all the requests yet.
    return vq->last_seen_used_index != *vq->used_index;
}

/**
 * @brief Notifies the VirtIO device of a new request in the virtqueue.
 *
 * Adds the descriptor index to the available ring and updates the index,
 * then writes to the device's queue notify register to inform it that
 * a new buffer is ready for processing.
 *
 * @param vq Pointer to the VirtIO virtqueue.
 * @param desc_index Index of the head descriptor of the new request.
 */
void virtq_kick(struct virtio_virtq *vq, int desc_index) {
    vq->avail.ring[vq->avail.index % VIRTQ_ENTRY_NUM] = desc_index;
    vq->avail.index++;
    __sync_synchronize();
    // Notifies the VirtIO device that a new request is available in the queue
    // with queue number: vq->queue_index
    virtio_reg_write32(VIRTIO_REG_QUEUE_NOTIFY, vq->queue_index);
    vq->last_seen_used_index++;
}

/**
 * @brief Initializes a VirtIO virtqueue for communication with the device.
 *
 * This function allocates memory for a virtqueue, sets up the queue structure,
 * and informs the VirtIO device of the queue's location and configuration.
 *
 * The initialization process follows the VirtIO specification:
 * 1. Selects the desired queue by writing its index to the QueueSel register.
 * 2. Sets the queue size via QueueNum.
 * 3. Configures the memory alignment with QueueAlign.
 * 4. Informs the device of the queue’s physical address using QueuePFN.
 *
 * @param index Index of the virtqueue to initialize (usually 0 for the first queue).
 * @return Pointer to the initialized `struct virtio_virtq`.
 */
struct virtio_virtq *virtq_init(unsigned index) {
    paddr_t virtq_paddr = alloc_pages(align_up(sizeof(struct virtio_virtq), PAGE_SIZE) / PAGE_SIZE);
    struct virtio_virtq *vq = (struct virtio_virtq *)virtq_paddr;
    vq->queue_index = index;
    vq->used_index = (volatile uint16_t *)&vq->used.index;

    // 1. Select the queue by writing its index (first queue is 0) to QueueSel.
    virtio_reg_write32(VIRTIO_REG_QUEUE_SEL, index);

    // 2. Notify the device about the queue size by writing the size to QueueNum.
    virtio_reg_write32(VIRTIO_REG_QUEUE_NUM, VIRTQ_ENTRY_NUM);

    // 3. Notify the device about the alignment (0 = default PAGE_SIZE alignment).
    virtio_reg_write32(VIRTIO_REG_QUEUE_ALIGN, 0);

    // 4. Provide the physical address of the queue.
    virtio_reg_write32(VIRTIO_REG_QUEUE_PFN, virtq_paddr);
    return vq;
}

void init_virtio_blk(void) {
    INFO("Initializing virtio block...");

    if (virtio_reg_read32(VIRTIO_REG_MAGIC) != 0x74726976)
        PANIC("virtio: invalid magic value");
    if (virtio_reg_read32(VIRTIO_REG_VERSION) != 1)
        PANIC("virtio: invalid version");
    if (virtio_reg_read32(VIRTIO_REG_DEVICE_ID) != VIRTIO_DEVICE_BLK)
        PANIC("virtio: invalid device id");

    // 1. Reset the device.
    virtio_reg_write32(VIRTIO_REG_DEVICE_STATUS, VIRTIO_STATUS_RESET);

    // 2. Guest OS has detected the device (Set the Acknowledgement bit).
    virtio_reg_fetch_and_or32(VIRTIO_REG_DEVICE_STATUS, VIRTIO_STATUS_ACK);

    // 2. Guest OS understands the device type (Set the DRIVER status bit).
    virtio_reg_fetch_and_or32(VIRTIO_REG_DEVICE_STATUS, VIRTIO_STATUS_DRIVER);

    // 3. Feature negotiation is skipped; immediately accept.
    virtio_reg_fetch_and_or32(VIRTIO_REG_DEVICE_STATUS, VIRTIO_STATUS_FEAT_OK);

    // 5. Set up the first virtqueue (queue 0).
    blk_request_vq = virtq_init(0);

    // 6. Driver is ready to use the device.
    virtio_reg_write32(VIRTIO_REG_DEVICE_STATUS, VIRTIO_STATUS_DRIVER_OK);

    // 7. Read block device capacity.
    blk_capacity = virtio_reg_read64(VIRTIO_REG_DEVICE_CONFIG + 0) * SECTOR_SIZE;
    INFO("virtio block: capacity is %d bytes", blk_capacity);

    // 8. Allocate request buffer.
    blk_req_paddr = alloc_pages(align_up(sizeof(*blk_req), PAGE_SIZE) / PAGE_SIZE);
    blk_req = (struct virtio_blk_req *)blk_req_paddr;

    OK("Initialized virtio block.");
}

void read_write_disk(void *buf, unsigned sector, bool is_write) {
    if (sector >= blk_capacity / SECTOR_SIZE) {
        FAILED("virtio block: tried to read/write sector=%d, but capacity is %d", sector, blk_capacity / SECTOR_SIZE);
        return;
    }

    // Construct the request according to the virtio-blk specification.
    //
    // struct virtio_blk_req {
    //     uint32_t type;
    //     uint32_t reserved;
    //     uint64_t sector;
    //     uint8_t data[512];
    //     uint8_t status;
    // } __attribute__((packed));
    //
    blk_req->sector = sector;
    blk_req->type = is_write ? VIRTIO_BLK_T_OUT : VIRTIO_BLK_T_IN;
    if (is_write)
        memcpy(blk_req->data, buf, SECTOR_SIZE);

    // Construct the virtqueue descriptors (using 3 descriptors).
    // Descriptor 0: Header (type, reserved, sector)
    struct virtio_virtq *vq = blk_request_vq;
    vq->descs[0].addr = blk_req_paddr;
    vq->descs[0].len = sizeof(uint32_t) * 2 + sizeof(uint64_t);  // uint32_t type, uint32_t reserved, uint36_t sector;
    vq->descs[0].flags = VIRTQ_DESC_F_NEXT;
    vq->descs[0].next = 1;

    // Descriptor 1: Data buffer (read or write)
    vq->descs[1].addr = blk_req_paddr + offsetof(struct virtio_blk_req, data);
    vq->descs[1].len = SECTOR_SIZE;  // uint8_t data[512];
    vq->descs[1].flags = VIRTQ_DESC_F_NEXT | (is_write ? 0 : VIRTQ_DESC_F_WRITE);
    vq->descs[1].next = 2;

    vq->descs[2].addr = blk_req_paddr + offsetof(struct virtio_blk_req, status);
    vq->descs[2].len = sizeof(uint8_t);       // uint8_t status;
    vq->descs[2].flags = VIRTQ_DESC_F_WRITE;  // means you can write to this descriptor

    // Descriptor 2: Status byte (write-only for device)
    virtq_kick(vq, 0);

    // Wait until the device finishes processing.
    while (virtq_is_busy(vq));

    // Check request status. If a non-zero value is returned, it's an error.
    if (blk_req->status != 0) {
        FAILED("virtio block: failed to read/write sector=%d status=%d", sector, blk_req->status);
        return;
    }

    // For read operations, copy data from the request into the caller's buffer.
    if (!is_write)
        memcpy(buf, blk_req->data, SECTOR_SIZE);
}
