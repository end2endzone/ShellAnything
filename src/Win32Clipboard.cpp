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

  std::wstring getLastErrorDescriptionUtf8()
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
          std::wstring error = getLastErrorDescriptionUtf8();

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
    case Clipboard::FormatUtf8:
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

  bool Clipboard::SetTextUtf8(const std::string & text)
  {
    std::wstring text_unicode = ra::unicode::Utf8ToUnicode(text);
    return setTextT<wchar_t>(CF_UNICODETEXT, text_unicode.data(), text_unicode.length());
  }

  template <typename T> inline bool getAsTextT(UINT format, std::basic_string<T, std::char_traits<T>, std::allocator<T>> & str)
  {
    ClipboardObject obj(DEFAULT_READ_CLIPBOARD_HANDLE);
    if (!obj.IsOpened())
      return false;

    //The clipboard controls the handle that the GetClipboardData function returns, not the application.
    //The application should copy the data immediately. The application must not free the handle nor leave it locked.
    HANDLE hData = GetClipboardData(format);
    if (hData == NULL)
      return false;

    size_t data_size = (size_t)GlobalSize(hData);
    size_t count = data_size / sizeof(T);
    const T* text_buffer = (const T*)GlobalLock(hData);
    str.assign(text_buffer, count - 1); //copy the data to output variable, minus the last \0 character
    GlobalUnlock(hData);

    return true;
  }

  bool Clipboard::GetAsText(std::string & text)
  {
    bool result = getAsTextT<char>(CF_TEXT, text);
    return result;
  }

  bool Clipboard::GetAsTextUtf8(std::string & text)
  {
    std::wstring unicode_text;
    bool result = getAsTextT<wchar_t>(CF_UNICODETEXT, unicode_text);
    if (!result)
      return false;
    text = ra::unicode::UnicodeToUtf8(unicode_text);
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

  template <typename T> inline bool setDragDropFilesT(UINT format, const Clipboard::DragDropType & type, const std::vector<std::basic_string<T, std::char_traits<T>, std::allocator<T>>> & files)
  {
    //http://support.microsoft.com/kb/231721/en-us
    //http://aclacl.brinkster.net/MFC/ch19b.htm
    //http://read.pudn.com/downloads22/sourcecode/windows/multimedia/73340/ShitClass3/%E7%B1%BB%E5%8C%85/SManageFile.cpp__.htm

    //Validate drag drop type
    if (type != Clipboard::DragDropCopy && type != Clipboard::DragDropCut)
      return false;

    ClipboardObject obj(DEFAULT_WRITE_CLIPBOARD_HANDLE);
    if (!obj.IsOpened())
      return false;

    //flush existing content
    if (!EmptyClipboard())
      return false;

    //Register files
    {
      DROPFILES df = { 0 };
      df.pFiles = sizeof(DROPFILES);
      df.pt.x = 0;
      df.pt.y = 0;
      df.fNC = FALSE;
      df.fWide = (sizeof(T) > 1); //we use WIDE CHAR for storing the file paths for utf8 files

      //Build the buffer content
      Clipboard::MemoryBuffer buff;

      //append the DROPFILES structure
      buff.assign((const char *)&df, sizeof(df));

      //append each files
      for (size_t i = 0; i < files.size(); i++)
      {
        const std::basic_string<T, std::char_traits<T>, std::allocator<T>> & file_path = files[i];

        //append
        const char * data_buffer = (const char*)file_path.c_str();
        const size_t data_size_bytes = (file_path.size() + 1) * sizeof(file_path[0]); // +1 for including the NULL terminating character
        buff.append(data_buffer, data_size_bytes);
      }

      //Append final empty file path
      {
        //Convert utf8 to unicode
        static const std::basic_string<T, std::char_traits<T>, std::allocator<T>> EMPTY_STRING;

        //append
        const char * data_buffer = (const char*)EMPTY_STRING.c_str();
        const size_t data_size_bytes = (EMPTY_STRING.size() + 1) * sizeof(EMPTY_STRING[0]); // +1 for including the NULL terminating character
        buff.append(data_buffer, data_size_bytes);
      }

      //copy data to global allocated memory
      HGLOBAL hMem = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE, buff.size());
      if (hMem == NULL)
        return false;
      void * buffer = GlobalLock(hMem);
      memcpy(buffer, buff.c_str(), buff.size());
      GlobalUnlock(hMem);

      //put it on the clipboard
      HANDLE hData = SetClipboardData(CF_HDROP, hMem);
      if (hData != hMem)
        return false;
    }

    //Register type
    {
      HGLOBAL hDropEffect = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE, sizeof(DWORD));
      DWORD * drop_effect = (DWORD*)GlobalLock(hDropEffect);
      if (type == Clipboard::DragDropCopy)
        (*drop_effect) = DROPEFFECT_COPY /*| DROPEFFECT_LINK*/;
      else
        (*drop_effect) = DROPEFFECT_MOVE;
      GlobalUnlock(hDropEffect);

      //put it on the clipboard
      HANDLE hData = SetClipboardData(gFormatDescriptorDropEffect, hDropEffect);
      if (hData != hDropEffect)
        return false;
    }

    return true;
  }

  bool Clipboard::SetDragDropFiles(const Clipboard::DragDropType & type, const Clipboard::StringVector & files)
  {
    bool result = setDragDropFilesT<char>(CF_TEXT, type, files);
    return result;
  }

  bool Clipboard::SetDragDropFilesUtf8(const Clipboard::DragDropType & type, const Clipboard::StringVector & files)
  {
    // Convert files encoded in utf8 to unicode
    WideStringVector files_unicode;
    for (size_t i = 0; i < files.size(); i++)
    {
      const std::string & path_utf8 = files[i];
      std::wstring path_unicode = ra::unicode::Utf8ToUnicode(path_utf8);
      files_unicode.push_back(path_unicode);
    }

    bool result = setDragDropFilesT<wchar_t>(CF_UNICODETEXT, type, files_unicode);
    return result;
  }

  void decode_string(const char * source, std::string & dest)
  {
    dest = source; // as is
  }
  void decode_string(const char * source, std::wstring & dest)
  {
    dest = ra::unicode::AnsiToUnicode(source);
  }
  void decode_string(const wchar_t * source, std::string & dest)
  {
    dest = ra::unicode::UnicodeToAnsi(source);
  }
  void decode_string(const wchar_t * source, std::wstring & dest)
  {
    dest = source; // as is
  }

  template <typename T> inline bool getAsDragDropFilesT(UINT format, Clipboard::DragDropType & type, std::vector<std::basic_string<T, std::char_traits<T>, std::allocator<T>>> & files)
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
          type = Clipboard::DragDropCopy;
        else if (drop_effect & DROPEFFECT_MOVE)
          type = Clipboard::DragDropCut;
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

    // Find out how many files the HDROP contains.
    int count = ::DragQueryFileW(hDrop, (UINT) -1, NULL, 0);
    if (count == 0)
      return true; // Empty.

    //Find out if files are unicode or ansi
    DROPFILES df = {0};
    df = *( ((DROPFILES*)buffer) );
    bool is_unicode = (df.fWide == 1);

    // Enumerate the file names.
    for (int i=0; i<count; i++)
    {
      std::basic_string<T, std::char_traits<T>, std::allocator<T>> file_path;

      if (is_unicode)
      {
        wchar_t temp_path[10240];
        UINT length = ::DragQueryFileW(hDrop, i, temp_path, sizeof (temp_path) / sizeof (temp_path[0]));
        decode_string(temp_path, file_path);
      }
      else
      {
        char temp_path[10240];
        ::DragQueryFileA(hDrop, i, temp_path, sizeof (temp_path) / sizeof (temp_path[0]));
        decode_string(temp_path, file_path);
      }

      //Add to output files
      //printf("Reading from clipboard %02d/%02d: %s\n", i+1, nCount, filePath.c_str());
      files.push_back(file_path);
    }

    GlobalUnlock(hDrop);
          
    return true;
  }

  bool Clipboard::GetAsDragDropFiles(DragDropType & type, Clipboard::StringVector & files)
  {
    bool result = getAsDragDropFilesT<char>(CF_TEXT, type, files);
    return result;
  }

  bool Clipboard::GetAsDragDropFilesUtf8(DragDropType & type, Clipboard::StringVector & files)
  {
    WideStringVector files_unicode;
    bool result = getAsDragDropFilesT<wchar_t>(CF_TEXT, type, files_unicode);
    if (!result)
      return false;

    // Convert files encoded in unicode to utf8
    files.clear();
    for (size_t i = 0; i < files_unicode.size(); i++)
    {
      const std::wstring & path_unicode = files_unicode[i];
      std::string path_utf8 = ra::unicode::UnicodeToUtf8(path_unicode);
      files.push_back(path_utf8);
    }

    return true;
  }

} //namespace Win32Clipboard
