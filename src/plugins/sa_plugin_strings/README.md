# sa_plugin_strings #
_sa_plugin_strings_ is a plugin library for [ShellAnything](https://github.com/end2endzone/ShellAnything). This plugin allow to execute string manipulation actions.

This plugin registers multiple actions to manipulate strings: `substr`, `strreplace`, `strlen`, `struppercase`, `strlowercase` and `strfind`.


# Usage #

Declare the plugin in a Configuration File as the following:
```xml
<root>
  <plugins>
    <plugin path="${application.directory}\sa_plugin_strings.dll"
            actions="substr;strreplace;strlen;struppercase;strlowercase;strfind"
            description="This plugin provide string manipulation actions.
                         It defines the following actions :
                           * `substr` which compute the substring of a string,
                           * `strreplace`, which replaces a value by another in a string,
                           * `strlen`, which compute the length of a value in bytes,
                           * `struppercase`, which uppercase a value,
                           * `strlowercase`, which lowercase a value,
                           * `strfind`, which searches for a value" />
  </plugins>
  <shell>
    <default>
      <property name="sa_plugin_strings.digits" value="0123456789abcdefghijklmnopqrstuvwxyz"/>
      <property name="sa_plugin_strings.name"   value="Marty McFly"/>
      <property name="sa_plugin_strings.quote"  value="Roads? Where We're Going, We Don't Need Roads."/>
    </default>

    <menu name="String demo">
      <actions>
        <substr offset="0"  count="10" value="${sa_plugin_strings.digits}" property="sa_plugin_strings.numbers" />
        <substr offset="10" count="26" value="${sa_plugin_strings.digits}" property="sa_plugin_strings.letters" />
        <strlen value="${sa_plugin_strings.name}" property="sa_plugin_strings.strlen" />
        <strreplace text="${sa_plugin_strings.name}" token="Marty" value="George" property="sa_plugin_strings.father" />
        <struppercase value="${sa_plugin_strings.quote}" property="sa_plugin_strings.uppercase" />
        <strlowercase value="${sa_plugin_strings.quote}" property="sa_plugin_strings.lowercase" />
        <strfind text="${sa_plugin_strings.quote}" value="Roads" property="sa_plugin_strings.roads.1" />
        <strfind text="${sa_plugin_strings.quote}" value="Roads" offset="10" property="sa_plugin_strings.roads.2" />
        <strfind text="${sa_plugin_strings.quote}" value="doesnotexists" property="sa_plugin_strings.roads.3" />

        <!-- Example code for proceeding a search and replace unknown user name in an error message -->
        <!-- Build a sample error message which contains a user name in the error message -->
        <property name="sa_plugin_strings.demo.message" value="Error: User &apos;foobar&apos; not found!" />

        <!-- Find the offset of the user name value -->
        <strfind text="${sa_plugin_strings.demo.message}" value="&apos;" property="sa_plugin_strings.demo.offset" />
        <property name="sa_plugin_strings.demo.offset" exprtk="${sa_plugin_strings.demo.offset}+1" />

        <!-- Find the length of the user name -->
        <strfind text="${sa_plugin_strings.demo.message}" value="&apos;" offset="${sa_plugin_strings.demo.offset}" property="sa_plugin_strings.demo.lenght" />
        <property name="sa_plugin_strings.demo.lenght" exprtk="${sa_plugin_strings.demo.lenght}-${sa_plugin_strings.demo.offset}" />

        <!-- Get the user name string -->
        <substr offset="${sa_plugin_strings.demo.offset}" count="${sa_plugin_strings.demo.lenght}" value="${sa_plugin_strings.demo.message}" property="sa_plugin_strings.demo.username" />

        <!-- proceed with the search and replace -->
        <strreplace text="${sa_plugin_strings.demo.message}" token="'${sa_plugin_strings.demo.username}'" value="'mcfly.m'" property="sa_plugin_strings.demo.message" />
      </actions>
    </menu>

  </shell>
</root>
```


# Building #

This section explains how to compile _sa_plugin_strings_ example.

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

Please refer to [install_shellanything_plugins.bat](https://github.com/end2endzone/ShellAnything/blob/master/ci/windows/install_shellanything_plugins.bat) for details about _sa_plugin_strings_ plugin.
