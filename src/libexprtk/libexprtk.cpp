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

#include "libexprtk.h"

#include <stdint.h>
#include <stddef.h>

#include <algorithm>    // std::min
#include <stdio.h>      // snprintf

#define _SCL_SECURE_NO_WARNINGS
#include "exprtk.hpp"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

// https://stackoverflow.com/questions/2915672/snprintf-and-visual-studio-2010
#if defined(_MSC_VER) && _MSC_VER < 1900

#define snprintf c99_snprintf
#define vsnprintf c99_vsnprintf

__inline int c99_vsnprintf(char *outBuf, size_t size, const char *format, va_list ap)
{
  int count = -1;
  if (size != 0)
    count = _vsnprintf_s(outBuf, size, _TRUNCATE, format, ap);
  if (count == -1)
    count = _vscprintf(format, ap);
  return count;
}

__inline int c99_snprintf(char *outBuf, size_t size, const char *format, ...)
{
  int count;
  va_list ap;
  va_start(ap, format);
  count = c99_vsnprintf(outBuf, size, format, ap);
  va_end(ap);
  return count;
}

#endif


int EvaluateDouble(const char * expression_string, double * result, char * error_buffer, int error_size)
{
  if (error_buffer != NULL && error_size > 0)
    error_buffer[0] = '\0';

  typedef exprtk::expression<double> expression_t;
  typedef exprtk::parser<double>         parser_t;

  expression_t expression;
  parser_t parser;

  if (!parser.compile(expression_string,expression))
  {
    //Output error description in output
    if (error_buffer != NULL && error_size > 0)
    {
      std::string error_description = parser.error();

      size_t min_buffer_size = MIN(error_description.size()+1, error_size); // +1 to include the last non-NULL character
      snprintf(error_buffer, min_buffer_size, "%s", error_description.c_str());
    }
    return 0;
  }

  if (result)
    *result = expression.value();

  return 1;
}

int EvaluateBoolean(const char * expression_string, int* result, char * error, int error_size)
{
  double tmp = 0.0;
  bool success = EvaluateDouble(expression_string, &tmp, error, error_size);
  if (!success)
    return 0;

  if (result)
  {
    if (tmp != 0.0)
      *result = 1;
    else
      *result = 0;
  }
  return 1;
}
