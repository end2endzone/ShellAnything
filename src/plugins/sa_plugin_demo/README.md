# sa_plugin_demo #
_sa_plugin_demo_ is a demo plugin library for [ShellAnything](https://github.com/end2endzone/ShellAnything). It can be used as a base code for new plugins. This demo provides new validation attributes called `demo1` and `demo2` for showing or enabling menus.



# Usage #

Declare the plugin as the following:
```xml
<plugins>
  <plugin path="${application.path}\sa_plugin_demo.dll"
          conditions="start_time;end_time"
          description="This plugin defines new visibility/validity attributes." />
</plugins>
```

# Building #

This section explains how to compile _sa_plugin_demo_ example.

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

Please refer to [install_shellanything_plugins.bat](https://github.com/end2endzone/ShellAnything/blob/master/ci/windows/install_shellanything_plugins.bat) for details about _sa_time_plugin_ plugin.
