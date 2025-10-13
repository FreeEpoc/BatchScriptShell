#include "shell.h"

/* 
 * BSS (Batch Script Shell) - A shell program that interprets batch commands
 * 
 * This shell is designed to interpret basic batch commands used in Symbian build scripts,
 * with support for Windows-style paths, environment variables, and basic command execution.
 */

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
            printf("Batch Script Shell (BSS) v1.0\n");
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
            
            printf("Batch Script Shell (BSS) v1.0\n");
            while (fgets(input, sizeof(input), batch_file)) {
                // Remove newline character
                input[strcspn(input, "\n")] = 0;
                
                if (strlen(input) > 0) {
                    execute_command(input);
                }
            }
            
            fclose(batch_file);
            return 0;
        }
    }
    
    // Interactive mode
    printf("Batch Script Shell (BSS) v1.0\n");
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
    // Get current working directory
    if (getcwd(shell_state.current_dir, sizeof(shell_state.current_dir)) == NULL) {
        perror("getcwd");
        return 1;
    }
    
    // Set default root mapping (Linux root "/" binds to "C:\\")
    strcpy(shell_state.root_path, "/");
    
    // Initialize environment variables
    setenv("OS", "Linux", 1);  // Default OS environment variable
    
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
    } else {
        // Not a built-in command, try external command
        return execute_external_command(args, arg_count);
    }
}

int execute_external_command(char** args, int arg_count) {
    // Fork and execute external command
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("fork");
        return 1;
    } else if (pid == 0) {
        // Child process - execute command
        execvp(args[0], args);
        perror("Command execution failed");
        exit(1);
    } else {
        // Parent process - wait for child
        int status;
        waitpid(pid, &status, 0);
        return WEXITSTATUS(status);
    }
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
        // Set environment variable
        // Format: SET var=value
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
    
    // Simple pause implementation - wait for input
    char c;
    read(0, &c, 1);
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
        } else if (strcmp(args[1], "set") == 0) {
            printf("SET [variable[=value]]\n");
            printf("  Sets environment variable or displays all variables if no arguments.\n");
            printf("  SET variable=value - Set environment variable\n");
            printf("  SET                - Display all environment variables\n");
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
        printf("IF: Invalid syntax. Usage: IF [condition] (EQU/NEQ/LSS/GTR/LEQ/GEQ) [value] (ECHO/SET/other command)\n");
        return 1;
    }
    
    // Simple IF implementation - handles basic comparisons
    char* left = args[1];
    char* op = args[2];
    char* right = args[3];
    
    // Check if the fourth argument is "ECHO" or other command to execute if condition is true
    if (arg_count < 5) {
        printf("IF: No command provided after condition\n");
        return 1;
    }
    
    char* command = args[4];  // Command after "EQU", "NEQ", etc.
    
    // Perform the comparison
    int result = 0;
    long left_val = strtol(left, NULL, 0);
    long right_val = strtol(right, NULL, 0);
    
    if (strcmp(op, "EQU") == 0 || strcmp(op, "==") == 0) {
        result = (left_val == right_val);
    } else if (strcmp(op, "NEQ") == 0 || strcmp(op, "!=") == 0) {
        result = (left_val != right_val);
    } else if (strcmp(op, "LSS") == 0 || strcmp(op, "<") == 0) {
        result = (left_val < right_val);
    } else if (strcmp(op, "GTR") == 0 || strcmp(op, ">") == 0) {
        result = (left_val > right_val);
    } else if (strcmp(op, "LEQ") == 0 || strcmp(op, "<=") == 0) {
        result = (left_val <= right_val);
    } else if (strcmp(op, "GEQ") == 0 || strcmp(op, ">=") == 0) {
        result = (left_val >= right_val);
    } else {
        // String comparison
        if (strcmp(op, "==") == 0) {
            result = (strcmp(left, right) == 0);
        } else if (strcmp(op, "!=") == 0) {
            result = (strcmp(left, right) != 0);
        } else {
            printf("IF: Unknown operator '%s'. Use EQU, NEQ, LSS, GTR, LEQ, GEQ\n", op);
            return 1;
        }
    }
    
    if (result) {
        // Execute the command (simplified - just handle basic ECHO)
        if (strcmp(command, "ECHO") == 0) {
            // Execute echo command with remaining arguments
            if (arg_count > 5) {
                for (int i = 5; i < arg_count; i++) {
                    printf("%s", args[i]);
                    if (i < arg_count - 1) printf(" ");
                }
                printf("\n");
            }
        } else if (strcmp(command, "SET") == 0) {
            // Execute SET command if there are enough arguments
            if (arg_count > 5) {
                // Create a new argument list starting from SET
                char* set_args[64];
                int new_arg_count = 0;
                for (int i = 4; i < arg_count && new_arg_count < 64; i++) {
                    set_args[new_arg_count++] = args[i];
                }
                handle_set(set_args, new_arg_count);
            }
        } else {
            // Execute as external command
            char* cmd_args[64];
            int new_arg_count = 0;
            for (int i = 4; i < arg_count && new_arg_count < 64; i++) {
                cmd_args[new_arg_count++] = args[i];
            }
            execute_external_command(cmd_args, new_arg_count);
        }
    }
    
    return 0;
}