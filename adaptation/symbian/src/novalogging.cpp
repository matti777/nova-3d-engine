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

#include <e32base.h>
#include <e32debug.h>

#include "NovaTypes.h"
#include "novalogging.h"

NOVA_EXPORT void NovaLogMessage( const char* fmt, ... )
    {
    VA_LIST va_list;
    VA_START(va_list, fmt);
    
    // wrap it in a TPtrC8
    TPtrC8 fmt8( (const TUint8*)fmt );
    TBuf8<1024> buf8;
    buf8.Format( fmt8, va_list );
    
    // log it to RDebug
    RDebug::RawPrint( buf8 );
    
    VA_END(va_list);
    }

