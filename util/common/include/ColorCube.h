/*
 *  $Id: ColorCube.h 17 2009-09-02 09:03:47Z matti $
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

#ifndef __COLORCUBE_H
#define __COLORCUBE_H

// FILE INFO
// This is a simple cube object with vertex colors. This code can be used for
// quickly plugging a simple object into a Nova3D scene graph for testing
// purposes, or just as an example of how to use the APIs.

#include "Shape.h"
#include "NovaTypes.h"
#include "Display.h"

namespace nova3d {

// "radius" of the cube (distance from the center of the cube
// to the center of any side)
const real_64 DefaultCubeRadius = 1.0;

// geometry complexity
const int_32 NumberOfCoordinates = 8;
const int_32 NumberOfPolygons = 12;

/**
 * A simple colored cube for testing out the polygon engine.<p />
 *
 * @author Matti Dahlbom
 * @version $Name:  $, $Revision: 17 $
 */
class ColorCube : public Shape 
    {
    public: // Constructors and destructor
        NOVA_IMPORT ColorCube( NovaPixelFormat pixelFormat, 
			       real_64 radius = DefaultCubeRadius );
        NOVA_IMPORT virtual ~ColorCube();

    protected: // New methods
        /** Call this method to construct the geometry */
	virtual int Create( real_64 radius = DefaultCubeRadius );
        
    private: // Data
    };

}; // namespace

#endif
