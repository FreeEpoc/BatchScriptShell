# Batch Script - Functions

---



---

A function is a set of statements organized together to perform a specific task. In batch scripts, a similar approach is adopted to group logical statements together to form a function.

As like any other languages, functions in Batch Script follows the same procedure −

* **Function Declaration** − It tells the compiler about a function's name, return type, and parameters.
* **Function Definition** − It provides the actual body of the function.

## Function Definition

In Batch Script, a function is defined by using the label statement. When a function is newly defined, it may take one or several values as input 'parameters' to the function, process the functions in the main body, and pass back the values to the functions as output 'return types'.

Every function has a function name, which describes the task that the function performs. To use a function, you "call" that function with its name and pass its input values (known as arguments) that matches the types of the function's parameters.

Following is the syntax of a simple function.

```
:function_name 
Do_something 
EXIT /B 0
```

* The function\_name is the name given to the function which should have some meaning to match what the function actually does.
* The EXIT statement is used to ensure that the function exits properly.

Following is an example of a simple function.

### Example

```
:Display 
SET /A index=2 
echo The value of index is %index% 
EXIT /B 0
```

| S.No | Functions & Description |
| --- | --- |
| 1 | [Calling a Function](batch_script_calling_function.md) A function is called in Batch Script by using the call command. |
| 2 | [Functions with Parameters](batch_script_functions_with_parameters.md) Functions can work with parameters by simply passing them when a call is made to the function. |
| 3 | [Functions with Return Values](batch_script_functions_with_return_values.md) Functions can work with return values by simply passing variables names |
| 4 | [Local Variables in Functions](batch_script_local_variables_functions.md) Local variables in functions can be used to avoid name conflicts and keep variable changes local to the function. |
| 5 | [Recursive Functions](batch_script_recursive_functions.md) The ability to completely encapsulate the body of a function by keeping variable changes local to the function and invisible to the caller. |
| 6 | [File I/O](batch_script_file_io.md) In Batch Script, it is possible to perform the normal file I/O operations that would be expected in any programming language. |
| 7 | [Creating Files](batch_script_creating_files.md) The creation of a new file is done with the help of the redirection filter >. This filter can be used to redirect any output to a file. |
| 8 | [Writing to Files](batch_script_writing_files.md) Content writing to files is also done with the help of the redirection filter >. This filter can be used to redirect any output to a file. |
| 9 | [Appending to Files](batch_script_appending_files.md) Content writing to files is also done with the help of the double redirection filter >>. This filter can be used to append any output to a file. |
| 10 | [Reading from Files](batch_script_reading_from_files.md) Reading of files in a batch script is done via using the FOR loop command to go through each line which is defined in the file that needs to be read. |
| 11 | [Deleting Files](batch_script_deleting_files.md) For deleting files, Batch Script provides the DEL command. |
| 12 | [Renaming Files](batch_script_renaming_files.md) For renaming files, Batch Script provides the REN or RENAME command. |
| 13 | [Moving Files](batch_script_moving_files.md) For moving files, Batch Script provides the MOVE command. |
| 14 | [Batch Files Pipes](batch_script_files_pipes.md) The pipe operator (|) takes the output (by default, STDOUT) of one command and directs it into the input (by default, STDIN) of another command. |
| 15 | [Batch Files Inputs](batch_script_files_inputs.md) When a batch file is run, it gives you the option to pass in command line parameters which can then be read within the program for further processing. |
| 16 | [Using the SHIFT Operator](batch_script_using_shift_operator.md) One of the limitations of command line arguments is that it can accept only arguments till %9. Lets take an example of this limitation. |
| 17 | [Folders](batch_script_folders.md) In Batch Script, it is possible to perform the normal folder based operations that would be expected in any programming language. |
| 18 | [Creating Folders](batch_script_creating_folders.md) The creation of a folder is done with the assistance of the MD (Make directory) command. |
| 19 | [Listing Folder Contents](batch_script_listing_folder_contents.md) The listing of folder contents can be done with the dir command. This command allows you to see the available files and directories in the current directory. |
| 20 | [Deleting Folders](batch_script_deleting_folders.md) For deleting folders, Batch Scripting provides the DEL command. |
| 21 | [Renaming Folders](batch_script_renaming_folders.md) For renaming folders, Batch Script provides the REN or RENAME command. |
| 22 | [Moving Folders](batch_script_moving_folders.md) For moving folders, Batch Script provides the MOVE command. |

