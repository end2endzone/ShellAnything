find_package(rapidassist REQUIRED)

set(REFRESH_DEBUG_ARGS_WIN_RESOURCE_FILES ""
  resource.h
  showargs.ico
  showargs.rc
)

add_executable(DebugArgumentsWindow WIN32
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
target_compile_definitions(DebugArgumentsWindow PRIVATE -D_UNICODE -DUNICODE)

# Force CMAKE_DEBUG_POSTFIX for executables
set_target_properties(DebugArgumentsWindow PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# Define include directories for the executable.
target_include_directories(DebugArgumentsWindow
  PRIVATE
    ${GTEST_INCLUDE_DIR}
    rapidassist
    ${CMAKE_SOURCE_DIR}/src/shared
)

# Define linking dependencies.
add_dependencies(DebugArgumentsWindow sa.shared)
target_link_libraries(DebugArgumentsWindow
  PRIVATE
    sa.shared
    rapidassist
)

install(TARGETS DebugArgumentsWindow
        EXPORT shellanything-targets
        ARCHIVE DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
        LIBRARY DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${SHELLANYTHING_INSTALL_BIN_DIR}
)
