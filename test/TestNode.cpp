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

#include "TestNode.h"
#include "shellanything/Node.h"

namespace shellanything { namespace test
{

  //--------------------------------------------------------------------------------------------------
  void TestNode::SetUp()
  {
  }
  //--------------------------------------------------------------------------------------------------
  void TestNode::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestNode, testGetParent)
  {
    Node root("root");
    Node * child1 = (new Node("child1"));

    ASSERT_TRUE( root.getParent() == NULL );

    root.addChild(child1);

    ASSERT_TRUE(    root.getParent() == NULL );
    ASSERT_TRUE( child1->getParent() != NULL );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestNode, testGetChildren)
  {
    Node root("root");
    Node * child1 = (new Node("child1"));
    Node * child2 = (new Node("child2"));

    //no children yet
    ASSERT_EQ(0, root.getChildren().size());

    root.addChild(child1);
    root.addChild(child2);

    //assert 2 children
    ASSERT_EQ(2, root.getChildren().size());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestNode, testFindChildren)
  {
    Node root("html");
    Node * body = (new Node("body"));
    Node * child1 = (new Node("h1"));
    Node * child2 = (new Node("p"));
    Node * child3 = (new Node("p"));

    //no children yet
    ASSERT_EQ(0, root.findChildren("").size());

    //build tree
    root.addChild(body);
    body->addChild(child1);
    body->addChild(child2);
    body->addChild(child3);

    //search for a node that is not found
    ASSERT_EQ(0, body->findChildren("a").size());

    //search for multiple nodes
    ASSERT_EQ(2, body->findChildren("p").size());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestNode, testFindFirst)
  {
    Node root("html");
    Node * body = (new Node("body"));
    Node * child1 = (new Node("h1"));
    Node * child2 = (new Node("p"));
    Node * child3 = (new Node("p"));

    //no children yet
    ASSERT_TRUE( root.findFirst("") == NULL );

    //build tree
    root.addChild(body);
    body->addChild(child1);
    body->addChild(child2);
    body->addChild(child3);

    //search for a node that is not found
    ASSERT_EQ( NULL, body->findFirst("a") );

    //search for multiple nodes
    ASSERT_EQ(child2, body->findFirst("p") );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestNode, testGetNumChildren)
  {
    Node root("root");
    Node * child1 = (new Node("child1"));
    Node * child2 = (new Node("child2"));

    //no children yet
    ASSERT_EQ(0, root.getNumChildren());

    root.addChild(child1);
    root.addChild(child2);

    //assert 2 children
    ASSERT_EQ(2, root.getNumChildren());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestNode, testGetChild)
  {
    Node root("html");
    Node * body = (new Node("body"));
    Node * child1 = (new Node("h1"));
    Node * child2 = (new Node("p"));
    Node * child3 = (new Node("p"));

    //no children yet
    ASSERT_TRUE( root.getChild(0) == NULL );

    //build tree
    root.addChild(body);
    body->addChild(child1);
    body->addChild(child2);
    body->addChild(child3);

    //search for a node that is not found
    ASSERT_EQ( NULL, body->getChild(999) );

    //search specific nodes
    ASSERT_EQ(child1, body->getChild(0) );
    ASSERT_EQ(child2, body->getChild(1) );
    ASSERT_EQ(child3, body->getChild(2) );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestNode, testDepth)
  {
    Node root("html");
    Node * body = (new Node("body"));
    Node * child1 = (new Node("h1"));
    Node * child2 = (new Node("p"));
    Node * child3 = (new Node("p"));

    //no children yet
    ASSERT_EQ( 0, root.depth() );

    //build tree
    root.addChild(body);
    body->addChild(child1);
    body->addChild(child2);
    body->addChild(child3);

    //assert
    ASSERT_EQ( 0, root.depth() );
    ASSERT_EQ( 1, body->depth() );
    ASSERT_EQ( 2, child1->depth() );
    ASSERT_EQ( 2, child2->depth() );
    ASSERT_EQ( 2, child3->depth() );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestNode, testLeaf)
  {
    Node root("html");
    Node * body = (new Node("body"));
    Node * child1 = (new Node("h1"));
    Node * child2 = (new Node("p"));
    Node * child3 = (new Node("p"));

    //no children yet
    ASSERT_TRUE( root.isLeaf() );

    //build tree
    root.addChild(body);
    body->addChild(child1);
    body->addChild(child2);
    body->addChild(child3);

    //assert
    ASSERT_FALSE(    root.isLeaf() );
    ASSERT_FALSE(   body->isLeaf() );
    ASSERT_TRUE ( child1->isLeaf() );
    ASSERT_TRUE ( child2->isLeaf() );
    ASSERT_TRUE ( child3->isLeaf() );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestNode, testRoot)
  {
    Node root("html");
    Node * body = (new Node("body"));
    Node * child1 = (new Node("h1"));
    Node * child2 = (new Node("p"));
    Node * child3 = (new Node("p"));

    //no children yet
    ASSERT_TRUE( root.isRoot() );

    //build tree
    root.addChild(body);
    body->addChild(child1);
    body->addChild(child2);
    body->addChild(child3);

    //assert
    ASSERT_TRUE (    root.isRoot() );
    ASSERT_FALSE(   body->isRoot() );
    ASSERT_FALSE( child1->isRoot() );
    ASSERT_FALSE( child2->isRoot() );
    ASSERT_FALSE( child3->isRoot() );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestNode, testSize)
  {
    Node root("html");
    Node * body = (new Node("body"));
    Node * child1 = (new Node("h1"));
    Node * child2 = (new Node("p"));
    Node * child3 = (new Node("p"));

    //no children yet
    ASSERT_EQ( 1, root.size() );

    //build tree
    root.addChild(body);
    body->addChild(child1);
    body->addChild(child2);
    body->addChild(child3);

    //assert
    ASSERT_EQ( 5, root.size() );
    ASSERT_EQ( 4, body->size() );
    ASSERT_EQ( 1, child1->size() );
    ASSERT_EQ( 1, child2->size() );
    ASSERT_EQ( 1, child3->size() );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestNode, testRemoveChild)
  {
    Node root("html");
    Node * body = (new Node("body"));
    Node * child1 = (new Node("h1"));
    Node * child2 = (new Node("p"));
    Node * child3 = (new Node("p"));

    //invalid values
    ASSERT_FALSE( root.removeChild( (Node*)NULL ) );
    ASSERT_FALSE( root.removeChild( child3 ) );
    ASSERT_FALSE( root.removeChild(9999) ); //out of bounds

    //build tree
    root.addChild(body);
    body->addChild(child1);
    body->addChild(child2);
    body->addChild(child3);

    //assert
    ASSERT_EQ( 5, root.size() );
    ASSERT_TRUE( body->removeChild(child3) );
    ASSERT_EQ( 4, root.size() );
    ASSERT_TRUE( body->removeChild(1) ); //matches child2
    ASSERT_EQ( 3, root.size() );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestNode, testRemoveChildren)
  {
    Node root("html");
    Node * body = (new Node("body"));
    Node * child1 = (new Node("h1"));
    Node * child2 = (new Node("p"));
    Node * child3 = (new Node("p"));

    //removeChildren empty nodes
    ASSERT_TRUE( root.removeChildren() );

    //build tree
    root.addChild(body);
    body->addChild(child1);
    body->addChild(child2);
    body->addChild(child3);

    //assert
    ASSERT_TRUE( body->removeChildren() );
    ASSERT_EQ( 1, body->size() ); //body should be a leaf since we removed all children 
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestNode, testRemoveChildrenType)
  {
    Node root("html");
    Node * body = (new Node("body"));
    Node * child1 = (new Node("h1"));
    Node * child2 = (new Node("p"));
    Node * child3 = (new Node("p"));

    //removeChildren empty nodes
    ASSERT_TRUE( root.removeChildren() );

    //build tree
    root.addChild(body);
    body->addChild(child1);
    body->addChild(child2);
    body->addChild(child3);

    //assert
    ASSERT_TRUE( body->removeChildren("p") );
    ASSERT_EQ( 2, body->size() ); //body should only contain child1 of type "h1"
    ASSERT_EQ( child1, body->getChildren()[0] );
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace shellanything
