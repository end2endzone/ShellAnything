# Overview #

This *User Manual* contains all essential information for the end-user to make full use of the ShellAnything application.
This manual includes a description of the system functionalities and capabilities with examples divided in the following sections:

* [Post Installation](#post-installation)
  * [Register the shell extension](#register-the-shell-extension)
  * [Unregister the shell extension](#unregister-the-shell-extension)
* [Configuration Files](#configuration-files)
  * [Menus](#menus)
  * [Visibility / Validity](#visibility-validity)
  * [Actions](#actions)
    * [&lt;exec&gt; action](#exec-action)
    * [&lt;open&gt; action](#open-action)
    * [&lt;clipboard&gt; action](#clipboard-action)
    * [&lt;prompt&gt; action](#prompt-action)
    * [&lt;property&gt; action](#property-action)
    * [&lt;file&gt; action](#file-action)
  * [Default](#Default)
* [Properties](#properties)
  * [Setting properties](#setting-properties)
  * [Property expansion](#property-expansion)
  * [Using properties](using-properties)
  * [Environment variables](#environment-variables)
  * [Selection-based properties](#selection-based-properties)
  * [Fixed properties](#fixed-properties)
  * [Default properties](#default-properties)
* [Use Cases](#use-cases)
  * [Integrate a third party application](#integrate-a-third-party-application)
  * [Run an application with parameters](#run-an-application-with-parameters)
  * [Open a command prompt in directory](#open-a-command-prompt-in-directory)
  * [Select two files for an operation](#select-two-files-for-an-operation)
* [Troubleshooting](#troubleshooting)
  * [Logging support](#logging-support)
  * [Reporting bugs](#reporting-bugs)




# Post Installation #

The following instructions define how to activate the Shell Extension after installing the application.



## Register the shell extension ##

Following the installation of the software, the shell extension must be registered on the system to be recognized as a trusted source.

To register the shell extension, execute the following commands:

1) Open a command prompt with administrative privileges.
   1) Click on the `Start Menu`.
   2) Select `All Programs`, `Accessories`.
   3) Right-click on `Command Prompt` shortcut and select `Run as administrator`.

2) Navigate to ShellAnything installation directory.
   Enter the command: `cd /d <insert-installation-directory-here>`, replacing `<insert-installation-directory-here>` by the actual installation directory.
   For instance, `cd /d C:\Program Files\Shellanything`.

3) Register the shell extension with the command:
   `regsvr32 shellext.dll`
   
4) The following confirmation message should displayed:
```
[Window Title]
RegSvr32

[Content]
DllRegisterServer in shellext.dll succeeded.

[OK]
```

Refer to the following post for details:
[How to Register a DLL File in Windows](https://helpdeskgeek.com/how-to/register-dll-file-in-windows/)



## Unregister the shell extension ##

To stop using the shell extension, you must unregister the extension from the system.

Note that prior to uninstall the application, the shell extension **must first** be unregistered from the system. Omitting to unregister the shell extension from the system will fail the uninstall process or not fully remove the shell extension from the system.


To unregister the shell extension, execute the following commands:

1) Open a command prompt with administrative privileges.
   1) Click on the `Start Menu`.
   2) Select `All Programs`, `Accessories`.
   3) Right-click on `Command Prompt` shortcut and select `Run as administrator`.

2) Navigate to ShellAnything installation directory.
   Enter the command: `cd /d <insert-installation-directory-here>`, replacing `<insert-installation-directory-here>` by the actual installation directory.
   For instance, `cd /d C:\Program Files\Shellanything`.

3) Register the shell extension with the following command:
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

The power of ShellAnything comes from the ability to define dynamic content from xml files. To add additional menu options to *Windows Explorer* context menu, one must define the desired menus (and their expected behavior) in a `Configuration File`.

A *configuration file* contains the definition of all [&lt;menu&gt;](#Menu) elements to be displayed on a context menu. *Configuration file* are written in xml file format and must be encoded in utf-8 format.

When a user right-click on a file in *Windows Explorer*, the application will load all available *configuration files* and display their content into the displayed context menu.

The list of *Configuration Files* is unique for each users of the system. The files are stored in `C:\Users\%USERNAME%\ShellAnything` directory where `%USERNAME%` is your current Windows session *username*. Note that *Windows Explorer* also support copy & pasting `C:\Users\%USERNAME%\ShellAnything` into an *address bar* to quickly jump to the directory.

The application support multiple *configuration files* at the same time. One can add new files in the *configuration directory* and the system will automatically detect and load them.

When a *configuration file* is deleted, the application automatically detect the missing file and properly unload the associated menus which stop displaying their content.

To temporary disable a *configuration file*, one can simply change the file extension from `xml` to `txt`. Change the file extension back to `xml` to re-enable the file.



## Basic Xml Document ##

The following xml file shows the minimum requirement of a `Configuration File`:

```xml
<?xml version="1.0" encoding="utf-8"?>
<root>
  <shell>
    <!-- insert menu definitions here -->
  </shell>
</root>
```

Note that this sample file does not define any menus. 



## Menus ##

A &lt;menu&gt; element in a `Configuration File` defines a menu item that is displayed on a context menu. A &lt;menu&gt; element must be inserted under the &lt;shell&gt; element.

The &lt;menu&gt; element have two attributes.



### name attribute: ###

The `name` attribute is the actual text that will be displayed on the context menu. For instance, the following menu element will display `Test 1-2-3` when you right-click a file or directory:

```xml
<menu name="Test 1-2-3">
</menu>
```



### separator attribute: ###

The `separator` attribute allows one to define a separator between menu options. To define a separator, one must set the `separator` attributes to one of the following values: `true`, `yes` or `on`. For example, add the following menu element in a *configuration file*:

```xml
<menu separator="true" />
```



### Creating Sub Menus: ###

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



## Visibility / Validity ##

The &lt;visibility&gt; and &lt;validity&gt; elements act as filters for menus. They affect a menu's status: visible/invisible and enabled/disabled respectively. The &lt;visibility&gt; and &lt;validity&gt; elements must be added under a &lt;menu&gt; element.

Each element must be validated against multiple criteria defined by the attributes below. If the validation is successful, the menu will be set visible/enabled. If the validation fails then the associated menu is set invisible/disabled.

The &lt;visibility&gt; and &lt;validity&gt; elements have the following attributes:



### maxfiles and maxfolders attributes: ###

The `maxfiles` and `maxfolders` attributes validates a menu based on the number of files and folders selected by the user.

If the number of selected files is equals or lower than the value specified by `maxfiles` attribute, the validation is successful. Otherwise, the validation fails.

If the number of selected directories is equals or lower than the value specified by `maxfolders` attribute, the validation is successful. Otherwise, the validation fails.

If `maxfiles` and `maxfolders` attributes are not specified, then the validation is successful.

Typical use of the `maxfiles` and `maxfolders` attributes is the following:

* Set `maxfiles`   to value '0' to force a folder-only menu.
* Set `maxfolders` to value '0' to force a file-only menu.
* Set `maxfiles`   to value '0' and `maxfolders` to value '1' to enable a menu only when a single folder is selected.
* Set `maxfiles`   to value '1' and `maxfolders` to value '0' to enable a menu only when a single file   is selected.

For example, the following set a menu visible only when the user right-click on a single file:
```xml
<visibility maxfiles="1" maxfolders="0" />
```



### fileextensions attribute: ###

The `fileextensions` attribute validates a menu based on the file's extension selected by the user.

If `fileextensions` attribute is specified, the file's extension selected by the user must match for the validation to be successful. To specify multiple extension, one must separate each extensions with the `;` character. If multiple extension are specified, **at least one** extension must match for the validation to be successful.

If `fileextensions` attribute is not specified, then the validation is successful.

For example, the following set a menu visible only when the user right-click on a single executable file:
```xml
<visibility maxfiles="1" maxfolders="0" fileextensions="com;exe;bat;cmd" />
```



### exists attribute: ###

The `exists` attribute validates a menu if the specified file exists.

If `exists` attribute is set, the application will search for the file. If the file exists on the system, the validation is successful. To specify multiple files, one must separate each extensions with the `;` character. If multiple files are specified, **all files** must exists on the system for the validation to be successful.

If `exists` attribute is not specified, then the validation is successful.

For example, the following set a menu visible only when *Microsoft Word 2016* is installed on the system:
```xml
<visibility exists="C:\Program Files (x86)\Microsoft Office\Office16\WINWORD.EXE" />
```



### properties attribute: ###

The `properties` attribute validates a menu if the specified property is not empty.

If `properties` attribute is set, the application will look for the specified property. If the property is not empty, the validation is successful. To specify multiple properties, one must separate each property with the `;` character. If multiple properties are specified, **all properties** must be not empty for the validation to be successful.

If `properties` attribute is not specified, then the validation is successful.

For example, the following set a menu visible when `process.started` property is not empty:
```xml
<visibility properties="process.started" />
```

See [properties](#properties) section for how to define custom properties.



## Icons ##

The &lt;icon&gt; element allows one to assign icon to menus. The &lt;icon&gt; element must be added under a &lt;menu&gt; element.

The &lt;icon&gt; elements have the following attributes:



### path attribute: ###

The `path` attribute defines the file path of an icon file or library. The path must be defined in absolute form. The `path` attribute supports the following icon file format: `ico`, `dll`, `ocx` and `cpl`.

For example, the following sets a menu with the *Home* icon (first icon of ieframe.dll library) :
```xml
<icon path="c:\windows\system32\ieframe.dll" />
```



### index attribute: ###

The `index` attribute defines the index of an icon inside an icon library. The index is 0-based. The index is optional for `ico` file format. If the index is not specified, the value `0` is used. 

For example, the following sets a menu with the *Empty Recycle Bin* icon :
```xml
<icon path="C:\windows\system32\imageres.dll" index="50" />
```



### fileextension attribute: ###

The `fileextension` attribute defines the icon of a menu based on the system's default icon for the given file extension. The fileextension is optional and have priority over the `path` and `index` attributes. 

For example, the following sets a menu with the *Text File* icon :
```xml
<icon fileextension="txt" />
```

The `fileextension` attribute supports dynamic properties and allows one to define an icon based on the user selection.
For instance, the following icon definition uses the same icon as the clicked file:
```xml
  <icon fileextension="${selection.filename.extension}" />
```



## Actions ##

The &lt;actions&gt; element contains the list of actions to execute when a user clicks on a menu option. Note that &lt;actions&gt; element does not define the actions to execute. The specific definition of each action must be inserted as children of the &lt;actions&gt; element.

The &lt;actions&gt; element must be added under a &lt;menu&gt; element.

The &lt;actions&gt; elements does not support any attributes.

The application support multiple types of actions. The list of each specific action supported by the application is defined below:



### &lt;exec&gt; action ###

The &lt;exec&gt; element is used to launch an application. The &lt;exec&gt; element must be added under the &lt;actions&gt; element.

The &lt;exec&gt; elements have the following attributes:



#### path attribute: ####

The `path` attribute defines the path of the executable to launch. The path may be specified in absolute or relative form. The system will verify that the file exists before launching the executable.

For example, the following launch the Windows Calculator:
```xml
<exec path="C:\Windows\System32\calc.exe" />
```



#### arguments attribute: ####

The `arguments` attribute defines the launching command line parameters sent to the application specified in the `path` attribute. The attribute is optional.

For example, the following launche `notepad.exe` and open the `License.txt` document :
```xml
<exec path="C:\Windows\notepad.exe" arguments="C:\Program Files\7-Zip\License.txt" />
```



#### basedir attribute: ####

The `basedir` attribute defines the directory to use as `current directory` when launching the application specified in the `path` attribute. The attribute is optional.

For example, the following launche `notepad.exe` and open the `License.txt` document from `7-Zip` installation directory :
```xml
<exec path="C:\Windows\notepad.exe" basedir="C:\Program Files\7-Zip" arguments="License.txt" />
```



### &lt;open&gt; action ###

The &lt;open&gt; element is used to open a document by the default associated application. The &lt;open&gt; element must be added under the &lt;actions&gt; element.

The &lt;open&gt; elements have the following attributes:



#### path attribute: ####

The `path` attribute defines the path of the document to open. The path must be specified in absolute form. The system will verify that the file exists before opening the document. The application will execute the `open` action (as if the user had right-clicked on the document ans selected `open` command).

For example, the following opens the default JPEG viewer to view following image:
```xml
<open path="C:\Windows\Web\Wallpaper\Landscapes\img7.jpg" />
```



### &lt;clipboard&gt; action ###

The &lt;clipboard&gt; element is used to change the value of the [Windows Clipboard](https://lifehacker.com/how-to-copy-cut-and-paste-for-beginners-5801525) to a new value. The &lt;clipboard&gt; element must be added under the &lt;actions&gt; element.

The &lt;clipboard&gt; elements have the following attributes:



#### value attribute: ####

The `value` attribute defines the new text value of the clipboard.

For example, the following sets the clipboard to the value `The quick brown fox jumps over the lazy dog.` :
```xml
<clipboard value="The quick brown fox jumps over the lazy dog." />
```



### &lt;prompt&gt; action ###

The &lt;prompt&gt; element is used to ask the user a question. The answer to the question is stored in [properties](#properties) and encoded in utf-8 format. The &lt;prompt&gt; element must be added under the &lt;actions&gt; element.

The &lt;prompt&gt; elements have the following attributes:



#### title attribute: ####

The `title` attribute defines the text value of the question displayed in the prompt message.

For example, the following ask the user a question :
```xml
<prompt title="What is your name?" />
```



#### name attribute: ####

The `name` attribute defines the name of the property to set with the value of the answer.

For example, the following sets the property `myprogram.user.fullname' with the prompt answer :
```xml
<prompt name="myprogram.user.fullname" />
```



#### default attribute: ####

The `default` attribute defines the default value of the answer. The attribute is optional.

For example, the following sets the value `Hugh Jass` as default value to the question `What is your name?` :
```xml
<prompt name="myprogram.user.fullname" title="What is your name?" default="Hugh Jass" />
```



#### type attribute: ####

The `type` attribute defines the type of the prompt. The attribute is optional.

The following table shows the attribute supported values:

| Values | Description                                                                  |
|--------|------------------------------------------------------------------------------|
| empty  | The prompt ask for a question and expect a text answer.                      |
| yesno  | The prompt ask for a question and expect a &lt;yes&gt; or &lt;no&gt; answer. |
| ok     | The prompt display the question and wait for the user to press OK.           |

For example, the following ask the question `Enable verbose output?` and sets `verbose` property to `/verbose` or `/normal` based on the the user response:
```xml
<prompt type="yesno" name="verbose" title="Enable verbose output mode?" valueyes="/verbose" valueno="/normal" />
```



#### valueyes attribute: ####

The `valueyes` attribute defines the value of the property when the user select "yes" as the answer. The attribute is optional and only valid if `type` attribute is set to `yesno`.

For example, the following sets `verbose` property to `/verbose` when if the user response is "yes":
```xml
<prompt type="yesno" name="verbose" title="Enable verbose output mode?" valueyes="/verbose" valueno="/normal" />
```



#### valueno attribute: ####

The `valueno` attribute defines the value of the property when the user select "no" as the answer. The attribute is optional and only valid if `type` attribute is set to `yesno`.

For example, the following sets `verbose` property to `/normal` when if the user response is "no":
```xml
<prompt type="yesno" name="verbose" title="Enable verbose output mode?" valueyes="/verbose" valueno="/normal" />
```



### &lt;property&gt; action ###

The &lt;property&gt; element is used to set a property to a specific value. The modified property can be used with the &lt;visibility&gt; and &lt;validity&gt; elements to create advanced dynamic menus.
The modified property can be used to temporary store the current selection path of filename in properties for use cases with multiple file selection.

See the [properties](#properties) section for details.

The &lt;property&gt; elements have the following attributes:



#### name attribute: ####

The `name` attribute defines the name of the property to set with a new value.

For example, the following sets the property `myprogram.user.name` to an empty value :
```xml
<property name="myprogram.user.name" />
```



#### value attribute: ####

The `value` attribute defines the actual new value of the given property.

For example, the following set the property `myprogram.user.name` to value `Al Coholic` :
```xml
<property name="myprogram.user.fullname" value="Al Coholic" />
```



### &lt;file&gt; action ###

The &lt;file&gt; element is used to create a text file on disk. The content of the file is specified between the opening and closing tags of the &lt;file&gt; element. The &lt;file&gt; element supports dynamic properties and can be used to create default configuration files or create support files for the menu.

The &lt;file&gt; elements have the following attributes:



#### path attribute: ####

The `path` attribute defines the path of the file. The path must be defined in absolute form.

For example, the following action creates a new configuration file named `config.ini` in the user's HOME directory :
```xml
<file path="${env.USERPROFILE}\config.ini">[user]
username=${env.USERNAME}
language=en

[environment]
java=${env.JAVA_HOME}\bin\java.exe
working_dir=${selection.path}

[database]
server=192.168.0.1
port=1562</file>
```



## Default ##

A &lt;default&gt; element in a `Configuration File` defines the default settings of a configuration file. A &lt;default&gt; element must be inserted under the &lt;shell&gt; element.

The &lt;default&gt; element have no attributes.

For example, an empty *default* section in a configuration file looks like this:
```xml
<?xml version="1.0" encoding="utf-8"?>
<root>
  <shell>
    <default>
    </default>
  </shell>
</root>
```

The default settings supported by the application is the following:
* Properties. See [default properties](#default-properties) section for details.




# Properties #

The ShellAnything application supports an advanced property system which allows one to assing values to custom named variables. The property system of ShellAnything plays a big role in implementing dynamic content for *Windows Explorer's* context menu.

Properties must conform to the following rules:
* Properties only support string values.
* Property names are case sensitive and cannot contain spaces.
* Property lists must be separated by `;` character.



## Setting properties ##

The application supports multiple way to set a property to a custom value:
* With the [&lt;prompt&gt;](#prompt-action) action element, one can set a property to the user's prompt answer.
* With the [&lt;property&gt;](#property-action) action element, one can set a property to the user's desired value.



## Property expansion ##

Property expansion allows you to insert variable content in a string value, menu name, icon path, custom action, and so on.

The syntax of a property expansion is as follows: `${name-of-property}` where `name-of-property` is the actual name of a property.
The name of a property is case sensitive.

For instance, the following would create a menu "send file by email" with the actual file name in the menu name:
```
<menu name="Send file '${my_filename}' by email.">
```



## Using properties ##

**Example #1:**

Properties can be used to store temporary values (for instance, based on the selection). For example, a menu can save the clicked file path to a property so that another menu can later refer to it. The typical use case is when comparing files: the first menu "select first file" and save the path to a property and the second menu "select second file" and launches the compare application with the two files.
The implementation of such menus would be like this:

```xml
  <menu name="Select first file">
    <actions>
      <property name="first.selection.path" value="${selection.path}" />
    </actions>
  </menu>
  <menu name="Compare with first file">
    <actions>
      <!--Store this file into a property for convenience reason.-->
      <property name="second.selection.path" value="${selection.path}" />

      <!-- run application that compares file "${first.selection.path}" with "${second.selection.path}" -->
    </actions>
  </menu>
```

This example uses [selection-based properties](#selection-based-properties). See the [associated section](#selection-based-properties) below for details.

**Example #2:**

Properties can be used with the &lt;visibility&gt; and &lt;validity&gt; elements to create advanced dynamic menus. 
For instance, if one wants to implement 2 menus (called 'A' and 'B') and only show menu 'B' after having clicked on menu 'A', one would be able to implement the desired functionality like the following:
```xml
  <menu name="A">
    <actions>
      <property name="menu_a_clicked" value="true" />
    </actions>
  </menu>
  <menu name="B">
    <!-- This menu will be visible only after menu A is clicked -->
    <visibility properties="menu_a_clicked" />
    <actions>
      <!--Reset the property to hide menu B-->
      <property name="menu_a_clicked" value="" />
    </actions>
  </menu>
```



## Environment variables ##

The list of environment variables is available through the property system.

The syntax of an environment variable property expansion is as follows: `${env.name-of-environment-variable}` where `name-of-environment-variable` is the actual name of an environment variable.
The name of the variable is case sensitive.

For example, the following would create a menu with the current user login name:
```xml
<menu name="Configure '${env.USERNAME}' settings.">
```

Environment variables properties are encoded in utf-8.



## Selection-based properties ##

The application provides a list of dynamic properties. The values of these properties will change based on the user selection when a user right-click files or folders.

The following table defines the list of dynamic properties and their utility:

| Property                     | Description                                                              |
|------------------------------|--------------------------------------------------------------------------|
| selection.path               | Matches the full path of the clicked element.                            |
| selection.filename           | Matches the filename  of the clicked element.                            |
| selection.filename.noext     | Matches the filename  of the clicked element without the file extension. |
| selection.parent.path        | Matches the full path of the parent  element.                            |
| selection.parent.filename    | Matches the filename  of the parent  element.                            |
| selection.filename.extension | Matches the file extension of the clicked element.                       |

Selection-based properties are encoded in utf-8.


## Fixed properties ##

The application defines a list of properties about the current runtime. The values of these properties are constant (fixed) and do not change.

The following table defines the list of fixed properties and their utility:

| Property              | Description                                       |
|-----------------------|---------------------------------------------------|
| application.path      | Matches the full path of the shell extension DLL. |
| application.directory | Matches the directory of the shell extension.     |
| log.directory         | Matches the directory where the logs are created. |
| config.directory      | Matches the directory of the configuration files. |

Fixed properties are encoded in utf-8.



## Default properties ##

Each configuration file optionally define a list of properties to initialize when a new configuration file is loaded. This mechanism allows one to define default property for the configuration. 

For example, the following would define `services.wce.command.start` and `services.wce.command.stop` properties when the configuration is loaded:
```xml
<default>
  <property name="services.wce.command.start" value="runwce /start" />
  <property name="services.wce.command.stop"  value="runwce /stop"  />
</default>
```




# Use Cases #



## Integrate a third party application ##

[WinDirStat](https://windirstat.net/) is a disk usage statistics viewer. It is a 3rd party application that do not provide a shell extension with the installer. The application have command line support with the following syntax: `windirstat.exe <folder>`.

With ShellAnything, a context menu option can be easily created to allow WinDirStat integration with *Windows Explorer* :

```xml
<?xml version="1.0" encoding="utf-8"?>
<root>
  <shell>
    <menu name="WinDirStat">
      <icon path="C:\Program Files (x86)\WinDirStat\windirstat.exe" index="0" />
      <visibility maxfiles="0" maxfolders="1" exists="C:\Program Files (x86)\WinDirStat\windirstat.exe" />
      <actions>
        <exec path="C:\Program Files (x86)\WinDirStat\windirstat.exe" arguments="&quot;${selection.path}&quot;" />
      </actions>
    </menu>    
  </shell>
</root>
```



## Run an application with parameters ##

Run an application that requires special parameters (a.k.a arguments) requires one to open a `command prompt`, navigate to the target directory, and run the command with the parameters.

ShellAnything can easily create a menu that is available only with one right-click a directory. The menu prompt the user for the arguments and run the desired application:

```xml
<?xml version="1.0" encoding="utf-8"?>
<root>
  <shell>
    <menu name="Run with parameters..." description="Run the selected executable with parameters">
      <icon path="C:\Windows\System32\shell32.dll" index="76" />
      <visibility maxfiles="1" maxfolders="0" fileextensions="com;exe;bat;cmd" />
      <actions>
        <prompt name="myarguments" title="Please enter the command line parameters" />
        <exec path="${selection.path}" arguments="${myarguments}" />
      </actions>
    </menu>
  </shell>
</root>
```



## Open a command prompt in directory ##

Developers often needs to open a command prompt at the current location when navigating *Windows Explorer*.
ShellAnything can quickly open a command prompt inside the current *Windows Explorer* directory:

```xml
<?xml version="1.0" encoding="utf-8"?>
<root>
  <shell>
    <menu name="Open command line here">
      <icon path="C:\Windows\System32\cmd.exe" index="0" />
      <visibility maxfiles="0" maxfolders="1" />
      <actions>
        <exec path="${env.ComSpec}" basedir="&quot;${selection.path}&quot;" />
      </actions>
    </menu>
  </shell>
</root>
```



## Select two files for an operation ##

If an application requires multiple input files (ie: compare files), ShellAnything allows creating a menu that select the first file and then show another menu for picking the second file and launch the application.

The following xml sample can select two files for an operation :

```xml
<?xml version="1.0" encoding="utf-8"?>
<root>
  <shell>
    <menu name="Compare">
      <visibility maxfiles="1" maxfolders="0" />
      <icon path="C:\Windows\System32\dsuiext.dll" index="32" />
      <menu name="Select file '${selection.filename}'">
        <actions>
          <!-- Remember the selected file in properties. The path will be used later -->
          <property name="compare.first.selection.path"     value="${selection.path}" />
          <property name="compare.first.selection.filename" value="${selection.filename}" />
        </actions>
      </menu>
      <menu name="Compare with '${compare.first.selection.filename}' (text)">
        <icon path="C:\Windows\System32\imageres.dll" index="14" />
        <!-- Hide this menu until a choice was made in 'Select file' menu -->
        <visibility properties="compare.first.selection.filename" />
        <actions>
          <!-- Run the compare command with the following 2 files: ${compare.first.selection.path} and ${selection.path} -->

          <!--Reset the properties now that compare is complete-->
          <property name="compare.first.selection.path"     value="" />
          <property name="compare.first.selection.filename" value="" />
        </actions>
      </menu>
    </menu>
  </shell>
</root>
```




# Troubleshooting #



## Logging support ##

ShellAnything provides logging support for troubleshooting and debugging commands.



### Location ###

The logging directory is unique for each users of the system.

The log files are stored in `C:\Users\%USERNAME%\ShellAnything\Logs` directory where `%USERNAME%` matches your current login username.
For example, the user `JohnSmith` can find his own ShellAnything log files in directory `C:\Users\JohnSmith\ShellAnything\Logs`.



### Filename Format ###

The log files names are generated in the following format:
`name.LEVEL.date-time.pid.log`.

The following table defines each field identifier in the filename format:

| Identifier | Description                                                                                                                                                           |
|:----------:|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------|
|    name    | Matches the name of the dll that generates log messages. Usually `shellext.dll`.                                                                                      |
|    LEVEL   | The logging level. Accepted values are `ERROR`, `WARNING` and `INFO`. See [glog documentation](https://github.com/google/glog/blob/v0.4.0/doc/glog.html) for details. |
|    date    | The current date when the file was created in `yyyymmdd` format.                                                                                                      |
|    time    | The current date when the file was created in `hhmmss` format.                                                                                                        |
|     pid    | The Process Id which generates the log message.                                                                                                                       |

For example, `shellext.dll.INFO.20191125-083305.9456.log` is a valid log filename.



### Levels ###

The logging library used by ShellAnything is [Google's logging module (glog)](https://github.com/google/glog). 

All messages logged by the application have an associated level to describe the severity of the message.

The application supports 3 differents logging levels: `ERROR`, `WARNING` and `INFO`. See [glog documentation](https://github.com/google/glog/blob/v0.4.0/doc/glog.html) for details.

Each level have a dedicated log file for storing messages. For example:
```
shellext.dll.INFO.20191125-083305.9456.log
shellext.dll.WARNING.20191125-083701.9456.log
shellext.dll.ERROR.20191125-084152.9456.log
```
See [Filename Format](#filename-format) for details.

Each log file contains the messages of it's matching level but also the messages with higher severity.
In other words, all logs messages are available in the INFO log file, including messages logged with WARNING and ERROR levels.

The ERROR   log file contains all messages with ERROR level.</br>
The WARNING log file contains all messages with ERROR and WARNING levels.</br>
The INFO    log file contains all messages with ERROR, WARNING and INFO levels.

Debugging log messages are only available in debug builds of the application.



### Log files life cycle ###

The log files are kept for a maximum of 5 days. This is for security reason as the files may contains sensitive information.

On application startup, the existing log files are all validated and files older than 5 days are deleted.

To force the application restart, reboot the computer and open a new Windows Explorer or close all Windows Explorer before opening a new Window Explorer window.

There are no plan for keeping the log files for more than 5 days.



## Reporting bugs ##

If you find any issues while using ShellAnything, please report your findings using the [project issue page on GitHub](https://github.com/end2endzone/ShellAnything/issues).

Improvements to the software are also welcome and can be proposed using the same method.
