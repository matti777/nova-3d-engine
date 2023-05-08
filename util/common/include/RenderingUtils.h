/*
 *  $Id: RenderingUtils.h 21 2009-09-08 11:12:30Z matti $
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

#ifndef __RENDERING_UTILS_H
#define __RENDERING_UTILS_H

#include "NovaTypes.h"
#include "Display.h"
#include "FixedPoint.h"

namespace nova3d {

/**
 * Orders the three vertex pointers by the y coordinate of the corresponding
 * screen coordinates, so that v1->m_y < v2->m_y < v3->m_y 
 */
void SelectVertexOrder( ScreenPolygon* face, 
                        ScreenVertex** v1, 
                        ScreenVertex** v2, 
                        ScreenVertex** v3 );

/**
 * Calculates gradients that are constant over a polygon.
 *
 * @param vertex1 
 * @param vertex2
 * @param vertex3 
 * @param dudx place to store the texture U gradient
 * @param dvdx place to store the texture V gradient 
 * @param dzdx place to store the Z gradient 
 */
void CalculatePolygonGradients( const ScreenVertex& vertex1, 
                                const ScreenVertex& vertex2, 
                                const ScreenVertex& vertex3, 
                                int_32& dudx, int_32& dvdx, 
				int_32& dzdx, int_32& didx );

/**
 * Checks whether the long edge of the polygon (triangle) is the left side edge
 * of the polygon by comparing the X coordinate of the middle vertex to the
 * X coordinate of the long edge at the Y of the middle vertex.<P>
 * 
 * The method also writes back values used to calculate things for the long
 * edge so they need not be recalculated.
 */
bool IsLongOnLeft( const ScreenVertex& vertex1, 
		   const ScreenVertex& vertex2, 
		   const ScreenVertex& vertex3, 
		   int& longInvLen, int& longX, int& longDxdy );

/**
 * Calculates 1/z, u/z, v/z for a screen vertex.
 */
void CalculateInverses( ScreenVertex& vertex );

}; // namespace

#endif

