# Batch Script - Devices

---



---

Windows now has an improved library which can be used in Batch Script for working with devices attached to the system. This is known as the device console DevCon.exe.

Windows driver developers and testers can use DevCon to verify that a driver is installed and configured correctly, including the proper INF files, driver stack, driver files, and driver package. You can also use the DevCon commands (enable, disable, install, start, stop, and continue) in scripts to test the driver. **DevCon** is a command-line tool that performs device management functions on local computers and remote computers.

Display driver and device info DevCon can display the following properties of drivers and devices on local computers, and remote computers (running Windows XP and earlier) −

* Hardware IDs, compatible IDs, and device instance IDs. These identifiers are described in detail in device identification strings.
* Device setup classes.
* The devices in a device setup class.
* INF files and device driver files.
* Details of driver packages.
* Hardware resources.
* Device status.
* Expected driver stack.
* Third-party driver packages in the driver store.
* Search for devices DevCon can search for installed and uninstalled devices on a local or remote computer by hardware ID, device instance ID, or device setup class.
* Change device settings DevCon can change the status or configuration of Plug and Play (PnP) devices on the local computer in the following ways −

  + Enable a device.
  + Disable a device.
  + Update drivers (interactive and non-interactive).
  + Install a device (create a devnode and install software).
  + Remove a device from the device tree and delete its device stack.
  + Rescan for Plug and Play devices.
  + Add, delete, and reorder the hardware IDs of root-enumerated devices.
  + Change the upper and lower filter drivers for a device setup class.
  + Add and delete third-party driver packages from the driver store.

DevCon (DevCon.exe) is included when you install the WDK, Visual Studio, and the Windows SDK for desktop apps. DevCon.exe kit is available in the following locations when installed.

```
%WindowsSdkDir%\tools\x64\devcon.exe
%WindowsSdkDir%\tools\x86\devcon.exe
%WindowsSdkDir%\tools\arm\devcon.exe
```

### Syntax

```
devcon [/m:\\computer] [/r] command [arguments]
```

wherein

* **/m:\\computer** − Runs the command on the specified remote computer. The backslashes are required.
* **/r** − Conditional reboot. Reboots the system after completing an operation only if a reboot is required to make a change effective.
* **command** − Specifies a DevCon command.
* To list and display information about devices on the computer, use the following commands −

  + DevCon HwIDs
  + DevCon Classes
  + DevCon ListClass
  + DevCon DriverFiles
  + DevCon DriverNodes
  + DevCon Resources
  + DevCon Stack
  + DevCon Status
  + DevCon Dp\_enum
* To search for information about devices on the computer, use the following commands −

  + DevCon Find
  + DevCon FindAll
* To manipulate the device or change its configuration, use the following commands −

  + DevCon Enable
  + DevCon Disable
  + DevCon Update
  + DevCon UpdateNI
  + DevCon Install
  + DevCon Remove
  + DevCon Rescan
  + DevCon Restart
  + DevCon Reboot
  + DevCon SetHwID
  + DevCon ClassFilter
  + DevCon Dp\_add
  + DevCon Dp\_delete

### Examples

Following are some examples on how the DevCon command is used.

```
List all driver files
```

The following command uses the DevCon DriverFiles operation to list the file names of drivers that devices on the system use. The command uses the wildcard character (\*) to indicate all devices on the system. Because the output is extensive, the command uses the redirection character (>) to redirect the output to a reference file, driverfiles.txt.

```
devcon driverfiles * > driverfiles.txt
```

The following command uses the DevCon status operation to find the status of all devices on the local computer. It then saves the status in the status.txt file for logging or later review. The command uses the wildcard character (\*) to represent all devices and the redirection character (>) to redirect the output to the status.txt file.

```
devcon status * > status.txt
```

The following command enables all printer devices on the computer by specifying the Printer setup class in a DevCon Enable command. The command includes the /r parameter, which reboots the system if it is necessary to make the enabling effective.

```
devcon /r enable = Printer
```

The following command uses the DevCon Install operation to install a keyboard device on the local computer. The command includes the full path to the INF file for the device (keyboard.inf) and a hardware ID (\*PNP030b).

```
devcon /r install c:\windows\inf\keyboard.inf *PNP030b
```

The following command will scan the computer for new devices.

```
devcon scan
```

The following command will rescan the computer for new devices.

```
devcon rescan
```

