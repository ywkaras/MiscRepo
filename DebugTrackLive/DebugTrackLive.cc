/** @file

  Implementation file for DebugTrackLive.h.

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

#include <cassert>
#include <set>

#include "DebugTrackLive.h"

using TypeData = DebugTrackLive::TypeData;

namespace
{

struct SetElem
{
  const char * const bound;
  TypeData * const d;

  // 'end' is true if 'bound' points to 1 past last instance byte, false if 'bound' points to first instance byte.
  const bool end;

  SetElem(const char *bound_, TypeData *d_, bool end_) : bound(bound_), d(d_), end(end_) {}
};

struct SetCompare
{
  bool
  operator () (const SetElem &a, const SetElem &b) const
  {
    if (a.bound < b.bound) {
      return true;
    }
    if (a.bound > b.bound) {
      return false;
    }
    if (a.end and !b.end) {
      return true;
    }
    if (!a.end and b.end) {
      return false;
    }
    return a.d < b.d;
  }
};

using SetT = std::set<SetElem, SetCompare>;

inline SetT &
set()
{
  static SetT s;
  return s;
}

inline const SetT &
cSet()
{
  return set();
}

} // end anonymous namespace

void
DebugTrackLive::create(TypeData *d, const char *obj, bool containsTrackedType)
{
  MtxGuard mg(mtx());

  SetElem beginBound(obj, d, false);

  // First check for bad overlaps.  It's presumed that any contained tracked object will be created before the containing object.

  auto iter = cSet().lower_bound(beginBound);

  while (iter != cSet().end()) {
    if (std::size_t(iter->bound - obj) >= d->_instanceSize) {
      if (!iter->end) {
        // Start of next object is beyond last byte of this object, so no more overlaps.
        break;

      } else {
        assert(containsTrackedType);
        assert((iter->bound - iter->d->_instanceSize) > obj);
        assert(iter->bound == (obj + d->_instanceSize)); // Must have same end as object being created.
      }
    }
    if (!iter->end) {
      assert(containsTrackedType);
      assert((iter->bound + iter->d->_instanceSize) <= (obj + d->_instanceSize));

    } else {
      assert(containsTrackedType);
      assert((iter->bound - iter->d->_instanceSize) >= obj);
    }
    ++iter;
  }

  // Put start and end of created object into the tracking set.
  //
  set().insert(beginBound);
  set().emplace(obj + d->_instanceSize, d, true);
}

void
DebugTrackLive::destroy(TypeData *d, const char *obj)
{
  MtxGuard mg(mtx());

  auto iter = set().find(SetElem(obj, d, false));
  assert(iter != set().end());
  set().erase(iter);

  iter = set().find(SetElem(obj + d->_instanceSize, d, true));
  assert(iter != set().end());
  set().erase(iter);
}

void
DebugTrackLive::check(TypeData *d, const char *obj)
{
  MtxGuard mg(mtx());

  auto iter = cSet().find(SetElem(obj, d, false));
  assert(iter != cSet().cend());

  iter = cSet().find(SetElem(obj + d->_instanceSize, d, true));
  assert(iter != cSet().cend());
}

void
DebugTrackLive::within(TypeData *d, const char *obj)
{
  MtxGuard mg(mtx());

  auto iter = cSet().upper_bound(SetElem(obj, d, false));
  assert(iter != cSet().cend());
  assert(iter->end);
  assert((obj + d->_instanceSize) <= iter->bound);
  assert(iter != cSet().cbegin());
  --iter;
  assert(!iter->end);
  assert(obj >= iter->bound);
}

std::mutex &
DebugTrackLive::mtx()
{
  static std::mutex m;
  return m;
}
