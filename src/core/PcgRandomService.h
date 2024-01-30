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

#ifndef SA_PCG_RANDOM_SERVICE_H
#define SA_PCG_RANDOM_SERVICE_H

#include "IRandomService.h"

namespace shellanything
{

  /// <summary>
  /// pcf-cpp implementation class of IRandomService.
  /// </summary>
  class SHELLANYTHING_EXPORT PcgRandomService : public IRandomService
  {
  public:
    PcgRandomService();
    virtual ~PcgRandomService();

    virtual uint32_t GetRandomValue();

    virtual uint32_t GetRandomValue(uint32_t min_value, uint32_t max_value);

  private:
    void * mPimpl;
  };

} //namespace shellanything

#endif //SA_PCG_RANDOM_SERVICE_H
