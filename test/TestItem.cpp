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
#include "shellanything/Icon.h"
#include "shellanything/Item.h"
#include "shellanything/ActionExecute.h"

namespace shellanything { namespace test
{
  Item * newItem(const std::string & name)
  {
    Item * item = new Item();
    item->setName(name);
    return item;
  }

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
    MyAction(bool * deleted_flag, bool * executed_flag)
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

  class MyIcon : public Icon
  {
  public:
    MyIcon(bool * deleted_flag)
    {
      this->deleted_flag  = deleted_flag;
      (*deleted_flag)  = false;
    }
    virtual ~MyIcon()
    {
      (*deleted_flag) = true;
    }
  private:
    bool * deleted_flag;
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
    Item * root =   newItem("html");
    Item * body =   newItem("body");
    Item * child1 = newItem("h1");
    Item * child2 = newItem("p");
    Item * child3 = newItem("p");

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
    Item * root =   newItem("html");
    Item * body =   newItem("body");
    Item * child1 = newItem("h1");
    Item * child2 = newItem("p");
    Item * child3 = newItem("p");

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
  TEST_F(TestItem, testSubItems)
  {
    Item * root =   newItem("html");
    Item * body =   newItem("body");
    Item * child1 = newItem("h1");
    Item * child2 = newItem("p");
    Item * child3 = newItem("p");

    //no children yet
    ASSERT_TRUE( root->getSubItems().empty() );

    //build tree
    root->addChild(body);
    body->addChild(child1);
    body->addChild(child2);
    body->addChild(child3);

    Item::ItemPtrList subs = body->getSubItems();
    ASSERT_EQ(3, subs.size());
    ASSERT_EQ( child1, subs[0] );
    ASSERT_EQ( child2, subs[1] );
    ASSERT_EQ( child3, subs[2] );

    //destroy the tree
    delete root;
    root = NULL;
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestItem, testAssignCommandIds)
  {
    Item * root =   newItem("html");
    Item * body =   newItem("body");
    Item * child1 = newItem("h1");
    Item * child2 = newItem("p0");
    Item * child3 = newItem("p1");
    Item * child4 = newItem("p1.1");
    Item * child5 = newItem("p1.2");
    Item * child6 = newItem("p2");
 
    //build tree
    root->addChild(body);
    body->addChild(child1);
    body->addChild(child2);
    body->addChild(child3);
    child3->addChild(child4);
    child3->addChild(child5);
    body->addChild(child6);
 
    //act
    uint32_t nextAvailableId = root->assignCommandIds(101);
 
    //assert
    ASSERT_EQ(109, nextAvailableId);
    ASSERT_EQ(101,   root->getCommandId());
    ASSERT_EQ(102,   body->getCommandId());
    ASSERT_EQ(103, child1->getCommandId());
    ASSERT_EQ(104, child2->getCommandId());
    ASSERT_EQ(105, child3->getCommandId());
    ASSERT_EQ(106, child4->getCommandId());
    ASSERT_EQ(107, child5->getCommandId());
    ASSERT_EQ(108, child6->getCommandId());
 
    //destroy the tree
    delete root;
    root = NULL;
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestItem, testFindItemByCommandId)
  {
    Item * root =   newItem("html");
    Item * body =   newItem("body");
    Item * child1 = newItem("h1");
    Item * child2 = newItem("p0");
    Item * child3 = newItem("p1");
    Item * child4 = newItem("p1.1");
    Item * child5 = newItem("p1.2");
    Item * child6 = newItem("p2");
 
    //build tree
    root->addChild(body);
    body->addChild(child1);
    body->addChild(child2);
    body->addChild(child3);
    child3->addChild(child4);
    child3->addChild(child5);
    body->addChild(child6);
 
    //act
    uint32_t nextAvailableId = root->assignCommandIds(101);
 
    //assert
    ASSERT_EQ( (Item*)NULL, root->findItemByCommandId(9999999));
 
    ASSERT_EQ(  root, root->findItemByCommandId(101));
    ASSERT_EQ(  body, root->findItemByCommandId(102));
    ASSERT_EQ(child1, root->findItemByCommandId(103));
    ASSERT_EQ(child2, root->findItemByCommandId(104));
    ASSERT_EQ(child3, root->findItemByCommandId(105));
    ASSERT_EQ(child4, root->findItemByCommandId(106));
    ASSERT_EQ(child5, root->findItemByCommandId(107));
    ASSERT_EQ(child6, root->findItemByCommandId(108));
 
    //destroy the tree
    delete root;
    root = NULL;
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace shellanything
