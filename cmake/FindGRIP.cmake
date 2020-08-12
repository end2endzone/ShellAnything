#-------------------------------------------------------------------
# This file is part of the CMake build system for ShellAnything
# For the latest info, see https://github.com/end2endzone/ShellAnything
#
# The contents of this file are placed in the public domain. Feel
# free to make use of it in any way you like.
#-------------------------------------------------------------------
#
# - Try to find Grip 
# See https://github.com/joeyespo/grip and https://pypi.org/project/grip/
# You can help this by defining GRIP_DIR in the environment / CMake
# Once done, this will define
#
#  GRIP_FOUND - True if GRIP is found
#  GRIP_PROGRAM - The path of the grip program
#
#-------------------------------------------------------------------

# Search for the main executable
find_program(GRIP_PROGRAM
  NAMES
    grip
  PATHS
    $ENV{GRIP_DIR}
    $ENV{PATH}
)
mark_as_advanced(GRIP_PROGRAM)
#message("GRIP_PROGRAM=" "${GRIP_PROGRAM}")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GRIP DEFAULT_MSG GRIP_PROGRAM)

if (GRIP_PROGRAM)
  set(GRIP_FOUND TRUE)
endif (GRIP_PROGRAM)
