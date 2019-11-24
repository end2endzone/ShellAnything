# Install #

The application does not provides pre-build binaries or provides an installation package. To install the binary files on the system, the source code must be compiled and copied to the appropriate directory.

The following steps show how to install the application:

1) Download the source code from an existing [tags](http://github.com/end2endzone/ShellAnything/tags) and extract the content to a local directory (for example `c:\projects\ShellAnything`).

2) Build the source code according to the [Build Steps](#build-steps) instructions specified in this document.

3) Navigate to the `build` directory and execute `cmake --build . --config Release --target INSTALL`.

# Build #

This section explains how to compile and build the software and how to get a development environment ready.



## Prerequisites ##


### Software Requirements ###
The following software must be installed on the system for compiling source code:

* [Google C++ Testing Framework v1.8.0](https://github.com/google/googletest/tree/release-1.8.0)
* [Google Logging Library v0.4.0](https://github.com/google/glog/tree/v0.4.0)
* [TinyXML 2 v6.2.0](https://github.com/leethomason/tinyxml2/tree/6.2.0)
* [RapidAssist v0.7.0](https://github.com/end2endzone/RapidAssist/tree/0.5.0)
* [win32Clipboard v0.2.0](https://github.com/end2endzone/win32Clipboard/tree/0.1.0)
* [CMake](http://www.cmake.org/) v3.4.3 (or newer)



### Windows Requirements ###

* Microsoft Visual C++ 2010 or newer



## Build steps ##

The project uses the CMake build system to generate a platform-specific build environment. CMake reads the CMakeLists.txt files that you'll find throughout the directories, checks for installed dependencies and then generates files for the selected build system.

To build the software, execute the following steps:

1) Get a copy of the source code by using one of the following methods:
   * Download the source code of the project from an existing [tags](http://github.com/end2endzone/ShellAnything) and extract the downloaded zip file to a local directory (for example `c:\projects\ShellAnything` or `~/dev/ShellAnything`).
   * Clone the github repository by running `git clone "http://github.com/end2endzone/ShellAnything"`.

2) Generate the project files for your build system. From your source code directory, enter the following commands:
```
mkdir build
cd build
cmake ..
```

Note:
See [CMake Quick Tips](#cmake-quick-tips) section for more details on building the library.

3) Build the source code. Run `cmake --build . --config Release` or open the generated `.sln` file with Visual Studio.



## CMake Quick Tips ##

The following section explains typical issues that you might run into when trying to build the project.

### CMake default install directory requires administrator rights ###

If you do want to specify a custom directory where this project should be installed, you can provide an extra option to CMake using the property `CMAKE_INSTALL_PREFIX`. The property must be specified at CMake configuration time. For example, the command `cmake -DCMAKE_INSTALL_PREFIX=c:\projects\install ..` will install the project binaries into directory `c:\projects\install\[NAME OF THE PROJECT]`.

If you do not specify a custom install directory, CMake will install the binary files into its default location, which differs from system to system.

**On Windows**, the default CMake installation directory is `C:\Program Files (x86)\[NAME OF THE PROJECT]` which requires administrator privileges.

**On Linux**, the default CMake installation directory is `/usr/local/[NAME OF THE PROJECT` and also requires administrator privileges (sudo).



### Tell CMake where to find depdendencies ###

If you installed your library dependencies into a directory other than CMake's default installation directory, CMake will not be able to find your dependencies automatically.

For example, on Windows, the CMake default installation directory is `C:\Program Files (x86)\${PROJECT_NAME}`. This makes the installation directory different for each dependency and create unnecessary complication when using CMake.

There are 2 different ways to tell CMake where to look for when searching for libraries:

1) **Using environment variables.**

The installation path of specific dependency can be specified manually by defining the environment variable `[NAME OF DEPENDENCY]_DIR` to the right installation directory. For instance, to specify the installation directory of [rapidassist](https://github.com/end2endzone/RapidAssist) dependency, one can define the `rapidassist_DIR` environment variable which will instruct CMake to look at this location for already installed libraries. By specifying the install directory manually, the `find_package()` command will be able to resolve the path of the dependency and locate the include directory and library files.

A different environment variable must be specified for all required dependencies.


2) **Define the `CMAKE_PREFIX_PATH` property.**

The installation path of all dependencies can be specified manually by defining the [CMAKE_PREFIX_PATH](https://cmake.org/cmake/help/v3.0/variable/CMAKE_PREFIX_PATH.html) property. This property must be set at configuration time. For example, the command `cmake -DCMAKE_PREFIX_PATH=c:\dependencies\install ..` will instruct CMake to use `c:\dependencies\install` directory (and subdirectories) while searching for installed dependency binaries.

You can provide multiple path in `CMAKE_PREFIX_PATH` property by delimiting each individual path using `;` (semicolon). See [this Stack Overflow article](https://stackoverflow.com/questions/32302289/how-to-put-several-paths-in-cmake-prefix-path/32302398) for details.



### Install project binaries and dependencies in same directory ###

For some project, it may be a better option to install the project and all its dependencies into the same directory. This is a convenient option when one wants to deploy an application without relying on the system's pre-installed binaries. The runtime environment stays the same even if the user update system libraries. For instance, [Portable Apps](https://portableapps.com/) deploys application using this method: all required binaries are included in the deployed package.

To use such a configuration, all dependencies must be reinstalled to a common directory. To follow best practices, it is suggested to re-download all dependencies, extract the sources to a new temporary directory and recompile all of them to make sure that no system library is used while compiling.

Users should define the property `CMAKE_INSTALL_PREFIX` of each dependency to the common installation directory (identical across all dependencies) and recompile all dependencies. The property must be specified at CMake configuration time. For example, the command `cmake -DCMAKE_INSTALL_PREFIX=c:\projects\install ..`.
 
With this method, users *do not* have to define environment variable such as `[NAME OF DEPENDENCY]_DIR` or [CMAKE_PREFIX_PATH](https://cmake.org/cmake/help/v3.0/variable/CMAKE_PREFIX_PATH.html) property. CMake will be able to locate all library files since the property `CMAKE_INSTALL_PREFIX` is one of the directories that is searched when CMake is looking for a dependency.



## Build options ##

The following table shows the available build option supported:

| Name                         | Type   |         Default         | Usage                                                      |
|------------------------------|--------|:-----------------------:|------------------------------------------------------------|
| CMAKE_INSTALL_PREFIX         | STRING | See CMake documentation | Defines the installation folder of the library.            |
| BUILD_SHARED_LIBS            | BOOL   |           OFF           | Enable/disable the generation of shared library makefiles  |
| SHELLANYTHING_BUILD_TEST     | BOOL   |           OFF           | Enable/disable the generation of unit tests target.        |
| SHELLANYTHING_BUILD_DOC      | BOOL   |           OFF           | Enable/disable the generation of API documentation target. |

To enable a build option, run the following command at the cmake configuration time:
```cmake
cmake -D<BUILD-OPTION-NAME>=ON ..
```




# Testing #
ShellAnything comes with unit tests which help maintaining the product stability and level of quality.

Test are build using the Google Test v1.8.0 framework. For more information on how googletest is working, see the [google test documentation primer](https://github.com/google/googletest/blob/release-1.8.0/googletest/docs/V1_6_Primer.md).  

Test are disabled by default and must be manually enabled. See the [Build Options](#build-options) for details on activating unit tests.

Test are automatically build when building the solution.

To run tests, navigate to the `build/bin` folder and run `shellanything_unittest` executable. For Windows users, the executable is located in `build\bin\Release`.

Test results are saved in junit format in file `shellanything_unittest.x86.debug.xml` or `shellanything_unittest.x86.release.xml` depending on the selected configuration.

The latest test results are available at the beginning of the [README.md](README.md) file.


