# resource.rc file
set(FLAT-COLOR-ICONS_RESOURCE_FILE ${CMAKE_CURRENT_BINARY_DIR}/resource.rc)
message("Generating ${FLAT-COLOR-ICONS_RESOURCE_FILE}...")
configure_file( resource.rc.in ${FLAT-COLOR-ICONS_RESOURCE_FILE} )

add_library(flat-color-icons SHARED
  resource.h
  resource.rc.in
  ${FLAT-COLOR-ICONS_RESOURCE_FILE}
)

# Force CMAKE_DEBUG_POSTFIX for executables
# NOTE: DO NOT FOR DEBUG POSTFIX FOR ICON LIBRARIES.
set_target_properties(flat-color-icons       PROPERTIES DEBUG_POSTFIX "")

# Specify the /NOENTRY linker option. /NOENTRY prevents the linker from linking a reference to _main into the DLL; this option is required to create a resource-only DLL.
set_target_properties(flat-color-icons       PROPERTIES LINK_FLAGS "/MANIFEST:NO /NODEFAULTLIB /NOENTRY" )

# Define include directories for the executable.
target_include_directories(flat-color-icons
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Define linking dependencies.
add_dependencies(flat-color-icons sa.core)
target_link_libraries(flat-color-icons
  PRIVATE
    rapidassist
    sa.core
    sa.shared
)

# Define files that will be part of the installation package.
install(TARGETS flat-color-icons
        EXPORT shellanything-targets
        ARCHIVE DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
        LIBRARY DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${SHELLANYTHING_INSTALL_BIN_DIR}
)


configure_file(${CMAKE_SOURCE_DIR}/third_parties/flat-color-icons/LICENSE.md ${CMAKE_BINARY_DIR}/third_parties/flat-color-icons/flat-color-icons.LICENSE.md COPYONLY)

install(FILES ${CMAKE_BINARY_DIR}/third_parties/flat-color-icons/flat-color-icons.LICENSE.md
              ${CMAKE_CURRENT_SOURCE_DIR}/flat-color-icons.url
              DESTINATION ${SHELLANYTHING_INSTALL_BIN_DIR})
