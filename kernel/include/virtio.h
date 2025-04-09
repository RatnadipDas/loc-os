#pragma once
#include "lib.h"
#include "types.h"

/**
 * @brief Definitions for VirtIO block device registers, statuses, and descriptor flags.
 *
 * These macros define offsets, flags, and constants used for interfacing
 * with a VirtIO block device. They help control device initialization,
 * queue management, and descriptor behavior.
 */

/** Number of descriptors in the virtqueue */
#define VIRTQ_ENTRY_NUM 16

/** Device type identifier for VirtIO block device */
#define VIRTIO_DEVICE_BLK 2

/** Device magic value: should be 0x74726976 ('virt') */
#define VIRTIO_REG_MAGIC 0x00

/** Device version register offset */
#define VIRTIO_REG_VERSION 0x04

/** Device ID register: identifies type (2 = block device) */
#define VIRTIO_REG_DEVICE_ID 0x08

/** Selects the current virtqueue being configured */
#define VIRTIO_REG_QUEUE_SEL 0x30

/** Maximum number of descriptors supported by the device */
#define VIRTIO_REG_QUEUE_NUM_MAX 0x34

/** Actual number of descriptors to be allocated in the queue */
#define VIRTIO_REG_QUEUE_NUM 0x38

/** Memory alignment of the virtqueue structure */
#define VIRTIO_REG_QUEUE_ALIGN 0x3c

/** Physical page number (PPN) of the virtqueue descriptor table */
#define VIRTIO_REG_QUEUE_PFN 0x40

/** Virtqueue ready register: 1 = ready, 0 = not ready */
#define VIRTIO_REG_QUEUE_READY 0x44

/** Notify device of a new entry in the specified queue */
#define VIRTIO_REG_QUEUE_NOTIFY 0x50

/** Device status register: manage negotiation and driver status */
#define VIRTIO_REG_DEVICE_STATUS 0x70

/** Start of device-specific configuration space */
#define VIRTIO_REG_DEVICE_CONFIG 0x100

/** Initial status: device is reset */
#define VIRTIO_STATUS_RESET 0

/** Driver has acknowledged the device */
#define VIRTIO_STATUS_ACK 1

/** Driver understands the device type */
#define VIRTIO_STATUS_DRIVER 2

/** Driver is fully ready and operational */
#define VIRTIO_STATUS_DRIVER_OK 4

/** Feature negotiation is complete */
#define VIRTIO_STATUS_FEAT_OK 8

/** Descriptor flag: descriptor continues in a chain */
#define VIRTQ_DESC_F_NEXT 1

/** Descriptor flag: buffer is write-only (used by device to write) */
#define VIRTQ_DESC_F_WRITE 2

/** Available ring flag: suppress interrupt after operation (guest to device) */
#define VIRTQ_AVAIL_F_NO_INTERRUPT 1

/** Used ring flag: suppress notification after device used buffer (device to guest) */
#define VIRTQ_USED_F_NO_INTERRUPT 1

/**
 * @brief Virtqueue Descriptor structure for VirtIO devices.
 *
 * Each descriptor describes a buffer that the device will read from or write to.
 * Descriptors can be chained together to form complex data structures using the `next` field.
 * This structure is used in the descriptor table of a virtqueue.
 *
 * @note This structure must be packed and laid out exactly as specified by the VirtIO specification.
 *
 * @link https://blogs.oracle.com/linux/post/introduction-to-VirtIO
 */
struct virtq_desc {
    uint64_t addr;  ///< Physical address of the data buffer. The device reads from or writes to this memory.
    uint32_t len;   ///< Length of the data buffer in bytes.
    uint16_t flags; /**< Descriptor flags:
                     * - `VIRTQ_DESC_F_NEXT` (1): This descriptor continues at the `next` index.
                     * - `VIRTQ_DESC_F_WRITE` (2): The buffer is write-only for the device (i.e., output buffer).
                     */
    uint16_t next;  ///< Index of the next descriptor in the chain, if `VIRTQ_DESC_F_NEXT` is set.
} __attribute__((packed));

/**
 * @brief Virtqueue Available Ring (Driver Area) structure for VirtIO devices.
 *
 * This structure is maintained by the driver to inform the device about buffers
 * that are ready for processing. It is part of the virtqueue and follows the
 * descriptor table in memory.
 *
 * @note The layout and usage must conform to the VirtIO specification.
 *
 * @link https://blogs.oracle.com/linux/post/introduction-to-VirtIO
 */
struct virtq_avail {
    uint16_t flags;                  /**< Flags controlling ring behavior:
                                      * - `VIRTQ_AVAIL_F_NO_INTERRUPT` (1): Suppresses interrupts from the device
                                      *   when buffers are consumed.
                                      */
    uint16_t index;                  /**< Index of the next slot the driver will write into the head of descriptor chain.
                                      * It increments after each new buffer is made available.
                                      */
    uint16_t ring[VIRTQ_ENTRY_NUM];  ///< Array of descriptor indices representing available buffers.
} __attribute__((packed));

/**
 * @brief Virtqueue Used Ring element structure.
 *
 * Each element in the Used Ring corresponds to a completed buffer that the device has finished processing.
 * These elements are written by the device and read by the driver.
 *
 * @link https://blogs.oracle.com/linux/post/introduction-to-VirtIO
 */
struct virtq_used_elem {
    uint32_t id;  /**< Index of the descriptor in the descriptor table (`virtq_desc`)
                   *   that the device has used. This allows the driver to match responses
                   *   to requests.
                   */
    uint32_t len; /**< Number of bytes the device wrote into the total chained buffer (for write operations).
                   *   May be less than or equal to the buffer size originally submitted.
                   */
} __attribute__((packed));

/**
 * @brief Virtqueue Used Ring (Device Area) structure.
 *
 * The Used Ring is written by the device to inform the driver which buffer descriptors
 * have been processed. Each entry corresponds to a completed request.
 *
 * @link https://blogs.oracle.com/linux/post/introduction-to-VirtIO
 */
struct virtq_used {
    uint16_t flags;                               /**< Flags controlling ring behavior:
                                                   * - `VIRTQ_USED_F_NO_INTERRUPT` (1): Suppresses interrupts from the device
                                                   *   when used buffers are written.
                                                   */
    uint16_t index;                               /**< Index into the Used Ring where the next completed descriptor will be written.
                                                   *   Increments as the device finishes processing descriptors.
                                                   */
    struct virtq_used_elem ring[VIRTQ_ENTRY_NUM]; /**< Array of elements indicating which descriptors
                                                   *   have been used and how many bytes were written.
                                                   */
} __attribute__((packed));

/**
 * @brief Represents a VirtIO virtqueue used for communication between the driver and the device.
 *
 * A virtqueue consists of three main parts: the descriptor table, the available ring (driver to device),
 * and the used ring (device to driver). This structure encapsulates all components along with bookkeeping
 * fields necessary for proper queue operation.
 *
 * @link https://blogs.oracle.com/linux/post/introduction-to-VirtIO
 */
struct virtio_virtq {
    struct virtq_desc descs[VIRTQ_ENTRY_NUM];                   /**< Array of descriptors representing buffer locations. */
    struct virtq_avail avail;                                   /**< Available ring: driver informs device of ready descriptors. */
    struct virtq_used used __attribute__((aligned(PAGE_SIZE))); /**< Used ring: device informs driver of completed descriptors. */

    int queue_index;               /**< Index of the virtqueue. Used to identify the queue in device registers. */
    volatile uint16_t *used_index; /**< Tracks the current position in the used ring. Points to used.index, updated by Device. */
    uint16_t last_seen_used_index; /**< Tracks the last seen used.index should be. */
} __attribute__((packed));
