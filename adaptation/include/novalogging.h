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

#ifndef __NOVA_LOGGING_H
#define __NOVA_LOGGING_H

// This file defines the logging interface used from the engine and available to 
// applications linking to it.

#include "NovaTypes.h"

// comment these out to disable debug printing
#define __DO_DEBUG 

// logging function prototypes
NOVA_IMPORT void NovaLogMessage( const char* fmt, ... ); 

#ifdef __DO_DEBUG
#if defined(GNU_MACROS)
 // GNU style macro vararg handling
#define LOG_DEBUG(msg) ::NovaLogMessage( msg )
#define LOG_DEBUG_F(fmt, s...) ::NovaLogMessage( fmt, s )
#elif defined(C9X_MACROS)
 // C9X style macro vararg handling
#define LOG_DEBUG(msg) ::NovaLogMessage( msg )
#define LOG_DEBUG_F(fmt, ...) ::NovaLogMessage( fmt, __VA_ARGS__ )
#else
#error "Must define either GNU_MACROS or C9X_MACROS!"
#endif
#else // __DO_DEBUG
#if defined(GNU_MACROS)
#define LOG_DEBUG(msg) 
#define LOG_DEBUG_F(fmt, ...) 
#else
#define LOG_DEBUG(msg) 
#define LOG_DEBUG_F(fmt, s...)
#endif
#endif //__DO_DEBUG

#endif // __NOVA_LOGGING_H
