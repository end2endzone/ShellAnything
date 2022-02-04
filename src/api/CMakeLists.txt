set(SHELLANYTHING_API_HEADER_FILES ""
  ${CMAKE_SOURCE_DIR}/include/shellanything/sa_action.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/sa_cfgmgr.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/sa_configuration.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/sa_context.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/sa_error.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/sa_icon.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/sa_logging.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/sa_memory.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/sa_menu.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/sa_properties.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/sa_string.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/sa_validator.h
)

add_library(sa.api SHARED
  ${SHELLANYTHING_API_HEADER_FILES}
  sa.api.def
  sa_action.cpp
  sa_cfgmgr.cpp
  sa_configuration.cpp
  sa_context.cpp
  sa_error.cpp
  sa_icon.cpp
  sa_logging.cpp
  sa_memory.cpp
  sa_menu.cpp
  sa_properties.cpp
  sa_string.cpp
  sa_string_private.cpp
  sa_string_private.h
  sa_types_private.cpp
  sa_types_private.h
  sa_validator.cpp
)

# Force CMAKE_DEBUG_POSTFIX for executables
set_target_properties(sa.api       PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# Define include directories for the library.
target_include_directories(sa.api
  PUBLIC
    $<INSTALL_INTERFACE:${SHELLANYTHING_INSTALL_INCLUDE_DIR}>  # for clients using the installed library.
  PRIVATE
    rapidassist
    glog::glog
    ${CMAKE_SOURCE_DIR}/src/core
)

# Define linking dependencies.
add_dependencies(sa.api sa.core)
target_link_libraries(sa.api
  PRIVATE
    rapidassist
    glog::glog
    sa.core
)

# Define files that will be part of the installation package.
install(TARGETS sa.api
        EXPORT shellanything-targets
        ARCHIVE DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
        LIBRARY DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${SHELLANYTHING_INSTALL_BIN_DIR}
)
