#include "fs.h"

#include "arg.h"
#include "lib.h"
#include "str.h"
#include "types.h"
#include "utils.h"
#include "virtio_disk.h"

// Global array of in-memory file structures.
// Used to simulate a simple filesystem with a limited number of files.
struct file files[FILES_MAX];

// Simulated disk storage buffer.
// Sized to hold all file structures, aligned to the sector size.
// This represents a basic disk abstraction used for read/write operations.
uint8_t disk[DISK_MAX_SIZE];

struct file *fs_lookup(const char *filename) {
    for (size_t i = 0; i < FILES_MAX; i++) {
        struct file *file = &files[i];
        if (!strcmp(file->name, filename))
            return file;
    }

    return NULL;
}

void init_fs(void) {
    INFO("Initializing file system...");

    // Step 1: Read disk sectors into memory buffer
    for (unsigned sector = 0; sector < sizeof(disk) / SECTOR_SIZE; sector++)
        read_write_disk(&disk[sector * SECTOR_SIZE], sector, false);

    // Step 2: Start parsing TAR archive format
    unsigned off = 0;
    for (size_t i = 0; i < FILES_MAX; i++) {
        struct tar_header *header = (struct tar_header *)&disk[off];

        // Empty TAR entry indicates end of archive
        if (header->name[0] == '\0')
            break;

        // Validate TAR format using magic field
        if (strcmp(header->magic, "ustar") != 0)
            break;

        // Step 3: Extract and convert the file size from octal string
        char hex_appended_header[33] = "0o";  // Prepend to interpret as octal
                                              // strlen(header->size) == 30, so hex_appended_header has to be of size 33 (extra '\0')
        strcat(hex_appended_header, header->size);
        int filesz = atoi(hex_appended_header);

        // Step 4: Load file data into in-memory structure
        struct file *file = &files[i];
        file->in_use = true;
        strcpy(file->name, header->name);
        memcpy(file->data, header->data, filesz);
        file->size = filesz;
        INFO("file: %s, size=%d", file->name, file->size);

        // Step 5: Move to the next TAR header, aligned to 512 bytes

        off += align_up(sizeof(struct tar_header) + filesz, SECTOR_SIZE);  // Step 5: Move to the next TAR header, aligned to 512 bytes
                                                                           // skip tar header + data size aligned with SECTOR_SIZE
    }

    OK("Initialized file system.");
}

void flush_fs(void) {
    // Step 1: Clear the disk buffer
    memset(disk, 0, sizeof(disk));
    unsigned off = 0;

    // Step 2: Serialize each active file into the TAR archive format
    for (size_t file_i = 0; file_i < FILES_MAX; file_i++) {
        struct file *file = &files[file_i];
        if (!file->in_use)
            continue;

        struct tar_header *header = (struct tar_header *)&disk[off];
        memset(header, 0, sizeof(*header));
        strcpy(header->name, file->name);
        strcpy(header->mode, "000644");  // Default permission: user rw, group r, world r: -rw-r--r-
        strcpy(header->magic, "ustar");  // USTAR format signature
        strcpy(header->version, "00");   // Format version
        header->type = '0';              // Type flag: regular file

        // Convert file size to an octal string (stored right-aligned in a 12-byte field)
        int32_t filesz = file->size;
        for (int32_t i = sizeof(header->size); i > 0; i--) {
            header->size[i - 1] = (filesz % 8) + '0';
            filesz /= 8;
        }

        // Calculate the TAR checksum:
        // First, treat checksum field as filled with spaces.
        // The checksum field in a TAR header is 8 bytes long,
        // but only the first 6 bytes store the actual octal representation of the checksum.
        // The last two bytes are reserved for:
        // A null terminator ('\0')
        // A space (' ')
        // Eg.: "123456\0 "
        int32_t checksum = ' ' * sizeof(header->checksum);
        for (unsigned i = 0; i < sizeof(struct tar_header); i++)
            checksum += (unsigned char)disk[off + i];

        // Store checksum as octal string in header->checksum[0..5]
        for (int32_t i = 5; i >= 0; i--) {
            header->checksum[i] = (checksum % 8) + '0';
            checksum /= 8;
        }
        // Ensure the last two bytes are properly set
        header->checksum[6] = '\0';
        header->checksum[7] = ' ';

        // Copy file content into the disk buffer right after the header
        memcpy(header->data, file->data, file->size);

        // Move offset forward, aligned to 512-byte TAR block boundary
        off += align_up(sizeof(struct tar_header) + file->size, SECTOR_SIZE);
    }

    // Step 3: Write the updated disk buffer to the virtual block device
    for (unsigned sector = 0; sector < sizeof(disk) / SECTOR_SIZE; sector++)
        read_write_disk(&disk[sector * SECTOR_SIZE], sector, true);

    INFO("Wrote %d bytes to disk.", sizeof(disk));
}
