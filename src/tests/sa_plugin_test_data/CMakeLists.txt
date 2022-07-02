cmake_minimum_required(VERSION 3.0)
project(sa_plugin_test_data)

# if plugin is compiled with Shellanything's source code
if (NOT SHELLANYTHING_BUILD_PLUGINS)
  find_package(shellanything 0.8.0 REQUIRED)
endif()

find_package(rapidassist REQUIRED)

add_library(sa_plugin_test_data SHARED
  sa_plugin_test_data.cpp
)

# Force a debug postfix if none specified.
# This allows publishing both release and debug binaries to the same location
# and it helps to prevent linking with the wrong library on Windows.
if(NOT CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX "-d")
endif()

# Force CMAKE_DEBUG_POSTFIX for executables and libraries
set_target_properties(sa_plugin_test_data PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# Define include directories for the library.
target_include_directories(sa_plugin_test_data
  PRIVATE
    sa.api
    rapidassist
)

# Define linking dependencies.
add_dependencies(sa_plugin_test_data sa.api)
target_link_libraries(sa_plugin_test_data
  PRIVATE
    sa.api
    rapidassist
)

# Define files that will be part of the installation package.
if (SHELLANYTHING_BUILD_PLUGINS)
  # Plugin is compiled with Shellanything's source code
  # Install the plugin in the same directory as ShellAnything but as an independent export.
  install(TARGETS sa_plugin_test_data
          EXPORT sa_plugin_test_data-targets
          ARCHIVE DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
          LIBRARY DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
          RUNTIME DESTINATION ${SHELLANYTHING_INSTALL_BIN_DIR}
  )
else()
  install(TARGETS sa_plugin_test_data
          EXPORT sa_plugin_test_data-targets
          ARCHIVE DESTINATION "lib"
          LIBRARY DESTINATION "lib"
          RUNTIME DESTINATION "bin"
          INCLUDES DESTINATION "include"
  )
endif()
