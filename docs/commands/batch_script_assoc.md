# Batch Script - ASSOC

---



---

This is a batch command that associates an extension with a file type (FTYPE), displays existing associations, or deletes an association.

## Syntax

```
assoc  Displays all the file extensions 
assoc | find .ext  Displays only those file extensions which have the extension ext.
```

## Example

```
@echo off 
assoc > C:\lists.txt 
assoc | find .doc > C:\listsdoc.txt
```

## Output

The list of file associations will be routed to the file lists.txt. The following output shows what is there in the listsdoc.txt file after the above batch file is run.

```
.doc=Word.Document.8 
.dochtml=wordhtmlfile 
.docm=Word.DocumentMacroEnabled.12 
.docmhtml=wordmhtmlfile 
.docx=Word.Document.12 
.docxml=wordxmlfile
```

batch\_script\_commands.htm

