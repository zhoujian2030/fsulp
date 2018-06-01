#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

// ---------------------------------
int main(int argc, char* argv[]) {
    char* child_argv[32] = {0};

    if (argc < 2) {
        printf("Usage: %s <exe_path> [args...]\n", argv[0]);
        return -1;
    }
    
    for (int i=1; i<argc; i++) {
        child_argv[i-1] = (char*)malloc(strlen(argv[i]) + 1);
        strncpy(child_argv[i-1], argv[i], strlen(argv[i]));
        child_argv[i-1][strlen(argv[i])] = '\0';
    }

    pid_t pid;
    int status;

    while(1) {
        pid = fork();
        if (pid == -1) {
            printf("fail to fork\n");
            sleep(1);
        } else if (pid == 0) {
            printf("start child process\n");
            if (execv(child_argv[0], (char**)child_argv) < 0) {
                printf("fail to execv\n");
                sleep(1);
                continue;
            }
            exit(0);
        } else {
            pid = wait(&status);
            printf("status = %d, try to restart\n", status);
        }
    }

    return 0;
}
