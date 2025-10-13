# Batch Script - MD

---



---

This batch command creates a new directory in the current location.

## Syntax

```
md [new directory name]
```

## Example

```
@echo off 
md newdir 
cd newdir 
cd Rem Goes back to the parent directory and create 2 directories 
cd.. 
md newdir1 newdir1 
cd newdir1 
cd 
cd.. 
cd newdir2 
cd
```

## Output

The above command produces the following output.

```
C:\newdir 
C:\newdir1 
C:\newdir2
```

batch\_script\_commands.htm

