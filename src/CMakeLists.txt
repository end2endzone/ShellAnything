find_package(rapidassist REQUIRED)
find_package(tinyxml2 REQUIRED)
find_package(win32clipboard REQUIRED)
find_package(glog REQUIRED)

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
  "${CMAKE_SOURCE_DIR}/resources/configurations/Microsoft Office 2003.xml"
  "${CMAKE_SOURCE_DIR}/resources/configurations/Microsoft Office 2007.xml"
  "${CMAKE_SOURCE_DIR}/resources/configurations/Microsoft Office 2010.xml"
  "${CMAKE_SOURCE_DIR}/resources/configurations/Microsoft Office 2013.xml"
  "${CMAKE_SOURCE_DIR}/resources/configurations/Microsoft Office 2016.xml"
   ${CMAKE_SOURCE_DIR}/resources/configurations/shellanything.xml
   ${CMAKE_SOURCE_DIR}/resources/configurations/WinDirStat.xml
)

set(SHELLANYTHING_HEADER_FILES ""
  ${CMAKE_SOURCE_DIR}/include/shellanything/Action.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/ActionClipboard.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/ActionExecute.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/ActionFile.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/ActionOpen.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/ActionPrompt.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/ActionProperty.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/Configuration.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/ConfigManager.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/Context.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/DefaultSettings.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/Icon.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/Menu.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/Node.h
  ${CMAKE_SOURCE_DIR}/include/shellanything/Validator.h
)

add_library(shellanything STATIC
  ${SHELLANYTHING_HEADER_FILES}
  ${SHELLANYTHING_EXPORT_HEADER}
  ${SHELLANYTHING_VERSION_HEADER}
  ${SHELLANYTHING_CONFIG_HEADER}
  ${CONFIGURATION_DEFAULT_FILES}
  Action.cpp
  ActionClipboard.cpp
  ActionExecute.cpp
  ActionFile.cpp
  ActionOpen.cpp
  ActionPrompt.cpp
  ActionProperty.cpp
  Configuration.cpp
  ConfigManager.cpp
  Context.cpp
  DefaultSettings.cpp
  Icon.cpp
  InputBox.h
  InputBox.cpp
  Menu.cpp
  Node.cpp
  ObjectFactory.h
  ObjectFactory.cpp
  Validator.cpp
  Platform.h
  Platform.cpp
  PropertyManager.h
  PropertyManager.cpp
)

add_library(shellext SHARED
  ${SHELLANYTHING_EXPORT_HEADER}
  ${SHELLANYTHING_VERSION_HEADER}
  ${SHELLANYTHING_CONFIG_HEADER}
  ${SHELLANYTHING_RESOURCE_RC}
  ${SHELLANYTHING_VERSION_RC}
  ${CMAKE_SOURCE_DIR}/src/resource.rc.in
  ${CMAKE_SOURCE_DIR}/src/version.rc.in
  BitmapCache.cpp
  BitmapCache.h
  glog_utils.cpp
  glog_utils.h
  shellext.cpp
  shellext.h
  shellext.def
  win32_utils.cpp
  win32_utils.h
  win32_registry.cpp
  win32_registry.h
)

# Group external files as filter for Visual Studio
source_group("Default Configuration Files"    FILES ${CONFIGURATION_DEFAULT_FILES})

# Copy default configuration files database to target dir
add_custom_command( TARGET shellanything POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_SOURCE_DIR}/resources/configurations $<TARGET_FILE_DIR:shellanything>/configurations
                    COMMENT "Copying default configurations files database.")

# Unit test projects requires to link with pthread if also linking with gtest
if(SHELLANYTHING_BUILD_GTESTHELP)
  if(NOT WIN32)
    set(PTHREAD_LIBRARIES -pthread)
  endif()
endif()

# Force CMAKE_DEBUG_POSTFIX for executables
set_target_properties(shellanything   PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
set_target_properties(shellext        PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# Define include directories for exported code.
target_include_directories(shellanything
  PUBLIC
    $<INSTALL_INTERFACE:${SHELLANYTHING_INSTALL_INCLUDE_DIR}>  # for clients using the installed library.
  PRIVATE
    ${GTEST_INCLUDE_DIR}
    rapidassist
    tinyxml2_static
    win32clipboard
    glog::glog
)
target_include_directories(shellext
  PUBLIC
    $<INSTALL_INTERFACE:${SHELLANYTHING_INSTALL_INCLUDE_DIR}>  # for clients using the installed library.
  PRIVATE
    rapidassist
    glog::glog
)
target_link_libraries(shellanything   PRIVATE ${PTHREAD_LIBRARIES} ${GTEST_LIBRARIES} rapidassist tinyxml2_static win32clipboard glog::glog)
target_link_libraries(shellext        PRIVATE shellanything rapidassist glog::glog)

install(TARGETS shellanything
        EXPORT shellanything-targets
        ARCHIVE DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
        LIBRARY DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${SHELLANYTHING_INSTALL_BIN_DIR}
)
install(DIRECTORY ${CMAKE_SOURCE_DIR}/resources/configurations
        DESTINATION ${SHELLANYTHING_INSTALL_BIN_DIR}
)
install(TARGETS shellext
        EXPORT shellanything-targets
        ARCHIVE DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
        LIBRARY DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${SHELLANYTHING_INSTALL_BIN_DIR}
)
