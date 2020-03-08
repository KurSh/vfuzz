//===- FuzzerExtFunctions.h - Interface to external functions ---*- C++ -* ===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// Defines an interface to (possibly optional) functions.
//===----------------------------------------------------------------------===//

#pragma once

#include <cstddef>
#include <cstdint>

struct GF_ExternalFunctions {
  // Initialize function pointers. Functions that are not available will be set
  // to nullptr.  Do not call this constructor  before ``main()`` has been
  // entered.
  GF_ExternalFunctions();

#define EXT_FUNC(NAME, RETURN_TYPE, FUNC_SIG, WARN)                            \
  RETURN_TYPE(*NAME) FUNC_SIG = nullptr

#include <interface/extfunctions.def>

#undef EXT_FUNC
};
