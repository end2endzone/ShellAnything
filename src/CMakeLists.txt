find_package(rapidassist REQUIRED)
find_package(tinyxml2 REQUIRED)
find_package(glog REQUIRED)
find_package(libmagic REQUIRED)
find_package(ZLIB REQUIRED)

#resource.rc
set(SHELLANYTHING_RESOURCE_RC ${CMAKE_BINARY_DIR}/src/resource.rc)
message("Generating ${SHELLANYTHING_RESOURCE_RC}...")
configure_file( ${CMAKE_SOURCE_DIR}/src/resource.rc.in ${SHELLANYTHING_RESOURCE_RC} )

#version.rc
set(SHELLANYTHING_VERSION_RC ${CMAKE_BINARY_DIR}/src/version.rc)
message("Generating ${SHELLANYTHING_VERSION_RC}...")
configure_file( ${CMAKE_SOURCE_DIR}/src/version.rc.in ${SHELLANYTHING_VERSION_RC} )

set(CONFIGURATION_DEFAULT_FILES ""
   ${CMAKE_SOURCE_DIR}/resources/configurations/default.xml
  "${CMAKE_SOURCE_DIR}/resources/configurations/git.xml"
  "${CMAKE_SOURCE_DIR}/resources/configurations/Hugo.xml"
  "${CMAKE_SOURCE_DIR}/resources/configurations/Mark Text.xml"
  "${CMAKE_SOURCE_DIR}/resources/configurations/"
  "${CMAKE_SOURCE_DIR}/resources/configurations/"
  "${CMAKE_SOURCE_DIR}/resources/configurations/Microsoft Office 2003.xml"
  "${CMAKE_SOURCE_DIR}/resources/configurations/Microsoft Office 2007.xml"
  "${CMAKE_SOURCE_DIR}/resources/configurations/Microsoft Office 2010.xml"
  "${CMAKE_SOURCE_DIR}/resources/configurations/Microsoft Office 2013.xml"
  "${CMAKE_SOURCE_DIR}/resources/configurations/Microsoft Office 2016.xml"
   ${CMAKE_SOURCE_DIR}/resources/configurations/shellanything.xml
   ${CMAKE_SOURCE_DIR}/resources/configurations/WinDirStat.xml
)

# Subprojects
add_subdirectory(shared)
add_subdirectory(libexprtk)
add_subdirectory(core)
add_subdirectory(shellextension)
add_subdirectory(api)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/resources/configurations
        DESTINATION ${SHELLANYTHING_INSTALL_BIN_DIR}
)

install(FILES ${LIBMAGIC_MGC_DIR}/magic.mgc
              ${LIBMAGIC_MGC_DIR}/file.exe
              ${ZLIB_INCLUDE_DIRS}/../bin/zlib.dll
              DESTINATION ${SHELLANYTHING_INSTALL_BIN_DIR}
)
