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

#include "BitmapCache.h"
#include <vector>

namespace shellanything
{   
  const HBITMAP BitmapCache::INVALID_BITMAP_HANDLE = 0;
   
  BitmapCache::BitmapCache()
  {
  }
   
  BitmapCache::~BitmapCache()
  {
    clear_and_destroy();
  }
   
  void BitmapCache::clear()
  {
    mFiles.clear();
  }
   
  void BitmapCache::clear_and_destroy()
  {
    //for each files
    for (FilenameMap::const_iterator wFilesIterator = mFiles.begin(); wFilesIterator != mFiles.end(); wFilesIterator++)
    {
      const std::string & filename    = wFilesIterator->first;
      const IndexMap    & indice_map  = wFilesIterator->second;
   
      //for each indice
      for (IndexMap::const_iterator wIndiceIterator = indice_map.begin(); wIndiceIterator != indice_map.end(); wIndiceIterator++)
      {
        const int & index   = wIndiceIterator->first;
        const USAGE & usage = wIndiceIterator->second;
   
        //delete the bitmap
        DeleteObject(usage.hBitmap);
      }
    }
   
    mFiles.clear();
  }
   
  void BitmapCache::reset_counters()
  {
    //for each files
    for (FilenameMap::iterator wFilesIterator = mFiles.begin(); wFilesIterator != mFiles.end(); wFilesIterator++)
    {
      const std::string & filename    = wFilesIterator->first;
            IndexMap    & indice_map  = wFilesIterator->second;
   
      //for each indice
      for (IndexMap::iterator wIndiceIterator = indice_map.begin(); wIndiceIterator != indice_map.end(); wIndiceIterator++)
      {
        const int & index = wIndiceIterator->first;
            USAGE & usage = wIndiceIterator->second;
   
        //reset
        usage.count = 0;
      }
    }
  }
   
  void BitmapCache::destroy_old_elements()
  {
    struct DELETE_ELEMENT
    {
      std::string filename;
      int index;
    };
   
    typedef std::vector<DELETE_ELEMENT> DeleteElementList;
    DeleteElementList deletions;
   
    //for each files
    for (FilenameMap::iterator wFilesIterator = mFiles.begin(); wFilesIterator != mFiles.end(); wFilesIterator++)
    {
      const std::string & filename    = wFilesIterator->first;
            IndexMap    & indice_map  = wFilesIterator->second;
   
      //for each indice
      for (IndexMap::iterator wIndiceIterator = indice_map.begin(); wIndiceIterator != indice_map.end(); wIndiceIterator++)
      {
        const int & index = wIndiceIterator->first;
            USAGE & usage = wIndiceIterator->second;
   
        //destroy if unused
        if (usage.count == 0)
        {
          //delete the bitmap
          DeleteObject(usage.hBitmap);
          usage.hBitmap = INVALID_BITMAP_HANDLE;
   
          //remember this element for deletion
          DELETE_ELEMENT del;
          del.filename = filename;
         del.index = index;
          deletions.push_back(del);
        }
      }
    }
   
    //for each deletion
    for(size_t i=0; i<deletions.size(); i++)
    {
      DELETE_ELEMENT & del = deletions[i];
   
      //search within 1st map level
      FilenameMap::iterator wFilesIterator = mFiles.find(del.filename);
      bool hasFoundFile = (wFilesIterator != mFiles.end());
      if (!hasFoundFile)
        continue; //should not happend
   
      bool delete_filename_record = false;
   
      //IndexMap is valid in the following scope as long as its not deleted
      {
        IndexMap & indice_map = wFilesIterator->second;
   
        //search within 2nd map level
        IndexMap::iterator wIndiceIterator = indice_map.find(del.index);
        bool hasFoundIndex = (wIndiceIterator != indice_map.end());
        if (!hasFoundIndex)
          continue; //should not happend
   
        //delete the record
        indice_map.erase(wIndiceIterator);
        if (indice_map.size() == 0)
        {
          delete_filename_record = true;
        }
      }
   
      if (delete_filename_record)
      {
        //also delete the filename record
        mFiles.erase(wFilesIterator);
      }
    }
  }
   
  void BitmapCache::addHandle(const std::string & iFilename, const int & iIndex, HBITMAP hBitmap)
  {
    USAGE usage;
    usage.hBitmap = hBitmap;
    usage.count = 0;
   
    mFiles[iFilename][iIndex] = usage;
  }
   
  HBITMAP BitmapCache::findHandle(const std::string & iFilename, const int & iIndex)
  {
    //search within 1st map level
    FilenameMap::iterator wFilesIterator = mFiles.find(iFilename);
    bool hasFoundFile = (wFilesIterator != mFiles.end());
    if (!hasFoundFile)
      return INVALID_BITMAP_HANDLE;
   
    IndexMap & indice_map = wFilesIterator->second;
   
    //search within 2nd map level
    IndexMap::iterator wIndiceIterator = indice_map.find(iIndex);
    bool hasFoundIndex = (wIndiceIterator != indice_map.end());
    if (!hasFoundIndex)
      return INVALID_BITMAP_HANDLE;
   
    USAGE & usage = wIndiceIterator->second;
   
    //increase how many time we requested this bitmap
    usage.count++;
   
    return usage.hBitmap;
  }

} //namespace shellanything
