/*
 *  $Id: Camera.h 23 2009-09-10 13:57:16Z matti $
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

#ifndef __CAMERA_H
#define __CAMERA_H

// FILE INFO
// This file describes a virtual 'camera' which is used to project+render the
// 3D world onto a 2D surface.

#include "NovaTypes.h"
#include "Display.h"
#include "Frustum.h"
#include "Node.h"
#include "VectorMath.h"
#include "Lights.h"
#include "Renderer.h"

namespace nova3d {

// forward declaration
class CameraNode;
class Shape;
class ShapeNode;

/**
 * Represents a 'camera' used for rendering. Each camera has a "canvas" 
 * to render to.<p />
 *
 * @author Matti Dahlbom
 * @version $Name:  $, $Revision: 23 $
 */
class Camera 
{
 public: // Destructor
    NOVA_IMPORT Camera( RenderingCanvas& renderingCanvas );
    NOVA_IMPORT ~Camera();
        
 public: // New methods (Public API)
    // default FOV
    static const real_64 DefaultFov = 90.0;

    // Minimum clipping pane depth. This cannot be < 1.0 or the perspective 
    // texture mapper rasterizer will bug badly because of an overflow.
    static const int_32 MinimumNearClippingDepth = 1.0;

    /** Set FOV (field of vision) in degrees */
    NOVA_IMPORT int SetFov( real_64 fov );
        
    /** Transforms and renders the current scene graph on current canvas */
    NOVA_IMPORT int Render();
        
    /** Makes the camera look at the given point. Use NULL to cancel. */
    NOVA_IMPORT void LookAt( const Vector* target );
        
    /** Notifies the camera that the rendering canvas was updated. */
    NOVA_IMPORT void RenderingCanvasUpdated();

    /** Sets the pointer to the node that contains this camera. */
    void SetNode( CameraNode* node );
        
    /**
     * Indicates whether the camera is looking at a location, and if so, 
     * writes the location into the given vector.
     */
    bool GetLookAt( Vector& location );
        
 private: // New methods
    /**
     * Finds the largest of three z values to be used for depth sorting
     * value for given polygon.
     */
    inline int_32 SelectZsortValue( int_32 z1, int_32 z2, int_32 z3 );
    
    /**
     * Quicksorts visible faces to back-to-front order by their zsort_values.
     *
     * The sorted face list is pointed to by m_visibleFaces.
     */
    void DepthSort( int_32 left, int_32 right );
        
    /**
     * Process a polygon list; clip each polygon and add visible 
     * polygon sections to the visible face list. Then perform perspective
     * projection for the visible polygons and apply backface culling.
     */
    void ProcessPolygonList( Shape& shape );
        
    /** Processes a visual shape (object) node for rendering. */
    void ProcessShapeNode( ShapeNode& shapeNode,
			   Vector& cameraPos, 
			   Matrix& inverseCameraMatrix );
        
    /**
     * Performs perspective projection for a clipped polygon.
     *
     * ##TODO## Do this better; this way each vertex gets projected 
     * multiple times. Use some kind of a cache?
     */
    void PerspectiveProject( ScreenPolygon& polygon, 
			     int_32 x1, int_32 y1, int_32 z1,
			     int_32 x2, int_32 y2, int_32 z2,
			     int_32 x3, int_32 y3, int_32 z3 );
    
    /** Clips an edge against the near clip plane */
    void NearClipEdge( int& count, 
		       int xBuffer[], int yBuffer[], int zBuffer[],
		       int x1, int y1, int z1, 
		       int x2, int y2, int z2,
		       int valueA1, int valueB1, int valueC1, 
		       int valueA2, int valueB2, int valueC2,
		       int valueABuffer[], 
		       int valueBBuffer[], 
		       int valueCBuffer[] );

    /** Applies scene lighting to a shape. */
    void ApplyLightingToShape( Shape& shape, Vector& objectPos, 
			       Matrix& inverseObjectMatrix );

    /** Calculates environment mapping texture coefficients */
    void EnvironmentMapFace( Shape& shape, int polygonIndex, 
			     Texture* texture,
			     int_32& u0, int_32& v0,
			     int_32& u1, int_32& v1,
			     int_32& u2, int_32& v2 );

    /**
     * Makes sure there is enough space in the visible face buffer for 
     * every possible visible polygon. This method is called by the 
     * scene graph every time it is set live.<p />
     *
     * @param numPolygon number of polygons in all the shapes in the 
     * whole scene graph
     */
    int CheckVisibleFaceBuffer( int numPolygons );

    /** Notifies the camera that the scene graph it belongs to was detached. */
    void SceneGraphDetached();
        
    /** Sets a pointer to the list of shapes */
    void SetShapeNodeList( const List<ShapeNode*>* shapeNodeList );
        
    /** Sets a pointer to the list of lights */
    void SetLightNodeList( const List<LightNode*>* lightNodeList );
        
    /** Sets the ambient light */
    void SetAmbientLight( const AmbientLight* ambientLight );
        
 private: // Data
    // graphics renderer
    Renderer m_renderer;

    // pointer to the scene graph node that contains this camera
    CameraNode* m_cameraNode;    

    // number of allocated instances in the visible face buffer
    int m_maxVisibleFaces;

    // number of visible faces in use
    int m_numVisibleFaces;

    // the visible face buffer
    ScreenPolygon* m_visibleFaceBuffer;

    // list of visible faces in use. this is needed for faster sorting!
    ScreenPolygon** m_visibleFaceList;

    // FOV (field-of-vision) value
    real_64 m_fov;

    // A fixed-point value precalculated in SetFov() to be used in
    // perspective calculations
    int m_perspectiveFactor; 

    // the view frustum (used for 3D clipping)
    Frustum m_frustum;

    // look-at point
    Vector m_lookAtTarget;
    bool m_isLookingAt;

    // rendering canvas
    const RenderingCanvas& m_canvas;

    // depth where to add near clipping at as fixed point value
    int_32 m_nearClippingDepth;

    // pointer to the list of shape nodes in the current live scene graph.
    // Not owned!
    const List<ShapeNode*>* m_shapeNodeList;
        
    // pointer to the ambient light node in the current live scene graph.
    // Not owned!
    const AmbientLight* m_ambientLight;
        
    // pointer to the list of light nodes in the current live scene graph.
    // Not owned!
    const List<LightNode*>* m_lightNodeList;
        
    // friend declarations
    friend class RootNode;
};

/////////////////////////////////////////
// inline method definitions
/////////////////////////////////////////

int_32 Camera::SelectZsortValue( int_32 z1, int_32 z2, int_32 z3 )
{
    // this code selects the largest z
    if ( z1 > z2 ) 
    {
        if( z1 > z3 )
	{
            return z1;
	}
        else
	{
            return z3;
	}
    } 
    else 
    {
        if( z2 > z3 )
	{
            return z2;
	}
        else
	{
            return z3;
	}
    }
    /*
    // this code selects the smallest z
    if( z1 < z2 ) {
    if( z1 < z3 ) 
    return z1;
    else
    return z3;
    } else {
    if( z2 < z3 ) 
    return z2;
    else
    return z3;
    }
    */
}

}; // namespace

#endif
