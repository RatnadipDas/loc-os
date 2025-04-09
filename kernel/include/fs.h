#pragma once
#include "arg.h"
#include "types.h"
#include "virtio_disk.h"

/**
 * @brief Constants related to file storage on disk.
 *
 * - `FILES_MAX` defines the maximum number of files that can be stored.
 * - `DISK_MAX_SIZE` calculates the total disk space required to store all file
 *   metadata structures, rounded up to the nearest sector size to ensure proper alignment.
 *   This ensures that the file metadata fits within whole disk sectors, as required by
 *   most block-based storage systems.
 */
#define FILES_MAX 2
#define DISK_MAX_SIZE align_up(sizeof(struct file) * FILES_MAX, SECTOR_SIZE)

/**
 * @struct tar_header
 * @brief Represents the metadata header of a file in a TAR archive (USTAR format).
 *
 * This structure defines the layout of a TAR file header used to store file metadata
 * such as name, size, permissions, and ownership. The fields are stored as fixed-width
 * character arrays, mostly in octal string format, to maintain compatibility with the
 * traditional TAR format.
 *
 * The structure is packed to match the exact byte layout of TAR headers on disk.
 */
struct tar_header {
    char name[100];      // File name.
    char mode[8];        // File permissions (octal string).
    char uid[8];         // User ID (octal).
    char gid[8];         // Group ID (octal).
    char size[12];       // File size in bytes (octal string).
    char mtime[12];      // Last modification time (octal string).
    char checksum[8];    // Header checksum (for integrity check).
    char type;           // File type (e.g., regular file '0', directory '5', symlink '2', etc.).
    char linkname[100];  // Target path if the file is a symlink.
    char magic[6];       // TAR format indicator ("ustar\0" for USTAR).
    char version[2];     // TAR version (typically "00").
    char uname[32];      // Owner user name.
    char gname[32];      // Owner group name.
    char devmajor[8];    // Major device number (for special files).
    char devminor[8];    // Minor device number (for special files).
    char prefix[155];    // Path prefix for long file names.
    char padding[12];    // Padding to align structure to 512 bytes.
    char data[];         // Array pointing to the data area following the header
                         // (flexible array member)
} __attribute__((packed));

/**
 * @struct file
 * @brief Represents a simple in-memory file structure.
 *
 * This structure is used to manage files stored in memory. It contains metadata
 * and a fixed-size buffer for file content.
 */
struct file {
    bool in_use;      ///< True if this file entry is currently allocated or active.
    char name[100];   ///< Null-terminated file name (up to 99 characters).
    char data[1024];  ///< File content stored in a fixed-size buffer.
    size_t size;      ///< Actual size of the file content (in bytes).
};

/**
 * @brief Look up a file by name in the in-memory file system.
 *
 * Iterates through the global `files` array to find a file whose name matches
 * the given filename. If a match is found, a pointer to the file structure is returned.
 * If no match is found, the function returns NULL.
 *
 * @param filename The name of the file to search for.
 * @return Pointer to the matching `struct file` if found, otherwise NULL.
 */
struct file *fs_lookup(const char *filename);

/**
 * @brief Initialize the in-memory file system by loading files from the virtual disk.
 *
 * This function reads the entire virtual disk into memory, then parses a TAR archive format
 * stored on the disk to populate the `files[]` array with file metadata and contents.
 *
 * Steps:
 * 1. Read each sector of the virtual disk into memory (`disk[]` array).
 * 2. Iterate over the TAR archive headers to extract file data.
 * 3. For each valid TAR header:
 *    - Validate using the "ustar" magic string.
 *    - Parse the file size (which is stored as an octal string).
 *    - Copy the file name and contents into a `struct file`.
 *    - Mark the file entry as in use and record its size.
 * 4. Advance the offset to the next file entry, aligned to 512-byte boundaries.
 * 5. Stop parsing if an invalid or empty TAR header is encountered.
 *
 * This initialization is typically performed once at boot time to load a simple
 * read-only file system into memory.
 */
void init_fs(void);

/**
 * @brief Flush the in-memory file system to the virtual disk in TAR format.
 *
 * This function serializes all in-use files from the `files[]` array into the `disk[]` buffer
 * using a simplified USTAR (Unix Standard TAR) format. After building the archive in memory,
 * it writes the entire `disk[]` buffer to the underlying virtual block device using
 * `read_write_disk()`.
 *
 * Steps:
 * 1. Clear the `disk[]` buffer to start with a clean slate.
 * 2. For each in-use file in `files[]`:
 *    - Construct a TAR header with metadata like file name, mode, size, and magic value.
 *    - Convert file size to an octal string.
 *    - Compute the checksum of the TAR header.
 *    - Copy the file content immediately after the header.
 *    - Advance the write offset to the next 512-byte aligned TAR block.
 * 3. Write the final `disk[]` buffer to disk sector-by-sector.
 *
 * @note The TAR headers and data are aligned to SECTOR_SIZE (typically 512 bytes),
 *       following the convention of block-based archive formats.
 */
void flush_fs(void);
