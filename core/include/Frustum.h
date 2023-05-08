/*
 *  $Id: Frustum.h 17 2009-09-02 09:03:47Z matti $
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

#ifndef __FRUSTUM_H
#define __FRUSTUM_H

// FILE INFO
// This file describes the View Frustum that is used for 3D clipping
// of geometry.

#include "List.h"
#include "NovaTypes.h"
#include "Display.h"
#include "VectorMath.h"

namespace nova3d {

// mask values for marking geometry being in/outside of the clip planes
const int_32 FrustumNearClipMask   = 0x00000001;
const int_32 FrustumRightClipMask  = 0x00000002;
const int_32 FrustumTopClipMask    = 0x00000004;
const int_32 FrustumLeftClipMask   = 0x00000008;
const int_32 FrustumBottomClipMask = 0x00000010;
const int_32 FrustumOutsideMask    = 0xffff0000;

/** 
 * Represents a view frustum with 3D geometry clipping functionality.
 * Bounding spheres are used for faster rejection.<p />
 *
 * @author Matti Dahlbom
 * @version $Name:  $, $Revision: 17 $
 */
class Frustum
{
 public: // Constructors and destructor
    Frustum();
    ~Frustum();

 public: // New methods
    /**
     * (Re)calculates the view frustum. Must be called when either of the
     * three values (fov, canvas size, near clip distance) changes.
     */
    void Calculate( real_64 fov, const RenderingCanvas& canvas, 
		    int_32 nearClipZ );

    /**
     * Evaluates which clipping planes the (object with) given
     * bounding sphere should be clipped against. This can be used for 
     * fast rejection of clipping when the object is completely outside
     * the view frustum (KOutsideFrustumMask is set) or is completely 
     * inside the view frustum (the bitmask returned is 0).
     */
    int_32 EvaluateClipping( const BoundingSphere& aBoundingSphere );

    /**
     * Clips a polygon defined by the given 3 vertices. Texture 
     * coordinates and lighting intensities are clipped as well.
     * The polygons produced by the clipping operation are stored
     * into vertexList parameter. The caller must handle the 
     * triangulation of the result polygon.<p />
     *
     * Note that while the datatype used is <code>TScreenVertex</code>,
     * the coordinates still represent unprojected 3D vertices.<p />
     *
     * @param aClipPlanes the clip plane mask as returned by
     * <code>EvaluateClipping()</code>.
     */
    void ClipTextured( int_32 clipPlanes, 
		       List<ScreenVertex>& vertexList,  
		       const ScreenVertex& vertex1,
		       const ScreenVertex& vertex2,
		       const ScreenVertex& vertex3 );

 private: // new methods
    void EvalPlane( int_32& maskBits,
		    int_32 planeMask, 
		    const PlaneEquation& plane,
		    const BoundingSphere& boundingSphere );

    void ClipLineAgainstPlane( const PlaneEquation& plane,
			       const ScreenVertex& vertex1,
			       const ScreenVertex& vertex2,
			       List<ScreenVertex>& destList );
        
    /**
     * Clips the polygon represented by the vertex list in 
     * <code>*srcList</code>
     * against the plane defined by <code>plane</code>, storing the result 
     * polygon to <code>*dstList</code>. 
     * NOTE: the implementation then swaps the contents
     * of the two pointers.
     */
    void ClipPolygonAgainstPlane( const PlaneEquation& plane,
				  List<ScreenVertex>** srcList,
				  List<ScreenVertex>** dstList );

 private: // Data
    PlaneEquation m_nearPlane;
    PlaneEquation m_rightPlane;
    PlaneEquation m_topPlane;
    PlaneEquation m_leftPlane;
    PlaneEquation m_bottomPlane;

    // clipped polygons are represented by lists of screen vertices 
    // (although these still are 3D vertices) contained in these lists. 
    // a pointer is held in the clipping methods to point to the 
    // up-to-date list
    List<ScreenVertex> m_vertexList1;
    List<ScreenVertex> m_vertexList2;
};

}; // namespace

#endif
