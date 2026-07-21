#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#define NUM_CHILDREN     5
#define TIMEOUT_SECONDS  5

typedef struct {
    pid_t pid;
    time_t start_time;
    int active;
} ChildInfo;

ChildInfo children[NUM_CHILDREN];

void sigchld_handler(int sig) {
    (void)sig;
    int saved_errno = errno;
    pid_t pid;
    int status;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        for (int i = 0; i < NUM_CHILDREN; i++) {
            if (children[i].pid == pid) {
                children[i].active = 0;
                if (WIFEXITED(status)) {
                    printf("[Parent] Child %d (PID %d) exited normally, code %d\n",
                           i, pid, WEXITSTATUS(status));
                } else if (WIFSIGNALED(status)) {
                    printf("[Parent] Child %d (PID %d) killed by signal %d\n",
                           i, pid, WTERMSIG(status));
                }
                break;
            }
        }
    }
    errno = saved_errno;
}

void child_task(int id) {
    srand(getpid());
    int work_time = (id % 2 == 0) ? 2 : 20;
    printf("[Child %d] PID %d starting, expected work time %ds\n",
           id, getpid(), work_time);

    for (int i = 0; i < work_time; i++) {
        sleep(1);
    }

    printf("[Child %d] PID %d finished work\n", id, getpid());
    exit(EXIT_SUCCESS);
}

int main(void) {
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    printf("[Parent] PID %d starting %d child processes\n",
           getpid(), NUM_CHILDREN);

    for (int i = 0; i < NUM_CHILDREN; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork failed");
            continue;
        } else if (pid == 0) {
            child_task(i);
        } else {
            children[i].pid = pid;
            children[i].start_time = time(NULL);
            children[i].active = 1;
        }
    }

    int remaining_active;
    do {
        remaining_active = 0;
        time_t now = time(NULL);

        for (int i = 0; i < NUM_CHILDREN; i++) {
            if (!children[i].active) continue;

            if (kill(children[i].pid, 0) == -1 && errno == ESRCH) {
                continue;
            }

            remaining_active++;

            double elapsed = difftime(now, children[i].start_time);
            if (elapsed >= TIMEOUT_SECONDS) {
                printf("[Parent] Child %d (PID %d) unresponsive after %.0fs "
                       "- sending SIGTERM\n", i, children[i].pid, elapsed);
                kill(children[i].pid, SIGTERM);

                sleep(1);

                if (kill(children[i].pid, 0) == 0) {
                    printf("[Parent] Child %d (PID %d) ignored SIGTERM "
                           "- sending SIGKILL\n", i, children[i].pid);
                    kill(children[i].pid, SIGKILL);
                }
            }
        }

        sleep(1);
    } while (remaining_active > 0);

    printf("[Parent] All children accounted for. Exiting.\n");
    return 0;
}
