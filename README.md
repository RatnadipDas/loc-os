# 🐚 LoC OS: RISC-V Kernel Project loc-os
This project is an attempt to build an OS kernel in C, based on the tutorial available at:

---

https://operating-system-in-1000-lines.vercel.app/en/

---

This project uses GNU Make to build and run a RISC-V kernel LoC OS with user-mode programs and simulate it using QEMU.

# 🛠️ Below are the key Makefile commands:

## 📦 `make all`

**Build and Run Everything**

This is the default target. It compiles all necessary components (kernel, user binaries, common code), generates assembly and symbol dumps, and runs the kernel on QEMU.

```bash
make all
```

---

## 🔨 `make build`

**Just Build (Don’t Run)**

Builds the kernel and user binaries, dumps assembly and symbol tables, but doesn't run QEMU. Use this when you're developing and want to inspect the build artifacts.

```bash
make build
```

---

## 🚀 `make run`

**Run the Kernel on QEMU**

Runs the kernel ELF file using QEMU, with the configured disk and peripherals. Use this after building to test your kernel.

```bash
make run
```

---

## 🧹 `make clean`

**Clean Up Build Files**

Deletes the entire `build/` directory and all generated files. Great for resetting the environment before a fresh build.

```bash
make clean
```

---

## 🧭 `make kernel-addr2line ADDRESS=xxxxxxxx`

**Get Kernel Source Line from Address**

Maps a kernel address (e.g., from a crash dump or log) back to the corresponding source line in the kernel ELF file using `llvm-addr2line`.

```bash
make kernel-addr2line ADDRESS=80001234
```

---

## 🧭 `make user-addr2line ADDRESS=xxxxxxxx`

**Get User Source Line from Address**

Just like `kernel-addr2line`, but for the user program ELF. Handy for debugging user-space crashes or bugs.

```bash
make user-addr2line ADDRESS=00001234
```

---

## ✅ Requirements

Ensure you have the following installed:

- LLVM toolchain (Clang, llvm-objcopy, llvm-addr2line, etc.)
- QEMU with RISC-V support (`qemu-system-riscv32`)

---

## 📁 Directory Structure Summary

- `build/`: Compiled files and artifacts
- `kernel/`: Kernel source and linker script
- `user/`: User-mode program source and linker script
- `common/`: Shared code between kernel and user programs
- `disk/`: Disk content to be bundled and loaded

---