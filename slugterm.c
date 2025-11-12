#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUFFER_SIZE 4096
#define MAX_ARGS    100

void parse_simple_cmd(char *cmd, char **argv) {
    int i = 0;
    char *token = strtok(cmd, " ");
    while (token != NULL && i < MAX_ARGS - 1) {
        argv[i++] = token;
        token = strtok(NULL, " ");
    }
    argv[i] = NULL; // argv must be NULL-terminated
}

int countChars(char *s, char c) {
    return *s == '\0' ? 0 : countChars(s + 1, c) + (*s == c);
}

int main(void) {

    while (1) {
        printf("slugterm>");
        fflush(stdout); // Make sure prompt is printed immediately

        char buffer[BUFFER_SIZE];
        char *input = fgets(buffer, BUFFER_SIZE, stdin);
        // leave if nothing
        if (input == NULL) {
            printf("\n");
            break;
        }

        buffer[strcspn(buffer, "\n")] = '\0'; // Remove trailing newline

        int commandCount = countChars(buffer, '|') + 1;
        char **commands = malloc(commandCount * sizeof(char *));

        int i = 0;

        // Create pipes
        int pipes[commandCount - 1][2];
        for (i = 0; i < commandCount - 1; i++) {
            pipe(pipes[i]);
        }
        pid_t pids[commandCount];
        for (i = 0; i < commandCount; i++) {
            pids[i] = fork();
            if (pids[i] == 0) {
                if (i >= commandCount - 1) {
                    dup2(pipes[i][1], STDOUT_FILENO);
                } else {
                    dup2(pipes[i - 1][0], STDOUT_FILENO);
                }

                // close non process pipes
                for (int j = 0; j < commandCount - 1; j++) {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }
                // parse and execute
                char *argv[MAX_ARGS];
                parse_simple_cmd(commands[i], argv);
                execv(argv[0], argv);
                perror("execv");
                _exit(1);
            }
        }
        //PARENT STARTS

        for (i = 0; i < commandCount - 1; i++) {
            close(pipes[i][0]);
            close(pipes[i][1]);
        }

        for (i = 0; i < commandCount; i++) {
            int status;
            waitpid(pids[i], &status, 0);
        }
        free(commands);
    }

    return 0;
}
