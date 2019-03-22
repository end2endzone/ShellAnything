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

#ifndef SA_NODEFACTORY_H
#define SA_NODEFACTORY_H

#include "shellanything/Item.h"
#include "shellanything/Action.h"
#include "shellanything/Icon.h"
#include "shellanything/Validator.h"
#include "tinyxml2.h"

namespace shellanything
{

  class NodeFactory
  {
  private:
    NodeFactory();
    virtual ~NodeFactory();

  public:
    static NodeFactory & getInstance();

    bool parseIcon(const tinyxml2::XMLElement * element, Icon & icon, std::string & error);
    bool parseValidator(const tinyxml2::XMLElement * element, Validator & validator, std::string & error);
    Action * parseAction(const tinyxml2::XMLElement * element, std::string & error);
    Item * parseItem(const tinyxml2::XMLElement * element, std::string & error);

  };

} //namespace shellanything

#endif //SA_NODEFACTORY_H
