#pragma once
#include "types.h"

/**
 * @brief Definitions and structures for VirtIO block device communication.
 */

/** Physical memory-mapped address for the VirtIO block device registers */
#define VIRTIO_BLK_PADDR 0x10001000

/** Block device sector size in bytes */
#define SECTOR_SIZE 512

/** VirtIO block request types */
#define VIRTIO_BLK_T_IN 0            /**< Read a sector from the device. */
#define VIRTIO_BLK_T_OUT 1           /**< Write a sector to the device. */
#define VIRTIO_BLK_T_FLUSH 4         /**< Flush device write cache. */
#define VIRTIO_BLK_T_DISCARD 11      /**< Discard (trim) sectors. */
#define VIRTIO_BLK_T_WRITE_ZEROES 13 /**< Write zeroes to sectors. */

/**
 * @brief Represents a request to the VirtIO block device.
 *
 * This structure is used to submit a read or write request to the VirtIO block device.
 * It contains the operation type, target sector, a data buffer, and a status field
 * which is set by the device after the request is completed.
 */
struct virtio_blk_req {
    uint32_t type; /**< Operation code (see VIRTIO_BLK_T_* defines). */

    uint32_t reserved; /**< Reserved for future use. Must be set to 0. */

    uint64_t sector; /**< Sector number to read/write.
                      *   Each sector is 512 bytes.
                      *   Sector n -> byte range: [n * 512, (n + 1) * 512 - 1]
                      */

    uint8_t data[512]; /**< Data buffer for reading from or writing to the sector. */

    uint8_t status; /**< Status returned by the device after processing:
                     *   - 0: Success
                     *   - 1: I/O error
                     *   - 2: Unsupported request
                     */
} __attribute__((packed));

/**
 * @brief Initializes the VirtIO block device.
 *
 * This function performs full initialization of the VirtIO block device according to the VirtIO specification.
 * It validates the device, resets it, performs the feature negotiation handshake, sets up the virtqueue,
 * and prepares memory for sending block requests.
 *
 * Initialization Steps:
 * 1. Validate the VirtIO device using magic value, version, and device ID.
 * 2. Reset the device and set the ACK and DRIVER status bits to indicate driver presence.
 * 3. Feature negotiation: since no features are used, directly set FEATURES_OK.
 * 4. Initialize the request virtqueue (queue index 0).
 * 5. Set the DRIVER_OK bit to signal that the driver is ready.
 * 6. Read block device capacity from configuration space.
 * 7. Allocate memory for block I/O request structure.
 */
void init_virtio_blk(void);

/**
 * @brief Performs a read or write operation on the VirtIO block device.
 *
 * This function prepares and submits a request to the VirtIO block device to either
 * read from or write to a specific sector. It constructs the required descriptors,
 * submits the request to the device, waits for completion, and then processes the result.
 *
 * @param buf      Pointer to the memory buffer to read into or write from (must be 512 bytes).
 * @param sector   Sector number to read/write. Each sector is 512 bytes.
 * @param is_write Set to true to perform a write operation, false for a read.
 */
void read_write_disk(void *buf, unsigned sector, bool is_write);
