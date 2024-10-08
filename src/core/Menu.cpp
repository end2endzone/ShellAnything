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
#include "PropertyManager.h"
#include "LoggerHelper.h"
#include "SaUtils.h"

#include "rapidassist/strings.h"
#include "rapidassist/environment.h"

namespace shellanything
{
  const uint32_t Menu::INVALID_COMMAND_ID = 0;
  const int Menu::DEFAULT_NAME_MAX_LENGTH = 250;

  Menu::Menu() :
    mParentMenu(NULL),
    mParentConfigFile(NULL),
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
    for (size_t i = 0; i < mValidities.size(); i++)
    {
      Validator* validator = mValidities[i];
      delete validator;
    }
    mValidities.clear();

    // delete visibilities
    for (size_t i = 0; i < mVisibilities.size(); i++)
    {
      Validator* validator = mVisibilities[i];
      delete validator;
    }
    mVisibilities.clear();

    // delete actions
    for (size_t i = 0; i < mActions.size(); i++)
    {
      IAction* action = mActions[i];
      delete action;
    }
    mActions.clear();

    // delete submenus
    for (size_t i = 0; i < mSubMenus.size(); i++)
    {
      Menu* sub = mSubMenus[i];
      delete sub;
    }
    mSubMenus.clear();
  }

  Menu* Menu::GetParentMenu()
  {
    return mParentMenu;
  }

  const Menu* Menu::GetParentMenu() const
  {
    return mParentMenu;
  }

  void Menu::SetParentMenu(Menu* menu)
  {
    mParentMenu = menu;
  }

  ConfigFile* Menu::GetParentConfigFile()
  {
    return mParentConfigFile;
  }

  const ConfigFile* Menu::GetParentConfigFile() const
  {
    return mParentConfigFile;
  }

  void Menu::SetParentConfigFile(ConfigFile* config_file)
  {
    mParentConfigFile = config_file;
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

  const std::string& Menu::GetName() const
  {
    return mName;
  }

  void Menu::SetName(const std::string& name)
  {
    mName = name;
  }

  const int& Menu::GetNameMaxLength() const
  {
    return mNameMaxLength;
  }

  void Menu::SetNameMaxLength(const int& name_max_length)
  {
    mNameMaxLength = name_max_length;

    // Limit out of range values
    if (mNameMaxLength < 1)
      mNameMaxLength = 1;
    if (mNameMaxLength > DEFAULT_NAME_MAX_LENGTH)
      mNameMaxLength = DEFAULT_NAME_MAX_LENGTH;
  }

  void Menu::TruncateName(std::string& str)
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

  const std::string& Menu::GetDescription() const
  {
    return mDescription;
  }

  void Menu::SetDescription(const std::string& description)
  {
    mDescription = description;
  }

  const Icon& Menu::GetIcon() const
  {
    return mIcon;
  }

  void Menu::SetIcon(const Icon& icon)
  {
    mIcon = icon;
  }

  void Menu::Update(const SelectionContext& context)
  {
    SA_DECLARE_SCOPE_LOGGER_ARGS(sli);
    sli.verbose = true;
    sli.instance = this;
    ScopeLogger logger(&sli);

    //update current menu
    bool visible = true;
    if (!mVisibilities.empty())
    {
      SA_VERBOSE_LOG(DEBUG) << "Validating menu '" << mName << "' against visibility validators...";

      visible = false;
      size_t count = GetVisibilityCount();
      for (size_t i = 0; i < count && visible == false; i++)
      {
        const Validator* validator = GetVisibility(i);
        if (validator)
        {
          visible |= validator->Validate(context);
        }
      }
    }
    bool enabled = true;
    if (!mValidities.empty())
    {
      SA_VERBOSE_LOG(DEBUG) << "Validating menu '" << mName << "' against validity validators...";

      enabled = false;
      size_t count = GetValidityCount();
      for (size_t i = 0; i < count && enabled == false; i++)
      {
        const Validator* validator = GetValidity(i);
        if (validator)
        {
          enabled |= validator->Validate(context);
        }
      }
    }
    SetVisible(visible);
    SetEnabled(enabled);
    if (!visible)
    {
      SA_VERBOSE_LOG(INFO) << "Menu '" << mName << "' is set invisible from validation.";
    }
    if (!enabled)
    {
      SA_VERBOSE_LOG(INFO) << "Menu '" << mName << "' is set disabled from validation.";
    }

    //update children
    bool all_invisible_children = true;

    //for each child
    Menu::MenuPtrList children = GetSubMenus();
    for (size_t i = 0; i < children.size(); i++)
    {
      Menu* child = children[i];
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
      SA_VERBOSE_LOG(INFO) << "Menu '" << mName << "' is forced invisible because all its children (" << children.size() << ") are invisibles.";
    }
  }

  Menu* Menu::FindMenuByCommandId(const uint32_t& command_id)
  {
    if (mCommandId == command_id)
      return this;

    //for each child
    Menu::MenuPtrList children = GetSubMenus();
    for (size_t i = 0; i < children.size(); i++)
    {
      Menu* child = children[i];
      Menu* match = child->FindMenuByCommandId(command_id);
      if (match)
        return match;
    }

    return NULL;
  }

  Menu* Menu::FindMenuByName(const std::string& name, FIND_BY_NAME_FLAGS flags)
  {
    // Get the menu name and expand it if requested.
    std::string menu_name = mName;
    if (flags & FIND_BY_NAME_EXPANDS)
    {
      PropertyManager& pmgr = PropertyManager::GetInstance();
      std::string expanded = pmgr.Expand(mName);
      menu_name = expanded;
    }

    // Is it this menu?
    if (menu_name == name)
      return this;
    else if (flags & FIND_BY_NAME_CASE_INSENSITIVE)
    {
      std::string u_menu_name = ra::strings::Uppercase(menu_name);
      std::string u_name = ra::strings::Uppercase(name);
      if (u_menu_name == u_name)
        return this;
    }

    //for each child
    Menu::MenuPtrList children = GetSubMenus();
    for (size_t i = 0; i < children.size(); i++)
    {
      Menu* child = children[i];
      Menu* match = child->FindMenuByName(name, flags);
      if (match)
        return match;
    }

    return NULL;
  }

  uint32_t Menu::AssignCommandIds(const uint32_t& first_command_id)
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
    for (size_t i = 0; i < children.size(); i++)
    {
      Menu* child = children[i];

      if (mCommandId == INVALID_COMMAND_ID)
        child->AssignCommandIds(INVALID_COMMAND_ID); //also assign invalid ids to sub menus
      else
        next_command_id = child->AssignCommandIds(next_command_id); //assign the next command ids to sub menus
    }

    return next_command_id;
  }

  const uint32_t& Menu::GetCommandId() const
  {
    return mCommandId;
  }

  void Menu::SetCommandId(const uint32_t& command_id)
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

  const Validator* Menu::GetValidity(size_t index) const
  {
    const Validator* validator = NULL;
    size_t count = mValidities.size();
    if (index < count)
      validator = mValidities[index];
    return validator;
  }

  void Menu::AddValidity(Validator* validator)
  {
    mValidities.push_back(validator);
    validator->SetParentMenu(this);
  }

  size_t Menu::GetVisibilityCount() const
  {
    return mVisibilities.size();
  }

  const Validator* Menu::GetVisibility(size_t index) const
  {
    const Validator* validator = NULL;
    size_t count = mVisibilities.size();
    if (index < count)
      validator = mVisibilities[index];
    return validator;
  }

  void Menu::AddVisibility(Validator* validator)
  {
    mVisibilities.push_back(validator);
    validator->SetParentMenu(this);
  }

  Menu::MenuPtrList Menu::GetSubMenus()
  {
    return mSubMenus;
  }

  void Menu::AddMenu(Menu* menu)
  {
    mSubMenus.push_back(menu);
    menu->SetParentMenu(this);
  }

  void Menu::AddAction(IAction* action)
  {
    mActions.push_back(action);
    action->SetParentMenu(this);
  }

  const IAction::ActionPtrList& Menu::GetActions() const
  {
    return mActions;
  }

  std::string Menu::ToShortString() const
  {
    std::string str;
    str += "Menu ";
    str += ToHexString(this);
    if (mSeparator || mColumnSeparator)
    {
      str += ", SEPARATOR";
    }
    if (!mName.empty())
    {
      str += ", ";
      str += mName;
    }
    if (!mVisible)
    {
      str += ", ";
      str += "INVISIBLE";
    }
    if (!mEnabled)
    {
      str += ", ";
      str += "DISABLED";
    }
    if (mCommandId != 0)
    {
      str += ", ";
      str += "id=";
      str += ra::strings::ToString(mCommandId);
    }
    //if (mIcon.IsValid())
    //{
    //  str += ", ";
    //  str += mIcon.ToShortString();
    //}
    if (mSubMenus.size())
    {
      str += ", ";
      IObject::AppendObjectCount(str, "submenu", mSubMenus.size());
    }
    if (mValidities.size())
    {
      str += ", ";
      IObject::AppendObjectCount(str, "validity", "validities", mValidities.size());
    }
    if (mVisibilities.size())
    {
      str += ", ";
      IObject::AppendObjectCount(str, "visibility", "visibilities", mVisibilities.size());
    }
    if (mActions.size())
    {
      str += ", ";
      IObject::AppendObjectCount(str, "action", mActions.size());
    }
    return str;
  }

  void Menu::ToLongString(std::string& str, int indent) const
  {
    const bool have_children = (mVisibilities.size() + mValidities.size() + mSubMenus.size() > 0);
    const std::string indent_str = std::string(indent, ' ');

    const std::string short_string = ToShortString();
    str += indent_str + short_string;
    if (have_children)
    {
      str += " {\n";

      // print visibility children
      if (mVisibilities.size())
      {
        str += indent_str + "  Visibilities:\n";
      }
      for (size_t i = 0; i < mVisibilities.size(); i++)
      {
        Validator* validator = mVisibilities[i];
        validator->ToLongString(str, indent + 4);

        str += "\n";
      }
      // print validity children
      if (mValidities.size())
      {
        str += indent_str + "  Validities:\n";
      }
      for (size_t i = 0; i < mValidities.size(); i++)
      {
        Validator* validator = mValidities[i];
        validator->ToLongString(str, indent + 4);

        str += "\n";
      }
      // print menu children
      for (size_t i = 0; i < mSubMenus.size(); i++)
      {
        Menu* submenu = mSubMenus[i];
        submenu->ToLongString(str, indent + 2);

        str += "\n";
      }

      str += indent_str + "}";
    }
  }

} //namespace shellanything
