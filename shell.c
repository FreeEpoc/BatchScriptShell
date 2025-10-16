#include "shell.h"
#include <time.h>

/* 
 * BSS (Batch Script Shell) - A shell program that interprets batch commands
 * 
 * This shell is designed to interpret basic batch commands used in Symbian build scripts,
 * with support for Windows-style paths, environment variables, and basic command execution.
 */

// Default version if not set by build system
#ifndef BSS_VERSION
#define BSS_VERSION "latest"
#endif

// Global shell state
ShellState shell_state = {
    .mount_count = 0,
    .current_dir = {0},
    .root_path = "/",
};

// Global echo state (on by default like in batch files)
int echo_enabled = 1;
int is_batch_mode = 0;

// Global command line arguments
char* command_line_args[64];
int num_command_line_args = 0;

// Environment variable stack for SETLOCAL/ENDLOCAL
char* env_stack[32][256];  // Stack of environment variable snapshots
int env_stack_size[32];    // Number of variables in each stack level
int env_stack_level = 0;   // Current stack level (0 = global)

// Function prototypes
int initialize_shell();
int execute_command(char* command);
int execute_builtin_command(char** args, int arg_count);
int execute_external_command(char** args, int arg_count);
char* expand_variables(char* command);
char* convert_path(char* path);
char* convert_path_to_windows(char* path);
int mount_directory(char* mount_point, char* target_path);
char* get_mounted_path(char* path);
int change_directory(char* path);
int print_working_directory();
int handle_echo(char** args, int arg_count);
int handle_set(char** args, int arg_count);
int handle_cd(char** args, int arg_count);
int handle_cls(char** args, int arg_count);
int handle_pause(char** args, int arg_count);
int handle_help(char** args, int arg_count);
int handle_setlocal(char** args, int arg_count);
int handle_endlocal(char** args, int arg_count);
int handle_if(char** args, int arg_count);

int main(int argc, char **argv) {
    char input[MAX_COMMAND_LENGTH];
    
    // Check for help flags
    if (argc == 2) {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            printf("Batch Script Shell (BSS) " BSS_VERSION "\n");
            printf("Usage: %s [options] [batch_file] [args...]\n", argv[0]);
            printf("\nOptions:\n");
            printf("  -h, --help     Show this help message\n");
            printf("\nDescription:\n");
            printf("  BSS is a shell program that interprets batch commands used in Windows/DOS environments.\n");
            printf("  It can run interactively or execute batch files.\n");
            printf("\nExamples:\n");
            printf("  %s                 # Run interactively\n", argv[0]);
            printf("  %s script.bat arg1 arg2  # Execute a batch file with arguments\n", argv[0]);
            printf("  %s --help          # Show this help\n", argv[0]);
            return 0;
        }
    }
    
    // Initialize shell once
    if (initialize_shell() != 0) {
        fprintf(stderr, "Failed to initialize shell\n");
        return 1;
    }
    
    // Store command line arguments (for %0, %1, %2, etc. access)
    num_command_line_args = 0;
    for (int i = 0; i < argc && i < 64; i++) {
        command_line_args[i] = argv[i];
        num_command_line_args++;
    }
    
    // Process command line argument if provided (batch file execution)
    if (argc > 1) {
        // Check if first argument is a batch file (not a flag)
        if (argv[1][0] != '-' || strcmp(argv[1], "--") == 0) {
            is_batch_mode = 1;  // Set batch mode flag
            
            // Execute batch file
            FILE *batch_file = fopen(argv[1], "r");
            if (batch_file == NULL) {
                perror("Error opening batch file");
                return 1;
            }
            
            printf("Batch Script Shell (BSS) " BSS_VERSION "\n");
            
            // Parse labels in the batch file first
            parse_batch_labels(batch_file, labels, &num_labels);
            
            // Execute batch file with GOTO support
            char* batch_lines[10000];  // Support up to 10000 lines
            int total_lines = 0;
            
            // Save current position
            long original_pos = ftell(batch_file);
            
            // Reset to beginning to read all lines
            fseek(batch_file, 0, SEEK_SET);
            
            // Read all lines into memory
            while (total_lines < 10000 && fgets(input, sizeof(input), batch_file)) {
                // Remove newline character
                input[strcspn(input, "\n")] = 0;
                
                batch_lines[total_lines] = malloc(strlen(input) + 1);
                strcpy(batch_lines[total_lines], input);
                total_lines++;
            }
            
            // Process lines with GOTO and CALL support
            for (int current_line = 0; current_line < total_lines; current_line++) {
                char* line = batch_lines[current_line];
                
                // Skip empty lines and labels
                if (strlen(line) == 0 || is_label_line(line)) {
                    continue;
                }
                
                // Make a copy of the line to tokenize without modifying the original
                char temp_line[MAX_COMMAND_LENGTH];
                strcpy(temp_line, line);
                
                // Tokenize the command to check the first word
                char* token = strtok(temp_line, " \t");
                if (token != NULL) {
                    if (strcasecmp(token, "goto") == 0) {
                        // Handle GOTO command
                        char* label_name = strtok(NULL, " \t");
                        if (label_name != NULL) {
                            // Find the label in our collection
                            for (int i = 0; i < total_lines; i++) {
                                if (is_label_line(batch_lines[i])) {
                                    char extracted_label[256];
                                    extract_label_name(batch_lines[i], extracted_label);
                                    if (strcasecmp(extracted_label, label_name) == 0) {
                                        current_line = i;  // Set to the label line index
                                        break;
                                    }
                                }
                            }
                        } else {
                            fprintf(stderr, "GOTO: No label specified\n");
                        }
                    } else if (strcasecmp(token, "call") == 0) {
                        // Handle CALL command
                        char* label_name = strtok(NULL, " \t");
                        if (label_name != NULL) {
                            // Save return address (current position + 1, to continue after this line)
                            if (call_stack_size < 100) {
                                call_stack[call_stack_size].return_line = current_line + 1;
                                call_stack_size++;
                                
                                // Jump to the label
                                for (int i = 0; i < total_lines; i++) {
                                    if (is_label_line(batch_lines[i])) {
                                        char extracted_label[256];
                                        extract_label_name(batch_lines[i], extracted_label);
                                        if (strcasecmp(extracted_label, label_name) == 0) {
                                            current_line = i;  // Set to the label line index
                                            break;
                                        }
                                    }
                                }
                            } else {
                                fprintf(stderr, "CALL: Call stack overflow\n");
                            }
                        } else {
                            fprintf(stderr, "CALL: No label specified\n");
                        }
                    } else if (strcasecmp(token, "exit") == 0) {
                        // Handle exit command
                        char* param = strtok(NULL, " \t");
                        if (param && strcasecmp(param, "/B") == 0) {
                            // EXIT /B: Return from a subroutine called with CALL
                            if (call_stack_size > 0) {
                                // Pop the return address from the call stack
                                call_stack_size--;
                                int return_line = call_stack[call_stack_size].return_line;
                                current_line = return_line - 1;  // -1 because the loop will increment
                            } else {
                                // No return address, just exit the batch file
                                for (int i = 0; i < total_lines; i++) {
                                    free(batch_lines[i]);
                                }
                                fclose(batch_file);
                                return 0;
                            }
                        } else {
                            // Regular EXIT with optional exit code
                            int exit_code = 0;
                            if (param) {
                                exit_code = atoi(param);
                            }
                            // Free allocated memory before exiting
                            for (int i = 0; i < total_lines; i++) {
                                free(batch_lines[i]);
                            }
                            fclose(batch_file);
                            return exit_code;
                        }
                    } else {
                        // Execute normal command
                        execute_command(line);
                    }
                } else {
                    // Execute normal command if line has only whitespace
                    execute_command(line);
                }
            }
            
            // Free allocated memory
            for (int i = 0; i < total_lines; i++) {
                free(batch_lines[i]);
            }
            
            fclose(batch_file);
            return 0;
        }
    }
    
    // Interactive mode
    printf("Batch Script Shell (BSS) " BSS_VERSION "\n");
    while (1) {
        // Display Windows-like prompt with current directory in Windows format
        char* windows_path = convert_path_to_windows(shell_state.current_dir);
        printf("%s> ", windows_path);
        fflush(stdout);
        
        if (!fgets(input, sizeof(input), stdin)) {
            break; // EOF
        }
        
        // Remove newline character
        input[strcspn(input, "\n")] = 0;
        
        if (strlen(input) == 0) {
            continue;
        }
        
        // Handle exit command
        if (strcmp(input, "exit") == 0) {
            break;
        }
        
        execute_command(input);
    }
    
    return 0;
}

int initialize_shell() {
    // Set default directory to mimic C:\Users\sblo on Windows
    const char* default_dir = "/home/sblo";  // Linux equivalent of Windows user directory
    
    // Try to change to default directory first
    if (chdir(default_dir) == 0) {
        // Successfully changed directory
        if (getcwd(shell_state.current_dir, sizeof(shell_state.current_dir)) == NULL) {
            perror("getcwd after changing to default directory");
            return 1;
        }
    } else {
        // If default directory doesn't exist, use current directory
        if (getcwd(shell_state.current_dir, sizeof(shell_state.current_dir)) == NULL) {
            perror("getcwd");
            return 1;
        }
    }
    
    // Set default root mapping (Linux root "/" binds to "C:\\")
    strcpy(shell_state.root_path, "/");
    
    // Initialize environment variables
    setenv("OS", "Linux", 1);  // Default OS environment variable
    setenv("USERNAME", "sblo", 1);  // Set default username
    setenv("USERPROFILE", default_dir, 1);  // Set user profile
    
    return 0;
}

int execute_command(char* command) {
    char temp_command[MAX_COMMAND_LENGTH];
    strcpy(temp_command, command);
    
    // Handle @ prefix (command echo suppression)
    int should_echo = echo_enabled;
    if (temp_command[0] == '@') {
        should_echo = 0;  // Don't echo this command
        // Skip the @ and any following whitespace
        char* cmd_start = temp_command + 1;
        while (*cmd_start == ' ' || *cmd_start == '\t') {
            cmd_start++;
        }
        memmove(temp_command, cmd_start, strlen(cmd_start) + 1);
    }
    
    // Check for REM or :: comments
    // Skip leading whitespace to check for comment indicators
    char* cmd_start = temp_command;
    while (*cmd_start == ' ' || *cmd_start == '\t') {
        cmd_start++;
    }
    
    // Check if it's a REM comment
    if (strncasecmp(cmd_start, "REM", 3) == 0) {
        // This is a REM comment, skip execution
        // But still print it in batch mode if echo is enabled
        if (is_batch_mode && echo_enabled && should_echo) {
            printf("BSS> %s\n", command);
        }
        return 0;
    }
    
    // Check if it's a :: comment (double colon)
    if (cmd_start[0] == ':' && cmd_start[1] == ':') {
        // This is a :: comment, skip execution
        // But still print it in batch mode if echo is enabled
        if (is_batch_mode && echo_enabled && should_echo) {
            printf("BSS> %s\n", command);
        }
        return 0;
    }
    
    // Only print the command in batch mode when echo is enabled (and it's not suppressed with @)
    if (is_batch_mode && echo_enabled && should_echo) {
        printf("BSS> %s\n", command);
    }
    
    // Expand variables in the command
    char* expanded_cmd = expand_variables(temp_command);
    if (!expanded_cmd) {
        return -1;
    }
    
    // Parse command into arguments
    char* args[MAX_ARGS];
    int arg_count = 0;
    
    // Tokenize the command
    char* token = strtok(expanded_cmd, " \t");
    while (token != NULL && arg_count < MAX_ARGS - 1) {
        args[arg_count++] = token;
        token = strtok(NULL, " \t");
    }
    args[arg_count] = NULL;
    
    if (arg_count == 0) {
        return 0; // Empty command
    }
    
    // Check if it's a built-in command
    return execute_builtin_command(args, arg_count);
}

// Structure to store label information


// Global variables for GOTO functionality
Label labels[1000];  // Support up to 1000 labels
int num_labels = 0;

// Global variables for CALL functionality
CallStackEntry call_stack[100];  // Support up to 100 nested calls
int call_stack_size = 0;

// Function to check if a line is a label
int is_label_line(char* line) {
    // A label line starts with : followed by a name (e.g., :label_name)
    if (line[0] != ':') {
        return 0;
    }
    
    // Skip the first colon
    char* label_name = line + 1;
    
    // Skip any leading whitespace
    while (*label_name == ' ' || *label_name == '\t') {
        label_name++;
    }
    
    if (*label_name == '\0') {
        return 0;  // Empty label
    }
    
    // Check if it's a :: comment (not a label)
    if (label_name[0] == ':') {
        return 0;
    }
    
    return 1;
}

// Function to extract label name from a label line
void extract_label_name(char* line, char* label_name) {
    // Skip the first colon
    char* start = line + 1;
    
    // Skip any leading whitespace
    while (*start == ' ' || *start == '\t') {
        start++;
    }
    
    // Copy until we hit whitespace or end of line
    int i = 0;
    while (*start && *start != ' ' && *start != '\t' && *start != '\r' && *start != '\n') {
        if (i < 255) {
            label_name[i] = *start;
            i++;
        }
        start++;
    }
    
    label_name[i] = '\0';
}

// Function to find a label by name
int find_label_index(char* label_name) {
    for (int i = 0; i < num_labels; i++) {
        if (strcasecmp(labels[i].name, label_name) == 0) {
            return i;
        }
    }
    return -1;
}

// Function to parse a batch file and collect all labels
int parse_batch_labels(FILE* batch_file, Label* label_list, int* num_labels_ptr) {
    char line[MAX_COMMAND_LENGTH];
    int line_number = 0;
    int current_label_count = 0;
    
    // Save current position
    long original_pos = ftell(batch_file);
    
    // Seek to the beginning of the file
    fseek(batch_file, 0, SEEK_SET);
    
    while (fgets(line, sizeof(line), batch_file)) {
        line_number++;
        
        if (is_label_line(line)) {
            if (current_label_count < 1000) {
                extract_label_name(line, label_list[current_label_count].name);
                label_list[current_label_count].line_number = line_number;
                current_label_count++;
            }
        }
    }
    
    *num_labels_ptr = current_label_count;
    
    // Restore original position
    fseek(batch_file, original_pos, SEEK_SET);
    
    return 0;
}

int execute_builtin_command(char** args, int arg_count) {
    // Handle built-in commands
    if (strcmp(args[0], "if") == 0) {
        return handle_if(args, arg_count);
    } else if (strcmp(args[0], "setlocal") == 0) {
        return handle_setlocal(args, arg_count);
    } else if (strcmp(args[0], "endlocal") == 0) {
        return handle_endlocal(args, arg_count);
    } else if (strcmp(args[0], "help") == 0) {
        return handle_help(args, arg_count);
    } else if (strcmp(args[0], "echo") == 0) {
        return handle_echo(args, arg_count);
    } else if (strcmp(args[0], "set") == 0) {
        return handle_set(args, arg_count);
    } else if (strcmp(args[0], "cd") == 0 || strcmp(args[0], "chdir") == 0) {
        return handle_cd(args, arg_count);
    } else if (strcmp(args[0], "cls") == 0 || strcmp(args[0], "clear") == 0) {
        return handle_cls(args, arg_count);
    } else if (strcmp(args[0], "pwd") == 0) {
        return print_working_directory();
    } else if (strcmp(args[0], "pause") == 0) {
        return handle_pause(args, arg_count);
    } else if (strcmp(args[0], "goto") == 0) {
        // GOTO is handled specially in batch file execution
        return 0; // This command is processed during batch file parsing
    } else if (strcmp(args[0], "call") == 0) {
        // CALL is handled specially in batch file execution
        return 0; // This command is processed during batch file parsing
    } else if (strcmp(args[0], "mkdir") == 0 || strcmp(args[0], "md") == 0) {
        // For now, just execute as external command
        return execute_external_command(args, arg_count);
    } else if (strcmp(args[0], "rmdir") == 0 || strcmp(args[0], "rd") == 0) {
        return execute_external_command(args, arg_count);
    } else if (strcmp(args[0], "del") == 0 || strcmp(args[0], "erase") == 0) {
        return execute_external_command(args, arg_count);
    } else if (strcmp(args[0], "copy") == 0) {
        return execute_external_command(args, arg_count);
    } else if (strcmp(args[0], "move") == 0) {
        return execute_external_command(args, arg_count);
    } else if (strcmp(args[0], "type") == 0 || strcmp(args[0], "cat") == 0) {
        return execute_external_command(args, arg_count);
    } else if (strcmp(args[0], "for") == 0) {
        return handle_for_loop(args, arg_count);
    } else {
        // Not a built-in command, try external command
        return execute_external_command(args, arg_count);
    }
}

int execute_external_command(char** args, int arg_count) {
    // Check for pipe operators in the command
    int pipe_pos = -1;
    for (int i = 0; i < arg_count; i++) {
        if (strcmp(args[i], "|") == 0) {
            pipe_pos = i;
            break;
        }
    }
    
    if (pipe_pos != -1) {
        // Handle piping between commands
        return execute_pipe_command(args, arg_count, pipe_pos);
    }
    
    // Check for redirection operators in the command
    char* input_redirect = NULL;
    char* output_redirect = NULL;
    char* error_redirect = NULL;
    int append_mode = 0;
    int args_count = arg_count;
    
    // Parse arguments to find redirection operators
    for (int i = 0; i < arg_count; i++) {
        if (strcmp(args[i], ">") == 0 || strcmp(args[i], ">>") == 0) {
            if (i + 1 >= arg_count) {
                fprintf(stderr, "Syntax error: No file specified after redirection operator\n");
                return 1;
            }
            
            output_redirect = args[i + 1];
            append_mode = (strcmp(args[i], ">>") == 0) ? 1 : 0;
            args[i] = NULL;  // Terminate the argument list before redirection
            args_count = i;
            break;
        } else if (strcmp(args[i], "<") == 0) {
            if (i + 1 >= arg_count) {
                fprintf(stderr, "Syntax error: No file specified after input redirection operator\n");
                return 1;
            }
            
            input_redirect = args[i + 1];
            args[i] = NULL;  // Terminate the argument list before redirection
            args_count = i;
            break;
        } else if (strcmp(args[i], "2>") == 0) {
            if (i + 1 >= arg_count) {
                fprintf(stderr, "Syntax error: No file specified after error redirection operator\n");
                return 1;
            }
            
            error_redirect = args[i + 1];
            args[i] = NULL;  // Terminate the argument list before redirection
            args_count = i;
            break;
        } else if (strcmp(args[i], "2>&1") == 0) {
            // Redirect stderr to stdout
            error_redirect = "STDOUT";
            args[i] = NULL;  // Terminate the argument list before redirection
            args_count = i;
            break;
        }
    }
    
    // Fork and execute external command
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("fork");
        return 1;
    } else if (pid == 0) {
        // Child process - handle redirection and execute command
        
        // Handle input redirection
        if (input_redirect) {
            int input_fd = open(input_redirect, O_RDONLY);
            if (input_fd < 0) {
                perror("Error opening input file for redirection");
                exit(1);
            }
            dup2(input_fd, STDIN_FILENO);
            close(input_fd);
        }
        
        // Handle output redirection
        if (output_redirect) {
            int output_fd;
            if (append_mode) {
                output_fd = open(output_redirect, O_WRONLY | O_CREAT | O_APPEND, 0644);
            } else {
                output_fd = open(output_redirect, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            }
            if (output_fd < 0) {
                perror("Error opening output file for redirection");
                exit(1);
            }
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
        }
        
        // Handle error redirection
        if (error_redirect) {
            if (strcmp(error_redirect, "STDOUT") == 0) {
                // Redirect stderr to stdout
                dup2(STDOUT_FILENO, STDERR_FILENO);
            } else {
                int error_fd = open(error_redirect, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (error_fd < 0) {
                    perror("Error opening error file for redirection");
                    exit(1);
                }
                dup2(error_fd, STDERR_FILENO);
                close(error_fd);
            }
        }
        
        // Create a new argument list without redirection parts
        char* new_args[args_count + 1];
        for (int i = 0; i < args_count; i++) {
            new_args[i] = args[i];
        }
        new_args[args_count] = NULL;
        
        execvp(new_args[0], new_args);
        perror("Command execution failed");
        exit(1);
    } else {
        // Parent process - wait for child
        int status;
        waitpid(pid, &status, 0);
        return WEXITSTATUS(status);
    }
}

// Function to execute command with pipe
int execute_pipe_command(char** args, int arg_count, int pipe_pos) {
    // Create the pipe
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }
    
    pid_t pid1 = fork();
    if (pid1 == -1) {
        perror("fork");
        return 1;
    }
    
    if (pid1 == 0) {
        // First command in pipe (left side)
        close(pipefd[0]); // Close read end
        dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe write end
        close(pipefd[1]);
        
        // Prepare arguments for first command
        char* cmd1_args[pipe_pos + 1];
        for (int i = 0; i < pipe_pos; i++) {
            cmd1_args[i] = args[i];
        }
        cmd1_args[pipe_pos] = NULL;
        
        execvp(cmd1_args[0], cmd1_args);
        perror("First command in pipe failed");
        exit(1);
    }
    
    pid_t pid2 = fork();
    if (pid2 == -1) {
        perror("fork");
        return 1;
    }
    
    if (pid2 == 0) {
        // Second command in pipe (right side)
        close(pipefd[1]); // Close write end
        dup2(pipefd[0], STDIN_FILENO); // Redirect stdin to pipe read end
        close(pipefd[0]);
        
        // Prepare arguments for second command
        int remaining_args = arg_count - pipe_pos - 1;
        char* cmd2_args[remaining_args + 1];
        for (int i = 0; i < remaining_args; i++) {
            cmd2_args[i] = args[pipe_pos + 1 + i];
        }
        cmd2_args[remaining_args] = NULL;
        
        execvp(cmd2_args[0], cmd2_args);
        perror("Second command in pipe failed");
        exit(1);
    }
    
    // Parent process
    close(pipefd[0]);
    close(pipefd[1]);
    
    // Wait for both processes
    int status1, status2;
    waitpid(pid1, &status1, 0);
    waitpid(pid2, &status2, 0);
    
    // Return the exit status of the last command in the pipe
    return WEXITSTATUS(status2);
}

char* expand_variables(char* command) {
    // This is a simplified variable expansion
    static char result[MAX_COMMAND_LENGTH];
    strcpy(result, command);
    
    // First, handle command line arguments like %1, %2, etc.
    char* p = result;
    while (*p) {
        if (*p == '%' && *(p + 1) != '\0') {
            // Check if it's a command line argument like %1, %2, etc.
            if (*(p + 1) >= '0' && *(p + 1) <= '9') {
                char arg_num_str[3];
                char* arg_num_ptr = p + 1;
                int arg_num_len = 0;
                
                // Parse the argument number
                while (*(arg_num_ptr + arg_num_len) >= '0' && *(arg_num_ptr + arg_num_len) <= '9') {
                    arg_num_len++;
                    if (arg_num_len >= 2) break; // Limit to 2 digits
                }
                
                strncpy(arg_num_str, arg_num_ptr, arg_num_len);
                arg_num_str[arg_num_len] = '\0';
                
                int arg_num = atoi(arg_num_str);
                
                // Replace %n with the nth command line argument
                if (arg_num >= 0 && arg_num < num_command_line_args) {
                    char* arg_value = command_line_args[arg_num];
                    if (arg_value != NULL) {
                        // Create new string with substitution
                        int before_len = p - result;
                        int after_pos = (p - result) + 1 + arg_num_len; // Position after %n
                        int value_len = strlen(arg_value);
                        int after_len = strlen(result + after_pos);
                        
                        char temp[MAX_COMMAND_LENGTH];
                        if (before_len > 0) {
                            strncpy(temp, result, before_len);
                        }
                        temp[before_len] = '\0';
                        strcat(temp, arg_value);
                        strcat(temp, result + after_pos);
                        
                        // Update result and continue from new position
                        strcpy(result, temp);
                        p = temp + before_len + value_len;
                        continue; // Restart processing from new position
                    }
                }
                // If argument not found, just advance past this
                p += 1 + arg_num_len;
                continue;
            }
        }
        p++;
    }
    
    // Now handle regular environment variables %VAR%
    p = result;
    while ((p = strchr(p, '%')) != NULL) {
        char* end = strchr(p + 1, '%');
        if (end == NULL) break; // No closing %
        
        *end = '\0';
        char var_name[256];
        strcpy(var_name, p + 1); // Skip the opening %
        
        // Get the environment variable value
        char* value = getenv(var_name);
        if (value != NULL) {
            // Replace the %VAR% with its value
            int before_len = p - result;
            int after_pos = end - result + 1; // Include the closing %
            int value_len = strlen(value);
            int after_len = strlen(result + after_pos);
            
            // Create the new string with substitution
            char temp[MAX_COMMAND_LENGTH];
            strncpy(temp, result, before_len);
            temp[before_len] = '\0';
            strcat(temp, value);
            strcat(temp, result + after_pos);
            
            // Update result and restart search
            strcpy(result, temp);
            p = temp + before_len + value_len;
        } else {
            // Variable not found, just move past it
            *end = '%';  // Restore the %
            p = end + 1;
        }
    }
    
    return result;
}

char* convert_path(char* path) {
    // Convert Windows-style paths to Linux-style
    static char converted[MAX_PATH_LENGTH];
    strcpy(converted, path);
    
    // Replace backslashes with forward slashes
    char* p = converted;
    while (*p) {
        if (*p == '\\') {
            *p = '/';
        }
        p++;
    }
    
    // Handle absolute paths that start with drive letter like C:/
    if (strlen(converted) >= 3 && converted[1] == ':' && converted[2] == '/') {
        // Replace C:/ with /c/ or however you want to handle drive letters
        memmove(converted, converted + 2, strlen(converted) - 1);
        converted[0] = '/';
    }
    
    return converted;
}

char* convert_path_to_windows(char* path) {
    // Convert Linux-style paths to Windows-style
    static char converted[MAX_PATH_LENGTH];
    strcpy(converted, path);
    
    // Handle root directory mapping: / -> C: 
    if (strcmp(converted, "/") == 0) {
        strcpy(converted, "C:\\\\");
        return converted;
    }
    
    // Check if this is the root directory or a mounted path
    if (converted[0] == '/') {
        // Map / to C: and convert forward slashes to backslashes
        memmove(converted + 3, converted + 1, strlen(converted) + 1); // Make space for C:
        converted[0] = 'C';
        converted[1] = ':';
        converted[2] = '\\';
        
        // Replace forward slashes with backslashes
        char* p = converted + 3;  // Start after C:
        while (*p) {
            if (*p == '/') {
                *p = '\\';
            }
            p++;
        }
    }
    
    return converted;
}

int mount_directory(char* mount_point, char* target_path) {
    if (shell_state.mount_count >= 32) {
        fprintf(stderr, "Too many mount points\n");
        return -1;
    }
    
    strncpy(shell_state.mounts[shell_state.mount_count].mount_point, mount_point, MAX_PATH_LENGTH - 1);
    shell_state.mounts[shell_state.mount_count].mount_point[MAX_PATH_LENGTH - 1] = '\0';
    
    strncpy(shell_state.mounts[shell_state.mount_count].target_path, target_path, MAX_PATH_LENGTH - 1);
    shell_state.mounts[shell_state.mount_count].target_path[MAX_PATH_LENGTH - 1] = '\0';
    
    shell_state.mount_count++;
    return 0;
}

char* get_mounted_path(char* path) {
    // Check if path is mounted and return actual path
    for (int i = 0; i < shell_state.mount_count; i++) {
        if (strncmp(path, shell_state.mounts[i].mount_point, strlen(shell_state.mounts[i].mount_point)) == 0) {
            static char actual_path[MAX_PATH_LENGTH];
            strcpy(actual_path, shell_state.mounts[i].target_path);
            strcat(actual_path, path + strlen(shell_state.mounts[i].mount_point));
            return actual_path;
        }
    }
    
    // Not mounted, return original path
    return path;
}

int change_directory(char* path) {
    char converted_path[MAX_PATH_LENGTH];
    strcpy(converted_path, path);
    
    // Convert path format if needed
    char* final_path = convert_path(converted_path);
    
    // Apply any mount points
    final_path = get_mounted_path(final_path);
    
    if (chdir(final_path) != 0) {
        perror("chdir");
        return -1;
    }
    
    // Update current working directory
    if (getcwd(shell_state.current_dir, sizeof(shell_state.current_dir)) == NULL) {
        perror("getcwd after chdir");
        return -1;
    }
    
    return 0;
}

int print_working_directory() {
    printf("%s\n", shell_state.current_dir);
    return 0;
}

int handle_echo(char** args, int arg_count) {
    if (arg_count < 2) {
        printf("ECHO is %s\n", echo_enabled ? "on" : "off");
        return 0;
    }
    
    // Handle echo off and echo on
    if (arg_count == 2) {
        if (strcasecmp(args[1], "off") == 0) {
            echo_enabled = 0;  // Turn echo off
            return 0;
        } else if (strcasecmp(args[1], "on") == 0) {
            echo_enabled = 1;  // Turn echo on
            return 0;
        }
    }
    
    // Print the message (only if it's not echo on/off)
    for (int i = 1; i < arg_count; i++) {
        printf("%s", args[i]);
        if (i < arg_count - 1) {
            printf(" ");
        }
    }
    printf("\n");
    
    return 0;
}

// Function to evaluate arithmetic and bitwise expressions
long evaluate_arithmetic_expression(char* expr) {
    // This is a simple evaluator that handles +, -, *, /, %, &, |, ^
    // It expects expressions like: "5 + 3" or "10 * 2" or "5 & 3"
    // For simplicity, we'll implement a basic left-to-right evaluation (no precedence)
    
    // First, expand any variables in the expression
    char expanded_expr[MAX_COMMAND_LENGTH];
    strcpy(expanded_expr, expr);
    
    // Expand variables in the expression
    char* var_expanded = expand_variables(expanded_expr);
    strcpy(expanded_expr, var_expanded);
    
    // Tokenize the expression
    char tokens[64][64];  // Store up to 64 tokens, max 64 chars each
    int token_count = 0;
    char* token = strtok(expanded_expr, " \t");
    
    while (token != NULL && token_count < 64) {
        strcpy(tokens[token_count], token);
        token_count++;
        token = strtok(NULL, " \t");
    }
    
    if (token_count == 0) {
        return 0;  // Empty expression
    }
    
    // Parse the first number
    long result = strtol(tokens[0], NULL, 0);
    
    // Process the expression left to right
    for (int i = 1; i < token_count; i += 2) {
        if (i + 1 >= token_count) break;  // Out of operands
        
        char* op = tokens[i];
        long next_num = strtol(tokens[i + 1], NULL, 0);
        
        if (strcmp(op, "+") == 0) {
            result += next_num;
        } else if (strcmp(op, "-") == 0) {
            result -= next_num;
        } else if (strcmp(op, "*") == 0) {
            result *= next_num;
        } else if (strcmp(op, "/") == 0) {
            if (next_num != 0) {
                result /= next_num;
            } else {
                printf("Error: Division by zero\n");
                return 0;
            }
        } else if (strcmp(op, "%") == 0) {
            if (next_num != 0) {
                result %= next_num;
            } else {
                printf("Error: Division by zero for modulo\n");
                return 0;
            }
        } else if (strcmp(op, "&") == 0) {
            result &= next_num;  // Bitwise AND
        } else if (strcmp(op, "|") == 0) {
            result |= next_num;  // Bitwise OR
        } else if (strcmp(op, "^") == 0) {
            result ^= next_num;  // Bitwise XOR
        } else {
            printf("Error: Unknown operator '%s'\n", op);
            return 0;
        }
    }
    
    return result;
}

int handle_set(char** args, int arg_count) {
    if (arg_count == 1) {
        // List all environment variables
        extern char **environ;
        for (char **env = environ; *env != NULL; env++) {
            printf("%s\n", *env);
        }
        return 0;
    } else if (arg_count == 2) {
        // Show specific environment variable
        char var_name[256];
        strcpy(var_name, args[1]);
        
        // Remove trailing = if present
        char* equals = strchr(var_name, '=');
        if (equals) {
            *equals = '\0';
        }
        
        char* value = getenv(var_name);
        if (value != NULL) {
            printf("%s=%s\n", var_name, value);
        } else {
            printf("%s\n", var_name);  // Variable not set
        }
        return 0;
    } else if (arg_count >= 3) {
        // Check if this is a SET /A command
        if (strcasecmp(args[1], "/A") == 0 && arg_count >= 4) {
            // Format: SET /A variable=expression or SET /A variable+=expression, etc.
            char assignment[MAX_COMMAND_LENGTH];
            strcpy(assignment, args[2]);
            
            // Combine all remaining args as the expression
            for (int i = 3; i < arg_count; i++) {
                strcat(assignment, " ");
                strcat(assignment, args[i]);
            }
            
            // Check for assignment operators (+=, -=, *=, /=, %=)
            char* assignment_op = NULL;
            char op_type = 0; // 0=none, 1=+=, 2-=, 3=*=, 4=/=, 5=%=
            
            // Find assignment operator in the variable assignment
            if ((assignment_op = strstr(assignment, "+=")) != NULL) {
                op_type = 1;
            } else if ((assignment_op = strstr(assignment, "-=")) != NULL) {
                op_type = 2;
            } else if ((assignment_op = strstr(assignment, "*=")) != NULL) {
                op_type = 3;
            } else if ((assignment_op = strstr(assignment, "/=")) != NULL) {
                op_type = 4;
            } else if ((assignment_op = strstr(assignment, "%=")) != NULL) {
                op_type = 5;
            }
            
            if (op_type != 0) {
                // Handle assignment operators
                *assignment_op = '\0';  // Split at assignment operator
                char* var = assignment;
                
                // Move past the operator symbols to get the expression
                char* expression = assignment_op + 2;
                
                // Get the current value of the variable
                char* current_val_str = getenv(var);
                long current_val = 0;
                if (current_val_str != NULL) {
                    current_val = strtol(current_val_str, NULL, 0);
                }
                
                // Evaluate the expression
                long expr_val = evaluate_arithmetic_expression(expression);
                
                // Apply the operation
                long result;
                switch (op_type) {
                    case 1: // +=
                        result = current_val + expr_val;
                        break;
                    case 2: // -=
                        result = current_val - expr_val;
                        break;
                    case 3: // *=
                        result = current_val * expr_val;
                        break;
                    case 4: // /=
                        if (expr_val != 0) {
                            result = current_val / expr_val;
                        } else {
                            printf("Error: Division by zero\n");
                            return 1;
                        }
                        break;
                    case 5: // %=
                        if (expr_val != 0) {
                            result = current_val % expr_val;
                        } else {
                            printf("Error: Division by zero for modulo\n");
                            return 1;
                        }
                        break;
                    default:
                        result = expr_val; // Should not happen
                        break;
                }
                
                // Convert result to string and set the variable
                char result_str[64];
                sprintf(result_str, "%ld", result);
                
                if (setenv(var, result_str, 1) != 0) {
                    perror("setenv");
                    return 1;
                }
                
                // In batch script style, also print the assignment
                printf("%s=%s\n", var, result_str);
            } else {
                // Regular assignment: variable=expression
                char* equals = strchr(assignment, '=');
                if (equals != NULL) {
                    *equals = '\0';  // Split at equals
                    char* var = assignment;
                    char* expression = equals + 1;
                    
                    // Evaluate the arithmetic expression
                    long result = evaluate_arithmetic_expression(expression);
                    
                    // Convert result to string and set the variable
                    char result_str[64];
                    sprintf(result_str, "%ld", result);
                    
                    if (setenv(var, result_str, 1) != 0) {
                        perror("setenv");
                        return 1;
                    }
                    
                    // In batch script style, also print the assignment
                    printf("%s=%s\n", var, result_str);
                } else {
                    printf("Invalid SET /A command format. Use: SET /A variable=expression\n");
                    return 1;
                }
            }
        } else {
            // Regular SET command - Format: SET var=value
            char assignment[MAX_COMMAND_LENGTH];
            strcpy(assignment, args[1]);
            
            // Combine all remaining args as the value
            for (int i = 2; i < arg_count; i++) {
                strcat(assignment, " ");
                strcat(assignment, args[i]);
            }
            
            // Find the equals sign
            char* equals = strchr(assignment, '=');
            if (equals != NULL) {
                *equals = '\0';  // Split at equals
                char* var = assignment;
                char* value = equals + 1;
                
                if (setenv(var, value, 1) != 0) {
                    perror("setenv");
                    return 1;
                }
            } else {
                printf("Invalid SET command format. Use: SET variable=value\n");
                return 1;
            }
        }
    }
    
    return 0;
}

int handle_cd(char** args, int arg_count) {
    if (arg_count < 2) {
        print_working_directory();
        return 0;
    }
    
    return change_directory(args[1]);
}

int handle_cls(char** args, int arg_count) {
    // Clear the screen (system dependent)
    system("clear");  // For Linux/Unix
    return 0;
}

int handle_pause(char** args, int arg_count) {
    printf("Press any key to continue . . . ");
    fflush(stdout);
    
    // Open /dev/tty to read directly from terminal, not from stdin which might be redirected
    int tty_fd = open("/dev/tty", O_RDONLY);
    if (tty_fd == -1) {
        // Fallback to stdin if /dev/tty is not available
        char c;
        read(0, &c, 1);
    } else {
        char c;
        read(tty_fd, &c, 1);
        close(tty_fd);
    }
    
    printf("\n");
    
    return 0;
}

int handle_help(char** args, int arg_count) {
    if (arg_count == 1) {
        // Show general help
        printf("Batch Script Shell (BSS) Help\n");
        printf("Commands available:\n");
        printf("  help                - Show this help message\n");
        printf("  setlocal            - Begin localization of environment variables\n");
        printf("  endlocal            - End localization of environment variables\n");
        printf("  echo [message]      - Display message or set echo on/off\n");
        printf("  set [variable[=value]] - Display or set environment variables\n");
        printf("  cd <directory>      - Change current directory\n");
        printf("  chdir <directory>   - Change current directory (alias for cd)\n");
        printf("  pwd                 - Print current working directory\n");
        printf("  cls                 - Clear screen\n");
        printf("  clear               - Clear screen (alias for cls)\n");
        printf("  pause               - Wait for user to press a key\n");
        printf("  exit                - Exit the shell\n");
        printf("  md <directory>      - Create directory\n");
        printf("  mkdir <directory>   - Create directory (alias for md)\n");
        printf("  rd <directory>      - Remove directory\n");
        printf("  rmdir <directory>   - Remove directory (alias for rd)\n");
        printf("  del <file>          - Delete file\n");
        printf("  erase <file>        - Delete file (alias for del)\n");
        printf("  copy <src> <dest>   - Copy files\n");
        printf("  move <src> <dest>   - Move files\n");
        printf("  type <file>         - Display content of file\n");
        printf("  cat <file>          - Display content of file (alias for type)\n");
        printf("\nSpecial variables:\n");
        printf("  %%0, %%1, %%2, ...      - Command line arguments\n");
        printf("  %%OS%%               - Operating system name\n");
        printf("  Environment variables like %%PATH%%, %%USERNAME%%, etc.\n");
        printf("\nFor more information on a specific command, type HELP command-name\n");
    } else {
        // Show help for specific command
        if (strcmp(args[1], "setlocal") == 0) {
            printf("SETLOCAL\n");
            printf("  Begins localization of environment variables in a batch file.\n");
            printf("  All environment changes after this command are local to the batch file.\n");
        } else if (strcmp(args[1], "endlocal") == 0) {
            printf("ENDLOCAL\n");
            printf("  Ends localization of environment variables begun by SETLOCAL.\n");
            printf("  Restores environment variables to the way they were at the time of the\n");
            printf("  matched SETLOCAL command.\n");
        } else if (strcmp(args[1], "echo") == 0) {
            printf("ECHO [ON | OFF] | [message]\n");
            printf("  Sets echo on or off, or displays a message.\n");
            printf("  ECHO ON    - Enable command echoing\n");
            printf("  ECHO OFF   - Disable command echoing\n");
            printf("  ECHO message - Display message\n");
        } else if (strcmp(args[1], "exit") == 0) {
            printf("EXIT [/B] [exitCode]\n");
            printf("  Exits the shell or batch script.\n");
            printf("  EXIT [exitCode] - Exits with the specified exit code\n");
            printf("  EXIT /B         - Returns from a subroutine called with CALL\n");
        } else if (strcmp(args[1], "set") == 0) {
            printf("SET [variable[=value]]\n");
            printf("  Sets environment variable or displays all variables if no arguments.\n");
            printf("  SET variable=value - Set environment variable\n");
            printf("  SET /A expression  - Set variable to the result of an arithmetic expression\n");
            printf("  SET /A var+=expr, SET /A var-=expr, etc. - Use assignment operators\n");
            printf("  SET                - Display all environment variables\n");
            printf("  Supported operators in SET /A: +, -, *, /, %%, &, |, ^\n");
            printf("  Examples:\n");
            printf("    SET /A result=5+3\n");
            printf("    SET /A counter+=1\n");
            printf("    SET /A val*=2\n");
            printf("    SET /A bitwise=5&3\n");
            printf("    SET /A flags=2|4\n");
        } else if (strcmp(args[1], "cd") == 0 || strcmp(args[1], "chdir") == 0) {
            printf("CD [directory] | CHDIR [directory]\n");
            printf("  Changes the current directory.\n");
            printf("  CD           - Display current directory\n");
            printf("  CD directory - Change to specified directory\n");
        } else if (strcmp(args[1], "help") == 0) {
            printf("HELP [command]\n");
            printf("  Displays help information for commands.\n");
            printf("  HELP        - Show general help\n");
            printf("  HELP command - Show help for specific command\n");
        } else if (strcmp(args[1], "if") == 0) {
            printf("IF [NOT] condition command [ELSE command]\n");
            printf("IF [NOT] condition AND condition command [ELSE command]\n");
            printf("IF [NOT] condition OR condition command [ELSE command]\n");
            printf("  Specifies a conditional command.\n");
            printf("  condition   - The condition to evaluate\n");
            printf("  command     - Command to execute if condition is true\n");
            printf("  NOT         - Negates the condition\n");
            printf("  AND, OR     - Logical operators to combine conditions\n");
            printf("  ELSE        - Specifies command to execute if condition is false\n");
            printf("  Examples:\n");
            printf("    IF 1 EQU 1 ECHO Success\n");
            printf("    IF 1 EQU 0 ECHO Fail ELSE ECHO Success\n");
            printf("    IF 1 EQU 1 AND 2 GTR 1 ECHO Both true\n");
            printf("    IF 1 EQU 1 OR 1 GTR 2 ECHO At least one true\n");
            printf("    IF NOT 1 EQU 2 ECHO Condition is not true\n");
        } else if (strcmp(args[1], "goto") == 0) {
            printf("GOTO <label>\n");
            printf("  Jumps to a line containing the specified label.\n");
            printf("  Labels are defined with a colon (:) followed by the label name.\n");
            printf("  Examples:\n");
            printf("    GOTO error_handler\n");
            printf("    :error_handler\n");
            printf("    ECHO An error occurred\n");
        } else if (strcmp(args[1], "call") == 0) {
            printf("CALL <label>\n");
            printf("  Calls a subroutine defined by a label, saving the return address.\n");
            printf("  The subroutine can return to the calling point using EXIT /B.\n");
            printf("  Examples:\n");
            printf("    CALL my_function\n");
            printf("    EXIT /B\n");
        } else if (strcmp(args[1], "cls") == 0 || strcmp(args[1], "clear") == 0) {
            printf("CLS | CLEAR\n");
            printf("  Clears the screen.\n");
        } else if (strcmp(args[1], "pause") == 0) {
            printf("PAUSE\n");
            printf("  Displays message \"Press any key to continue . . .\" and waits for user input.\n");
        } else if (strcmp(args[1], "args") == 0 || strcmp(args[1], "arguments") == 0) {
            printf("Command Line Arguments (%%0, %%1, %%2, ...)\n");
            printf("  %%0 - The script name\n");
            printf("  %%1 - First argument passed to the script\n");
            printf("  %%2 - Second argument passed to the script\n");
            printf("  %%n - nth argument passed to the script\n");
            printf("  These variables can be used in batch scripts to access command line parameters.\n");
        } else {
            printf("No help available for command: %s\n", args[1]);
        }
    }
    
    return 0;
}

int handle_setlocal(char** args, int arg_count) {
    // Save current environment variables to the stack
    if (env_stack_level >= 31) {
        fprintf(stderr, "SETLOCAL: Too many nested SETLOCAL calls\n");
        return 1;
    }
    
    // Store current environment variables
    env_stack_size[env_stack_level] = 0;
    
    extern char **environ;
    for (char **env = environ; *env != NULL && env_stack_size[env_stack_level] < 255; env++) {
        char* env_copy = malloc(strlen(*env) + 1);
        if (env_copy) {
            strcpy(env_copy, *env);
            env_stack[env_stack_level][env_stack_size[env_stack_level]] = env_copy;
            env_stack_size[env_stack_level]++;
        }
    }
    
    env_stack_level++;
    return 0;
}

int handle_endlocal(char** args, int arg_count) {
    // Restore environment variables from the stack
    if (env_stack_level <= 0) {
        fprintf(stderr, "ENDLOCAL: No matching SETLOCAL found\n");
        return 1;
    }
    
    env_stack_level--;
    
    // Clear current environment
    // First, get all current environment variable names
    extern char **environ;
    char **current_env = environ;
    while (*current_env != NULL) {
        char *var_name = malloc(strlen(*current_env) + 1);
        if (var_name) {
            strcpy(var_name, *current_env);
            char *eq_pos = strchr(var_name, '=');
            if (eq_pos) {
                *eq_pos = '\0';
                unsetenv(var_name);
            }
            free(var_name);
        }
        current_env++;
    }
    
    // Restore from the saved level
    for (int i = 0; i < env_stack_size[env_stack_level]; i++) {
        if (env_stack[env_stack_level][i]) {
            char *var = malloc(strlen(env_stack[env_stack_level][i]) + 1);
            if (var) {
                strcpy(var, env_stack[env_stack_level][i]);
                char *eq_pos = strchr(var, '=');
                if (eq_pos) {
                    *eq_pos = '\0';
                    setenv(var, eq_pos + 1, 1);
                }
                free(var);
            }
        }
    }
    
    // Free the memory used for this level
    for (int i = 0; i < env_stack_size[env_stack_level]; i++) {
        if (env_stack[env_stack_level][i]) {
            free(env_stack[env_stack_level][i]);
            env_stack[env_stack_level][i] = NULL;
        }
    }
    
    return 0;
}

int handle_if(char** args, int arg_count) {
    if (arg_count < 4) {
        printf("IF: Invalid syntax. Usage: IF [condition] (EQU/NEQ/LSS/GTR/LEQ/GEQ) [value] (AND/OR [condition] (EQU/NEQ/LSS/GTR/LEQ/GEQ) [value]) (ECHO/SET/other command) [ELSE (ECHO/SET/other command)]\n");
        return 1;
    }
    
    // Check for NOT operator at the beginning
    int is_negated = 0;
    int start_idx = 1;
    if (strcasecmp(args[1], "NOT") == 0) {
        is_negated = 1;
        start_idx = 2;
        if (arg_count < 5) {
            printf("IF: Invalid syntax with NOT operator\n");
            return 1;
        }
    }
    
    // Parse and evaluate the conditions
    int final_result = 0;
    int current_idx = start_idx;
    
    // First condition
    char* left = args[current_idx];
    char* op = args[current_idx + 1];
    char* right = args[current_idx + 2];
    
    // Perform the comparison for the first condition
    int result1 = 0;
    long left_val = strtol(left, NULL, 0);
    long right_val = strtol(right, NULL, 0);
    
    if (strcmp(op, "EQU") == 0 || strcmp(op, "==") == 0) {
        result1 = (left_val == right_val);
    } else if (strcmp(op, "NEQ") == 0 || strcmp(op, "!=") == 0) {
        result1 = (left_val != right_val);
    } else if (strcmp(op, "LSS") == 0 || strcmp(op, "<") == 0) {
        result1 = (left_val < right_val);
    } else if (strcmp(op, "GTR") == 0 || strcmp(op, ">") == 0) {
        result1 = (left_val > right_val);
    } else if (strcmp(op, "LEQ") == 0 || strcmp(op, "<=") == 0) {
        result1 = (left_val <= right_val);
    } else if (strcmp(op, "GEQ") == 0 || strcmp(op, ">=") == 0) {
        result1 = (left_val >= right_val);
    } else {
        // String comparison
        if (strcmp(op, "==") == 0) {
            result1 = (strcmp(left, right) == 0);
        } else if (strcmp(op, "!=") == 0) {
            result1 = (strcmp(left, right) != 0);
        } else {
            printf("IF: Unknown operator '%s'. Use EQU, NEQ, LSS, GTR, LEQ, GEQ\n", op);
            return 1;
        }
    }
    
    current_idx += 3;
    
    // Check for AND/OR logical operators
    int has_logical_op = 0;
    int result2 = 0;
    
    if (current_idx < arg_count) {
        if (strcasecmp(args[current_idx], "AND") == 0 || strcasecmp(args[current_idx], "OR") == 0) {
            has_logical_op = 1;
            
            // Second condition after AND/OR
            if (current_idx + 3 < arg_count) {
                char* left2 = args[current_idx + 1];
                char* op2 = args[current_idx + 2];
                char* right2 = args[current_idx + 3];
                
                long left_val2 = strtol(left2, NULL, 0);
                long right_val2 = strtol(right2, NULL, 0);
                
                if (strcmp(op2, "EQU") == 0 || strcmp(op2, "==") == 0) {
                    result2 = (left_val2 == right_val2);
                } else if (strcmp(op2, "NEQ") == 0 || strcmp(op2, "!=") == 0) {
                    result2 = (left_val2 != right_val2);
                } else if (strcmp(op2, "LSS") == 0 || strcmp(op2, "<") == 0) {
                    result2 = (left_val2 < right_val2);
                } else if (strcmp(op2, "GTR") == 0 || strcmp(op2, ">") == 0) {
                    result2 = (left_val2 > right_val2);
                } else if (strcmp(op2, "LEQ") == 0 || strcmp(op2, "<=") == 0) {
                    result2 = (left_val2 <= right_val2);
                } else if (strcmp(op2, "GEQ") == 0 || strcmp(op2, ">=") == 0) {
                    result2 = (left_val2 >= right_val2);
                } else {
                    // String comparison
                    if (strcmp(op2, "==") == 0) {
                        result2 = (strcmp(left2, right2) == 0);
                    } else if (strcmp(op2, "!=") == 0) {
                        result2 = (strcmp(left2, right2) != 0);
                    } else {
                        printf("IF: Unknown operator '%s'. Use EQU, NEQ, LSS, GTR, LEQ, GEQ\n", op2);
                        return 1;
                    }
                }
                
                // Apply the logical operator
                if (strcasecmp(args[current_idx], "AND") == 0) {
                    final_result = result1 && result2;
                } else if (strcasecmp(args[current_idx], "OR") == 0) {
                    final_result = result1 || result2;
                }
                
                current_idx += 4; // Advance past the AND/OR and second condition
            } else {
                printf("IF: Incomplete logical expression after %s\n", args[current_idx]);
                return 1;
            }
        }
    }
    
    // If no logical operator, use the first condition's result directly
    if (!has_logical_op) {
        final_result = result1;
    }
    
    // Apply NOT if specified
    if (is_negated) {
        final_result = !final_result;
    }
    
    // Find the ELSE keyword if it exists
    int else_index = -1;
    for (int i = current_idx; i < arg_count; i++) {
        if (strcasecmp(args[i], "ELSE") == 0) {
            else_index = i;
            break;
        }
    }
    
    if (final_result) {
        // Execute the 'if' command (before ELSE)
        int cmd_start = current_idx;
        int cmd_end = (else_index != -1) ? else_index : arg_count;
        
        if (cmd_start < cmd_end) {
            char* command = args[cmd_start];
            
            if (strcmp(command, "ECHO") == 0) {
                // Execute echo command with remaining arguments before ELSE
                for (int i = cmd_start + 1; i < cmd_end; i++) {
                    printf("%s", args[i]);
                    if (i < cmd_end - 1) printf(" ");
                }
                printf("\n");
            } else if (strcmp(command, "SET") == 0) {
                // Execute SET command if there are enough arguments
                // Create a new argument list starting from SET
                char* set_args[64];
                int new_arg_count = 0;
                for (int i = cmd_start; i < cmd_end && new_arg_count < 64; i++) {
                    set_args[new_arg_count++] = args[i];
                }
                handle_set(set_args, new_arg_count);
            } else {
                // Execute as external command
                char* cmd_args[64];
                int new_arg_count = 0;
                for (int i = cmd_start; i < cmd_end && new_arg_count < 64; i++) {
                    cmd_args[new_arg_count++] = args[i];
                }
                execute_external_command(cmd_args, new_arg_count);
            }
        }
    } else if (else_index != -1) {
        // Execute the 'else' command (after ELSE)
        if (else_index + 1 < arg_count) {
            char* command = args[else_index + 1];
            
            if (strcmp(command, "ECHO") == 0) {
                // Execute echo command with remaining arguments after ELSE
                for (int i = else_index + 2; i < arg_count; i++) {
                    printf("%s", args[i]);
                    if (i < arg_count - 1) printf(" ");
                }
                printf("\n");
            } else if (strcmp(command, "SET") == 0) {
                // Execute SET command if there are enough arguments
                // Create a new argument list starting after ELSE
                char* set_args[64];
                int new_arg_count = 0;
                for (int i = else_index + 1; i < arg_count && new_arg_count < 64; i++) {
                    set_args[new_arg_count++] = args[i];
                }
                handle_set(set_args, new_arg_count);
            } else {
                // Execute as external command
                char* cmd_args[64];
                int new_arg_count = 0;
                for (int i = else_index + 1; i < arg_count && new_arg_count < 64; i++) {
                    cmd_args[new_arg_count++] = args[i];
                }
                execute_external_command(cmd_args, new_arg_count);
            }
        }
    }
    
    return 0;
}

// Function to handle FOR loops - DOS-style iteration
int handle_for_loop(char** args, int arg_count) {
    if (arg_count < 6) {
        printf("FOR: Invalid syntax. Usage: FOR %%<variable> IN (list) DO command\n");
        return 1;
    }
    
    // Parse FOR syntax: FOR %%variable IN (item1,item2,item3,...) DO command
    char* variable = args[1];  // This should be %%X format
    if (strlen(variable) < 3 || variable[0] != '%' || variable[1] != '%') {
        printf("FOR: Invalid variable format. Use %%X where X is the variable name.\n");
        return 1;
    }
    
    char var_name = variable[2];  // The actual variable character
    if (strlen(variable) > 3) {
        printf("FOR: Variable name should be a single character after %%. Use %%X format.\n");
        return 1;
    }
    
    // Check for "IN" keyword
    if (strcasecmp(args[2], "IN") != 0) {
        printf("FOR: Expected 'IN' keyword. Usage: FOR %%<variable> IN (list) DO command\n");
        return 1;
    }
    
    // Extract the list in parentheses (args[3])
    char* list_str = args[3];
    if (strlen(list_str) < 3 || list_str[0] != '(' || list_str[strlen(list_str)-1] != ')') {
        printf("FOR: List must be enclosed in parentheses. Usage: FOR %%<variable> IN (item1,item2,item3) DO command\n");
        return 1;
    }
    
    // Remove the parentheses
    char list_cleaned[MAX_COMMAND_LENGTH];
    strncpy(list_cleaned, list_str + 1, strlen(list_str) - 2);  // Skip first '(' and last ')'
    list_cleaned[strlen(list_str) - 2] = '\0';
    
    // Check for "DO" keyword
    if (strcasecmp(args[4], "DO") != 0) {
        printf("FOR: Expected 'DO' keyword. Usage: FOR %%<variable> IN (list) DO command\n");
        return 1;
    }
    
    // Prepare the command template by combining remaining arguments
    char command_template[MAX_COMMAND_LENGTH];
    command_template[0] = '\0';
    for (int i = 5; i < arg_count; i++) {
        strcat(command_template, args[i]);
        if (i < arg_count - 1) strcat(command_template, " ");
    }
    
    // Tokenize the list by commas to get individual items
    char* token = strtok(list_cleaned, ",");
    while (token != NULL) {
        // Skip leading spaces in the token
        while (*token == ' ') token++;
        
        // Replace the variable in the command template with the current token value
        char final_command[MAX_COMMAND_LENGTH];
        strcpy(final_command, command_template);
        
        // Replace %variable% with the actual token value
        char var_pattern[4];
        snprintf(var_pattern, sizeof(var_pattern), "%%%c", var_name);
        
        // Simple replacement - find and replace the variable pattern with the token
        char temp[MAX_COMMAND_LENGTH];
        char* pos = strstr(final_command, var_pattern);
        if (pos != NULL) {
            // Create new string with substitution
            *pos = '\0';  // Terminate at position of pattern
            strcpy(temp, final_command);  // Copy part before pattern
            strcat(temp, token);          // Add the token value
            strcat(temp, pos + 3);        // Add part after pattern (skip '%%X')
            strcpy(final_command, temp);  // Copy back to final command
        }
        
        // Execute the command with the substituted variable
        execute_command(final_command);
        
        // Get next token
        token = strtok(NULL, ",");
    }
    
    return 0;
}

// Function to handle mkdir command (create directory)
int handle_mkdir(char** args, int arg_count) {
    if (arg_count != 2) {
        printf("Usage: MD directory_name or MKDIR directory_name\n");
        return 1;
    }
    
    // Create directory with default permissions
    if (mkdir(args[1], 0755) == -1) {
        perror("MD/MKDIR");
        return 1;
    }
    
    return 0;
}

// Function to handle rmdir command (remove directory)
int handle_rmdir(char** args, int arg_count) {
    if (arg_count != 2) {
        printf("Usage: RD directory_name or RMDIR directory_name\n");
        return 1;
    }
    
    if (rmdir(args[1]) == -1) {
        perror("RD/RMDIR");
        return 1;
    }
    
    return 0;
}

// Function to handle del command (delete files)
int handle_del(char** args, int arg_count) {
    if (arg_count < 2) {
        printf("Usage: DEL file1 [file2...] or ERASE file1 [file2...]\n");
        return 1;
    }
    
    int result = 0;
    for (int i = 1; i < arg_count; i++) {
        if (unlink(args[i]) == -1) {
            perror("DEL/ERASE");
            result = 1;  // Set error result but continue with other files
        }
    }
    
    return result;
}

// Function to handle copy command
int handle_copy(char** args, int arg_count) {
    if (arg_count != 3) {
        printf("Usage: COPY source destination\n");
        return 1;
    }
    
    // Use system copy command for now
    char command[MAX_COMMAND_LENGTH];
    snprintf(command, sizeof(command), "cp \"%s\" \"%s\"", args[1], args[2]);
    
    int result = system(command);
    if (result == -1) {
        perror("COPY");
        return 1;
    }
    
    return WEXITSTATUS(result);
}

// Function to handle move command
int handle_move(char** args, int arg_count) {
    if (arg_count != 3) {
        printf("Usage: MOVE source destination\n");
        return 1;
    }
    
    // Use system move command for now
    char command[MAX_COMMAND_LENGTH];
    snprintf(command, sizeof(command), "mv \"%s\" \"%s\"", args[1], args[2]);
    
    int result = system(command);
    if (result == -1) {
        perror("MOVE");
        return 1;
    }
    
    return WEXITSTATUS(result);
}

// Function to handle type command (display file contents)
int handle_type(char** args, int arg_count) {
    if (arg_count != 2) {
        printf("Usage: TYPE filename or CAT filename\n");
        return 1;
    }
    
    FILE* fp = fopen(args[1], "r");
    if (fp == NULL) {
        perror("TYPE/CAT");
        return 1;
    }
    
    char line[1024];
    while (fgets(line, sizeof(line), fp) != NULL) {
        printf("%s", line);
    }
    
    fclose(fp);
    return 0;
}

// Function to handle dir command (list directory contents)
int handle_dir(char** args, int arg_count) {
    // Default to listing current directory if no arguments provided
    char* directory = ".";
    if (arg_count > 1) {
        directory = args[1];
    }
    
    // Use system ls command for now, formatted like Windows dir
    char command[MAX_COMMAND_LENGTH];
    snprintf(command, sizeof(command), "ls -la \"%s\"", directory);
    
    int result = system(command);
    if (result == -1) {
        perror("DIR");
        return 1;
    }
    
    return WEXITSTATUS(result);
}

// Function to handle date command
int handle_date(char** args, int arg_count) {
    time_t now;
    struct tm *timeinfo;
    
    time(&now);
    timeinfo = localtime(&now);
    
    // Format to look like DOS date output
    printf("Current date: %02d/%02d/%04d\n", 
           timeinfo->tm_mon + 1, 
           timeinfo->tm_mday, 
           timeinfo->tm_year + 1900);
    
    // If no arguments, just show the date, but in DOS you would be prompted to change it
    if (arg_count == 1) {
        printf("Enter new date (mm-dd-yy) or press Enter for current date: ");
        fflush(stdout);
        // In a real implementation, we would read the input and parse it
        // For now, just showing the current date
    }
    
    return 0;
}

// Function to handle time command
int handle_time(char** args, int arg_count) {
    time_t now;
    struct tm *timeinfo;
    
    time(&now);
    timeinfo = localtime(&now);
    
    // Format to look like DOS time output
    printf("Current time: %02d:%02d:%02d.%02d\n", 
           timeinfo->tm_hour, 
           timeinfo->tm_min, 
           timeinfo->tm_sec,
           0); // DOS shows hundredths of seconds, but we'll just show 0
    
    // If no arguments, just show the time, but in DOS you would be prompted to change it
    if (arg_count == 1) {
        printf("Enter new time (hh:mm:ss) or press Enter for current time: ");
        fflush(stdout);
        // In a real implementation, we would read the input and parse it
        // For now, just showing the current time
    }
    
    return 0;
}

// Function to handle more command (paginate output)
int handle_more(char** args, int arg_count) {
    if (arg_count == 1) {
        // If no arguments, read from stdin (like more command does)
        char line[1024];
        int line_count = 0;
        
        while (fgets(line, sizeof(line), stdin) != NULL) {
            printf("%s", line);
            line_count++;
            
            // Pause after every 24 lines (standard DOS more behavior)
            if (line_count % 24 == 0) {
                printf("-- More -- Press any key to continue ...");
                fflush(stdout);
                
                char c;
                read(0, &c, 1);
                printf("\n");
                
                // If user presses 'q' or 'Q', quit
                if (c == 'q' || c == 'Q') {
                    break;
                }
            }
        }
    } else {
        // Process each file specified as an argument
        for (int i = 1; i < arg_count; i++) {
            FILE* fp = fopen(args[i], "r");
            if (fp == NULL) {
                perror("MORE");
                continue;
            }
            
            char line[1024];
            int line_count = 0;
            
            while (fgets(line, sizeof(line), fp) != NULL) {
                printf("%s", line);
                line_count++;
                
                // Pause after every 24 lines
                if (line_count % 24 == 0) {
                    printf("-- More -- Press any key to continue ...");
                    fflush(stdout);
                    
                    char c;
                    read(0, &c, 1);
                    printf("\n");
                    
                    // If user presses 'q' or 'Q', quit
                    if (c == 'q' || c == 'Q') {
                        fclose(fp);
                        return 0;
                    }
                }
            }
            
            fclose(fp);
        }
    }
    
    return 0;
}

// Function to handle sort command (sort input lines)
int handle_sort(char** args, int arg_count) {
    if (arg_count == 1) {
        // Sort stdin input
        char lines[1000][1024];  // Buffer to store lines
        int line_count = 0;
        
        while (line_count < 1000) {
            if (fgets(lines[line_count], sizeof(lines[line_count]), stdin) == NULL) {
                break;
            }
            line_count++;
        }
        
        // Simple bubble sort
        for (int i = 0; i < line_count - 1; i++) {
            for (int j = 0; j < line_count - i - 1; j++) {
                if (strcmp(lines[j], lines[j + 1]) > 0) {
                    // Swap lines
                    char temp[1024];
                    strcpy(temp, lines[j]);
                    strcpy(lines[j], lines[j + 1]);
                    strcpy(lines[j + 1], temp);
                }
            }
        }
        
        // Print sorted lines
        for (int i = 0; i < line_count; i++) {
            printf("%s", lines[i]);
        }
    } else {
        // Sort content of specified files
        for (int i = 1; i < arg_count; i++) {
            FILE* fp = fopen(args[i], "r");
            if (fp == NULL) {
                perror("SORT");
                continue;
            }
            
            char lines[1000][1024];  // Buffer to store lines
            int line_count = 0;
            
            while (line_count < 1000 && 
                   fgets(lines[line_count], sizeof(lines[line_count]), fp) != NULL) {
                line_count++;
            }
            
            fclose(fp);
            
            // Simple bubble sort
            for (int j = 0; j < line_count - 1; j++) {
                for (int k = 0; k < line_count - j - 1; k++) {
                    if (strcmp(lines[k], lines[k + 1]) > 0) {
                        // Swap lines
                        char temp[1024];
                        strcpy(temp, lines[k]);
                        strcpy(lines[k], lines[k + 1]);
                        strcpy(lines[k + 1], temp);
                    }
                }
            }
            
            // Print sorted lines
            for (int j = 0; j < line_count; j++) {
                printf("%s", lines[j]);
            }
        }
    }
    
    return 0;
}

// Function to handle find command (search for text in files)
int handle_find(char** args, int arg_count) {
    if (arg_count < 2) {
        printf("Usage: FIND \"text_to_find\" [file1] [file2] ...\n");
        return 1;
    }
    
    char* search_text = args[1];
    int files_specified = (arg_count > 2);
    
    if (!files_specified) {
        // Search in stdin
        char line[1024];
        int line_number = 1;
        int matches_found = 0;
        
        while (fgets(line, sizeof(line), stdin) != NULL) {
            if (strstr(line, search_text) != NULL) {
                printf("%s", line);
                matches_found++;
            }
            line_number++;
        }
        
        if (matches_found == 0) {
            printf("----------\n");
        }
    } else {
        // Search in specified files
        for (int i = 2; i < arg_count; i++) {
            FILE* fp = fopen(args[i], "r");
            if (fp == NULL) {
                perror("FIND");
                continue;
            }
            
            char line[1024];
            int line_number = 1;
            int matches_found = 0;
            
            printf("---------- %s\n", args[i]);
            
            while (fgets(line, sizeof(line), fp) != NULL) {
                if (strstr(line, search_text) != NULL) {
                    printf("%s", line);
                    matches_found++;
                }
                line_number++;
            }
            
            if (matches_found == 0) {
                printf("----------\n");
            }
            
            fclose(fp);
        }
    }
    
    return 0;
}

// Function to handle choice command (user input options)
int handle_choice(char** args, int arg_count) {
    if (arg_count < 2) {
        printf("Usage: CHOICE [/C[:]choices] [/N] [/S] text\n");
        printf("  /C[:]    Specifies allowable keys. Default is: yn.\n");
        printf("  /N       Do not display the choices at end of prompt.\n");
        printf("  /S       Enables case-sensitive choices to be selected.\n");
        printf("Example: CHOICE /C:YN /S \"Continue?\"\n");
        return 1;
    }
    
    // Default choices
    char choices[] = "YN";
    int display_choices = 1;
    int case_sensitive = 0;
    char* prompt = NULL;
    
    // Parse command line arguments
    for (int i = 1; i < arg_count; i++) {
        if (args[i][0] == '/' || args[i][0] == '-') {
            if (strlen(args[i]) >= 2) {
                if (args[i][1] == 'C' || args[i][1] == 'c') {
                    // Extract choices from /C: or /C format
                    if (args[i][2] == ':') {
                        strcpy(choices, args[i] + 3);
                    } else if (i + 1 < arg_count) {
                        strcpy(choices, args[i + 1]);
                        i++;  // Skip next argument since it was used as choices
                    }
                } else if (args[i][1] == 'N' || args[i][1] == 'n') {
                    display_choices = 0;
                } else if (args[i][1] == 'S' || args[i][1] == 's') {
                    case_sensitive = 1;
                }
            }
        } else {
            // This is the prompt text
            prompt = args[i];
        }
    }
    
    if (prompt == NULL) {
        prompt = "Continue?";
    }
    
    // Display the prompt
    printf("%s", prompt);
    
    // Display choices if needed
    if (display_choices) {
        printf(" [");
        for (int i = 0; i < strlen(choices); i++) {
            if (i > 0) printf(", ");
            printf("%c", choices[i]);
        }
        printf("]? ");
    } else {
        printf(" ");
    }
    fflush(stdout);
    
    // Get user input
    char input[10];
    if (fgets(input, sizeof(input), stdin) != NULL) {
        // Get the first character from the input
        char user_choice = input[0];
        
        // Convert to uppercase if not case sensitive
        if (!case_sensitive && user_choice >= 'a' && user_choice <= 'z') {
            user_choice = user_choice - 'a' + 'A';
        }
        
        // Check if user choice is valid
        int valid = 0;
        for (int i = 0; i < strlen(choices); i++) {
            char choice = choices[i];
            if (!case_sensitive && choice >= 'a' && choice <= 'z') {
                choice = choice - 'a' + 'A';
            }
            
            if (user_choice == choice) {
                valid = 1;
                break;
            }
        }
        
        if (valid) {
            printf("\nYou selected: %c\n", user_choice);
            // Set ERRORLEVEL to the position of the selected choice (1-indexed)
            // For now, we'll just return success
            return 0;
        } else {
            printf("\nInvalid choice. Valid choices are: %s\n", choices);
            return 1;
        }
    }
    
    return 1;
}