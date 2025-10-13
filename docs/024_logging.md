# Batch Script - Logging

---



---

Logging in is possible in Batch Script by using the redirection command.

## Syntax

```
test.bat > testlog.txt 2> testerrors.txt
```

## Example

Create a file called test.bat and enter the following command in the file.

```
net statistics /Server
```

The above command has an error because the option to the net statistics command is given in the wrong way.

## Output

If the command with the above test.bat file is run as

```
test.bat > testlog.txt 2> testerrors.txt
```

And you open the file testerrors.txt, you will see the following error.

```
The option /SERVER is unknown.
```

The syntax of this command is −

```
NET STATISTICS
[WORKSTATION | SERVER]
```

More help is available by typing NET HELPMSG 3506.

If you open the file called testlog.txt, it will show you a log of what commands were executed.

```
C:\tp>net statistics /Server
```


