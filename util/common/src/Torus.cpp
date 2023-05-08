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

#include <math.h>

#include "Torus.h"
#include "NovaErrors.h"
#include "novalogging.h"

namespace nova3d {

NOVA_EXPORT Torus::Torus( NovaPixelFormat pixelFormat, 
			  real_64 torusRadius, 
			  real_64 bodyRadius, 
			  int torusSections, 
			  int bodySections )
    : Shape( pixelFormat )
{
    int res = Create( torusRadius, bodyRadius, torusSections, bodySections );
    if ( res != NovaErrNone )
    {
	//##TODO## dedicated exception instead
	throw res;
    }
}

NOVA_EXPORT Torus::~Torus()
{
}
    
int Torus::Create( real_64 torusRadius, 
		   real_64 bodyRadius, 
		   int torusSections, 
		   int bodySections)
{
    // create the circle of points in XY plane
    real_64* points = new real_64[bodySections * 3];
    real_64* point = points;
    real_64 angle = 0.0;
    real_64 angleIncRadians = (2 * M_PI) / bodySections;
    
    for ( int i = 0; i < bodySections; i++ )
    {
	*point++ = bodyRadius * cos( angle );
	*point++ = bodyRadius * sin( angle );
	*point++ = 0.0;
	angle += angleIncRadians;
    }

    // now create the actual coordinates by rotating the circle 360 deg
    // around the Y axis to form the torus
    int numCoordinates = torusSections * bodySections;
    real_64* coordinates = new real_64[numCoordinates * 3];
    real_64* coordinate = coordinates;
    angle = M_PI / 2.0; // start from angle = 90 deg
    angleIncRadians = (2 * M_PI) / torusSections; // rotate counterclockwise
    
    for ( int i = 0; i < torusSections; i++ )
    {
	for ( int j = 0; j < bodySections; j++ )
	{
	    *coordinate++ =  (torusRadius + points[j * 3]) * cos( angle );
	    *coordinate++ = points[j * 3 + 1];
	    *coordinate++ =  (torusRadius + points[j * 3]) * sin( angle );
	}
	angle += angleIncRadians;
	if ( angle < 0.0 )
	{
	    angle += 2 * M_PI; 
	}
    }

    // create the polygons 
    int numPolygons = 2 * numCoordinates;
    uint_32* vertices = new uint_32[numPolygons * 3];
    uint_32* colors = new uint_32[numPolygons * 3];
    uint_32* vertex = vertices;
    uint_32* color = colors;
    
    for ( int i = 0; i < torusSections; i++ )
    {
	for ( int j = 0; j < bodySections; j++ )
	{
	    int ptIndex = i * bodySections + j;

	    int nextBodyIndex = ptIndex + 1;
	    int nextTorusIndex = ptIndex + bodySections;
	    int nextTorusIndexPlusOne = ptIndex + bodySections + 1;

	    if ( i == (torusSections - 1) ) 
	    {
		nextTorusIndex = j;
		nextTorusIndexPlusOne = j + 1;
	    }

	    if ( j == (bodySections - 1) ) 
	    {
		nextTorusIndexPlusOne -= bodySections;
		nextBodyIndex -= bodySections;
	    }

	    *vertex++ = ptIndex; // first polygon
	    *vertex++ = nextTorusIndex;
	    *vertex++ = nextTorusIndexPlusOne;
	    *vertex++ = ptIndex; // second polygon
	    *vertex++ = nextTorusIndexPlusOne;
	    *vertex++ = nextBodyIndex;

	    uint_32 color1 = nova3d::CreateColor( m_pixelFormat, 
						  i * 255 / torusSections, 
						  0, 
						  0 );	    
    	    *color++ = color1;
    	    *color++ = color1;
    	    *color++ = color1;
	    uint_32 color2 = nova3d::CreateColor( m_pixelFormat, 
						  0, 
						  0, 
						  (torusSections - i) * 255 / 
						  torusSections );    
    	    *color++ = color2;
    	    *color++ = color2;
    	    *color++ = color2;
	}
    }

    // call base class to initialize the internal geometry
    int ret = CreateGeometry( numCoordinates, numPolygons, 
			      coordinates, vertices );
    if ( ret == NovaErrNone )
    {
	// call base class to initialize the vertex colors
	ret = SetVertexColors( colors );
    }

    // cleanup
    delete colors;
    delete vertices;
    delete coordinates;
    delete points;
    
    return ret;
}

}; // namespace

