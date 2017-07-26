/** @file

  A class template that provides "compiled-in" data breakpoints.

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

/*
This header is for executing with a breakpoint debugger only.  You replace the (member or non-member) variable defintion:

Type inst;

with:

Wrap<Type> inst;

(or

WrapP<Type> inst;

if 'Type' is a pointer type.)

and then set a break point on WrapImpl::read/write/readTarget/writeTarget() to see accesses.

In some cases, the Wrap<> conversion operators to will not be used implicitly, and you must all the cz000() or z000()
member functions.  The names of these members were chosen so they can easily be located by grepping for z000.
*/

#if !defined(WRAPDBG_H_)
#define WRAPDBG_H_

#include <utility>

namespace WrapImpl
{

inline void read() { }
inline void readTarget() { }
inline void write() { }
inline void writeTarget() { }

/*
break WrapImpl::read
break WrapImpl::readTarget
break WrapImpl::write
break WrapImpl::writeTarget
*/

// Call function fp through a volatile pointer in order to block the calls from being optimized out.

inline void callNoOpt(void (* volatile fp)())
{
  fp();
}

} // end WrapImpl namespace

template <typename T>
class Wrap
{
protected:
  T _data;

public:
  template <typename ... Args>
  Wrap(Args && ... args) : _data(std::forward<Args>(args)...) { WrapImpl::callNoOpt(WrapImpl::write); }

  // To avoid Meta-template hell, this only works with copyable types.

  Wrap(const Wrap &that) : _data(that._data) { WrapImpl::callNoOpt(WrapImpl::write); }
  Wrap & operator = (const Wrap &that) { WrapImpl::callNoOpt(WrapImpl::write); _data = that._data; return *this; }

  Wrap & operator = (const T &t) { WrapImpl::callNoOpt(WrapImpl::write); _data = t; return *this; }

  const T & cz000() const { WrapImpl::callNoOpt(WrapImpl::read); return _data; }

  operator const T & () const { WrapImpl::callNoOpt(WrapImpl::read); return _data; }

  T & z000() { WrapImpl::callNoOpt(WrapImpl::write); return _data; }

  operator T & () { WrapImpl::callNoOpt(WrapImpl::write); return _data; }

  ~Wrap() { WrapImpl::callNoOpt(WrapImpl::write); }
};

template <typename T>
class WrapP;

template <typename T>
class WrapP<T *> : public Wrap<T *>
{
private:
  using Base = Wrap<T *>;

public:

  WrapP(T *p = nullptr) : Base(p) { }

  WrapP(const WrapP &that) : Base(that._data) { }

  WrapP & operator = (const WrapP &that) { * static_cast<Base *>(this) = that._data; return *this; }

  WrapP & operator = (T *t) { * static_cast<Base *>(this) = t; return *this; }

  operator const T * () const { WrapImpl::callNoOpt(WrapImpl::readTarget); return this->_data; }

  operator T * () const { WrapImpl::callNoOpt(WrapImpl::writeTarget); return this->_data; }

  const T * operator -> () const { WrapImpl::callNoOpt(WrapImpl::readTarget); return this->_data; }

  T * operator -> () { WrapImpl::callNoOpt(WrapImpl::writeTarget); return this->_data; }
};

#endif // Include once.
