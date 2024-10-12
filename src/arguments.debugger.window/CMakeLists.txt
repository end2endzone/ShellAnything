find_package(rapidassist REQUIRED)

set(REFRESH_DEBUG_ARGS_WIN_RESOURCE_FILES ""
  resource.h
  showargs.rc
)

add_executable(arguments.debugger.window WIN32
  ${SHELLANYTHING_EXPORT_HEADER}
  ${SHELLANYTHING_VERSION_HEADER}
  ${SHELLANYTHING_CONFIG_HEADER}
  ${REFRESH_DEBUG_ARGS_WIN_RESOURCE_FILES}
  arguments.cpp
  arguments.h
  framework.h
  gui.cpp
  targetver.h
)

# Group external files as filter for Visual Studio
source_group("Resource Files"          FILES ${REFRESH_DEBUG_ARGS_WIN_RESOURCE_FILES})

# Force UNICODE for target
target_compile_definitions(arguments.debugger.window PRIVATE -D_UNICODE -DUNICODE)

# Force CMAKE_DEBUG_POSTFIX for executables
set_target_properties(arguments.debugger.window PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# Define include directories for the executable.
target_include_directories(arguments.debugger.window
  PRIVATE
    ${GTEST_INCLUDE_DIR}
    rapidassist
    ${CMAKE_SOURCE_DIR}/src/shared
    ${CMAKE_SOURCE_DIR}/resources/icons
    ${CMAKE_BINARY_DIR}/include # for finding config.h
    ${CMAKE_BINARY_DIR}
)

# Define linking dependencies.
add_dependencies(arguments.debugger.window sa.shared)
target_link_libraries(arguments.debugger.window
  PRIVATE
    sa.shared
    rapidassist
)

install(TARGETS arguments.debugger.window
        EXPORT shellanything-targets
        ARCHIVE DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
        LIBRARY DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${SHELLANYTHING_INSTALL_BIN_DIR}
)
