find_package(rapidassist REQUIRED)

set(REFRESH_FILE_EXP_RESOURCE_FILES ""
  resource.h
  resource.ico
  resource.rc
)

add_executable(file_explorer_renew WIN32
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
  user_feedback.cpp
  user_feedback.h
)

# Group external files as filter for Visual Studio
source_group("Resource Files"          FILES ${REFRESH_FILE_EXP_RESOURCE_FILES})

# Force UNICODE for target
target_compile_definitions(file_explorer_renew PRIVATE -D_UNICODE -DUNICODE)

# Force CMAKE_DEBUG_POSTFIX for executables
set_target_properties(file_explorer_renew PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# Define include directories for the executable.
target_include_directories(file_explorer_renew
  PRIVATE
    ${GTEST_INCLUDE_DIR}
    rapidassist
    ${CMAKE_SOURCE_DIR}/src/shared
)

# Define linking dependencies.
add_dependencies(file_explorer_renew sa.shared)
target_link_libraries(file_explorer_renew
  PRIVATE
    sa.shared
    rapidassist
)

install(TARGETS file_explorer_renew
        EXPORT shellanything-targets
        ARCHIVE DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
        LIBRARY DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${SHELLANYTHING_INSTALL_BIN_DIR}
)
