add_library(sa.shellextension SHARED
  ${SHELLANYTHING_EXPORT_HEADER}
  ${SHELLANYTHING_VERSION_HEADER}
  ${SHELLANYTHING_CONFIG_HEADER}
  ${SHELLANYTHING_RESOURCE_RC}
  ${SHELLANYTHING_VERSION_RC}
  ${CMAKE_SOURCE_DIR}/src/resource.rc.in
  ${CMAKE_SOURCE_DIR}/src/version.rc.in
  CClassFactory.cpp
  CClassFactory.h
  CContextMenu.cpp
  CContextMenu.h
  CCriticalSection.cpp
  CCriticalSection.h
  dllmain.cpp
  shellext.cpp
  shellext.h
  shellext.def
  stdafx.h
  targetver.h
)

# Force CMAKE_DEBUG_POSTFIX for executables
set_target_properties(sa.shellextension        PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# Define include directories for the library.
target_include_directories(sa.shellextension
  PUBLIC
    $<INSTALL_INTERFACE:${SHELLANYTHING_INSTALL_INCLUDE_DIR}>  # for clients using the installed library.
  PRIVATE
    rapidassist
    glog::glog
    libmagic
    ${CMAKE_SOURCE_DIR}/src/shared
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/logger/glog
    ${CMAKE_BINARY_DIR}/src/logger/glog
)

# Define linking dependencies.
target_link_libraries(sa.shellextension
  PRIVATE
    sa.core
    sa.shared
    sa.logger.glog
    rapidassist
    glog::glog
    libmagic
)

# Define files that will be part of the installation package.
install(TARGETS sa.shellextension
        EXPORT shellanything-targets
        ARCHIVE DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
        LIBRARY DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${SHELLANYTHING_INSTALL_BIN_DIR}
)
