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

#include "ActionManager.h"
#include "PropertyManager.h"
#include "LoggerHelper.h"

#include "SaUtils.h"

#include "rapidassist/errors.h"

namespace shellanything
{

  bool ActionManager::Execute(const Menu* menu, const SelectionContext& context)
  {
    shellanything::ScopeLogger verbose_scope_logger(__FUNCTION__ "()", true);

    //compute the visual menu title
    shellanything::PropertyManager& pmgr = shellanything::PropertyManager::GetInstance();
    std::string title = pmgr.Expand(menu->GetName());

    bool success = true;

    SA_LOG(INFO) << "Executing action(s) for menu '" << title.c_str() << "', id=" << menu->GetCommandId() << "...";

    //execute actions
    const shellanything::IAction::ActionPtrList& actions = menu->GetActions();
    for (size_t i = 0; i < actions.size(); i++)
    {
      SA_LOG(INFO) << "Executing action " << (i + 1) << " of " << actions.size() << ".";
      const shellanything::IAction* action = actions[i];
      if (action)
      {
        ra::errors::ResetLastErrorCode(); //reset win32 error code in case the action fails.
        success = success && action->Execute(context);

        if (!success)
        {
          //try to get an error message from win32
          ra::errors::errorcode_t dwError = ra::errors::GetLastErrorCode();
          if (dwError)
          {
            std::string error_message = ra::errors::GetErrorCodeDescription(dwError);
            SA_LOG(ERROR) << "Action #" << (i + 1) << " has failed: " << ToHexString(dwError) << ", " << error_message;
          }
          else
          {
            //simply log an error
            SA_LOG(ERROR) << "Action #" << (i + 1) << " has failed.";
          }

          //stop executing the next actions
          i = actions.size();
        }
      }
    }

    if (success)
      SA_LOG(INFO) << "Executing action(s) for menu '" << title.c_str() << "' completed.";
    else
      SA_LOG(WARNING) << "Executing action(s) for menu '" << title.c_str() << "' completed with errors.";

    return success;
  }

} //namespace shellanything
