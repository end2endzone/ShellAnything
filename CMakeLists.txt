# CMakeLists.txt
cmake_minimum_required(VERSION 3.4.3 FATAL_ERROR)
project(ShellAnything)

#Find optional packages.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(WIX)
find_package(NSIS)
find_package(GRIP)

##############################################################################################################################################
# Standard CMake variables
##############################################################################################################################################

# BUILD_SHARED_LIBS is a standard CMake variable, but we declare it here to
# make it prominent in the GUI.
option(BUILD_SHARED_LIBS "Build shared libraries (DLLs)." OFF)

# Set a default build type if none was specified.
# See https://blog.kitware.com/cmake-and-the-default-build-type/
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  mark_as_advanced(CMAKE_BUILD_TYPE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Export no symbols by default (if the compiler supports it). 
# This makes e.g. GCC's "visibility behavior" consistent with MSVC's.  
# On Windows/MSVC this is a noop. 
if (BUILD_SHARED_LIBS)
  set(CMAKE_C_VISIBILITY_PRESET hidden) 
  set(CMAKE_CXX_VISIBILITY_PRESET hidden) 
endif()

# Set the output folder where your program will be created
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(   LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

##############################################################################################################################################
# CMake properties
##############################################################################################################################################
MESSAGE( STATUS "PROJECT_NAME:             " ${PROJECT_NAME} )
MESSAGE( STATUS "CMAKE_BINARY_DIR:         " ${CMAKE_BINARY_DIR} )
MESSAGE( STATUS "CMAKE_CURRENT_BINARY_DIR: " ${CMAKE_CURRENT_BINARY_DIR} )
MESSAGE( STATUS "CMAKE_SOURCE_DIR:         " ${CMAKE_SOURCE_DIR} )
MESSAGE( STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR} )
MESSAGE( STATUS "PROJECT_BINARY_DIR:       " ${PROJECT_BINARY_DIR} )
MESSAGE( STATUS "PROJECT_SOURCE_DIR:       " ${PROJECT_SOURCE_DIR} )
MESSAGE( STATUS "EXECUTABLE_OUTPUT_PATH:   " ${EXECUTABLE_OUTPUT_PATH} )
MESSAGE( STATUS "LIBRARY_OUTPUT_PATH:      " ${LIBRARY_OUTPUT_PATH} )
MESSAGE( STATUS "CMAKE_MODULE_PATH:        " ${CMAKE_MODULE_PATH} )
MESSAGE( STATUS "CMAKE_COMMAND:            " ${CMAKE_COMMAND} )
MESSAGE( STATUS "CMAKE_ROOT:               " ${CMAKE_ROOT} )
MESSAGE( STATUS "CMAKE_CURRENT_LIST_FILE:  " ${CMAKE_CURRENT_LIST_FILE} )
MESSAGE( STATUS "CMAKE_CURRENT_LIST_LINE:  " ${CMAKE_CURRENT_LIST_LINE} )
MESSAGE( STATUS "CMAKE_INCLUDE_PATH:       " ${CMAKE_INCLUDE_PATH} )
MESSAGE( STATUS "CMAKE_LIBRARY_PATH:       " ${CMAKE_LIBRARY_PATH} )
MESSAGE( STATUS "CMAKE_SYSTEM:             " ${CMAKE_SYSTEM} )
MESSAGE( STATUS "CMAKE_SYSTEM_NAME:        " ${CMAKE_SYSTEM_NAME} )
MESSAGE( STATUS "CMAKE_SYSTEM_VERSION:     " ${CMAKE_SYSTEM_VERSION} )
MESSAGE( STATUS "CMAKE_SYSTEM_PROCESSOR:   " ${CMAKE_SYSTEM_PROCESSOR} )

##############################################################################################################################################
# Global settings
##############################################################################################################################################

# Product version according to Semantic Versioning v2.0.0 https://semver.org/
set(SHELLANYTHING_VERSION_MAJOR 0)
set(SHELLANYTHING_VERSION_MINOR 5)
set(SHELLANYTHING_VERSION_PATCH 0)
set(SHELLANYTHING_VERSION ${SHELLANYTHING_VERSION_MAJOR}.${SHELLANYTHING_VERSION_MINOR}.${SHELLANYTHING_VERSION_PATCH})

# read license file
file(READ ${CMAKE_SOURCE_DIR}/LICENSE.h LICENSE)

# version.h file
set(SHELLANYTHING_VERSION_HEADER ${CMAKE_BINARY_DIR}/include/shellanything/version.h)
message("Generating ${SHELLANYTHING_VERSION_HEADER}...")
configure_file( ${CMAKE_SOURCE_DIR}/src/version.h.in ${SHELLANYTHING_VERSION_HEADER} )

# config.h file
set(SHELLANYTHING_CONFIG_HEADER ${CMAKE_BINARY_DIR}/include/shellanything/config.h)
message("Generating ${SHELLANYTHING_CONFIG_HEADER}...")
if (BUILD_SHARED_LIBS)
  set(SHELLANYTHING_BUILD_TYPE_CPP_DEFINE "#define SHELLANYTHING_BUILT_AS_SHARED")
else()
  set(SHELLANYTHING_BUILD_TYPE_CPP_DEFINE "#define SHELLANYTHING_BUILT_AS_STATIC")
endif()
configure_file( ${CMAKE_SOURCE_DIR}/src/config.h.in ${SHELLANYTHING_CONFIG_HEADER} )
set(SHELLANYTHING_BUILD_TYPE_CPP_DEFINE)

# Define installation directories
set(SHELLANYTHING_INSTALL_ROOT_DIR     "/")
set(SHELLANYTHING_INSTALL_BIN_DIR      "bin")
set(SHELLANYTHING_INSTALL_LIB_DIR      "lib/shellanything-${SHELLANYTHING_VERSION}")
set(SHELLANYTHING_INSTALL_INCLUDE_DIR  "include/shellanything-${SHELLANYTHING_VERSION}")
set(SHELLANYTHING_INSTALL_CMAKE_DIR    ${SHELLANYTHING_INSTALL_LIB_DIR}) # CMake files (*.cmake) should have the same destination as the library files. Some also prefers to use "cmake".
set(SHELLANYTHING_INSTALL_DOC_DIR      "docs")

##############################################################################################################################################
# Project settings
##############################################################################################################################################

# Build options
option(SHELLANYTHING_BUILD_DOC "Build ShellAnything documentation" OFF)
option(SHELLANYTHING_BUILD_TEST "Build all ShellAnything's unit tests" OFF)

# Force a debug postfix if none specified.
# This allows publishing both release and debug binaries to the same location
# and it helps to prevent linking with the wrong library on Windows.
if(NOT CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX "-d")
endif()

# Prevents annoying warnings on MSVC
if (WIN32)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# Define include directories for source code.
# The specified values will not be exported.
set( SHELLANYTHING_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include )
include_directories(${SHELLANYTHING_INCLUDE_DIR}                # public header files, for source code.
                    ${CMAKE_BINARY_DIR}/include                 # for ${SHELLANYTHING_VERSION_HEADER} and ${SHELLANYTHING_CONFIG_HEADER} generated files.
)

# Subprojects
add_subdirectory(src)

if(SHELLANYTHING_BUILD_TEST)
  add_subdirectory(test)
endif()

##############################################################################################################################################
# Support for static and shared library
##############################################################################################################################################

if (BUILD_SHARED_LIBS)
  set(SHELLANYTHING_EXPORT_HEADER_FILENAME "export.h")
  set(SHELLANYTHING_EXPORT_HEADER ${CMAKE_BINARY_DIR}/include/shellanything/${SHELLANYTHING_EXPORT_HEADER_FILENAME})
  message("Generating ${SHELLANYTHING_EXPORT_HEADER_FILENAME} for shared library...")
  include (GenerateExportHeader) 
  GENERATE_EXPORT_HEADER(shellanything 
               BASE_NAME shellanything 
               EXPORT_MACRO_NAME SHELLANYTHING_EXPORT 
               EXPORT_FILE_NAME ${SHELLANYTHING_EXPORT_HEADER} 
               STATIC_DEFINE SHELLANYTHING_BUILT_AS_STATIC
  )
endif()

##############################################################################################################################################
# Generate doxygen documentation
# See https://vicrucann.github.io/tutorials/quick-cmake-doxygen/
##############################################################################################################################################
if (SHELLANYTHING_BUILD_DOC)
  # check if Doxygen is installed
  find_package(Doxygen)
  if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
 
    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")
 
    # note the option ALL which allows to build the docs together with the application
    add_custom_target( shellanything_doc ALL
      COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating API documentation with Doxygen"
      VERBATIM )
  else (DOXYGEN_FOUND)
    message("Doxygen need to be installed to generate the doxygen documentation")
  endif (DOXYGEN_FOUND)
endif()

##############################################################################################################################################
# Convert markdown files to html for publishing with installers.
##############################################################################################################################################

# Set the default filenames to be published with the install/package.
set (README_FILENAME README.md)
set (USERMANUAL_FILENAME UserManual.md)

if (GRIP_FOUND)
  # Convert README.md
  execute_process(COMMAND grip README.md --export README.html --user-content --context=end2endzone/ShellAnything RESULT_VARIABLE res_var WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  if("${res_var}" STREQUAL "0")
    set (README_FILENAME README.html)
  else()
    message(FATAL_ERROR "grip failed converting README.md with res_var='${res_var}'")
  endif()

  # Convert UserManual.md
  execute_process(COMMAND grip UserManual.md --export UserManual.html --user-content --context=end2endzone/ShellAnything RESULT_VARIABLE res_var WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  if("${res_var}" STREQUAL "0")
    set (USERMANUAL_FILENAME UserManual.html)
  else()
    message(FATAL_ERROR "grip failed converting UserManual.md with res_var='${res_var}'")
  endif()
else()
  message(WARNING "grip is not found on the system. grip is required for publishing markdown files to html. Files will be published as *.md files.")
endif()


##############################################################################################################################################
# Install
##############################################################################################################################################

# Install locations:   See https://unix.stackexchange.com/a/36874
#   On UNIX, installs to "/usr/local".
#   On Windows, installs to "C:\Program Files (x86)\${PROJECT_NAME}" or to "C:\Program Files\${PROJECT_NAME}" for 64 bit binaries

# Target config version verification file
configure_file(${CMAKE_SOURCE_DIR}/cmake/shellanything-config-version.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/cmake/shellanything-config-version.cmake @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cmake/shellanything-config-version.cmake DESTINATION ${SHELLANYTHING_INSTALL_CMAKE_DIR})

# Target config file
configure_file(${CMAKE_SOURCE_DIR}/cmake/shellanything-config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/cmake/shellanything-config.cmake @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cmake/shellanything-config.cmake DESTINATION ${SHELLANYTHING_INSTALL_CMAKE_DIR})

install(FILES ${CMAKE_SOURCE_DIR}/resources/register.bat
              ${CMAKE_SOURCE_DIR}/resources/unregister.bat
              DESTINATION ${SHELLANYTHING_INSTALL_BIN_DIR})

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/shellanything DESTINATION ${SHELLANYTHING_INSTALL_INCLUDE_DIR})
install(FILES ${SHELLANYTHING_EXPORT_HEADER}
              ${SHELLANYTHING_VERSION_HEADER}
              ${SHELLANYTHING_CONFIG_HEADER}
              DESTINATION ${SHELLANYTHING_INSTALL_INCLUDE_DIR}/shellanything)
install(EXPORT shellanything-targets DESTINATION ${SHELLANYTHING_INSTALL_CMAKE_DIR})
install(DIRECTORY ${CMAKE_SOURCE_DIR}/licenses DESTINATION ${SHELLANYTHING_INSTALL_ROOT_DIR})
install(FILES ${CMAKE_SOURCE_DIR}/docs/screenshot_file.png
              ${CMAKE_SOURCE_DIR}/docs/screenshot_folder.png
              ${CMAKE_SOURCE_DIR}/docs/ShellAnything-splashscreen.jpg
              DESTINATION ${SHELLANYTHING_INSTALL_DOC_DIR})

install(FILES ${CMAKE_SOURCE_DIR}/README.md
              ${CMAKE_SOURCE_DIR}/UserManual.md
              ${CMAKE_SOURCE_DIR}/${README_FILENAME}
              ${CMAKE_SOURCE_DIR}/${USERMANUAL_FILENAME}
              DESTINATION ${SHELLANYTHING_INSTALL_ROOT_DIR})

##############################################################################################################################################
# Packaging
##############################################################################################################################################

set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${SHELLANYTHING_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR "${SHELLANYTHING_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${SHELLANYTHING_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${SHELLANYTHING_VERSION_PATCH}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "ShellAnything - ShellAnything allows one to integrate third party applications with Windows File Explorer.")
set(CPACK_PACKAGE_VENDOR "end2endzone")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_GENERATOR "ZIP")
if (NSIS_FOUND)
  list(APPEND CPACK_GENERATOR "NSIS")
endif (NSIS_FOUND)
if (WIX_FOUND)
  list(APPEND CPACK_GENERATOR "WIX")
endif (WIX_FOUND)

#######################
# NSIS specific stuff #
#######################

set(CPACK_NSIS_CONTACT "end2endzone@gmail.com")
set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/end2endzone/ShellAnything")
set(CPACK_NSIS_HELP_LINK "https://github.com/end2endzone/ShellAnything")
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)

# Icon in the add/remove control panel. Must be an .exe file.
# Use the main dll as we do not have main exe.
# The filename will be wrong in debug builds.
set(CPACK_NSIS_INSTALLED_ICON_NAME bin\\\\shellext.dll)

# Define our "main" start menu shortcuts for our execuables.
# NSIS uses CPACK_PACKAGE_EXECUTABLES to create start menu shortcuts.
# It only supports creating shortcuts to target executable. Nothing else.
# To create custom shortcuts, including shortcuts with arguments, one must use another method.
# For more flexibility, see the following: 
# https://stackoverflow.com/questions/2640760/creating-windows-desktop-icon-in-cmake-cpack-nsis
set(CPACK_PACKAGE_EXECUTABLES
  # CMake generator expression such as '$<TARGET_FILE_NAME:shellext>' do not work with CPACK_PACKAGE_EXECUTABLES.
  # "$<TARGET_FILE_NAME:shellanything_unittest>" "Run Unit Tests"

  # Won't work in debug build but its the best we dan do.
  "shellanything_unittest" "Run Unit Tests"
)

# Define our secondaries start menu shortcuts (for files that are not direct target executables).
# See the following for details:
# https://stackoverflow.com/questions/14831739/how-do-i-add-a-start-menu-entry-for-a-text-file-in-cpack-ans-nsis
# https://stackoverflow.com/questions/2640760/creating-windows-desktop-icon-in-cmake-cpack-nsis
set(CPACK_NSIS_MENU_LINKS
  "${README_FILENAME}" "${README_FILENAME}"
  "${USERMANUAL_FILENAME}" "${USERMANUAL_FILENAME}"
  "licenses" "Third party licenses"
  "https://github.com/end2endzone/ShellAnything" "ShellAnything Web Site"
  "bin/register.bat"   "Register shell extension"
  "bin/unregister.bat" "Unregister shell extension"
)

#######################
# WIX specific stuff #
#######################

# Set variables for WIX installer
set(CPACK_WIX_UPGRADE_GUID "EBEF1698-CA72-480A-9FE3-E8A08E7E03ED")
set(CPACK_WIX_LICENSE_RTF  "${CMAKE_SOURCE_DIR}/LICENSE.rtf")
set(CPACK_WIX_PRODUCT_ICON "${CMAKE_SOURCE_DIR}/resources/icons/0.2.0.ico")
set(CPACK_WIX_HELP_LINK "https://github.com/end2endzone/ShellAnything")

# Do not set 'CPACK_WIX_PRODUCT_GUID' to a valid GUID (ie "C532D05F-E319-4A0E-BD41-14B0B26F245B")
# Set CPACK_WIX_UPGRADE_GUID to something fixed while leaving CPACK_WIX_PRODUCT_GUID unset
# (so that you get a unique new product GUID for every package you create).
# This would cause an existing installation (with the same upgrade GUID) to be replaced 
# (files of the old package are removed)
# rather than installed in parallel.
# See https://gitlab.kitware.com/cmake/cmake/-/issues/18081 for details.

# Define start menu shortcuts for WIX
set_property(INSTALL "bin/$<TARGET_FILE_NAME:shellext>"                 PROPERTY CPACK_START_MENU_SHORTCUTS "Run Unit Tests"            )
set_property(INSTALL "${README_FILENAME}"                               PROPERTY CPACK_START_MENU_SHORTCUTS "${README_FILENAME}"        )
set_property(INSTALL "${USERMANUAL_FILENAME}"                           PROPERTY CPACK_START_MENU_SHORTCUTS "${USERMANUAL_FILENAME}"    )
set_property(INSTALL "./licenses"                                       PROPERTY CPACK_START_MENU_SHORTCUTS "Third party licenses"      ) # This one does not work!
set_property(INSTALL "https://github.com/end2endzone/ShellAnything"     PROPERTY CPACK_START_MENU_SHORTCUTS "ShellAnything Web Site"    )
set_property(INSTALL "bin/register.bat"                                 PROPERTY CPACK_START_MENU_SHORTCUTS "Register shell extension"  )
set_property(INSTALL "bin/unregister.bat"                               PROPERTY CPACK_START_MENU_SHORTCUTS "Unregister shell extension")

# we don't want to split our program up into several things
set(CPACK_MONOLITHIC_INSTALL 1)

# This must be last
include(CPack)
