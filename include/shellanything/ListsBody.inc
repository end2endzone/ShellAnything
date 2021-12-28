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

  struct SA_LISTS_CLASS_NAME::PImpl
  {
    std::vector<SA_LISTS_CLASS_NAME::Element> elements;
  };

  SA_LISTS_CLASS_NAME::SA_LISTS_CLASS_NAME()
  {
    mPImpl = new SA_LISTS_CLASS_NAME::PImpl();
  }

  SA_LISTS_CLASS_NAME::~SA_LISTS_CLASS_NAME()
  {
    delete mPImpl;
  }

  SA_LISTS_CLASS_NAME::SA_LISTS_CLASS_NAME(const SA_LISTS_CLASS_NAME& listcopy)
  {
    mPImpl = new SA_LISTS_CLASS_NAME::PImpl();
    (*this) = listcopy;
  }

  SA_LISTS_CLASS_NAME& SA_LISTS_CLASS_NAME::operator=(const SA_LISTS_CLASS_NAME& listcopy)
  {
    mPImpl->elements = listcopy.mPImpl->elements;
    return (*this);
  }

  size_t SA_LISTS_CLASS_NAME::GetSize() const
  {
    return mPImpl->elements.size();
  }

  void SA_LISTS_CLASS_NAME::Clear()
  {
    return mPImpl->elements.clear();
  }

  SA_LISTS_CLASS_NAME::Element & SA_LISTS_CLASS_NAME::GetElement(size_t index)
  {
    return mPImpl->elements[index];
  }

  const SA_LISTS_CLASS_NAME::Element & SA_LISTS_CLASS_NAME::GetElement(size_t index) const
  {
    return mPImpl->elements[index];
  }

  size_t SA_LISTS_CLASS_NAME::FindElement(const SA_LISTS_CLASS_NAME::Element & value) const
  {
    //std::vector<SA_LISTS_CLASS_NAME::Element>::iterator it;
    //it = std::find(mPImpl->elements.begin(), mPImpl->elements.end(), value);
    //bool found = (it != mPImpl->elements.end());
    //return found;

    std::vector<SA_LISTS_CLASS_NAME::Element>& v = mPImpl->elements;
    for (size_t i = 0; i < v.size(); i++)
    {
      const SA_LISTS_CLASS_NAME::Element & element = v[i];
      if (element == value)
        return i;
    }

    return INVALID_INDEX;
  }

  void SA_LISTS_CLASS_NAME::AddElement(SA_LISTS_CLASS_NAME::Element instance)
  {
    return mPImpl->elements.push_back(instance);
  }

  void SA_LISTS_CLASS_NAME::RemoveElementByValue(SA_LISTS_CLASS_NAME::Element value)
  {
    size_t index = FindElement(value);
    if (index != INVALID_INDEX)
      RemoveElementByIndex(index);
  }

  void SA_LISTS_CLASS_NAME::RemoveElementByIndex(size_t index)
  {
    if (index != INVALID_INDEX)
      mPImpl->elements.erase(mPImpl->elements.begin() + index);
  }

} //namespace shellanything
