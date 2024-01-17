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

#ifndef SA_VALIDATION_H
#define SA_VALIDATION_H

#include "shellanything/export.h"
#include "shellanything/config.h"
#include "PropertyStore.h"
#include "SelectionContext.h"
#include "Plugin.h"
#include <string>
#include <vector>

#define SA_DEFAULT_ATTRIBUTE_SEPARATOR_CHAR   ';'
#define SA_DEFAULT_ATTRIBUTE_SEPARATOR_STR    ";"
#define SA_PROPERTIES_ATTR_SEPARATOR_CHAR     SA_DEFAULT_ATTRIBUTE_SEPARATOR_CHAR
#define SA_PROPERTIES_ATTR_SEPARATOR_STR      SA_DEFAULT_ATTRIBUTE_SEPARATOR_STR 
#define SA_FILEEXTENSION_ATTR_SEPARATOR_CHAR  SA_DEFAULT_ATTRIBUTE_SEPARATOR_CHAR
#define SA_FILEEXTENSION_ATTR_SEPARATOR_STR   SA_DEFAULT_ATTRIBUTE_SEPARATOR_STR 
#define SA_INVERSE_ATTR_SEPARATOR_CHAR        SA_DEFAULT_ATTRIBUTE_SEPARATOR_CHAR
#define SA_INVERSE_ATTR_SEPARATOR_STR         SA_DEFAULT_ATTRIBUTE_SEPARATOR_STR 
#define SA_CLASS_ATTR_SEPARATOR_CHAR          SA_DEFAULT_ATTRIBUTE_SEPARATOR_CHAR
#define SA_CLASS_ATTR_SEPARATOR_STR           SA_DEFAULT_ATTRIBUTE_SEPARATOR_STR 
#define SA_EXISTS_ATTR_SEPARATOR_CHAR         SA_DEFAULT_ATTRIBUTE_SEPARATOR_CHAR
#define SA_EXISTS_ATTR_SEPARATOR_STR          SA_DEFAULT_ATTRIBUTE_SEPARATOR_STR 
#define SA_PATTERN_ATTR_SEPARATOR_CHAR        SA_DEFAULT_ATTRIBUTE_SEPARATOR_CHAR
#define SA_PATTERN_ATTR_SEPARATOR_STR         SA_DEFAULT_ATTRIBUTE_SEPARATOR_STR 
#define SA_ISTRUE_ATTR_SEPARATOR_CHAR         SA_DEFAULT_ATTRIBUTE_SEPARATOR_CHAR
#define SA_ISTRUE_ATTR_SEPARATOR_STR          SA_DEFAULT_ATTRIBUTE_SEPARATOR_STR 
#define SA_ISFALSE_ATTR_SEPARATOR_CHAR        SA_DEFAULT_ATTRIBUTE_SEPARATOR_CHAR
#define SA_ISFALSE_ATTR_SEPARATOR_STR         SA_DEFAULT_ATTRIBUTE_SEPARATOR_STR 
#define SA_KEYBOARD_ATTR_SEPARATOR_CHAR       SA_DEFAULT_ATTRIBUTE_SEPARATOR_CHAR
#define SA_KEYBOARD_ATTR_SEPARATOR_STR        SA_DEFAULT_ATTRIBUTE_SEPARATOR_STR 
#define SA_CONDITIONS_ATTR_SEPARATOR_CHAR     SA_DEFAULT_ATTRIBUTE_SEPARATOR_CHAR
#define SA_CONDITIONS_ATTR_SEPARATOR_STR      SA_DEFAULT_ATTRIBUTE_SEPARATOR_STR 
#define SA_ACTIONS_ATTR_SEPARATOR_CHAR        SA_DEFAULT_ATTRIBUTE_SEPARATOR_CHAR
#define SA_ACTIONS_ATTR_SEPARATOR_STR         SA_DEFAULT_ATTRIBUTE_SEPARATOR_STR 

namespace shellanything
{
  class Menu; // For Get/SetParentMenu()

  class SHELLANYTHING_EXPORT Validator
  {
  public:
    /// <summary>
    /// A list of Validator class pointers.
    /// </summary>
    typedef std::vector<Validator*> ValidatorPtrList;

    Validator();
    virtual ~Validator();

  private:
    // Disable copy constructor and copy operator
    Validator(const Validator&);
    Validator& operator=(const Validator&);
  public:

    static const std::string& ATTRIBUTE_MAXFILES;
    static const std::string& ATTRIBUTE_MAXDIRECTORIES;
    static const std::string& ATTRIBUTE_PROPERTIES;
    static const std::string& ATTRIBUTE_FILEEXTENSIONS;
    static const std::string& ATTRIBUTE_EXISTS;
    static const std::string& ATTRIBUTE_CLASS;
    static const std::string& ATTRIBUTE_PATTERN;
    static const std::string& ATTRIBUTE_EXPRTK;
    static const std::string& ATTRIBUTE_ISTRUE;
    static const std::string& ATTRIBUTE_ISFALSE;
    static const std::string& ATTRIBUTE_ISEMPTY;
    static const std::string& ATTRIBUTE_KEYBOARD;
    static const std::string& ATTRIBUTE_INSERVE;

    /// <summary>
    /// Get the parent menu.
    /// </summary>
    /// <returns>Returns a pointer to the parent menu. Returns NULL if the object has no parent menu.</returns>
    Menu* GetParentMenu();
    const Menu* GetParentMenu() const;

    /// <summary>
    /// Set the parent menu.
    /// </summary>
    /// <param name="menu">The parent of this menu</param>
    void SetParentMenu(Menu* menu);

    /// <summary>
    /// Getter for the 'maxfiles' parameter.
    /// </summary>
    const int& GetMaxFiles() const;

    /// <summary>
    /// Setter for the 'maxfiles' parameter.
    /// </summary>
    void SetMaxFiles(const int& max_files);

    /// <summary>
    /// Getter for the 'maxdirectories' parameter.
    /// </summary>
    const int& GetMaxDirectories() const;

    /// <summary>
    /// Setter for the 'maxdirectories' parameter.
    /// </summary>
    void SetMaxDirectories(const int& max_directories);

    /// <summary>
    /// Getter for the 'properties' parameter.
    /// </summary>
    const std::string& GetProperties() const;

    /// <summary>
    /// Setter for the 'properties' parameter.
    /// </summary>
    void SetProperties(const std::string& properties);

    /// <summary>
    /// Getter for the 'fileextensions' parameter.
    /// </summary>
    const std::string& GetFileExtensions() const;

    /// <summary>
    /// Setter for the 'fileextensions' parameter.
    /// </summary>
    void SetFileExtensions(const std::string& file_extensions);

    /// <summary>
    /// Getter for the 'fileexists' parameter.
    /// </summary>
    const std::string& GetFileExists() const;

    /// <summary>
    /// Setter for the 'fileexists' parameter.
    /// </summary>
    void SetFileExists(const std::string& file_exists);

    /// <summary>
    /// Getter for the 'class' parameter.
    /// </summary>
    const std::string& GetClass() const;

    /// <summary>
    /// Setter for the 'class' parameter.
    /// </summary>
    void SetClass(const std::string& classes);

    /// <summary>
    /// Getter for the 'pattern' parameter.
    /// </summary>
    const std::string& GetPattern() const;

    /// <summary>
    /// Setter for the 'pattern' parameter.
    /// </summary>
    void SetPattern(const std::string& pattern);

    /// <summary>
    /// Getter for the 'exprtk' parameter.
    /// </summary>
    const std::string& GetExprtk() const;

    /// <summary>
    /// Setter for the 'exprtk' parameter.
    /// </summary>
    void SetExprtk(const std::string& exprtk);

    /// <summary>
    /// Getter for the 'istrue' parameter.
    /// </summary>
    const std::string& GetIsTrue() const;

    /// <summary>
    /// Setter for the 'istrue' parameter.
    /// </summary>
    void SetIsTrue(const std::string& istrue);

    /// <summary>
    /// Getter for the 'isfalse' parameter.
    /// </summary>
    const std::string& GetIsFalse() const;

    /// <summary>
    /// Setter for the 'isfalse' parameter.
    /// </summary>
    void SetIsFalse(const std::string& isfalse);

    /// <summary>
    /// Getter for the 'isempty' parameter.
    /// </summary>
    const std::string& GetIsEmpty() const;

    /// <summary>
    /// Setter for the 'isempty' parameter.
    /// </summary>
    void SetIsEmpty(const std::string& isempty);

    /// <summary>
    /// Getter for the 'keyboard' parameter.
    /// </summary>
    const std::string& GetKeyboard() const;

    /// <summary>
    /// Setter for the 'keyboard' parameter.
    /// </summary>
    void SetKeyboard(const std::string& keyboard);

    /// <summary>
    /// Get the custom attributes (key and values) defined by plugins.
    /// </summary>
    const PropertyStore& GetCustomAttributes() const;

    /// <summary>
    /// Set the custom attributes (key and values).
    /// </summary>
    void SetCustomAttributes(const PropertyStore& attributes);

    /// <summary>
    /// Getter for the 'inserve' parameter.
    /// </summary>
    const std::string& GetInserve() const;

    /// <summary>
    /// Setter for the 'inserve' parameter.
    /// </summary>
    void SetInserve(const std::string& inserve);

    /// <summary>
    /// Returns true if the given attribute name is in the list of inversed attributes.
    /// The accepted attribute names matches the list of attributes of <validity> or <visibility> elements in a Configuration File.
    /// See the 'User Manual' for details.
    /// </summary>
    /// <param name="name">The name of the attribute. The name is case sensitive.</param>
    /// <returns>Returns true if the given attribute name is in the list of inversed attributes. Returns false otherwise.</returns>
    bool IsInversed(const char* name) const;

    /// <summary>
    /// Validate the object against a set of constraints.
    /// Note: this function is usually used to enable or disable a menu.
    /// </summary>
    /// <param name="context">The selection context used for validating.</param>
    /// <returns>Returns true if the given context is valid against the set of constraints. Returns false otherwise.</returns>
    bool Validate(const SelectionContext& context) const;

    /// <summary>
    /// Validates if a given string can be evaluated as logical true.
    /// </summary>
    /// <remarks>
    /// The following values shall be evaluated to true:
    ///    true (case insensitive)
    ///    yes (case insensitive)
    ///    ok
    ///    on
    ///    1
    ///    ${system.true}
    /// </remarks>
    /// <param name="value">The value to test as true</param>
    /// <returns>Returns true if the given value can be evaluated as logical true. Returns false otherwise.</returns>
    static bool IsTrue(const std::string& value);

    /// <summary>
    /// Validates if a given string can be evaluated as logical true.
    /// </summary>
    /// <remarks>
    /// The following values shall be evaluated to false:
    ///    false (case insensitive)
    ///    no (case insensitive)
    ///    fail
    ///    off
    ///    0
    ///    ${system.false}
    /// </remarks>
    /// <param name="value">The value to test as true</param>
    /// <returns>Returns true if the given value can be evaluated as logical true. Returns false otherwise.</returns>
    static bool IsFalse(const std::string& value);

  private:
    bool ValidateProperties(const SelectionContext& context, const std::string& properties, bool inversed) const;
    bool ValidateFileExtensions(const SelectionContext& context, const std::string& file_extensions, bool inversed) const;
    bool ValidateExists(const SelectionContext& context, const std::string& file_exists, bool inversed) const;
    bool ValidateClass(const SelectionContext& context, const std::string& class_, bool inversed) const;
    bool ValidateSingleFileMultipleClasses(const std::string& path, const std::string& class_, bool inversed) const;
    bool ValidateSingleFileSingleClass(const std::string& path, const std::string& class_, bool inversed) const;
    bool ValidatePattern(const SelectionContext& context, const std::string& pattern, bool inversed) const;
    bool ValidateExprtk(const SelectionContext& context, const std::string& exprtk, bool inversed) const;
    bool ValidateIsTrue(const SelectionContext& context, const std::string& istrue, bool inversed) const;
    bool ValidateIsFalse(const SelectionContext& context, const std::string& isfalse, bool inversed) const;
    bool ValidateIsEmpty(const SelectionContext& context, const std::string& isempty, bool inversed) const;
    bool ValidatePlugin(const SelectionContext& context, Plugin* plugin) const;
    bool ValidateKeyboard(const SelectionContext& context, const std::string& keyboard, bool inversed) const;

  private:
    int mMaxFiles;
    int mMaxDirectories;
    PropertyStore mAttributes;
    PropertyStore mCustomAttributes;
    Plugin::PluginPtrList mPlugins;
    Menu* mParentMenu;
  };

} //namespace shellanything

#endif //SA_VALIDATION_H
