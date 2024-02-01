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

#include "PcgRandomService.h"

#include "pcg_random.hpp"

#include <random>

namespace shellanything
{
  inline pcg32& GetPcg(void* pimpl)
  {
    // mPimpl to local pcg reference
    pcg32& rng = *((pcg32*)(pimpl));

    return rng;
  }

  PcgRandomService::PcgRandomService() :
    mPimpl(NULL)
  {
    // Seed with a real random value, if available
    pcg_extras::seed_seq_from<std::random_device> seed_source;

    mPimpl = new pcg32(seed_source);
  }

  PcgRandomService::~PcgRandomService()
  {
    if (mPimpl)
      delete mPimpl;
    mPimpl = NULL;
  }

  uint32_t PcgRandomService::GetRandomValue()
  {
    pcg32& rng = GetPcg(mPimpl);

    uint32_t value = rng();
    return value;
  }

  uint32_t PcgRandomService::GetRandomValue(uint32_t min_value, uint32_t max_value)
  {
    pcg32& rng = GetPcg(mPimpl);

    uint32_t range = max_value - min_value;
    uint32_t value = rng(range) + min_value;
    return value;
  }

  bool PcgRandomService::Seed()
  {
    pcg32& rng = GetPcg(mPimpl);

    // Seed with a real random value, if available
    pcg_extras::seed_seq_from<std::random_device> seed_source;

    rng.seed(seed_source);
    return true;
  }

  bool PcgRandomService::Seed(uint32_t seed)
  {
    pcg32& rng = GetPcg(mPimpl);
    rng.seed(seed);
    return true;
  }

  bool PcgRandomService::Seed(uint64_t seed)
  {
    pcg32& rng = GetPcg(mPimpl);
    rng.seed(seed);
    return true;
  }

} //namespace shellanything
