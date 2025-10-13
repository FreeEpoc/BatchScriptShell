# Batch Script - CHOICE

---



---

This batch command provides a list of options to the user.

## Syntax

```
CHOICE /c [Options] /m [Message]
```

Where Options is the list of options to provide to the user and Message is the string message which needs to be displayed.

## Example

```
@echo off 
echo "What is the file size you what" 
echo "A:10MB" 
echo "B:20MB" 
echo "C:30MB" 
choice /c ABC /m "What is your option A , B or C"
```

## Output

The above program produces the following output.

```
"What is the file size you what"
"A:10MB"
"B:20MB"
"C:30MB"
What is your option A , B or C [A,B,C]?
```

batch\_script\_commands.htm

