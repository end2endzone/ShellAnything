add_library(sa.windows SHARED
  dllmain.cpp
  ClipboardService.cpp
  ClipboardService.h
  RegistryService.cpp
  RegistryService.h
  Win32Clipboard.cpp
  Win32Clipboard.h
)

# Force CMAKE_DEBUG_POSTFIX for executables
set_target_properties(sa.windows       PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

set(SA_WINDOWS_EXPORT_HEADER_FILENAME "sa_windows_export.h")
set(SA_WINDOWS_EXPORT_HEADER ${CMAKE_BINARY_DIR}/src/windows/${SA_WINDOWS_EXPORT_HEADER_FILENAME})
message("Generating ${SA_WINDOWS_EXPORT_HEADER_FILENAME} for shared library...")
include (GenerateExportHeader) 
GENERATE_EXPORT_HEADER( sa.windows 
                        BASE_NAME sa_windows 
                        EXPORT_MACRO_NAME SA_WINDOWS_EXPORT 
                        EXPORT_FILE_NAME ${SA_WINDOWS_EXPORT_HEADER} 
)

# Define include directories for the library.
target_include_directories(sa.windows
  PUBLIC
    $<INSTALL_INTERFACE:${SHELLANYTHING_INSTALL_INCLUDE_DIR}>  # for clients using the installed library.
  PRIVATE
    rapidassist
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_BINARY_DIR}/src/windows
    ${CMAKE_SOURCE_DIR}/src/shared
)

# Define linking dependencies.
add_dependencies(sa.windows sa.core)
target_link_libraries(sa.windows
  PRIVATE
    rapidassist
    sa.core
    sa.shared
)

# Define files that will be part of the installation package.
install(TARGETS sa.windows
        EXPORT shellanything-targets
        ARCHIVE DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
        LIBRARY DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${SHELLANYTHING_INSTALL_BIN_DIR}
)
