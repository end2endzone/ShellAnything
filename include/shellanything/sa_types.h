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

#ifndef SA_API_TYPES_H
#define SA_API_TYPES_H

#ifdef __cplusplus
extern "C" {
#if 0
}  // do not indent code inside extern C
#endif
#endif

typedef int sa_boolean;

typedef struct sa_action_t
{
  void* opaque;
} sa_action_t;

typedef struct sa_action_immutable_t
{
  void* opaque;
} sa_action_immutable_t;

typedef struct sa_configuration_t
{
  void* opaque;
} sa_configuration_t;

typedef struct sa_configuration_immutable_t
{
  void* opaque;
} sa_configuration_immutable_t;

typedef struct sa_icon_t
{
  void* opaque;
} sa_icon_t;

typedef struct sa_icon_immutable_t
{
  void* opaque;
} sa_icon_immutable_t;

typedef struct sa_menu_t
{
  void* opaque;
} sa_menu_t;

typedef struct sa_menu_immutable_t
{
  void* opaque;
} sa_menu_immutable_t;

typedef struct sa_property_store_t
{
  void* opaque;
} sa_property_store_t;

typedef struct sa_property_store_immutable_t
{
  void* opaque;
} sa_property_store_immutable_t;

typedef struct
{
  void* opaque;
} sa_selection_context_t;

typedef struct
{
  void* opaque;
} sa_selection_context_immutable_t;

typedef struct
{
  void* opaque;
} sa_string_t;

typedef struct sa_validator_t
{
  void* opaque;
} sa_validator_t;

typedef struct sa_validator_immutable_t
{
  void* opaque;
} sa_validator_immutable_t;

#ifdef __cplusplus
#if 0
{  // do not indent code inside extern C
#endif
}  // extern "C"
#endif

#endif //SA_API_TYPES_H
