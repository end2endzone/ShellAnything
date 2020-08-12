#-------------------------------------------------------------------
# This file is part of the CMake build system for ShellAnything
# For the latest info, see https://github.com/end2endzone/ShellAnything
#
# The contents of this file are placed in the public domain. Feel
# free to make use of it in any way you like.
#-------------------------------------------------------------------
#
# - Try to find NSIS
# You can help this by defining NSIS_HOME, NSIS_DIR or NSIS_ROOT in the environment / CMake
# Once done, this will define
#
#  NSIS_FOUND - True if NSIS is found
#  NSIS_MAKE - The NSIS executable
#  NSIS_ROOT_PATH - The directory location where NSIS was found
#
#-------------------------------------------------------------------

if (WIN32)
  # Define possible search locations in the registry
  set(_NSIS_ROOT_HINTS
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\NSIS;Default]")

  # Define know directory locations where it could be installed.
  set(_NSIS_ROOT_PATHS
    $ENV{NSIS_HOME}
    $ENV{NSIS_DIR}
    $ENV{NSIS_ROOT}
    $ENV{PROGRAMFILES}/NSIS
    "C:/Program Files/NSIS"
    "C:/Program Files (x86)/NSIS"
  )
  #message("_NSIS_ROOT_PATHS=" "${_NSIS_ROOT_PATHS}")
  
  # Search for the ROOT installation directory
  find_path(NSIS_ROOT_PATH
    NAMES
      Include/Library.nsh
    HINTS
      ${_NSIS_ROOT_HINTS}
    PATHS
      ${_NSIS_ROOT_PATHS}
  )
  mark_as_advanced(NSIS_ROOT_PATH)
  #message("NSIS_ROOT_PATH=" "${NSIS_ROOT_PATH}")

  # Search for the main executable
  find_program(NSIS_MAKE
    NAMES
      makensis
    PATHS
      ${NSIS_ROOT_PATH}
  )
  #message("NSIS_MAKE=" "${NSIS_MAKE}")
  mark_as_advanced(NSIS_MAKE)
endif (WIN32)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(NSIS DEFAULT_MSG NSIS_MAKE)

if (NSIS_MAKE)
  set(NSIS_FOUND TRUE)
endif (NSIS_MAKE)

