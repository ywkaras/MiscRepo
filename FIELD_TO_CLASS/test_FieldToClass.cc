/** @file

    Unit tests for FieldToClass.h

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

#include "catch.hpp"
#include "tscpp/util/FieldToClass.h"

TEST_CASE("FieldToClass", "[FTC]")
{
  struct S {
    double m, n;
    struct {
      int a, b, c;
    } ss;
    int q;
  } s;

  REQUIRE(TS_FIELD_TO_CLASS_PTR(S, m, &s.m) == &s);
  REQUIRE(TS_FIELD_TO_CLASS_PTR(S, n, &s.n) == &s);
  REQUIRE(TS_FIELD_TO_CLASS_PTR(S, ss.a, &s.ss.a) == &s);
  REQUIRE(TS_FIELD_TO_CLASS_PTR(S, ss.b, &s.ss.b) == &s);
  REQUIRE(TS_FIELD_TO_CLASS_PTR(S, ss.c, &s.ss.c) == &s);
  REQUIRE(TS_FIELD_TO_CLASS_PTR(S, q, &s.q) == &s);
  {
    const S *sp = &s;
    REQUIRE(TS_CV_FIELD_TO_CLASS_PTR(const, S, q, &sp->q) == sp);
  }
  {
    volatile S *sp = &s;
    REQUIRE(TS_CV_FIELD_TO_CLASS_PTR(volatile, S, q, &sp->q) == sp);
  }
  {
    const volatile S *sp = &s;
    REQUIRE(TS_CV_FIELD_TO_CLASS_PTR(const volatile, S, q, &sp->q) == sp);
  }
}
