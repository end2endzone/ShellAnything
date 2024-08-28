find_package(rapidassist REQUIRED)

set(REFRESH_DEBUG_ARGS_CON_RESOURCE_FILES ""
  resource.h
  showargs.ico
  showargs.rc
)

add_executable(arguments.debugger.console
  ${SHELLANYTHING_EXPORT_HEADER}
  ${SHELLANYTHING_VERSION_HEADER}
  ${SHELLANYTHING_CONFIG_HEADER}
  ${REFRESH_DEBUG_ARGS_CON_RESOURCE_FILES}
  ../arguments.debugger.window/arguments.cpp
  ../arguments.debugger.window/arguments.h
  framework.h
  main.cpp
  targetver.h
)

# Group external files as filter for Visual Studio
source_group("Resource Files"          FILES ${REFRESH_DEBUG_ARGS_CON_RESOURCE_FILES})

# Force UNICODE for target
target_compile_definitions(arguments.debugger.console PRIVATE -D_UNICODE -DUNICODE)

# Force CMAKE_DEBUG_POSTFIX for executables
set_target_properties(arguments.debugger.console PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# Define include directories for the executable.
target_include_directories(arguments.debugger.console
  PRIVATE
    ${GTEST_INCLUDE_DIR}
    rapidassist
    ${CMAKE_SOURCE_DIR}/src/shared
)

# Define linking dependencies.
add_dependencies(arguments.debugger.console sa.shared)
target_link_libraries(arguments.debugger.console
  PRIVATE
    sa.shared
    rapidassist
)

install(TARGETS arguments.debugger.console
        EXPORT shellanything-targets
        ARCHIVE DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
        LIBRARY DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${SHELLANYTHING_INSTALL_BIN_DIR}
)
