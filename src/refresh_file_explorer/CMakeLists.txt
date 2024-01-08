find_package(rapidassist REQUIRED)

set(REFRESH_FILE_EXP_RESOURCE_FILES ""
  resource.h
  resource.ico
  resource.rc
)

add_executable(refresh_file_explorer WIN32
  ${SHELLANYTHING_EXPORT_HEADER}
  ${SHELLANYTHING_VERSION_HEADER}
  ${SHELLANYTHING_CONFIG_HEADER}
  ${REFRESH_FILE_EXP_RESOURCE_FILES}
  file_explorer.cpp
  file_explorer.h
  framework.h
  main.cpp
  main.h
  targetver.h
  user.cpp
  user.h
)

# Group external files as filter for Visual Studio
source_group("Resource Files"          FILES ${REFRESH_FILE_EXP_RESOURCE_FILES})

# Force UNICODE for target
target_compile_definitions(refresh_file_explorer PRIVATE -D_UNICODE -DUNICODE)

# Force CMAKE_DEBUG_POSTFIX for executables
set_target_properties(refresh_file_explorer PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# Define include directories for the executable.
target_include_directories(refresh_file_explorer
  PRIVATE
    ${GTEST_INCLUDE_DIR}
    rapidassist
    ${CMAKE_SOURCE_DIR}/src/shared
)

# Define linking dependencies.
add_dependencies(refresh_file_explorer sa.shared)
target_link_libraries(refresh_file_explorer
  PRIVATE
    sa.shared
    rapidassist
)

install(TARGETS refresh_file_explorer
        EXPORT shellanything-targets
        ARCHIVE DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
        LIBRARY DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${SHELLANYTHING_INSTALL_BIN_DIR}
)
