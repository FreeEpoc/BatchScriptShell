#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>
#include <ctype.h>
#include <strings.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define MAX_CMD_LENGTH 1024
#define MAX_ARGS 64

// Global variable to track if we're in batch mode
int batch_mode = 0;
int last_exit_code = 0;

// Function declarations
void print_prompt();
char* read_command();
char** parse_command(char* cmd, int* arg_count);
int execute_command(char** args, int arg_count);
void free_args(char** args);
int handle_redirection(char** args, int arg_count);
int is_batch_file(const char* filename);
int execute_batch_file(const char* filename);
char* expand_path(char* path);
char* convert_win_to_unix_path(const char* win_path);
int starts_with(const char* str, const char* prefix);
void print_help();

int main(int argc, char* argv[]) {
    if (argc > 1) {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            printf("Batch Script Shell (BSS) - A shell for executing batch scripts\n");
            printf("Usage: %s [batch_file.bat]\n", argv[0]);
            printf("       %s -h | --help (show this help)\n", argv[0]);
            return 0;
        } else {
            // Execute batch file
            int result = execute_batch_file(argv[1]);
            // Reset batch mode after executing batch file from command line
            batch_mode = 0;
            return result;
        }
    }

    // Interactive mode
    char* cmd;
    char** args;
    int arg_count;
    int status = 1;

    printf("Batch Script Shell (BSS) - Type 'help' for available commands\n");

    do {
        print_prompt();
        cmd = read_command();

        if (cmd == NULL) {
            // EOF or error reading command
            break;
        }

        if (strlen(cmd) == 0) {
            // Empty command, continue to next prompt
            free(cmd);
            continue;
        }

        args = parse_command(cmd, &arg_count);

        if (args == NULL) {
            free(cmd);
            continue;
        }

        status = execute_command(args, arg_count);

        free_args(args);
        free(cmd);
    } while (status);

    return 0;
}

void print_prompt() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        // Convert to Windows-style path for display
        char win_path[PATH_MAX];
        char* ptr = cwd;
        int i = 0;
        
        // If path starts with '/', replace with 'C:\'
        if (*ptr == '/') {
            strcpy(win_path, "C:\\");
            i = 3;
            ptr++;
        }
        
        while (*ptr != '\0' && i < PATH_MAX-1) {
            if (*ptr == '/') {
                win_path[i] = '\\';
            } else {
                win_path[i] = *ptr;
            }
            i++;
            ptr++;
        }
        win_path[i] = '\0';
        
        printf("%s> ", win_path);
    } else {
        printf("C:\\> ");
    }
    fflush(stdout);
}

char* read_command() {
    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    read = getline(&line, &len, stdin);

    if (read == -1) {
        if (feof(stdin)) {
            free(line);
            return NULL;
        } else {
            perror("getline");
            exit(EXIT_FAILURE);
        }
    }

    // Remove trailing newline
    if (line[read - 1] == '\n') {
        line[read - 1] = '\0';
    }

    return line;
}

char** parse_command(char* cmd, int* arg_count) {
    char** args = malloc(MAX_ARGS * sizeof(char*));
    if (args == NULL) {
        perror("malloc");
        return NULL;
    }

    char* token;
    int count = 0;

    // Handle @echo off and similar commands at start
    if (cmd[0] == '@') {
        memmove(cmd, cmd + 1, strlen(cmd)); // Remove the '@'
    }

    token = strtok(cmd, " \t\r\n");
    while (token != NULL && count < MAX_ARGS - 1) {
        args[count] = strdup(token);
        if (args[count] == NULL) {
            perror("strdup");
            for (int i = 0; i < count; i++) {
                free(args[i]);
            }
            free(args);
            return NULL;
        }
        count++;
        token = strtok(NULL, " \t\r\n");
    }

    args[count] = NULL;
    *arg_count = count;

    return args;
}

int execute_command(char** args, int arg_count) {
    last_exit_code = 0;

    if (args[0] == NULL) {
        return 1; // No command entered, continue shell
    }

    // Check for redirections first, before handling built-in commands
    // This ensures commands with redirections like 'echo Hello > file.txt' work properly
    if (handle_redirection(args, arg_count) == 0) {
        // Redirection was handled successfully
        return 1;
    }

    // Built-in commands
    if (strcmp(args[0], "echo") == 0) {
        if (arg_count == 1) {
            printf("\n");
        } else if (arg_count == 2) {
            if (strcmp(args[1], "on") == 0) {
                printf("ECHO is on.\n");
            } else if (strcmp(args[1], "off") == 0) {
                printf("ECHO is off.\n");
            } else {
                printf("%s\n", args[1]);
            }
        } else {
            for (int i = 1; i < arg_count; i++) {
                printf("%s ", args[i]);
            }
            printf("\n");
        }
        return 1;
    } else if (strcmp(args[0], "set") == 0) {
        // Handle SET command to display or set environment variables
        if (arg_count == 1) {
            // Display all environment variables
            extern char **environ;
            for (char **env = environ; *env != 0; env++) {
                printf("%s\n", *env);
            }
        } else if (arg_count >= 2) {
            // Simple variable setting (not persistent in this implementation)
            char* var = args[1];
            char* equals = strchr(var, '=');
            if (equals != NULL) {
                *equals = '\0';
                setenv(var, equals + 1, 1);
            } else {
                // Display specific variable
                char* value = getenv(var);
                if (value != NULL) {
                    printf("%s=%s\n", var, value);
                } else {
                    printf("%s\n", var);
                }
            }
        }
        return 1;
    } else if (strcmp(args[0], "cd") == 0 || strcmp(args[0], "chdir") == 0) {
        if (arg_count == 1) {
            // No arguments - just print current directory
            char cwd[PATH_MAX];
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                printf("%s\n", cwd);
            }
        } else if (arg_count == 2) {
            char* path = convert_win_to_unix_path(args[1]);
            if (chdir(path) != 0) {
                printf("cd: %s: %s\n", path, strerror(errno));
                last_exit_code = 1;
            }
            free(path);
        } else {
            printf("cd: Too many arguments\n");
            last_exit_code = 1;
        }
        return 1;
    } else if (strcmp(args[0], "pwd") == 0) {
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        } else {
            perror("getcwd() error");
            last_exit_code = 1;
            return 1;
        }
        return 1;
    } else if (strcmp(args[0], "cls") == 0 || strcmp(args[0], "clear") == 0) {
        system("clear");
        return 1;
    } else if (strcmp(args[0], "pause") == 0) {
        if (batch_mode) {
            // In batch mode, don't actually pause, just continue
            return 1;
        } else {
            // In interactive mode, behave normally
            printf("Press any key to continue . . . ");
            getchar();
            return 1;
        }
    } else if (strcmp(args[0], "help") == 0) {
        print_help();
        return 1;
    } else if (strcmp(args[0], "exit") == 0) {
        int exit_code = 0;
        if (arg_count >= 2) {
            exit_code = atoi(args[1]);
        }
        last_exit_code = exit_code;
        return 0; // Exit the shell
    } else if (strcasecmp(args[0], "echo.") == 0) {
        printf("\n");
        return 1;
    } else if (strcmp(args[0], "for") == 0) {
        // For loops are not supported in this basic shell
        // In batch mode, just continue without executing
        if (batch_mode) {
            return 1;
        } else {
            printf("Command 'for' not supported in interactive mode\n");
            return 1;
        }
    } else if (strcasecmp(args[0], "choice") == 0) {
        // CHOICE command implementation - skip in batch mode to avoid user input
        if (batch_mode) {
            // In batch mode, just continue without executing (to avoid hanging)
            return 1;
        } else {
            // In interactive mode, we could implement the full choice functionality
            // For now, just show what would happen
            printf("CHOICE command (not fully implemented in this version):\n");
            for (int i = 1; i < arg_count; i++) {  // Start from 1 to skip "CHOICE"
                printf("  %s", args[i]);
                if (i < arg_count - 1) printf(" ");  // Add space between arguments except for the last
            }
            printf("\nThis command would pause for user input in interactive mode\n");
            return 1;
        }
    } else if (strcasecmp(args[0], "find") == 0) {
        // FIND command implementation - basic functionality
        if (arg_count < 2) {
            printf("FIND: Insufficient arguments\n");
            printf("Usage: FIND \"string\" [filename]\n");
            last_exit_code = 1;
            return 1;
        }
        
        char* search_string = args[1];
        char* filename = NULL;
        
        if (arg_count >= 3) {
            filename = args[2];
        }
        
        // If no filename provided, read from stdin in interactive mode
        if (filename == NULL && !batch_mode) {
            printf("FIND: Reading from stdin (Ctrl+D to end)\n");
            char line[MAX_CMD_LENGTH];
            int line_num = 1;
            int found_count = 0;
            
            while (fgets(line, sizeof(line), stdin) != NULL) {
                if (strstr(line, search_string) != NULL) {
                    printf("%s", line);
                    found_count++;
                }
                line_num++;
            }
            
            if (found_count == 0) {
                last_exit_code = 1; // Not found
            }
            return 1;
        } else if (filename != NULL) {
            // Search in the specified file
            FILE* file = fopen(filename, "r");
            if (file == NULL) {
                printf("FIND: %s: No such file or directory\n", filename);
                last_exit_code = 1;
                return 1;
            }
            
            char line[MAX_CMD_LENGTH];
            int line_num = 1;
            int found_count = 0;
            
            while (fgets(line, sizeof(line), file) != NULL) {
                if (strstr(line, search_string) != NULL) {
                    printf("%s", line);
                    found_count++;
                }
                line_num++;
            }
            
            fclose(file);
            
            if (found_count == 0) {
                last_exit_code = 1; // Not found
            }
            return 1;
        } else {
            printf("FIND: No filename provided in batch mode\n");
            last_exit_code = 1;
            return 1;
        }
    } else if (strcasecmp(args[0], "del") == 0 || strcasecmp(args[0], "erase") == 0) {
        // DEL/ERASE command implementation
        if (arg_count < 2) {
            printf("DEL: Insufficient arguments\n");
            printf("Usage: DEL filename\n");
            last_exit_code = 1;
            return 1;
        }
        
        if (batch_mode) {
            // In batch mode, just try to delete the file
            if (remove(args[1]) != 0) {
                printf("DEL: Cannot delete %s: %s\n", args[1], strerror(errno));
                last_exit_code = 1;
            }
        } else {
            // In interactive mode, maybe ask for confirmation
            char response[10];
            printf("Delete %s? (Y/N): ", args[1]);
            fflush(stdout);
            if (fgets(response, sizeof(response), stdin) && 
                (response[0] == 'Y' || response[0] == 'y')) {
                if (remove(args[1]) != 0) {
                    printf("DEL: Cannot delete %s: %s\n", args[1], strerror(errno));
                    last_exit_code = 1;
                }
            }
        }
        return 1;
    }

    // For other commands, try to execute them as external programs
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    } else if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            perror("Command not found");
            exit(EXIT_FAILURE);
        }
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            last_exit_code = WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
            last_exit_code = 128 + WTERMSIG(status);
        } else {
            last_exit_code = 1;
        }
    }

    return 1;
}

void free_args(char** args) {
    for (int i = 0; args[i] != NULL; i++) {
        free(args[i]);
    }
    free(args);
}

int handle_redirection(char** args, int arg_count) {
    // Look for redirection operators in the arguments
    for (int i = 0; i < arg_count; i++) {
        if (strcmp(args[i], ">") == 0 || strcmp(args[i], ">>") == 0 || strcmp(args[i], "<") == 0) {
            // Found a redirection operator
            if (i + 1 >= arg_count) {
                // No file specified after redirection operator
                printf("Syntax error: No file specified after redirection operator\n");
                last_exit_code = 1;
                return -1; // Indicate error
            }
            
            char* cmd = args[0]; // Command is first argument
            char* file = args[i + 1]; // File is after redirection operator
            
            // We need to handle the redirection by executing the command with proper file handling
            if (strcmp(args[i], "<") == 0) {
                // Input redirection - not implemented in this simple version
                printf("Input redirection not fully implemented\n");
                return -1;
            }
            
            // Create new argument list without redirection part
            char** new_args = malloc((i + 1) * sizeof(char*));
            if (new_args == NULL) {
                perror("malloc");
                return -1;
            }
            
            // Copy command and arguments before redirection
            for (int j = 0; j < i; j++) {
                new_args[j] = strdup(args[j]);
                if (new_args[j] == NULL) {
                    perror("strdup");
                    for (int k = 0; k < j; k++) {
                        free(new_args[k]);
                    }
                    free(new_args);
                    return -1;
                }
            }
            new_args[i] = NULL;
            
            // Special handling for echo command with redirection
            if (strcmp(cmd, "echo") == 0 && strcmp(args[i], ">") == 0) {
                FILE* f = fopen(file, "w");
                if (f == NULL) {
                    perror("fopen");
                    free_args(new_args);
                    last_exit_code = 1;
                    return -1;
                }
                
                // Write the echo content to the file
                for (int j = 1; j < i; j++) { // Start from 1 to skip "echo"
                    fputs(new_args[j], f);
                    if (j < i - 1) { // Add space between arguments
                        fputc(' ', f);
                    }
                }
                fputc('\n', f); // Add newline at the end
                fclose(f);
                
                free_args(new_args);
                return 0; // Success
            } else if (strcmp(cmd, "echo") == 0 && strcmp(args[i], ">>") == 0) {
                FILE* f = fopen(file, "a");
                if (f == NULL) {
                    perror("fopen");
                    free_args(new_args);
                    last_exit_code = 1;
                    return -1;
                }
                
                // Append the echo content to the file
                for (int j = 1; j < i; j++) { // Start from 1 to skip "echo"
                    fputs(new_args[j], f);
                    if (j < i - 1) { // Add space between arguments
                        fputc(' ', f);
                    }
                }
                fputc('\n', f); // Add newline at the end
                fclose(f);
                
                free_args(new_args);
                return 0; // Success
            } else {
                // For other commands, we would need to use pipes and redirection
                // which is more complex. For this implementation, we'll handle only echo.
                free_args(new_args);
                return -1; // Not a supported redirection case - continue with normal execution
            }
        }
    }
    
    // No redirection operators found
    return -1; // Indicate no redirection to handle
}

int is_batch_file(const char* filename) {
    const char* ext = strrchr(filename, '.');
    return (ext != NULL && (strcmp(ext, ".bat") == 0 || strcmp(ext, ".cmd") == 0));
}

int execute_batch_file(const char* filename) {
    // Set batch mode to true when executing a batch file
    batch_mode = 1;
    last_exit_code = 0;
    
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Cannot open file '%s'\n", filename);
        // Reset batch mode before returning
        batch_mode = 0;
        return 1;
    }

    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    int echo_on = 1; // Echo is on by default
    int script_exit_code = 0;

    while ((read = getline(&line, &len, file)) != -1) {
        // Remove trailing newline
        if (line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }

        // Skip empty lines
        char* trimmed = line;
        while (*trimmed && isspace((unsigned char)*trimmed)) {
            trimmed++;
        }
        if (*trimmed == '\0') {
            continue;
        }

        // Check for @echo on/off at the beginning of the line
        if (starts_with(trimmed, "@echo off")) {
            echo_on = 0;
            // Remove the @echo off command and continue to next line
            continue;
        } else if (starts_with(trimmed, "@echo on")) {
            echo_on = 1;
            // Remove the @echo on command and continue to next line
            continue;
        } else if (starts_with(trimmed, "echo off")) {
            echo_on = 0;
            continue;
        } else if (starts_with(trimmed, "echo on")) {
            echo_on = 1;
            continue;
        }

        // Skip REM or :: comments
        if (strncasecmp(trimmed, "rem", 3) == 0 && (trimmed[3] == '\0' || isspace((unsigned char)trimmed[3]))) {
            continue;
        }
        if (trimmed[0] == ':' && trimmed[1] == ':') {
            continue;
        }

        // Print command if echo is on and it doesn't start with @
        if (echo_on && trimmed[0] != '@') {
            printf("%s\n", line);
        }

        // Execute the command
        char** args;
        int arg_count;
        char* cmd_copy = strdup(trimmed);
        
        // Handle @ at the beginning of line (suppress echo)
        if (cmd_copy[0] == '@') {
            memmove(cmd_copy, cmd_copy + 1, strlen(cmd_copy)); // Remove the '@'
        }
        
        args = parse_command(cmd_copy, &arg_count);
        if (args != NULL) {
            int should_continue = execute_command(args, arg_count);
            if (last_exit_code != 0) {
                script_exit_code = last_exit_code;
            }
            free_args(args);
            if (should_continue == 0) {
                free(cmd_copy);
                break;
            }
        }
        free(cmd_copy);
    }

    free(line);
    fclose(file);
    
    // Reset batch mode after completing the batch file
    batch_mode = 0;
    return script_exit_code;
}

char* convert_win_to_unix_path(const char* win_path) {
    char* unix_path = malloc(strlen(win_path) + 1);
    if (unix_path == NULL) {
        return NULL;
    }

    strcpy(unix_path, win_path);

    // Convert Windows-style paths to Unix-style
    for (int i = 0; unix_path[i] != '\0'; i++) {
        if (unix_path[i] == '\\') {
            unix_path[i] = '/';
        }
    }

    // Handle drive letter mapping (e.g., C:\ to /)
    if (strlen(unix_path) >= 3 && 
        ((unix_path[0] >= 'A' && unix_path[0] <= 'Z') || (unix_path[0] >= 'a' && unix_path[0] <= 'z')) && 
        unix_path[1] == ':' && unix_path[2] == '/') {
        // Move the path after the drive letter to the beginning
        memmove(unix_path, unix_path + 2, strlen(unix_path) - 1);
    }

    return unix_path;
}

int starts_with(const char* str, const char* prefix) {
    return strncmp(str, prefix, strlen(prefix)) == 0;
}

void print_help() {
    printf("Batch Script Shell (BSS) Help\n");
    printf("Commands available:\n");
    printf("  echo [text]     - Display message or set echo on/off\n");
    printf("  set [variable[=value]] - Display or set environment variables\n");
    printf("  cd [directory]  - Change current directory\n");
    printf("  chdir [directory] - Change current directory\n");
    printf("  pwd             - Print working directory\n");
    printf("  cls/clear       - Clear screen\n");
    printf("  pause           - Pause and wait for keypress\n");
    printf("  help            - Show this help message\n");
    printf("  exit            - Exit the shell\n");
    printf("\n");
    printf("Additional features:\n");
    printf("  - Execute batch files (.bat, .cmd)\n");
    printf("  - Path conversion between Windows and Unix formats\n");
    printf("  - Environment variable expansion\n");
}
