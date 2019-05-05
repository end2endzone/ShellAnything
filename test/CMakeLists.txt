find_package(GTest)
find_package(rapidassist)
find_package(glog REQUIRED)

set(CONFIGURATION_TEST_FILES ""
  ${CMAKE_CURRENT_SOURCE_DIR}/test_files/default.xml
  "${CMAKE_CURRENT_SOURCE_DIR}/test_files/Microsoft Office 2003.xml"
  "${CMAKE_CURRENT_SOURCE_DIR}/test_files/Microsoft Office 2007.xml"
  "${CMAKE_CURRENT_SOURCE_DIR}/test_files/Microsoft Office 2010.xml"
  "${CMAKE_CURRENT_SOURCE_DIR}/test_files/Microsoft Office 2013.xml"
  "${CMAKE_CURRENT_SOURCE_DIR}/test_files/Microsoft Office 2016.xml"
  ${CMAKE_CURRENT_SOURCE_DIR}/test_files/samples.xml
  ${CMAKE_CURRENT_SOURCE_DIR}/test_files/TestConfigManager.testAssignCommandId.1.xml
  ${CMAKE_CURRENT_SOURCE_DIR}/test_files/TestConfigManager.testAssignCommandId.2.xml
  ${CMAKE_CURRENT_SOURCE_DIR}/test_files/TestConfigManager.testClear.xml
  ${CMAKE_CURRENT_SOURCE_DIR}/test_files/TestConfigManager.testDetectNewFile.xml
  ${CMAKE_CURRENT_SOURCE_DIR}/test_files/TestConfigManager.testFileModifications.xml
  ${CMAKE_CURRENT_SOURCE_DIR}/test_files/TestConfigManager.testParentWithoutChildren.xml
  ${CMAKE_CURRENT_SOURCE_DIR}/test_files/TestObjectFactory.testParseValidator.xml
  ${CMAKE_CURRENT_SOURCE_DIR}/test_files/tests.xml
  ${CMAKE_CURRENT_SOURCE_DIR}/test_files/WinDirStat.xml
)

# Include files from shellanything library which are not public
set(SHELLANYTHING_PRIVATE_FILES ""
  ${CMAKE_SOURCE_DIR}/src/BitmapCache.h
  ${CMAKE_SOURCE_DIR}/src/BitmapCache.cpp
  ${CMAKE_SOURCE_DIR}/src/glog_utils.h
  ${CMAKE_SOURCE_DIR}/src/glog_utils.cpp
  ${CMAKE_SOURCE_DIR}/src/InputBox.h
  ${CMAKE_SOURCE_DIR}/src/InputBox.cpp
  ${CMAKE_SOURCE_DIR}/src/Platform.h
  ${CMAKE_SOURCE_DIR}/src/Platform.cpp
  ${CMAKE_SOURCE_DIR}/src/PropertyManager.h
  ${CMAKE_SOURCE_DIR}/src/PropertyManager.cpp
  ${CMAKE_SOURCE_DIR}/src/utf_strings.h
  ${CMAKE_SOURCE_DIR}/src/utf_strings.cpp
  ${CMAKE_SOURCE_DIR}/src/win32_registry.h
  ${CMAKE_SOURCE_DIR}/src/win32_registry.cpp
  ${CMAKE_SOURCE_DIR}/src/win32_utils.h
  ${CMAKE_SOURCE_DIR}/src/win32_utils.cpp
)

add_executable(shellanything_unittest
  ${SHELLANYTHING_EXPORT_HEADER}
  ${SHELLANYTHING_VERSION_HEADER}
  ${SHELLANYTHING_CONFIG_HEADER}
  ${CONFIGURATION_TEST_FILES}
  ${SHELLANYTHING_PRIVATE_FILES}
  main.cpp
  TestBitmapCache.cpp
  TestBitmapCache.h
  TestConfigManager.cpp
  TestConfigManager.h
  TestConfiguration.cpp
  TestConfiguration.h
  TestContext.cpp
  TestContext.h
  TestDemoSamples.cpp
  TestDemoSamples.h
  TestGlogUtils.cpp
  TestGlogUtils.h
  TestInputBox.cpp
  TestInputBox.h
  TestMenu.cpp
  TestMenu.h
  TestNode.cpp
  TestNode.h
  TestObjectFactory.cpp
  TestObjectFactory.h
  TestWin32Registry.cpp
  TestWin32Registry.h
  TestPropertyManager.cpp
  TestPropertyManager.h
  TestShellExtension.cpp
  TestShellExtension.h
  TestValidator.cpp
  TestValidator.h
  TestWin32Registry.cpp
  TestWin32Registry.h
  TestWin32Utils.cpp
  TestWin32Utils.h
)

# Group external files as filster for Visual Studio
source_group("Test Files"                     FILES ${CONFIGURATION_TEST_FILES})
source_group("ShellAnything Library Files"    FILES ${SHELLANYTHING_PRIVATE_FILES})

# Unit test projects requires to link with pthread if also linking with gtest
if(NOT WIN32)
  set(PTHREAD_LIBRARIES -pthread)
endif()

# Force CMAKE_DEBUG_POSTFIX for executables
set_target_properties(shellanything_unittest PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

target_include_directories(shellanything_unittest PRIVATE ${GTEST_INCLUDE_DIR} rapidassist glog::glog ${CMAKE_SOURCE_DIR}/src)
add_dependencies(shellanything_unittest shellanything shellext)
target_link_libraries(shellanything_unittest PUBLIC shellanything shellext PRIVATE ${PTHREAD_LIBRARIES} ${GTEST_LIBRARIES} rapidassist glog::glog)

# Copy test files database to target dir
add_custom_command( TARGET shellanything_unittest POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                    ${CMAKE_CURRENT_SOURCE_DIR}/test_files $<TARGET_FILE_DIR:shellanything_unittest>/test_files
                    COMMENT "Copying test_files database.")

# Copy test files to build dir for local execution (from within the IDE)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test_files DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

install(TARGETS shellanything_unittest
        EXPORT shellanything-targets
        ARCHIVE DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
        LIBRARY DESTINATION ${SHELLANYTHING_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${SHELLANYTHING_INSTALL_BIN_DIR}
)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test_files
        DESTINATION ${SHELLANYTHING_INSTALL_BIN_DIR}
)
