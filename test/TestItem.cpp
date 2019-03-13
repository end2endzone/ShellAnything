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

#include "TestItem.h"
#include "shellanything/Item.h"

namespace shellanything { namespace test
{
  class MyItem : public Item
  {
  public:
    MyItem(bool * deleted_flag)
    {
      this->deleted_flag = deleted_flag;
      (*deleted_flag) = false;
    }
    virtual ~MyItem()
    {
      (*deleted_flag) = true;
    }
  private:
    bool * deleted_flag;
  };

  class MyAction : public Action
  {
  public:
    MyAction(bool * deleted_flag, bool * executed_flag) : Action("MyAction")
    {
      this->deleted_flag  = deleted_flag;
      this->executed_flag = executed_flag;
      (*deleted_flag)  = false;
      (*executed_flag) = false;
    }
    virtual ~MyAction()
    {
      (*deleted_flag) = true;
    }
    virtual bool execute(const Context & iContext) const
    {
      (*executed_flag) = true;
      return true;
    }
  private:
    bool * deleted_flag;
    bool * executed_flag;
  };


  //--------------------------------------------------------------------------------------------------
  void TestItem::SetUp()
  {
  }
  //--------------------------------------------------------------------------------------------------
  void TestItem::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestItem, testItemOwnership)
  {
    Item * root =   (new Item("html"));
    Item * body =   (new Item("body"));
    Item * child1 = (new Item("h1"));
    Item * child2 = (new Item("p"));
    Item * child3 = (new Item("p"));

    //no children yet
    bool deleted = false;
    MyItem * my_test_item = new MyItem(&deleted);
    ASSERT_FALSE( deleted );

    //build tree
    root->addChild(body);
    body->addChild(child1);
    body->addChild(child2);
    body->addChild(child3);
    child3->addChild(my_test_item); //root takes ownership of my_test_item

    //destroy the tree
    delete root;
    root = NULL;

    //assert that MyItem destructor was run
    ASSERT_TRUE(deleted);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestItem, testActionOwnership)
  {
    Item * root =   (new Item("html"));
    Item * body =   (new Item("body"));
    Item * child1 = (new Item("h1"));
    Item * child2 = (new Item("p"));
    Item * child3 = (new Item("p"));

    //no children yet
    bool deleted = false;
    bool executed = false;
    MyAction * my_test_action = new MyAction(&deleted, &executed);
    ASSERT_FALSE( deleted );

    //build tree
    root->addChild(body);
    body->addChild(child1);
    body->addChild(child2);
    body->addChild(child3);
    child3->addAction(my_test_action); //child3 takes ownership of my_test_item

    //destroy the tree
    delete root;
    root = NULL;

    //assert that MyAction destructor was run
    ASSERT_TRUE(deleted);
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace shellanything
