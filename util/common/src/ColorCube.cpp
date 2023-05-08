/*
 *  $Id: ColorCube.cpp 17 2009-09-02 09:03:47Z matti $
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

#include "ColorCube.h"

namespace nova3d {

NOVA_EXPORT ColorCube::ColorCube( NovaPixelFormat pixelFormat, 
				  real_64 radius )
    : Shape( pixelFormat )
{
    int ret = Create( radius );
    if ( ret != NovaErrNone )
    {
	throw ret;
    }
}

NOVA_EXPORT ColorCube::~ColorCube()
{
}

/**
 * Creates the vertex, polygon and color lists for the object. This is
 * how the object geometry is organized:<p />
 * <pre>
 *
 *      1/-------------/2
 *     /  |          / |
 *  0/-------------/3  |
 *   |    |        |   |
 *   |    |        |   |
 *   |    |        |   |
 *   |    |        |   |
 *   |   5|        |   |6
 *   |   /---------|---/
 *   | /           | / 
 *  4/-------------/7  
 * </pre>
 */
int ColorCube::Create( real_64 radius )
{
    real_64 coordinates[NumberOfCoordinates * 3] = {
	-1.0, 1.0, -1.0,
	-1.0, 1.0, 1.0, 
	1.0, 1.0, 1.0, 
	1.0, 1.0, -1.0, 
	-1.0, -1.0, -1.0,
	-1.0, -1.0, 1.0, 
	1.0, -1.0, 1.0, 
	1.0, -1.0, -1.0 
    };

    // scale by the radius
    for ( int i = 0; i < (3 * NumberOfCoordinates); i++ ) 
    {
        coordinates[i] *= radius;
    }

    uint_32 vertices[3 * NumberOfPolygons] = {   
	0, 4, 3, 
	3, 4, 7, 
	1, 5, 0, 
	0, 5, 4, 
	2, 6, 1,
	1, 6, 5,
	3, 7, 2,
	2, 7, 6,
	1, 0, 2, 
	2, 0, 3, 
	7, 4, 6,
	6, 4, 5 
    };

    // create colors
    uint_32 red = nova3d::CreateColor( m_pixelFormat, 255, 0, 0 ); 
    uint_32 green = nova3d::CreateColor( m_pixelFormat, 0, 255, 0 ); 
    uint_32 blue = nova3d::CreateColor( m_pixelFormat, 0, 0, 255 ); 
    uint_32 yellow = nova3d::CreateColor( m_pixelFormat, 250, 250, 50 ); 
    uint_32 grey = nova3d::CreateColor( m_pixelFormat, 100, 100, 100 ); 
    uint_32 violet = nova3d::CreateColor( m_pixelFormat, 250, 0, 250 );
    
    uint_32 colors[NumberOfPolygons * 3];

    // polygons 0+1 
    colors[0] = colors[1] = colors[2] = colors[3] = colors[4] = colors[5] = 
	red; 
    // polygons 2+3 
    colors[6] = colors[7] = colors[8] = colors[9] = colors[10] = colors[11] = 
        green; 
    // polygons 4+5 
    colors[12] = colors[13] = colors[14] = colors[15] = colors[16] = 
        colors[17] = blue; 
    // polygons 6+7 
    colors[18] = colors[19] = colors[20] = colors[21] = colors[22] = 
        colors[23] = yellow; 
    // polygons 8+9 
    colors[24] = colors[25] = colors[26] = colors[27] = colors[28] = 
        colors[29] = grey; 
    // polygons 10+11 
    colors[30] = colors[31] = colors[32] = colors[33] = colors[34] = 
        colors[35] = violet;
    
    // call base class to initialize the internal geometry
    int ret = CreateGeometry( NumberOfCoordinates, NumberOfPolygons, 
			      coordinates, vertices );
    if ( ret != NovaErrNone )
    {
        return ret;
    }
    
    // call base class to initialize the vertex colors
    ret = SetVertexColors( colors );
    
    return ret;
}

}; // namespace
