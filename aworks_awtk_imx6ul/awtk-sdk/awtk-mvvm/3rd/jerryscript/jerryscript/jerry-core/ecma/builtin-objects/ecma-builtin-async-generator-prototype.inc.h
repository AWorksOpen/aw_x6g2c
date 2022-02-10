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

/*
 * AsyncGenerator.prototype built-in description
 */

#include "ecma-builtin-helpers-macro-defines.inc.h"

#if JERRY_ESNEXT

/* Object properties:
 *  (property name, object pointer getter) */

/* ECMA-262 v6, 25.3.1.5 */
STRING_VALUE (LIT_GLOBAL_SYMBOL_TO_STRING_TAG, LIT_MAGIC_STRING_ASYNC_GENERATOR_UL, ECMA_PROPERTY_FLAG_CONFIGURABLE)

/* ECMA-262 v6, 25.2.3.1 */
OBJECT_VALUE (LIT_MAGIC_STRING_CONSTRUCTOR, ECMA_BUILTIN_ID_ASYNC_GENERATOR, ECMA_PROPERTY_FLAG_CONFIGURABLE)

/* Routine properties:
 *  (property name, C routine name, arguments number or NON_FIXED, value of the routine's length property) */
ROUTINE (LIT_MAGIC_STRING_NEXT, ECMA_ASYNC_GENERATOR_PROTOTYPE_ROUTINE_NEXT, 1, 1)
ROUTINE (LIT_MAGIC_STRING_RETURN, ECMA_ASYNC_GENERATOR_PROTOTYPE_ROUTINE_RETURN, 1, 1)
ROUTINE (LIT_MAGIC_STRING_THROW, ECMA_ASYNC_GENERATOR_PROTOTYPE_ROUTINE_THROW, 1, 1)

#endif /* JERRY_ESNEXT */

#include "ecma-builtin-helpers-macro-undefs.inc.h"
