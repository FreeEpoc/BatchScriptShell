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
            char* mode = (strcmp(args[i], ">") == 0) ? "w" : 
                        (strcmp(args[i], ">>") == 0) ? "a" : "r";
            
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
                return -1; // Not a supported redirection case
            }
        }
    }
    
    // No redirection operators found
    return -1; // Indicate no redirection to handle
}