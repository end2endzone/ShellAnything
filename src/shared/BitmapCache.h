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

#ifndef SA_BITMAPCACHE_H
#define SA_BITMAPCACHE_H

#include <map>
#include <string>
#include <Windows.h>

namespace shellanything
{

  class BitmapCache
  {
  public:
    BitmapCache();
    virtual ~BitmapCache();

    static const HBITMAP INVALID_BITMAP_HANDLE;

    void Clear();
    void ClearAndDestroy();
    void ResetCounters();
    int DestroyOldHandles();
    void AddHandle(const std::string& filename, const int& index, HBITMAP hBitmap);
    HBITMAP FindHandle(const std::string& filename, const int& index);
    int GetUsage(const std::string& filename, const int& index);

  private:
    // Typedef
    struct USAGE
    {
      HBITMAP hBitmap;
      int count;
    };

    // Typedef
    typedef std::map<int          /*index*/, USAGE> IndexMap;
    typedef std::map<std::string  /*filename*/, IndexMap> FilenameMap;

    FilenameMap mFiles;
  };

} //namespace shellanything

#endif //SA_BITMAPCACHE_H
