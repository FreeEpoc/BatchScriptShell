# Batch Script - MORE

---



---

This batch command displays the contents of a file or files, one screen at a time.

## Syntax

```
More [filename]
```

Where filename is the file whose contents need to be listed one screen at a time.

## Example

```
@echo off
More C:\tp\lists.txt
Directory of C:\Program Files
```

## Output

The above command will display the contents of the file lists.txt one screen at a time. Following is an example of an output. Note the -- More (12%) at the end of the screen. In order to proceed and display the remaining contents of the file, you need to enter a key.

```
12/22/2015 02:31 AM   <DIR>       .
12/22/2015 02:31 AM   <DIR>       ..
12/15/2015 11:14 PM   <DIR>       Application Verifier
12/18/2015 05:06 PM   <DIR>       Bonjour
12/26/2015 08:01 PM   <DIR>       CCleaner
12/18/2015 05:05 PM   <DIR>       Common Files
12/17/2015 11:04 AM   <DIR>       Git
12/15/2015 11:09 PM   <DIR>       IIS
12/15/2015 11:10 PM   <DIR>       IIS Express
12/15/2015 10:16 PM   <DIR>       Intel
03/18/2014 02:24 AM   <DIR>       Internet Explorer
12/18/2015 05:06 PM   <DIR>       iPod
12/18/2015 05:06 PM   <DIR>       iTunes
12/15/2015 11:16 PM   <DIR>       Microsoft Identity Extensions
12/15/2015 11:46 PM   <DIR>       Microsoft Office
12/22/2015 02:31 AM   <DIR>       Microsoft Silverlight
12/15/2015 11:15 PM   <DIR>       Microsoft SQL Server
12/15/2015 11:15 PM   <DIR>       Microsoft SQL Server Compact Edition
12/15/2015 10:59 PM   <DIR>       Microsoft Visual Studio 12.0
-- More (12%) --
```

batch\_script\_commands.htm

