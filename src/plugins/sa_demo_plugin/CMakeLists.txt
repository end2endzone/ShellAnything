cmake_minimum_required(VERSION 3.0)
project(sa_demo_plugin)

find_package(shellanything 0.8.0 REQUIRED)

## Debugging code
#message(shellanything_FOUND=        ${shellanything_FOUND})
#message(shellanything_LIBRARY=      ${shellanything_LIBRARY})
#message(shellanything_LIBRARIES=    ${shellanything_LIBRARIES})
#message(shellanything_INCLUDE=      ${shellanything_INCLUDE})
#message(shellanything_INCLUDES=     ${shellanything_INCLUDES})
#message(shellanything_INCLUDE_DIR=  ${shellanything_INCLUDE_DIR})
#message(shellanything_VERSION=      ${shellanything_VERSION})
#message(shellanything_DIR=          ${shellanything_DIR})

add_library(sa_demo_plugin SHARED
  sa_demo_plugin.cpp
)

# Force a debug postfix if none specified.
# This allows publishing both release and debug binaries to the same location
# and it helps to prevent linking with the wrong library on Windows.
if(NOT CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX "-d")
endif()

# Force CMAKE_DEBUG_POSTFIX for executables and libraries
set_target_properties(sa_demo_plugin PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# Define include directories for the library.
target_include_directories(sa_demo_plugin
  PRIVATE
    sa.api
)

# Define linking dependencies.
add_dependencies(sa_demo_plugin sa.api)
target_link_libraries(sa_demo_plugin
  PRIVATE
    sa.api
)

# Define files that will be part of the installation package.
install(TARGETS sa_demo_plugin
		EXPORT sa_demo_plugin-targets
		ARCHIVE DESTINATION "lib"
		LIBRARY DESTINATION "lib"
		RUNTIME DESTINATION "bin"
		INCLUDES DESTINATION "include"
)
