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

#include "shellanything/Node.h"

namespace shellanything
{

  Node::Node() :
    mParent(NULL)
  {
  }

  Node::Node(const std::string & name) :
    mParent(NULL)
  {
    mName = name;
  }

  Node::~Node()
  {
    //delete children
    for(size_t i=0; i<mChildren.size(); i++) 
    {
      Node * node = mChildren[i];
      if (node)
        delete node;
    }
    mChildren.clear();
  }

  const std::string & Node::getName() const
  {
    return mName;
  }

  Node * Node::getParent() const
  {
    return mParent;
  }

  Node * Node::addChild(Node * child)
  {
    if (child->mParent != NULL)
    {
      throw std::runtime_error("Node already in another tree.");
    }

    child->mParent = this;
    mChildren.push_back(child);

    return child;
  }

  Node::NodePtrList Node::getChildren() const
  {
    return mChildren;
  }

  Node::NodePtrList Node::findChildren(const std::string & name) const
  {
    Node::NodePtrList nodes;
    for(size_t i=0; i<mChildren.size(); i++) 
    {
      Node * n = mChildren[i];
      if (n->mName == name)
        nodes.push_back(n);
    }
    return nodes;
  }

  Node * Node::findFirst(const std::string & name) const
  {
    for(size_t i=0; i<mChildren.size(); i++) 
    {
      Node * n = mChildren[i];
      if (n->mName == name)
        return n;
    }
    return NULL;
  }

  size_t Node::getNumChildren() const
  {
    return mChildren.size();
  }

  Node * Node::getChild(size_t index) const
  {
    if (index < mChildren.size())
    {
      Node * node = mChildren[index];
      return node;
    }
    return NULL;
  }

  size_t Node::depth() const
  {
    size_t depth = 0;
    Node * ancestor = mParent;
    for (depth = 0; ancestor != NULL; depth++)
    {
      ancestor = ancestor->mParent;
    }
    return depth;
  }

  bool Node::isLeaf() const
  {
    return (mChildren.size() == 0);
  }

  bool Node::isRoot() const
  {
    return (mParent == NULL);
  }

} //namespace shellanything
