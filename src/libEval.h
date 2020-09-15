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

#ifndef SA_LIBEVAL_H
#define SA_LIBEVAL_H

namespace shellanything
{
  /// <summary>
  /// Evaluates a text expression and calculates the result.
  /// </summary>
  /// <param name="expression_string">The text expression to evaluate.</param>
  /// <param name="output">The output double value of the result expression.</param>
  /// <returns>Returns true if the evaluation is successfull. Returns false otherwise.</returns>
  bool evaluate(const char * expression_string, double * result);

  /// <summary>
  /// Evaluates a boolean text expression as a true or false value.
  /// </summary>
  /// <param name="expression_string">The text expression to evaluate.</param>
  /// <param name="output">The output bool value of the result expression.</param>
  /// <returns>Returns true if the evaluation is successfull. Returns false otherwise.</returns>
  bool evaluate(const char * expression_string, bool * result);

} //namespace shellanything

#endif //SA_LIBEVAL_H
