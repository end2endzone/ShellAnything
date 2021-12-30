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

 namespace shellanything
{

  /// <summary>
  /// A structure that contains a list of Configuration class pointers.
  /// </summary>
  class String
  {
  public:
    String();
    String(const char* str);
    String(const String& str);
    virtual ~String();

    static const size_t INVALID_INDEX = (size_t)-1;

    String& operator=(const char* str);
    String& operator=(const String& str);
         char & operator[](size_t index);
    const char& operator[](size_t index) const;
    bool operator==(const char* str) const;
    bool operator==(const String& str) const;
    bool operator!=(const char* str) const;
    bool operator!=(const String& str) const;
    String& operator+=(const char* str);
    String& operator+=(const String& str);

    /// <summary>
    /// Get the internal value of the string.
    /// </summary>
    const char* GetValue() const;
    inline const char * c_str() const { return GetValue(); } // for std::vector and std::list code compatibility.

    /// <summary>
    /// Define if the string is empty.
    /// </summary>
    bool IsEmpty() const;
    inline bool empty() const { return IsEmpty(); } // for std::vector and std::list code compatibility.

    /// <summary>
    /// Get the length of the string.
    /// </summary>
    size_t GetSize() const;
    inline size_t size() const { return GetSize(); } // for std::vector and std::list code compatibility.

    /// <summary>
    /// Empty the string.
    /// </summary>
    void Clear();
    inline void clear() { Clear(); } // for std::vector and std::list code compatibility.

    /// <summary>
    /// Find a value in the list.
    /// </summary>
    /// <param name="value">A value to find within the string.</param>
    /// <returns>Returns the position where the search value was found. Returns INVALID_INDEX otherwise.</returns>
    size_t Find(const char value) const;
    size_t Find(const char * value) const;
    size_t Find(const String & value) const;
    inline size_t find(const char value) const    { return Find(value); } // for std::vector and std::list code compatibility.
    inline size_t find(const char* value) const   { return Find(value); } // for std::vector and std::list code compatibility.
    inline size_t find(const String& value) const { return Find(value); } // for std::vector and std::list code compatibility.

    /// <summary>
    /// Find a value in the list, starting at the given offset.
    /// </summary>
    /// <param name="value">A value to find within the string.</param>
    /// <param name="offset">Position at which the search must start.</param>
    /// <returns>Returns the position where the search value was found. Returns INVALID_INDEX otherwise.</returns>
    size_t Find(const char value, size_t offset) const;
    size_t Find(const char * value, size_t offset) const;
    size_t Find(const String & value, size_t offset) const;
    inline size_t find(const char value, size_t offset) const    { return Find(value, offset); } // for std::vector and std::list code compatibility.
    inline size_t find(const char* value, size_t offset) const   { return Find(value, offset); } // for std::vector and std::list code compatibility.
    inline size_t find(const String& value, size_t offset) const { return Find(value, offset); } // for std::vector and std::list code compatibility.

  private:
    struct PImpl;
    PImpl* mPImpl;
  };

} //namespace shellanything
