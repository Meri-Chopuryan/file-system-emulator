# File System Emulator (C)

## Overview
This project implements a simplified Unix-style file system emulator written in C.  
The program models directories and regular files using inode-based metadata structures and binary file storage.

The emulator loads an existing inode list from disk, allows interactive navigation and modification of the file system, and persists changes on exit.

---

## Features

- Supports up to 1024 inodes using `uint32_t` indexing
- Differentiates directories and regular files (`d` / `f`)
- Binary metadata storage and parsing
- Interactive command interpreter supporting:
  - `ls`
  - `cd <name>`
  - `cd ..` (parent directory traversal)
  - `mkdir <name>`
  - `touch <name>`
  - `exit`
- State management (updates inode list on exit)
- Input validation and error handling for invalid commands
- Handles root-directory edge cases safely
- Extended the emulator to support parent directory traversal using `cd ..`, enabling navigation to a directory’s parent inode.

---

## Technical Concepts

- Binary file I/O (`read`, `fread`, `write`)
- Inode-based metadata modeling
- Directory entry structures
- Command-line parsing and input validation
- State management and directory traversal logic
- Resource management and defensive programming

---

## Build & Run

```bash
make
./fs_emulator <file_system_directory>
