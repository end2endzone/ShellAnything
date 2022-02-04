set(SHELLANYTHING_CORE_HEADER_FILES ""
  ${CMAKE_SOURCE_DIR}/src/core/Action.h
  ${CMAKE_SOURCE_DIR}/src/core/ActionClipboard.h
  ${CMAKE_SOURCE_DIR}/src/core/ActionExecute.h
  ${CMAKE_SOURCE_DIR}/src/core/ActionFile.h
  ${CMAKE_SOURCE_DIR}/src/core/ActionMessage.h
  ${CMAKE_SOURCE_DIR}/src/core/ActionOpen.h
  ${CMAKE_SOURCE_DIR}/src/core/ActionPrompt.h
  ${CMAKE_SOURCE_DIR}/src/core/ActionProperty.h
  ${CMAKE_SOURCE_DIR}/src/core/ActionStop.h
  ${CMAKE_SOURCE_DIR}/src/core/Configuration.h
  ${CMAKE_SOURCE_DIR}/src/core/ConfigManager.h
  ${CMAKE_SOURCE_DIR}/src/core/Context.h
  ${CMAKE_SOURCE_DIR}/src/core/DefaultSettings.h
  ${CMAKE_SOURCE_DIR}/src/core/Icon.h
  ${CMAKE_SOURCE_DIR}/src/core/Menu.h
  ${CMAKE_SOURCE_DIR}/src/core/Validator.h
)

add_library(sa.core SHARED
  ${SHELLANYTHING_EXPORT_HEADER}
  ${SHELLANYTHING_VERSION_HEADER}
  ${SHELLANYTHING_CONFIG_HEADER}
  ${CONFIGURATION_DEFAULT_FILES}
  ${SHELLANYTHING_CORE_HEADER_FILES}
  Action.cpp
  ActionClipboard.cpp
  ActionExecute.cpp
  ActionFile.cpp
  ActionMessage.cpp
  ActionOpen.cpp
  ActionPrompt.cpp
  ActionProperty.cpp
  ActionStop.cpp
  Configuration.cpp
  ConfigManager.cpp
  Context.cpp
  DefaultSettings.cpp
  FileMagicManager.h
  FileMagicManager.cpp
  Icon.cpp
  InputBox.h
  InputBox.cpp
  Menu.cpp
  ObjectFactory.h
  ObjectFactory.cpp
  Unicode.h
  Unicode.cpp
  Validator.cpp
  DriveClass.h
  DriveClass.cpp
  ErrorManager.h
  ErrorManager.cpp
  PropertyManager.h
  PropertyManager.cpp
  Win32Clipboard.h
  Win32Clipboard.cpp
  Wildcard.cpp
  Wildcard.h
)

# Group external files as filter for Visual Studio
source_group("Default Configuration Files"          FILES ${CONFIGURATION_DEFAULT_FILES})

# Copy default configuration files database to target dir
add_custom_command( TARGET sa.core POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_SOURCE_DIR}/resources/configurations $<TARGET_FILE_DIR:sa.core>/configurations
                    COMMENT "Copying default configurations files database.")

# Copy registration and unregistration scripts to target dir
add_custom_command( TARGET sa.core POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    ${CMAKE_SOURCE_DIR}/resources/register.bat $<TARGET_FILE_DIR:sa.core>/register.bat
                    COMMENT "Copying register.bat script.")
add_custom_command( TARGET sa.core POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    ${CMAKE_SOURCE_DIR}/resources/unregister.bat $<TARGET_FILE_DIR:sa.core>/unregister.bat
                    COMMENT "Copying unregister.bat script.")
add_custom_command( TARGET sa.core POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    ${CMAKE_SOURCE_DIR}/resources/refresh.bat $<TARGET_FILE_DIR:sa.core>/refresh.bat
                    COMMENT "Copying refresh.bat script.")
add_custom_command( TARGET sa.core POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    ${LIBMAGIC_MGC_DIR}/magic.mgc $<TARGET_FILE_DIR:sa.core>/magic.mgc
                    COMMENT "Copying magic.mgc.")
add_custom_command( TARGET sa.core POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    ${LIBMAGIC_MGC_DIR}/file.exe $<TARGET_FILE_DIR:sa.core>/file.exe
                    COMMENT "Copying file.exe.")
add_custom_command( TARGET sa.core POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    ${ZLIB_INCLUDE_DIRS}/../bin/zlib.dll $<TARGET_FILE_DIR:sa.core>/zlib.dll
                    COMMENT "Copying zlib.dll.")
add_custom_command( TARGET sa.core POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    $<TARGET_FILE:glog::glog> $<TARGET_FILE_DIR:sa.core>/$<TARGET_FILE_NAME:glog::glog>
                    COMMENT "Copying glog.")

# Force CMAKE_DEBUG_POSTFIX for executables
set_target_properties(sa.core         PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# Define include directories for the library.
target_include_directories(sa.core
  PUBLIC
    $<INSTALL_INTERFACE:${SHELLANYTHING_INSTALL_INCLUDE_DIR}>  # for clients using the installed library.
  PRIVATE
    ${GTEST_INCLUDE_DIR}
    rapidassist
    glog::glog
    libexprtk
    libmagic
    ${CMAKE_SOURCE_DIR}/src/libexprtk
    ${CMAKE_SOURCE_DIR}/src/shared
)

# Define linking dependencies.
add_dependencies(sa.core libexprtk sa.shared)
target_link_libraries(sa.core
  PRIVATE
    ${PTHREAD_LIBRARIES}
    ${GTEST_LIBRARIES}
    rapidassist
    glog::glog
    libexprtk
    libmagic
    sa.shared
)

# Also add Tinyxml2 include and libraries.
# The include/libraries are added at the end to allow supporting both static or shared libraries (the target names are different).
# See issue #67 (https://github.com/end2endzone/ShellAnything/issues/67) for details.
if (TARGET tinyxml2)
  target_include_directories(sa.core PRIVATE tinyxml2)
  target_link_libraries(sa.core PRIVATE tinyxml2)
else()
  target_include_directories(sa.core PRIVATE tinyxml2_static)
  target_link_libraries(sa.core PRIVATE tinyxml2_static)
endif()

# Define files that will be part of the installation package.
install(TARGETS sa.core
        EXPORT shellanything-targets
        ARCHIVE DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
        LIBRARY DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${SHELLANYTHING_INSTALL_BIN_DIR}
)
