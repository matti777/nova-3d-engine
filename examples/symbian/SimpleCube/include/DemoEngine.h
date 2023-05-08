/*
 *  $Id: Camera.cpp,v 1.42 2005/03/20 18:26:48 matti Exp $
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

#ifndef __DEMOENGINE_H
#define __DEMOENGINE_H

#include "DSAEngine.h"
#include "Shape.h"
#include "Node.h"
#include "Camera.h"

//using namespace nova3d;

// info bitmap dimensions
const TInt KInfoBitmapWidth = 50;
const TInt KInfoBitmapHeight = 15;

/**
 * DSAEngine implementation for Nova3D example application.<p />
 *
 * @author Matti Dahlbom
 * @version $Name:  $, $Revision: 1.7 $
 */
class CDemoEngine : public CDSAEngine 
    {
    public:
        CDemoEngine( RWsSession& aSession,
                     CWsScreenDevice& aScreenDevice,
                     RWindow &aWindow );
        ~CDemoEngine();

        void ConstructL();

    public: // new methods
        void ToggleObject();

    private: // new methods
        void CreateSceneGraphL();

    private: // from DSAEngine
        void RenderL();
        void CanvasUpdated() const;

    private: // data
        // polygon engine
//        CPolygonEngine *iPolygonEngine;

        // primary (and only) camera
        Camera *iCamera;

        // scene graph root
        RootNode *iSceneGraph;

        // translation vector + node for the object(s)
        TransformationNode *iTranslationNode;

        // rotation matrix + node for the object(s)
        TransformationNode *iRotationNode;
        
        // visual objects
        ShapeNode* iColorCubeNode;
        Shape* iColorCube;
        ShapeNode* iTexturedCubeNode;
        Shape* iTexturedCube;

        // currently used shape node
        ShapeNode* iCurrentShapeNode;

        // current rotation angle
        TInt iRotationAngle;

        // info bitmap
        CFbsBitmap* iInfoBitmap;
        CFbsBitmapDevice* iInfoBitDevice;
        CFbsBitGc* iInfoBitGc;
        TSize iInfoBitmapSize;
    };

#endif
