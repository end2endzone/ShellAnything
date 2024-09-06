add_library(sa.shellextension SHARED
  ${SHELLANYTHING_EXPORT_HEADER}
  ${SHELLANYTHING_VERSION_HEADER}
  ${SHELLANYTHING_CONFIG_HEADER}
  ${SHELLANYTHING_RESOURCE_RC}
  ${SHELLANYTHING_VERSION_RC}
  ${CMAKE_SOURCE_DIR}/src/resource.rc.in
  ${CMAKE_SOURCE_DIR}/src/version.rc.in
  CContextMenu.cpp
  CContextMenu.h
  CCriticalSection.cpp
  CCriticalSection.h
  dllmain.cpp
  Reg.cpp
  Reg.h
  resource.h
  shellext.def
  shellext.idl
  shellext.rgs
  stdafx.cpp
  stdafx.h
  targetver.h
  TypeLibHelper.cpp
  TypeLibHelper.h
  utils.cpp
  utils.h
)

# Force *.rgs to be groupped in Source Files
source_group("Source Files" FILES "shellext.rgs")

# Force CMAKE_DEBUG_POSTFIX for executables
set_target_properties(sa.shellextension        PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# Force UNICODE for target
target_compile_definitions(sa.shellextension PRIVATE -D_UNICODE -DUNICODE)

# Define include directories for the library.
target_include_directories(sa.shellextension
  PUBLIC
    $<INSTALL_INTERFACE:${SHELLANYTHING_INSTALL_INCLUDE_DIR}>  # for clients using the installed library.
  PRIVATE
    rapidassist
    libmagic
    ${CMAKE_SOURCE_DIR}/src/shellextension
    ${CMAKE_BINARY_DIR}/src/shellextension/sa.shellextension.dir/\$\(Configuration\)
    ${CMAKE_SOURCE_DIR}/src/shared
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/logger/glog
    ${CMAKE_BINARY_DIR}/src/logger/glog
    ${CMAKE_SOURCE_DIR}/src/windows
    ${CMAKE_BINARY_DIR}/src/windows
)

# Define linking dependencies.
target_link_libraries(sa.shellextension
  PRIVATE
    sa.core
    sa.shared
    sa.logger.glog
    sa.windows
    rapidassist
    libmagic
)

# Define files that will be part of the installation package.
install(TARGETS sa.shellextension
        EXPORT shellanything-targets
        ARCHIVE DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
        LIBRARY DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${SHELLANYTHING_INSTALL_BIN_DIR}
)
