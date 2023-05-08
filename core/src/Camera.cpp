/*
 *  $Id: Camera.cpp 23 2009-09-10 13:57:16Z matti $
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

#include "Camera.h"
#include "Display.h"
#include "NovaErrors.h"
#include "RenderingUtils.h"
#include "Shape.h"
#include "novalogging.h"

namespace nova3d {

NOVA_EXPORT Camera::Camera( RenderingCanvas& renderingCanvas )
    : m_renderer( renderingCanvas ),
      m_cameraNode( NULL ), 
      m_maxVisibleFaces( 0 ), 
      m_numVisibleFaces( 0 ),
      m_visibleFaceBuffer( NULL ),
      m_visibleFaceList( NULL ),
      m_fov( 0.0 ),
      m_perspectiveFactor( 0 ),
      m_isLookingAt( false ),
      m_canvas( renderingCanvas ),
      m_nearClippingDepth( ::RealToFixed( MinimumNearClippingDepth ) ),
      m_shapeNodeList( NULL ),
      m_ambientLight( NULL ),
      m_lightNodeList( NULL )
{
    SetFov( DefaultFov );
}

NOVA_EXPORT Camera::~Camera()
{
    m_shapeNodeList = NULL; // not owned, must not delete
    free( m_visibleFaceList );
    free( m_visibleFaceBuffer );
}

NOVA_EXPORT int Camera::SetFov( real_64 fov )
{
    if ( (fov < 1.0) || (fov > 179.0) )
    {
        return NovaErrOutOfBounds;
    }

    LOG_DEBUG_F("Camera::SetFov() = %f", fov);
    
    m_fov = fov;
    
    // recalculate things
    RenderingCanvasUpdated();
    
    return NovaErrNone;
}

void Camera::SceneGraphDetached()
{
    // reset all the properties related to scene graph
    m_shapeNodeList = NULL;
    m_lightNodeList = NULL;
    delete m_ambientLight;
    m_ambientLight = NULL;

    m_maxVisibleFaces = -1;
    free( m_visibleFaceList );
    m_visibleFaceList = NULL;
    free( m_visibleFaceBuffer );
    m_visibleFaceBuffer = NULL;
}

void Camera::SetShapeNodeList( const List<ShapeNode*>* shapeNodeList )
{
    LOG_DEBUG("Camera::SetShapeNodeList()");
    m_shapeNodeList = shapeNodeList;

    // calculate total number of polygons in the list of shapes 
    int totalPolygons = 0;

    ShapeNode* shapeNode;
    for ( int i = 0; i < m_shapeNodeList->Count(); i++ )
    {
	if ( m_shapeNodeList->Get( i, shapeNode ) == NovaErrNone )
	{
	    totalPolygons += shapeNode->GetShape().GetNumPolygons();
	}
    }

    // check that the visible face list buffer is large enough
    CheckVisibleFaceBuffer( totalPolygons );
    LOG_DEBUG("Camera::SetShapeNodeList() done.");
}

void Camera::SetLightNodeList( const List<LightNode*>* lightNodeList )
{
    LOG_DEBUG("Camera::SetLightNodeList()");
    m_lightNodeList = lightNodeList;

    // try to find an ambient light from the list
    LightNode* lightNode;
    AmbientLight* ambientLight = NULL;
    for ( int i = 0; i < lightNodeList->Count(); i++ )
    {
	if ( lightNodeList->Get( i, lightNode ) == NovaErrNone )
	{
	    if ( lightNode->GetLight().GetType() == Light::TypeAmbient )
	    {
		ambientLight = (AmbientLight*)(&lightNode->GetLight());
		break;
	    }
	}
    }
    SetAmbientLight( ambientLight );
    LOG_DEBUG("Camera::SetLightNodeList() done.");
}

void Camera::SetAmbientLight( const AmbientLight* ambientLight )
{
    delete m_ambientLight;
    m_ambientLight = NULL;

    if ( ambientLight != NULL )
    {
	m_ambientLight = new AmbientLight( *ambientLight );
    }
    else 
    {
	// ambient light not found; use a dummy one 
	m_ambientLight = new AmbientLight();
    }
}

NOVA_EXPORT int Camera::Render()
{
    // reset number of visible faces to 0
    m_numVisibleFaces = 0;

    // transform camera 
    m_cameraNode->TransformBySceneGraph();

    // calculate the camera position with the camera transform
    const Matrix& cameraTransform = m_cameraNode->GetCameraMatrix();
    Vector cameraPos;
    cameraTransform.GetTranslation( cameraPos );
    cameraPos.RotateAndSet( cameraTransform, cameraPos );

    // calculate inverse camera transform for transforming objects
    Matrix inverseCameraMatrix( m_cameraNode->GetCameraMatrix() );
    inverseCameraMatrix.InvertTransformation();

    // process each shape node
    for ( int i = 0; i < m_shapeNodeList->Count(); i++ ) 
    {
        ShapeNode* shapeNode;
        if ( m_shapeNodeList->Get( i, shapeNode ) == NovaErrNone )
	{
            ProcessShapeNode( *shapeNode, cameraPos, inverseCameraMatrix );
	}
    }

    //LOG_DEBUG_F("visfaces = %d", m_numVisibleFaces);

    // initialize visible object pointer list for sorting
    ScreenPolygon** face = m_visibleFaceList;

    for ( int i = 0; i < m_numVisibleFaces; i++ ) 
    {
        *face++ = &m_visibleFaceBuffer[i];
    }

    // quicksort all visible polygons to back-to-front order 
    if ( m_numVisibleFaces > 0 ) 
    {
        DepthSort( 0, m_numVisibleFaces - 1 );
    }

    // draws all transformed, clipped, projected and sorted polygons on the 
    // camera's canvas
    ScreenPolygon** visibleFace = m_visibleFaceList;
    for ( int i = 0; i < m_numVisibleFaces; i++ ) 
    {
        ScreenPolygon* polygon = *visibleFace++;

        if ( polygon->m_texture == NULL ) 
	{
            // polygon has no texture; draw using vertex colors
	    m_renderer.DrawTriangle( polygon );
	} 
        else 
	{
            // polygon has texture; calculate 1/z, u/z, v/z for texture 
	    // mapping for all vertices
            nova3d::CalculateInverses( polygon->m_v1 );
            nova3d::CalculateInverses( polygon->m_v2 );
            nova3d::CalculateInverses( polygon->m_v3 );

            // select renderer based on whether polygon is to 
	    // illuminated or not
	    if ( polygon->m_polygonFlags & PolygonInfoIlluminated ) 
	    {
		m_renderer.DrawLightedTexturedTriangle( polygon );
	    } 
	    else 
	    {
		m_renderer.DrawTexturedTriangle( polygon );
	    }
	}
    }

    return NovaErrNone;
}

NOVA_EXPORT void Camera::LookAt( const Vector* target )
{
    m_isLookingAt = true;
    m_lookAtTarget.Set( *target );
}

NOVA_EXPORT void Camera::RenderingCanvasUpdated()
{
    real_64 fov_div2 = m_fov / 2.0;
    real_64 view_width_div2 = m_canvas.m_width / 2.0;
    real_64 angle = (M_PI * fov_div2) / 180.0;
    real_64 tangent = tan( angle );

//     LOG_DEBUG_F("fov_div2=%f, view_width_div2=%f, angle=%f, tangent=%f",
// 		fov_div2, view_width_div2, angle, tangent);
    
    m_perspectiveFactor = (int_32)(view_width_div2 / tangent);
    //    LOG_DEBUG_F("f = %d", m_perspectiveFactor);
    // only update the view frustum if a valid fov/canvas has been given
    if ( m_perspectiveFactor > 0 ) 
    {
        m_frustum.Calculate( m_fov, m_canvas, m_nearClippingDepth );
    } 
    else 
    {
	//##TODO## use custom exception
	throw "Illegal perspective factor";
    }
}

// The implementation is a QuickSort
void Camera::DepthSort( int_32 left, int_32 right )
{
    int_32 qleft = left;
    int_32 qright = right;
    int_32 qpivot = 
        (*(m_visibleFaceList + ((qleft + qright) >> 1)))->m_zSortValue;

    do {
	while( ((*(m_visibleFaceList + qleft))->m_zSortValue > qpivot) && 
	       (qleft < right) ) 
	{
	    qleft++;
	}
        
	while( (qpivot > (*(m_visibleFaceList + qright))->m_zSortValue) && 
	       (qright > left) ) 
	{
	    qright--;
	}
	
	if ( qleft <= qright ) 
	{
	    // swap elements 
	    ScreenPolygon *tmp = *(m_visibleFaceList + qright);
	    *(m_visibleFaceList + qright) = *(m_visibleFaceList + qleft);
	    *(m_visibleFaceList + qleft) = tmp;
	    qleft++;
	    qright--;
	}
    } while( qleft <= qright );
    
    // recursively sort left side
    if ( left < qright ) 
    {
	DepthSort( left, qright );
    }

    // recursively sort right side
    if ( qleft < right ) 
    {
	DepthSort( qleft, right );
    }
}

int Camera::CheckVisibleFaceBuffer( int numPolygons )
{
    LOG_DEBUG_F("Camera::CheckVisibleFaceBuffer() = %d", numPolygons);

    // maximum number of visible polygons is two times the amount of
    // polygons in the whole scene due to the clipping.
    int maxVisiblePolygons = numPolygons * 2;

    // check that the visible faces buffer is large enough.
    // if not, make it larger. if it is larger than required,
    // make it smaller to reduce memory consumption.
    if ( m_maxVisibleFaces != maxVisiblePolygons ) 
    {
        // delete current allocations
        free( m_visibleFaceList );
        m_visibleFaceList = NULL;
        free( m_visibleFaceBuffer );
        m_visibleFaceBuffer = NULL;
	
        // save new max amount of screen polygons
        m_maxVisibleFaces = maxVisiblePolygons;

        // reallocate
        size_t bufferSize = m_maxVisibleFaces * sizeof(ScreenPolygon);
        m_visibleFaceBuffer = (ScreenPolygon*)malloc( bufferSize );
        if ( m_visibleFaceBuffer == NULL )
	{
            return NovaErrNoMemory;
	}

        size_t listSize = m_maxVisibleFaces * sizeof(ScreenPolygon*);
        m_visibleFaceList = (ScreenPolygon**)malloc( listSize );
        if ( m_visibleFaceList == NULL )
	{
            free( m_visibleFaceBuffer );
            m_visibleFaceBuffer = NULL;
            return NovaErrNoMemory;
	}

        // clear the buffer entries to all zeros
        memset( m_visibleFaceBuffer, 0, bufferSize );
    }

    return NovaErrNone;
}

inline void Camera::NearClipEdge( int& count, 
				  int xBuffer[], int yBuffer[], int zBuffer[],
				  int x1, int y1, int z1, 
				  int x2, int y2, int z2,
				  int valueA1, int valueB1, int valueC1, 
				  int valueA2, int valueB2, int valueC2,
				  int valueABuffer[], 
				  int valueBBuffer[], 
				  int valueCBuffer[] )
{
    int f;

    if ( z1 < m_nearClippingDepth ) 
    {
	if( z2 < m_nearClippingDepth ) 
	{
	    // neither visible; add nothing
	} 
	else 
	{
	    // 2 visible; add clipped and 2
            f = ::FixedLargeDiv( (m_nearClippingDepth - z1), (z2 - z1) );
            xBuffer[count] = ::FixedLargeMul( (x2 - x1), f ) + x1;
            yBuffer[count] = ::FixedLargeMul( (y2 - y1), f ) + y1;
            zBuffer[count] = m_nearClippingDepth;
            valueABuffer[count] = 
		::FixedLargeMul( (valueA2 - valueA1), f ) + valueA1;
            valueBBuffer[count] = 
		::FixedLargeMul( (valueB2 - valueB1), f ) + valueB1;
            valueCBuffer[count] = 
		::FixedLargeMul( (valueC2 - valueC1), f ) + valueC1;
            xBuffer[count + 1] = x2;
            yBuffer[count + 1] = y2;
            zBuffer[count + 1] = z2;
            valueABuffer[count + 1] = valueA2;
            valueBBuffer[count + 1] = valueB2;
            valueCBuffer[count + 1] = valueC2;
            count += 2;
	}
    } 
    else 
    {
	if ( z2 < m_nearClippingDepth ) 
	{
	    // 1 visible; add clipped
            f = ::FixedLargeDiv( (z1 - m_nearClippingDepth), (z1 - z2) );
            xBuffer[count] = ::FixedLargeMul( (x2 - x1), f ) + x1;
            yBuffer[count] = ::FixedLargeMul( (y2 - y1), f ) + y1;
            zBuffer[count] = m_nearClippingDepth;
            valueABuffer[count] = 
		::FixedLargeMul( (valueA2 - valueA1), f ) + valueA1;
            valueBBuffer[count] = 
		::FixedLargeMul( (valueB2 - valueB1), f ) + valueB1;
            valueCBuffer[count] = 
		::FixedLargeMul( (valueC2 - valueC1), f ) + valueC1;
            count++;
	} 
	else 
	{
	    // both visible; add 2
            xBuffer[count] = x2;
            yBuffer[count] = y2;
            zBuffer[count] = z2;
            valueABuffer[count] = valueA2;
            valueBBuffer[count] = valueB2;
            valueCBuffer[count] = valueC2;
            count++;
	}
    }
}

void Camera::EnvironmentMapFace( Shape& shape, int polygonIndex, 
				 Texture* texture,
				 int_32& u0, int_32& v0,
				 int_32& u1, int_32& v1,
				 int_32& u2, int_32& v2 )
{
    int numNormals;
    const Vector* normals = shape.GetTransformedVertexNormals( numNormals );
    const uint_32* indices = shape.GetVertexNormalIndices();
    
    // go to the indices of the current polygon
    indices += polygonIndex * 3;

    // extract vertex normal vectors
    const Vector* normal_v0 = (normals + *indices++);
    const Vector* normal_v1 = (normals + *indices++);
    const Vector* normal_v2 = (normals + *indices++);
//     LOG_DEBUG(" ** NORMALS");
//     normal_v0->PrintToStdout();
//     normal_v1->PrintToStdout();
//     normal_v2->PrintToStdout();
//     if ( !((normal_v0 == normal_v1) && (normal_v0 == normal_v2)) )
//     {
// 	LOG_DEBUG(" ** DIFFERING NORMALS");
// 	normal_v0->PrintToStdout();
// 	normal_v1->PrintToStdout();
// 	normal_v2->PrintToStdout();	
//     }

    // extract texture dimensions
    int halfTexWidth = (texture->GetWidth() >> 1) - 1;
    int halfTexHeight = (texture->GetHeight() >> 1) - 1;
    int halfTexWidthFixed = halfTexWidth << FixedPointPrec;
    int halfTexHeightFixed = halfTexHeight << FixedPointPrec;

    // calculate the texture coordinates from the vertex normal values
    u0 = (int_32)(normal_v0->GetFixedX() * halfTexWidth + halfTexWidthFixed);
    v0 = (int_32)(normal_v0->GetFixedY() * halfTexHeight + halfTexHeightFixed);

    u1 = (int_32)(normal_v1->GetFixedX() * halfTexWidth + halfTexWidthFixed);
    v1 = (int_32)(normal_v1->GetFixedY() * halfTexHeight + halfTexHeightFixed);

    u2 = (int_32)(normal_v2->GetFixedX() * halfTexWidth + halfTexWidthFixed);
    v2 = (int_32)(normal_v2->GetFixedY() * halfTexHeight + halfTexHeightFixed);
}

void Camera::ProcessPolygonList( Shape& shape )
{
    //##TODO## break this down to (inline) methods

    int_32 x1, y1, z1, x2, y2, z2, x3, y3, z3;
    int_32 z_buffer[4];
    int_32 x_buffer[4];
    int_32 y_buffer[4];
    int_32 valueA1, valueA2, valueA3; 
    int_32 valueB1, valueB2, valueB3;
    int_32 valueC1, valueC2, valueC3;
    int_32 valueABuffer[4], valueBBuffer[4], valueCBuffer[4];
    Texture* texture = NULL;
    
    int_32 numPolygons;
    const uint_32* v = shape.GetPolygons( numPolygons );
    const Vector* coordList = shape.GetTransformedCoordinates();
    const uint_32* color = shape.GetVertexColors();
    Texture** tex = shape.GetTextures();
    const int_32* texCoord = shape.GetTextureCoordinates();
    const uint_32* polyInfo = shape.GetPolygonInfo();
    const int_32* lightIntensity = shape.GetLightingIntensities();
    
    // process all polygons adding all visible ones to the visible list
    for( int i = 0; i < numPolygons; i++ ) 
    {
        // get flags for this polygon
        uint_32 polyFlags = *polyInfo++;

        // check for polygon visibility (determined earlier in 
        // Shape::BackfaceCull())
        if ( (polyFlags & PolygonInfoVisible) == 0 ) {
            // polygon not visible; skip it entirely
            v += 3;
            texCoord += 6;
            color += 3;
            lightIntensity += 3;

            continue;
        }

        // get texture for this polygon
        texture = (tex != NULL) ? (*tex++) : NULL;

        // get coordinates for this polygon
        coordList[*v++].GetFixed( x1, y1, z1 );
        coordList[*v++].GetFixed( x2, y2, z2 );
        coordList[*v++].GetFixed( x3, y3, z3 );

        if ( texture != NULL ) 
	{
	    // get lighting intensities for vertices of this polygon 
	    valueC1 = *lightIntensity++;
	    valueC2 = *lightIntensity++;
	    valueC3 = *lightIntensity++;

            // textured polygon; get texture mapping coordinates
            if ( (polyFlags & PolygonInfoEnvMapped) != 0 ) 
	    {
                // Using an environment map - calculate u,v values 
                EnvironmentMapFace( shape, i, texture, 
				    valueA1, valueB1, valueA2, valueB2, 
				    valueA3, valueB3 );
    
                if ( texCoord != NULL ) 
		{
                    // skip the normal texture coordinates for this polygon
                    texCoord += 6;
		}
	    } 
            else 
	    {
                // Using a standard texture
                valueA1 = *texCoord++ << FixedPointPrec;
                valueB1 = *texCoord++ << FixedPointPrec;
                valueA2 = *texCoord++ << FixedPointPrec;
                valueB2 = *texCoord++ << FixedPointPrec;
                valueA3 = *texCoord++ << FixedPointPrec;
                valueB3 = *texCoord++ << FixedPointPrec;
	    }

            if ( shape.IsIlluminated() ) 
	    {
                // multiply the lighting values by the number of the palettes 
		// used for illumination
		texture->ScaleIntensity( valueC1 );
		texture->ScaleIntensity( valueC2 );
		texture->ScaleIntensity( valueC3 );
	    }
	}
        else 
	{
	    // vertex colored polygon
	    int_32 intensity1 = *lightIntensity++;
	    int_32 intensity2 = *lightIntensity++;
	    int_32 intensity3 = *lightIntensity++;

            nova3d::SplitColorIntoFixedPoint( shape.GetPixelFormat(), *color++, 
					      valueA1, valueB1, valueC1 );
            nova3d::SplitColorIntoFixedPoint( shape.GetPixelFormat(), *color++, 
					      valueA2, valueB2, valueC2 );
            nova3d::SplitColorIntoFixedPoint( shape.GetPixelFormat(), *color++, 
					      valueA3, valueB3, valueC3 );

            if ( shape.IsIlluminated() ) 
	    {
                // shape is illuminated; calculate lighted vertex colors
		nova3d::ScaleColor( shape.GetPixelFormat(), 
				    valueA1, valueB1, valueC1, intensity1 );
		nova3d::ScaleColor( shape.GetPixelFormat(), 
				    valueA2, valueB2, valueC2, intensity2 );
		nova3d::ScaleColor( shape.GetPixelFormat(), 
				    valueA3, valueB3, valueC3, intensity3 );
	    }
	}

        // check if near clipping needed
        if ( (z1 < m_nearClippingDepth) || 
             (z2 < m_nearClippingDepth) || 
             (z3 < m_nearClippingDepth) ) 
	{
            // yes - clip the polygon against Z = iNearClippingDepth
            int_32 count = 0;

	    NearClipEdge( count, 
			  x_buffer, y_buffer, z_buffer, 
			  x1, y1, z1, x2, y2, z2, 
			  valueA1, valueB1, valueC1, 
			  valueA2, valueB2, valueC2, 
			  valueABuffer, valueBBuffer, valueCBuffer );
	    NearClipEdge( count, 
			  x_buffer, y_buffer, z_buffer, 
			  x2, y2, z2, x3, y3, z3,
			  valueA2, valueB2, valueC2, 
			  valueA3, valueB3, valueC3, 
			  valueABuffer, valueBBuffer, valueCBuffer );
	    NearClipEdge( count, 
			  x_buffer, y_buffer, z_buffer, 
			  x3, y3, z3, x1, y1, z1,
			  valueA3, valueB3, valueC3, 
			  valueA1, valueB1, valueC1, 
			  valueABuffer, valueBBuffer, valueCBuffer );
	    
            // clipping produces 0, 1 or 2 triangles
            if ( count >= 3 ) 
	    {
                // one or more triangles; the first is defined by points 0,1,2 
                ScreenPolygon* face = &(m_visibleFaceBuffer[m_numVisibleFaces]);
                face->m_zSortValue = 
		    SelectZsortValue( z_buffer[0], z_buffer[1], z_buffer[2] );
                
                PerspectiveProject(*face, 
                                   x_buffer[0], y_buffer[0], z_buffer[0],
                                   x_buffer[1], y_buffer[1], z_buffer[1],
                                   x_buffer[2], y_buffer[2], z_buffer[2]);

                face->m_polygonFlags = polyFlags;
                face->m_texture = texture;

                if ( texture != NULL ) 
		{
                    // polygon has texture; store the coordinates
                    face->m_v1.m_textureCoordinates.m_u = valueABuffer[0];
                    face->m_v1.m_textureCoordinates.m_v = valueBBuffer[0];
                    face->m_v1.m_textureCoordinates.m_intensity = 
			valueCBuffer[0];
                    face->m_v2.m_textureCoordinates.m_u = valueABuffer[1];
                    face->m_v2.m_textureCoordinates.m_v = valueBBuffer[1];
                    face->m_v2.m_textureCoordinates.m_intensity = 
			valueCBuffer[1];
                    face->m_v3.m_textureCoordinates.m_u = valueABuffer[2];
                    face->m_v3.m_textureCoordinates.m_v = valueBBuffer[2];
                    face->m_v3.m_textureCoordinates.m_intensity = 
			valueCBuffer[2];
		} 
                else 
		{
                    // polygon is untextured; use color
                    face->m_v1.m_color.m_red = valueABuffer[0];
                    face->m_v1.m_color.m_green = valueBBuffer[0];
                    face->m_v1.m_color.m_blue = valueCBuffer[0];
                    face->m_v2.m_color.m_red = valueABuffer[1];
                    face->m_v2.m_color.m_green = valueBBuffer[1];
                    face->m_v2.m_color.m_blue = valueCBuffer[1];
                    face->m_v3.m_color.m_red = valueABuffer[2];
                    face->m_v3.m_color.m_green = valueBBuffer[2];
                    face->m_v3.m_color.m_blue = valueCBuffer[2];
		}

                // check whether the clipping resulted in 2 triangles
                if ( count == 4 ) 
		{
                    // yes - add the another too; it is defined by points 0,2,3
                    ScreenPolygon* face = 
			&(m_visibleFaceBuffer[m_numVisibleFaces]);
                    face->m_zSortValue = 
			SelectZsortValue(z_buffer[0], z_buffer[2], z_buffer[3]);

                    PerspectiveProject(*face, 
                                       x_buffer[0], y_buffer[0], z_buffer[0],
                                       x_buffer[2], y_buffer[2], z_buffer[2],
                                       x_buffer[3], y_buffer[3], z_buffer[3]);

                    face->m_polygonFlags = polyFlags;
                    face->m_texture = texture;

                    if ( texture != NULL ) 
		    {
                        // polygon has texture; store the coordinates
			face->m_v1.m_textureCoordinates.m_u = valueABuffer[0];
			face->m_v1.m_textureCoordinates.m_v = valueBBuffer[0];
			face->m_v1.m_textureCoordinates.m_intensity = 
			    valueCBuffer[0];
			face->m_v2.m_textureCoordinates.m_u = valueABuffer[2];
			face->m_v2.m_textureCoordinates.m_v = valueBBuffer[2];
			face->m_v2.m_textureCoordinates.m_intensity = 
			    valueCBuffer[2];
			face->m_v3.m_textureCoordinates.m_u = valueABuffer[3];
			face->m_v3.m_textureCoordinates.m_v = valueBBuffer[3];
			face->m_v3.m_textureCoordinates.m_intensity = 
			    valueCBuffer[3];
                    } 
		    else 
		    {
                        // polygon is untextured; use color
			face->m_v1.m_color.m_red = valueABuffer[0];
			face->m_v1.m_color.m_green = valueBBuffer[0];
			face->m_v1.m_color.m_blue = valueCBuffer[0];
			face->m_v2.m_color.m_red = valueABuffer[2];
			face->m_v2.m_color.m_green = valueBBuffer[2];
			face->m_v2.m_color.m_blue = valueCBuffer[2];
			face->m_v3.m_color.m_red = valueABuffer[3];
			face->m_v3.m_color.m_green = valueBBuffer[3];
			face->m_v3.m_color.m_blue = valueCBuffer[3];
                    }
                }
            }
        } 
	else 
	{
            // no near clipping needed
            ScreenPolygon* face = &(m_visibleFaceBuffer[m_numVisibleFaces]);
            face->m_zSortValue = SelectZsortValue( z1, z2, z3 );

            PerspectiveProject( *face, x1, y1, z1, x2, y2, z2, x3, y3, z3 );
            
            face->m_polygonFlags = polyFlags;
            face->m_texture = texture;

            if ( texture != NULL ) 
	    {
                // polygon has texture; store the coordinates
                face->m_v1.m_textureCoordinates.m_u = valueA1;
                face->m_v1.m_textureCoordinates.m_v = valueB1;
                face->m_v1.m_textureCoordinates.m_intensity = valueC1;
                face->m_v2.m_textureCoordinates.m_u = valueA2;
                face->m_v2.m_textureCoordinates.m_v = valueB2;
                face->m_v2.m_textureCoordinates.m_intensity = valueC2;
                face->m_v3.m_textureCoordinates.m_u = valueA3;
                face->m_v3.m_textureCoordinates.m_v = valueB3;
                face->m_v3.m_textureCoordinates.m_intensity = valueC3;
            } 
	    else 
	    {
                // polygon is untextured; use color
                face->m_v1.m_color.m_red = valueA1;
                face->m_v1.m_color.m_green = valueB1;
                face->m_v1.m_color.m_blue = valueC1;
                face->m_v2.m_color.m_red = valueA2;
                face->m_v2.m_color.m_green = valueB2;
                face->m_v2.m_color.m_blue = valueC2;
                face->m_v3.m_color.m_red = valueA3;
                face->m_v3.m_color.m_green = valueB3;
                face->m_v3.m_color.m_blue = valueC3;
            }
        }
    }
    
}

void Camera::ProcessShapeNode( ShapeNode& shapeNode,
                               Vector& cameraPos, 
                               Matrix& inverseCameraMatrix )
{
    //    LOG_DEBUG("Camera::ProcessShapeNode()");

    Shape& shape = shapeNode.GetShape();
    
    // transform the shape
    shapeNode.TransformBySceneGraph();

    // transform camera position to object space using the inverse 
    // object transformation
    const Matrix& objectMatrix = shapeNode.GetObjectMatrix();

    Vector objectPos;
    objectMatrix.GetTranslation( objectPos );

    Vector cameraObjectSpacePos( cameraPos, objectPos );
    Matrix inverseObjectMatrix( objectMatrix );
    inverseObjectMatrix.ClearTranslation();
    inverseObjectMatrix.InvertTransformation();
    cameraObjectSpacePos.TransformAndSet( inverseObjectMatrix, 
                                          cameraObjectSpacePos );

    // perform backface removal in object space
    shape.BackfaceCull( cameraObjectSpacePos );

    // if the shape is to receive lighting, apply it
    if ( shape.IsIlluminated() ) 
    {
        ApplyLightingToShape( shape, objectPos, inverseObjectMatrix );
    }

    // transform the object matrix by the inverse camera transformation
    // to bring it to the camera space
    shapeNode.TransformByCamera( inverseCameraMatrix );

    // transform all geometry in the shape with the combined transform
    // object space -> camera space
    shape.TransformAll( shapeNode.GetObjectMatrix() );

    // process all polygons: each polygon of the shape is near clipped,
    // perspective transformed and all the visible polygons are added
    // to the list of visible polygons
    ProcessPolygonList( shape );
}

void Camera::PerspectiveProject( ScreenPolygon& polygon, 
                                 int_32 x1, int_32 y1, int_32 z1,
                                 int_32 x2, int_32 y2, int_32 z2,
                                 int_32 x3, int_32 y3, int_32 z3 )
{
    int_32 proj_x1, proj_y1, proj_x2, proj_y2, proj_x3, proj_y3;
    int_32 inv_z;

    // vertex 1
    inv_z = (int_32)(MaxUint32 / (uint_32)z1);
    proj_x1 = FixedLargeMul( (x1 * m_perspectiveFactor), inv_z );
    proj_y1 = -FixedLargeMul( (y1 * m_perspectiveFactor), inv_z );

    // vertex 2
    inv_z = (int_32)(MaxUint32 / (uint_32)z2);
    proj_x2 = FixedLargeMul( (x2 * m_perspectiveFactor), inv_z );
    proj_y2 = -FixedLargeMul( (y2 * m_perspectiveFactor), inv_z );

    // vertex 3
    inv_z = (int_32)(MaxUint32 / (uint_32)z3);
    proj_x3 = FixedLargeMul( (x3 * m_perspectiveFactor), inv_z );
    proj_y3 = -FixedLargeMul( (y3 * m_perspectiveFactor), inv_z );

    int_32 centerx = m_canvas.m_centerX << FixedPointPrec;
    int_32 centery = m_canvas.m_centerY << FixedPointPrec;

    // store the coordinates in fixed point for better 
    // accuracy in scan conversion
    polygon.m_v1.m_x = proj_x1 + centerx;
    polygon.m_v1.m_y = proj_y1 + centery;
    polygon.m_v1.m_z = z1;
    polygon.m_v2.m_x = proj_x2 + centerx;
    polygon.m_v2.m_y = proj_y2 + centery;
    polygon.m_v2.m_z = z2;
    polygon.m_v3.m_x = proj_x3 + centerx;
    polygon.m_v3.m_y = proj_y3 + centery;
    polygon.m_v3.m_z = z3;

    // mark this face added to the list of visible faces
    m_numVisibleFaces++;
}

void Camera::SetNode( CameraNode* node )
{
    m_cameraNode = node;
}

bool Camera::GetLookAt( Vector& location )
{
    if ( !m_isLookingAt ) 
    {
        return false;
    }
    else
    {
        location.Set( m_lookAtTarget );
        return true;
    }
}

void Camera::ApplyLightingToShape( Shape& shape, Vector& objectPos, 
                                   Matrix& inverseObjectMatrix )
{
    // transform all lights to the shape's object space
    for ( int i = 0; i < m_lightNodeList->Count(); i++ )
    {
	LightNode* lightNode;
	if ( m_lightNodeList->Get( i, lightNode ) != NovaErrNone )
	{
	    // could not fetch a light node at this index
	    continue;
	}

	Light& light = lightNode->GetLight();
        if ( light.GetType() == Light::TypePoint )
	{
            PointLight& pointLight = 
		static_cast<PointLight&>(light);
            Vector lightObjectSpacePos( pointLight.GetPosition(), objectPos );
            lightObjectSpacePos.TransformAndSet( inverseObjectMatrix, 
                                                 lightObjectSpacePos );
            pointLight.SetPosition( lightObjectSpacePos );
	}            
    }
    
    shape.ApplyLighting( *m_ambientLight, *m_lightNodeList );
}

}; // namespace
