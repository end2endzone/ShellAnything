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

#ifndef SA_PROPERTYMANAGER_H
#define SA_PROPERTYMANAGER_H

#include "shellanything/export.h"
#include "shellanything/config.h"
#include <string>
#include <map>

namespace shellanything
{
  /// <summary>
  /// Manages the property system
  /// </summary>
  class SHELLANYTHING_EXPORT PropertyManager
  {
  public:
    static PropertyManager & GetInstance();
  private:
    PropertyManager();
    ~PropertyManager();

    // Disable copy constructor and copy operator
    PropertyManager(const PropertyManager&);
    PropertyManager& operator=(const PropertyManager&);

  public:

    /// <summary>
    /// Name of the property that defines the system true.
    /// </summary>
    static const std::string SYSTEM_TRUE_PROPERTY_NAME;

    /// <summary>
    /// Default value for the property 'SYSTEM_TRUE_PROPERTY_NAME'.
    /// </summary>
    static const std::string SYSTEM_TRUE_DEFAULT_VALUE;

    /// <summary>
    /// Name of the property that defines the system false.
    /// </summary>
    static const std::string SYSTEM_FALSE_PROPERTY_NAME;

    /// <summary>
    /// Default value for the property 'SYSTEM_FALSE_PROPERTY_NAME'.
    /// </summary>
    static const std::string SYSTEM_FALSE_DEFAULT_VALUE;

  public:

    //------------------------
    // Typedef
    //------------------------
    typedef std::map<std::string /*name*/, std::string /*value*/> PropertyMap;

    /// <summary>
    /// Clears all the registered properties.
    /// Note that environement variable properties are still registered to the manager.
    /// </summary>
    void Clear();

    /// <summary>
    /// Delete the given property.
    /// </summary>
    /// <param name="name">The name of the property to delete.</param>
    void ClearProperty(const std::string & name);

    /// <summary>
    /// Check if a property have been set.
    /// An empty property value is defined as 'set'.
    /// </summary>
    /// <param name="name">The name of the property to check.</param>
    /// <returns>Returns true if the property is set. Returns false otherwise.</returns>
    bool HasProperty(const std::string & name) const;

    /// <summary>
    /// Sets the value of the given property name.
    /// </summary>
    /// <param name="name">The name of the property to set.</param>
    /// <param name="value">The new value of the property.</param>
    void SetProperty(const std::string & name, const std::string & value);

    /// <summary>
    /// Gets the value of the given property name.
    /// </summary>
    /// <param name="name">The name of the property to get.</param>
    /// <returns>Returns value of the property if the property is set. Returns an empty string otherwise.</returns>
    const std::string & GetProperty(const std::string & name) const;

    /// <summary>
    /// Expands the given string by replacing property variable reference by the actual variable's value.
    /// The syntax of a property variable reference is the following: `${variable-name}` where `variable-name` is the name of a variable.
    /// </summary>
    /// <remarks>
    /// The string is expanded until there are no change in the given string.
    /// </remarks>
    /// <param name="value">The given value to expand.</param>
    /// <returns>Returns a copy of the given value with the property references expanded.</returns>
    std::string Expand(const std::string & value) const;

    /// <summary>
    /// Expands the given string by replacing property variable reference by the actual variable's value.
    /// The syntax of a property variable reference is the following: `${variable-name}` where `variable-name` is the name of a variable.
    /// </summary>
    /// <remarks>
    /// The string is expanded in a single pass. Some property reference may not be expanded after the first pass.
    /// </remarks>
    /// <param name="value">The given value to expand.</param>
    /// <returns>Returns a copy of the given value with the property references expanded.</returns>
    std::string ExpandOnce(const std::string & value) const;

  private:

    void RegisterEnvironmentVariables();
    void RegisterDefaultProperties();
    PropertyMap properties;
  };

} //namespace shellanything

#endif //SA_PROPERTYMANAGER_H
