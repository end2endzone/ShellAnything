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

#include "shellanything/sa_selection_context.h"
#include "sa_string_private.h"
#include "SelectionContext.h"
#include "sa_private_casting.h"

using namespace shellanything;

sa_selection_context_immutable_t sa_selection_context_to_immutable(sa_selection_context_t* context)
{
  sa_selection_context_immutable_t output;
  output.opaque = context->opaque;
  return output;
}

void sa_selection_context_register_properties(sa_selection_context_immutable_t* ctx)
{
  AS_CLASS_SELECTION_CONTEXT(ctx)->RegisterProperties();
}

void sa_selection_context_unregister_properties(sa_selection_context_immutable_t* ctx)
{
  AS_CLASS_SELECTION_CONTEXT(ctx)->UnregisterProperties();
}

void sa_selection_context_set_elements_buffer(sa_selection_context_t* ctx, const char** strarray, size_t count)
{
  shellanything::StringList elements;

  //copy to elements
  for (size_t i = 0; i < count; i++)
  {
    const char* str = strarray[i];
    elements.push_back(str);
  }

  AS_CLASS_SELECTION_CONTEXT(ctx)->SetElements(elements);
}

size_t sa_selection_context_get_element_count(sa_selection_context_immutable_t* ctx)
{
  const shellanything::StringList& elements = AS_CLASS_SELECTION_CONTEXT(ctx)->GetElements();
  size_t count = elements.size();
  return count;
}

sa_error_t sa_selection_context_get_element_buffer(sa_selection_context_immutable_t* ctx, size_t idx, int* length, char* buffer, size_t buffer_size)
{
  if (length != NULL)
    *length = -1;
  const shellanything::StringList& elements = AS_CLASS_SELECTION_CONTEXT(ctx)->GetElements();
  if (idx >= elements.size())
    return SA_ERROR_VALUE_OUT_OF_BOUNDS;
  const std::string& element = elements[idx];
  sa_error_t result = sa_cstr_copy_buffer(buffer, buffer_size, length, element);
  return result;
}

sa_error_t sa_selection_context_get_element_string(sa_selection_context_immutable_t* ctx, size_t idx, sa_string_t* str)
{
  const shellanything::StringList& elements = AS_CLASS_SELECTION_CONTEXT(ctx)->GetElements();
  if (idx >= elements.size())
    return SA_ERROR_VALUE_OUT_OF_BOUNDS;
  const std::string& element = elements[idx];
  sa_string_copy_stdstr(str, element);
  return SA_ERROR_SUCCESS;
}

const char* sa_selection_context_get_element_cstr(sa_selection_context_immutable_t* ctx, size_t idx)
{
  const shellanything::StringList& elements = AS_CLASS_SELECTION_CONTEXT(ctx)->GetElements();
  if (idx >= elements.size())
    return NULL;
  const std::string& element = elements[idx];
  const char* output = element.c_str();
  return output;
}

const char* sa_selection_context_get_element_alloc(sa_selection_context_immutable_t* ctx, size_t idx)
{
  const shellanything::StringList& elements = AS_CLASS_SELECTION_CONTEXT(ctx)->GetElements();
  if (idx >= elements.size())
    return NULL;
  const std::string& element = elements[idx];
  const char* output = element.c_str();
  return _strdup(output);
}

int sa_selection_context_get_num_files(sa_selection_context_immutable_t* ctx)
{
  return AS_CLASS_SELECTION_CONTEXT(ctx)->GetNumFiles();
}

int sa_selection_context_get_num_directories(sa_selection_context_immutable_t* ctx)
{
  return AS_CLASS_SELECTION_CONTEXT(ctx)->GetNumDirectories();
}
