#-------------------------------------------------------------------
# This file is part of the CMake build system for ShellAnything
# For the latest info, see https://github.com/end2endzone/ShellAnything
#
# The contents of this file are placed in the public domain. Feel
# free to make use of it in any way you like.
#-------------------------------------------------------------------
#
# - Try to find WIX Toolset
# You can help this by defining WIX in the environment / CMake
# Once done, this will define
#
#  WIX_FOUND - True if WIX is found
#  WIX_BIN_DIR - The directory that contains candle.exe and light.exe
#  WIX_ROOT_PATH - The directory location where WIX was found
#
#-------------------------------------------------------------------

if (WIN32)
  # Define possible search locations in the registry
  set(_WIX_ROOT_HINTS
    "[HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Microsoft\\Windows Installer XML;InstallFolder")
  
  # Search for directories matching 'WiX Toolset*' in both Program Files directories.
  file(GLOB _WIX_DIRECTORIES_X64 "C:/Program Files/WiX Toolset*")
  file(GLOB _WIX_DIRECTORIES_X86 "C:/Program Files (x86)/WiX Toolset*")
  
  # Read WIX environment variable if defined to an existing path
  set(_WIX_ENV_PATH "") # Set to an empty value by default
  if(EXISTS "$ENV{WIX}")
    # The environment variable WIX can end with a '\' character.
    # This backslash character is escaping the next character which is the list separator ';'.
    set(_WIX_ENV_PATH "$ENV{WIX}")
    string(REPLACE "\\" "/" _WIX_ENV_PATH ${_WIX_ENV_PATH})
  endif(EXISTS "$ENV{WIX}")
  
  # Define know directory locations where it could be installed.
  set(_WIX_ROOT_PATHS
    ${_WIX_ENV_PATH}
    ${_WIX_DIRECTORIES_X86}
    ${_WIX_DIRECTORIES_X64}
  )
  #message("_WIX_ROOT_PATHS=" "${_WIX_ROOT_PATHS}")
  
  # Search for the ROOT installation directory
  find_path(WIX_ROOT_PATH
    NAMES
      bin/candle.exe
    HINTS
      ${_WIX_ROOT_HINTS}
    PATHS
      ${_WIX_ROOT_PATHS}
  )
  mark_as_advanced(WIX_ROOT_PATH)
  #message("WIX_ROOT_PATH=" "${WIX_ROOT_PATH}")
  
  if (WIX_ROOT_PATH)
    set(WIX_BIN_DIR ${WIX_ROOT_PATH}/bin)
  endif (WIX_ROOT_PATH)
  
  # Search for the main executables
  find_program(WIX_CANDLE
    NAMES
      candle
    PATHS
      ${WIX_BIN_DIR}
  )
  #message("WIX_CANDLE=" "${WIX_CANDLE}")
  mark_as_advanced(WIX_CANDLE)
  find_program(WIX_LIGHT
    NAMES
      light
    PATHS
      ${WIX_BIN_DIR}
  )
  #message("WIX_LIGHT=" "${WIX_LIGHT}")
  mark_as_advanced(WIX_LIGHT)
endif (WIN32)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(WIX DEFAULT_MSG WIX_CANDLE WIX_LIGHT WIX_ROOT_PATH WIX_BIN_DIR)

if (WIX_CANDLE)
  set(WIX_FOUND TRUE)
endif (WIX_CANDLE)
