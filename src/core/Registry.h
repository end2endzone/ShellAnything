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

#ifndef SA_REGISTRY_H
#define SA_REGISTRY_H

#include "shellanything/export.h"
#include "shellanything/config.h"
#include "Action.h"
#include "IActionFactory.h"
#include <map>

namespace shellanything
{

  /// <summary>
  /// Abstract action class.
  /// </summary>
  class SHELLANYTHING_EXPORT Registry
  {
  public:

    Registry();
    virtual ~Registry();

  private:
    // Disable copy constructor and copy operator
    Registry(const Registry&);
    Registry& operator=(const Registry&);
  public:

    /// <summary>
    /// Get an IActionFactory instance that can parse the given xml element name.
    /// </summary>
    /// <param name="name">The name of the xml element.</param>
    /// <returns>Returns an IActionFactory instance that can parse the given name. Returns NULL otherwise.</returns>
    IActionFactory* GetActionFactoryFromName(const std::string& name) const;

    /// <summary>
    /// Add an IActionFactory to the registry. The registry takes ownership of the given IActionFactory.
    /// </summary>
    /// <param name="factory">A valid IActionFactory instance</param>
    void AddActionFactory(IActionFactory* factory);

  private:

    //------------------------
    // Typedef
    //------------------------
    typedef std::map<std::string /*name*/, IActionFactory* /*factory*/> ActionFactoryMap;

    //------------------------
    // Members
    //------------------------
    ActionFactoryMap mActionFactories;
  };


} //namespace shellanything

#endif //SA_REGISTRY_H
