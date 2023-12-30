add_library(sa.logger.glog SHARED
  dllmain.cpp
  GlogLoggerService.cpp
  GlogLoggerService.h
  GlogUtils.cpp
  GlogUtils.h
)

# Force CMAKE_DEBUG_POSTFIX for executables
set_target_properties(sa.logger.glog       PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

set(SA_GLOG_EXPORT_HEADER_FILENAME "sa_glog_export.h")
set(SA_GLOG_EXPORT_HEADER ${CMAKE_BINARY_DIR}/src/logger/glog/${SA_GLOG_EXPORT_HEADER_FILENAME})
message("Generating ${SA_GLOG_EXPORT_HEADER_FILENAME} for shared library...")
include (GenerateExportHeader) 
GENERATE_EXPORT_HEADER( sa.logger.glog 
                        BASE_NAME sa_glog 
                        EXPORT_MACRO_NAME SA_GLOG_EXPORT 
                        EXPORT_FILE_NAME ${SA_GLOG_EXPORT_HEADER} 
)

# Define include directories for the library.
target_include_directories(sa.logger.glog
  PUBLIC
    $<INSTALL_INTERFACE:${SHELLANYTHING_INSTALL_INCLUDE_DIR}>  # for clients using the installed library.
  PRIVATE
    rapidassist
    glog::glog
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_BINARY_DIR}/src/logger/glog
    ${CMAKE_SOURCE_DIR}/src/shared
)

# Define linking dependencies.
add_dependencies(sa.logger.glog sa.core)
target_link_libraries(sa.logger.glog
  PRIVATE
    rapidassist
    glog::glog
    sa.core
    sa.shared
)

# Define files that will be part of the installation package.
install(TARGETS sa.logger.glog
        EXPORT shellanything-targets
        ARCHIVE DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
        LIBRARY DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${SHELLANYTHING_INSTALL_BIN_DIR}
)
