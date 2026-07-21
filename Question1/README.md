# Question 1: Duplicate Submission Detector & Backup

## Overview
This script scans a directory of student assignment submissions,
detects duplicate files based on content (not filename) using MD5
hashing, backs up only the unique files into a timestamped backup
folder, and generates a summary report. All errors are logged
separately.

## Setup
Created a `submissions/` folder with 5 test files, where two pairs
had identical content but different filenames, to properly test the
duplicate-detection logic.

**Explanation:** I set this up so that filename-based duplicate
detection would fail but content-based (hash) detection would succeed,
directly testing the core requirement of the assignment.

## Command: ./process_submissions.sh

**Explanation:** This ran the full script against the submissions
folder. It computed an MD5 hash for each file, compared it against
previously seen hashes, copied unique files into a new backup
directory, and logged duplicates and errors separately.

## Command: cat report.txt

**Explanation:** The report confirmed 5 total files were processed,
2 were identified as duplicates (student2 matched student1's content,
student5 matched student4's content), and 3 unique files were backed
up. This confirms the hash-based comparison worked correctly even
though the duplicate files had different names.

## Command: cat errors.log

**Explanation:** The error log was empty, confirming the script ran
without any failures in directory creation, hashing, or file copying
during this test run.

## Command: ls backup_20260721_105226/

**Explanation:** This confirmed the backup directory contained
exactly the 3 unique files (student1, student3, student4), and
excluded the 2 duplicate files, matching the report's numbers exactly.

## Key Linux concepts demonstrated
- `md5sum` for content-based file fingerprinting
- `grep -q` for silent existence checking against a hash log
- `>>` append redirection to build up report/error logs incrementally
- `2>>` to redirect stderr separately from normal output
- `$(( ))` for shell arithmetic counters
- `{ } >> file` grouped redirection for writing a multi-line report block
