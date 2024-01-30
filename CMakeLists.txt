# CMakeLists.txt
cmake_minimum_required(VERSION 3.4.3 FATAL_ERROR)
project(ShellAnything)

#Find optional packages.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(WIX)
find_package(GRIP)

##############################################################################################################################################
# Dependencies
##############################################################################################################################################

# packages directory
set(PACKAGE_DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/external) 
if(NOT EXISTS ${PACKAGE_DOWNLOAD_DIR})
  file(MAKE_DIRECTORY ${PACKAGE_DOWNLOAD_DIR})
endif()

# exprtk
# The exprtk library do not provides version based releases.
# At the time of this writing, HEAD revision is commit d312ba91419c9cb12c8279fd3a19096d39dfcb5e 
# available at https://github.com/ArashPartow/exprtk/archive/d312ba91419c9cb12c8279fd3a19096d39dfcb5e.zip
# Using latest revision to get all bug fixes.
set(EXPRTK_PACKAGE_URL "https://github.com/ArashPartow/exprtk/archive/master.zip")
set(EXPRTK_PACKAGE_PATH ${PACKAGE_DOWNLOAD_DIR}/exprtk.zip)
if(NOT EXISTS ${EXPRTK_PACKAGE_PATH})
  message(STATUS "Downloading exprtk package from ${EXPRTK_PACKAGE_URL}")
  file(DOWNLOAD ${EXPRTK_PACKAGE_URL} ${EXPRTK_PACKAGE_PATH}
     TIMEOUT 60  # seconds
     TLS_VERIFY ON
  )
  
  message(STATUS "Extracting exprtk package to ${EXPRTK_PACKAGE_PATH}")
  execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf 
    ${EXPRTK_PACKAGE_PATH}
    WORKING_DIRECTORY ${PACKAGE_DOWNLOAD_DIR}
  )
endif()
set(EXPRTK_HEADER_PATH ${PACKAGE_DOWNLOAD_DIR}/exprtk-master/exprtk.hpp)
if(EXISTS ${EXPRTK_HEADER_PATH})
  message(STATUS "Found exprtk header file: ${EXPRTK_HEADER_PATH}")
else()
  message(FATAL_ERROR "exprtk header file not found.")
endif()

# pcg-cpp
# The pcg-cpp library has not created tags since 2015 even if the number of commits has double since the last release.
# At the time of this writing, HEAD revision is commit 428802d1a5634f96bcd0705fab379ff0113bcf13 
# available at https://github.com/imneme/pcg-cpp/archive/428802d1a5634f96bcd0705fab379ff0113bcf13.zip
# Using latest revision to get all bug fixes.
set(PCGCPP_PACKAGE_URL "https://github.com/imneme/pcg-cpp/archive/master.zip")
set(PCGCPP_PACKAGE_PATH ${PACKAGE_DOWNLOAD_DIR}/pcg-cpp.zip)
if(NOT EXISTS ${PCGCPP_PACKAGE_PATH})
  message(STATUS "Downloading pcg-cpp package from ${PCGCPP_PACKAGE_URL}")
  file(DOWNLOAD ${PCGCPP_PACKAGE_URL} ${PCGCPP_PACKAGE_PATH}
     TIMEOUT 60  # seconds
     TLS_VERIFY ON
  )
  
  message(STATUS "Extracting pcg-cpp package to ${PCGCPP_PACKAGE_PATH}")
  execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf 
    ${PCGCPP_PACKAGE_PATH}
    WORKING_DIRECTORY ${PACKAGE_DOWNLOAD_DIR}
  )
endif()
set(PCGCPP_HEADER_PATH ${PACKAGE_DOWNLOAD_DIR}/pcg-cpp-master/include/pcg_random.hpp)
if(EXISTS ${PCGCPP_HEADER_PATH})
  message(STATUS "Found pcg-cpp header file: ${PCGCPP_HEADER_PATH}")
  get_filename_component(PCGCPP_INCLUDE_DIR ${PCGCPP_HEADER_PATH} PATH)
else()
  message(FATAL_ERROR "pcg-cpp header file not found.")
endif()

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
set(SHELLANYTHING_VERSION_MINOR 9)
set(SHELLANYTHING_VERSION_PATCH 0)
set(SHELLANYTHING_VERSION ${SHELLANYTHING_VERSION_MAJOR}.${SHELLANYTHING_VERSION_MINOR}.${SHELLANYTHING_VERSION_PATCH})

# Create a c++ file header from the project LICENSE file.
# The c++ header will be added to all generated files.
include(MakeCplusplusHeader)
MakeCplusplusHeader(${CMAKE_SOURCE_DIR}/LICENSE LICENSE_CPLUSPLUS_HEADER)

# version.h file
set(SHELLANYTHING_VERSION_HEADER ${CMAKE_BINARY_DIR}/include/shellanything/version.h)
message("Generating ${SHELLANYTHING_VERSION_HEADER}...")
configure_file( ${CMAKE_SOURCE_DIR}/src/version.h.in ${SHELLANYTHING_VERSION_HEADER} )

# config.h file
set(SHELLANYTHING_CONFIG_HEADER ${CMAKE_BINARY_DIR}/include/shellanything/config.h)
message("Generating ${SHELLANYTHING_CONFIG_HEADER}...")
configure_file( ${CMAKE_SOURCE_DIR}/src/config.h.in ${SHELLANYTHING_CONFIG_HEADER} )

# VERSION.TXT file
set(SHELLANYTHING_VERSION_TXT ${CMAKE_BINARY_DIR}/VERSION.TXT)
message("Generating ${SHELLANYTHING_VERSION_TXT}...")
configure_file( ${CMAKE_SOURCE_DIR}/src/VERSION.TXT.in ${SHELLANYTHING_VERSION_TXT} )

# Define installation directories
set(SHELLANYTHING_INSTALL_ROOT_DIR     "/")
set(SHELLANYTHING_INSTALL_BIN_DIR      "bin")
set(SHELLANYTHING_INSTALL_LIB_DIR      "lib/shellanything-${SHELLANYTHING_VERSION}")
set(SHELLANYTHING_INSTALL_INCLUDE_DIR  "include/shellanything-${SHELLANYTHING_VERSION}")
set(SHELLANYTHING_INSTALL_CMAKE_DIR    ${SHELLANYTHING_INSTALL_LIB_DIR}) # CMake files (*.cmake) should have the same destination as the library files. Some also prefers to use "cmake".
set(SHELLANYTHING_INSTALL_DOC_DIR      "docs")

# magic.mgc file
set(LIBMAGIC_MGC_DIR "${CMAKE_SOURCE_DIR}/third_parties/libmagic/install/binary")

##############################################################################################################################################
# Project settings
##############################################################################################################################################

# Build options
option(SHELLANYTHING_BUILD_DOC "Build ${PROJECT_NAME} documentation" OFF)
option(SHELLANYTHING_BUILD_TEST "Build all ${PROJECT_NAME}'s unit tests" OFF)
option(SHELLANYTHING_BUILD_PLUGINS "Build all ${PROJECT_NAME}'s plugins" OFF)

# Force a debug postfix if none specified.
# This allows publishing both release and debug binaries to the same location
# and it helps to prevent linking with the wrong library on Windows.
if(NOT CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX "-d")
endif()

# Define EXEC_FILENAME_POSTFIX to accurately guess the end of filename of library/executable files
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(EXEC_FILENAME_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
else()
  set(EXEC_FILENAME_POSTFIX "")
endif()
MESSAGE( STATUS "EXEC_FILENAME_POSTFIX:        " ${EXEC_FILENAME_POSTFIX} )

# Prevents annoying warnings on MSVC
if (WIN32)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  add_definitions(-D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING)
endif()

# .editorconfig support on Windows
if (CMAKE_GENERATOR MATCHES "Visual Studio")
	# Unfortunately CMake doesn't allow us to add solution items directly, so
	# instead we abuse that it doesn't validate the TYPE parameter and instead
	# passes it straight through into the .sln file.
  # In other words, we inject the following into the solution :
  #   Project("{2150E333-8FDC-42A3-9474-1A3956D46DE8}") = "Solution Items", "Solution Items", "{A63B4829-6639-4213-A13B-6AD6104D0BC8}"
  #   	ProjectSection(SolutionItems) = preProject
  #   		..\.editorconfig = ..\.editorconfig
  #   	EndProjectSection
  #   EndProject

  set(VS_SolutionFolder_TYPE_GUID "2150E333-8FDC-42A3-9474-1A3956D46DE8")
  set(my_SolutionItems_GUID "A63B4829-6639-4213-A13B-6AD6104D0BC8")
  set(EDITORCONFIG_FILE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/.editorconfig")
  set(MSVC_SLN_INJECTION_CODE "${VS_SolutionFolder_TYPE_GUID}}\") = \"Solution Items\", \"Solution Items\", \"{${my_SolutionItems_GUID}}\"\n 	ProjectSection(SolutionItems) = preProject\n		${EDITORCONFIG_FILE_PATH} = ${EDITORCONFIG_FILE_PATH}\n	EndProjectSection\nEndProject\nProject(\"{")
    
  include_external_msproject(
    "Solution Items (dummy)" "Solution Items (dummy)"
    TYPE "${MSVC_SLN_INJECTION_CODE}${VS_SolutionFolder_TYPE_GUID}"
  )
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
  add_subdirectory(src/tests)
endif()

##############################################################################################################################################
# Support for static and shared library
##############################################################################################################################################

set(SHELLANYTHING_EXPORT_HEADER_FILENAME "export.h")
set(SHELLANYTHING_EXPORT_HEADER ${CMAKE_BINARY_DIR}/include/shellanything/${SHELLANYTHING_EXPORT_HEADER_FILENAME})
message("Generating ${SHELLANYTHING_EXPORT_HEADER_FILENAME} for shared library...")
include (GenerateExportHeader) 
GENERATE_EXPORT_HEADER( sa.core 
                        BASE_NAME shellanything 
                        EXPORT_MACRO_NAME SHELLANYTHING_EXPORT 
                        EXPORT_FILE_NAME ${SHELLANYTHING_EXPORT_HEADER} 
)

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
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      COMMENT "Generating API documentation with Doxygen"
      VERBATIM )
  else (DOXYGEN_FOUND)
    message("Doxygen need to be installed to generate the doxygen documentation")
  endif (DOXYGEN_FOUND)
endif()

##############################################################################################################################################
# Package sa_plugin_demo files into a zip
##############################################################################################################################################

function(create_zip target_name output_file input_files working_dir)
  add_custom_command(
    # COMMAND ${CMAKE_COMMAND} -E echo create_zip && exit 42
    COMMAND ${CMAKE_COMMAND} -E tar "cvf" "${output_file}" --format=zip -- ${input_files}
    WORKING_DIRECTORY "${working_dir}"
    OUTPUT  "${output_file}"
    DEPENDS ${input_files}
    COMMENT "Creating zip archive: ${output_file}."
  )
  add_custom_target(${target_name} ALL DEPENDS "${output_file}")
endfunction()

# Find all files in sa_plugin_demo directory
set(PLUGIN_DEMO_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/sa_plugin_demo")
file(GLOB PLUGIN_DEMO_FILES "${PLUGIN_DEMO_DIR}/*")
list(REMOVE_ITEM PLUGIN_DEMO_FILES "${PLUGIN_DEMO_DIR}/build")
list(REMOVE_ITEM PLUGIN_DEMO_FILES "${PLUGIN_DEMO_DIR}/install")
MESSAGE("Found sa_plugin_demo files: ${PLUGIN_DEMO_FILES}" )
create_zip(PACKAGE_sa_plugin_demo "${CMAKE_CURRENT_BINARY_DIR}/sa_plugin_demo.zip" "${PLUGIN_DEMO_FILES}" "${PLUGIN_DEMO_DIR}")

##############################################################################################################################################
# Convert markdown/text files to html/txt for publishing with installers.
##############################################################################################################################################

# Set the default filenames to be published with the install/package.
set (README_FILENAME README.md)
set (USERMANUAL_FILENAME UserManual.md)
set (CONTRIBUTING_FILENAME CONTRIBUTING.md)

if (GRIP_FOUND)
  # Convert README.md
  execute_process(COMMAND grip README.md --export ${CMAKE_BINARY_DIR}/README.html --user-content --context=end2endzone/ShellAnything RESULT_VARIABLE res_var WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  if("${res_var}" STREQUAL "0")
    set (README_FILENAME README.html)
  else()
    message(FATAL_ERROR "grip failed converting README.md with res_var='${res_var}'")
  endif()

  # Convert UserManual.md
  execute_process(COMMAND grip UserManual.md --export ${CMAKE_BINARY_DIR}/UserManual.html --user-content --context=end2endzone/ShellAnything RESULT_VARIABLE res_var WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  if("${res_var}" STREQUAL "0")
    set (USERMANUAL_FILENAME UserManual.html)
  else()
    message(FATAL_ERROR "grip failed converting UserManual.md with res_var='${res_var}'")
  endif()

  # Convert CONTRIBUTING.md
  execute_process(COMMAND grip CONTRIBUTING.md --export ${CMAKE_BINARY_DIR}/CONTRIBUTING.html --user-content --context=end2endzone/ShellAnything RESULT_VARIABLE res_var WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  if("${res_var}" STREQUAL "0")
    set (CONTRIBUTING_FILENAME CONTRIBUTING.html)
  else()
    message(FATAL_ERROR "grip failed converting CONTRIBUTING.md with res_var='${res_var}'")
  endif()
else()
  message(WARNING "grip is not found on the system. grip is required for publishing markdown files to html. Files will be published as *.md files.")
endif()

# AUTHORS.txt
set(SHELLANYTHING_AUTHOR_TXT ${CMAKE_BINARY_DIR}/AUTHORS.txt)
message("Exporting to ${SHELLANYTHING_AUTHOR_TXT}...")
configure_file( ${CMAKE_SOURCE_DIR}/AUTHORS ${SHELLANYTHING_AUTHOR_TXT} )
get_filename_component(AUTHOR_FILENAME ${SHELLANYTHING_AUTHOR_TXT} NAME)

# CHANGES.txt
set(SHELLANYTHING_CHANGES_TXT ${CMAKE_BINARY_DIR}/CHANGES.txt)
message("Exporting to ${SHELLANYTHING_CHANGES_TXT}...")
configure_file( ${CMAKE_SOURCE_DIR}/CHANGES ${SHELLANYTHING_CHANGES_TXT} )
get_filename_component(CHANGES_FILENAME ${SHELLANYTHING_CHANGES_TXT} NAME)

# LICENSE.txt
set(SHELLANYTHING_LICENSE_TXT ${CMAKE_BINARY_DIR}/LICENSE.txt)
message("Exporting to ${SHELLANYTHING_LICENSE_TXT}...")
configure_file( ${CMAKE_SOURCE_DIR}/LICENSE ${SHELLANYTHING_LICENSE_TXT} )
get_filename_component(LICENSE_FILENAME ${SHELLANYTHING_LICENSE_TXT} NAME)

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
              ${CMAKE_SOURCE_DIR}/resources/icons/shellanything.ico
              ${CMAKE_CURRENT_BINARY_DIR}/sa_plugin_demo.zip
              DESTINATION ${SHELLANYTHING_INSTALL_DOC_DIR})

install(FILES ${CMAKE_SOURCE_DIR}/README.md
              ${CMAKE_SOURCE_DIR}/UserManual.md
              ${CMAKE_SOURCE_DIR}/CONTRIBUTING.md
              ${CMAKE_BINARY_DIR}/${README_FILENAME}
              ${CMAKE_BINARY_DIR}/${USERMANUAL_FILENAME}
              ${CMAKE_BINARY_DIR}/${CONTRIBUTING_FILENAME}
              ${CMAKE_BINARY_DIR}/${AUTHOR_FILENAME}
              ${CMAKE_BINARY_DIR}/${CHANGES_FILENAME}
              ${CMAKE_BINARY_DIR}/${LICENSE_FILENAME}
              ${SHELLANYTHING_VERSION_TXT}
              DESTINATION ${SHELLANYTHING_INSTALL_ROOT_DIR})

if (SHELLANYTHING_BUILD_DOC AND DOXYGEN_FOUND)
  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doxygen_out/html DESTINATION ${SHELLANYTHING_INSTALL_DOC_DIR}/api)
endif()

##############################################################################################################################################
# Packaging
##############################################################################################################################################

set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VERSION ${SHELLANYTHING_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR "${SHELLANYTHING_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${SHELLANYTHING_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${SHELLANYTHING_VERSION_PATCH}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_NAME} is a C++ open-source software which allow one to easily customize and add new options to *Windows Explorer* context menu. Define specific actions when a user right-click on a file or a directory.")
set(CPACK_PACKAGE_VENDOR "end2endzone")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PROJECT_NAME}")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
set(CPACK_GENERATOR "ZIP")

if (WIX_FOUND)
  list(APPEND CPACK_GENERATOR "WIX")
endif (WIX_FOUND)

####################################
# Common generator packaging stuff #
####################################

# Define our "main" start menu shortcuts for our execuables.
# NSIS uses CPACK_PACKAGE_EXECUTABLES to create start menu shortcuts.
# It only supports creating shortcuts to target executable. Nothing else.
# To create custom shortcuts, including shortcuts with arguments, one must use another method.
# For more flexibility, see the following: 
# https://stackoverflow.com/questions/2640760/creating-windows-desktop-icon-in-cmake-cpack-nsis
set(CPACK_PACKAGE_EXECUTABLES
  # CMake generator expression such as '$<TARGET_FILE_NAME:sa.shellextension>' do not work with CPACK_PACKAGE_EXECUTABLES.
  # "$<TARGET_FILE_NAME:sa.tests>" "Run Unit Tests"

  # Issue #60 Unit tests cannot execute from installation directory:
  # The shortcut created by NSIS does not "Start in" 'bin' directory which is a problem for unit tests.
  # See issue #60 for details.
  # The shortcut will be created manually. See CPACK_NSIS_CREATE_ICONS_EXTRA below.
  # "sa.tests${CMAKE_DEBUG_POSTFIX}.exe" "Run Unit Tests"
)

######################
# WIX specific stuff #
######################

# Set variables for WIX installer

# If we do not specify a value for CPACK_WIX_PRODUCT_GUID,
# cpack will generate a new random PRODUCT_GUID value for every package you create.
# If we do not set a PRODUCT_GUID (so that you get a unique new product GUID for every package you create)
# and use a fixed UPGRADE_GUID (the same in every builds),
# the installation of a newer version should silently uninstall the previous version.
# This would cause an existing installation (with the same upgrade GUID) to be replaced 
# (files of the old package are removed)
# rather than installed in parallel.
# See https://gitlab.kitware.com/cmake/cmake/-/issues/18081 for details.
# This is intended behavior for ShellAnything.
set(CPACK_WIX_UPGRADE_GUID "EBEF1698-CA72-480A-9FE3-E8A08E7E03ED")  # as per v0.7.0 and older

set(CPACK_WIX_LICENSE_RTF  "${CMAKE_SOURCE_DIR}/resources/wix_license_template.rtf")
set(CPACK_WIX_PRODUCT_ICON "${CMAKE_SOURCE_DIR}/resources/icons/shellanything.ico")
set(CPACK_WIX_HELP_LINK "https://github.com/end2endzone/ShellAnything")
set(CPACK_WIX_PROGRAM_MENU_FOLDER "${PROJECT_NAME}")

set(CPACK_WIX_PROPERTY_ARPCOMMENTS "${CPACK_PACKAGE_DESCRIPTION_SUMMARY}")
set(CPACK_WIX_PROPERTY_ARPHELPLINK "${CPACK_WIX_HELP_LINK}")
set(CPACK_WIX_PROPERTY_ARPURLINFOABOUT "${CPACK_WIX_HELP_LINK}")

# Define start menu shortcuts for WIX
# More shortcuts are defined in file "wix/WIX.extra.in"
set_property(INSTALL "bin/$<TARGET_FILE_NAME:sa.tests>"                 PROPERTY CPACK_START_MENU_SHORTCUTS "${PROJECT_NAME} unit tests")
set_property(INSTALL "bin/$<TARGET_FILE_NAME:file_explorer_renew>"      PROPERTY CPACK_START_MENU_SHORTCUTS "File Explorer Renew")
set_property(INSTALL "${README_FILENAME}"                               PROPERTY CPACK_START_MENU_SHORTCUTS "${README_FILENAME}"        )
set_property(INSTALL "${USERMANUAL_FILENAME}"                           PROPERTY CPACK_START_MENU_SHORTCUTS "${USERMANUAL_FILENAME}"    )
set_property(INSTALL "${CONTRIBUTING_FILENAME}"                         PROPERTY CPACK_START_MENU_SHORTCUTS "${CONTRIBUTING_FILENAME}"  )
set_property(INSTALL "${AUTHOR_FILENAME}"                               PROPERTY CPACK_START_MENU_SHORTCUTS "${AUTHOR_FILENAME}"        )
set_property(INSTALL "${CHANGES_FILENAME}"                              PROPERTY CPACK_START_MENU_SHORTCUTS "${CHANGES_FILENAME}"       )
set_property(INSTALL "${LICENSE_FILENAME}"                              PROPERTY CPACK_START_MENU_SHORTCUTS "${LICENSE_FILENAME}"       )
set_property(INSTALL "bin/register.bat"                                 PROPERTY CPACK_START_MENU_SHORTCUTS "Register ${PROJECT_NAME}"  )
set_property(INSTALL "bin/unregister.bat"                               PROPERTY CPACK_START_MENU_SHORTCUTS "Unregister ${PROJECT_NAME}")

# Define custom shortcuts
configure_file("${CMAKE_SOURCE_DIR}/wix/WIX.patches.in" "${CMAKE_BINARY_DIR}/wix/WIX.patches.wxs")
configure_file("${CMAKE_SOURCE_DIR}/wix/WIX.extra.in"   "${CMAKE_BINARY_DIR}/wix/WIX.extra.wxs"  )
set(CPACK_WIX_PATCH_FILE    "${CMAKE_BINARY_DIR}/wix/WIX.patches.wxs")
set(CPACK_WIX_EXTRA_SOURCES
  "${CMAKE_BINARY_DIR}/wix/WIX.extra.wxs"
  "${CMAKE_SOURCE_DIR}/wix/WixUI_FileExplorerRenewDlg.wxs"
  "${CMAKE_SOURCE_DIR}/wix/WixUI_ShellAnythingUI.wxs"
)
set(CPACK_WIX_EXTENSIONS "WiXUtilExtension") # This is required for the util:InternetShortcut in WIX.extra.in
set(CPACK_WIX_UI_REF "WixUI_ShellAnythingUI") # To show a custom install directory and list of features.

# Starting from version 0.8.0 we must disable the installation of ShellAnything if an existing version is already installed.
# See issues for details:
#  #101 (Deprecate setup.exe installer in favor of the Windows Installer (*.msi)) and
#  #113 (Upgrading from v0.7.0 to 0.8.0 installs at the wrong location),
# To implement this, we must change WIX <MajorUpgrade> node in the <Product> definition.
# CPack does not allow modifications to <MajorUpgrade> using the CPACK_WIX_PATCH_FILE or other mecanisms.
# So we must do it the hard way by changing its default WIX template.
#
# Copy cmake's WIX template...
set(CUSTOM_WIX_TEMPLATE_FILE_PATH "${CMAKE_BINARY_DIR}/WIX.template.in")
configure_file("${CMAKE_ROOT}/Modules/Internal/CPack/WIX.template.in" "${CUSTOM_WIX_TEMPLATE_FILE_PATH}" COPYONLY)
# Change the template as we need it.
file(READ "${CUSTOM_WIX_TEMPLATE_FILE_PATH}" CUSTOM_WIX_TEMPLATE_FILE_CONTENTS)
string(REPLACE
  "AllowSameVersionUpgrades=\"yes\""
  "AllowSameVersionUpgrades=\"yes\"\n\t\t\tDisallow=\"yes\"\n\t\t\tDisallowUpgradeErrorMessage=\"Another version of [ProductName] is already installed. Installation of this version cannot continue. To configure or remove the existing version of [ProductName], use Add/Remove Programs on the Control Panel.\""
  CUSTOM_WIX_TEMPLATE_FILE_CONTENTS "${CUSTOM_WIX_TEMPLATE_FILE_CONTENTS}")
file(WRITE "${CUSTOM_WIX_TEMPLATE_FILE_PATH}" "${CUSTOM_WIX_TEMPLATE_FILE_CONTENTS}")
# Apply the new custom template
set(CPACK_WIX_TEMPLATE "${CUSTOM_WIX_TEMPLATE_FILE_PATH}")

# we don't want to split our program up into several things
set(CPACK_MONOLITHIC_INSTALL 1)

# This must be last
include(CPack)
