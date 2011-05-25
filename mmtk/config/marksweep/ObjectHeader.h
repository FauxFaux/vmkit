//===----- ObjectHeader.h - Macros for describing an object header --------===//
//
//                          The VMKit project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef VMKIT_OBJECT_HEADER_H
#define VMKIT_OBJECT_HEADER_H

#include <stdint.h>

namespace vmkit {
  static const uint32_t GCBits = 8;
  static const bool MovesObject = false;
}

#endif // VMKIT_OBJECT_HEADER_H
