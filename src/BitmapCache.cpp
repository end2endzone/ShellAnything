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
    ClearAndDestroy();
  }
   
  void BitmapCache::Clear()
  {
    mFiles.clear();
  }
   
  void BitmapCache::ClearAndDestroy()
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
   
  void BitmapCache::ResetCounters()
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
   
  int BitmapCache::DestroyOldHandles()
  {
    int num_destroyed = 0;

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
        num_destroyed++;
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

    return num_destroyed;
  }
   
  void BitmapCache::AddHandle(const std::string & filename, const int & index, HBITMAP hBitmap)
  {
    USAGE usage;
    usage.hBitmap = hBitmap;
    usage.count = 0;
   
    mFiles[filename][index] = usage;
  }
   
  HBITMAP BitmapCache::FindHandle(const std::string & filename, const int & index)
  {
    //search within 1st map level
    FilenameMap::iterator wFilesIterator = mFiles.find(filename);
    bool hasFoundFile = (wFilesIterator != mFiles.end());
    if (!hasFoundFile)
      return INVALID_BITMAP_HANDLE;
   
    IndexMap & indice_map = wFilesIterator->second;
   
    //search within 2nd map level
    IndexMap::iterator wIndiceIterator = indice_map.find(index);
    bool hasFoundIndex = (wIndiceIterator != indice_map.end());
    if (!hasFoundIndex)
      return INVALID_BITMAP_HANDLE;
   
    USAGE & usage = wIndiceIterator->second;
   
    //increase how many time we requested this bitmap
    usage.count++;
   
    return usage.hBitmap;
  }

  int BitmapCache::GetUsage(const std::string & filename, const int & index)
  {
    //search within 1st map level
    FilenameMap::iterator wFilesIterator = mFiles.find(filename);
    bool hasFoundFile = (wFilesIterator != mFiles.end());
    if (!hasFoundFile)
      return -1;
   
    IndexMap & indice_map = wFilesIterator->second;
   
    //search within 2nd map level
    IndexMap::iterator wIndiceIterator = indice_map.find(index);
    bool hasFoundIndex = (wIndiceIterator != indice_map.end());
    if (!hasFoundIndex)
      return -1;
   
    USAGE & usage = wIndiceIterator->second;
   
    return usage.count;
  }

} //namespace shellanything
