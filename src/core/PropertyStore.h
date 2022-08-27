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

#ifndef SA_PROPERTY_STORE_H
#define SA_PROPERTY_STORE_H

#include "shellanything/export.h"
#include "shellanything/config.h"
#include "StringList.h"
#include <string>
#include <vector>
#include <map>

namespace shellanything
{
  /// <summary>
  /// Defines a key-value property store
  /// </summary>
  class SHELLANYTHING_EXPORT PropertyStore
  {
  public:
    PropertyStore();
    PropertyStore(const PropertyStore& store);
    virtual ~PropertyStore();

    /// <summary>
    /// Copy operator
    /// </summary>
    const PropertyStore& operator =(const PropertyStore& store);

    //------------------------
    // Typedef
    //------------------------
    typedef std::map<std::string /*name*/, std::string /*value*/> PropertyMap;

    /// <summary>
    /// Clears all the registered properties.
    /// </summary>
    void Clear();

    /// <summary>
    /// Delete the given property.
    /// </summary>
    /// <param name="name">The name of the property to delete.</param>
    void ClearProperty(const std::string& name);

    /// <summary>
    /// Check if a property have been set.
    /// An empty property value is defined as 'set'.
    /// </summary>
    /// <param name="name">The name of the property to check.</param>
    /// <returns>Returns true if the property is set. Returns false otherwise.</returns>
    bool HasProperty(const std::string& name) const;

    /// <summary>
    /// Check if the properties are all set.
    /// An empty property value is defined as 'set'.
    /// </summary>
    /// <param name="properties">The list of properties to check.</param>
    /// <returns>Returns true if all the properties are set. Returns false otherwise.</returns>
    bool HasProperties(const StringList& properties) const;

    /// <summary>
    /// Sets the value of the given property name.
    /// </summary>
    /// <param name="name">The name of the property to set.</param>
    /// <param name="value">The new value of the property.</param>
    void SetProperty(const std::string& name, const std::string& value);

    /// <summary>
    /// Gets the value of the given property name.
    /// </summary>
    /// <param name="name">The name of the property to get.</param>
    /// <returns>Returns value of the property if the property is set. Returns an empty string otherwise.</returns>
    const std::string& GetProperty(const std::string& name) const;

    /// <summary>
    /// Counts how many properties are registered in the store.
    /// </summary>
    /// <returns>Returns how many properties are registered in the store.</returns>
    size_t GetPropertyCount() const;

    /// <summary>
    /// Returns whether the list container is empty
    /// </summary>
    /// <returns>Returns true if the store is empty. Returns false otherwise.</returns>
    bool IsEmpty() const;

    /// <summary>
    /// Get the list of properties in the store
    /// </summary>
    /// <param name="names">The output list of properties</param>
    void GetProperties(StringList& names) const;

    /// <summary>
    /// Find the list of properties which are not in the store.
    /// </summary>
    /// <param name="input_names">The list of property names to test.</param>
    /// <param name="output_names">The output list of property names which are not in the store.</param>
    void FindMissingProperties(const StringList& input_names, StringList& output_names) const;

  private:
    PropertyMap properties;
  };

} //namespace shellanything

#endif //SA_PROPERTY_STORE_H
