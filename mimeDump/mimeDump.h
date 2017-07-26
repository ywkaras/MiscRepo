/** @file

  Dump mime headers to stream.

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

#include <iostream>
#include <fstream>
#include <cstdio>

#include <MIME.h>

namespace
{

void
mimeDumpStr(std::ostream &os, const MIMEField &mf, const char *(MIMEField::*get_func)(int *length) const)
{
  int length;
  const char *data = (mf.*get_func)(&length);

  if (!data) {
    length = 0;
  }

  while (length--) {
    os.put(*(data++));
  }
}

} // end anonymous namespace

void
mimeDumpS(MIMEHdr *hdr, std::ostream &os)
{
  if (hdr) {
    MIMEFieldIter mfIter;
    const MIMEField *mfp = hdr->iter_get_first(&mfIter);

    while (mfp) {
      mimeDumpStr(os, *mfp, &MIMEField::name_get);
      os << ": ";
      mimeDumpStr(os, *mfp, &MIMEField::value_get);
      os << std::endl;

      mfp = hdr->iter_get_next(&mfIter);
    }
  }
}

void
mimeDump(MIMEHdr *hdr)
{
  mimeDumpS(hdr, std::cout);
}

void
mimeDumpF(MIMEHdr *hdr, const char *fileSpec)
{
  std::ofstream os(fileSpec);
  mimeDumpS(hdr, os);
  os.close();
}

void
mimeDumpT(MIMEHdr *hdr)
{
  mimeDumpF(hdr, "/tmp/mime");
}

void
mimeDumpTN(MIMEHdr *hdr, unsigned n)
{
  char fileSpec[50];
  sprintf(fileSpec, "/tmp/mime%u", n);
  mimeDumpF(hdr, fileSpec);
}
