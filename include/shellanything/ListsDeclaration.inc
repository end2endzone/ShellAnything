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
  class SA_LISTS_CLASS_NAME
  {
  public:
    SA_LISTS_CLASS_NAME();
    virtual ~SA_LISTS_CLASS_NAME();

    static const size_t INVALID_INDEX = (size_t)-1;

    SA_LISTS_CLASS_NAME(const SA_LISTS_CLASS_NAME& listcopy);
    SA_LISTS_CLASS_NAME& operator=(const SA_LISTS_CLASS_NAME& listcopy);

    /// <summary>
    /// A custom type definition for the base type
    /// </summary>
    typedef SA_LISTS_BASE_TYPE Element;

    /// <summary>
    /// Define if the list is empty.
    /// </summary>
    bool IsEmpty() const;
    inline bool empty() const { return IsEmpty(); } // for std::vector and std::list code compatibility.

    /// <summary>
    /// Get the number of element in the list.
    /// </summary>
    size_t GetSize() const;
    inline size_t size() const { return GetSize(); } // for std::vector and std::list code compatibility.

    /// <summary>
    /// Remove all elements in the list.
    /// </summary>
    void Clear();
    inline void clear() { Clear(); } // for std::vector and std::list code compatibility.

    /// <summary>
    /// Get an element from an index.
    /// </summary>
    /// <param name="index">The index of the requested instance.</param>
    /// <returns>Returns a valid element if the index is valid. Returns an invalid element otherwise.</returns>
    Element & GetElement(size_t index);
    const Element & GetElement(size_t index) const;
    inline       Element & operator[](size_t index)       { return GetElement(index); }
    inline const Element & operator[](size_t index) const { return GetElement(index); }

    /// <summary>
    /// Find an element in the list.
    /// </summary>
    /// <param name="value">A value to find.</param>
    /// <returns>Returns true when the element is found. Returns false otherwise.</returns>
    size_t FindElement(const Element & value) const;

    /// <summary>
    /// Add a value to the list.
    /// </summary>
    /// <param name="value">The value to add.</param>
    void AddElement(Element value);

    /// <summary>
    /// Remove a value from the list.
    /// </summary>
    /// <param name="value">The value to remove.</param>
    void RemoveElementByValue(Element value);

    /// <summary>
    /// Remove a value from the list.
    /// </summary>
    /// <param name="index">The index of the value to remove.</param>
    void RemoveElementByIndex(size_t index);

  private:
    struct PImpl;
    PImpl* mPImpl;
  };

} //namespace shellanything
