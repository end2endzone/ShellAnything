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

#ifndef SA_ACTION_PROMPT_H
#define SA_ACTION_PROMPT_H

#include "IAction.h"
#include "BaseAction.h"
#include "IActionFactory.h"

namespace shellanything
{

  /// <summary>
  /// Action class that prompts the user for a value.
  /// </summary>
  class SHELLANYTHING_EXPORT ActionPrompt : public BaseAction
  {
  public:
    ActionPrompt();
    virtual ~ActionPrompt();

    /// <summary>
    /// Name of the xml element for this action.
    /// </summary>
    static const std::string XML_ELEMENT_NAME;

    /// <summary>
    /// Instanciate an IActionFactory that is able to parse this action.
    /// </summary>
    /// <returns>Returns a IActionFactory to parse this action.</returns>
    static IActionFactory* NewFactory();

    /// <summary>
    /// Returns true if the ActionPrompt is configued as a Yes/No question.
    /// </summary>
    /// <returns>Returns true if the ActionPrompt is configued as a Yes/No question. Returns false otherwise.</returns>
    virtual bool IsYesNoQuestion() const;

    /// <summary>
    /// Returns true if the ActionPrompt is configued as a OK only question.
    /// </summary>
    /// <returns>Returns true if the ActionPrompt is configued as a OK question. Returns false otherwise.</returns>
    virtual bool IsOkQuestion() const;

    /// <summary>
    /// Prompts the user for a value.
    /// </summary>
    /// <param name="context">The current context of execution.</param>
    /// <returns>Returns true if the execution is successful. Returns false otherwise.</returns>
    virtual bool Execute(const SelectionContext& context) const;

    /// <summary>
    /// Getter for the 'name' parameter.
    /// </summary>
    const std::string& GetName() const;

    /// <summary>
    /// Setter for the 'name' parameter.
    /// </summary>
    void SetName(const std::string& name);

    /// <summary>
    /// Getter for the 'type' parameter.
    /// </summary>
    const std::string& GetType() const;

    /// <summary>
    /// Setter for the 'type' parameter.
    /// </summary>
    void SetType(const std::string& type);

    /// <summary>
    /// Getter for the 'title' parameter.
    /// </summary>
    const std::string& GetTitle() const;

    /// <summary>
    /// Setter for the 'title' parameter.
    /// </summary>
    void SetTitle(const std::string& title);

    /// <summary>
    /// Getter for the 'default' parameter.
    /// </summary>
    const std::string& GetDefault() const;

    /// <summary>
    /// Setter for the 'default' parameter.
    /// </summary>
    void SetDefault(const std::string& default_value);

    /// <summary>
    /// Getter for the 'valueyes' parameter.
    /// </summary>
    const std::string& GetValueYes() const;

    /// <summary>
    /// Setter for the 'valueyes' parameter.
    /// </summary>
    void SetValueYes(const std::string& value_yes);

    /// <summary>
    /// Getter for the 'valueno' parameter.
    /// </summary>
    const std::string& GetValueNo() const;

    /// <summary>
    /// Setter for the 'valueno' parameter.
    /// </summary>
    void SetValueNo(const std::string& value_no);

  private:
    std::string mType;
    std::string mName;
    std::string mTitle;
    std::string mDefault;
    std::string mValueYes;
    std::string mValueNo;
  };

} //namespace shellanything

#endif //SA_ACTION_PROMPT_H
