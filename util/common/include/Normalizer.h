/*
 *  $Id: Shape.cpp 19 2009-09-07 07:16:57Z matti $
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

#ifndef __NORMALIZER_H
#define __NORMALIZER_H

#include "NovaTypes.h"
#include "List.h"

namespace nova3d {

class Shape;
class Vector;

/**
 * Utility class for manipulating the vertex normals of a visual 
 * shape object.<p />
 * 
 * @author Matti Dahlbom
 * @version $Revision$
 */
class Normalizer 
{
 public: // New methods (Public API)
    // angle value for always smoothening (combining vertice normals)
    static const real_64 AlwaysSmoothenAngle = 180.0;
    
    /**
     * Smoothens the shape by combining vertex normals where faces share a
     * vertex where the angle between them is less than the given 
     * threshold value.<p />
     *
     * @return an error code or NovaErrNone if successful
     */
    NOVA_IMPORT static int 
	SmoothenVertexNormals( Shape& shape, 
			       real_64 angle = AlwaysSmoothenAngle );

    /** 
     * Optimizes the shape by combining identical vertex normals. 
     *
     * @return an error code or NovaErrNone if successful
     */
    NOVA_IMPORT static int OptimizeVertexNormals( Shape& shape );

    /** 
     * Creates vertex normals for shape by using the face normal for 
     * every vertex in the face as the vertex normal.
     *
     * @return an error code or NovaErrNone if successful
     */
    NOVA_IMPORT static int CreateVertexNormals( Shape& shape );

 private: // New methods
    static int SmoothenVertex( const List<uint_32>& vertexNormalIndices,
			       Vector* normals, real_64 angle );
    
 private: // Constructor - prevent instantiation
    Normalizer();
};

}; // namespace

#endif

