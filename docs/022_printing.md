# Batch Script - Printing

---



---

Printing can also be controlled from within Batch Script via the NET PRINT command.

### Syntax

```
PRINT [/D:device] [[drive:][path]filename[...]]
```

Where /D:device - Specifies a print device.

### Example

```
print c:\example.txt /c /d:lpt1
```

The above command will print the example.txt file to the parallel port lpt1.

## Command Line Printer Control

As of Windows 2000, many, but not all, printer settings can be configured from Windows's command line using PRINTUI.DLL and RUNDLL32.EXE

### Syntax

```
RUNDLL32.EXE PRINTUI.DLL,PrintUIEntry [ options ] [ @commandfile ]
```

Where some of the options available are the following −

* **/dl** − Delete local printer.
* **/dn** − Delete network printer connection.
* **/dd** − Delete printer driver.
* **/e** − Display printing preferences.
* **/f[file]** − Either inf file or output file.
* **/F[file]** − Location of an INF file that the INF file specified with /f may depend on.
* **/ia** − Install printer driver using inf file.
* **/id** − Install printer driver using add printer driver wizard.
* **/if** − Install printer using inf file.
* **/ii** − Install printer using add printer wizard with an inf file.
* **/il** − Install printer using add printer wizard.
* **/in** − Add network printer connection.
* **/ip** − Install printer using network printer installation wizard.
* **/k** − Print test page to specified printer, cannot be combined with command when installing a printer.
* **/l[path]** − Printer driver source path.
* **/m[model]** − Printer driver model name.
* **/n[name]** − Printer name.
* **/o** − Display printer queue view.
* **/p** − Display printer properties.
* **/Ss** − Store printer settings into a file.
* **/Sr** − Restore printer settings from a file.
* **/y** − Set printer as the default.
* **/Xg** − Get printer settings.
* **/Xs** − Set printer settings.

## Testing if a Printer Exists

There can be cases wherein you might be connected to a network printer instead of a local printer. In such cases, it is always beneficial to check if a printer exists in the first place before printing.

The existence of a printer can be evaluated with the help of the RUNDLL32.EXE PRINTUI.DLL which is used to control most of the printer settings.

### Example

```
SET PrinterName = Test Printer
SET file=%TEMP%\Prt.txt
RUNDLL32.EXE PRINTUI.DLL,PrintUIEntry /Xg /n "%PrinterName%" /f "%file%" /q

IF EXIST "%file%" (
   ECHO %PrinterName% printer exists
) ELSE (
   ECHO %PrinterName% printer does NOT exists
)
```

The above command will do the following −

* It will first set the printer name and set a file name which will hold the settings of the printer.
* The RUNDLL32.EXE PRINTUI.DLL commands will be used to check if the printer actually exists by sending the configuration settings of the file to the file Prt.txt

