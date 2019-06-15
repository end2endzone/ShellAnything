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

#include "shellanything/Node.h"
#include "shellanything/Icon.h"
#include "shellanything/Validator.h"
#include "shellanything/Action.h"
#include <string>
#include <vector>
#include <stdint.h>

namespace shellanything
{

  /// <summary>
  /// The Menu class defines a displayed menu option.
  /// </summary>
  class Menu : public Node
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
    bool isSeparator() const;

    /// <summary>
    /// Sets the menu separator property.
    /// </summary>
    /// <param name="iSeparator">True to define the menu as a separator. False otherwise.</param>
    void setSeparator(bool iSeparator);

    /// <summary>
    /// Returns true if the menu is a parent menu (if this menu have submenus).
    /// </summary>
    /// <returns>Returns true if the menu is a parent menu (if this menu have submenus). Returns false otherwise.</returns>
    bool isParentMenu() const;

    /// <summary>
    /// Getter for the 'name' parameter.
    /// </summary>
    const std::string & getName() const;

    /// <summary>
    /// Setter for the 'name' parameter.
    /// </summary>
    void setName(const std::string & iName);

    /// <summary>
    /// Getter for the 'description' parameter.
    /// </summary>
    const std::string & getDescription() const;

    /// <summary>
    /// Setter for the 'description' parameter.
    /// </summary>
    void setDescription(const std::string & iDescription);

    /// <summary>
    /// Get this menu icon instance.
    /// </summary>
    const Icon & getIcon() const;

    /// <summary>
    /// Set this menu icon instance.
    /// </summary>
    /// <param name="iIcon">An icon definition</param>
    void setIcon(const Icon & iIcon);

    /// <summary>
    /// Updates the menu and submenus 'visible' and 'enabled' properties based on the given Context.
    /// </summary>
    /// <param name="c">The context used for updating the menu.</param>
    void update(const Context & c);

    /// <summary>
    /// Searches this menu and submenus for a menu whose command id is iCommandId.
    /// </summary>
    /// <param name="iCommandId">The search command id value.</param>
    /// <returns>Returns a Menu pointer if a match is found. Returns NULL otherwise.</returns>
    Menu * findMenuByCommandId(const uint32_t & iCommandId);

    /// <summary>
    /// Assign unique command id to this menus and submenus.
    /// </summary>
    /// <param name="iFirstCommandId">The first command id available.</param>
    /// <returns>Returns the next available command id. Returns iFirstCommandId if no command id was assigned.</returns>
    uint32_t assignCommandIds(const uint32_t & iFirstCommandId);

    /// <summary>
    /// Getter for the 'command-id' parameter.
    /// </summary>
    const uint32_t & getCommandId() const;

    /// <summary>
    /// Setter for the 'command-id' parameter.
    /// </summary>
    void setCommandId(const uint32_t & iCommandId);

    /// <summary>
    /// Getter for the 'visible' parameter.
    /// </summary>
    bool isVisible() const;

    /// <summary>
    /// Setter for the 'visible' parameter.
    /// </summary>
    void setVisible(bool visible);

    /// <summary>
    /// Getter for the 'enabled' parameter.
    /// </summary>
    bool isEnabled() const;

    /// <summary>
    /// Setter for the 'enabled' parameter.
    /// </summary>
    void setEnabled(bool enabled);

    /// <summary>
    /// Get the Validator for the 'validity' parameter. 
    /// </summary>
    const Validator & getValidity();

    /// <summary>
    /// Set the Validator for the 'validity' parameter.
    /// </summary>
    /// <param name="iValidity">Set the new Validator for the 'validity' parameter.</param>
    void setValidity(const Validator & iValidity);

    /// <summary>
    /// Get the Validator for the 'visibility' parameter.
    /// </summary>
    const Validator & getVisibility();

    /// <summary>
    /// Set the Validator for the 'visibility' parameter.
    /// </summary>
    /// <param name="iVisibility">Set the new Validator for the 'visibility' parameter.</param>
    void setVisibility(const Validator & iVisibility);

    /// <summary>
    /// Add a new Action to the menu. The menu instance takes ownership of the action.
    /// </summary>
    /// <param name="action">The given action to add to the menu</param>
    void addAction(Action * action);
    
    /// <summary>
    /// Get the list of action of the menu.
    /// </summary>
    const Action::ActionPtrList & getActions() const;

    /// <summary>
    /// Get the list of submenu of the menu.
    /// </summary>
    MenuPtrList getSubMenus();

  private:
    Icon mIcon;
    Validator mValidity;
    Validator mVisibility;
    bool mVisible;
    bool mEnabled;
    bool mSeparator;
    uint32_t mCommandId;
    std::string mName;
    std::string mDescription;
    Action::ActionPtrList mActions;
  };

} //namespace shellanything

#endif //SA_MENU_H
