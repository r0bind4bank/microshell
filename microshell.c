#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

void execute_cd(char* parameters) {
    chdir(parameters);
}

void execute_exit() {
    exit(0);
}

void execute_help() {
    printf("Author: Szymon Niewiadomski\n");
    printf("Commands: exit, cd, help\n");
}

void execute_program(char* program, char* parameters) {
    int pid = fork();
    if (pid == 0) {
        char* argv[] = { program, parameters, NULL };
        execvp(program, argv);
    } else {
        wait(NULL);
    }
}

int main() {
    char command[256];
    char function[256];
    char parameters[256];
    char cwd[256];
    char* path = getenv("PATH");
    execute_help();
    while (1) {
        // Get current working directory
        getcwd(cwd, sizeof(cwd));

        // Display prompt
        printf("[%s] $ ", cwd);

        // Get command from user
        fgets(command, sizeof(command), stdin);

        // Remove newline character
        command[strlen(command) - 1] = '\0';

        // Split command into function and parameters
        sscanf(command, "%s %[^\n]", function, parameters);

        // Compare function to known commands
        if (strcmp(function, "exit") == 0) {
            execute_exit();
        } else if (strcmp(function, "cd") == 0) {
            execute_cd(parameters);
        } else if (strcmp(function, "help") == 0) {
            execute_help();
        } else {
            char program[256];
            char* token = strtok(path, ":");
            int found = 0;
            while (token != NULL) {
                snprintf(program, sizeof(program), "%s/%s", token, function);
                if (access(program, F_OK) != -1) {
                    execute_program(program, parameters);
                    found = 1;
                    break;
                }
                token = strtok(NULL, ":");
            }
            if (!found) {
                printf("Error: Unable to interpret command: %s\n", function);
            }
        }
    }
    return 0;
}
