/**********************************************************************************
 * MIT License
 * 
 * Copyright (c) 2018 Antoine Beauchamp
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *********************************************************************************/

#include "DriveClass.h"
#include "Validator.h"

#include <Windows.h>

namespace shellanything
{

  inline bool IsDriveLetter(char c)
  {
    if (  (c >= 'a' && c <= 'z') ||
          (c >= 'A' && c <= 'Z')    )
      return true;
    return false;
  }

  std::string GetDriveLetter(const std::string & element)
  {
    std::string letter;
    if (element.size() >= 2 && element[1] == ':' && IsDriveLetter(element[0]))
      letter.append(1, element[0]);
    return letter;
  }

  std::string GetDrivePath(const std::string & element)
  {
    std::string letter;
    if (element.size() >= 3 && element[1] == ':' && IsDriveLetter(element[0]) && element[2] == '\\')
      letter.append(element.c_str(), 3);
    return letter;
  }

  bool IsNetworkPath(const std::string & path)
  {
    if (path.size() >= 2 && path.substr(0, 2) == "\\\\")
    {
      // The path starts with \\
      // Extract hostname
      const char * hostname = &path[2];
      if (hostname[0] != '\0')
      {
        // Get first character of hostname
        char first = hostname[0];
        if ((first >= 'a' && first <= 'z') ||
            (first >= 'A' && first <= 'Z') ||
            (first >= '0' && first <= '9')  )
        {
          return true;
        }
      }
    }
    return false;
  }

  DRIVE_CLASS GetDriveClassFromPath(const std::string & path)
  {
    // Patch for DRIVE_CLASS_NETWORK.
    // The function GetDriveTypeA() will return DRIVE_UNKNOWN when the given path is a network path.
    // For example \\localhost\shared\public will be reported as DRIVE_UNKNOWN.
    // To get the expected DRIVE_REMOTE returned value, the shared directory 'shared' must be mapped to a drive letter.
    // For example, mapping \\localhost\shared to Z:, calling GetDriveTypeA("Z:\public") will return DRIVE_REMOTE.
    // This is not what we want. The value DRIVE_CLASS_NETWORK is expected when the given path is a network path.
    if (IsNetworkPath(path))
      return DRIVE_CLASS_NETWORK;

    UINT dwDriveType = GetDriveTypeA(path.c_str());
    switch(dwDriveType)
    {
    case DRIVE_NO_ROOT_DIR:
      {
        // The function GetDriveTypeA() returns DRIVE_NO_ROOT_DIR on a path that does not exist,
        // if the given path is a file, or if the given path is a directory that is not the root directory.
        // The functions returns DRIVE_NO_ROOT_DIR even if the drive letter, of the given non-existing path, is a CD-ROM or DVD-Drive.
        // However, the function will return DRIVE_CDROM is the root path of the same path is used, even if there are no disk in the optical drive.
        // To work around these issues, try to resolve with the root path of the drive.
        std::string root_path = GetDrivePath(path);
        if (root_path.empty() || // We may be dealing with a network path
            root_path == path  ) // We are already using a root drive path
        {
          // There is nothing we can do.
          return DRIVE_CLASS_UNKNOWN;
        }
        
        // Try to resolve with the root path instead
        DRIVE_CLASS resolved = GetDriveClassFromPath(root_path);
        return resolved;
      }
      break;
    case DRIVE_REMOVABLE:
      return DRIVE_CLASS_REMOVABLE;
    case DRIVE_FIXED:
      return DRIVE_CLASS_FIXED;
    case DRIVE_REMOTE:
      return DRIVE_CLASS_NETWORK;
    case DRIVE_CDROM:
      return DRIVE_CLASS_OPTICAL;
    case DRIVE_RAMDISK:
      return DRIVE_CLASS_RAMDISK;

    default:
      return DRIVE_CLASS_UNKNOWN;
    };
  }

  DRIVE_CLASS GetDriveClassFromString(const std::string & value)
  {
    if (value == "drive:removable")
      return DRIVE_CLASS_REMOVABLE;
    if (value == "drive:fixed")
      return DRIVE_CLASS_FIXED;
    if (value == "drive:network")
      return DRIVE_CLASS_NETWORK;
    if (value == "drive:optical")
      return DRIVE_CLASS_OPTICAL;
    if (value == "drive:ramdisk")
      return DRIVE_CLASS_RAMDISK;
    if (value == "drive:unknown")
      return DRIVE_CLASS_UNKNOWN;

    return DRIVE_CLASS_UNKNOWN;
  }

  const char * ToString(DRIVE_CLASS & value)
  {
    switch(value)
    {
    case DRIVE_CLASS_REMOVABLE:
      return "drive:removable";
    case DRIVE_CLASS_FIXED:
      return "drive:fixed";
    case DRIVE_CLASS_NETWORK:
      return "drive:network";
    case DRIVE_CLASS_OPTICAL:
      return "drive:optical";
    case DRIVE_CLASS_RAMDISK:
      return "drive:ramdisk";
    default:
      return "drive:unknown";
    };
  }



} //namespace shellanything
