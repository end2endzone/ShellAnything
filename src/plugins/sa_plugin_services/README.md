# sa_plugin_services #
_sa_plugin_services_ is a plugin library for [ShellAnything](https://github.com/end2endzone/ShellAnything). This plugin allow to query the status of a service. The plugin requires a mandatory property named `sa_plugin_services.names` to be set. For example, to get the service status of _Microsoft Defender Antivirus Network Inspection Service_, the property `sa_plugin_service.names` should be set to `WdNisSvc`. If the service name is found, the property `sa_plugin_services.WdNisSvc.status` is set to the service status.

The status values are: `unknown`, `continuing`, `pausing`, `paused`, `running`, `starting`, `stopping` and `stopped`.

Multiple service names can be specified with the `;` character.


# Usage #

Declare the plugin in a Configuration File as the following:
```xml
<root>
  <plugins>
    <plugin path="${application.directory}\sa_plugin_services.dll"
            description="This plugin allow to query the status of a service.
                         The plugin requires a mandatory property named 'sa_plugin_services.names' to be set.
                         For example, to get the service status of 'Microsoft Defender Antivirus Network Inspection Service',
                         the property 'sa_plugin_service.names' should be set to 'WdNisSvc'.
                         If the service name is found, the property 'sa_plugin_services.WdNisSvc.status' is set to the service status.
                         The status values are: unknown, continuing, pausing, paused, running, starting, stopping and stopped.
                         Multiple service names can be specified with the ';' character." />
  </plugins>
  <shell>
    <default>
      <property name="sa_plugin_services.names" value=""/>
      <property name="sa_plugin_services.names" value="WinDefend;${sa_plugin_services.names}"/>
      <property name="sa_plugin_services.names" value="Dhcp;${sa_plugin_services.names}"/>
      <property name="sa_plugin_services.names" value="Dnscache;${sa_plugin_services.names}"/>
      <property name="sa_plugin_services.names" value="OneDrive Updater Service;${sa_plugin_services.names}"/>
      <property name="sa_plugin_services.names" value="wuauserv;${sa_plugin_services.names}"/>
      <property name="sa_plugin_services.names" value="Fax;${sa_plugin_services.names}"/>
      <property name="sa_plugin_services.names" value="msiserver;${sa_plugin_services.names}"/>
       <property name="sa_plugin_services.names" value="mpssvc;${sa_plugin_services.names}"/>
   </default>

    <menu name="Windows Defender Firewall">
      <menu name="Disable">
        <!-- This menu is only visible if the service is running -->
        <visibility exprtk="'${sa_plugin_services.mpssvc.status}' == 'running'">
        <actions>
          <!-- exec action to stop a this service -->
        </actions>
      </menu>
      <menu name="Enable">
        <!-- This menu is only visible if the service is stopped -->
        <visibility exprtk="'${sa_plugin_services.mpssvc.status}' == 'stopped'">
        <actions>
          <!-- exec action to start a this service -->
        </actions>
      </menu>
    </menu>

  </shell>
</root>
```

Note that you can specify multiple service names using a single property:
```xml
<property name="sa_plugin_services.names" 
          value="WinDefend;Dhcp;Dnscache;OneDrive;wuauserv;Fax;msiserver"/>
```
or you can append to the existing property using a construction like the following:
```xml
<property name="sa_plugin_services.names" value=""/>
<property name="sa_plugin_services.names" value="value1;${sa_plugin_services.names}"/>
<property name="sa_plugin_services.names" value="value2;${sa_plugin_services.names}"/>
<property name="sa_plugin_services.names" value="value3;${sa_plugin_services.names}"/>
```


# Building #

This section explains how to compile _sa_plugin_services_ example.

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

Please refer to [install_shellanything_plugins.bat](https://github.com/end2endzone/ShellAnything/blob/master/ci/windows/install_shellanything_plugins.bat) for details about _sa_plugin_services_ plugin.
