#!/bin/bash
# =========================================================
# Script: process_submissions.sh
# Purpose: Detect duplicate assignment submissions, backup
#          unique ones, and generate a summary report.
# =========================================================
# ---------- Configuration ----------
SUBMIT_DIR="./submissions"        # where student files live
BACKUP_DIR="./backup_$(date +%Y%m%d_%H%M%S)"
REPORT_FILE="./report.txt"
ERROR_LOG="./errors.log"
HASH_FILE="./hashes.tmp"
# ---------- Counters ----------
total_files=0
duplicate_files=0
backed_up_files=0
# ---------- Setup ----------
mkdir -p "$BACKUP_DIR" 2>>"$ERROR_LOG"
> "$HASH_FILE"
> "$REPORT_FILE"
> "$ERROR_LOG"
if [ ! -d "$SUBMIT_DIR" ]; then
    echo "ERROR: Submission directory '$SUBMIT_DIR' not found." >>"$ERROR_LOG"
    exit 1
fi
# ---------- Main Processing ----------
for file in "$SUBMIT_DIR"/*; do
    if [ ! -f "$file" ]; then
        echo "WARNING: '$file' is not a regular file, skipped." >>"$ERROR_LOG"
        continue
    fi
    total_files=$((total_files + 1))
    file_hash=$(md5sum "$file" 2>>"$ERROR_LOG" | awk '{print $1}')
    if [ -z "$file_hash" ]; then
        echo "ERROR: Could not compute hash for '$file'." >>"$ERROR_LOG"
        continue
    fi
    if grep -q "^$file_hash$" "$HASH_FILE" 2>>"$ERROR_LOG"; then
        duplicate_files=$((duplicate_files + 1))
        echo "Duplicate found: $file" >>"$REPORT_FILE"
    else
        echo "$file_hash" >> "$HASH_FILE"
        cp "$file" "$BACKUP_DIR"/ 2>>"$ERROR_LOG"
        if [ $? -eq 0 ]; then
            backed_up_files=$((backed_up_files + 1))
        else
            echo "ERROR: Failed to back up '$file'." >>"$ERROR_LOG"
        fi
    fi
done
# ---------- Generate Final Report ----------
{
    echo "==================================================="
    echo "        Assignment Submission Processing Report"
    echo "==================================================="
    echo "Date            : $(date)"
    echo "Source Directory: $SUBMIT_DIR"
    echo "Backup Directory: $BACKUP_DIR"
    echo "---------------------------------------------------"
    echo "Total files processed : $total_files"
    echo "Duplicate files found  : $duplicate_files"
    echo "Files backed up        : $backed_up_files"
    echo "---------------------------------------------------"
    echo "Details of duplicates recorded above (if any)."
    echo "Error details (if any) recorded in: $ERROR_LOG"
} >> "$REPORT_FILE"
rm -f "$HASH_FILE" 2>>"$ERROR_LOG"
echo "Processing complete. See $REPORT_FILE for summary."
exit 0
