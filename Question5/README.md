# Question 5: Vi Recovery Mechanisms After a System Crash

## Overview
This evaluates vi's five recovery mechanisms (swap files, undo
history, registers, backup files, auto-recovery) for a scenario
where a system crashes before an edited file is saved, and
demonstrates the recommended recovery strategy live.

## Written Evaluation

### 1. Swap Files (.swp)
Vi continuously updates a hidden swap file as changes are made,
independent of whether the file has been saved. This is the primary
mechanism relevant to this scenario, since it captures unsaved
buffer content in real time.

### 2. Undo History
Only written to disk (persistent undo) when a save occurs. Since no
save happened before the crash, there is nothing to recover from
this mechanism.

### 3. Registers
Hold only explicitly yanked/deleted fragments, not the full buffer
state. Not a reliable recovery path for arbitrary unsaved edits.

### 4. Backup Files (~)
Only reflect the last *saved* state, which is identical to what's
already on disk since no save occurred after the crash. Offers no
new recovery information.

### 5. Auto-Recovery (vi -r)
The operational mechanism that reads the swap file and reconstructs
the buffer. This is how the swap file's contents actually get
restored.

## Recommended Strategy
The swap file, accessed via vi's auto-recovery prompt (or `vi -r`),
is the only mechanism that stores unsaved buffer content on disk
during an active session — making it the most reliable and, in this
scenario, the only genuinely useful recovery mechanism.

## Live Demonstration

### Command: vi testconfig.txt
**Explanation:** Opened a new file and typed two unsaved config
lines, deliberately not saving, to simulate the exact scenario
described in the question.

### Command: ps aux | grep vi
**Explanation:** Identified the running vi process by its PID (2281)
in a separate terminal, distinguishing it from the grep command
itself which also matched the search pattern.

### Command: kill -9 2281
**Explanation:** Force-killed the vi process to simulate an abrupt
system crash, since SIGKILL cannot be caught or handled gracefully,
closely mimicking a real crash.

### Command: ls -la
**Explanation:** Confirmed the hidden swap file `.testconfig.txt.swp`
was left behind after the crash, since it's only cleaned up on a
normal `:wq` exit, not an abnormal termination.

### Command: vi testconfig.txt (reopening)
**Explanation:** Vi detected the leftover swap file and displayed the
E325 recovery prompt, correctly identifying the crashed process ID
(2281, matching the one killed), timestamp, and filename.

### Recovery chosen: (R)ecover
**Explanation:** After choosing recover, vi displayed "Recovery
completed," and `cat testconfig.txt` confirmed both originally
unsaved lines (server_port and max_connections) were successfully
restored, directly validating the swap-file recovery mechanism.

## Key Linux/vi concepts demonstrated
- Swap file creation and persistence across an abnormal termination
- `ps aux | grep` for identifying a specific running process
- `kill -9` (SIGKILL) to simulate an unrecoverable crash
- Vi's automatic swap-file detection and E325 recovery prompt
- The distinction between recovery mechanisms that depend on a prior
  save (undo, backup) versus one that doesn't (swap file)
