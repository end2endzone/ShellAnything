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

#ifndef SA_IRANDOM_SERVICE_H
#define SA_IRANDOM_SERVICE_H

#include "shellanything/export.h"
#include "shellanything/config.h"

#include <stdint.h>

namespace shellanything
{
  /// <summary>
  /// Abstract random service class.
  /// Used to decouple the core from an external random library.
  /// </summary>
  class SHELLANYTHING_EXPORT IRandomService
  {
  public:
    IRandomService(); 
    virtual ~IRandomService();

  private:
    // Disable and copy constructor, dtor and copy operator
    IRandomService(const IRandomService&);
    IRandomService& operator=(const IRandomService&);
  public:

    /// <summary>
    /// Get a random 32 bit value between 0 and uint32_t::max - 1.
    /// </summary>
    /// <returns>Returns a valid uint32_t value.</returns>
    virtual uint32_t GetRandomValue() = 0;

    /// <summary>
    /// Get a random 32 bit value in [min_value,max_value[.
    /// </summary>
    /// <param name="min_value">The minimum range of the function range (included).</param>
    /// <param name="max_value">The maximum range of the function range (excluded).</param>
    /// <returns>Returns a valid uint32_t value.</returns>
    virtual uint32_t GetRandomValue(uint32_t min_value, uint32_t max_value) = 0;

    /// <summary>
    /// Seed the random number generator with a real random value, if available.
    /// </summary>
    /// <returns>Returns true if the operation is succesful. Return false otherwise.</returns>
    virtual bool Seed() = 0;

    /// <summary>
    /// Seed the random number generator with a custom 32 bit value.
    /// </summary>
    /// <param name="seed">The seed value.</param>
    /// <returns>Returns true if the operation is succesful. Return false otherwise.</returns>
    virtual bool Seed(uint32_t seed) = 0;

    /// <summary>
    /// Seed the random number generator with a custom 32 bit value.
    /// </summary>
    /// <param name="seed">The seed value.</param>
    /// <returns>Returns true if the operation is succesful. Return false otherwise.</returns>
    virtual bool Seed(uint64_t seed) = 0;

  };

} //namespace shellanything

#endif //SA_IRANDOM_SERVICE_H
