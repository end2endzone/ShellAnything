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

#include "TestMenu.h"
#include "Icon.h"
#include "Menu.h"
#include "ActionExecute.h"

namespace shellanything { namespace test
{
  Menu * NewMenu(const std::string & name)
  {
    Menu * menu = new Menu();
    menu->SetName(name);
    return menu;
  }

  class MyMenu : public Menu
  {
  public:
    MyMenu(bool * deleted_flag)
    {
      this->deleted_flag = deleted_flag;
      (*deleted_flag) = false;
    }
    virtual ~MyMenu()
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
    virtual bool Execute(const Context & context) const
    {
      (*executed_flag) = true;
      return true;
    }
  private:
    bool * deleted_flag;
    bool * executed_flag;
  };

  class MyValidator : public Validator
  {
  public:
    MyValidator(bool * deleted_flag)
    {
      this->deleted_flag  = deleted_flag;
      (*deleted_flag)  = false;
    }
    virtual ~MyValidator()
    {
      (*deleted_flag) = true;
    }
  private:
    bool * deleted_flag;
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
  void TestMenu::SetUp()
  {
  }
  //--------------------------------------------------------------------------------------------------
  void TestMenu::TearDown()
  {
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestMenu, testMenuOwnership)
  {
    Menu * root =   NewMenu("html");
    Menu * body =   NewMenu("body");
    Menu * child1 = NewMenu("h1");
    Menu * child2 = NewMenu("p");
    Menu * child3 = NewMenu("p");

    //no children yet
    bool deleted = false;
    MyMenu * my_test_menu = new MyMenu(&deleted);
    ASSERT_FALSE( deleted );

    //build tree
    root->AddMenu(body);
    body->AddMenu(child1);
    body->AddMenu(child2);
    body->AddMenu(child3);
    child3->AddMenu(my_test_menu); //root takes ownership of my_test_menu

    //destroy the tree
    delete root;
    root = NULL;

    //assert that MyMenu destructor was run
    ASSERT_TRUE(deleted);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestMenu, testActionOwnership)
  {
    Menu * root =   NewMenu("html");
    Menu * body =   NewMenu("body");
    Menu * child1 = NewMenu("h1");
    Menu * child2 = NewMenu("p");
    Menu * child3 = NewMenu("p");

    //no children yet
    bool deleted = false;
    bool executed = false;
    MyAction * my_test_action = new MyAction(&deleted, &executed);
    ASSERT_FALSE( deleted );

    //build tree
    root->AddMenu(body);
    body->AddMenu(child1);
    body->AddMenu(child2);
    body->AddMenu(child3);
    child3->AddAction(my_test_action); //child3 takes ownership of my_test_menu

    //destroy the tree
    delete root;
    root = NULL;

    //assert that MyAction destructor was run
    ASSERT_TRUE(deleted);
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestMenu, testVisibilityValidityOwnership)
  {
    Menu * root =   NewMenu("root");

    //no children yet
    bool visibility_deleted   = false;
    bool validity_deleted     = false;
    MyValidator * my_visibility = new MyValidator(&visibility_deleted);
    MyValidator * my_validity = new MyValidator(&validity_deleted);
    ASSERT_FALSE( visibility_deleted  );
    ASSERT_FALSE( validity_deleted    );

    //build tree
    root->AddVisibility(my_visibility); //root takes ownership of my_visibility
    root->AddValidity(my_validity);     //root takes ownership of my_validity

    //destroy the menu
    delete root;
    root = NULL;

    //assert that my_visibility and my_validity destructor was run
    ASSERT_TRUE( visibility_deleted  );
    ASSERT_TRUE( validity_deleted    );
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestMenu, testSubMenus)
  {
    Menu * root =   NewMenu("html");
    Menu * body =   NewMenu("body");
    Menu * child1 = NewMenu("h1");
    Menu * child2 = NewMenu("p");
    Menu * child3 = NewMenu("p");

    //no children yet
    ASSERT_TRUE( root->GetSubMenus().empty() );

    //build tree
    root->AddMenu(body);
    body->AddMenu(child1);
    body->AddMenu(child2);
    body->AddMenu(child3);

    Menu::MenuPtrList subs = body->GetSubMenus();
    ASSERT_EQ(3, subs.size());
    ASSERT_EQ( child1, subs[0] );
    ASSERT_EQ( child2, subs[1] );
    ASSERT_EQ( child3, subs[2] );

    //destroy the tree
    delete root;
    root = NULL;
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestMenu, testAssignCommandIds)
  {
    Menu * root =   NewMenu("html");
    Menu * body =   NewMenu("body");
    Menu * child1 = NewMenu("h1");
    Menu * child2 = NewMenu("p0");
    Menu * child3 = NewMenu("p1");
    Menu * child4 = NewMenu("p1.1");
    Menu * child5 = NewMenu("p1.2");
    Menu * child6 = NewMenu("p2");
 
    //build tree
    root->AddMenu(body);
    body->AddMenu(child1);
    body->AddMenu(child2);
    body->AddMenu(child3);
    child3->AddMenu(child4);
    child3->AddMenu(child5);
    body->AddMenu(child6);
 
    //act
    uint32_t nextAvailableId = root->AssignCommandIds(101);
 
    //assert
    ASSERT_EQ(109, nextAvailableId);
    ASSERT_EQ(101,   root->GetCommandId());
    ASSERT_EQ(102,   body->GetCommandId());
    ASSERT_EQ(103, child1->GetCommandId());
    ASSERT_EQ(104, child2->GetCommandId());
    ASSERT_EQ(105, child3->GetCommandId());
    ASSERT_EQ(106, child4->GetCommandId());
    ASSERT_EQ(107, child5->GetCommandId());
    ASSERT_EQ(108, child6->GetCommandId());
 
    //destroy the tree
    delete root;
    root = NULL;
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestMenu, testFindMenuByCommandId)
  {
    Menu * root =   NewMenu("html");
    Menu * body =   NewMenu("body");
    Menu * child1 = NewMenu("h1");
    Menu * child2 = NewMenu("p0");
    Menu * child3 = NewMenu("p1");
    Menu * child4 = NewMenu("p1.1");
    Menu * child5 = NewMenu("p1.2");
    Menu * child6 = NewMenu("p2");
 
    //build tree
    root->AddMenu(body);
    body->AddMenu(child1);
    body->AddMenu(child2);
    body->AddMenu(child3);
    child3->AddMenu(child4);
    child3->AddMenu(child5);
    body->AddMenu(child6);
 
    //act
    uint32_t nextAvailableId = root->AssignCommandIds(101);
 
    //assert
    ASSERT_EQ( (Menu*)NULL, root->FindMenuByCommandId(9999999));
 
    ASSERT_EQ(  root, root->FindMenuByCommandId(101));
    ASSERT_EQ(  body, root->FindMenuByCommandId(102));
    ASSERT_EQ(child1, root->FindMenuByCommandId(103));
    ASSERT_EQ(child2, root->FindMenuByCommandId(104));
    ASSERT_EQ(child3, root->FindMenuByCommandId(105));
    ASSERT_EQ(child4, root->FindMenuByCommandId(106));
    ASSERT_EQ(child5, root->FindMenuByCommandId(107));
    ASSERT_EQ(child6, root->FindMenuByCommandId(108));
 
    //destroy the tree
    delete root;
    root = NULL;
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestMenu, testGetNameMaxLength)
  {
    //test default value
    Menu m;
    ASSERT_EQ(Menu::DEFAULT_NAME_MAX_LENGTH, m.GetNameMaxLength());

    m.SetNameMaxLength(34);
    ASSERT_EQ(34, m.GetNameMaxLength());
  }
  //--------------------------------------------------------------------------------------------------
  TEST_F(TestMenu, testTruncateName)
  {
    //Test no truncation required
    {
      const std::string expected = "foobar";
      std::string actual = expected;
      Menu menu;
      menu.TruncateName(actual);
      ASSERT_EQ(expected, actual);
    }

    //Test truncation
    {
      const std::string expected = "foobar";
      std::string actual = expected;
      Menu menu;
      menu.SetNameMaxLength(3);
      menu.TruncateName(actual);
      ASSERT_EQ("foo", actual);
    }

    // Test maximum length
    {
      std::string actual(3000, 'z');
      Menu menu;
      menu.TruncateName(actual);

      int expected_length = Menu::DEFAULT_NAME_MAX_LENGTH + 3; // for the trailing "..."
      ASSERT_EQ(expected_length, actual.size());

      // Look for the trailing "..."
      ASSERT_EQ('.', actual[actual.size() - 1]);
      ASSERT_EQ('.', actual[actual.size() - 2]);
      ASSERT_EQ('.', actual[actual.size() - 3]);
    }

    // Test maximum length with no trailing
    {
      std::string actual(Menu::DEFAULT_NAME_MAX_LENGTH, 'z');
      Menu menu;
      menu.TruncateName(actual);

      ASSERT_EQ(Menu::DEFAULT_NAME_MAX_LENGTH, actual.size());
    }
  }
  //--------------------------------------------------------------------------------------------------

} //namespace test
} //namespace shellanything
