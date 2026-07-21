# Question 2: Zombie Process Prevention & Unresponsive Child Termination

## Overview
This program forks 5 child processes simulating web server request
handlers. Even-numbered children finish quickly (2s); odd-numbered
children simulate hangs (20s). A SIGCHLD handler reaps children
immediately on exit to prevent zombies, while a monitoring loop
detects and kills children that exceed a 5-second timeout using
SIGTERM (escalating to SIGKILL if needed).

## Command: gcc web_server_monitor.c -o web_server_monitor

**Explanation:** Compiled the program with no errors, confirming the
sigaction, fork, waitpid, and kill syscalls were used correctly.

## Command: ./web_server_monitor

**Explanation:** Running the program showed children 0, 2, and 4
(2-second work time) finished normally and were reaped immediately by
the SIGCHLD handler, printing their exit codes as soon as they
terminated. Children 1 and 3 (20-second work time) exceeded the
5-second timeout, and the parent correctly detected this and sent
SIGTERM (signal 15) to terminate them, confirming both zombie
prevention and unresponsive-process termination worked as designed.

## Key Linux concepts demonstrated
- `fork()` to create child processes
- `sigaction()` with SA_RESTART to register a robust SIGCHLD handler
- `waitpid(-1, &status, WNOHANG)` in a loop to reap all terminated
  children without blocking, avoiding zombie processes
- `kill(pid, 0)` to check if a process is still alive without
  actually signaling it
- `kill(pid, SIGTERM)` for graceful termination, with `SIGKILL` as a
  fallback for unresponsive processes
- `WIFEXITED`/`WEXITSTATUS`/`WIFSIGNALED`/`WTERMSIG` macros to
  interpret child exit status correctly
