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
  /// <summary>
  /// A Node class for defining an object hierarchy
  /// </summary>
  class Node
  {
  public:
    /// <summary>
    /// A list of Node pointer
    /// </summary>
    typedef std::vector<Node*> NodePtrList;

    Node();

    /// <summary>
    /// Constructor for create a new Node instance with a 'type' node-type.
    /// </summary>
    Node(const std::string & type);
    virtual ~Node();

  private:
    // Disable copy constructor and copy operator
    Node(const Node&);
    Node& operator=(const Node&);
  public:

    /// <summary>
    /// Getter for the 'node-type' parameter.
    /// </summary>
    const std::string & getNodeType() const;

    /// <summary>
    /// Get the parent of this node.
    /// </summary>
    /// <returns>Returns a valid Node pointer if the node have a parent. Returns NULL otherwise.</returns>
    Node * getParent() const;

    /// <summary>
    /// Add a new subnode to this node. The node takes ownership of the given child node.
    /// The given node instance must not already be the child of another node.
    /// </summary>
    /// <param name="child">A node instance to add as a subnode.</param>
    /// <returns>Returns a valid Node pointer if the node was inserted as a child node. Returns NULL otherwise.</returns>
    Node * addChild(Node * child);

    /// <summary>
    /// Getter list of all subnode of this node.
    /// </summary>
    NodePtrList getChildren() const;

    /// <summary>
    /// Returns the number of child this node have.
    /// </summary>
    size_t getNumChildren() const;

    /// <summary>
    /// Searches the subnodes for a Node with a node-type 'type'.
    /// </summary>
    /// <param name="type">The given type of node to search for.</param>
    /// <returns>Returns a list of Node pointer that matches the given node type.</returns>
    NodePtrList findChildren(const std::string & type) const;

    /// <summary>
    /// Searches for the first node with a node-type 'type'.
    /// </summary>
    /// <param name="type">The given type of node to search for.</param>
    /// <returns>Returns the first Node pointer that matches the given node type.</returns>
    Node * findFirst(const std::string & type) const;

    /// <summary>
    /// Returns the nth subnode of this node.
    /// </summary>
    /// <param name="index">The given index of the child node.</param>
    /// <returns>Returns the node pointer that matches the nth subnode of this node. Returns NULL if index is invalid or out of bounds.</returns>
    Node * getChild(size_t index) const;

    /// <summary>
    /// Deletes the nth subnode of this node.
    /// </summary>
    /// <param name="index">The given index of the child node.</param>
    /// <returns>Returns true if the subnode was deleted. Returns false otherwise.</returns>
    bool removeChild(size_t index);

    /// <summary>
    /// Deletes the subnode matching the given node.
    /// </summary>
    /// <param name="child">The given node to delete.</param>
    /// <returns>Returns true if the subnode was deleted. Returns false otherwise.</returns>
    bool removeChild(Node * child);

    /// <summary>
    /// Deletes all subnode of the node.
    /// </summary>
    /// <returns>Returns true if all the subnode were deleted. Returns false otherwise.</returns>
    bool removeChildren();

    /// <summary>
    /// Deletes all subnode of the node that are of type 'type'.
    /// </summary>
    /// <returns>Returns true if subnodes were deleted. Returns false otherwise.</returns>
    bool removeChildren(const std::string & type);

    /// <summary>
    /// Returns the depth of this node based on the root node. The root node have a depth of 0.
    /// </summary>
    /// <returns>Returns the depth of this node based on the root node.</returns>
    size_t depth() const;

    /// <summary>
    /// Returns true if this node have no subnodes.
    /// </summary>
    /// <returns>Returns true if this node have no subnodes. Returns false otherwise.</returns>
    bool isLeaf() const;

    /// <summary>
    /// Returns true if this node have no parent node.
    /// </summary>
    /// <returns>Returns true if this node have no parent node. Returns false otherwise.</returns>
    bool isRoot() const;

    /// <summary>
    /// Returns the number of nodes (including itself) in this node's hierarchy.
    /// A leaf node have a size() of 1.
    /// </summary>
    /// <returns>Returns the number of nodes (including itself) in this node's hierarchy.</returns>
    size_t size() const;

protected:
    std::string mNodeType;
    Node * mParent;
    NodePtrList mChildren;
  };

  /// <summary>
  /// Utility fonctions for converting Node::NodePtrList to a vector of type T
  /// </summary>
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
