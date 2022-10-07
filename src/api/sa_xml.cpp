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

#include "shellanything/sa_xml.h"
#include "shellanything/sa_logging.h"
#include "shellanything/sa_properties.h"
#include "sa_string_private.h"
#include "sa_private_casting.h"

#include "PropertyStore.h"

#include "tinyxml2.h"
#include <string>

using namespace shellanything;
using namespace tinyxml2;

#define SA_API_LOG_IDDENTIFIER "XML API"

sa_error_t xml_error_to_sa_error(XMLError result)
{
  switch (result)
  {
  case XML_SUCCESS:
    return SA_ERROR_SUCCESS;
  case XML_NO_ATTRIBUTE:
  case XML_ERROR_FILE_NOT_FOUND:
  case XML_NO_TEXT_NODE:
    return SA_ERROR_NOT_FOUND;
  case XML_ERROR_MISMATCHED_ELEMENT:
    return SA_ERROR_VALUE_OUT_OF_BOUNDS;
  case XML_ERROR_FILE_COULD_NOT_BE_OPENED:
  case XML_ERROR_FILE_READ_ERROR:
    return SA_ERROR_CANT_READ;
  case XML_ERROR_PARSING_ELEMENT:
  case XML_ERROR_PARSING_ATTRIBUTE:
  case XML_ERROR_PARSING_TEXT:
  case XML_ERROR_PARSING_CDATA:
  case XML_ERROR_PARSING_COMMENT:
  case XML_ERROR_PARSING_DECLARATION:
  case XML_ERROR_PARSING_UNKNOWN:
  case XML_ERROR_PARSING:
    return SA_ERROR_INVALID_ARGUMENTS;
  case XML_ERROR_EMPTY_DOCUMENT:
    return SA_ERROR_EMPTY;
  case XML_ELEMENT_DEPTH_EXCEEDED:
    return SA_ERROR_VALUE_OUT_OF_BOUNDS;
  case XML_WRONG_ATTRIBUTE_TYPE:
  case XML_CAN_NOT_CONVERT_TEXT:
  default:
    return SA_ERROR_UNKNOWN;
  };
}

sa_error_t sa_xml_parse_attr_list_internal(const char* xml, XML_ATTR* attrs, size_t num_attrs, PropertyStore& ps)
{
  if (xml == NULL || attrs == NULL || num_attrs == 0)
    return SA_ERROR_INVALID_ARGUMENTS;

  //Parse the xml file
  //http://leethomason.github.io/tinyxml2/
  XMLDocument doc;
  XMLError xml_result = doc.Parse(xml);
  if (xml_result != XML_SUCCESS)
  {
    const char* xml_error = doc.ErrorStr();
    if (xml_error == NULL)
      xml_error = "Unknown error reported by XML library.";
    sa_logging_print_format(SA_LOG_LEVEL_ERROR, SA_API_LOG_IDDENTIFIER, "Failed to parse xml content: xml_error=%d, %s.", (int)xml_result, xml_error);

    sa_error_t sa_result = xml_error_to_sa_error(xml_result);
    return sa_result;
  }

  //Search for attributes
  for (size_t i = 0; i < num_attrs; i++)
  {
    XML_ATTR& attr = attrs[i];

    //find this attribute in each xml nodes
    bool found = false;
    const XMLElement* xml_element = XMLHandle(&doc).FirstChild().ToElement();
    while (xml_element && !found)
    {
      const char* xml_name = xml_element->Name();

      const XMLAttribute* xml_attr = xml_element->FindAttribute(attr.name);
      if (xml_attr != NULL)
      {
        const char* attr_value = xml_attr->Value();
        ps.SetProperty(attr.name, attr_value);
        found = true;
      }

      //next node
      xml_element = xml_element->NextSiblingElement();
    }

    //Check mandatory attributes
    if (attr.mandatory == SA_XML_ATTR_MANDATORY && !found)
    {
      sa_logging_print_format(SA_LOG_LEVEL_INFO, SA_API_LOG_IDDENTIFIER, "Unable to find mandatory attribute '%s'.", attr.name);
      return SA_ERROR_NOT_FOUND;
    }
  }

  return SA_ERROR_SUCCESS;
}

sa_error_t sa_xml_parse_attr_buffer(const char* xml, XML_ATTR* attr, int* length, char* buffer, size_t size)
{
  if (length)
    *length = -1;
  PropertyStore ps;
  sa_error_t result = sa_xml_parse_attr_list_internal(xml, attr, 1, ps);
  if (result != SA_ERROR_SUCCESS)
    return result;
  const std::string& value = ps.GetProperty(attr->name);
  result = sa_cstr_copy_buffer(buffer, size, length, value);
  return result;
}

sa_error_t sa_xml_parse_attr_string(const char* xml, XML_ATTR* attr, sa_string_t* str)
{
  PropertyStore ps;
  sa_error_t result = sa_xml_parse_attr_list_internal(xml, attr, 1, ps);
  if (result != SA_ERROR_SUCCESS)
    return result;
  const std::string& value = ps.GetProperty(attr->name);
  sa_string_copy_stdstr(str, value);
  return SA_ERROR_SUCCESS;
}

const char* sa_xml_parse_attr_alloc(const char* xml, XML_ATTR* attr)
{
  PropertyStore ps;
  sa_error_t result = sa_xml_parse_attr_list_internal(xml, attr, 1, ps);
  if (result != SA_ERROR_SUCCESS)
    return NULL;
  const std::string& value = ps.GetProperty(attr->name);
  const char* output = value.c_str();
  return _strdup(output);
}

sa_error_t sa_xml_parse_attr_store(const char* xml, XML_ATTR* attr, sa_property_store_t* store)
{
  if (store == NULL)
    return SA_ERROR_INVALID_ARGUMENTS;
  PropertyStore* psptr = AS_CLASS_PROPERTY_STORE(store);
  sa_error_t result = sa_xml_parse_attr_list_internal(xml, attr, 1, *psptr);
  return result;
}

sa_error_t sa_xml_parse_attr_list_store(const char* xml, XML_ATTR* attrs, size_t count, sa_property_store_t* store)
{
  if (store == NULL)
    return SA_ERROR_INVALID_ARGUMENTS;
  PropertyStore* psptr = AS_CLASS_PROPERTY_STORE(store);
  sa_error_t result = sa_xml_parse_attr_list_internal(xml, attrs, count, *psptr);
  return result;
}

void sa_xml_attr_list_init(XML_ATTR* attrs, size_t count)
{
  for (size_t i = 0; i < count; i++)
  {
    XML_ATTR& attr = attrs[i];

    attr.tmp_value = NULL;
    attr.tmp_expanded = NULL;
  }
}

void sa_xml_attr_list_cleanup(XML_ATTR* attrs, size_t count)
{
  for (size_t i = 0; i < count; i++)
  {
    XML_ATTR& attr = attrs[i];

    // Free temporary original value
    if (attr.tmp_value != NULL)
      free((void*)attr.tmp_value);
    attr.tmp_value = NULL;

    // Free temporary expanded value
    if (attr.tmp_expanded != NULL)
      free((void*)attr.tmp_expanded);
    attr.tmp_expanded = NULL;
  }
}

void sa_xml_attr_list_update(XML_ATTR* attrs, size_t count, sa_property_store_immutable_t* store)
{
  sa_xml_attr_list_cleanup(attrs, count);

  const PropertyStore* psptr = AS_CLASS_PROPERTY_STORE(store);

  for (size_t i = 0; i < count; i++)
  {
    XML_ATTR& attr = attrs[i];

    if (psptr->HasProperty(attr.name))
    {
      // Get the original value of this attribute from the property store
      const std::string& property_value = psptr->GetProperty(attr.name);
      attr.tmp_value = _strdup(property_value.c_str());

      // And expand the value
      attr.tmp_expanded = sa_properties_expand_alloc(attr.tmp_value);
    }
  }
}

sa_error_t sa_xml_attr_group_is_mutually_exclusive(XML_ATTR* attrs, size_t count, int group, sa_property_store_immutable_t* store)
{
  // Build attribute names in group
  std::string all_names_in_group;
  for (size_t i = 0; i < count; i++)
  {
    XML_ATTR& attr = attrs[i];
    if (attr.group == group)
    {
      if (!all_names_in_group.empty())
        all_names_in_group.append(",");
      all_names_in_group.append(attr.name);
    }
  }

  // Check if group is found
  if (all_names_in_group.empty())
  {
    sa_logging_print_format(SA_LOG_LEVEL_INFO, SA_API_LOG_IDDENTIFIER, "Fail to identify an attribute in group %d.", group);
    return SA_ERROR_NOT_FOUND;
  }

  // Count how many attributes are specified in group
  size_t num_specified = 0;
  std::string specified_names_in_group;
  for (size_t i = 0; i < count; i++)
  {
    XML_ATTR& attr = attrs[i];
    if (attr.group == group)
    {
      // Check if this attribute was specified (is available in the store)
      if (sa_property_store_has_property(store, attr.name))
      {
        num_specified++;
        if (!specified_names_in_group.empty())
          specified_names_in_group.append(",");
        specified_names_in_group.append(attr.name);
      }
    }
  }

  // Assert
  if (num_specified == 0)
  {
    sa_logging_print_format(SA_LOG_LEVEL_INFO, SA_API_LOG_IDDENTIFIER, "One attributes of group %d must be specified: %s.", group, all_names_in_group.c_str());
    return SA_ERROR_VALUE_OUT_OF_BOUNDS;
  }
  else if (num_specified > 1)
  {
    sa_logging_print_format(SA_LOG_LEVEL_INFO, SA_API_LOG_IDDENTIFIER, "One attributes of group %d must be specified but the following attributes was specified: %s.", group, specified_names_in_group.c_str());
    return SA_ERROR_VALUE_OUT_OF_BOUNDS;
  }

  return SA_ERROR_SUCCESS;
}
