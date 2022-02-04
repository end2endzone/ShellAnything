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

#include "shellanything/sa_icon.h"
#include "sa_string_private.h"
#include "Icon.h"
#include "sa_types_private.h"

using namespace shellanything;

sa_icon_immutable_t sa_icon_to_immutable(sa_icon_t* icon)
{
  sa_icon_immutable_t output;
  output.opaque = icon->opaque;
  return output;
}

int sa_icon_is_valid(sa_icon_immutable_t* icon)
{
  bool is_valid = AS_CLASS_ICON(icon)->IsValid();
  if (is_valid)
    return 1;
  return 0;
}

sa_error_t sa_icon_get_file_extension_buffer(sa_icon_immutable_t* icon, int* length, char* buffer, size_t size)
{
  if (length)
    *length = -1;
  const std::string& fileextension = AS_CLASS_ICON(icon)->GetFileExtension();
  sa_error_t result = sa_cstr_copy_buffer(buffer, size, length, fileextension);
  return result;
}

sa_error_t sa_icon_get_file_extension_string(sa_icon_immutable_t* icon, sa_string_t* str)
{
  const std::string& fileextension = AS_CLASS_ICON(icon)->GetFileExtension();
  sa_cstr_copy_string(str, fileextension);
  return SA_ERROR_SUCCESS;
}

void sa_icon_set_file_extension(sa_icon_t* icon, const char* fileextension)
{
  extern sa_icon_immutable_t AS_TYPE_ICON(const shellanything::Icon * object);

  shellanything::Icon* tmp_class = AS_CLASS_ICON(icon);
  sa_icon_immutable_t tmp_type = AS_TYPE_ICON(tmp_class);

  AS_CLASS_ICON(icon)->SetFileExtension(fileextension);
}

sa_error_t sa_icon_get_path_buffer(sa_icon_immutable_t* icon, int* length, char* buffer, size_t size)
{
  if (length)
    *length = -1;
  const std::string& path = AS_CLASS_ICON(icon)->GetPath();
  sa_error_t result = sa_cstr_copy_buffer(buffer, size, length, path);
  return result;
}

sa_error_t sa_icon_get_path_string(sa_icon_immutable_t* icon, sa_string_t* str)
{
  const std::string& path = AS_CLASS_ICON(icon)->GetPath();
  sa_cstr_copy_string(str, path);
  return SA_ERROR_SUCCESS;
}

void sa_icon_set_path(sa_icon_t* icon, const char* path)
{
  AS_CLASS_ICON(icon)->SetPath(path);
}

int sa_icon_get_index(sa_icon_immutable_t* icon)
{
  return AS_CLASS_ICON(icon)->GetIndex();
}

void sa_icon_set_index(sa_icon_t* icon, int index)
{
  AS_CLASS_ICON(icon)->SetIndex(index);
}
