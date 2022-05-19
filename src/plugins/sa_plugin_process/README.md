# sa_plugin_process #
_sa_plugin_process_ is a plugin library for [ShellAnything](https://github.com/end2endzone/ShellAnything). This plugin declares actions and validation to interact with external processes.

This plugin registers custom actions `<terminateprocess>` and `<killprocess>` which can ask an application to exit gracefully or force stop an application. It also defines new validation attributes `process_filename` and `process_pid` for validating a menu based on running processes.

# Usage #

Declare the plugin in a Configuration File as the following:
```xml
<root>
  <plugins>
    <plugin path="${application.directory}\sa_plugin_process.dll"
            actions="terminateprocess;killprocess"
            conditions="process_filename;process_pid"
            description="This plugin declares actions and validation to interract with external processes.
                         It defines the following actions :
                           * `terminateprocess` which ask an application to exit gracefully,
                           * `killprocess`, which force stop an application.
                         and defines the following conditons:
                           * `process_filename`, to validate a menu if a running process filename matches the value,
                           * `process_pid`, to validate a menu if a running process id (pid) matches the value" />
  </plugins>
  <shell>

    <menu name="Terminate Notepad (by filename)">
      <visibility process_filename="notepad.exe" />
      <actions>
        <terminateprocess filename="notepad.exe" />
      </actions>
    </menu>

    <menu name="Terminate Notepad (by pid)">
      <visibility process_pid="${sa_plugin_process.pid}" />
      <actions>
        <terminateprocess pid="${sa_plugin_process.pid}" />
      </actions>
    </menu>

    <menu name="Kill Notepad (by filename)">
      <visibility process_filename="notepad.exe" />
      <actions>
        <killprocess filename="notepad.exe" />
      </actions>
    </menu>

    <menu name="Kill Notepad (by pid)">
      <visibility process_pid="${sa_plugin_process.pid}" />
      <actions>
        <killprocess pid="${sa_plugin_process.pid}" />
      </actions>
    </menu>

  </shell>
</root>
```


# Building #

This section explains how to compile _sa_plugin_process_ example.

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

Please refer to [install_shellanything_plugins.bat](https://github.com/end2endzone/ShellAnything/blob/master/ci/windows/install_shellanything_plugins.bat) for details about _sa_plugin_process_ plugin.
