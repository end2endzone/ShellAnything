# sa_time_plugin #
sa_time_plugin is a plugin library for [ShellAnything](https://github.com/end2endzone/ShellAnything). It provides new validation attributes called `start_time`, `end_time` for showing or enabling menus based on the current system time.



# Usage #

Declare the plugin in a Configuration File as the following:
```xml
<plugins>
  <plugin path="${application.path}\sa_time_plugin.dll"
          conditions="start_time;end_time"
          description="This plugin defines new visibility/validity attributes.
                       The validation is based on current system time.
                       Values should be specifed in hh:mm format." />
</plugins>
```



# Building #

This section explains how to compile sa_time_plugin example.

To build the project, execute the following steps:

1) Open a command prompt and navigate to the plugin source directory.
2) Enter the following commands:

```cmake
mkdir build
cd build
cmake -DCMAKE_GENERATOR_PLATFORM=x64 -DCMAKE_INSTALL_PREFIX="path\to\plugin\installation\directory" -DCMAKE_PREFIX_PATH="path\to\shellanything\installation\directory" ..
cmake --build . --config Release -- -maxcpucount /m
cmake --build . --config Release --target INSTALL
cmake  ..
```

Please refer to [install_shellanything_plugins.bat](https://github.com/end2endzone/ShellAnything/blob/master/ci/windows/install_shellanything_plugins.bat) for details about sa_time_plugin plugin.
