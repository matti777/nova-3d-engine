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

#include <stdlib.h>
#include <math.h>

#include "Normalizer.h"
#include "VectorMath.h"
#include "Shape.h"
#include "NovaErrors.h"
#include "novalogging.h"

namespace nova3d {

int Normalizer::SmoothenVertex( const List<uint_32>& vertexNormalIndices,
				Vector* normals, real_64 angle )
{
    //##TODO## try to use int here not real
    //    real_64 x = 0.0, y = 0.0, z = 0.0;
    int_32 x = 0, y = 0, z = 0;
    int numIndices = vertexNormalIndices.Count();

    for ( int i = 0; i < numIndices; i++ )
    {
	uint_32 vertexNormalIndex;
	if ( vertexNormalIndices.Get( i, vertexNormalIndex ) == NovaErrNone )
	{
	    Vector* normal = (normals + vertexNormalIndex);
// 	    x += normal->GetRealX();
// 	    y += normal->GetRealY();
// 	    z += normal->GetRealZ();
	    x += normal->GetFixedX();
	    y += normal->GetFixedY();
	    z += normal->GetFixedZ();
	}
    }

    // create a vector out of an average of the normals at the vertex
    Vector avg;
    avg.SetFixed( x, y, z );
    
    // if the vector is null, then the vectors definitely are not smoothable
    if ( avg.IsNull() ) 
    {
	LOG_DEBUG("Normalizer::SmoothenVertex() null average vector!");
        return NovaErrNotSet;
    }

    // make sure the vector has enough precision
    avg.CheckPrecision();

    // normalize the vector to unit length
    avg.Normalize();

    real_64 angleInRadians = angle * (M_PI / 180.0);

    // compare each normal to this avg. normal; those with angle smaller than
    // angle will be "smoothed", ie. they will be assigned the avg. vector
    for ( int i = 0; i < numIndices; i++ )
    {
	uint_32 vertexNormalIndex;
	if ( vertexNormalIndices.Get( i, vertexNormalIndex ) == NovaErrNone )
	{
	    Vector* normal = (normals + vertexNormalIndex);
	    
	    if ( normal->IsNull() || 
		 (normal->AngleBetweenRadReal( avg ) <= angleInRadians) )
	    {
		normal->Set( avg );
	    }
	}
    }

    return NovaErrNone;
}

NOVA_EXPORT int Normalizer::SmoothenVertexNormals( Shape& shape, real_64 angle )
{
    LOG_DEBUG("Normalizer::SmoothenVertexNormals()");

    int numCoordinates = shape.GetNumCoordinates();
    int numPolygons;
    const uint_32* vertices = shape.GetPolygons( numPolygons );
    int numNormals;
    Vector* normals = const_cast<Vector*>(shape.GetVertexNormals( numNormals ));
    const uint_32* normalIndices = shape.GetVertexNormalIndices();

    List<uint_32> list( 10 );

    // iterate over every vertex in the shape
    for ( uint_32 i = 0; i < (uint_32)numCoordinates; i++ )
    {
	const uint_32* vertex = vertices;
	const uint_32* normalIndex = normalIndices;

	// iterate over every polygon in the shape and find those that share
	// the current vertex
	for ( int j = 0; j < numPolygons; j++ )
	{
	    uint_32 v0 = *vertex++;
	    uint_32 v1 = *vertex++;
	    uint_32 v2 = *vertex++;
	    uint_32 normalIndex0 = *normalIndex++;
	    uint_32 normalIndex1 = *normalIndex++;
	    uint_32 normalIndex2 = *normalIndex++;

	    if ( v0 == i )
	    {
		list.Append( normalIndex0 );
	    }
	    else if ( v1 == i )
	    {
		list.Append( normalIndex1 );
	    }
	    else if ( v2 == i )
	    {
		list.Append( normalIndex2 );
	    }
	}

	// smoothen the normals at the vertex in question
	SmoothenVertex( list, normals, angle );

	// reset the list
	list.Reset();
    }

    return NovaErrNone;
}

NOVA_EXPORT int Normalizer::OptimizeVertexNormals( Shape& shape )
{
    // get existing vertex normal information
    int numOldNormals;
    const Vector* oldNormals = shape.GetVertexNormals( numOldNormals );
    int numPolygons = shape.GetNumPolygons();
    int numIndices = numPolygons * 3;

    // allocate memory for new vertex normal information
    Vector* vertexNormals = (Vector*)malloc( numPolygons * sizeof(Vector) );
    if ( vertexNormals == NULL )
    {
	return NovaErrNoMemory;
    }
    uint_32* vertexNormalIndices = 
	(uint_32*)malloc( numPolygons * 3 * sizeof(uint_32) );
    if ( vertexNormalIndices == NULL )
    {
	free( vertexNormals );
	return NovaErrNoMemory;
    }

    //
    const uint_32* srcIndex = shape.GetVertexNormalIndices();
    uint_32* destIndex = vertexNormalIndices;
    int numNewNormals = 0;

    for ( int i = 0; i < numIndices; i++ )
    {
	const Vector* normal = (oldNormals + (*srcIndex++));
	uint_32 newNormalIndex = 0;
	bool found = false;

	// try to look up the normal in the new normal list
	for ( int j = 0; j < numNewNormals; j++ )
	{
	    if ( *normal == *(vertexNormals + j) )
	    {
		newNormalIndex = j;
		found = true;
		break;
	    }
	}

	// if the normal wasnt found in the list, add it as a new entry
	if ( !found )
	{
	    newNormalIndex = numNewNormals++;
	    (vertexNormals + newNormalIndex)->Set( *normal );
	}

	// write new index information
	*destIndex++ = newNormalIndex;
    }

    LOG_DEBUG_F("Normalizer::OptimizeVertexNormals() optimized %d -> %d", 
		numOldNormals, numNewNormals);

    // set the optimized vertex information to the shape
    int ret = shape.SetVertexNormals( numNewNormals, vertexNormals, 
				      vertexNormalIndices );
    
    // clean up
    free( vertexNormals );
    free( vertexNormalIndices );

    return ret;
}

NOVA_EXPORT int Normalizer::CreateVertexNormals( Shape& shape )
{
    int numPolygons = shape.GetNumPolygons();
    int numNormals = numPolygons * 3;

    // allocate memory for vertex normals
    Vector* vertexNormals = (Vector*)malloc( numNormals * sizeof(Vector) );
    if ( vertexNormals == NULL )
    {
	return NovaErrNoMemory;
    }
    uint_32* vertexNormalIndices = 
	(uint_32*)malloc( numPolygons * 3 * sizeof(uint_32) );
    if ( vertexNormalIndices == NULL )
    {
	free( vertexNormals );
	return NovaErrNoMemory;
    }

    // create normals for all faces, making each vertex of the face
    // share a single normal, that of the polygon itself
    const PlaneEquation* planeEquation = shape.GetPlaneEquations();
    Vector* normal = vertexNormals;
    uint_32* normalIndex = vertexNormalIndices;

    uint_32 normalIndexSequence = 0;

    for ( int i = 0; i < numPolygons; i++ )
    {
	*normal++ = planeEquation->GetNormal();
	*normal++ = planeEquation->GetNormal();
	*normal++ = planeEquation->GetNormal();
	*normalIndex++ = normalIndexSequence++;
	*normalIndex++ = normalIndexSequence++;
	*normalIndex++ = normalIndexSequence++;
	planeEquation++; 
    }

    LOG_DEBUG_F("Normalizer::CreateVertexNormals() numNormals = %d", 
		numNormals);

    // set the vertex normals to the shape
    int ret = shape.SetVertexNormals( numNormals, vertexNormals, 
				      vertexNormalIndices );

    // clean up
    free( vertexNormals );
    free( vertexNormalIndices );

    return ret;
}

}; // namespace
