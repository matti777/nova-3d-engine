/*
 *  $Id: NovaErrors.h 23 2009-09-10 13:57:16Z matti $
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

#ifndef __NOVAERR_H
#define __NOVAERR_H

// FILE INFO
// This file holds the error codes used throughout the Nova3d code and APIs.
// (Public API)

namespace nova3d {

// error codes for nova3d
const int NovaErrNone = 0;
const int NovaErrNotFound = -1;
const int NovaErrOutOfBounds = -2;
const int NovaErrNoMemory = -3; 
const int NovaErrOverflow = -4;
const int NovaErrUnderrun = -5;
const int NovaErrAlreadySet = -6;
const int NovaErrNotSet = -7;
const int NovaErrLiveSceneGraph = -8;
const int NovaErrInvalidArgument = -9;
const int NovaErrNotInitialized = -10;
const int NovaErrAlreadyInitialized = -11;
const int NovaErrTextureTooLarge = -12;
const int NovaErrInvalidPixelFormat = -13;
const int NovaErrNoVertexNormals = -14;

// indicates the texture dimensions werent powers of 2
const int NovaErrTextureDimensionInvalid = -13; 

}; // namespace

#endif
