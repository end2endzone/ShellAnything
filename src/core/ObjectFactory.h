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

#ifndef SA_NODEFACTORY_H
#define SA_NODEFACTORY_H

#include "shellanything/export.h"
#include "shellanything/config.h"
#include "Menu.h"
#include "Action.h"
#include "Icon.h"
#include "Configuration.h"
#include "Validator.h"
#include "DefaultSettings.h"
#include "Plugin.h"
#include "Registry.h"
#include "tinyxml2.h"

namespace shellanything
{

  /// <summary>
  /// An utility class for parsing this procet's classes from xml
  /// </summary>
  class SHELLANYTHING_EXPORT ObjectFactory
  {
  private:
    ObjectFactory();
    virtual ~ObjectFactory();

    // Disable copy constructor and copy operator
    ObjectFactory(const ObjectFactory&);
    ObjectFactory& operator=(const ObjectFactory&);

  public:
    static ObjectFactory & GetInstance();

    /// <summary>
    /// Parse a string attribute in a xml node.
    /// </summary>
    /// <param name="element">The tinyxml element to read from</param>
    /// <param name="attr_name">The name of the attribute find</param>
    /// <param name="is_optional">True if the attribute is optional. False otherwise. An error is reported if the attribute is mandatory and missing.</param>
    /// <param name="allow_empty_values">True if the attribute is allowed to be empty. False otherwise. An error is reported if the arribute is </param>
    /// <param name="attr_value">The output attribute value.</param>
    /// <param name="error">An output error description string.</param>
    /// <returns>Returns true if the attribute was parsed. Returns false otherwise.</returns>
    static bool ParseAttribute(const tinyxml2::XMLElement* element, const char* attr_name, bool is_optional, bool allow_empty_values, std::string& attr_value, std::string& error);

    /// <summary>
    /// Parse an integer attribute in a xml node.
    /// </summary>
    /// <param name="element">The tinyxml element to read from</param>
    /// <param name="attr_name">The name of the attribute find</param>
    /// <param name="is_optional">True if the attribute is optional. False otherwise. An error is reported if the attribute is mandatory and missing.</param>
    /// <param name="allow_empty_values">True if the attribute is allowed to be empty. False otherwise. An error is reported if the arribute is </param>
    /// <param name="attr_value">The output attribute value.</param>
    /// <param name="error">An output error description string.</param>
    /// <returns>Returns true if the attribute was parsed. Returns false otherwise.</returns>
    static bool ParseAttribute(const tinyxml2::XMLElement* element, const char* attr_name, bool is_optional, bool allow_empty_values, int& attr_value, std::string& error);

    /// <summary>
    /// Set the list of active plugins that must be used for parsing object.
    /// </summary>
    /// <param name="plugins">The list of plugins objects.</param>
    void SetActivePlugins(const Plugin::PluginPtrList& plugins);

    /// <summary>
    /// Clears the active plugins used for parsing.
    /// </summary>
    void ClearActivePlugins();

    /// <summary>
    /// Parses an Icon class from xml. Returns false if the parsing failed.
    /// </summary>
    /// <param name="element">The xml element that contains an Icon to parse.</param>
    /// <param name="error">The error description if the parsing failed.</param>
    /// <returns>Returns true if the object was properly parsed. Returns false otherwise.</returns>
    bool ParseIcon(const tinyxml2::XMLElement * element, Icon & icon, std::string & error);

    /// <summary>
    /// Parses a Validator class from xml. Returns NULL if the parsing failed.
    /// </summary>
    /// <param name="element">The xml element that contains a Validator to parse.</param>
    /// <param name="error">The error description if the parsing failed.</param>
    /// <returns>Returns a valid Validator pointer if the object was properly parsed. Returns NULL otherwise.</returns>
    Validator * ParseValidator(const tinyxml2::XMLElement * element, std::string & error);

    /// <summary>
    /// Parses a Action class from xml. Returns NULL if the parsing failed.
    /// </summary>
    /// <param name="element">The xml element that contains an Action to parse.</param>
    /// <param name="error">The error description if the parsing failed.</param>
    /// <returns>Returns a valid Action pointer if the object was properly parsed. Returns NULL otherwise.</returns>
    Action * ParseAction(const tinyxml2::XMLElement * element, std::string & error);

    /// <summary>
    /// Parses a Menu class from xml. Returns NULL if the parsing failed.
    /// </summary>
    /// <param name="element">The xml element that contains an Menu to parse.</param>
    /// <param name="error">The error description if the parsing failed.</param>
    /// <returns>Returns a valid Menu pointer if the object was properly parsed. Returns NULL otherwise.</returns>
    Menu * ParseMenu(const tinyxml2::XMLElement * element, std::string & error);

    /// <summary>
    /// Parses a DefaultSettings class from xml. Returns NULL if the parsing failed.
    /// </summary>
    /// <param name="element">The xml element that contains an DefaultSettings to parse.</param>
    /// <param name="error">The error description if the parsing failed.</param>
    /// <returns>Returns a valid DefaultSettings pointer if the object was properly parsed. Returns NULL otherwise.</returns>
    DefaultSettings * ParseDefaults(const tinyxml2::XMLElement * element, std::string & error);

    /// <summary>
    /// Parses a plugin from xml. Returns NULL if the parsing failed.
    /// </summary>
    /// <param name="element">The xml element that contains a PluginDescriptor to parse.</param>
    /// <param name="error">The error description if the parsing failed.</param>
    /// <returns>Returns a valid plugin pointer if the object was properly parsed. Returns NULL otherwise.</returns>
    Plugin * ParsePlugin(const tinyxml2::XMLElement * element, std::string & error);

  public:
    Registry registry;
    Plugin::PluginPtrList mPlugins;
  };

} //namespace shellanything

#endif //SA_NODEFACTORY_H
