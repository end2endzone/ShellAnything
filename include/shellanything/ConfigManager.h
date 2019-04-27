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

#ifndef SA_CONFIGMANAGER_H
#define SA_CONFIGMANAGER_H

#include "shellanything/Node.h"
#include "shellanything/Configuration.h"
#include "shellanything/Context.h"

namespace shellanything
{

  class ConfigManager
  {
  private:
    ConfigManager();
    virtual ~ConfigManager();

  public:
    typedef std::vector<std::string> PathList;

    static ConfigManager & getInstance();

    Configuration * loadFile(const std::string & path, std::string & error);
    
    void refresh();

    Menu * findMenuByCommandId(const uint32_t & iCommandId);
    uint32_t assignCommandIds(const uint32_t & iFirstCommandId);

    Configuration::ConfigurationPtrList getConfigurations();

    void clearSearchPath();
    void addSearchPath(const std::string & path);

    bool isLoadedConfigurationFile(const std::string & path) const;

  private:
    //methods
    static bool isConfigurationFile(const std::string & path);

    //attributes
    PathList mPaths;
    Node mConfigurations;
  };

} //namespace shellanything

#endif //SA_CONFIGMANAGER_H
