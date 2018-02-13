/** @file

   Defines TS_FIELD_TO_CLASS_PTR macro.

   @section license License

   Licensed to the Apache Software Foundation (ASF) under one
   or more contributor license agreements.  See the NOTICE file
   distributed with this work for additional information
   regarding copyright ownership.  The ASF licenses this file
   to you under the Apache License, Version 2.0 (the
   "License"); you may not use this file except in compliance
   with the License.  You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 */

#pragma once

#include <cstddef>
#include <utility>

// If FIELD_PTR is a pointer to a field specified by FIELD_SPEC in an instance of a class named CLASS_NAME, then
// TS_FIELD_TO_CLASS_PTR(CLASS_NAME, FIELD_SPEC, FIELD_PTR) returns a pointer to the instance of CLASS_NAME.
//
// Example:
//
// struct S
// {
//   int m, n;
//   struct { int a, b, c; } ss;
//   int q;
// } s;
//
// assert(TS_FIELD_TO_CLASS_PTR(S, ss.b, &(s.ss.b)) == &s);
//
#define TS_FIELD_TO_CLASS_PTR(CLASS_NAME, FIELD_SPEC, FIELD_PTR)                                                \
  ([](decltype(std::declval<CLASS_NAME>().FIELD_SPEC) *fldPtr) -> CLASS_NAME * {                                \
    return reinterpret_cast<CLASS_NAME *>(reinterpret_cast<char *>(fldPtr) - offsetof(CLASS_NAME, FIELD_SPEC)); \
  }(FIELD_PTR))

// Version for const, volatile, const volatile pointers.
//
// Example:
//
// struct S
// {
//   int m, n;
//   struct { int a, b, c; } ss;
//   int q;
// } s;
//
// const S *sp = &s;
//
// assert(TS_CV_FIELD_TO_CLASS_PTR(const, S, ss.b, &(sp->ss.b)) == sp);
//
#define TS_CV_FIELD_TO_CLASS_PTR(CVSPEC, CLASS_NAME, FIELD_SPEC, FIELD_PTR)                                                   \
  ([](CVSPEC decltype(std::declval<CLASS_NAME>().FIELD_SPEC) *fldPtr) -> CVSPEC CLASS_NAME * {                                \
    return reinterpret_cast<CVSPEC CLASS_NAME *>(reinterpret_cast<CVSPEC char *>(fldPtr) - offsetof(CLASS_NAME, FIELD_SPEC)); \
  }(FIELD_PTR))

/*
NOTE: this is generalizing the technique of static_cast'ing from a base class pointer to a derived class pointer.  It
allows this technique with class fields as well as base classes.  But it's easy to make a mistake that the compiler
would not catch unfortunately, must be used very carefully.  The lambda function is used so that at least the compiler
can check that the type of the pointer and the type of the class field match.
*/
