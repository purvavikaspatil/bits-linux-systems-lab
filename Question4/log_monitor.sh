#!/bin/bash
LOG_FILE="./application.log"
ERROR_REPORT="./error_report.txt"
TIMESTAMP=$(date +"%Y-%m-%d %H:%M:%S")

touch "$LOG_FILE" 2>/dev/null

echo "===== Monitoring started: $TIMESTAMP =====" >> "$ERROR_REPORT"

tail -F "$LOG_FILE" 2>>/dev/null | \
    grep --line-buffered "ERROR" | \
    tee -a "$ERROR_REPORT" > /dev/null
