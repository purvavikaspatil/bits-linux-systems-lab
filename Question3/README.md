# Question 3: Secure File-Processing Utility Using Raw Syscalls

## Overview
This program manages employee records using low-level Linux syscalls
(open, read, write, lseek, close) instead of stdio functions. Records
are fixed-size structs, which allows direct offset calculation for
O(1) random access, in-place updates, and efficient retrieval without
scanning the whole file.

## Command: gcc employee_file_manager.c -o employee_file_manager

**Explanation:** Compiled with no errors, confirming correct usage of
open(), read(), write(), lseek(), and close() syscalls along with
proper fixed-size struct definitions.

## Command: ./employee_file_manager

**Explanation:** The program created employees.dat, wrote 3 employee
records by appending via lseek(fd, 0, SEEK_END), then printed all
records by sequentially reading from the start. It then updated
record index 1 (Rohan Verma's salary from 48000 to 53000) using
lseek() to jump directly to that record's byte offset and overwrite
only those bytes — records 0 and 2 remained completely unchanged,
confirming the update did not rewrite the entire file. Finally, it
directly fetched record index 2 (Meera Iyer) using a single lseek +
read, without reading through records 0 and 1 first, demonstrating
efficient random access.

## Key Linux concepts demonstrated
- `open()` with O_RDWR | O_CREAT and explicit permission bits
  (S_IRUSR | S_IWUSR) for secure file creation
- Fixed-size record structs enabling direct offset calculation
  (index * RECORD_SIZE)
- `lseek(fd, offset, SEEK_SET)` for O(1) random access to any record
- `lseek(fd, 0, SEEK_END)` for safe appending regardless of current
  file position
- `write()` for both appending new records and overwriting existing
  ones in place
- `read()` for both sequential scanning and direct single-record
  retrieval
- `close()` to release the file descriptor safely
