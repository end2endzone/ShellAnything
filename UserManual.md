# Overview #

The User Manual contains all essential information for the end-user to make full use of the ShellAnything application.
This manual includes a description of the system functionalities and capabilities, contingencies and alternate modes of operation, and step-by-step procedures for system access and use.

The following document explains the structure of configuration files.

This help file describes the process and documentation. It is divided in the following sections:
* section 1
* section 2
* section 3
* section 4



# Installation #

## Register the shell extension ##

Following the installation of the software, the shell extension must be registered on the Windows system to be recognized as a trusted source.

To register the shell extension, execute the following commands:
1) Open a command prompt with administrative priviledges.
   1) Click on the `Start Menu`
   2) Select `Accessories`
   3) Right-click on `Command Prompt` shortcut and select `Run as administrator`.

2) Navigate to the Shellanything installation directory. Enter the following command: `cd /d <insert-installation-directory-here>`.

3) 

Refer to the following document for details:
[How to Register a DLL File in Windows](https://helpdeskgeek.com/how-to/register-dll-file-in-windows/)




## Unregister the shell extension ##

To stop using the shell extension, you must unregister the extension from the Windows system.

Note that prior to uninstallation, the shell extension must also be unregistered from the system. Failure to unregister the shell extension from the system will 






# Configuration Files #

location
structure


## Basic Xml Document ##

sample

```xml
<?xml version="1.0" encoding="utf-8"?>
<root>
  <shell>
  <!-- define menus here -->
  </shell>
</root>
```

# Menus #

Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.
Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure
dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non
proident, sunt in culpa qui officia deserunt mollit anim id est laborum



# Visibility / Validity #

Affect menu's status (enabled/disabled and visible/invisible)



# Actions #

defines what is executed when 

# 
# Properties #

## Selection-based Properties ##

The following section defines the list of dynamic properties. Theses properties are updated based on the user selection.

```xml
<!--
Help:
  ${selection.path} is the full path of the clicked element
  ${selection.parent.path} is the full path of the parent element
  ${selection.parent.filename} is the filename of the parent element
  ${selection.filename} is selection.filename (including file extension)
  ${selection.filename.noext} is selection.filename without file extension
  
  ${env.XYZ} refers to environment variable XYZ
  
  Property names must not contain spaces
  Property lists must be separated by ';' character
  File extension lists must be separated by ';' character
-->
```
