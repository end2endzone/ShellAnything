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

#include "libEval.h"

#define _SCL_SECURE_NO_WARNINGS
#include "exprtk.hpp"

namespace shellanything
{

  bool evaluate(const char * expression_string, double * result)
  {
    typedef exprtk::expression<double> expression_t;
    typedef exprtk::parser<double>         parser_t;

    //symbol_table_t symbol_table;
    //symbol_table.add_variable("x",x);
    //symbol_table.add_variable("y",y);
    //symbol_table.add_variable("z",z);

    expression_t expression;

    parser_t parser;

    if (!parser.compile(expression_string,expression))
      return false;

    if (result)
      *result = expression.value();

    return true;
  }

  bool evaluate(const char * expression_string, bool * result)
  {
    double tmp = 0.0;
    bool success = evaluate(expression_string, &tmp);
    if (!success)
      return false;

    if (result)
    {
      if (tmp != 0.0)
        *result = true;
      else
        *result = false;
    }
    return true;
  }

} //namespace shellanything
