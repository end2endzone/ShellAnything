find_package(rapidassist REQUIRED)

set(REFRESH_DEBUG_ARGS_CON_RESOURCE_FILES ""
  resource.h
  showargs.ico
  showargs.rc
)

add_executable(DebugArgumentsConsole
  ${SHELLANYTHING_EXPORT_HEADER}
  ${SHELLANYTHING_VERSION_HEADER}
  ${SHELLANYTHING_CONFIG_HEADER}
  ${REFRESH_DEBUG_ARGS_CON_RESOURCE_FILES}
  ../DebugArgumentsWindow/arguments.cpp
  ../DebugArgumentsWindow/arguments.h
  framework.h
  main.cpp
  targetver.h
)

# Group external files as filter for Visual Studio
source_group("Resource Files"          FILES ${REFRESH_DEBUG_ARGS_CON_RESOURCE_FILES})

# Force UNICODE for target
target_compile_definitions(DebugArgumentsConsole PRIVATE -D_UNICODE -DUNICODE)

# Force CMAKE_DEBUG_POSTFIX for executables
set_target_properties(DebugArgumentsConsole PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# Define include directories for the executable.
target_include_directories(DebugArgumentsConsole
  PRIVATE
    ${GTEST_INCLUDE_DIR}
    rapidassist
    ${CMAKE_SOURCE_DIR}/src/shared
)

# Define linking dependencies.
add_dependencies(DebugArgumentsConsole sa.shared)
target_link_libraries(DebugArgumentsConsole
  PRIVATE
    sa.shared
    rapidassist
)

install(TARGETS DebugArgumentsConsole
        EXPORT shellanything-targets
        ARCHIVE DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
        LIBRARY DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${SHELLANYTHING_INSTALL_BIN_DIR}
)
