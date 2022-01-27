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

#ifndef SA_MENU_H
#define SA_MENU_H

#include "shellanything/export.h"
#include "shellanything/config.h"
#include "Icon.h"
#include "Validator.h"
#include "Action.h"
#include <string>
#include <vector>
#include <stdint.h>

namespace shellanything
{

  /// <summary>
  /// The Menu class defines a displayed menu option.
  /// </summary>
  class SHELLANYTHING_EXPORT Menu
  {
  public:
    /// <summary>
    /// A list of Menu pointers.
    /// </summary>
    typedef std::vector<Menu*> MenuPtrList;

    /// <summary>
    /// An invalid command id.
    /// </summary>
    static const uint32_t INVALID_COMMAND_ID;

    /// <summary>
    /// The default maximum length for the 'name' parameter.
    /// Default initialization value for the 'GetNameMaxLength()' method.
    /// </summary>
    static const int DEFAULT_NAME_MAX_LENGTH;

    Menu();
    virtual ~Menu();

  private:
    // Disable copy constructor and copy operator
    Menu(const Menu&);
    Menu& operator=(const Menu&);
  public:

    /// <summary>
    /// Returns true of the menu is a separator.
    /// </summary>
    /// <returns>Returns true of the menu is a separator. Returns false otherwise.</returns>
    bool IsSeparator() const;

    /// <summary>
    /// Sets the menu separator property.
    /// </summary>
    /// <param name="separator">True to define the menu as a separator. False otherwise.</param>
    void SetSeparator(bool separator);

    /// <summary>
    /// Returns true of the menu is a column separator.
    /// </summary>
    /// <returns>Returns true of the menu is a column separator. Returns false otherwise.</returns>
    bool IsColumnSeparator() const;

    /// <summary>
    /// Sets the menu column separator property.
    /// </summary>
    /// <param name="separator">True to define the menu as a column separator. False otherwise.</param>
    void SetColumnSeparator(bool column_separator);

    /// <summary>
    /// Returns true if the menu is a parent menu (if this menu have submenus).
    /// </summary>
    /// <returns>Returns true if the menu is a parent menu (if this menu have submenus). Returns false otherwise.</returns>
    bool IsParentMenu() const;

    /// <summary>
    /// Getter for the 'name' parameter.
    /// </summary>
    const std::string & GetName() const;

    /// <summary>
    /// Setter for the 'name' parameter.
    /// </summary>
    void SetName(const std::string & name);

    /// <summary>
    /// Getter for the 'max_length' parameter.
    /// </summary>
    const int & GetNameMaxLength() const;

    /// <summary>
    /// Setter for the 'max_length' parameter.
    /// </summary>
    void SetNameMaxLength(const int & name_max_length);

    /// <summary>
    /// Truncate a string to the maximum length allowed by this menu.
    /// Note, the given string must be already expanded.
    /// If the length of the given string is longer than DEFAULT_NAME_MAX_LENGTH, a trailing "..." will
    /// be added at the end of a string to indicate that the maximum supported length is reached.
    /// </summary>
    /// <remarks>
    /// There is no need to validate the maximum length of a given string if it is not already expanded
    /// because property expansion can shorten or lengthen the given string.
    /// </remarks>
    /// <param name="str">The expanded string value which length must be validated.</param>
    void TruncateName(std::string & str);

    /// <summary>
    /// Getter for the 'description' parameter.
    /// </summary>
    const std::string & GetDescription() const;

    /// <summary>
    /// Setter for the 'description' parameter.
    /// </summary>
    void SetDescription(const std::string & description);

    /// <summary>
    /// Get this menu icon instance.
    /// </summary>
    const Icon & GetIcon() const;

    /// <summary>
    /// Set this menu icon instance.
    /// </summary>
    void SetIcon(const Icon & icon);

    /// <summary>
    /// Updates the menu and submenus 'visible' and 'enabled' properties based on the given Context.
    /// </summary>
    /// <param name="context">The context used for updating the menu.</param>
    void Update(const Context & context);

    /// <summary>
    /// Searches this menu and submenus for a menu whose command id is command_id.
    /// </summary>
    /// <param name="command_id">The search command id value.</param>
    /// <returns>Returns a Menu pointer if a match is found. Returns NULL otherwise.</returns>
    Menu * FindMenuByCommandId(const uint32_t & command_id);

    /// <summary>
    /// Assign unique command id to this menus and submenus.
    /// </summary>
    /// <param name="first_command_id">The first command id available.</param>
    /// <returns>Returns the next available command id. Returns first_command_id if no command id was assigned.</returns>
    uint32_t AssignCommandIds(const uint32_t & first_command_id);

    /// <summary>
    /// Getter for the 'command-id' parameter.
    /// </summary>
    const uint32_t & GetCommandId() const;

    /// <summary>
    /// Setter for the 'command-id' parameter.
    /// </summary>
    void SetCommandId(const uint32_t & command_id);

    /// <summary>
    /// Getter for the 'visible' parameter.
    /// </summary>
    bool IsVisible() const;

    /// <summary>
    /// Setter for the 'visible' parameter.
    /// </summary>
    void SetVisible(bool visible);

    /// <summary>
    /// Getter for the 'enabled' parameter.
    /// </summary>
    bool IsEnabled() const;

    /// <summary>
    /// Setter for the 'enabled' parameter.
    /// </summary>
    void SetEnabled(bool enabled);

    /// <summary>
    /// Get the number of Validator instances used for validity.
    /// </summary>
    size_t GetValidityCount() const;

    /// <summary>
    /// Get a Validator instance used for validity.
    /// </summary>
    /// <param name="index">The index of the requested instance.</param>
    /// <returns>Returns a valid Validator instance. Returns NULL if index is invalid.</returns>
    const Validator * GetValidity(size_t index) const;

    /// <summary>
    /// Add a new validity instance used for validity to menu. The menu instance takes ownership of the validator.
    /// </summary>
    /// <param name="validator">A valid validity validator instance.</param>
    void AddValidity(Validator * validator);

    /// <summary>
    /// Get the number of Validator instances used for visibility.
    /// </summary>
    size_t GetVisibilityCount() const;

    /// <summary>
    /// Get a Validator instance used for visibility.
    /// </summary>
    /// <param name="index">The index of the requested instance.</param>
    /// <returns>Returns a valid Validator instance. Returns NULL if index is invalid.</returns>
    const Validator * GetVisibility(size_t index) const;

    /// <summary>
    /// Add a new Validator instance used for visibility to menu. The menu instance takes ownership of the validator.
    /// </summary>
    /// <param name="validator">A valid visibility validator instance.</param>
    void AddVisibility(Validator * validator);

    /// <summary>
    /// Add a new Action to the menu. The menu instance takes ownership of the action.
    /// </summary>
    /// <param name="action">The given action to add to the menu</param>
    void AddAction(Action * action);
    
    /// <summary>
    /// Get the list of action of the menu.
    /// </summary>
    const Action::ActionPtrList & GetActions() const;

    /// <summary>
    /// Add a new sub menu to the menu. The menu instance takes ownership of the sub menu.
    /// </summary>
    /// <param name="menu">The given menu to add as a sub menu</param>
    void AddMenu(Menu* menu);

    /// <summary>
    /// Get the list of submenu of the menu.
    /// </summary>
    MenuPtrList GetSubMenus();

  private:
    Icon mIcon;
    Validator::ValidatorPtrList mValidities;
    Validator::ValidatorPtrList mVisibilities;
    bool mVisible;
    bool mEnabled;
    bool mSeparator;
    bool mColumnSeparator;
    uint32_t mCommandId;
    std::string mName;
    int mNameMaxLength;
    std::string mDescription;
    Action::ActionPtrList mActions;
    MenuPtrList mSubMenus;
  };

} //namespace shellanything

#endif //SA_MENU_H
