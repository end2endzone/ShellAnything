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

#include "Menu.h"
#include "Unicode.h"

namespace shellanything
{
  const uint32_t Menu::INVALID_COMMAND_ID = 0;
  const int Menu::DEFAULT_NAME_MAX_LENGTH = 250;

  Menu::Menu() :
    mNameMaxLength(DEFAULT_NAME_MAX_LENGTH),
    mSeparator(false),
    mColumnSeparator(false),
    mCommandId(INVALID_COMMAND_ID),
    mVisible(true),
    mEnabled(true)
  {
  }

  Menu::~Menu()
  {
    // delete validities
    for(size_t i=0; i<mValidities.size(); i++)
    {
      Validator * validator = mValidities[i];
      delete validator;
    }
    mValidities.clear();

    // delete visibilities
    for(size_t i=0; i<mVisibilities.size(); i++)
    {
      Validator * validator = mVisibilities[i];
      delete validator;
    }
    mVisibilities.clear();

    // delete actions
    for(size_t i=0; i<mActions.size(); i++)
    {
      Action * action = mActions[i];
      delete action;
    }
    mActions.clear();

    // delete submenus
    for(size_t i=0; i<mSubMenus.size(); i++)
    {
      Menu * sub = mSubMenus[i];
      delete sub;
    }
    mSubMenus.clear();
  }

  bool Menu::IsSeparator() const
  {
    return mSeparator;
  }

  void Menu::SetSeparator(bool separator)
  {
    mSeparator = separator;
  }

  bool Menu::IsColumnSeparator() const
  {
    return mColumnSeparator;
  }

  void Menu::SetColumnSeparator(bool column_separator)
  {
    mColumnSeparator = column_separator;
  }

  bool Menu::IsParentMenu() const
  {
    bool parent_menu = (mSubMenus.size() != 0);
    return parent_menu;
  }

  const std::string & Menu::GetName() const
  {
    return mName;
  }

  void Menu::SetName(const std::string & name)
  {
    mName = name;
  }

  const int & Menu::GetNameMaxLength() const
  {
    return mNameMaxLength;
  }

  void Menu::SetNameMaxLength(const int & name_max_length)
  {
    mNameMaxLength = name_max_length;

    // Limit out of range values
    if (mNameMaxLength < 1)
      mNameMaxLength = 1;
    if (mNameMaxLength > DEFAULT_NAME_MAX_LENGTH)
      mNameMaxLength = DEFAULT_NAME_MAX_LENGTH;
  }

  void Menu::TruncateName(std::string & str)
  {
    // Issue #55: Menu name maximum length limit and escape string
  
    // Test if we need to truncate the input string.
    if (mNameMaxLength <= 0)
      return; // Nothing to do.
    size_t num_cp = GetLengthUtf8(str.c_str());
    if (num_cp <= mNameMaxLength)
      return; // Nothing to do.

    std::string truncated_str = SubstringUtf8(str.c_str(), 0, mNameMaxLength);

    // Add a trailing "..." indicating that we reached the maximum length for a menu
    if (num_cp > DEFAULT_NAME_MAX_LENGTH)
      truncated_str += "..."; // Indicate that string is much longer than maximum allowed by ShellAnything.

    str = truncated_str;
  }

  const std::string & Menu::GetDescription() const
  {
    return mDescription;
  }

  void Menu::SetDescription(const std::string & description)
  {
    mDescription = description;
  }

  const Icon & Menu::GetIcon() const
  {
    return mIcon;
  }

  void Menu::SetIcon(const Icon & icon)
  {
    mIcon = icon;
  }

  void Menu::Update(const Context & context)
  {
    //update current menu
    bool visible = true;
    if (!mVisibilities.empty())
    {
      visible = false;
      size_t count = GetVisibilityCount();
      for(size_t i=0; i<count && visible == false; i++)
      {
        const Validator * validator = GetVisibility(i);
        if (validator)
        {
          visible |= validator->Validate(context);
        }
      }
    }
    bool enabled = true;
    if (!mValidities.empty())
    {
      enabled = false;
      size_t count = GetValidityCount();
      for(size_t i=0; i<count && enabled == false; i++)
      {
        const Validator * validator = GetValidity(i);
        if (validator)
        {
          enabled |= validator->Validate(context);
        }
      }
    }
    SetVisible(visible);
    SetEnabled(enabled);

    //update children
    bool all_invisible_children = true;

    //for each child
    Menu::MenuPtrList children = GetSubMenus();
    for(size_t i=0; i<children.size(); i++)
    {
      Menu * child = children[i];
      child->Update(context);

      //refresh the flag
      all_invisible_children = all_invisible_children && !child->IsVisible();
    }

    //Issue #4 - Parent menu with no children.
    //if all the direct children of this menu are invisible
    if (IsParentMenu() && visible && all_invisible_children)
    {
      //force this node as invisible.
      SetVisible(false);
    }
  }

  Menu * Menu::FindMenuByCommandId(const uint32_t & command_id)
  {
    if (mCommandId == command_id)
      return this;
 
    //for each child
    Menu::MenuPtrList children = GetSubMenus();
    for(size_t i=0; i<children.size(); i++)
    {
      Menu * child = children[i];
      Menu * match = child->FindMenuByCommandId(command_id);
      if (match)
        return match;
    }
 
    return NULL;
  }
 
  uint32_t Menu::AssignCommandIds(const uint32_t & first_command_id)
  {
    uint32_t next_command_id = first_command_id;

    //Issue #5 - ConfigManager::AssignCommandIds() should skip invisible menus
    if (!mVisible || first_command_id == INVALID_COMMAND_ID)
    {
      this->SetCommandId(INVALID_COMMAND_ID); //invalidate this menu's command id
    }
    else
    {
      //assign a command id to this menu
      this->SetCommandId(next_command_id);
      next_command_id++;
    }

    //for each child
    Menu::MenuPtrList children = GetSubMenus();
    for(size_t i=0; i<children.size(); i++)
    {
      Menu * child = children[i];

      if (mCommandId == INVALID_COMMAND_ID)
        child->AssignCommandIds(INVALID_COMMAND_ID); //also assign invalid ids to sub menus
      else
        next_command_id = child->AssignCommandIds(next_command_id); //assign the next command ids to sub menus
    }
 
    return next_command_id;
  }
 
  const uint32_t & Menu::GetCommandId() const
  {
    return mCommandId;
  }
 
  void Menu::SetCommandId(const uint32_t & command_id)
  {
    mCommandId = command_id;
  }
 
  bool Menu::IsVisible() const
  {
    return mVisible;
  }

  bool Menu::IsEnabled() const
  {
    return mEnabled;
  }

  void Menu::SetVisible(bool visible)
  {
    mVisible = visible;
  }

  void Menu::SetEnabled(bool enabled)
  {
    mEnabled = enabled;
  }

  size_t Menu::GetValidityCount() const
  {
    return mValidities.size();
  }

  const Validator * Menu::GetValidity(size_t index) const
  {
    const Validator * validator = NULL;
    size_t count = mValidities.size();
    if (index < count)
      validator = mValidities[index];
    return validator;
  }

  void Menu::AddValidity(Validator * validator)
  {
    if (validator)
      mValidities.push_back(validator);
  }

  size_t Menu::GetVisibilityCount() const
  {
    return mVisibilities.size();
  }

  const Validator * Menu::GetVisibility(size_t index) const
  {
    const Validator * validator = NULL;
    size_t count = mVisibilities.size();
    if (index < count)
      validator = mVisibilities[index];
    return validator;
  }

  void Menu::AddVisibility(Validator * validator)
  {
    if (validator)
      mVisibilities.push_back(validator);
  }

  Menu::MenuPtrList Menu::GetSubMenus()
  {
    return mSubMenus;
  }

  void Menu::AddMenu(Menu* menu)
  {
    mSubMenus.push_back(menu);
  }

  void Menu::AddAction(Action * action)
  {
    mActions.push_back(action);
  }

  const Action::ActionPtrList & Menu::GetActions() const
  {
    return mActions;
  }

} //namespace shellanything
