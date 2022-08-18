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

#ifndef SA_PLUGING_H
#define SA_PLUGING_H

#include "shellanything/export.h"
#include "shellanything/config.h"
#include "SelectionContext.h"
#include "Registry.h"
#include <string>
#include <vector>

namespace shellanything
{
  class Configuration; // For Set/GetParentConfiguration()

  /// <summary>
  /// A Plugin class holds a list of plugin features.
  /// </summary>
  class SHELLANYTHING_EXPORT Plugin
  {
  public:
    /// <summary>
    /// A list of Plugin pointer.
    /// </summary>
    typedef std::vector<Plugin*> PluginPtrList;

    /// <summary>
    /// Get the active plugin that is currently loading. See Load() method.
    /// </summary>
    /// <returns>Returns a Plugin pointer which is currently executing the Load() method. Returns NULL if no plugin is actually loading.</returns>
    static Plugin* GetLoadingPlugin();

    Plugin();
    Plugin(const Plugin& p);
    virtual ~Plugin();

    /// <summary>
    /// Get the parent configuration.
    /// </summary>
    /// <returns>Returns a pointer to the parent configuration. Returns NULL if the object has no parent configuration.</returns>
    virtual Configuration* GetParentConfiguration();
    virtual const Configuration* GetParentConfiguration() const;

    /// <summary>
    /// Set the parent configuration.
    /// </summary>
    /// <param name="configuration">The parent configuration of this object</param>
    virtual void SetParentConfiguration(Configuration* configuration);

    /// <summary>
    /// Copy operator
    /// </summary>
    const Plugin& operator =(const Plugin& p);

    /// <summary>
    /// Getter for the 'path' parameter.
    /// </summary>
    const std::string& GetPath() const;

    /// <summary>
    /// Setter for the 'path' parameter.
    /// </summary>
    void SetPath(const std::string& path);

    /// <summary>
    /// Getter for the 'description' parameter.
    /// </summary>
    const std::string& GetDescription() const;

    /// <summary>
    /// Setter for the 'description' parameter.
    /// </summary>
    void SetDescription(const std::string& description);

    /// <summary>
    /// Getter for the 'conditions' parameter.
    /// </summary>
    const std::string& GetConditions() const;

    /// <summary>
    /// Setter for the 'conditions' parameter.
    /// </summary>
    void SetConditions(const std::string& conditions);

    /// <summary>
    /// Getter for the 'actions' parameter.
    /// </summary>
    const std::string& GetActions() const;

    /// <summary>
    /// Setter for the 'actions' parameter.
    /// </summary>
    void SetActions(const std::string& actions);

    /// <summary>
    /// Check if this plugin supports the given condition.
    /// </summary>
    /// <param name="name">The name of the condition to check</param>
    /// <returns>Returns true if this plugin supports the given condition. Returns false otherwise.</returns>
    bool SupportCondition(const std::string& name);

    /// <summary>
    /// Check if this plugin supports the given condition.
    /// </summary>
    /// <param name="name">The name of the condition to check</param>
    /// <returns>Returns true if this plugin supports the given condition. Returns false otherwise.</returns>
    bool SupportAction(const std::string& name);

    /// <summary>
    /// Check if a plugin is loaded.
    /// </summary>
    /// <returns>Returns true if the plugin is loaded. Returns false otherwise.</returns>
    bool IsLoaded() const;

    /// <summary>
    /// Load the plugin into memory.
    /// </summary>
    /// <returns>Returns true when the plugin load is successful. Returns false otherwise.</returns>
    bool Load();

    /// <summary>
    /// Unload the plugin from memory.
    /// </summary>
    /// <returns>Returns true when the plugin unload is successful. Returns false otherwise.</returns>
    bool Unload();

    /// <summary>
    /// Get this plugin Registry class.
    /// </summary>
    /// <returns>Returns this plugin Registry class.</returns>
    Registry& GetRegistry();

    /// <summary>
    /// Find a plugin which has a condition field matching the given name.
    /// </summary>
    /// <param name="plugins">The list of plugins</param>
    /// <param name="name">The name of the attribute to find</param>
    /// <returns>Returns a Plugin pointer matching the query. Returns NULL otherwise</returns>
    static Plugin* FindPluginByConditionName(const PluginPtrList& plugins, const std::string& name);

    /// <summary>
    /// Find a plugin which has a condition field matching the given name.
    /// </summary>
    /// <param name="plugins">The list of plugins</param>
    /// <param name="name">The name of the attribute to find</param>
    /// <returns>Returns a Plugin pointer matching the query. Returns NULL otherwise</returns>
    static Plugin* FindPluginByActionName(const PluginPtrList& plugins, const std::string& name);

  private:
    Configuration* mParentConfiguration;

    std::string mPath;
    std::string mDescription;
    std::string mConditions;
    std::string mActions;

    // Plugin loaded state members
    bool mLoaded;
    struct ENTRY_POINTS;
    ENTRY_POINTS* mEntryPoints;
    Registry mRegistry;
  };


} //namespace shellanything

#endif //SA_CONTEXT_H
