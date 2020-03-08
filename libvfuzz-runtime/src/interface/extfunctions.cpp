//===- FuzzerExtFunctionsWeak.cpp - Interface to external functions -------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// Implementation for Linux. This relies on the linker's support for weak
// symbols. We don't use this approach on Apple platforms because it requires
// clients of LibFuzzer to pass ``-U _<symbol_name>`` to the linker to allow
// weak symbols to be undefined. That is a complication we don't want to expose
// to clients right now.
//===----------------------------------------------------------------------===//

#include <interface/extfunctions.h>

extern "C" {
// Declare these symbols as weak to allow them to be optionally defined.
#define EXT_FUNC(NAME, RETURN_TYPE, FUNC_SIG, WARN)                            \
  __attribute__((weak)) RETURN_TYPE NAME FUNC_SIG

#include <interface/extfunctions.def>

#undef EXT_FUNC
}

GF_ExternalFunctions::GF_ExternalFunctions() {
#define EXT_FUNC(NAME, RETURN_TYPE, FUNC_SIG, WARN)                            \
  this->NAME = ::NAME;                                                         \

#include <interface/extfunctions.def>

#undef EXT_FUNC
}
