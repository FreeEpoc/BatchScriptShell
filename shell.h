#ifndef SHELL_H
#define SHELL_H

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>

#define MAX_COMMAND_LENGTH 1024
#define MAX_ARGS 64
#define MAX_PATH_LENGTH 4096
#define MAX_VAR_NAME_LENGTH 256

// Directory mounting structure - for mapping directories like DOS
typedef struct {
    char mount_point[MAX_PATH_LENGTH];
    char target_path[MAX_PATH_LENGTH];
} MountEntry;

typedef struct {
    MountEntry mounts[32];  // Maximum 32 mount points
    int mount_count;
    char current_dir[MAX_PATH_LENGTH];
    char root_path[MAX_PATH_LENGTH];
} ShellState;

// Global shell state
extern ShellState shell_state;
extern int echo_enabled;
extern int is_batch_mode;

// Global command line arguments
extern char* command_line_args[64];
extern int num_command_line_args;

// Environment variable stack for SETLOCAL/ENDLOCAL
extern char* env_stack[32][256];
extern int env_stack_size[32];
extern int env_stack_level;

// Structure to store label information
typedef struct {
    char name[256];
    int line_number;
} Label;

// Structure to store call stack information for CALL/RETURN
typedef struct {
    int return_line;
} CallStackEntry;

// Global variables for GOTO functionality
extern Label labels[1000];  // Support up to 1000 labels
extern int num_labels;

// Global variables for CALL functionality
extern CallStackEntry call_stack[100];  // Support up to 100 nested calls
extern int call_stack_size;

// Additional function prototypes
int parse_batch_labels(FILE* batch_file, Label* label_list, int* num_labels_ptr);
int is_label_line(char* line);
void extract_label_name(char* line, char* label_name);

// Function prototypes
int initialize_shell();
int execute_command(char* command);
int execute_builtin_command(char** args, int arg_count);
int execute_external_command(char** args, int arg_count);
int execute_pipe_command(char** args, int arg_count, int pipe_pos);
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
int handle_for_loop(char** args, int arg_count);
int handle_mkdir(char** args, int arg_count);
int handle_rmdir(char** args, int arg_count);
int handle_del(char** args, int arg_count);
int handle_copy(char** args, int arg_count);
int handle_move(char** args, int arg_count);
int handle_type(char** args, int arg_count);
int handle_dir(char** args, int arg_count);
int handle_date(char** args, int arg_count);
int handle_time(char** args, int arg_count);
int handle_more(char** args, int arg_count);
int handle_sort(char** args, int arg_count);
int handle_find(char** args, int arg_count);
int handle_choice(char** args, int arg_count);

#endif // SHELL_H