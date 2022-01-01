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

#include "shellanything/String.h"
#include <string>

namespace shellanything
{

  struct String::PImpl
  {
    std::string str;
  };

  String::String()
  {
    mPImpl = new String::PImpl();
  }

  String::String(const char* str)
  {
    mPImpl = new String::PImpl();
    mPImpl->str = str;
  }

  String::String(const char* str, const size_t count)
  {
    mPImpl = new String::PImpl();
    mPImpl->str.append(str, count);
  }

  String::String(const String& str)
  {
    mPImpl = new String::PImpl();
    mPImpl->str = str.mPImpl->str;
  }

  String::~String()
  {
    delete mPImpl;
  }

  String& String::operator=(const char* str)
  {
    mPImpl->str = str;
    return (*this);
  }

  String& String::operator=(const String& str)
  {
    mPImpl->str = str.mPImpl->str;
    return (*this);
  }

  char& String::operator[](size_t index)
  {
    return mPImpl->str[index];
  }

  const char& String::operator[](size_t index) const
  {
    return mPImpl->str[index];
  }

  bool String::operator==(const char* str) const
  {
    return mPImpl->str == str;
  }

  bool String::operator==(const String& str) const
  {
    return mPImpl->str == str.mPImpl->str;
  }

  bool String::operator!=(const char* str) const
  {
    return mPImpl->str != str;
  }

  bool String::operator!=(const String& str) const
  {
    return mPImpl->str != str.mPImpl->str;
  }

  String& String::operator+=(const char* str)
  {
    mPImpl->str += str;
    return (*this);
  }

  String& String::operator+=(const String& str)
  {
    mPImpl->str += str.mPImpl->str;
    return (*this);
  }

  String String::operator+(const char* str)
  {
    String temp = (*this);
    temp += str;
    return temp;
  }

  String String::operator+(const String& str)
  {
    String temp = (*this);
    temp += str;
    return temp;
  }

  bool String::operator<  (const char* str) const
  {
    return mPImpl->str < str;
  }

  bool String::operator<  (const String& str) const
  {
    return mPImpl->str < str.mPImpl->str;
  }

  bool String::operator<= (const char* str) const
  {
    return mPImpl->str <= str;
  }

  bool String::operator<= (const String& str) const
  {
    return mPImpl->str <= str.mPImpl->str;
  }

  bool String::operator>  (const char* str) const
  {
    return mPImpl->str > str;
  }

  bool String::operator>  (const String& str) const
  {
    return mPImpl->str > str.mPImpl->str;
  }

  bool String::operator>= (const char* str) const
  {
    return mPImpl->str >= str;
  }

  bool String::operator>= (const String& str) const
  {
    return mPImpl->str >= str.mPImpl->str;
  }

  const char* String::GetValue() const
  {
    return mPImpl->str.c_str();
  }

  bool String::IsEmpty() const
  {
    return mPImpl->str.empty();
  }

  size_t String::GetSize() const
  {
    return mPImpl->str.size();
  }

  void String::Clear()
  {
    return mPImpl->str.clear();
  }

  void String::Reserve(const size_t size)
  {
    mPImpl->str.reserve(size);
  }

  size_t String::Find(const char value) const
  {
    return mPImpl->str.find(value);
  }

  size_t String::Find(const char* value) const
  {
    return mPImpl->str.find(value);
  }

  size_t String::Find(const String& value) const
  {
    return mPImpl->str.find(value.mPImpl->str);
  }

  size_t String::Find(const char value, size_t offset) const
  {
    return mPImpl->str.find(value, offset);
  }

  size_t String::Find(const char* value, size_t offset) const
  {
    return mPImpl->str.find(value, offset);
  }

  size_t String::Find(const String& value, size_t offset) const
  {
    return mPImpl->str.find(value.mPImpl->str, offset);
  }

  void String::Append(const char value)
  {
    mPImpl->str.append(1, value);
  }

  void String::Append(const char* value)
  {
    mPImpl->str.append(value);
  }

  void String::Append(const char* value, const size_t count)
  {
    mPImpl->str.append(value, count);
  }

  void String::Append(const size_t count, const char value)
  {
    mPImpl->str.append(count, value);
  }

  void String::Append(const String& value)
  {
    mPImpl->str.append(value.mPImpl->str);
  }

} //namespace shellanything

shellanything::String operator+(const shellanything::String& s1, const shellanything::String& s2)
{
  shellanything::String temp = (s1);
  temp += s2;
  return temp;
}

shellanything::String operator+(const char* s1, const shellanything::String& s2)
{
  shellanything::String temp = (s1);
  temp += s2;
  return temp;
}

shellanything::String operator+(const shellanything::String& s1, const char* s2)
{
  shellanything::String temp = (s1);
  temp += s2;
  return temp;
}

//bool operator<  (const char* s1, const shellanything::String& s2)
//{
//  return shellanything::String(s1) < s2;
//}
//
//bool operator<= (const char* s1, const shellanything::String& s2)
//{
//  return shellanything::String(s1) <= s2;
//}
//
//bool operator>  (const char* s1, const shellanything::String& s2)
//{
//  return shellanything::String(s1) > s2;
//}
//
//bool operator>= (const char* s1, const shellanything::String& s2)
//{
//  return shellanything::String(s1) >= s2;
//}


/// <summary>
/// A list of String.
/// </summary>
#define SA_LISTS_CLASS_NAME StringList
#define SA_LISTS_BASE_TYPE  String
#include "shellanything/ListsBody.inc"
#undef SA_LISTS_BASE_TYPE
#undef SA_LISTS_CLASS_NAME
