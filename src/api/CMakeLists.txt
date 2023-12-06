set(SHELLANYTHING_API_HEADER_FILES ""
  ${CMAKE_SOURCE_DIR}/include/shellanything/sa_action.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/sa_cfgmgr.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/sa_configuration.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/sa_enums.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/sa_selection_context.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/sa_enums.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/sa_error.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/sa_icon.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/sa_logging.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/sa_memory.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/sa_menu.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/sa_types.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/sa_plugin.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/sa_plugin_definitions.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/sa_properties.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/sa_property_store.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/sa_string.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/sa_validator.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/sa_xml.h
)

add_library(sa.api SHARED
  ${SHELLANYTHING_API_HEADER_FILES}
  sa.api.def
  sa_action.cpp
  sa_cfgmgr.cpp
  sa_configuration.cpp
  sa_selection_context.cpp
  sa_error.cpp
  sa_icon.cpp
  sa_logging.cpp
  sa_memory.cpp
  sa_menu.cpp
  sa_plugin.cpp
  sa_private_casting.cpp
  sa_private_casting.h
  sa_properties.cpp
  sa_property_store.cpp
  sa_string.cpp
  sa_string_private.cpp
  sa_string_private.h
  sa_validator.cpp
  sa_xml.cpp
)

# Force CMAKE_DEBUG_POSTFIX for executables
set_target_properties(sa.api       PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# Define include directories for the library.
target_include_directories(sa.api
  PUBLIC
    $<INSTALL_INTERFACE:${SHELLANYTHING_INSTALL_INCLUDE_DIR}>  # for clients using the installed library.
  PRIVATE
    rapidassist
    ${CMAKE_SOURCE_DIR}/src/core
)

# Define linking dependencies.
add_dependencies(sa.api sa.core)
target_link_libraries(sa.api
  PRIVATE
    rapidassist
    sa.core
)

# Also add Tinyxml2 include and libraries.
# The include/libraries are added at the end to allow supporting both static or shared libraries (the target names are different).
# See issue #67 (https://github.com/end2endzone/ShellAnything/issues/67) for details.
if (TARGET tinyxml2)
  target_include_directories(sa.api PRIVATE tinyxml2)
  target_link_libraries(sa.api PRIVATE tinyxml2)
else()
  target_include_directories(sa.api PRIVATE tinyxml2_static)
  target_link_libraries(sa.api PRIVATE tinyxml2_static)
endif()

# Define files that will be part of the installation package.
install(TARGETS sa.api
        EXPORT shellanything-targets
        ARCHIVE DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
        LIBRARY DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${SHELLANYTHING_INSTALL_BIN_DIR}
)
