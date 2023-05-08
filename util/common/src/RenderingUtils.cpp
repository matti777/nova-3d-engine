/*
 *  $Id: RenderingUtils.cpp 21 2009-09-08 11:12:30Z matti $
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

#include "RenderingUtils.h"
#include "novalogging.h"

namespace nova3d {

void SelectVertexOrder( ScreenPolygon* face, 
                        ScreenVertex** v1, 
                        ScreenVertex** v2, 
                        ScreenVertex** v3 )
{
    int y1, y2, y3;

    y1 = (int)face->m_v1.m_y;
    y2 = (int)face->m_v2.m_y;
    y3 = (int)face->m_v3.m_y;

    if( y1 < y2 ) 
    {
        if( y1 < y3 ) 
	{
            // y1 smallest
            *v1 = &(face->m_v1);
            if( y2 < y3 ) 
	    {
                // y1 < y2 < y3
                *v2 = &(face->m_v2);
                *v3 = &(face->m_v3);
            } 
	    else 
	    {
                // y1 < y3 < y2
                *v2 = &(face->m_v3);
                *v3 = &(face->m_v2);
            }
        } 
	else 
	{
            // y3 smallest; y3 < y1 < y2
            *v1 = &(face->m_v3);
            *v2 = &(face->m_v1);
            *v3 = &(face->m_v2);
        }
    } 
    else
    {
        if( y2 < y3 ) 
	{
            // y2 smallest
            *v1 = &(face->m_v2);
            if( y1 < y3 ) 
	    {
                // y2 < y1 < y3
                *v2 = &(face->m_v1);
                *v3 = &(face->m_v3);
            } 
	    else
	    {
                // y2 < y3 < y1
                *v2 = &(face->m_v3);
                *v3 = &(face->m_v1);
            }
        } 
	else
	{
            // y3 smallest; y3 < y2 < y1
            *v1 = &(face->m_v3);
            *v2 = &(face->m_v2);
            *v3 = &(face->m_v1);
        }
    }
}

void CalculateInverses( ScreenVertex& vertex )
{
    vertex.m_z = (int_32)(MaxUint32 / (uint_32)vertex.m_z); 
    vertex.m_textureCoordinates.m_u = 
	::FixedLargeMul( vertex.m_textureCoordinates.m_u, vertex.m_z );
    vertex.m_textureCoordinates.m_v = 
	::FixedLargeMul( vertex.m_textureCoordinates.m_v, vertex.m_z );
}

void CalculatePolygonGradients( const ScreenVertex& vertex1, 
                                const ScreenVertex& vertex2, 
                                const ScreenVertex& vertex3, 
                                int_32& dudx, int_32& dvdx, 
				int_32& dzdx, int_32& didx )
{
    real_64 product = 
	((real_64)(vertex1.m_x - vertex3.m_x) *
	 (real_64)(vertex2.m_y - vertex3.m_y)) -
	((real_64)(vertex2.m_x - vertex3.m_x) *
	 (real_64)(vertex1.m_y - vertex3.m_y));
    product /= 65536.0;

    real_64 inverse = 1.0 / product;
    
    real_64 ldudx = 
	((real_64)(vertex1.m_textureCoordinates.m_u - 
		  vertex3.m_textureCoordinates.m_u) *
	 (real_64)(vertex2.m_y - vertex3.m_y)) -
	((real_64)(vertex2.m_textureCoordinates.m_u - 
		  vertex3.m_textureCoordinates.m_u) *
	 (real_64)(vertex1.m_y - vertex3.m_y));
    dudx = (int_32)((ldudx * inverse));

    real_64 ldvdx =
	((real_64)(vertex1.m_textureCoordinates.m_v - 
		  vertex3.m_textureCoordinates.m_v) *
	 (real_64)(vertex2.m_y - vertex3.m_y)) -
	((real_64)(vertex2.m_textureCoordinates.m_v - 
		  vertex3.m_textureCoordinates.m_v) *
	 (real_64)(vertex1.m_y - vertex3.m_y));
    dvdx = (int_32)((ldvdx * inverse));

    real_64 ldzdx = 
	((real_64)(vertex1.m_z - vertex3.m_z) *
	 (real_64)(vertex2.m_y - vertex3.m_y)) -
	((real_64)(vertex2.m_z - vertex3.m_z) *
	 (real_64)(vertex1.m_y - vertex3.m_y));
    dzdx = (int_32)((ldzdx * inverse));

    if ( didx != -1 )
    {
	real_64 ldidx = 
	    ((real_64)(vertex1.m_textureCoordinates.m_intensity - 
		       vertex3.m_textureCoordinates.m_intensity) *
	     (real_64)(vertex2.m_y - vertex3.m_y)) -
	    ((real_64)(vertex2.m_textureCoordinates.m_intensity - 
		       vertex3.m_textureCoordinates.m_intensity) *
	     (real_64)(vertex1.m_y - vertex3.m_y));
	didx = (int_32)((ldidx * inverse));
    }

    //LOG_DEBUG_F("dudx = %d, dvdx = %d, dzdx = %d", dudx, dvdx, dzdx);
}

bool IsLongOnLeft( const ScreenVertex& vertex1, 
		   const ScreenVertex& vertex2, 
		   const ScreenVertex& vertex3, 
		   int& longInvLen, int& longX, int& longDxdy )
{
    // setup
    longInvLen = (int_32)(MaxUint32 / (uint_32)(vertex3.m_y - vertex1.m_y));
    longX = vertex1.m_x;
    longDxdy = ::FixedLargeMul( (vertex3.m_x - longX), longInvLen );

    // find out which side of the long edge (v1-v3) v2 resides on
    int_32 middleX = longX + ::FixedLargeMul( (vertex2.m_y - vertex1.m_y), 
					      longDxdy );

    return (middleX < vertex2.m_x);
}

}; // namespace
