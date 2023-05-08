/*
 *  $Id: Camera.cpp,v 1.42 2005/03/20 18:26:48 matti Exp $
 *
 *  Nova 3D Engine - A portable object oriented, scene graph based, 
 *  lightweight real-time 3D software rendering framework. 
 *  Copyright (C) 2001-2009 Matti Dahlbom
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Contact: Matti Dahlbom <matti at 777-team dot org>
 */

#ifndef __NOVA_TYPES_H
#define __NOVA_TYPES_H

// FILE INFO
// This file contains the data type mappings from nova3d types to 
// platform dependent types.
// When porting to a new platform, a section in the ifdef needs to be added.
// This file will be #include'd by every source file in nova3d.

// defines the no-throw version of new 
//##TODO## is this possible without std::nothrow ? maybe reimplement self?
#define NO_THROW 

#if defined(NOVA_SYMBIAN32)
//////////////////////////////////////
// Symbian32 types 
//////////////////////////////////////

#include <e32base.h>

// basic types
typedef TInt32 int_32;
typedef TUint32 uint_32;
typedef TInt8 int_8;
typedef TUint8 uint_8;
typedef TReal real_64;
typedef TInt64 int_64;
typedef TInt64 uint_64; // guess there is no native unsigned one

// library import/export 
#define NOVA_IMPORT IMPORT_C
#define NOVA_EXPORT EXPORT_C

// c9x/gnu style macro ellipsis syntax
#ifdef __WINSCW__
#define C9X_MACROS
#else
#define GNU_MACROS
#endif //__WINSCW__

#elif defined(NOVA_LINUX32)
//////////////////////////////////////
// Linux 32bit types 
//////////////////////////////////////
// basic types
typedef int int_32;
typedef unsigned int uint_32;
typedef char int_8;
typedef unsigned char uint_8;
typedef double real_64;
typedef long long int_64;
typedef unsigned long long uint_64;

// library import/export 
#define NOVA_IMPORT 
#define NOVA_EXPORT 

#define GNU_MACROS

#else
#error "Unsupported platform!"
#endif

// largest uint_32 possible
const uint_32 MaxUint32 =0xffffffffu;

#endif

