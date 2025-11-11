#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//https://stackoverflow.com/questions/4235519/counting-number-of-occurrences-of-a-char-in-a-string-in-c
int countChars(char *s, char c) {
    return *s == '\0' ? 0 : countChars(s + 1, c) + (*s == c);
}

int main(void) {
    while (1) {
        printf("slugterm>");

        const size_t buffer_size = 4096;
        char buffer[4096];
        char *input = fgets(buffer, buffer_size, stdin);
        if (input == NULL) {
            printf("\n");
            break;
        }
        input[strcspn(input, "\n")] = '\0';

        // Get number of commands
        const int commandCount = countChars(input, '|') + 1;

        char **commands = malloc(commandCount * sizeof(char *));
        const char *delim = "|";
        char *command = strtok(input, delim);

        int i = 0;
        while (command != NULL && i < commandCount) {
            commands[i] = command;
            i++;
            command = strtok(NULL, delim);
        }

        // testing the parsing
        // for (int j = 0; j < commandCount; j++) {
        //     printf("Command %d: '%s'\n", j + 1, commands[j]);
        // }

        int fd[2];
        pipe(fd);
        pid_t pid = fork();
        //child
        if (pid == 0) {
            if (commandCount == 1) {
                execv(commands[0], NULL);
            }

            exit(1);
        } else //parent
        {
            int status;
            waitpid(pid, &status, 0);
        }
        free(commands);
    }

    return 0;
}
