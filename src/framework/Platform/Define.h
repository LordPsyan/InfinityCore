/*
 * This file is part of the OregonCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef OREGON_DEFINE_H
#define OREGON_DEFINE_H

#include <cstdint>

#include <sys/types.h>

#include <ace/ACE_export.h>

#include "Platform/CompilerDefs.h"

#define OREGON_LITTLEENDIAN 0
#define OREGON_BIGENDIAN    1

#if !defined(OREGON_ENDIAN)
#  if defined (ACE_BIG_ENDIAN)
#    define OREGON_ENDIAN OREGON_BIGENDIAN
#  else //ACE_BYTE_ORDER != ACE_BIG_ENDIAN
#    define OREGON_ENDIAN OREGON_LITTLEENDIAN
#  endif //ACE_BYTE_ORDER
#endif //OREGON_ENDIAN

#if PLATFORM == PLATFORM_WINDOWS
#  define OREGON_PATH_MAX MAX_PATH
#  ifndef DECLSPEC_NORETURN
#    define DECLSPEC_NORETURN __declspec(noreturn)
#  endif //DECLSPEC_NORETURN
#else //PLATFORM != PLATFORM_WINDOWS
#  define OREGON_PATH_MAX PATH_MAX
#  define DECLSPEC_NORETURN
#endif //PLATFORM

#if !defined(COREDEBUG)
#  define OREGON_INLINE inline
#else //COREDEBUG
#  if !defined(OREGON_DEBUG)
#    define OREGON_DEBUG
#  endif //OREGON_DEBUG
#  define OREGON_INLINE
#endif //!COREDEBUG

#if COMPILER == COMPILER_GNU
#  define ATTR_NORETURN __attribute__((noreturn))
#  define ATTR_PRINTF(F,V) __attribute__ ((format (printf, F, V)))
#else //COMPILER != COMPILER_GNU
#  define ATTR_NORETURN
#  define ATTR_PRINTF(F,V)
#endif //COMPILER == COMPILER_GNU

typedef std::int64_t int64;
typedef std::int32_t int32;
typedef std::int16_t int16;
typedef std::int8_t int8;
typedef std::uint64_t uint64;
typedef std::uint32_t uint32;
typedef std::uint16_t uint16;
typedef std::uint8_t uint8;

typedef std::uint64_t OBJECT_HANDLE;

#endif //OREGON_DEFINE_H

