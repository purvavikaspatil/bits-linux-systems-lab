# Question 4: Real-Time Log Monitoring Tool

## Overview
This script uses tail -F, grep, and tee in a pipeline to monitor a
log file in real time, extract only ERROR lines, write them to a
persistent report file, and suppress unnecessary terminal output
using /dev/null.

## Command: ./log_monitor.sh &

**Explanation:** Started the monitor in the background so the
terminal remained free to simulate new log entries. The ampersand
runs it as a background job (job [1], PID 2158) instead of blocking
the shell.

## Command: echo ... >> application.log (multiple entries)

**Explanation:** Simulated 5 new log lines being appended to the log
file, 2 of which contained ERROR and 3 of which contained INFO. Since
tail -F was actively following the file, each new line was picked up
immediately without needing to restart the monitor.

## Command: kill %1

**Explanation:** Terminated the background monitoring job using its
job number, since tail -F runs indefinitely and needed to be manually
stopped for this test.

## Command: cat error_report.txt

**Explanation:** The report confirms only the 2 ERROR lines were
captured, while the 3 INFO lines were correctly filtered out by grep.
No output appeared on the live terminal during monitoring because the
script intentionally redirects tee's stdout copy to /dev/null,
satisfying the "suppress unnecessary output" requirement while still
preserving a full historical record in the report file.

## Key Linux concepts demonstrated
- `tail -F` to follow a log file in real time, resilient to log
  rotation
- Pipes (`|`) to stream data between tail, grep, and tee without
  intermediate files
- `grep --line-buffered` to avoid buffering delays in a piped
  real-time context
- `tee -a` to simultaneously write to a file and (optionally) stdout
- `/dev/null` to discard unwanted output at the kernel level with
  zero I/O cost
- Background jobs (`&`) and `kill %1` to manage a long-running
  process during testing
