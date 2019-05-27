# Overview #

This *User Manual* contains all essential information for the end-user to make full use of the ShellAnything application.
This manual includes a description of the system functionalities and capabilities with examples.

This help file is divided in the following sections:
* [Installation](#installation)
* [Configuration Files](#configuration-files)
  * [Menus](#menus)
  * [Visibility / Validity](#visibility-validity)
  * [Actions](#actions)
  * [Properties](#properties)
* [Complex Configuration File](#complex-configuration-file)
* [Use Cases](#use-cases)




# Installation #

The following instructions define how to activate the Shell Extension following the installation.



## Register the shell extension ##

Following the installation of the software, the shell extension must be registered on Windows system to be recognized as a trusted source.

To register the shell extension, execute the following commands:

1) Open a command prompt with administrative privileges.
   1) Click on the `Start Menu`.
   2) Select `Programs`, `Accessories`.
   3) Right-click on `Command Prompt` shortcut and select `Run as administrator`.

2) Navigate to the ShellAnything installation directory.</br>
   Enter the following command: `cd /d <insert-installation-directory-here>`.

3) Register the shell extension by entering the following command:
   `regsvr32 shellext.dll`
   
4) The following confirmation message should displayed:
```
[Window Title]
RegSvr32

[Content]
DllRegisterServer in shellext.dll succeeded.

[OK]
```

Refer to the following document for details:
[How to Register a DLL File in Windows](https://helpdeskgeek.com/how-to/register-dll-file-in-windows/)



## Unregister the shell extension ##

To stop using the shell extension, you must unregister the extension from the Windows system.

Note that prior uninstalling the application, the shell extension **must first** be unregistered from the system. Failure to unregister the shell extension from the system will probably fail the uninstall process or not fully remove the shell extension from the system.


To register the shell extension, execute the following commands:

1) Open a command prompt with administrative privileges.
   1) Click on the `Start Menu`.
   2) Select `Programs`, `Accessories`.
   3) Right-click on `Command Prompt` shortcut and select `Run as administrator`.

2) Navigate to the ShellAnything installation directory.</br>
   Enter the following command: `cd /d <insert-installation-directory-here>`.

3) Register the shell extension by entering the following command:
   `regsvr32 /u shellext.dll`
   
4) The following confirmation message should displayed:
```
[Window Title]
RegSvr32

[Content]
DllUnregisterServer in shellext.dll succeeded.

[OK]
```




# Configuration Files #

The power of ShellAnything comes from the ability to define dynamic content. To add additional menus to Windows Explorer context menu, one must define the desired menus and their behavior in a `Configuration File`.

A *configuration file* contains the definition of all [Menu](#Menu) element that should be displayed on a context menu. *Configuration file* are written in xml file format.

When a user right-click on a file in Windows Explorer, the application will load all available *configuration files* and display their content into the displayed context menu.

The location of *Configuration Files* is unique for each users of the system. The list is available in `C:\Users\%USERNAME%\ShellAnything` directory. Replace `%USERNAME%` by your current *username*. Note that *Windows Explorer* also support copy & pasting `C:\Users\%USERNAME%\ShellAnything` into an *address bar* to quickly get to the directory.

When a *configuration file* is deleted, the application automatically detect the missing file and properly unload the associated menus which stop displaying their content.

The application support multiple *configuration files* at the same time. To temporary disable a *configuration file*, one can simply change the file extension from `xml` to `txt`. Change the file extension back to `xml` to re-enable the file.



## Basic Xml Document ##

The following file sample shows the minimum requirement of a `Configuration File`:

```xml
<?xml version="1.0" encoding="utf-8"?>
<root>
  <shell>
    <!-- define menus here -->
  </shell>
</root>
```

Note that this sample file does not define any menus. 




# Menus #

A &lt;menu&gt; element in a `Configuration File` defines a menu item that is displayed on a context menu. A &lt;menu&gt; element must be inserted under the &lt;shell&gt; element.

The &lt;menu&gt; element have two attributes.

## Name attribute: ##

The first attribute is the `name` of the menu. The value of the name is the text that will be displayed on the context menu. For instance, the following menu element will display a `Test 1-2-3` options when you right-click a file or directory:

```xml
<menu name="Test 1-2-3">
</menu>
```

## Separator attribute: ##

The `separator` attribute allows one to define a separator between menu options. To define a separator, one must set the `separator` attributes to one of the following values: `true`, `yes` or `on`. For example, add the following menu element in a *configuration file*:

```xml
<menu separator="true" />
```

## Creating Sub Menus: ##

A `Configuration File` can have multiple menu element. 

To insert a sub menu, define a &lt;menu&gt; element under another &lt;menu&gt; element. For instance, to create a `Operations` top menu with a `Connect`, `Update` and `Restart` sub menus, the following xml should be used:

```xml
<menu name="Operations">
  <menu name="Connect">
  </menu>
  <menu name="Update">
  </menu>
  <menu name="Restart">
  </menu>
</menu>
```




# Visibility / Validity #

The &lt;visibility&gt; and &lt;validity&gt; elements act as filters for menus. They affect a menu's status: visible/invisible and enabled/disabled respectively. The &lt;visibility&gt; and &lt;validity&gt; elements must be added under a &lt;menu&gt; element.

Each element must be validated against multiple criterias defined by the attributes below. If the validation is successful, the menu will be set visible/enabled. If the validation fails then the associated menu is set invisible/disabled.

The &lt;visibility&gt; and &lt;validity&gt; elements have the following attributes:



## Maxfiles and Maxfolders attributes: ##

The `maxfiles` and `maxfolders` attributes validates a menu based on the files and folders selected by the user.

**MaxFiles Validation:**

If the number of selected files is equals or lower than the value specified by `maxfiles` attribute, the validation is successful. Otherwise, the validation fails.

If the number of selected directories is equals or lower than the value specified by `maxfolders` attribute, the validation is successful. Otherwise, the validation fails.

If `maxfiles` and `maxfolders` attributes are not specified, then the validation is successful.

Typical use of the `maxfiles` and `maxfolders` attributes is the following:

* Set `maxfiles`   to value '0' to force a folder-only menu.
* Set `maxfolders` to value '0' to force a file-only menu.
* Set `maxfiles`   to value '0' and `maxfolders` to value '1' to enable a menu when only a single folder is selected.
* Set `maxfiles`   to value '1' and `maxfolders` to value '0' to enable a menu when only a single file   is selected.

For example, the following set a menu visible only when the user right-click on a single file:
```xml
<visibility maxfiles="1" maxfolders="0" />
```



## Fileextensions attributes: ##

The `fileextensions` attribute validates a menu based on the file's extension selected by the user.

If `fileextensions` attribute is specified, the file's extension selected by the user must match for the validation to be successful. To specify multiple extension, one must separate each extensions with the `;` character.

If `fileextensions` attribute is not specified, then the validation is successful.

For example, the following set a menu visible only when the user right-click on a single executable file:
```xml
<visibility maxfiles="1" maxfolders="0" fileextensions="com;exe;bat;cmd" />
```



## Exists attribute: ##

The `exists` attribute validates a menu if the specified file exists.

If `exists` attribute is set, the application will search for the file. If the file can be found the validation is successful. To specify multiple files, one must separate each extensions with the `|` character.

If `exists` attribute is not specified, then the validation is successful.

For example, the following set a menu visible only when *Microsoft Word 2016* is installed on the system:
```xml
<visibility exists="C:\Program Files (x86)\Microsoft Office\Office16\WINWORD.EXE" />
```



## Properties attribute: ##

The `properties` attribute validates a menu if the specified property is not empty.

If `properties` attribute is set, the application will look for the specified property. If the property is not empty, the validation is successful. To specify multiple properties, one must separate each property with the `;` character.

If `properties` attribute is not specified, then the validation is successful.

For example, the following set a menu visible when the `process.started` property is set:
```xml
<visibility properties="process.started" />
```




# Icons #

The &lt;icon&gt; element allows one to assign icon to menus. The &lt;icon&gt; element must be added under a &lt;menu&gt; element.

The &lt;icon&gt; elements have the following attributes:



## Path attribute: ##

The `path` attribute defines the file path to an icon file or database. The path must be defined in absolute form. The `path` attribute supports the following icon file format: `ico`, `dll`, `ocx` and `cpl`.

For example, the following sets a menu with the *Home* icon :
```xml
<icon path="c:\windows\system32\ieframe.dll" />
```



## Index attribute: ##

The `index` attribute defines the index of an icon inside an icon database. The index is 0-based. The index is optional for `ico` file format. If the index is not specified, the value `0` is used. 

For example, the following sets a menu with the *Empty Recycle Bin* icon :
```xml
<icon path="C:\windows\system32\imageres.dll" index="50" />
```




# &lt;actions&gt; #

The `action` family elements defines the actions to execute when a user have clicked on a menu option. A specific action element must be added under the &lt;actions&gt; element which is added under a &lt;menu&gt; element.

The application support multiple types of actions. The available action are defined in the system below:



## &lt;exec&gt; action ##

Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.
Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure
dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non
proident, sunt in culpa qui officia deserunt mollit anim id est laborum


## &lt;open&gt; action ##

Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.
Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure
dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non
proident, sunt in culpa qui officia deserunt mollit anim id est laborum


## &lt;clipboard&gt; action ##

Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.
Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure
dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non
proident, sunt in culpa qui officia deserunt mollit anim id est laborum



## &lt;prompt&gt; action ##

Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.
Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure
dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non
proident, sunt in culpa qui officia deserunt mollit anim id est laborum



## &lt;property&gt; action ##

Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.
Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure
dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non
proident, sunt in culpa qui officia deserunt mollit anim id est laborum




 
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

# Complex Configuration File #



## Lorem ipsum dolor sit amet ##

Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.
Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure
dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non
proident, sunt in culpa qui officia deserunt mollit anim id est laborum



# Use Cases #



## Lorem ipsum dolor sit amet ##

Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.
Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure
dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non
proident, sunt in culpa qui officia deserunt mollit anim id est laborum


