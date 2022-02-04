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

#ifndef SA_API_MEMORY_H
#define SA_API_MEMORY_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/// <summary>
/// Allocate an amount of memory usable by the library.
/// </summary>
/// <param name="size">The size of the allocation in bytes.</param>
/// <returns>Returns a non-null void pointer on success. Returns NULL on error.</returns>
void* sa_memory_alloc(size_t size);

/// <summary>
/// Free memory allocated with sa_memory_alloc()
/// </summary>
/// <param name="ptr">A void pointer of memory allocated with sa_memory_alloc().</param>
void sa_memory_free(void* ptr);

#ifdef __cplusplus
}
#endif

#endif //SA_API_MEMORY_H
