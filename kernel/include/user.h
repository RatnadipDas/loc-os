#pragma once

// TODO: Edit the comment
// The base virtual address of an application image. This needs to match the
// starting address defined in `user.ld`.
#define USER_BASE 0x1000000

// TODO: Add comment later
#define SSTATUS_SPIE (1 << 5)

// TODO: add comment later
void init_user(void);