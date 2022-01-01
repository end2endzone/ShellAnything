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

    ASSERT_TRUE( root.GetParent() == NULL );

    root.AddChild(child1);

    ASSERT_TRUE(    root.GetParent() == NULL );
    ASSERT_TRUE( child1->GetParent() != NULL );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestNode, testGetChildren)
  {
    Node root("root");
    Node * child1 = (new Node("child1"));
    Node * child2 = (new Node("child2"));

    //no children yet
    ASSERT_EQ(0, root.GetChildren().size());

    root.AddChild(child1);
    root.AddChild(child2);

    //assert 2 children
    ASSERT_EQ(2, root.GetChildren().size());
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
    ASSERT_EQ(0, root.FindChildren("").size());

    //build tree
    root.AddChild(body);
    body->AddChild(child1);
    body->AddChild(child2);
    body->AddChild(child3);

    //search for a node that is not found
    ASSERT_EQ(0, body->FindChildren("a").size());

    //search for multiple nodes
    ASSERT_EQ(2, body->FindChildren("p").size());
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
    ASSERT_TRUE( root.FindFirst("") == NULL );

    //build tree
    root.AddChild(body);
    body->AddChild(child1);
    body->AddChild(child2);
    body->AddChild(child3);

    //search for a node that is not found
    ASSERT_EQ( NULL, body->FindFirst("a") );

    //search for multiple nodes
    ASSERT_EQ(child2, body->FindFirst("p") );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestNode, testGetNumChildren)
  {
    Node root("root");
    Node * child1 = (new Node("child1"));
    Node * child2 = (new Node("child2"));

    //no children yet
    ASSERT_EQ(0, root.GetNumChildren());

    root.AddChild(child1);
    root.AddChild(child2);

    //assert 2 children
    ASSERT_EQ(2, root.GetNumChildren());
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
    ASSERT_TRUE( root.GetChild(0) == NULL );

    //build tree
    root.AddChild(body);
    body->AddChild(child1);
    body->AddChild(child2);
    body->AddChild(child3);

    //search for a node that is not found
    ASSERT_EQ( NULL, body->GetChild(999) );

    //search specific nodes
    ASSERT_EQ(child1, body->GetChild(0) );
    ASSERT_EQ(child2, body->GetChild(1) );
    ASSERT_EQ(child3, body->GetChild(2) );
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
    ASSERT_EQ( 0, root.Depth() );

    //build tree
    root.AddChild(body);
    body->AddChild(child1);
    body->AddChild(child2);
    body->AddChild(child3);

    //assert
    ASSERT_EQ( 0, root.Depth() );
    ASSERT_EQ( 1, body->Depth() );
    ASSERT_EQ( 2, child1->Depth() );
    ASSERT_EQ( 2, child2->Depth() );
    ASSERT_EQ( 2, child3->Depth() );
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
    ASSERT_TRUE( root.IsLeaf() );

    //build tree
    root.AddChild(body);
    body->AddChild(child1);
    body->AddChild(child2);
    body->AddChild(child3);

    //assert
    ASSERT_FALSE(    root.IsLeaf() );
    ASSERT_FALSE(   body->IsLeaf() );
    ASSERT_TRUE ( child1->IsLeaf() );
    ASSERT_TRUE ( child2->IsLeaf() );
    ASSERT_TRUE ( child3->IsLeaf() );
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
    ASSERT_TRUE( root.IsRoot() );

    //build tree
    root.AddChild(body);
    body->AddChild(child1);
    body->AddChild(child2);
    body->AddChild(child3);

    //assert
    ASSERT_TRUE (    root.IsRoot() );
    ASSERT_FALSE(   body->IsRoot() );
    ASSERT_FALSE( child1->IsRoot() );
    ASSERT_FALSE( child2->IsRoot() );
    ASSERT_FALSE( child3->IsRoot() );
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
    ASSERT_EQ( 1, root.Size() );

    //build tree
    root.AddChild(body);
    body->AddChild(child1);
    body->AddChild(child2);
    body->AddChild(child3);

    //assert
    ASSERT_EQ( 5, root.Size() );
    ASSERT_EQ( 4, body->Size() );
    ASSERT_EQ( 1, child1->Size() );
    ASSERT_EQ( 1, child2->Size() );
    ASSERT_EQ( 1, child3->Size() );
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
    ASSERT_FALSE( root.RemoveChild( (Node*)NULL ) );
    ASSERT_FALSE( root.RemoveChild( child3 ) );
    ASSERT_FALSE( root.RemoveChild(9999) ); //out of bounds

    //build tree
    root.AddChild(body);
    body->AddChild(child1);
    body->AddChild(child2);
    body->AddChild(child3);

    //assert
    ASSERT_EQ( 5, root.Size() );
    ASSERT_TRUE( body->RemoveChild(child3) );
    ASSERT_EQ( 4, root.Size() );
    ASSERT_TRUE( body->RemoveChild(1) ); //matches child2
    ASSERT_EQ( 3, root.Size() );
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
    ASSERT_TRUE( root.RemoveChildren() );

    //build tree
    root.AddChild(body);
    body->AddChild(child1);
    body->AddChild(child2);
    body->AddChild(child3);

    //assert
    ASSERT_TRUE( body->RemoveChildren() );
    ASSERT_EQ( 1, body->Size() ); //body should be a leaf since we removed all children 
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
    ASSERT_TRUE( root.RemoveChildren() );

    //build tree
    root.AddChild(body);
    body->AddChild(child1);
    body->AddChild(child2);
    body->AddChild(child3);

    //assert
    ASSERT_TRUE( body->RemoveChildren("p") );
    ASSERT_EQ( 2, body->Size() ); //body should only contain child1 of type "h1"
    ASSERT_EQ( child1, body->GetChildren()[0] );
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace shellanything
