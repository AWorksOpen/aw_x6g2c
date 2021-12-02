/* Copyright JS Foundation and other contributors, http://js.foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ecma-builtins.h"
#include "ecma-exceptions.h"
#include "ecma-gc.h"
#include "ecma-globals.h"
#include "ecma-helpers.h"
#include "ecma-proxy-object.h"
#include "jrt.h"

#if ENABLED (JERRY_ES2015_BUILTIN_PROXY)

#define ECMA_BUILTINS_INTERNAL
#include "ecma-builtins-internal.h"

#define BUILTIN_INC_HEADER_NAME "ecma-builtin-proxy.inc.h"
#define BUILTIN_UNDERSCORED_ID proxy
#include "ecma-builtin-internal-routines-template.inc.h"

/** \addtogroup ecma ECMA
 * @{
 *
 * \addtogroup ecmabuiltins
 * @{
 *
 * \addtogroup proxy ECMA Proxy object built-in
 * @{
 */

/**
 * The Proxy object's 'revocable' routine
 *
 * See also:
 *         ES2015 26.2.2.1
 *
 * @return ecma value
 *         Returned value must be freed with ecma_free_value.
 */
static ecma_value_t
ecma_builtin_proxy_object_revocable (ecma_value_t this_arg, /**< 'this' argument */
                                     ecma_value_t target, /**< target argument */
                                     ecma_value_t handler) /**< handler argument */
{
  JERRY_UNUSED (this_arg);

  ecma_object_t *rev_proxy_p = ecma_proxy_create_revocable (target, handler);

  if (JERRY_UNLIKELY (rev_proxy_p == NULL))
  {
    return ECMA_VALUE_ERROR;
  }

  return ecma_make_object_value (rev_proxy_p);
} /* ecma_builtin_proxy_object_revocable */

/**
 * Handle calling [[Call]] of built-in Proxy object
 *
 * See also:
 *          ES2015 26.2.2
 *
 * @return raised error
 */
ecma_value_t
ecma_builtin_proxy_dispatch_call (const ecma_value_t *arguments_list_p, /**< arguments list */
                                  ecma_length_t arguments_list_len) /**< number of arguments */
{
  JERRY_ASSERT (arguments_list_len == 0 || arguments_list_p != NULL);

  /* 1. */
  return ecma_raise_type_error (ECMA_ERR_MSG ("Constructor Proxy requires 'new'"));
} /* ecma_builtin_proxy_dispatch_call */

/**
 * Handle calling [[Construct]] of built-in proxy object
 *
 * See also:
 *          ES2015 26.2.2
 *
 * @return ECMA_VALUE_ERROR - if the operation fails
 *         new proxy object - otherwise
 */
ecma_value_t
ecma_builtin_proxy_dispatch_construct (const ecma_value_t *arguments_list_p, /**< arguments list */
                                       ecma_length_t arguments_list_len) /**< number of arguments */
{
  JERRY_ASSERT (arguments_list_len == 0 || arguments_list_p != NULL);

  /* 2. */
  ecma_object_t *proxy_p = ecma_proxy_create (arguments_list_len > 0 ? arguments_list_p[0] : ECMA_VALUE_UNDEFINED,
                                              arguments_list_len > 1 ? arguments_list_p[1] : ECMA_VALUE_UNDEFINED);

  if (JERRY_UNLIKELY (proxy_p == NULL))
  {
    return ECMA_VALUE_ERROR;
  }

  return ecma_make_object_value (proxy_p);
} /* ecma_builtin_proxy_dispatch_construct */

/**
 * @}
 * @}
 * @}
 */

#endif /* ENABLED (JERRY_ES2015_BUILTIN_PROXY) */
