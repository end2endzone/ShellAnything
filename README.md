![ShellAnything logo](docs/ShellAnything-splashscreen.jpg?raw=true)


# ShellAnything #
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Github Releases](https://img.shields.io/github/release/end2endzone/shellanything.svg)](https://github.com/end2endzone/shellanything/releases)

ShellAnything is a C++ open-source software which allow one to easily customize and add new options to *Windows Explorer* context menu. Define specific actions when a user right-click on a file or a directory.




## Status ##

Build:

| Service | Build | Tests |
|----|-------|-------|
| AppVeyor | [![Build status](https://img.shields.io/appveyor/ci/end2endzone/ShellAnything/master.svg?logo=appveyor)](https://ci.appveyor.com/project/end2endzone/shellanything) | [![Tests status](https://img.shields.io/appveyor/tests/end2endzone/shellanything/master.svg?logo=appveyor)](https://ci.appveyor.com/project/end2endzone/shellanything/branch/master/tests) |


Statistics:

[![Statistics](https://buildstats.info/appveyor/chart/end2endzone/shellanything)](https://ci.appveyor.com/project/end2endzone/shellanything/branch/master)




# Screenshots #

**Files context menu:**

![ShellAnything screenshot](docs/screenshot_file.png?raw=true)


**Directories context menu:**

![ShellAnything screenshot](docs/screenshot_folder.png?raw=true)




# Purpose #

Adding new options to Windows Explorer context menu can be done in many ways. One can create new context menus by [manually editing the registry](https://www.groovypost.com/howto/add-any-program-windows-context-menu/) but this option is limited in support (static text, no submenus). If you are technical person, you can also follow [this guide](https://stackoverflow.com/questions/20449316/how-add-context-menu-item-to-windows-explorer-for-folders) on StackOverflow.

With Shell Extension, you can dynamically create a context menu item (or submenu) that is more relevant to the selected file(s).
Shell extension are dynamic by nature because they are runtime DLL inside Windows Explorer. They can show or hide options based on the selected file's name, size, content, etc.
Shell extensions also have the ability to handle multiple selected files simultaneously.

On the downside, writing shell extensions is complicated, tedious and difficult to debug. 

This is where ShellAnything is useful. It provides a simple xml-based way of adding new dynamic options to Windows Explorer context menu.

ShellAnything is implemented in C++ which, for writing Shell Extension, is a safe programming language compared to C#. See [Writing Shell Extensions with C&#35;](#writing-shell-extensions-with-c) for details.



## Writing Shell Extensions with C&#35;. ##

Writing Shell Extensions with C# is possible but highly discouraged and probably not a good idea.

Many articles already discussed the subject and they all comes to the same conslusion: ***Don't do it.***

For instance, the following articles are good references:
* StackOverflow have discussed the issue rigorously : [StackOverflow - Windows shell extension with C#](https://stackoverflow.com/questions/2194572/windows-shell-extension-with-c-sharp/2194638#2194638).
* Raymond Chen, from **The Old New Thing**, says "[Do not write in-process shell extensions in managed code](https://devblogs.microsoft.com/oldnewthing/20061218-01/?p=28693)" and following the release of version 4 of the .NET Framework (with in-process side-by-side runtimes), "[the answer is still no](https://devblogs.microsoft.com/oldnewthing/?p=5163)".
* Microsoft [Guidance for Implementing In-Process Extensions](https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/dd758089(v=vs.85)) is discouraging the use of C#.




# Features #

The main features of ShellAnything are:

* Easily add custom menus and submenus.
* Dynamic content based on xml files. Change xml configuration files to see the menu options update automatically.
* Support for custom icons for each menu.
* Support activation and visibility filters based on selected files.
* Define actions executed when a menu option is selected:
  * Launch an application.
  * Open a document.
  * Prompt for a value.
  * Change the clipboard.
  * and many more.
* Powerful property-based system for rich dynamic content and filters.
* Easily integrate any third party application with custom menus.
* Supports windows [keyboard mnemonics](https://en.wikipedia.org/wiki/Mnemonics_(keyboard)).



# Usage #

For instructions on how to use ShellAnything, please refer to the [User Manual](UserManual.md).

The User Manual also features *Use Case* with the following examples:

* [Integrate a third party application](UserManual.md#integrate-a-third-party-application).
* [Run an application with parameters](UserManual.md#run-an-application-with-parameters).
* [Open a command prompt in directory](UserManual.md#open-a-command-prompt-in-directory).
* [Select two files for an operation](UserManual.md#select-two-files-for-an-operation).




# Build #

Please refer to file [INSTALL.md](INSTALL.md) for details on how installing/building the application.




# Platform #

ShellAnything has been tested with the following platform:

*   Windows 7 (32 and 64 bit)
*   Windows 10 (32 and 64 bit)

**NOTE:** Version 0.6.1 is the last version of ShellAnything that supports 32-bit Windows.



# Versioning #

This project use [Semantic Versioning 2.0.0](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/end2endzone/ShellAnything/tags).




# Authors #

* **Antoine Beauchamp** - *Initial work* - [end2endzone](https://github.com/end2endzone)

See also the list of [contributors](https://github.com/end2endzone/ShellAnything/blob/master/AUTHORS) who participated in this project.




# License #

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details
