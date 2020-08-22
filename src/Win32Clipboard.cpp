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

#include <windows.h>
#undef min
#undef max

#include <shlobj.h>
#include <shellApi.h>

#include "Win32Clipboard.h"

#include "rapidassist/strings.h"
#include "rapidassist/environment.h"
#include "rapidassist/timing.h"
#include "rapidassist/unicode.h"

namespace Win32Clipboard
{
  //Format descriptors for different types of object sent to the clipboard
  static const UINT gFormatDescriptorBinary     = RegisterClipboardFormat("Binary");
  static const UINT gFormatDescriptorDropEffect = RegisterClipboardFormat("Preferred DropEffect");

  static const std::string CRLF = ra::environment::GetLineSeparator();
  static const std::string  EMPTY_STRING;
  static const std::wstring EMPTY_WIDE_STRING;

  #define DEFAULT_READ_CLIPBOARD_HANDLE   NULL
  #define DEFAULT_WRITE_CLIPBOARD_HANDLE  GetDesktopWindow()

  std::string getLastErrorDescription()
  {
    DWORD dwLastError = ::GetLastError();
    char lpErrorBuffer[10240] = {0};
    DWORD dwErrorBufferSize = sizeof(lpErrorBuffer);
    ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
                    NULL,
                    dwLastError,
                    MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
                    lpErrorBuffer,
                    dwErrorBufferSize-1,
                    NULL);
    char lpDescBuffer[10240] = {0};
    sprintf(lpDescBuffer, "Error %d, %s", dwLastError, lpErrorBuffer);
    return std::string(lpDescBuffer);
  }

  class ClipboardObject
  {
  public:
    ClipboardObject(HWND hWnd)
    {
      mHwnd = hWnd;
      mOpenStatus = FALSE;
      mCloseStatus = FALSE;

      //Calling OpenClipboard() following a CloseClipboard() may sometimes fails with "Error 0x00000005, Access is denied."
      //Retry a maximum of 5 times to open the clipboard
      for(size_t i=0; i<5 && mOpenStatus == FALSE; i++)
      {
        mOpenStatus = OpenClipboard(mHwnd);
        if (mOpenStatus == FALSE)
        {
          std::string error = getLastErrorDescription();

          //Failed opening the clipboard object. Will try again little bit later
          ra::timing::Millisleep(50);
        }
      }
    }

    bool isOpened()
    {
      return (mOpenStatus != 0);
    }

    ~ClipboardObject()
    {
      if (isOpened())
      {
        mCloseStatus = CloseClipboard();
      }
    }

  private:
    HWND mHwnd;
    BOOL mOpenStatus;
    BOOL mCloseStatus;
  };

  Clipboard::Clipboard()
  {
  }

  Clipboard::~Clipboard()
  {
  }

  Clipboard & Clipboard::GetInstance()
  {
    static Clipboard _instance;
    return _instance;
  }

  bool Clipboard::Empty()
  {
    ClipboardObject obj( DEFAULT_WRITE_CLIPBOARD_HANDLE );
    if (!obj.isOpened())
      return false;

    BOOL empty = EmptyClipboard();
    
    return (empty == TRUE);
  }

  bool Clipboard::IsEmpty()
  {
    //Check if the clipboard Contains any of the known formats
    for(size_t i=0; i<Clipboard::NUM_FORMATS; i++)
    {
      Clipboard::Format test_format = (Clipboard::Format)i;
      if (Contains(test_format))
      {
        return false;
      }
    }
    return true;
  }

  bool Clipboard::Contains(Clipboard::Format iClipboardFormat)
  {
    ClipboardObject obj( DEFAULT_READ_CLIPBOARD_HANDLE );
    if (!obj.isOpened())
      return false;

    bool containsFormat = false;
    switch(iClipboardFormat)
    {
    case Clipboard::FormatText:
      {
        HANDLE hData = GetClipboardData(CF_TEXT);
        containsFormat = (hData != NULL);
      }
      break;
    case Clipboard::FormatUnicode:
      {
        HANDLE hData = GetClipboardData(CF_UNICODETEXT);
        containsFormat = (hData != NULL);
      }
      break;
    case Clipboard::FormatImage:
      {
        HANDLE hData = GetClipboardData(CF_BITMAP);
        containsFormat = (hData != NULL);
      }
      break;
    case Clipboard::FormatBinary:
      {
        HANDLE hData = GetClipboardData(gFormatDescriptorBinary);
        containsFormat = (hData != NULL);
      }
      break;
    };

    return containsFormat;
  }

  template <typename T> inline bool setTextT(UINT uFormat, const T* str, size_t length)
  {
    ClipboardObject obj( DEFAULT_WRITE_CLIPBOARD_HANDLE );
    if (!obj.isOpened())
      return false;

    //flush existing content
    if (!EmptyClipboard())
      return false;

    size_t memory_size = (length+1)*sizeof(T); // +1 character to include the NULL terminating character

    //copy data to global allocated memory
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, memory_size);
    if (hMem == NULL)
      return false;
    void * buffer = GlobalLock(hMem);
    memcpy(buffer, str, memory_size);
    GlobalUnlock(hMem);

    //put it on the clipboard
    HANDLE hData = SetClipboardData(uFormat, hMem);
    if (hData != hMem)
      return false;

    return true;
  }
 
  bool Clipboard::SetText(const std::string & iText)
  {
    return setTextT<char>(CF_TEXT, iText.data(), iText.length());
  }

  bool Clipboard::GetAsText(std::string & oText)
  {
    ClipboardObject obj( DEFAULT_READ_CLIPBOARD_HANDLE );
    if (!obj.isOpened())
      return false;

    HANDLE hData = GetClipboardData(CF_TEXT);
    if (hData == NULL)
      return false;

    size_t data_size = (size_t)GlobalSize(hData);
    size_t count = data_size / sizeof(char);
    const char* text_buffer = (const char*)GlobalLock(hData);
    oText.assign(text_buffer, count-1); //copy the data to output variable, minus the last \0 character
    GlobalUnlock( hData );

    return true;
  }

  bool Clipboard::SetTextUnicode(const std::wstring & iText)
  {
    return setTextT<wchar_t>(CF_UNICODETEXT, iText.data(), iText.length());
  }

  bool Clipboard::GetAsTextUnicode(std::wstring & oText)
  {
    ClipboardObject obj(DEFAULT_READ_CLIPBOARD_HANDLE);
    if (!obj.isOpened())
      return false;

    HANDLE hData = GetClipboardData(CF_UNICODETEXT);
    if (hData == NULL)
      return false;

    size_t data_size = (size_t)GlobalSize(hData);
    size_t count = data_size / sizeof(wchar_t);
    const wchar_t* text_buffer = (const wchar_t*)GlobalLock(hData);
    oText.assign(text_buffer, count- 1); //copy the data to output variable, minus the last \0 character
    GlobalUnlock(hData);

    return true;
  }

  bool Clipboard::SetBinary(const MemoryBuffer & iMemoryBuffer)
  {
    ClipboardObject obj( DEFAULT_WRITE_CLIPBOARD_HANDLE );
    if (!obj.isOpened())
      return false;

    //flush existing content
    if (!EmptyClipboard())
      return false;

    //copy data to global allocated memory
    HGLOBAL hMem = GlobalAlloc(GMEM_DDESHARE, iMemoryBuffer.size());
    if (hMem == NULL)
      return false;
    void * buffer = GlobalLock(hMem);
    memcpy(buffer, iMemoryBuffer.c_str(), iMemoryBuffer.size());
    GlobalUnlock(hMem);

    //put it on the clipboard
    HANDLE hData = SetClipboardData(gFormatDescriptorBinary, hMem);
    if (hData != hMem)
      return false;

    return true;
  }

  bool Clipboard::GetAsBinary(MemoryBuffer & oMemoryBuffer)
  {
    ClipboardObject obj( DEFAULT_READ_CLIPBOARD_HANDLE );
    if (!obj.isOpened())
      return false;

    //get the buffer
    HANDLE hData = GetClipboardData(gFormatDescriptorBinary);
    if (hData == NULL)
      return false;

    size_t data_size = (size_t)GlobalSize(hData);
    void * buffer = GlobalLock( hData );
    oMemoryBuffer.assign((const char*)buffer, data_size); //copy the data to output variable
    GlobalUnlock( hData );
    
    return true;
  }

  bool Clipboard::SetDragDropFiles(const Clipboard::DragDropType & iDragDropType, const Clipboard::StringVector & iFiles)
  {
    //http://support.microsoft.com/kb/231721/en-us
    //http://aclacl.brinkster.net/MFC/ch19b.htm
    //http://read.pudn.com/downloads22/sourcecode/windows/multimedia/73340/ShitClass3/%E7%B1%BB%E5%8C%85/SManageFile.cpp__.htm

    //Validate drag drop type
    if (iDragDropType != Clipboard::DragDropCopy && iDragDropType != Clipboard::DragDropCut)
      return false;

    ClipboardObject obj( DEFAULT_WRITE_CLIPBOARD_HANDLE );
    if (!obj.isOpened())
      return false;

    //flush existing content
    if (!EmptyClipboard())
      return false;

    //Register iFiles
    {
      DROPFILES df = {0};
      df.pFiles = sizeof(DROPFILES);
      df.pt.x = 0;
      df.pt.y = 0;
      df.fNC = FALSE;
      df.fWide = TRUE; //we will use WIDE CHAR for storing the file paths

      //Build the buffer content
      MemoryBuffer buff;

      //append the DROPFILES structure
      buff.assign((const char *)&df, sizeof(df));

      //append each files
      for(size_t i=0; i<iFiles.size(); i++)
      {
        const std::string & utf8_file_path = iFiles[i];

        //Convert utf8 to unicode
        std::wstring unicode_file_path = ra::unicode::Utf8ToUnicode(utf8_file_path);

        //append
        const char * data_buffer = (const char*)unicode_file_path.c_str();
        const size_t data_size_bytes = (unicode_file_path.size() + 1) * sizeof(unicode_file_path[0]); // +1 for including the NULL terminating character
        buff.append(data_buffer, data_size_bytes);
      }
        
      //Append final empty filepath
      {
        //Convert utf8 to unicode
        std::wstring unicode_file_path = ra::unicode::Utf8ToUnicode("");

        //append
        const char * data_buffer = (const char*)unicode_file_path.c_str();
        const size_t data_size_bytes = (unicode_file_path.size() + 1) * sizeof(unicode_file_path[0]); // +1 for including the NULL terminating character
        buff.append(data_buffer, data_size_bytes);
      }

      //copy data to global allocated memory
      HGLOBAL hMem = GlobalAlloc(GMEM_ZEROINIT|GMEM_MOVEABLE|GMEM_DDESHARE, buff.size());
      if (hMem == NULL)
        return false;
      void * buffer = GlobalLock(hMem);
      memcpy(buffer, buff.c_str(), buff.size());
      GlobalUnlock(hMem);

      //put it on the clipboard
      HANDLE hData = SetClipboardData( CF_HDROP, hMem );
      if (hData != hMem)
        return false;
    }

    //Register iDragDropType
    {
      HGLOBAL hDropEffect = GlobalAlloc(GMEM_ZEROINIT|GMEM_MOVEABLE|GMEM_DDESHARE,sizeof(DWORD));
      DWORD * dwDropEffect = (DWORD*)GlobalLock(hDropEffect);
      if (iDragDropType == Clipboard::DragDropCopy)
        (*dwDropEffect) = DROPEFFECT_COPY /*| DROPEFFECT_LINK*/;
      else
        (*dwDropEffect) = DROPEFFECT_MOVE;
      GlobalUnlock(hDropEffect);

      //put it on the clipboard
      HANDLE hData = SetClipboardData( gFormatDescriptorDropEffect, hDropEffect );
      if (hData != hDropEffect)
        return false;
    }

    return true;
  }

  bool Clipboard::GetAsDragDropFiles(DragDropType & oDragDropType, Clipboard::StringVector & oFiles)
  {
    //Invalidate
    oDragDropType = Clipboard::DragDropType(-1);
    oFiles.clear();

    ClipboardObject obj( DEFAULT_READ_CLIPBOARD_HANDLE );
    if (!obj.isOpened())
      return false;

    //Detect if CUT or COPY
    {
      HANDLE hDropEffect = ::GetClipboardData (gFormatDescriptorDropEffect);
      if (hDropEffect)
      {
        LPVOID lpResults = GlobalLock(hDropEffect);
        {
          SIZE_T lBufferSize = GlobalSize(lpResults);
          DWORD dropEffect = *((DWORD*)lpResults);
          //if (dropEffect == DROPEFFECT_NONE)
          //  "NONE:"
          if (dropEffect & DROPEFFECT_COPY)
            oDragDropType = DragDropCopy;
          else if (dropEffect & DROPEFFECT_MOVE)
            oDragDropType = DragDropCut;
          //if (dropEffect & DROPEFFECT_LINK)
          //  "LINK:"
          //if (dropEffect & DROPEFFECT_SCROLL)
          //  "SCROLL:"
        }
        GlobalUnlock(hDropEffect);
      }

      if (oDragDropType == -1)
      {
        //unknown drop effect
        return false;
      }
    }

    //Retreive files
    HDROP hDrop = (HDROP) ::GetClipboardData (CF_HDROP);
    if (hDrop == NULL)
      return false;

    LPVOID lpResults = GlobalLock(hDrop);
    if (hDrop == NULL)
      return false;

    SIZE_T lBufferSize = GlobalSize(lpResults);
              
    // Find out how many file names the HDROP Contains.
    int nCount = ::DragQueryFile (hDrop, (UINT) -1, NULL, 0);
    if (nCount == 0)
      return false;

    //Find out if files are unicode or ansi
    DROPFILES df = {0};
    df = *( ((DROPFILES*)lpResults) );
    bool isUnicode = (df.fWide == 1);

    // Enumerate the file names.
    for (int i=0; i<nCount; i++)
    {
      std::string file_path;

      if (isUnicode)
      {
        WCHAR szFile[MAX_PATH];
        UINT length = ::DragQueryFileW (hDrop, i, szFile, sizeof (szFile) / sizeof (WCHAR));
                  
        std::wstring utf16_file_path = szFile;

        //Convert from unicode to ansi
        file_path = ra::unicode::UnicodeToAnsi( utf16_file_path );
      }
      else
      {
        char szFile[MAX_PATH];
        ::DragQueryFileA (hDrop, i, szFile, sizeof (szFile) / sizeof (char));
        file_path = szFile;
      }

      //Add to output files
      //printf("Reading from clipboard %02d/%02d: %s\n", i+1, nCount, filePath.c_str());
      oFiles.push_back(file_path);
    }

    GlobalUnlock(hDrop);
          
    return true;
  }
} //namespace Win32Clipboard
