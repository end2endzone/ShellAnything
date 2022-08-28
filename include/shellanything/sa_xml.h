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

#ifndef SA_API_XML_H
#define SA_API_XML_H

#include "shellanything/sa_types.h"
#include "shellanything/sa_error.h"
#include "shellanything/sa_string.h"
#include "shellanything/sa_property_store.h"

#ifdef __cplusplus
extern "C" {
#if 0
}  // do not indent code inside extern C
#endif
#endif

static const sa_boolean SA_XML_ATTR_OPTINAL = 0;
static const sa_boolean SA_XML_ATTR_MANDATORY = 1;
static const int SA_XML_ATTR_GROUP_ANY = -1;

struct XML_ATTR
{
  const char* name;         // must be statiscally allocated
  sa_boolean mandatory;
  int group;
  const char* tmp_value;    // a temporary variable for temporary storing the original value of the attribute. Must be free with sa_memory_free() or sa_xml_attr_list_cleanup().
  const char* tmp_expanded; // a temporary variable for temporary storing the expanded value of the atribute   Must be free with sa_memory_free() or sa_xml_attr_list_cleanup().
};

/// <summary>
/// Parses an xml attribute from an xml string.
/// </summary>
/// <param name="xml">The xml content to parse.</param>
/// <param name="attr">The attribute definition.</param>
/// <param name="length">The length of the output value in bytes.</param>
/// <param name="buffer">The output buffer for the value.</param>
/// <param name="size">The size of the output buffer in bytes.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_xml_parse_attr_buffer(const char* xml, XML_ATTR* attr, int* length, char* buffer, size_t size);

/// <summary>
/// Parses an xml attribute from an xml string.
/// </summary>
/// <param name="xml">The xml content to parse.</param>
/// <param name="attr">The attribute definition.</param>
/// <param name="str">The output string allocated by the function. The string must be freed with sa_string_free() to prevent leaks.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_xml_parse_attr_string(const char* xml, XML_ATTR* attr, sa_string_t* str);

/// <summary>
/// Parses an xml attribute from an xml string.
/// </summary>
/// <param name="xml">The xml content to parse.</param>
/// <param name="attr">The attribute definition.</param>
/// <returns>Returns a valid string value. Must be free with sa_memory_free() to prevent memory leaks. Returns NULL if no value is defined.</returns>
const char* sa_xml_parse_attr_alloc(const char* xml, XML_ATTR* attr);

/// <summary>
/// Parses an xml attribute from an xml string. The attribute's value is saved into a property store.
/// </summary>
/// <param name="xml">The xml content to parse.</param>
/// <param name="attr">The attribute definition.</param>
/// <param name="store">The property store to save the value of the arribute.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_xml_parse_attr_store(const char* xml, XML_ATTR* attr, sa_property_store_t* store);

/// <summary>
/// Parses a list of XML_ATTR from an xml string. The attribute's values are saved into a property store.
/// </summary>
/// <param name="xml">The xml content to parse.</param>
/// <param name="attrs">The an array of XML_ATTR to parse.</param>
/// <param name="count">The number of elements in the attrs array.</param>
/// <param name="store">The property store to save the value of the arribute.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_xml_parse_attr_list_store(const char* xml, XML_ATTR* attrs, size_t count, sa_property_store_t* store);

/// <summary>
/// Initialize temporary fields of an XML_ATTR array.
/// </summary>
/// <param name="attrs">The an array of XML_ATTR to parse.</param>
/// <param name="count">The number of elements in the attrs array.</param>
void sa_xml_attr_list_init(XML_ATTR* attrs, size_t count);

/// <summary>
/// Clear temporary fields of an XML_ATTR array.
/// </summary>
/// <param name="attrs">The an array of XML_ATTR to parse.</param>
/// <param name="count">The number of elements in the attrs array.</param>
void sa_xml_attr_list_cleanup(XML_ATTR* attrs, size_t count);

/// <summary>
/// Update and assign temporary values to an XML_ATTR array from a property store.
/// </summary>
/// <param name="attrs">The an array of XML_ATTR to parse.</param>
/// <param name="count">The number of elements in the attrs array.</param>
/// <param name="store">The property store to save the value of the arribute.</param>
void sa_xml_attr_list_update(XML_ATTR* attrs, size_t count, sa_property_store_immutable_t* store);

/// <summary>
/// Check that a single value is specified for a given group.
/// </summary>
/// <param name="attrs">The an array of XML_ATTR to parse.</param>
/// <param name="count">The number of elements in the attrs array.</param>
/// <param name="group">The group identifier.</param>
/// <param name="store">The property store to get attribute values.</param>
/// <returns>Returns 0 on success. Returns non-zero otherwise.</returns>
sa_error_t sa_xml_attr_group_is_mutually_exclusive(XML_ATTR* attrs, size_t count, int group, sa_property_store_immutable_t* store);

#ifdef __cplusplus
#if 0
{  // do not indent code inside extern C
#endif
}  // extern "C"
#endif

#endif //SA_API_XML_H
