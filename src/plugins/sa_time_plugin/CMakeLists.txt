cmake_minimum_required(VERSION 3.0)
project(sa_time_plugin)

# if plugin is compiled with Shellanything's source code
if (NOT SHELLANYTHING_BUILD_PLUGINS)
  find_package(shellanything 0.8.0 REQUIRED)
endif()

## Debugging code
#message(shellanything_FOUND=        ${shellanything_FOUND})
#message(shellanything_LIBRARY=      ${shellanything_LIBRARY})
#message(shellanything_LIBRARIES=    ${shellanything_LIBRARIES})
#message(shellanything_INCLUDE=      ${shellanything_INCLUDE})
#message(shellanything_INCLUDES=     ${shellanything_INCLUDES})
#message(shellanything_INCLUDE_DIR=  ${shellanything_INCLUDE_DIR})
#message(shellanything_VERSION=      ${shellanything_VERSION})
#message(shellanything_DIR=          ${shellanything_DIR})

add_library(sa_time_plugin SHARED
  sa_time_plugin.cpp
)

# Force a debug postfix if none specified.
# This allows publishing both release and debug binaries to the same location
# and it helps to prevent linking with the wrong library on Windows.
if(NOT CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX "-d")
endif()

# Force CMAKE_DEBUG_POSTFIX for executables
set_target_properties(sa_time_plugin       PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# Define include directories for the library.
target_include_directories(sa_time_plugin
  PUBLIC
    $<INSTALL_INTERFACE:${SHELLANYTHING_INSTALL_INCLUDE_DIR}>  # for clients using the installed library.
  PRIVATE
    sa.api
)

# Define linking dependencies.
add_dependencies(sa_time_plugin sa.api)
target_link_libraries(sa_time_plugin
  PRIVATE
    sa.api
)

# Define files that will be part of the installation package.
if (SHELLANYTHING_BUILD_PLUGINS)
  # Plugin is compiled with Shellanything's source code
  # Install the plugin as part of a ShellAnything binary code.
  install(TARGETS sa_time_plugin
          EXPORT shellanything-targets
          ARCHIVE DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
          LIBRARY DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
          RUNTIME DESTINATION ${SHELLANYTHING_INSTALL_BIN_DIR}
  )
else()
  install(TARGETS sa_time_plugin
          EXPORT sa_time_plugin-targets
          ARCHIVE DESTINATION "lib"
          LIBRARY DESTINATION "lib"
          RUNTIME DESTINATION "bin"
          INCLUDES DESTINATION "include"
  )
endif()
