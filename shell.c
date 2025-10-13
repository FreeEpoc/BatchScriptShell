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

// Function prototypes
int initialize_shell();
int execute_command(char* command);
int execute_builtin_command(char** args, int arg_count);
int execute_external_command(char** args, int arg_count);
char* expand_variables(char* command);
char* convert_path(char* path);
int mount_directory(char* mount_point, char* target_path);
char* get_mounted_path(char* path);
int change_directory(char* path);
int print_working_directory();
int handle_echo(char** args, int arg_count);
int handle_set(char** args, int arg_count);
int handle_cd(char** args, int arg_count);
int handle_cls(char** args, int arg_count);
int handle_pause(char** args, int arg_count);

int main(int argc, char **argv) {
    char input[MAX_COMMAND_LENGTH];
    
    // Initialize shell once
    if (initialize_shell() != 0) {
        fprintf(stderr, "Failed to initialize shell\n");
        return 1;
    }
    
    // Process command line argument if provided (batch file execution)
    if (argc > 1) {
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
    
    // Interactive mode
    printf("Batch Script Shell (BSS) v1.0\n");
    while (1) {
        printf("BSS> ");
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
    if (strcmp(args[0], "echo") == 0) {
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
    
    // Find and replace %VAR% style environment variables
    char* start = result;
    while ((start = strchr(start, '%')) != NULL) {
        char* end = strchr(start + 1, '%');
        if (end == NULL) break; // No closing %
        
        *end = '\0';
        char var_name[256];
        strcpy(var_name, start + 1); // Skip the opening %
        
        // Get the environment variable value
        char* value = getenv(var_name);
        if (value != NULL) {
            // Replace the %VAR% with its value
            int before_len = start - result;
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
            start = temp + before_len + value_len;
        } else {
            // Variable not found, just move past it
            *end = '%';  // Restore the %
            start = end + 1;
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