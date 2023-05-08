/*
 *  $Id: Shape.h 17 2009-09-02 09:03:47Z matti $
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

#ifndef __SHAPE_H
#define __SHAPE_H

// FILE INFO
// This file contains the class definition for Shape, which is used to 
// describe all visual polygon objects.

#include "NovaTypes.h"
#include "List.h"
#include "VectorMath.h"
#include "Texture.h"

namespace nova3d {

// forward declarations
class LightNode;
class Vector;
class AmbientLight;

// polygon info flag masks
const uint_32 PolygonInfoTextureFilterMask = 0x0007; // 0000 0000 0000 0111
const uint_32 PolygonInfoIlluminated = 0x0008; // 0000 0000 0000 1000
const uint_32 PolygonInfoEnvMapped = 0x0010; // 0000 0000 0001 0000
const uint_32 PolygonInfoVisible = 0x0020; // 0000 0000 0010 0000

// vertex info flag masks
const uint_32 VertexInfoVisible = 0x0020; // 0000 0000 0010 0000

/**
 * Represents a renderable mesh object constructed of polygons.<p />
 *
 * @author Matti Dahlbom
 * @version $Name:  $, $Revision: 17 $
 */
class Shape
{
 public: // Constructors and destructor
    NOVA_IMPORT Shape( NovaPixelFormat pixelFormat );
    NOVA_IMPORT virtual ~Shape();
        
 public: // New methods (Public API)
    NOVA_IMPORT int CreateGeometry( int numCoordinates, int numPolygons,
				    real_64* coordinates, uint_32* vertices );
        
    /** Sets the polygons to the Shape. */
    //        NOVA_IMPORT int SetPolygons( int numPolygons, 
    //                                     const uint_32* polygonVertices );

    /**
     * Sets the polygon vertices for given polygon.<p />
     *
     * The polygons are defined in <b>counter-clockwise</b> order.<p />
     */
    //        NOVA_IMPORT int SetPolygon( int polygonIndex, 
    //                                     uint_32 v0, uint_32 v1, uint_32 v2 );

    /**
     * Sets the colors for the vertices. There has to be 3 colors per each
     * polygon. The colors are mapped
     * to the vertices of the polygons in the respective order.<P>
     *
     * @param colors colors for the vertices in their definition order
     */
    NOVA_IMPORT int SetVertexColors( const uint_32* colors );

    /**
     * Retrieves the color list for the polygons in this shape. 
     * There is a color entry for each polygon in the color list, in the 
     * same order.<P>
     *
     * @return list of colors for the vertices of this shape, in the 
     * respective order
     */
    inline const uint_32* VertexColors() const;

    /**
     * Sets a texture for a polygon. The caller retains the ownership of the
     * texture object.
     *
     * @param polygonIndex polygon index
     * @param texture texture to set
     */
    NOVA_IMPORT int SetTexture( int polygonIndex, Texture* texture );

    /**
     * Sets a texture for all of the polygons in the shape. 
     * The caller retains the ownership of the texture object.
     *
     * @param texture texture to set
     */
    NOVA_IMPORT int SetTexture( Texture* texture );

    /**
     * Sets the texture coordinates for the shape. A local copy of the 
     * coordinates is made. <p />
     *
     * @param aCoords texture coordinates. Must contain 2 values (u,v) per 
     * each vertex of each polygon, ie. 6 values per each polygon.
     */
    NOVA_IMPORT int SetTextureCoordinates( const int* coordinates );

    /** Sets texture coordinates for a given polygon. */
    NOVA_IMPORT int SetTextureCoordinates( int polygonIndex, 
					   int u0, int v0, 
					   int u1, int v1, 
					   int u2, int v2 );

    /**
     * Sets the vertex normals for the polygons.<p />
     *
     * @param numNormals number of normals supplied
     * @param normalList the vertex normal list
     * @param indices indices to the vertex normal list. One index per
     * vertex of a polygon, ie. there are 3 indices / polygon.
     */
    NOVA_IMPORT int SetVertexNormals( int numNormals, 
				      Vector* normalList, 
				      uint_32* indices );

    /** 
     * Returns the textures for all the polygons of the shape. If the 
     * texture pointer is NULL for a certain polygon, it means the polygon 
     * is not textured. It the whole list is NULL, there are no textures 
     * for any of the polygons.
     */
    inline Texture** GetTextures() const;

    /**
     * Returns the texture coordinates for the shape. There are 3 (u,v) 
     * value pairs per each polygon, making it 6 coordinates per polygon, 
     * in their definition order. The returned pointer is NULL if no 
     * texture coordinates have been defined for the object.
     */
    inline const int* GetTextureCoordinates() const;

    /** Returns a buffer to place the transformed coordinates. */
    inline Vector* GetTransformedCoordinates() const;

    /**
     * Returns the polygon list. There are 3 vertices per each polygon.<p />
     *
     * @param polygonCount place to store the number of polygons.
     */
    inline const uint_32* GetPolygons( int_32& polygonCount ) const;

    /** Returns the number of polygons. */
    inline int GetNumPolygons() const;

    /** Returns the number of coordinates (vertices). */
    inline int GetNumCoordinates() const;

    /** Returns the bounding sphere radius */
    inline int_32 GetBoundingSphereRadius() const;

    /** 
     * Aligns the shape's coordinates so that the shape's bottom is set 
     * at the XZ plane (lowest Y = 0).<P>
     */
    NOVA_IMPORT void AlignOnXZPlane(); 

    /**
     * Centers the object, moving its middle point (average of 
     * smallest-largest coordinates) onto (0,0,0) in object space.
     */
    NOVA_IMPORT void Center();

    /**
     * Sets/unsets the "environment mapped" -property for a given polygon.
     *
     * @param polygonIndex polygon index
     * @param mapped true to enable environment mapping, false to disable
     */
    NOVA_IMPORT int SetEnvironmentMapped( int polygonIndex, bool mapped );

    /**
     * Sets/unsets the "environment mapped" -property for every polygon in 
     * the shape.
     *
     * @param mapped true to enable environment mapping, false to disable
     */
    NOVA_IMPORT int SetEnvironmentMapped( bool mapped );

    /**
     * Sets the texture filtering mode for a given polygon in the shape.
     *
     * @param polygonIndex polygon index
     * @param filterMode filtering mode
     */
    NOVA_IMPORT int SetTextureFilterMode( int polygonIndex, 
					  Texture::FilterMode filterMode );

    /**
     * Sets the texture filtering mode for all polygons in the shape.
     *
     * @param filterMode filtering mode
     */
    NOVA_IMPORT int SetTextureFilterMode( Texture::FilterMode filterMode );

    /**
     * Sets whether this shape is illuminated (receives lighting) or not. 
     * By default, the shape is NOT illuminated.
     */
    NOVA_IMPORT void SetIlluminated( bool isIlluminated );

    /**
     * Calculates the bounding sphere radius for the shape. This is 
     * a <b>very</b> heavy operation and should not be called unless 
     * changes to the coordinate data are applied. Note that this method
     * is called automatically by 
     * <code>PolygonEngine::SetSceneGraph()</code>
     * so it is rarely necessary to call it from the client code.
     */
    NOVA_IMPORT void CalculateBoundingSphereRadius();

    /** Returns the shape's coordinates and their count. */
    inline const Vector* GetCoords( int_32 count ) const;
        
    /** Returns the polygon info bitmasks for each polygon in shape. */
    inline const uint_32* GetPolygonInfo() const;

    /** Returns the pixel format used for the colors of this shape. */
    inline NovaPixelFormat GetPixelFormat() const;

    /** Returns the vertex colors - 3 per polygon. */
    inline const uint_32* GetVertexColors() const;
        
    /** Returns vertex normals for each vertex in the shape. */
    inline const Vector* GetVertexNormals( int_32& count ) const;
        
    /** Returns transformed vertex normals for each vertex in the shape. */
    inline const Vector* GetTransformedVertexNormals( int_32& count ) const;
        
    /** Returns vertex normal indices for the shape - 3 per polygon. */
    inline const uint_32* GetVertexNormalIndices() const;
        
    /** Returns lighting intensities for the shape - 3 per polygon. */
    inline int* GetLightingIntensities() const;
        
    /** Returns the plane equations for eacn polygon in the shape. */
    inline const PlaneEquation* GetPlaneEquations() const;

 public: // New methods 
    /**
     * Performs backface culling (in object space) by comparing the camera
     * position (transformed to object space) to the plane equations of the
     * polygons. m_polygonInfos / m_vertexInfos are updated correctly.
     * This is done every frame. 
     */
    void BackfaceCull( const Vector& cameraObjectSpacePosition );

    /**
     * Applies lighting to the shape. The lighting equation is: 
     * <pre>
     * Ia + Sigma(i=0->n)( f(d)i * Ipi )
     * <pre>
     *
     * where Ia = ambient light intensity, n = number of point 
     * light sources, f(d)i = the attenuation of i:th 
     * point light source given distance d between the light source
     * and a vertex, and Ipi = the intensity of the i:th point light 
     * source.<p />
     * 
     * The results are written to m_lightingIntensities. This is done
     * every frame.
     */
    void ApplyLighting( const AmbientLight& ambientLight, 
			const List<LightNode*>& lightNodeList );

    /**
     * Transforms all coordinates and [if needed] vertex normals  by the
     * given transform. In practice this means object space -> camera space
     * transformation. This is done every frame.
     */
    void TransformAll( const Matrix& transform );

    /** Indicates whether this shape is to receive illumination from lights. */
    inline bool IsIlluminated() const;
        
 private: // New methods
    /** Calculates the plane equation for the specified polygon */
    void CalculatePlaneEquation( int polygonIndex );
        
    int AllocateVertexList();
    int AllocateColorList();
    int AllocateCoordinateList();
    int AllocateTextureList();
    int AllocateTextureCoordinateList();
    int AllocateLightingIntensities();
    void DeallocateAll();
    void DeallocateVertexNormals();
        
 protected: // Data
    // number of coordinates in the coordinate list 
    int m_numCoordinates;
        
    // coordinate list (size: m_numCoordinates)
    Vector* m_coordinates;
        
    // transformed coordinate list (in camera space)
    Vector* m_transformedCoordinates;
        
    // number of polygons in the polygon list
    int m_numPolygons;
        
    // polygon list (polygon vertices; 3 per polygon). These values are 
    // indices to the m_coordinates list.
    uint_32* m_vertices;
        
    // pixel format used for the vertex colors 
    NovaPixelFormat m_pixelFormat;
        
    // colors for the polygon vertices of this shape. 
    // The number of entries matches the number of vertices, 
    // ie. 3 * m_numPolygons. The pixel format used depends 
    // on m_pixelFormat.
    uint_32* m_vertexColors;

    // textures. the number of entries matches
    // the number of polygons, ie. m_numPolygons
    Texture** m_textures;

    // texture coordinates. there are 2 values (u,v) per vertex of each 
    // polygon, in the order the polygons are defined.
    int_32* m_textureCoordinates;

    // number of vertex normals
    int m_numVertexNormals;

    // vertex normals. this list is indexed by values from 
    // iVertexNormalIndices
    Vector* m_vertexNormals;

    // transformed vertex normals (in camera space)
    Vector* m_transformedVertexNormals;

    // vertex normal indices. there are 3 values / triangle, one for 
    // each vertex of a polygon. the index are used to address vectors 
    // in m_vertexNormals
    uint_32* m_vertexNormalIndices;

    // whether this node is to receive illumination (default: true)
    bool m_isIlluminated;
    
    // lighting intensities (3 per polygon) at vertices as fixed point. 
    // These are dynamically calculated every frame
    int_32* m_lightingIntensities;

    // Distance cache for lighting. This is used to store distances 
    // between vertices and point lights to avoid calculating them 
    // more than once
    int_32* m_distanceCache;
        
 private: // Data
    // polygon plane equations (size: m_numPolygons)
    PlaneEquation* m_planeEquations;

    // polygon info flags (size: m_numPolygons)
    uint_32* m_polygonInfos;
        
    // vertex info flags (size: m_numCoordinates)
    uint_32* m_vertexInfos;
        
    // bounding sphere radius
    int_32 m_boundingSphereRadius;
};

///////////////////////////////////////////////////
// inline method definitions
///////////////////////////////////////////////////

int_32 Shape::GetBoundingSphereRadius() const
{
    return m_boundingSphereRadius;
}

Texture** Shape::GetTextures() const
{
    return m_textures;
}

const int* Shape::GetTextureCoordinates() const
{
    return m_textureCoordinates;
}

Vector* Shape::GetTransformedCoordinates() const
{
    return m_transformedCoordinates;
}

const uint_32* Shape::GetPolygons( int& polygonCount ) const
{
    polygonCount = m_numPolygons;
    return m_vertices;
}

int Shape::GetNumPolygons() const
{
    return m_numPolygons;
}

int Shape::GetNumCoordinates() const
{
    return m_numCoordinates;
}

NovaPixelFormat Shape::GetPixelFormat() const
{
    return m_pixelFormat;
}

const uint_32* Shape::GetVertexColors() const
{
    return m_vertexColors;
}

const Vector* Shape::GetCoords( int_32 count ) const
{
    count = m_numCoordinates;
    return m_coordinates;
}

const uint_32* Shape::GetPolygonInfo() const
{
    return m_polygonInfos;
}

const Vector* Shape::GetVertexNormals( int_32& count ) const
{
    count = m_numVertexNormals;
    return m_vertexNormals;
}

const Vector* Shape::GetTransformedVertexNormals( int_32& count ) const
{
    count = m_numVertexNormals;
    return m_transformedVertexNormals;
}

const uint_32* Shape::GetVertexNormalIndices() const
{
    return m_vertexNormalIndices;
}

bool Shape::IsIlluminated() const
{
    return m_isIlluminated;
}

int_32* Shape::GetLightingIntensities() const
{
    return m_lightingIntensities;
}

const PlaneEquation* Shape::GetPlaneEquations() const
{
    return m_planeEquations;
}

}; // namespace

#endif

