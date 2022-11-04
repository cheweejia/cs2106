/*
 * CS2106 AY22/23 Semester 1 - Lab 2
 *
 * This file contains function definitions. Your implementation should go in
 * this file.
 */


#include "myshell.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

struct PCBTable processes[50];
int num_of_processes;

void extract_arguments(char **array, char ***arguments);
int has_ampersand(char **tokens);
void execute_info(char **tokens);
void execute_wait(char **tokens);
void execute_terminate(char **tokens);
void print_process_details(void);
void update_child_status(pid_t child_process, int status, int exit_code);
void print_num_of_processes(int status);
void handle_exited_processes(void);
char*** parse_command(size_t num_tokens, char **tokens);
char** parse_ridirection(char** command);
void handle_stdout(char *file);
void handle_stdin(char *file);
void handle_stderr(char *file);

void my_init(void) {
    num_of_processes = 0;
    signal(SIGCHLD, handle_exited_processes);    
}

void my_process_command(size_t num_tokens, char **tokens) {
    int index = 0;
    char ***commands = parse_command(num_tokens, tokens);
    while (commands[index] != NULL) {
        char **command = commands[index];
        char *program = command[0];

        if (strcmp(program, "info") == 0) {
            execute_info(command);
        } else if (strcmp(program, "wait") == 0) {
            execute_wait(command);
        } else if (strcmp(program, "terminate") == 0) {
            execute_terminate(command);
        } else if (access(program, R_OK) == 0 && access(program, X_OK) == 0) {
            // Executes program and parent waits for child
            pid_t child = fork();
            // Add process to PCBTable
            processes[num_of_processes].pid = child;
            processes[num_of_processes].status = 2;
            processes[num_of_processes].exitCode = -1;
            num_of_processes++;

            // Child process
            if (child == 0) {
                // Handle redirection before execution
                char **files = parse_ridirection(command);
                if (files[0] != NULL) {
                    handle_stdin(files[0]);
                }
                if (files[1] != NULL) {
                    handle_stdout(files[1]);
                }
                if (files[2] != NULL) {
                    handle_stderr(files[2]);
                }

                char **arguments = NULL;
                extract_arguments(command, &arguments);
                // First argument should point to filename associated to file being executed by convention.
                execv(program, arguments);
            } else {
                // Parent process
                // Checks for &. If & doesnt exists, parent must wait for child. Else, parent will proceed without waiting.
                int no_waiting = has_ampersand(command);
                if (no_waiting) {
                    printf("Child [%d] in background\n", child);
                } else {
                    int status;
                    waitpid(child, &status, 0);
                    int code = WEXITSTATUS(status);
                    update_child_status(child, 1, code);
                }
            }
        } else {
            fprintf(stderr, "%s not found\n", program);
        }
        index++;
    }
}

void my_quit(void) {

    // Clean up function, called after "quit" is entered as a user command
    for (int i = 0; i < num_of_processes; i++) {
        if (processes[i].status == 2) {
            printf("Killing [%d]\n", processes[i].pid);
            kill(processes[i].pid, SIGTERM);
        }
    }

    printf("\nGoodbye\n");
}

// Extract arguments from token
void extract_arguments(char **array, char ***arguments) {
    char **ret = calloc(1, sizeof(char *));
    int i = 0;
    while (array[i] != (char *) NULL && strcmp(array[i], (char *) "&") != 0 && strcmp(array[i], (char *) "<") != 0
        && strcmp(array[i], (char *) ">") != 0 && strcmp(array[i], (char *) "2>") != 0) {
        ret[i] = array[i];
        i++;
        ret = realloc(ret, (i + 1) * sizeof(char *));
    }
    ret[i] = NULL;
    *arguments = ret;
}

// Checks if the command terminates with &
int has_ampersand(char **tokens) {
    int index = 0;
    while (tokens[index] != (char *) NULL) {
        if (strcmp(tokens[index], (char *) "&") == 0) {
            return 1;
        }
        index++;
    }
    return 0;
}

// Executes info command
void execute_info(char **tokens) {
    char **argument = NULL;
    extract_arguments(tokens, &argument);
    if (argument[1] == NULL || (strcmp(argument[1], "0") != 0 && strcmp(argument[1], "1") != 0 && strcmp(argument[1], "2") != 0 && strcmp(argument[1], "3") != 0)) {
        fprintf(stderr, "Wrong command\n");
        return;
    }
    
    int option = atoi(argument[1]);
    switch (option) {
        case 0:
            print_process_details();
            break;
        case 1:
            print_num_of_processes(1);
            break;
        case 2:
            print_num_of_processes(2);
            break;
        case 3:
            print_num_of_processes(3);
            break;
        default:
            fprintf(stderr, "Wrong command\n");
    }
}

// Executes wait command
void execute_wait(char **tokens) {
    char **argument = NULL;
    extract_arguments(tokens, &argument);
    int pid = atoi(argument[1]);
    int status;
    if (waitpid(pid, &status, 0) > 0) {
        int code = WEXITSTATUS(status);
        update_child_status(pid, 1, code);
    }
}

// Executes terminate command
void execute_terminate(char **tokens) {
    char **argument = NULL;
    extract_arguments(tokens, &argument);
    int pid = atoi(argument[1]);
    for (int i = 0; i < num_of_processes; i++) {
        if (processes[i].status == 2) {
            int exit_code = kill(pid, SIGTERM);
            update_child_status(pid, 3, exit_code);
            break;
        }
    }
}

// Executes "info 0"
void print_process_details(void) {
    for (int i = 0; i < num_of_processes; i++) {
        pid_t process_number = processes[i].pid;
        int status = processes[i].status;
        int exit_code = processes[i].exitCode;
        printf("[%d] ", process_number);
        // Print exit code if process has exited
        switch (status) {
            case 1:
                printf("Exited %d\n", exit_code);
                break;
            case 2:
                printf("Running\n");
                break;
            case 3:
                printf("Terminating\n");
                break;
        }
    }
}

// Update status of PID in the PCBTable
void update_child_status(pid_t child_process, int status, int exit_code) {
    int i = 0;
    while (i < num_of_processes) {
        if (processes[i].pid == child_process) {
            processes[i].status = status;
            processes[i].exitCode = exit_code;
            break;
        }
        i++;
    }
}

// info 1/2/3
void print_num_of_processes(int status) {
    int result = 0;
    for (int i = 0; i < num_of_processes; i++) {
        if (processes[i].status == status) {
            result++;
        }
    }

    switch (status) {
        case 1:
            printf("Total exited process: %d\n", result);
            break;
        case 2:
            printf("Total running process: %d\n", result);
            break;
        case 3:
            printf("Total terminating process: %d\n", result);
            break;
    }
}

// Handler for SIGCHLD
void handle_exited_processes(void) {
    pid_t child;
    int code;
    child = wait(&code);
    if (child > 0) {
        update_child_status(child, 1, code);
    }
}

// Parse chained commands
char*** parse_command(size_t num_tokens, char **tokens) {
    char ***ret;
    char **temp = tokens;
    char *delimiter = ";";
    
    int num_of_cmds = 1;
    for (size_t i = 0; i < num_tokens - 1; i++) {
        if (strcmp(temp[i], delimiter) == 0) {
            num_of_cmds++;
        }
    }

    ret = malloc(sizeof(char) * (num_of_cmds + 1));
    int cmds_index = 0;
    int args_index = 0;
    
    for (int j = 0; j < num_of_cmds; j++) {
        char **command = malloc(sizeof(char *) * num_tokens);
        int num_of_args = 0;
        while (temp[args_index] != NULL && strcmp(temp[args_index], delimiter) != 0)  {
            command[num_of_args] = temp[args_index];
            num_of_args++;
            args_index++;
        }   
        command[num_of_args] = NULL;
        args_index++;
        ret[cmds_index] = command;
        cmds_index++;
    }
    ret[num_of_cmds] = NULL;
    return ret;
}

// Parse for redirection options and return the files in an array as such [input, output, stderr]
char** parse_ridirection(char** command) {
    char **result = malloc(sizeof(char *) * 3);
    int i = 0;
    while (command[i] != NULL) {
        if (strcmp(command[i], "<") == 0) {
            result[0] = command[i + 1];
        } else if (strcmp(command[i], ">") == 0) {
            result[1] = command[i + 1];
        } else if (strcmp(command[i], "2>") == 0) {
            result[2] = command[i + 1];
        }
        i++;
    }
    return result;
}

// Redirection for stdin
void handle_stdin(char *file) {
    int fd = open(file, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "%s does not exist\n", file);
        exit(1);
    }

    // Close the STDIN file descriptor, change the STDIN fd to the file descriptor of file
    dup2(fd, STDIN_FILENO);  
    close(fd);
}

// Redirection for stdout
void handle_stdout(char *file) {
    int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0777);
    // Close the STDOUT file descriptor, change the STDOUT fd to the file descriptor of file
    dup2(fd, STDOUT_FILENO);  
    close(fd);
}

// Redirection for stderr
void handle_stderr(char *file) {
    int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0777);
    // Close the STDOUT file descriptor, change the STDOUT fd to the file descriptor of file
    dup2(fd, STDERR_FILENO);  
    close(fd);
}