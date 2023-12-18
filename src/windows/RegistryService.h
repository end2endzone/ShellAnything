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

#ifndef SA_REGISTRY_SERVICE_H
#define SA_REGISTRY_SERVICE_H

#include "sa_windows_export.h"
#include "IRegistryService.h"

namespace shellanything
{
  /// <summary>
  /// Abstract IRegistryService class.
  /// </summary>
  class SA_WINDOWS_EXPORT RegistryService : public virtual IRegistryService
  {
  public:
    RegistryService();
    virtual ~RegistryService();

  private:
    // Disable and copy constructor, dtor and copy operator
    RegistryService(const RegistryService&);
    RegistryService& operator=(const RegistryService&);
  public:

    /// <summary>
    /// Get a registry key as a string.
    /// </summary>
    /// <param name="path">The path to a registry key or a registry value.</param>
    /// <param name="value">The output value to store the result.</param>
    /// <returns>Returns true if the registry key/value is found. Returns false otherwise.</returns>
    virtual bool GetRegistryKeyAsString(const std::string& path, std::string& value);

  };

} //namespace shellanything

#endif //SA_REGISTRY_SERVICE_H
