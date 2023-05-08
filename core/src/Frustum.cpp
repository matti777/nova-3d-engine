/*
 *  $Id: Frustum.cpp 17 2009-09-02 09:03:47Z matti $
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

#include "Frustum.h"

namespace nova3d {

Frustum::Frustum()
    {
    //##TODO##
    }

Frustum::~Frustum()
    {
    //##TODO##
    }

void Frustum::Calculate( real_64 fov, const RenderingCanvas& canvas, 
                         int_32 nearClipZ )
    {
    //##TODO##
    }

int_32 Frustum::EvaluateClipping( const BoundingSphere& aBoundingSphere )
    {
    //##TODO##
    return 0;
    }

void Frustum::ClipTextured( int_32 clipPlanes, List<ScreenVertex>& vertexList,  
                            const ScreenVertex& vertex1,
                            const ScreenVertex& vertex2,
                            const ScreenVertex& vertex3 )
    {
    //##TODO##
    }

void Frustum::EvalPlane( int_32& maskBits,
                         int_32 planeMask, 
                         const PlaneEquation& plane,
                         const BoundingSphere& boundingSphere )
    {
    //##TODO##
    }

void Frustum::ClipLineAgainstPlane( const PlaneEquation& plane,
                                    const ScreenVertex& vertex1,
                                    const ScreenVertex& vertex2,
                                    List<ScreenVertex>& destList )
    {
    //##TODO##
    }

void Frustum::ClipPolygonAgainstPlane( const PlaneEquation& plane,
                                       List<ScreenVertex>** srcList,
                                       List<ScreenVertex>** dstList )
    {
    //##TODO##
    }

}; // namespace
