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

#ifndef SA_NODE_H
#define SA_NODE_H

#include <string>
#include <vector>

namespace shellanything
{

  class Node
  {
  public:
    typedef std::vector<Node*> NodePtrList;

    Node();
    Node(const std::string & type);
    virtual ~Node();

    const std::string & getNodeType() const;

    Node * getParent() const;

    Node * addChild(Node * child);

    NodePtrList getChildren() const;
    NodePtrList findChildren(const std::string & type) const;
    Node * findFirst(const std::string & type) const;

    size_t getNumChildren() const;
    Node * getChild(size_t index) const;

    size_t depth() const;
    bool isLeaf() const;
    bool isRoot() const;

protected:
    std::string mNodeType;
    Node * mParent;
    NodePtrList mChildren;
  };

} //namespace shellanything

#endif //SA_NODE_H
