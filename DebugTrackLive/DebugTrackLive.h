/** @file

  Debug utility for detecting stale pointers and corrupt memory allocation.

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

#include <cstdlib> // for size_t
#include <mutex>

// This class is a scope only, do not instantiate.
//
class DebugTrackLive
{
public:
  class TypeData
  {
  private:
    const std::size_t _instanceSize;

    TypeData(std::size_t s) : _instanceSize(s) {}

    friend class DebugTrackLive;
  };

  // Call this in the constructor of or when creating an instance of T.  (If T is a class, it should have at least one data member
  // if used as a base class.)
  //
  template <typename T>
  static void
  create(const T *obj, bool containsTrackedType = false)
  {
    create(typeData<T>(), reinterpret_cast<const char *>(obj), containsTrackedType);
  }

  // Call this in the destructor of or when destroying an instance of T.
  //
  template <typename T>
  static void
  destroy(const T *obj)
  {
    destroy(typeData<T>(), reinterpret_cast<const char *>(obj));
  }

  // Verify sure 'obj' is not stale pointer.

  template <typename T>
  static void
  check(const T *obj)
  {
    check(typeData<T>(), reinterpret_cast<const char *>(obj));
  }

  // Because of the way lib/bs/Allocator.h "works", there can be "valid" objects for which the constructor is not called.  To
  // help in these cases, this function checks if an object is within another object for which the create function above has
  // been called.
  //
  template <typename T>
  static void
  within(const T *obj)
  {
    within(typeData<T>(), reinterpret_cast<const char *>(obj));
  }

private:
  using MtxGuard = std::lock_guard<std::mutex>;

  // Return pointer to unique TypeData instance per tracked type.
  //
  template <typename T>
  static TypeData *
  typeData()
  {
    MtxGuard mg(mtx());
    {
      static TypeData d(sizeof(T));
      return &d;
    }
  }

  static void create(TypeData *d, const char *obj, bool containsTrackedType);

  static void destroy(TypeData *d, const char *obj);

  static void check(TypeData *d, const char *obj);

  static void within(TypeData *d, const char *obj);

  static std::mutex & mtx();

}; // end class DebugTrackLive
