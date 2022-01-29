![ShellAnything logo](docs/ShellAnything-splashscreen.jpg?raw=true)


# Overview #

This *User Manual* contains all essential information for the end-user to make full use of ShellAnything application.
This manual includes a description of the system functionalities and capabilities with examples divided in the following sections:

* [Post Installation](#post-installation)
  * [Register the shell extension](#register-the-shell-extension)
  * [Unregister the shell extension](#unregister-the-shell-extension)
* [Configuration Files](#configuration-files)
  * [Menus](#menus)
  * [Visibility / Validity](#visibility--validity)
    * [class attribute](#class-attribute)
    * [maxfiles and maxfolders attributes](#maxfiles-and-maxfolders-attributes)
    * [fileextensions attribute](#fileextensions-attribute)
    * [pattern attribute](#pattern-attribute)
    * [exists attribute](#exists-attribute)
    * [properties attribute](#properties-attribute)
    * [exprtk attribute](#exprtk-attribute)
    * [istrue attribute](#istrue-attribute)
    * [isfalse attribute](#isfalse-attribute)
    * [isempty attribute](#isempty-attribute)
    * [inverse attribute](#inverse-attribute)
    * [Combining multiple &lt;visibility&gt; and &lt;validity&gt; elements](#combining-multiple-visibility-and-validity-elements)
  * [Icons](#icons)
  * [Actions](#actions)
    * [&lt;exec&gt; action](#exec-action)
    * [&lt;open&gt; action](#open-action)
    * [&lt;clipboard&gt; action](#clipboard-action)
    * [&lt;prompt&gt; action](#prompt-action)
    * [&lt;message&gt; action](#message-action)
    * [&lt;property&gt; action](#property-action)
    * [&lt;file&gt; action](#file-action)
    * [&lt;stop&gt; action](#stop-action)
  * [Default](#default)
* [Properties](#properties)
  * [Setting properties](#setting-properties)
  * [Property expansion](#property-expansion)
  * [Using properties](#using-properties)
  * [Environment variables](#environment-variables)
  * [Selection-based properties](#selection-based-properties)
  * [Multi-selection-based properties](#multi-selection-based-properties)
  * [Fixed properties](#fixed-properties)
  * [Default properties](#default-properties)
* [Use Cases](#use-cases)
  * [Integrate a third party application](#integrate-a-third-party-application)
  * [Run an application with parameters](#run-an-application-with-parameters)
  * [Open a command prompt in directory](#open-a-command-prompt-in-directory)
  * [Select two files for an operation](#select-two-files-for-an-operation)
* [Other](#other)
  * [Keyboard mnemonics](#mnemonics--keyboard-shortcuts) 
* [Troubleshooting](#troubleshooting)
  * [Logging support](#logging-support)
  * [Missing ampersand character (`&`) in menus](#missing-ampersand-character--in-menus)
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
   For instance, `cd /d C:\Program Files\ShellAnything`.

3) Register the shell extension with the command:
   `regsvr32 sa.shellextension.dll`
   
4) The following confirmation message should displayed:
```
[Window Title]
RegSvr32

[Content]
DllRegisterServer in sa.shellextension.dll succeeded.

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
   For instance, `cd /d C:\Program Files\ShellAnything`.

3) Register the shell extension with the following command:
   `regsvr32 /u sa.shellextension.dll`
   
4) The following confirmation message should displayed:
```
[Window Title]
RegSvr32

[Content]
DllUnregisterServer in sa.shellextension.dll succeeded.

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

Menu names supports _keyboard mnemonics_, see the [Mnemonics / keyboard shortcuts](#mnemonics--keyboard-shortcuts) section for details.



### separator attribute: ###

The `separator` attribute allows one to define an *horizontal* separator between menu options. To define an *horizontal* separator, one must set the `separator` attributes to one of the following values: `true`, `yes`, `on` or `horizontal`. For example, the following creates a separator between menu *foo* and *bar* :

```xml
<menu name="foo" />
<menu separator="true" />
<menu name="bar" />
```

ShellAnything also support creating *vertical* (or column) separators. When using such a separator, *Windows* renders the popup menu with multiple columns. The menus following a column separator are rendered in a new column. For example, look at [this image](docs/multiple_columns_menu.png?raw=true) to see a multi-column menu. To define a *vertical* separator, one must set the `separator` attributes to value  `vertical` or `column`. For example, the following creates a two column menu :

```xml
<menu name="Left" />
<menu name="logical" />
<menu name="analytical" />
<menu name="objective" />
<menu separator="column" />
<menu name="Right" />
<menu name="intuitive" />
<menu name="creativity" />
<menu name="subjective" />
```



### maxlength attribute: ###

The `maxlength` attribute allows one to define a maximum length of a menu name. This allows one to limit the width of the menu and prevent system (or at least, File Explorer) stability issues.

If a menu name expands to a string longer than `maxlength` characters, the name is truncated to exactly `maxlength` characters.

The length validation is applied to the expanded value of the `name` attribute.

If `maxlength` attribute is not specified, the value `250` is used. If the menu name expands to a string longer than 250 characters, the name is trimmed to 250 characters and a trailing "..." is added at the end to indicate that menu name reached the maximum value supported by ShellAnything. The trailing "..." is not added if `maxlength` is set to a value different than 250.

To set a maximum length, one must set the `maxlength` attributes to a value between 1 and 250.

For example, the following limits a menu name to 40 characters :
```xml
<menu name="${super_long_property}" maxlength="40" />
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

The &lt;visibility&gt; and &lt;validity&gt; elements act as filters for menus. They affect a menu's visibility and validity status respectively allowing one to set a menu visible or invisible, enabled or disabled.

The &lt;visibility&gt; and &lt;validity&gt; elements must be added under a &lt;menu&gt; element.

Each &lt;visibility&gt; and &lt;validity&gt; element must be validated against multiple criteria defined by the attributes below. If the validation is successful, the menu will be set visible or enabled. If the validation fails then the associated menu is set invisible or disabled.

If multiple attributes are specified for a single &lt;visibility&gt; or &lt;validity&gt; element, all attributes must be evaluated for the validation to be successful. The logical `and` operator is used with each attribute.

Multiple &lt;visibility&gt; or &lt;validity&gt; elements can be added under a &lt;menu&gt; element. The logical `or` operator is used with each element of the same type. This allows combining elements to achieve more complex validation. See [Combining multiple &lt;visibility&gt; and &lt;validity&gt; elements](#combining-multiple-visibility-and-validity-elements) for details.
 
The &lt;visibility&gt; and &lt;validity&gt; elements have the following attributes:



### class attribute: ###

The `class` attribute validates a menu based on type (class) of the selected files. The attribute can be used to quickly filter a menu based on the user selection. It covers most filtering use cases:
* Filter for files only.
* Filter for directories only.
* Filter by file extensions.

If `class` attribute is specified, the classes of the files selected by the user must match for the validation to be successful. To specify multiple classes, one must separate each class with the `;` character. If multiple classes are specified, **at least one** class must match for the validation to be successful.

If multiple files are selected, **each file** must match **at least one** allowed class for the validation to be successful.

If `class` attribute is not specified, then the validation is successful.

The `class` attribute supported values are explained in the following table:

| Values          | Meaning                                                                                                                                                |
|-----------------|--------------------------------------------------------------------------------------------------------------------------------------------------------|
| .&lt;ext&gt;    | Validates a menu if the selected file extension matches the file extension *&lt;ext&gt;* . Replace *&lt;ext&gt;* by your desired file extension value. |
| file            | Validates a menu if the user has **only** selected files.                                                                                              |
| directory       | Validates a menu if the user has **only** selected folders or directories.                                                                             |
| drive           | Validates a menu if the selected file is located on a drive.                                                                                           |
| drive:removable | Validates a menu if the selected file is located on a removable drive.                                                                                 |
| drive:fixed     | Validates a menu if the selected file is located on a fixed drive.                                                                                     |
| drive:network   | Validates a menu if the selected file is located on a network drive or a network path.                                                                 |
| drive:optical   | Validates a menu if the selected file is located on an optical drive.                                                                                  |
| drive:ramdisk   | Validates a menu if the selected file is located on a ramdisk drive.                                                                                   |

For example, the following set a menu visible only when the user right-click on files:
```xml
<visibility class="file" />
```



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

If `fileextensions` attribute is specified, the file's extension selected by the user must match for the validation to be successful. To specify multiple file extensions, one must separate each extension with the `;` character. If multiple file extensions are specified, **at least one** extension must match for the validation to be successful.

If multiple files are selected, ***each file*** must match **at least one** allowed file extension for the validation to be successful.

If `fileextensions` attribute is not specified, then the validation is successful.

For example, the following set a menu visible only when the user right-click on a single executable file:
```xml
<visibility maxfiles="1" maxfolders="0" fileextensions="com;exe;bat;cmd" />
```



### pattern attribute: ###

The `pattern` attribute validates a menu based on a wildcard pattern matching algorithm. The wildcard pattern can include special characters such as `*` and `?` where
* `*` Matches any string of zero or more characters.
* `?` Matches any single character.

If `pattern` attribute is specified, the files selected by the user must match the wildcard pattern for the validation to be successful. To specify multiple patterns, one must separate each pattern value with the `;` character. If multiple patterns are specified, **at least one** pattern must match for the validation to be successful.

If multiple files are selected, ***each file*** must match **at least one** pattern for the validation to be successful.

If `pattern` attribute is not specified, then the validation is successful.

For example, the following set a menu visible only when the user right-click on JPEG image files which filenames start by `IMG``:
```xml
<visibility pattern="*\IMG*.jpg" />
```

The following table show useful pattern examples:

| Pattern                                                       | Meaning                                                                                       |
|---------------------------------------------------------------|-----------------------------------------------------------------------------------------------|
| *\\e\*.???                                                    | Matches filenames beginning with the letter `e`.                                              |
| \*e.???                                                       | Matches filenames ending with the letter `e`.                                                 |
| \*vacations\*                                                 | Matches files that have the word `vacations` in their path.                                   |
| \*2019\*                                                      | Matches files that have the year 2019 in the filename or directory path.                      |
| \*\\DCIM\\\*                                                  | Matches the files located in a Digital Camera Images directory.                               |
| C:\Program Files\\\*;<br>C:\Program Files (x86)\\\*           | Matches files that are located in `C:\Program Files` or `C:\Program Files (x86)` directories. |
| ${env.USERPROFILE}\\Downloads;<br>${env.USERPROFILE}\\Desktop | Matches files that are located in the user's `Downloads` or `Desktop` directories.            |
| D:\\\*                                                        | Matches files located on the D: drive.                                                        |
| \*\\IMG_????.JPG;<br>\*\\DSC_????.JPG                         | Matches Canon or Nikon image files.                                                           |

**Note:**
The `pattern` attribute should not be used for matching files by file extension. The `fileextensions` attribute should be used instead.



### exists attribute: ###

The `exists` attribute validates a menu if the specified file or directory exists.

If `exists` attribute is set, the application will search for the file. If the file or directory exists on the system, the validation is successful. To specify multiple files/directories, one must separate each value with the `;` character. If multiple values are specified, **all values** must exists on the system for the validation to be successful.

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



### exprtk attribute: ###

The `exprtk` attribute validates a menu based on a string expression algorithm. The expression must be specified as a mathematical expression and the result must evaluates to `true` or `false` such as `4 == 5` or `10 > 3`.

If `exprtk` attribute is specified, the expression must evaluates to `true` for the validation to be successful. The `exprtk` attribute does not support multiple expressions but logical `and` and `or` operators can be use to group expressions.

If `exprtk` attribute is not specified, then the validation is successful.

The attribute supports the following operators: 
* Basic operators: `+`, `-`, `*`, `/`, `%`, `^`
* Equalities & Inequalities: `=`, `==`, `<>`, `!=`, `<`, `<=`, `>`, `>=`
* Logic operators: `and`, `not`, `or`, `xor`, `true`, `false`
* String operators: `in`, `like`, `ilike`, []

Strings may be comprised of any combination of letters, digits special characters including (~!@#$%^&*()[]|=+ ,./?<>;:"_) or hexadecimal escaped sequences (eg: \0x30) and must be enclosed with single-quotes.
eg: `'Frankly my dear, \0x49 do n0t give a damn!'`

The `exprtk` attribute allows advanced menu validation. The following table show useful expression examples:

| Use cases                                                        | Expression                        | Meaning                                                                                                                    |
|------------------------------------------------------------------|-----------------------------------|----------------------------------------------------------------------------------------------------------------------------|
| Test a property for a numeric value                              | ${value} == 6                     | Evaluates to true when property `value` is set to numeric value `6`.                                                       |
| Test a property for a string value.                              | '${name}' == 'John'               | Evaluates to true when property `name` is set to string value `John`.                                                      |
| Set a menu visible based on how many file are selected           | ${selection.count} == 3           | Evaluates to true when user clicked on exactly 3 elements.                                                                 |
| Set a menu visible based on a state machine                      | '${myapp.state}' == 'PAUSED'      | Evaluates to true when application's state is `PAUSED`.                                                                    |
| Set a menu *invisible* when it was selected 3 times.             | ${myapp.runs} <= 2                | Evaluates to true when property `myapp.runs` is lower or equals to `2`.                                                    |
| Set a menu visible by filename length.                           | '${selection.filename}'[] == 9    | Evaluates to true when user clicked on a file whose filename<br>(including file extension) is exactly 9 characters.        |
| Combine expressions with `and` and `or` logic.                   | ${foo} == 2 or ${bar} >= 5        | Evaluates to true when property `foo` is set to `2` *or*<br>when property `bar` is set to a value greater or equal to `5`. |
| Set a menu visible if user selection contains a specific string. | 'abc' in '${selection.path}'      | Evaluates to true when user clicked on a file that contains the string `abc`.                                              |
| Set a menu visible if user selection matches a string pattern.   | '${selection.path}' ilike '*.exe' | Evaluates to true when user clicked on a file with `exe` extension.                                                        |

**Note:**
The `exprtk` attribute uses the *exprtk library* to parse the expression. For more details and supported expressions, see the exprtk documentation on the [official github page](https://github.com/ArashPartow/exprtk) or the [library website](http://www.partow.net/programming/exprtk/index.html).



### istrue attribute: ###

The `istrue` attribute validates a menu if the specified value evaluates to *true*.

If `istrue` attribute is set, the application will evaluate the given value. If the value evaluates to *true*, the validation is successful. To specify multiple values, one must separate each value with the `;` character. If multiple values are specified, **all values** must evaluates to *true* for the validation to be successful.

If `istrue` attribute is not specified, then the validation is successful.

For example, the following set a menu visible only when the property `initialized` evaluates to *true*:
```xml
<visibility istrue="${initialized}" />
```

The following values evaluates to *true* : `true`, `yes`, `ok`, `on` and `1`. The evaluation is case insensitive. All other values **does not** evaluates to true and will fail the validation. 

For systems that are not configured in English, your system may use another value for *true*. For such system, ShellAnything has defined property `system.true` which allows one to set the property to the same value as `Yes` but in your system native language.
For example, you can set property `system.true` to `是的`, `sí`, `हां`, `sim`, `да`, `はい`, `oui` or `ja`.



### isfalse attribute: ###

The `isfalse` attribute validates a menu if the specified value evaluates to *false*.

If `isfalse` attribute is set, the application will evaluate the given value. If the value evaluates to *false*, the validation is successful. To specify multiple values, one must separate each value with the `;` character. If multiple values are specified, **all values** must evaluates to *false* for the validation to be successful.

If `isfalse` attribute is not specified, then the validation is successful.

For example, the following set a menu visible only when the property `initialized` evaluates to *false*:
```xml
<visibility isfalse="${initialized}" />
```

The following values evaluates to *false* : `false`, `no`, `fail`, `off` and `0`. The evaluation is case insensitive. All other values **does not** evaluates to false and will fail the validation. 

For systems that are not configured in English, your system may use another value for *false*. For such system, ShellAnything has defined property `system.false` which allows one to set the property to the same value as `No` but in your system native language.
For example, you can set property `system.false` to `不`, `नहीं`, `não`, `нет`, `いいえ`, `non` or `Nein`.



### isempty attribute: ###

The `isempty` attribute validates a menu if the specified text is empty. The attribute does not support multiple values.

If `isempty` attribute is not specified, then the validation is successful. If a text value is specified, the resulting expansion must be empty for the validation to be successful.

For example, the following set a menu visible only when the property `first_selected` is empty:
```xml
<visibility isempty="${first_selection}" />
```



### inverse attribute: ###

The `inverse` attribute inverts the logic of one or multiple attributes. For example, to inverse the meaning of the `maxfiles` attribute, set `inverse` attribute to the value `maxfiles`. 

To specify multiple inverted attributes, one must separate each attribute names with the `;` character. To invert all possible attributes, use the value 'all'.

If `inverse` attribute is not specified, then the validation is successful.

The meaning of each inversed attribute in explained in the following table:

| Attributes     | Meaning                                                                                                                                                                                                                                      |
|----------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| class          | Validates a menu if the selected file or directory **does not** match the class of the selected files.<br>If multiple classes are specified, **no classes** must match the selected files for the validation to be successful.               |
| maxfiles       | Defines a minimum number of selected files. Validates a menu if **more than** _x_ files are selected.<br>If 'maxfiles` is set to 5, _more than_ 5 files must be selected for the validation to be successful.                                |
| maxfolders     | Defines a minimum number of selected folder. Validates a menu if **more than** _x_ folders are selected.<br>If 'maxfolders` is set to 3, _more than_ 3 directories must be selected for the validation to be successful.                     |
| fileextensions | Validates a menu if the given file's extension **does not** match the file extension selected by the user.<br>If multiple file extensions are specified, **no extension** must match the selected files for the validation to be successful. |
| pattern        | Validates a menu if the selected file or directory **does not** match the wildcard pattern matching algorithm.<br>If multiple patterns are specified, **no pattern** must match the selected files for the validation to be successful.      |
| exists         | Validates a menu if the selected file or directory **does not** exists.<br>If multiple files/directories are specified, **all values** must _not exists_ on the system for the validation to be successful.                                  |
| properties     | Validates a menu if the specified property is **empty** or **not defined**.<br>If multiple properties are specified, **all properties** must be _empty_ or _not defined_ for the validation to be successful.                                |
| exprtk         | Validates a menu if the given string expression **does not** evaluates to logical _true_.<br>If multiple expressions are specified, **no expression** must evaluate to logical _true_ for the validation to be successful.                   |
| istrue         | Validates a menu if the given value **does not** evaluates to logical _true_.<br>If multiple values are specified, **no value** must evaluate to logical _true_ for the validation to be successful.                                         |
| isfalse        | Validates a menu if the given value **does not** evaluates to logical _false_.<br>If multiple values are specified, **no value** must evaluate to logical _false_ for the validation to be successful.                                       |
| isempty        | Validates a menu if the given text **is not** empty. If the attribute is not specified, then the validation is successful. Otherwise, if text is specified, the expanded value **must not** be empty.                                        |

Typical use case of the `inverse` attribute is about filtering out known file extensions.

For examples:



#### Split File and Join File menus: ####

* A  `split file`  menu accept only a single file. When executed, it split the selected file `foo.dat` into multiple files of which the first "part" has the file extension  `001`. For example, `foo.dat.001`, `foo.dat.002`, `foo.dat.003` and `foo.dat.004`.
-   A  `join files`  menu accept all files that matches  `fileextensions="001"`

To prevent the `split file` menu to be displayed when user selects a *.001 file (we don't need to split a file twice), the menu should also ***not*** accept files that matches `fileextensions="001"`.

In the end, the `split file` menu validity should be:
1. accept only a **single file**.
2. accept files which file extension **is not** `001`.

The validity element should look like this:
```xml
<validity maxfiles="1" maxfolders="0" fileextensions="001" inverse="fileextensions" />
```



#### Compress menu: ####

A `Compress File` menu which main action compresses the selected files should not be available when one (or more) of the selected files is already compressed. 

The validity element should look like this:
```xml
<validity maxfolders="0" fileextensions="zip;7z;bz2;gz;gzip;rar;tar" inverse="fileextensions" />
```



#### More examples: ####

* `<validity maxfiles="1" maxfolders="0" inverse="maxfiles" />`, valid when _**more than**_ one file is selected.
* `<validity properties="process.started" inverse="properties" />`, valid when a process **_is not_** started  (assuming the property `process.started` is set when the process is started.
* `<validity exists="${env.USERPROFILE}\config.ini" inverse="exists"  />`, valid only when the user's configuration file **_is not_** yet created.



### Combining multiple &lt;visibility&gt; and &lt;validity&gt; elements ###

Multiple &lt;visibility&gt; or &lt;validity&gt; elements can be added under a &lt;menu&gt; element. The logical `or` operator is used with each element of the same type. In other words, &lt;visibility&gt; elements are evaluated together and so are &lt;validity&gt; elements. This feature allows combining elements to achieve more complex validation. 

For example, the following set a menu visible if only a single file is selected ***or*** if only a single directory is selected:
```xml
<visibility maxfiles="1" maxfolders="0" />
<visibility maxfiles="0" maxfolders="1" />
```

The following set a menu visible if the property `notepad++.found` is set ***or*** if the file `C:\Windows\System32\notepad.exe` exists:
```xml
<visibility properties="notepad++.found" />
<visibility exists="C:\Windows\System32\notepad.exe" />
```

The following set a menu visible if the selected filenames begin with `IMG`, `DSC`, `CSC` ***or*** `GP`:
```xml
<visibility pattern="*\IMG*.???" />
<visibility pattern="*\DSC*.???" />
<visibility pattern="*\CSC*.???" />
<visibility pattern="*\GP*.???" />
```




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


*_Hint_*: you can use Windows own icons to assign your &lt;icon&gt; with familiar Windows icons.
The following links shows a visual representation and the index of each icons:
* [Windows  7 icons in shell32.dll](https://help4windows.com/windows_7_shell32_dll.shtml)
* [Windows  7 icons in imageres.dll](https://help4windows.com/windows_7_imageres_dll.shtml)
* [Windows  8 icons in shell32.dll](https://help4windows.com/windows_8_shell32_dll.shtml)
* [Windows  8 icons in imageres.dll](https://help4windows.com/windows_8_imageres_dll.shtml)
* [Windows 10 icons in shell32.dll](https://help4windows.com/)
* [Windows 10 icons in imageres.dll](https://help4windows.com/)




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



#### verb attribute: ####

The `verb` attribute defines special directives on how to execute a file or launching the application. For example, the verb  `open` or `edit` allows the user to open a document using the associated application. The attribute is optional.

Verbs are specific to a file type but some are supported by multiple types. Commonly available verbs include:
| Verb       | Description                                                                                                                                                                                                             |
|------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| edit       | Launches an editor and opens the document for editing.                                                                                                                                                                  |
| find       | Initiates a search starting from the executed directory.                                                                                                                                                                |
| open       | Launches an application. If this file is not an executable file, its associated application is launched.                                                                                                                |
| print      | Prints the document file.                                                                                                                                                                                               |
| properties | Displays the object's properties.                                                                                                                                                                                       |
| runas      | Launches an application as Administrator. User Account Control (UAC) will prompt the user for consent to run the application elevated or enter the credentials of an administrator account used to run the application. |

For example, the following launches `notepad.exe` and open the text file `C:\Windows\System32\drivers\etc\hosts` which can only be modified with elevated privileges (as an Administrator) :
```xml
<exec path="C:\Windows\notepad.exe" arguments="C:\Windows\System32\drivers\etc\hosts" verb="runas" />
```
To get extended information about verbs, see the following Microsoft documentation article: [ShellExecute and ShellExecuteEx, Object Verbs](https://docs.microsoft.com/en-us/windows/win32/shell/launch#object-verbs).



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



### &lt;message&gt; action ###

The &lt;message&gt; element is used to display a message to the user. The &lt;message&gt; element must be added under the &lt;actions&gt; element.

The &lt;message&gt; elements have the following attributes:



#### title attribute: ####

The `title` attribute defines the text of the message displayed to the user.

For example, the following show a confirmation message to the user before opening a file :
```xml
<message title="Opening document ${selection.path}. Press OK to continue." />
```



#### caption attribute: ####

The `caption` attribute defines the caption of the window that displays the message.

For example, the following show a status message to the user :
```xml
<message caption="Status" title="The Operation Completed Successfully." />
```



#### icon attribute: ####

The `icon` attribute defines the type of icon for the message. The attribute is optional.
If the `icon` attribute is not specified, an information icon is displayed.

The following table shows the attribute supported values:

| Values      | Description                                                                             |
|-------------|-----------------------------------------------------------------------------------------|
| Asterisk    | Show an icon consisting of a lowercase letter i in a circle.                            |
| Exclamation | Show an icon with an exclamation point in a triangle with a yellow background.          |
| Hand        | Show an icon consisting of a white X in a circle with a red background.                 |
| Information | Show an icon consisting of a lowercase letter i in a circle (default option).           |
| Warning     | Show an icon consisting of an exclamation point in a triangle with a yellow background. |
| None        | No icon is displayed.                                                                   |

For example, the following show a warning message :
```xml
<message caption="Message to user" title="Do not press the X button before the download is completed." icon="warning" />
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

The `value` attribute defines the new value of the given property.

For example, the following set the property `myprogram.user.name` to value `Al Coholic` :
```xml
<property name="myprogram.user.fullname" value="Al Coholic" />
```



#### exprtk attribute: ####

The `exprtk` attribute defines an expression that is evaluated to set a new value for the given property. The expression must be specified as a mathematical expression and the result must evaluates to an integer or a floating point value such as `4+9` or `${foo}+1`.

The `exprtk` attribute can also be set to an expression that evaluates to `true` or `false` and logical `and` and `or` operators can be use to group expressions. eg: `${foo.count} > 1 and '${foo.state}'=='PAUSED'`.

The attribute supports the following operators: 
* Basic operators: `+`, `-`, `*`, `/`, `%`, `^`
* Equalities & Inequalities: `=`, `==`, `<>`, `!=`, `<`, `<=`, `>`, `>=`
* Logic operators: `and`, `not`, `or`, `xor`, `true`, `false`
* String operators: `in`, `like`, `ilike`, []

Strings may be comprised of any combination of letters, digits special characters including (~!@#$%^&*()[]|=+ ,./?<>;:"_) or hexadecimal escaped sequences (eg: \0x30) and must be enclosed with single-quotes.
eg: `'Frankly my dear, \0x49 do n0t give a damn!'`

The `exprtk` attribute allows advanced property evaluation. The following table show useful expression examples:

| Use cases                                                                               | Expression                                                                                                                                                                                                                                                                                                                                                                                                 | Meaning                                                                                                                                                                                 |
|-----------------------------------------------------------------------------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Update a property using a generic equation                                              | \${myvalue}^2 + 5*\${myvalue} + 100                                                                                                                                                                                                                                                                                                                                                                        | Evaluate the quadratic equation `x^2 + 5x + 100` where `x` is equal to the value of property `myvalue`.                                                                                 |
| Create a counter using properties.<br>(with a default property value set)               | \<property name="mycounter" exprtk="${mycounter}+1"\>                                                                                                                                                                                                                                                                                                                                                      | The property update itself by increasing its own value by 1.<br>Note: this only work if the property is defined to a numeric value first.                                               |
| Create a counter using properties.<br>(without having to initialize the property first) | if ('\${mycounter}' == '$'+'{mycounter}' or '\${mycounter}' == '0') 1;<br>else if ('\${mycounter}' == '1') 2;<br>else if ('\${mycounter}' == '2') 3;<br>else if ('\${mycounter}' == '3') 4;<br>else if ('\${mycounter}' == '4') 5;<br>else if ('\${mycounter}' == '5') 6;<br>else if ('\${mycounter}' == '6') 7;<br>else if ('\${mycounter}' == '7') 8;<br>else if ('\${mycounter}' == '8') 9;<br>else 10; | Increase the value of property `mycounter` by `1` going from `1` up to `10`.<br>On the first call, the first line of the expression detects if the property is unset and set it to `1`. |
| Get the length of a property value.                                                     | '\${command}'[]                                                                                                                                                                                                                                                                                                                                                                                            | Set the property to the length of the `command` property value.                                                                                                                         |
| Set a property to logical `true` or `false`.                                            | ${foo} == 2 or ${bar} >= 5                                                                                                                                                                                                                                                                                                                                                                                 | The property will be set to value `1` if the expression evaluates to `true` <br>and set to `0` if the expression evaluates to `false`.                                                  |


**Note:**
The `exprtk` attribute uses the *exprtk library* to parse the expression. For more details and supported expressions, see the exprtk documentation on the [official github page](https://github.com/ArashPartow/exprtk) or the [library website](http://www.partow.net/programming/exprtk/index.html).



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



#### encoding attribute: ####

The `encoding` attribute defines the encoding of the text file. The `encoding` attribute supports the following values: `utf-8`, `ansi` and `unicode`. The default value for the encoding is `utf-8`.

For example, the following action creates a new text file encoded in ANSI format, named `selection.txt` in the user's HOME directory :
```xml
<file path="${env.USERPROFILE}\selection.txt" encoding="ansi">${selection.path}</file>
```



### &lt;stop&gt; action ###

The &lt;stop&gt; element is used to stop the action execution sequence when a validation fails. The &lt;stop&gt; element supports dynamic properties and can be used to validate user entered data. The &lt;stop&gt; element must be added under the &lt;actions&gt; element.

The &lt;stop&gt; elements have the same attributes as the [&lt;visibility&gt;](#visibility--validity) or [&lt;validity&gt;](#visibility--validity) elements. Namely, one can stop the action execution sequence using [properties](#properties-attribute), [exprtk](#exprtk-attribute), [istrue](#istrue-attribute), [isfalse](#isfalse-attribute) and [inverse](#inverse-attribute) attributes.

For example, the following actions ask for an answer to a question and stop the action execution progress if the user answer _No_ :
```xml
<prompt type="yesno" name="continue" title="Are you sure you want to continue with the operation?"
        valueyes="true" valueno="false" />
<stop isfalse="${continue}" />
<exec path="..." />
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
      <property name="compare.first.selection.path" value="${selection.path}" />
    </actions>
  </menu>
  <menu name="Compare with first file">
    <actions>
      <!--Store this file into a property for convenience reason.-->
      <property name="compare.second.selection.path" value="${selection.path}" />

      <!-- run application that compares file "${compare.first.selection.path}" with "${compare.second.selection.path}" -->
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

| Property                     | Description                                                                                                                                                                             |
|------------------------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| selection.path               | Matches the full path of the clicked element.                                                                                                                                           |
| selection.dir                | Matches the directory of the clicked element. When selecting a file, matches the directory of the clicked element. When selecting a directory, matches the path of the clicked element. |
| selection.dir.count          | Matches the number of files and directories in directory `selection.dir`.                                                                                                               |
| selection.dir.empty          | Set to `true` when directory `selection.dir` is empty. Set to `false` otherwise.                                                                                                        |
| selection.filename           | Matches the filename of the clicked element.                                                                                                                                            |
| selection.filename.noext     | Matches the filename of the clicked element without the file extension.                                                                                                                 |
| selection.parent.path        | Matches the full path of the parent element.                                                                                                                                            |
| selection.parent.filename    | Matches the filename of the parent element.                                                                                                                                             |
| selection.filename.extension | Matches the file extension of the clicked element.                                                                                                                                      |
| selection.drive.letter       | Matches the drive letter of the clicked element. For example `C`.                                                                                                                       |
| selection.drive.path         | Matches the drive path of the clicked element. For example `C:\`.                                                                                                                       |
| selection.count              | Matches the number of clicked elements (files and directories).                                                                                                                         |
| selection.files.count        | Matches the number of clicked files.                                                                                                                                                    |
| selection.directories.count  | Matches the number of clicked directories.                                                                                                                                              |
| selection.mimetype           | Matches the [MIME type](https://en.wikipedia.org/wiki/Media_type) of the selected file. [See below](#mime-types-general-description-and-charset) for examples.                          |
| selection.description        | Matches a general description of the file's content. See below for examples.                                                                                                            |
| selection.charset            | Matches the [Character Set](https://www.w3schools.com/html/html_charset.asp) of the file. [See below](#mime-types-general-description-and-charset) for examples.                        |

Selection-based properties are encoded in utf-8.

**Notes:**
Properties `selection.drive.letter` and  `selection.drive.path` are empty when user select files from a network share.

Properties `selection.dir.count` and  `selection.dir.empty` are empty when multiple elements are selected.

The properties `selection.mimetype`, `selection.description` and `selection.mimetype` are based on the content of the selected file. The properties are provided by the *File* and *Libmagic* libraries to extract information about selected files. For more details, see the documentation at [github.com/Cirn09/file-windows](https://github.com/Cirn09/file-windows), [github.com/file/file](https://github.com/file/file) or the [official file documentation](http://www.darwinsys.com/file/).


### MIME types, general description and charset: ###

A MIME type is a label used to identify a type of data. It is used so software can know how to handle the data. It serves the same purpose on the Internet that file extensions do on Microsoft Windows. A MIME type is calculated from the actual content in a file.

A MIME type has two parts: a **type** and a **subtype**. They are separated by a slash character (`/`). For example, the MIME type for *Microsoft Word* files is *application* and the subtype is *msword*. Together, the complete MIME type is `application/msword`.

The following table shows examples values for different type of content :

| Type of content                                                                                                                                 | MIME type             | Character set | Description                                                                                                                              |
|-------------------------------------------------------------------------------------------------------------------------------------------------|-----------------------|---------------|------------------------------------------------------------------------------------------------------------------------------------------|
| [This image](https://github.com/end2endzone/ShellAnything/blob/0.6.1/docs/screenshot_file.png) in png format.                                   | image/png             | binary        | PNG image data, 614 x 668, 8-bit/color RGBA, non-interlaced                                                                              |
| [This image](https://github.com/end2endzone/ShellAnything/blob/0.6.1/docs/ShellAnything-splashscreen.jpg) in jpeg format.                       | image/jpeg            | binary        | JPEG image data, JFIF standard 1.01, resolution (DPI), density 72x72, segment length 16, progressive, precision 8, 853x480, components 3 |
| [This text file](https://github.com/end2endzone/ShellAnything/blob/0.6.1/CMakeLists.txt)                                                        | text/plain            | us-ascii      | ASCII text, with CRLF line terminators                                                                                                   |
| ShellAnything's [default.xml](https://github.com/end2endzone/ShellAnything/blob/0.6.1/resources/configurations/default.xml) Configuration file. | text/xml              | utf-8         | XML 1.0 document, UTF-8 Unicode text, with CRLF line terminators                                                                         |
| ShellAnything's unit test executable.                                                                                                           | application/x-dosexec | binary        | PE32+ executable (console) x86-64, for MS Windows                                                                                        |
| [Media Player Classic](https://github.com/mpc-hc/mpc-hc) executable.                                                                            | application/x-dosexec | binary        | PE32 executable (GUI) Intel 80386, for MS Windows                                                                                        |
| An MP3 audio file                                                                                                                               | audio/mpeg            | binary        | MPEG ADTS, layer III, v1, 160 kbps, 44.1 kHz, Monaural                                                                                   |


### How to get my files MIME types, general description and charset ? ###

ShellAnything's is bundle with *file.exe* which is an application that is able to print a file's MIME type, charset or description. Execute the following commands to get the MIME type of a given file:
1. Open a *Command Prompt*.
2. Navigate to ShellAnything's installation folder: `cd /d C:\Program Files\ShellAnything [version]\bin`.
3. To get the file's MIME type, enter the command `file --mime-type <path_of_file>`.
4. To get the file's encoding (charset), enter the command `file --mime-encoding <path_of_file>`.
5. To get a general description of the content of the file, enter the command `file <path_of_file>`.


Another option is to create your own *Configuration* and add the following menu that shows these values. For example:
```xml
  <menu name="Show MIMEtype, description and charset">
    <visibility maxfiles="1" maxfolders="0" />
    <actions>
       <message caption="MIMEType debugger" title="selection.mimetype=${selection.mimetype}${line.separator}selection.charset=${selection.charset}${line.separator}selection.description=${selection.description}" />
    </actions>
  </menu>
``` 
Press `CTRL+C` when the message is shown to copy the values to the clipboard.



## Multi-selection-based properties ##

If multiple files are selected, the application generates the same properties but combines the selected elements on multiple lines. In other words, each selected files are separated by the string `\r\n` (carriage return). 

For example, assume the following files are selected:
* C:\Program Files (x86)\Winamp\libFLAC.dll
* C:\Program Files (x86)\Winamp\winamp.exe
* C:\Program Files (x86)\Winamp\zlib.dll

The system will generates the following property values (note the `\r\n` characters) :
| Property                     | Value                                                                                                                               |
|------------------------------|-------------------------------------------------------------------------------------------------------------------------------------|
| selection.path               | C:\Program Files (x86)\Winamp\libFLAC.dll`\r\n`C:\Program Files (x86)\Winamp\winamp.exe`\r\n`C:\Program Files (x86)\Winamp\zlib.dll |
| selection.dir                | C:\Windows\System32`\r\n`C:\Windows\System32`\r\n`C:\Windows\System32`\r\n`|
| selection.filename           | libFLAC.dll`\r\n`winamp.exe`\r\n`zlib.dll     
| selection.filename.noext     | libFLAC`\r\n`winamp`\r\n`zlib                                                                                                       |
| selection.parent.path        | C:\Program Files (x86)\Winamp`\r\n`C:\Program Files (x86)\Winamp`\r\n`C:\Program Files (x86)\Winamp                                 |
| selection.parent.filename    | Winamp`\r\n`Winamp`\r\n`Winamp                                                                                                      |
| selection.filename.extension | dll`\r\n`exe`\r\n`dll                                                                                                               |
| selection.drive.letter       | C`\r\n`C`\r\n`C                                                                                                                     |
| selection.drive.path         | C:\\`\r\n`C:\\`\r\n`C:\\                                                                                                            |

**Notes:**
* Properties `selection.drive.letter` and  `selection.drive.path` are empty when all selected files are from a network share.
* Properties `selection.dir.count` and  `selection.dir.empty` are empty when multiple files are selected.
* Properties `selection.count`, `selection.files.count` and `selection.directories.count` are not multi-selection-based properties. They are defined as a single value whether a single or multiple elements are selected.



### selection.multi.separator property ###

If you need more flexibility when dealing with multiple files, the system defines the property `selection.multi.separator` that allows customizing the separator when combining multiple files.

By default, this property is set to the value `\r\n` (new line)  when the application initialize.

The property can be modified at any time using a [&lt;property&gt;](#property-action) action for changing the property when a menu is executed or with the  [&lt;default&gt;](#default) element to change the value globally (when the `Configuration File` is loaded).

To reset the property back to the default value, use the following &lt;property&gt; action:
```xml
<property name="selection.multi.separator" value="${line.separator}" />
``` 

**Example #1:**

If an executable must get the list of selected files in a single command line (one after the other), one can set the property `selection.multi.separator` to  `" "` (double quote, space, double quote) and use the string `"${selection.path}"` (including the double quotes) to get the required expanded value:

```xml
<actions>
  <property name="selection.multi.separator" value="&quot; &quot;" />
  <exec path="myprogram.exe" arguments="&quot;${selection.path}&quot;" />
  <property name="selection.multi.separator" value="${line.separator}" />
</actions>
```
Which result in the following expanded value:
```
"C:\Program Files (x86)\Winamp\libFLAC.dll" "C:\Program Files (x86)\Winamp\winamp.exe" "C:\Program Files (x86)\Winamp\zlib.dll"
```
**Example #2:**

Assume that you want to run a specific command on each of the selected files (for example reset the files attributes), one can set the property `selection.multi.separator` to  `${line.separator}attrib -r -a -s -h "` (including the ending double quote character) and use the string `attrib -r -a -s -h "${selection.filename}"` (including the double quotes) to get the following expanded value:
```
attrib -r -a -s -h "C:\Program Files (x86)\Winamp\libFLAC.dll"
attrib -r -a -s -h "C:\Program Files (x86)\Winamp\winamp.exe"
attrib -r -a -s -h "C:\Program Files (x86)\Winamp\zlib.dll"
```
The result can then be used to:
* Copy the commands to the clipboard:

```xml
<clipboard  value="attrib -r -a -s -h &quot;${selection.filename}&quot;"  />
```

* Create a batch file which content is the commands:
```xml
<file path="${env.TEMP}\reset_attribs.bat">@echo off
attrib -r -a -s -h &quot;${selection.filename}&quot;
</file>
```




## Fixed properties ##

The application defines a list of properties about the current runtime. The values of these properties are constant (fixed) and do not change.

The following table defines the list of fixed properties and their utility:

| Property              | Description                                              |
|-----------------------|----------------------------------------------------------|
| application.path      | Matches the full path of the shell extension DLL.        |
| application.directory | Matches the directory of the shell extension.            |
| log.directory         | Matches the directory where the logs are created.        |
| config.directory      | Matches the directory of the configuration files.        |
| home.directory        | Matches the home directory of the current user.          |
| path.separator        | Matches the `\` character.                               |
| line.separator        | Matches the `\r\n` string.                               |
| newline               | Matches the `\r\n` string. Same as `${line.separator}`.  |


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
        <exec path="${env.ComSpec}" basedir="${selection.path}" />
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




# Other #



## Mnemonics / keyboard shortcuts: ##

Menu names supports [keyboard mnemonics](https://en.wikipedia.org/wiki/Mnemonics_(keyboard)), a.k.a *Access Keys*, *Keyboard shortcuts* or *Keybindings*.  A mnemonic indicates to the user which key to press (often in conjunction with the Alt key) to activate a menu and execute its associated actions. They provides a method of quickly navigating a menu using the only the keyboard.  

To assign a specific key to a menu,  add an ampersand character (`&`) before the character of the associated keyboard key.

For example, the following would create a menu that is activated when one presses the `z` key:
```xml
<menu name="Press the &z key to execute this menu" >
  <actions>
    <message title="It works!" />
  </actions>
</menu>
```

The `&` character is a special character in menu names and must be escaped to be used as a plain text character. To escape an `&` character, one must double the character.

For example, the following would create a menu named `Smith&Co` :
```xml
<menu name="Smith&&Co" />
```
Note the use of the double `&` which prevents creating a keyboard shortcut with the `C` key.

**Note:** By default, mnemonics are not activated on Windows. Please read [this SuperUser article]([https://superuser.com/questions/16952/how-to-enable-underscore-shortcut-mnemonics-for-menu-items](https://superuser.com/questions/16952/how-to-enable-underscore-shortcut-mnemonics-for-menu-items)) to know how to activate them on Windows.




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
|    name    | Matches the name of the dll that generates log messages. Usually `sa.shellextension.dll`.                                                                             |
|    LEVEL   | The logging level. Accepted values are `ERROR`, `WARNING` and `INFO`. See [glog documentation](https://github.com/google/glog/blob/v0.4.0/doc/glog.html) for details. |
|    date    | The current date when the file was created in `yyyymmdd` format.                                                                                                      |
|    time    | The current date when the file was created in `hhmmss` format.                                                                                                        |
|     pid    | The Process Id which generates the log message.                                                                                                                       |

For example, `sa.shellextension.dll.INFO.20191125-083305.9456.log` is a valid log filename.



### Levels ###

The logging library used by ShellAnything is [Google's logging module (glog)](https://github.com/google/glog). 

All messages logged by the application have an associated level to describe the severity of the message.

The application supports 3 differents logging levels: `ERROR`, `WARNING` and `INFO`. See [glog documentation](https://github.com/google/glog/blob/v0.4.0/doc/glog.html) for details.

Each level have a dedicated log file for storing messages. For example:
```
sa.shellextension.dll.INFO.20191125-083305.9456.log
sa.shellextension.dll.WARNING.20191125-083701.9456.log
sa.shellextension.dll.ERROR.20191125-084152.9456.log
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



## Missing ampersand character (`&`) in menus ##

One might be puzzled as to why his menus do not display ampersand character (`&`) properly.

For example, if one create a menu with the name set to `Smith&Co`,  ShellAnything will render the menu as `SmithCo` or "Smith<u>C</u>o". This is beause the `&` character is a special character used in keyboard mnemonics. 

Because the `&` character is a special character in menu names, it must be escaped to be used as a plain character. To escape an `&` character, one must double the character.

Refer to the [keyboard mnemonics](#mnemonics--keyboard-shortcuts) section for details.



## Reporting bugs ##

If you find any issues while using ShellAnything, please report your findings using the [project issue page on GitHub](https://github.com/end2endzone/ShellAnything/issues).

Improvements to the software are also welcome and can be proposed using the same method.
