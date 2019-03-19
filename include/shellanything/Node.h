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
    size_t getNumChildren() const;
    NodePtrList findChildren(const std::string & type) const;
    Node * findFirst(const std::string & type) const;
    Node * getChild(size_t index) const;

    bool removeChild(size_t index);
    bool removeChild(Node * child);
    bool removeChildren();
    bool removeChildren(const std::string & type);

    size_t depth() const;
    bool isLeaf() const;
    bool isRoot() const;
    size_t size() const;

protected:
    std::string mNodeType;
    Node * mParent;
    NodePtrList mChildren;
  };

  //utility fonctions for converting Node::NodePtrList to a vector of type T
  template <typename T>
  std::vector<T> filterNodes(Node::NodePtrList & nodes)
  {
    std::vector<T> output;
    for(size_t i=0; i<nodes.size(); i++)
    {
      Node * node = nodes[i];
      T out = dynamic_cast<T>(node);
      if (out)
        output.push_back(out);
    }
    return output;
  }

} //namespace shellanything

#endif //SA_NODE_H
