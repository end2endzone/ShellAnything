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

#include <wchar.h>  //for swprintf()

#include <shlobj.h>
#include <shellApi.h>

#include "Win32Clipboard.h"

#include "rapidassist/strings.h"
#include "rapidassist/environment.h"
#include "rapidassist/timing.h"
#include "rapidassist/unicode.h"
#include "rapidassist/errors.h"

namespace Win32Clipboard
{
  //Format descriptors for different types of object sent to the clipboard
  static const UINT gFormatDescriptorBinary     = RegisterClipboardFormatA("Binary");
  static const UINT gFormatDescriptorDropEffect = RegisterClipboardFormatA("Preferred DropEffect");

  static const std::string  CRLF = ra::environment::GetLineSeparator();
  static const std::string  EMPTY_STRING;
  static const std::wstring EMPTY_WIDE_STRING;

  #define DEFAULT_READ_CLIPBOARD_HANDLE   NULL
  #define DEFAULT_WRITE_CLIPBOARD_HANDLE  GetDesktopWindow()

  std::string getLastErrorDescriptionAnsi()
  {
    DWORD last_error = ::GetLastError();
    char error_buffer[10240] = {0};
    DWORD error_buffer_size = sizeof(error_buffer);
    ::FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM,
                    NULL,
                    last_error,
                    MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
                    error_buffer,
                    error_buffer_size-1,
                    NULL);
    char desc_buffer[10240] = {0};
    sprintf(desc_buffer, "Error %d, %s", last_error, error_buffer);
    return std::string(desc_buffer);
  }

  std::wstring getLastErrorDescriptionUnicode()
  {
    DWORD last_error = ::GetLastError();
    wchar_t error_buffer[10240] = { 0 };
    DWORD error_buffer_size = sizeof(error_buffer);
    ::FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM,
      NULL,
      last_error,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      error_buffer,
      error_buffer_size - 1,
      NULL);
    wchar_t desc_buffer[10240] = { 0 };
    _swprintf(desc_buffer, L"Error %d, %s", last_error, error_buffer);
    return std::wstring(desc_buffer);
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
          std::wstring error = getLastErrorDescriptionUnicode();

          //Failed opening the clipboard object. Will try again little bit later
          ra::timing::Millisleep(50);
        }
      }
    }

    bool IsOpened()
    {
      return (mOpenStatus != 0);
    }

    ~ClipboardObject()
    {
      if (IsOpened())
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
    if (!obj.IsOpened())
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

  bool Clipboard::Contains(Clipboard::Format clipboard_format)
  {
    ClipboardObject obj( DEFAULT_READ_CLIPBOARD_HANDLE );
    if (!obj.IsOpened())
      return false;

    //The clipboard controls the handle that the GetClipboardData function returns, not the application.
    //The application should copy the data immediately. The application must not free the handle nor leave it locked.

    bool contains_format = false;
    switch(clipboard_format)
    {
    case Clipboard::FormatText:
      {
        HANDLE hData = GetClipboardData(CF_TEXT);
        contains_format = (hData != NULL);
      }
      break;
    case Clipboard::FormatUnicode:
      {
        HANDLE hData = GetClipboardData(CF_UNICODETEXT);
        contains_format = (hData != NULL);
      }
      break;
    case Clipboard::FormatImage:
      {
        HANDLE hData = GetClipboardData(CF_BITMAP);
        contains_format = (hData != NULL);
      }
      break;
    case Clipboard::FormatBinary:
      {
        HANDLE hData = GetClipboardData(gFormatDescriptorBinary);
        contains_format = (hData != NULL);
      }
      break;
    };

    return contains_format;
  }

  template <typename T> inline bool setTextT(UINT format, const T* str, size_t length)
  {
    ClipboardObject obj( DEFAULT_WRITE_CLIPBOARD_HANDLE );
    if (!obj.IsOpened())
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
    HANDLE hData = SetClipboardData(format, hMem);
    if (hData != hMem)
      return false;

    return true;
  }
 
  bool Clipboard::SetText(const std::string & text)
  {
    return setTextT<char>(CF_TEXT, text.data(), text.length());
  }

  bool Clipboard::GetAsText(std::string & text)
  {
    ClipboardObject obj( DEFAULT_READ_CLIPBOARD_HANDLE );
    if (!obj.IsOpened())
      return false;

    //The clipboard controls the handle that the GetClipboardData function returns, not the application.
    //The application should copy the data immediately. The application must not free the handle nor leave it locked.
    HANDLE hData = GetClipboardData(CF_TEXT);
    if (hData == NULL)
      return false;

    size_t data_size = (size_t)GlobalSize(hData);
    size_t count = data_size / sizeof(char);
    const char* text_buffer = (const char*)GlobalLock(hData);
    text.assign(text_buffer, count-1); //copy the data to output variable, minus the last \0 character
    GlobalUnlock( hData );

    return true;
  }

  bool Clipboard::SetTextUnicode(const std::wstring & text)
  {
    return setTextT<wchar_t>(CF_UNICODETEXT, text.data(), text.length());
  }

  bool Clipboard::GetAsTextUnicode(std::wstring & text)
  {
    ClipboardObject obj(DEFAULT_READ_CLIPBOARD_HANDLE);
    if (!obj.IsOpened())
      return false;

    //The clipboard controls the handle that the GetClipboardData function returns, not the application.
    //The application should copy the data immediately. The application must not free the handle nor leave it locked.
    HANDLE hData = GetClipboardData(CF_UNICODETEXT);
    if (hData == NULL)
      return false;

    size_t data_size = (size_t)GlobalSize(hData);
    size_t count = data_size / sizeof(wchar_t);
    const wchar_t* text_buffer = (const wchar_t*)GlobalLock(hData);
    text.assign(text_buffer, count- 1); //copy the data to output variable, minus the last \0 character
    GlobalUnlock(hData);

    return true;
  }

  bool Clipboard::SetBinary(const MemoryBuffer & buffer)
  {
    ClipboardObject obj( DEFAULT_WRITE_CLIPBOARD_HANDLE );
    if (!obj.IsOpened())
      return false;

    //flush existing content
    if (!EmptyClipboard())
      return false;

    //copy data to global allocated memory
    HGLOBAL hMem = GlobalAlloc(GMEM_DDESHARE, buffer.size());
    if (hMem == NULL)
      return false;
    void * temp_buffer = GlobalLock(hMem);
    memcpy(temp_buffer, buffer.c_str(), buffer.size());
    GlobalUnlock(hMem);

    //put it on the clipboard
    HANDLE hData = SetClipboardData(gFormatDescriptorBinary, hMem);
    if (hData != hMem)
      return false;

    return true;
  }

  bool Clipboard::GetAsBinary(MemoryBuffer & buffer)
  {
    ClipboardObject obj( DEFAULT_READ_CLIPBOARD_HANDLE );
    if (!obj.IsOpened())
      return false;

    //The clipboard controls the handle that the GetClipboardData function returns, not the application.
    //The application should copy the data immediately. The application must not free the handle nor leave it locked.
    HANDLE hData = GetClipboardData(gFormatDescriptorBinary);
    if (hData == NULL)
      return false;

    //get the buffer
    size_t data_size = (size_t)GlobalSize(hData);
    void * temp_buffer = GlobalLock( hData );
    buffer.assign((const char*)temp_buffer, data_size); //copy the data to output variable
    GlobalUnlock( hData );
    
    return true;
  }

  bool Clipboard::SetDragDropFiles(const Clipboard::DragDropType & type, const Clipboard::StringVector & files)
  {
    //http://support.microsoft.com/kb/231721/en-us
    //http://aclacl.brinkster.net/MFC/ch19b.htm
    //http://read.pudn.com/downloads22/sourcecode/windows/multimedia/73340/ShitClass3/%E7%B1%BB%E5%8C%85/SManageFile.cpp__.htm

    //Validate drag drop type
    if (type != Clipboard::DragDropCopy && type != Clipboard::DragDropCut)
      return false;

    ClipboardObject obj( DEFAULT_WRITE_CLIPBOARD_HANDLE );
    if (!obj.IsOpened())
      return false;

    //flush existing content
    if (!EmptyClipboard())
      return false;

    //Register files
    {
      DROPFILES df = {0};
      df.pFiles = sizeof(DROPFILES);
      df.pt.x = 0;
      df.pt.y = 0;
      df.fNC = FALSE;
      df.fWide = FALSE; //we don't use WIDE CHAR for storing the file paths

      //Build the buffer content
      MemoryBuffer buff;

      //append the DROPFILES structure
      buff.assign((const char *)&df, sizeof(df));

      //append each files
      for(size_t i=0; i<files.size(); i++)
      {
        const std::string & file_path = files[i];

        //append
        const char * data_buffer = (const char*)file_path.c_str();
        const size_t data_size_bytes = (file_path.size() + 1) * sizeof(file_path[0]); // +1 for including the NULL terminating character
        buff.append(data_buffer, data_size_bytes);
      }
        
      //Append final empty filepath
      {
        //Convert utf8 to unicode
        static const std::string file_path = "";

        //append
        const char * data_buffer = (const char*)file_path.c_str();
        const size_t data_size_bytes = (file_path.size() + 1) * sizeof(file_path[0]); // +1 for including the NULL terminating character
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

    //Register type
    {
      HGLOBAL hDropEffect = GlobalAlloc(GMEM_ZEROINIT|GMEM_MOVEABLE|GMEM_DDESHARE,sizeof(DWORD));
      DWORD * drop_effect = (DWORD*)GlobalLock(hDropEffect);
      if (type == Clipboard::DragDropCopy)
        (*drop_effect) = DROPEFFECT_COPY /*| DROPEFFECT_LINK*/;
      else
        (*drop_effect) = DROPEFFECT_MOVE;
      GlobalUnlock(hDropEffect);

      //put it on the clipboard
      HANDLE hData = SetClipboardData( gFormatDescriptorDropEffect, hDropEffect );
      if (hData != hDropEffect)
        return false;
    }

    return true;
  }

  bool Clipboard::GetAsDragDropFiles(DragDropType & type, Clipboard::StringVector & files)
  {
    //Invalidate
    type = Clipboard::DragDropType(-1);
    files.clear();

    ClipboardObject obj( DEFAULT_READ_CLIPBOARD_HANDLE );
    if (!obj.IsOpened())
      return false;

    //Detect if CUT or COPY
    {
      //The clipboard controls the handle that the GetClipboardData function returns, not the application.
      //The application should copy the data immediately. The application must not free the handle nor leave it locked.
      HANDLE hDropEffect = ::GetClipboardData(gFormatDescriptorDropEffect);
      if (hDropEffect)
      {
        void * buffer = GlobalLock(hDropEffect);
        SIZE_T buffer_size = GlobalSize(buffer);
        DWORD drop_effect = *((DWORD*)buffer);
        //if (drop_effect == DROPEFFECT_NONE)
        //  "NONE:"
        if (drop_effect & DROPEFFECT_COPY)
          type = DragDropCopy;
        else if (drop_effect & DROPEFFECT_MOVE)
          type = DragDropCut;
        //if (drop_effect & DROPEFFECT_LINK)
        //  "LINK:"
        //if (drop_effect & DROPEFFECT_SCROLL)
        //  "SCROLL:"
        GlobalUnlock(hDropEffect);
      }

      if (type == -1)
      {
        //unknown drop effect
        return false;
      }
    }

    //Retreive files
    HDROP hDrop = (HDROP) ::GetClipboardData(CF_HDROP);
    if (hDrop == NULL)
      return false;

    void * buffer = GlobalLock(hDrop);
    if (buffer == NULL)
      return false;

    size_t buffer_size = GlobalSize(buffer);

    // Find out how many file names the HDROP Contains.
    int count = ::DragQueryFileW(hDrop, (UINT) -1, NULL, 0);
    if (count == 0)
      return false;

    //Find out if files are unicode or ansi
    DROPFILES df = {0};
    df = *( ((DROPFILES*)buffer) );
    bool is_unicode = (df.fWide == 1);

    // Enumerate the file names.
    for (int i=0; i<count; i++)
    {
      std::string file_path;

      if (is_unicode)
      {
        WCHAR temp_path[10240];
        UINT length = ::DragQueryFileW(hDrop, i, temp_path, sizeof (temp_path) / sizeof (temp_path[0]));
                  
        std::wstring utf16_file_path = temp_path;

        //Convert from unicode to ansi
        file_path = ra::unicode::UnicodeToAnsi( utf16_file_path );
      }
      else
      {
        char temp_path[10240];
        ::DragQueryFileA(hDrop, i, temp_path, sizeof (temp_path) / sizeof (temp_path[0]));
        file_path = temp_path;
      }

      //Add to output files
      //printf("Reading from clipboard %02d/%02d: %s\n", i+1, nCount, filePath.c_str());
      files.push_back(file_path);
    }

    GlobalUnlock(hDrop);
          
    return true;
  }

  bool Clipboard::SetDragDropFilesUnicode(const Clipboard::DragDropType & type, const Clipboard::StringVectorUnicode & files)
  {
    //http://support.microsoft.com/kb/231721/en-us
    //http://aclacl.brinkster.net/MFC/ch19b.htm
    //http://read.pudn.com/downloads22/sourcecode/windows/multimedia/73340/ShitClass3/%E7%B1%BB%E5%8C%85/SManageFile.cpp__.htm

    //Validate drag drop type
    if (type != Clipboard::DragDropCopy && type != Clipboard::DragDropCut)
      return false;

    ClipboardObject obj( DEFAULT_WRITE_CLIPBOARD_HANDLE );
    if (!obj.IsOpened())
      return false;

    //flush existing content
    if (!EmptyClipboard())
      return false;

    //Register files
    {
      DROPFILES df = {0};
      df.pFiles = sizeof(DROPFILES);
      df.pt.x = 0;
      df.pt.y = 0;
      df.fNC = FALSE;
      df.fWide = TRUE; //we use WIDE CHAR for storing the file paths

      //Build the buffer content
      MemoryBuffer buff;

      //append the DROPFILES structure
      buff.assign((const char *)&df, sizeof(df));

      //append each files
      for(size_t i=0; i<files.size(); i++)
      {
        //Convert utf8 to unicode
        const std::wstring unicode_file_path = files[i];

        //append
        const char * data_buffer = (const char*)unicode_file_path.c_str();
        const size_t data_size_bytes = (unicode_file_path.size() + 1) * sizeof(unicode_file_path[0]); // +1 for including the NULL terminating character
        buff.append(data_buffer, data_size_bytes);
      }
        
      //Append final empty filepath
      {
        //Convert utf8 to unicode
        static const std::wstring unicode_file_path = ra::unicode::Utf8ToUnicode("");

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

    //Register type
    {
      HGLOBAL hDropEffect = GlobalAlloc(GMEM_ZEROINIT|GMEM_MOVEABLE|GMEM_DDESHARE,sizeof(DWORD));
      DWORD * drop_effect = (DWORD*)GlobalLock(hDropEffect);
      if (type == Clipboard::DragDropCopy)
        (*drop_effect) = DROPEFFECT_COPY /*| DROPEFFECT_LINK*/;
      else
        (*drop_effect) = DROPEFFECT_MOVE;
      GlobalUnlock(hDropEffect);

      //put it on the clipboard
      HANDLE hData = SetClipboardData( gFormatDescriptorDropEffect, hDropEffect );
      if (hData != hDropEffect)
        return false;
    }

    return true;
  }

  bool Clipboard::GetAsDragDropFilesUnicode(DragDropType & type, Clipboard::StringVectorUnicode & files)
  {
    //Invalidate
    type = Clipboard::DragDropType(-1);
    files.clear();

    ClipboardObject obj( DEFAULT_READ_CLIPBOARD_HANDLE );
    if (!obj.IsOpened())
      return false;

    //Detect if CUT or COPY
    {
      //The clipboard controls the handle that the GetClipboardData function returns, not the application.
      //The application should copy the data immediately. The application must not free the handle nor leave it locked.
      HANDLE hDropEffect = ::GetClipboardData(gFormatDescriptorDropEffect);
      if (hDropEffect)
      {
        void * buffer = GlobalLock(hDropEffect);
        SIZE_T buffer_size = GlobalSize(buffer);
        DWORD drop_effect = *((DWORD*)buffer);
        //if (drop_effect == DROPEFFECT_NONE)
        //  "NONE:"
        if (drop_effect & DROPEFFECT_COPY)
          type = DragDropCopy;
        else if (drop_effect & DROPEFFECT_MOVE)
          type = DragDropCut;
        //if (drop_effect & DROPEFFECT_LINK)
        //  "LINK:"
        //if (drop_effect & DROPEFFECT_SCROLL)
        //  "SCROLL:"
        GlobalUnlock(hDropEffect);
      }

      if (type == -1)
      {
        //unknown drop effect
        return false;
      }
    }

    //Retreive files
    HDROP hDrop = (HDROP) ::GetClipboardData(CF_HDROP);
    if (hDrop == NULL)
      return false;

    void * buffer = GlobalLock(hDrop);
    if (buffer == NULL)
      return false;

    size_t buffer_size = GlobalSize(buffer);

    // Find out how many file names the HDROP Contains.
    int count = ::DragQueryFileW(hDrop, (UINT) -1, NULL, 0);
    if (count == 0)
      return false;

    //Find out if files are unicode or ansi
    DROPFILES df = {0};
    df = *( ((DROPFILES*)buffer) );
    bool is_unicode = (df.fWide == 1);

    // Enumerate the file names.
    for (int i=0; i<count; i++)
    {
      std::wstring file_path;

      if (is_unicode)
      {
        WCHAR temp_path[10240];
        UINT length = ::DragQueryFileW(hDrop, i, temp_path, sizeof (temp_path) / sizeof (temp_path[0]));
                  
        std::wstring utf16_file_path = temp_path;

        file_path = temp_path;
      }
      else
      {
        char temp_path[10240];
        ::DragQueryFileA(hDrop, i, temp_path, sizeof (temp_path) / sizeof (temp_path[0]));

        std::string file_path_ansi = temp_path;

        //Convert from unicode to ansi
        file_path = ra::unicode::AnsiToUnicode(file_path_ansi);
      }

      //Add to output files
      //printf("Reading from clipboard %02d/%02d: %s\n", i+1, nCount, filePath.c_str());
      files.push_back(file_path);
    }

    GlobalUnlock(hDrop);
          
    return true;
  }

} //namespace Win32Clipboard
