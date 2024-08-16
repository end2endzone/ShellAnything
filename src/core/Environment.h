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

#ifndef SA_ENVIRONMENT_H
#define SA_ENVIRONMENT_H

#include "shellanything/export.h"
#include "shellanything/config.h"
#include <string>

namespace shellanything
{
  /// <summary>
  /// Manages the environment variables list for the application.
  /// Environment variables can enable or disable options and features.
  /// </summary>
  class SHELLANYTHING_EXPORT Environment
  {
  public:
    static Environment& GetInstance();
  private:
    Environment();
    ~Environment();

    // Disable copy constructor and copy operator
    Environment(const Environment&);
    Environment& operator=(const Environment&);

  public:

    /// <summary>
    /// Name of the environment variable that defines the verbose logging.
    /// </summary>
    static const std::string SYSTEM_LOGGING_VERBOSE_ENVIRONMENT_VARIABLE_NAME;

  public:

    /// <summary>
    /// Check if an environment variable have been set.
    /// An empty environment variable value is defined as 'set'.
    /// </summary>
    /// <param name="name">The name of the environment variable to check.</param>
    /// <returns>Returns true if the environment variable is set. Returns false otherwise.</returns>
    bool IsOptionSet(const std::string& name) const;

    /// <summary>
    /// Check if an environment variable evaluates to true.
    /// An empty environment variable value returns false.
    /// </summary>
    /// <param name="name">The name of the environment variable to check.</param>
    /// <returns>Returns true if the environment variable evaluates to true. Returns false otherwise.</returns>
    bool IsOptionTrue(const std::string& name) const;

    /// <summary>
    /// Check if an environment variable evaluates to false.
    /// An empty environment variable value returns false.
    /// </summary>
    /// <param name="name">The name of the environment variable to check.</param>
    /// <returns>Returns true if the environment variable evaluates to true. Returns false otherwise.</returns>
    bool IsOptionFalse(const std::string& name) const;

  };

} //namespace shellanything

#endif //SA_ENVIRONMENT_H
