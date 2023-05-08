/*
 *  $Id: Shape.cpp 23 2009-09-10 13:57:16Z matti $
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

//#include <string.h>

#include "Lights.h"
#include "Shape.h"
#include "Node.h"
#include "novalogging.h"
#include "NovaErrors.h"

namespace nova3d {

NOVA_EXPORT Shape::Shape( NovaPixelFormat pixelFormat )
    : m_numCoordinates( 0 ),
      m_coordinates( NULL ),
      m_transformedCoordinates( NULL ),
      m_numPolygons( 0 ),
      m_vertices( NULL ),
      m_pixelFormat( pixelFormat ),
      m_vertexColors( NULL ),
      m_textures( NULL ),
      m_textureCoordinates( NULL ),
      m_numVertexNormals( 0 ),
      m_vertexNormals( NULL ),
      m_transformedVertexNormals( NULL ),
      m_vertexNormalIndices( NULL ),
      m_isIlluminated( false ), 
      m_lightingIntensities( NULL ),
      m_distanceCache( NULL ), 
      m_planeEquations( NULL ),
      m_polygonInfos( NULL ),
      m_vertexInfos( NULL ), 
      m_boundingSphereRadius( -1 )
{
}

NOVA_EXPORT Shape::~Shape()
{
    DeallocateAll();
}

void Shape::DeallocateAll()
{
    m_numCoordinates = 0;
    m_numPolygons = 0;
    
    free( m_coordinates );
    free( m_transformedCoordinates );
    free( m_vertices );
    free( m_vertexColors );
    free( m_textures );
    free( m_textureCoordinates );
    free( m_vertexNormals );
    free( m_transformedVertexNormals );
    free( m_vertexNormalIndices );
    free( m_lightingIntensities );
    free( m_distanceCache );
    free( m_planeEquations );
    free( m_polygonInfos );
    free( m_vertexInfos );
}

NOVA_EXPORT int Shape::CreateGeometry( int numCoordinates, int numPolygons,
                                       real_64* coordinates, uint_32* vertices )
{
    LOG_DEBUG_F("Shape::CreateGeometry() #coords = %d, #polys = %d",
		numCoordinates, numPolygons);

    m_numCoordinates = numCoordinates;
    m_numPolygons = numPolygons;
    
    int ret = AllocateVertexList();
    if ( ret != NovaErrNone )
    {
        DeallocateAll();
        return ret;
    }
    
    ret = AllocateColorList();
    if ( ret != NovaErrNone )
    {
        DeallocateAll();
        return ret;
    }

    ret = AllocateCoordinateList();
    if ( ret != NovaErrNone )
    {
        DeallocateAll();
        return ret;
    }

    ret = AllocateLightingIntensities();
    if ( ret != NovaErrNone )
    {
        DeallocateAll();
        return ret;
    }

    // initialize the coordinate list (vectors) from the data
    const real_64 *coordinate = coordinates; 
    for ( int i = 0; i < m_numCoordinates; i++ ) 
    {
        real_64 x = *coordinate++;
        real_64 y = *coordinate++;
        real_64 z = *coordinate++;
        
        (m_coordinates + i)->SetReal( x, y, z );
    }
    
    // copy the polygon vertex list
    memcpy( m_vertices, vertices, 3 * m_numPolygons * sizeof(uint_32) );
    
    // the shape receives no lighting by default
    SetIlluminated( false );

    // calculate plane equations for all polygons
    for ( int i = 0; i < m_numPolygons; i++ )
    {
        CalculatePlaneEquation( i );
    }
    
    return NovaErrNone;
}

// NOTE: CreateGeometry() handles the cleanup if this method returns with error
int Shape::AllocateVertexList()
{
    if ( m_vertices != NULL )
    {
        return NovaErrAlreadyInitialized;
    }
    
    // allocate polygon list
    size_t size = 3 * m_numPolygons * sizeof(uint_32);
    m_vertices = (uint_32*)malloc( size );
    if ( m_vertices == NULL )
    {
        return NovaErrNoMemory;
    }
    memset( m_vertices, 0, size ); 

    // allocate polygon flag list
    m_polygonInfos = (uint_32*)malloc( m_numPolygons * sizeof(uint_32) );
    if ( m_polygonInfos == NULL )
    {
        return NovaErrNoMemory;
    }
    memset( m_polygonInfos, 0, m_numPolygons * sizeof(uint_32) );

    // allocate plane equation list
    m_planeEquations = 
        (PlaneEquation*)malloc( m_numPolygons * sizeof(PlaneEquation) );
    if ( m_planeEquations == NULL )
    {
        return NovaErrNoMemory;
    }
    memset( m_planeEquations, 0, m_numPolygons * sizeof(PlaneEquation) );
    
    return NovaErrNone;
}

// NOTE: CreateGeometry() handles the cleanup if this method returns with error
int Shape::AllocateColorList()
{
    if ( m_vertexColors != NULL )
    {
        return NovaErrAlreadyInitialized;
    }

    int numColors = m_numPolygons * 3;
    m_vertexColors = (uint_32*)malloc( numColors * sizeof(uint_32) );

    return (m_vertexColors == NULL) ? NovaErrNoMemory : NovaErrNone;
}

// NOTE: CreateGeometry() handles the cleanup if this method returns with error
int Shape::AllocateCoordinateList()
{
    if ( m_coordinates != NULL )
    {
        return NovaErrAlreadyInitialized;
    }
    
    // allocate coordinate list + transformed coordinate list
    m_coordinates = (Vector*)malloc( m_numCoordinates * sizeof(Vector) );
    if ( m_coordinates == NULL )
    {
        return NovaErrNoMemory;
    }
    m_transformedCoordinates = 
	(Vector*)malloc( m_numCoordinates * sizeof(Vector) );
    if ( m_transformedCoordinates == NULL )
    {
        return NovaErrNoMemory;
    }
    memset( m_coordinates, 0, m_numCoordinates * sizeof(Vector) );
    memset( m_transformedCoordinates, 0, m_numCoordinates * sizeof(Vector) );

    // allocate vertex info for each vertex
    m_vertexInfos = (uint_32*)malloc( m_numCoordinates * sizeof(uint_32) );
    if ( m_vertexInfos == NULL )
    {
        return NovaErrNoMemory;
    }
    memset( m_vertexInfos, 0, m_numCoordinates * sizeof(uint_32) );

    // allocate distance cache for lighting
    m_distanceCache = (int_32*)malloc( m_numCoordinates * sizeof(int_32) );
    if ( m_distanceCache == NULL )
    {
        return NovaErrNoMemory;
    }
    memset( m_distanceCache, 0, m_numCoordinates * sizeof(int_32) );

    return NovaErrNone;
}

// NOTE: CreateGeometry() handles the cleanup if this method returns with error
int Shape::AllocateLightingIntensities()
{
    if ( m_lightingIntensities != NULL )
    {
        return NovaErrAlreadyInitialized;
    }
    
    size_t size = 3 * m_numPolygons * sizeof(int);
    m_lightingIntensities = (int*)malloc( size );
    return (m_lightingIntensities == NULL) ? NovaErrNoMemory : NovaErrNone;
}

int Shape::AllocateTextureList()
{
    if ( m_textures != NULL )
    {
        return NovaErrAlreadyInitialized;
    }
    
    m_textures = (Texture**)malloc( m_numPolygons * sizeof(Texture*) );
    return (m_textures == NULL) ? NovaErrNoMemory : NovaErrNone;
}

int Shape::AllocateTextureCoordinateList()
{
    if ( m_textureCoordinates != NULL )
    {
        return NovaErrAlreadyInitialized;
    }

    size_t size = 6 * m_numPolygons * sizeof(int);
    m_textureCoordinates = (int*)malloc( size );
    if ( m_textureCoordinates == NULL )
    {
        return NovaErrNoMemory;
    }
    memset( m_textureCoordinates, 0, size );
    
    return NovaErrNone;
}

NOVA_EXPORT int Shape::SetVertexColors( const uint_32* colors )
{
    if ( m_vertexColors == NULL )
    {
        return NovaErrNotInitialized;
    }
    
    size_t size = 3 * m_numPolygons * sizeof(uint_32);
    memcpy( m_vertexColors, colors, size );
    
    return NovaErrNone;
}

NOVA_EXPORT int Shape::SetTexture( int polygonIndex, Texture* texture )
{
    if ( (polygonIndex < 0) || (polygonIndex >= m_numPolygons) )
    {
        return NovaErrOutOfBounds;
    }

    // texture's pixel format must match the shape's
    if ( texture->GetPixelFormat() != m_pixelFormat )
    {
	return NovaErrInvalidPixelFormat;
    }
    
    if ( m_textures == NULL )
    {
        int ret = AllocateTextureList();
        if ( ret != NovaErrNone )
	{
            return ret;
	}
    }
    
    m_textures[polygonIndex] = texture;
    return NovaErrNone;
}

NOVA_EXPORT int Shape::SetTexture( Texture* texture )
{
    for ( int i = 0; i < m_numPolygons; i++ )
    {
        int ret = SetTexture( i, texture );
        if ( ret != NovaErrNone )
	{
            return ret;
	}
    }
    
    return NovaErrNone;
}

NOVA_EXPORT int Shape::SetTextureCoordinates( const int_32* coordinates )
{
    if ( m_textureCoordinates == NULL )
    {
        int ret = AllocateTextureCoordinateList();
        if ( ret != NovaErrNone )
	{
            return ret;
	}
    }
    
    // 3 vertices, 2 coordinates (u,v) each
    size_t size = 6 * m_numPolygons * sizeof(int_32);
    memcpy( m_textureCoordinates, coordinates, size );

    return NovaErrNone;
}

NOVA_EXPORT int Shape::SetTextureCoordinates( int polygonIndex, 
                                              int u0, int v0, 
                                              int u1, int v1, 
                                              int u2, int v2 )
{
    if ( m_textureCoordinates == NULL )
    {
        int ret = AllocateTextureCoordinateList();
        if ( ret != NovaErrNone )
	{
            return ret;
	}
    }

    // 3 vertices, 2 coordinates (u,v) each
    int* textureCoordinate = m_textureCoordinates + polygonIndex  * 6;
    *textureCoordinate++ = u0;
    *textureCoordinate++ = v0;
    *textureCoordinate++ = u1;
    *textureCoordinate++ = v1;
    *textureCoordinate++ = u2;
    *textureCoordinate++ = v2;
    
    return NovaErrNone;
}

void Shape::DeallocateVertexNormals()
{
    free( m_vertexNormals );
    m_vertexNormals = NULL;
    free( m_transformedVertexNormals );
    m_transformedVertexNormals = NULL;
    free( m_vertexNormalIndices );
    m_vertexNormalIndices = NULL;
}

NOVA_EXPORT int Shape::SetVertexNormals( int numNormals, 
                                         Vector* normalList, 
                                         uint_32* indices )
{
    // deallocate existing lists
    DeallocateVertexNormals();
    
    // save new amount of normals
    m_numVertexNormals = numNormals;
    
    // allocate space for new lists
    size_t sizeNormals = numNormals * sizeof(Vector);
    m_vertexNormals = (Vector*)malloc( sizeNormals );
    if ( m_vertexNormals == NULL )
    {
        DeallocateVertexNormals();
        return NovaErrNoMemory;
    }
    m_transformedVertexNormals = (Vector*)malloc( sizeNormals );
    if ( m_transformedVertexNormals == NULL )
    {
        DeallocateVertexNormals();
        return NovaErrNoMemory;
    }
    size_t sizeIndices = m_numPolygons * 3 * sizeof(uint_32);
    m_vertexNormalIndices = (uint_32*)malloc( sizeIndices );
    if ( m_vertexNormalIndices == NULL )
    {
        DeallocateVertexNormals();
        return NovaErrNoMemory;
    }
    
    // copy data
    memcpy( m_vertexNormals, normalList, sizeNormals );
    memcpy( m_vertexNormalIndices, indices, sizeIndices );
    
    return NovaErrNone;
}

NOVA_EXPORT void Shape::AlignOnXZPlane()
{
    // find the smallest Y value
    Vector* v = m_coordinates;
    int_32 smallestY = (v++)->GetFixedY();

    for ( int i = 1; i < m_numCoordinates; i++ ) 
    {
        if ( v->GetFixedY() < smallestY ) 
	{
            smallestY = v->GetFixedY();
	}
        v++;
    }

    // shift all vertices along the Y axis the amount of the smallest Y coordinate
    v = m_coordinates;
    Vector sub;
    sub.SetFixed( 0, smallestY, 0 );

    for ( int i = 1; i < m_numCoordinates; i++ ) 
    {
        (v++)->Substract( sub );
    }    
} 

NOVA_EXPORT void Shape::Center()
{
    // find the smallest and largest x,y,z values
    Vector* v = m_coordinates;
    int_32 smallestX = v->GetFixedX();
    int_32 largestX = smallestX;
    int_32 smallestY = v->GetFixedY();
    int_32 largestY = smallestY;
    int_32 smallestZ = v->GetFixedZ();
    int_32 largestZ = smallestZ;
    v++;

    for ( int i = 1; i < m_numCoordinates; i++ ) 
    {
        int_32 x = v->GetFixedX();
        int_32 y = v->GetFixedY();
        int_32 z = v->GetFixedZ();
        v++;

        if ( x < smallestX ) smallestX = x;
        if ( x > largestX ) largestX = x;
        if ( y < smallestY ) smallestY = y;
        if ( y > largestY ) largestY = y;
        if ( z < smallestZ ) smallestZ = z;
        if ( z > largestZ ) largestZ = z;
    }

    // shift all vertices the amount of the middle point 
    Vector sub;
    sub.SetFixed( (smallestX + largestX) / 2,
                  (smallestY + largestY) / 2,
                  (smallestZ + largestZ) / 2 );
    v = m_coordinates;
    for ( int i = 1; i < m_numCoordinates; i++ ) 
    {
        (v++)->Substract( sub );
    }    
}

NOVA_EXPORT int Shape::SetEnvironmentMapped( int polygonIndex, bool mapped )
{
    if ( (polygonIndex < 0) || (polygonIndex >= m_numPolygons) )
    {
        return NovaErrOutOfBounds;
    }

    if ( m_numVertexNormals == 0 )
    {
	return NovaErrNoVertexNormals;
    }
    
    if ( mapped ) 
    {
        m_polygonInfos[polygonIndex] |= PolygonInfoEnvMapped;
    } 
    else 
    {
        m_polygonInfos[polygonIndex] &= ~PolygonInfoEnvMapped;
    }
    
    return NovaErrNone;
}

NOVA_EXPORT int Shape::SetEnvironmentMapped( bool mapped )
{
    for ( int i = 0; i < m_numPolygons; i++ )
    {
        int ret = SetEnvironmentMapped( i, mapped );
	if ( ret != NovaErrNone )
	{
	    return ret;
	}
    }
    
    return NovaErrNone;
}

NOVA_EXPORT int Shape::SetTextureFilterMode( int polygonIndex, 
                                             Texture::FilterMode filterMode )
{
    if ( (polygonIndex < 0) || (polygonIndex >= m_numPolygons) )
    {
        return NovaErrOutOfBounds;
    }
    
    // zero out the previous setting
    m_polygonInfos[polygonIndex] &= ~PolygonInfoTextureFilterMask;
    
    // set the new one
    uint_32 mode = (uint_32)(filterMode & PolygonInfoTextureFilterMask);
    m_polygonInfos[polygonIndex] |= mode;
    
    return NovaErrNone;
}

NOVA_EXPORT int Shape::SetTextureFilterMode( Texture::FilterMode filterMode )
{
    for ( int i = 0; i < m_numPolygons; i++ )
    {
        SetTextureFilterMode( i, filterMode );
    }
    
    return NovaErrNone;
}

NOVA_EXPORT void Shape::SetIlluminated( bool isIlluminated )
{
    LOG_DEBUG_F("Shape::SetIlluminated() = %d", isIlluminated);

    m_isIlluminated = isIlluminated;

    uint_32* pi = m_polygonInfos;
    for ( int i = 0; i < m_numPolygons; i++ )
    {
        if ( isIlluminated )
	{
            *pi++ |= PolygonInfoIlluminated;
	}
        else
	{
            *pi++ &= ~PolygonInfoIlluminated;
	}
    }
}

NOVA_EXPORT void Shape::CalculateBoundingSphereRadius()
{
    // the bounding sphere radius is the largest distance of a 
    // vertex coordinate from the object space origin
    Vector* v = m_coordinates;
    int_32 maxlen = 0;

    for ( int i = 0; i < m_numCoordinates; i++ ) 
    {
        int_32 len = v->LengthFixed();
        maxlen = MAX( maxlen, len );
    }

    m_boundingSphereRadius = maxlen;
}

void Shape::BackfaceCull( const Vector& cameraObjectSpacePosition )
{
    // clear the vertex info for every vertex
    memset( m_vertexInfos, 0, m_numCoordinates * sizeof(uint_32) );

    // determine the visibility of each face
    uint_32* polygonInfo = m_polygonInfos;
    PlaneEquation* planeEquation = m_planeEquations;
    uint_32* vertexIndex = m_vertices;
    
    for ( int i = 0; i < m_numPolygons; i++, polygonInfo++, planeEquation++ ) 
    {
        // determine the polygon visibility by the inside-outside test 
        if ( !planeEquation->IsOutside( cameraObjectSpacePosition ) ) 
	{
            // plane facing the camera position; mark it and all 3 vertices 
            // belonging to it visible
            *polygonInfo |= PolygonInfoVisible;
            *(m_vertexInfos + *vertexIndex++) |= VertexInfoVisible;
            *(m_vertexInfos + *vertexIndex++) |= VertexInfoVisible;
            *(m_vertexInfos + *vertexIndex++) |= VertexInfoVisible;
	} 
        else 
	{
            // plane facing away from camera position; mark it not visible
            *polygonInfo &= ~PolygonInfoVisible;
            vertexIndex += 3;
	}
    }
}

void Shape::ApplyLighting( const AmbientLight& ambientLight, 
                           const List<LightNode*>& lightNodeList )
{
    // vector pointing to the point light source from a vertex
    Vector toLight; 
    
    // get ambient light intensity for the scene
    int ambientIntensity = ambientLight.IntensityFixed();

    // initialize each vertex's lighting intensity to the ambient intensity
    int numIntensities = 3 * m_numPolygons;
    int* intensity = m_lightingIntensities;
    for ( int i = 0; i < numIntensities; i++ ) 
    {
        *intensity++ = ambientIntensity;
    }

    // process each point light
    for ( int i = 0; i < lightNodeList.Count(); i++ ) 
    {
        LightNode* lightNode;
        if ( lightNodeList.Get( i, lightNode ) != NovaErrNone )
	{
	    continue;
	}

	if ( lightNode->GetLight().GetType() != Light::TypePoint )
	{
	    continue;
	}

        PointLight& pointLight = (PointLight&)lightNode->GetLight();

        // we use a buffer to cache distances between the light 
        // source and each vertex to avoid calculating
        // them more than once. 
	// let's initialize the distance cache entries to 0.
        memset( m_distanceCache, 0, m_numCoordinates * sizeof(int_32) );

        // extract the light position in the shape's object space
        const Vector& lightObjectSpacePos = pointLight.GetPosition();

        const uint_32* vertexIndex = m_vertices;
        const uint_32* normalIndex = m_vertexNormalIndices;
        const uint_32* polygonInfo = m_polygonInfos;
        PlaneEquation* planeEquation = m_planeEquations;
        intensity = m_lightingIntensities;

        // process each polygon in the shape
        for ( int j = 0; j < m_numPolygons; j++, planeEquation++ ) 
	{
            uint_32 info = *polygonInfo++;

            // check that the both the camera AND the light source "see" 
            // the polygon, ie. the polygon must be facing the camera and light 
            // source to receive lighting
            if ( ((info & PolygonInfoVisible) == 0) ||
                 (planeEquation->IsOutside( lightObjectSpacePos )) ) 
	    {
                // polygon is not visible for the current camera and/or light; 
                // skip its lighting
                vertexIndex += 3;
                normalIndex += 3;
                intensity += 3;
                continue;
	    }

            // process each vertex in polygon
            for ( int k = 0; k < 3; k++ ) 
	    {
                // get current vertex coordinate
                int vertIndex = *vertexIndex++;
                const Vector* vertex = (m_coordinates + vertIndex);

                // get normal for this vertex
                const Vector* normal = (m_vertexNormals + *normalIndex++);

                // form the vector vertex -> light source
                toLight.SubstractAndSet( lightObjectSpacePos, *vertex );

                // check the cache for if the distance is already calculated
                int_32 distance = m_distanceCache[vertIndex];
                if ( distance == 0 ) 
		{
                    // cache entry not found. calculate and store in cache
                    distance = toLight.LengthFixed();
                    m_distanceCache[vertIndex] = distance;
		};

                // calculate the point light intensity as a function of the 
                // cosine of the angle between the vertex normal and the vector 
                // from the vertex to the light source
                int_32 dotProd = toLight.DotProductFixed( *normal );
                int_32 tmpIntensity = 0;

                // divide dot product by the distance (toLight length) to get 
                // cosine
                dotProd = (dotProd / (distance >> (FixedPointPrec/2))) << 
		    (FixedPointPrec/2);

                // only add if cosine > 0 (hence the angle < 90)
                if ( dotProd > 0 ) 
		{
                    tmpIntensity = dotProd;
		}
                
                // if the light has attenuation turned on, apply it based on 
                // the distance
                if ( pointLight.IsAttenuated() ) 
		{
                    int_32 attenuation = 
                        pointLight.CalculateAttenuationFactor( distance );
                    tmpIntensity = ::FixedLargeMul( tmpIntensity, attenuation );
		}

                // add this light's effect on the total intensity 
		// for this vertex
                *intensity++ += tmpIntensity;
	    }
	}
    }
}

void Shape::TransformAll( const Matrix& transform )
{
    // transform all the visible vertices (decided in BackfaceCull())
    // with the given transform
    Vector* srcCoord = m_coordinates;
    Vector* dstCoord = m_transformedCoordinates;
    uint_32* vertexInfo = m_vertexInfos;

    for ( int i = 0; i < m_numCoordinates; i++ ) 
    {
        if ( (*vertexInfo++ & VertexInfoVisible) != 0 ) 
	{
            dstCoord->TransformAndSet( transform, *srcCoord );
	}
        dstCoord++;
        srcCoord++;
    }

    // if vertex normals exist, transform them as well.
    if ( m_vertexNormals != NULL ) 
    {
        Vector* srcVector = m_vertexNormals;
        Vector* dstVector = m_transformedVertexNormals;

        for ( int i = 0; i < m_numVertexNormals; i++ ) 
	{
            dstVector->RotateAndSet( transform, *srcVector );
            dstVector++;
            srcVector++;
	}
    }
}

void Shape::CalculatePlaneEquation( int polygonIndex )
{
    uint_32* vertex = m_vertices + polygonIndex * 3;
    Vector* v0 = m_coordinates + *vertex++;
    Vector* v1 = m_coordinates + *vertex++;
    Vector* v2 = m_coordinates + *vertex++;

    // calculate the plane equation
    (m_planeEquations + polygonIndex)->Calculate( *v0, *v1, *v2 );
}

}; // namespace
