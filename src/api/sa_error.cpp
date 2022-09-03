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

#include "shellanything/sa_error.h"
#include "ErrorManager.h"

using namespace shellanything;

const char* sa_error_get_error_description(sa_error_t code)
{
  switch (code)
  {
  case SA_ERROR_SUCCESS:      return "No error. The operation completed successfully"; break;
  case SA_ERROR_INVALID_ARGUMENTS:      return "Invalid arguments"; break;
  case SA_ERROR_BUFFER_TOO_SMALL:      return "The given buffer is too small"; break;
  case SA_ERROR_VALUE_OUT_OF_BOUNDS:      return "The given value is out of bounds"; break;
  case SA_ERROR_NOT_FOUND:      return "The specified resource is not found"; break;
  case SA_ERROR_NOT_SUPPORTED:      return "This operation is not supported"; break;
  case SA_ERROR_NOT_IMPLEMENTED:      return "This operation is not implemented"; break;
  case SA_ERROR_EMPTY:      return "A list or collection is empty."; break;
  case SA_ERROR_ALREADY_EXIST:      return "The object or resource already exists"; break;
  case SA_ERROR_MISSING_RESOURCE:      return "A resource is missing to complete the operation"; break;
  case SA_ERROR_ACCESS_DENIED:      return "The access to object or resource is denied"; break;
  case SA_ERROR_BUSY:      return "The object is busy"; break;
  case SA_ERROR_TIME_OUT:      return "The operation has timed out"; break;
  case SA_ERROR_CANT_READ:      return "Can not read from the resource."; break;
  case SA_ERROR_CANT_WRITE:      return "Can not write to the resource."; break;
  case SA_ERROR_INVALID_RESOURCE:      return "The resource is invalid."; break;
  case SA_ERROR_ALREADY_RUNNING:      return "The resource is already running."; break;
  case SA_ERROR_ABORTED:      return "The operation was aborted"; break;
  case SA_ERROR_CANCELLED:      return "The operation was cancelled"; break;
  case SA_ERROR_UNKNOWN:      return "Unknown error"; break;
  default:
    return "Unspecified error";
  };
}

void sa_error_show_message(const char* title, const char* message)
{
  std::string title_str;
  std::string message_str;

  if (title)
    title_str = title;
  if (message)
    message_str = message;

  ShowErrorMessage(title, message);
}

void sa_error_show_message_utf8(const char* title, const char* message)
{
  std::string title_str;
  std::string message_str;

  if (title)
    title_str = title;
  if (message)
    message_str = message;

  ShowErrorMessageUtf8(title, message);
}
