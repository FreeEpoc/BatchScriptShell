# Batch Script - ATTRIB

---



---

Displays or sets the attributes of the files in the current directory

## Syntax

```
attrib
```

## Example

The following example shows the different variants of the attrib command.

```
@echo off
Rem Displays the attribites of the file in the current directory
Attrib

Rem Displays the attributes of the file lists.txt
attrib C:\tp\lists.txt

Rem Adds the "Read-only" attribute to the file.
attrib &plus;r C:\tp\lists.txt
Attrib C:\tp\lists.txt

Rem Removes the "Archived" attribute from the file
attrib -a C:\tp\lists.txt
Attrib C:\tp\lists.txt
```

## Output

For example,

```
A           C:\tp\assoclst.txt
A           C:\tp\List.cmd
A           C:\tp\lists.txt
A           C:\tp\listsA.txt
A           C:\tp\lists.txt
A    R      C:\tp\lists.txt
     R      C:\tp\lists.txt
```

batch\_script\_commands.htm

