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

#include "DemoEngine.h"


CDemoEngine::CDemoEngine( RWsSession &aSession,
                          CWsScreenDevice &aScreenDevice,
                          RWindow &aWindow )
    : CDSAEngine( aSession, aScreenDevice, aWindow, KDrawInterval ),
      iInfoBitmapSize( KInfoBitmapWidth, KInfoBitmapHeight )
    {
    }

void CDemoEngine::ConstructL()
    {
    // allow base class to construct itself
    CDSAEngine::ConstructL();

//    iPolygonEngine = CPolygonEngine::NewL();

    CreateSceneGraphL();

    // create info bitmap resource
    iInfoBitmap = new (ELeave) CFbsBitmap();
    User::LeaveIfError( iInfoBitmap->Create( iInfoBitmapSize, KNovaDisplayMode ) );
    iInfoBitDevice = CFbsBitmapDevice::NewL( iInfoBitmap );
    iInfoBitGc = CFbsBitGc::NewL();
    iInfoBitGc->Activate( iInfoBitDevice );
    iInfoBitGc->SetPenStyle( CBitmapContext::ESolidPen );
    iInfoBitGc->SetBrushStyle( CBitmapContext::ESolidBrush );
    iInfoBitGc->SetBrushColor( KRgbBlack );
    iInfoBitGc->SetPenColor( KRgbWhite );
    iInfoBitGc->UseFont( CEikonEnv::Static()->DenseFont() );
    }

CDemoEngine::~CDemoEngine()
    {
    delete iInfoBitGc;
    delete iInfoBitDevice;
    delete iInfoBitmap;

    delete iCamera;
    delete iPolygonEngine;
    delete iColorCube;
    delete iTexturedCube;

    // delete all scene graph resources
    delete iSceneGraph;

    // must delete the unattached node by hand
    if ( iCurrentShapeNode == iTexturedCubeNode ) 
        {
        delete iColorCubeNode;
        } 
    else 
        {
        delete iTexturedCubeNode;
        }
    }

// node names
//_LIT( KTranslationNodeName, "Translation" );
//_LIT( KRotationNodeName, "Rotation" );
//_LIT( KColorCubeNodeName, "ColorCube" );
//_LIT( KRootNodeName, "Root" );
//_LIT( KCameraNodeName, "Camera" );
//_LIT( KTexturedCubeNodeName, "TexturedCube" );

void CDemoEngine::CreateSceneGraphL()
    {
    // create the scene graph root
    iSceneGraph = new NO_THROW RootNode();
    User::LeaveIfNull( iSceneGraph );
    
    // create translation + translation node for the object and 
    // attach it to the scene graph
    iTranslationNode = new NO_THROW TransformationNode();
    User::LeaveIfNull( iTranslationNode );
    Vector translation( 0.0, 0.0, 3.0 );
    iTranslationNode->SetTranslation( translation );
    User::LeaveIfError( iSceneGraph->AddChild( iTranslationNode ) );

    // create rotation + rotation node and attach it to the translation node
    iRotationNode = new NO_THROW TransformNode();
    User::LeaveIfNull( iRotationNode );
    iRotationAngle = 0;
    User::LeaveIfError( iTranslationNode->AddChild( iRotationNode ) );
    
    // create the color cube and set it as the default object
    iColorCube = new NO_THROW ColorCube();
    User::LeaveIfNull( iColorCube );
    iColorCubeNode = new NO_THROW ShapeNode( *iColorCube );
    User::LeaveIfNull( iColorCubeNode );
    iCurrentShapeNode = iColorCubeNode;
    User::LeaveIfError( iRotationNode->AddChild( iCurrentShapeNode ) );
    
    
//    // create the textured cube
//    iTexturedCube = CTexturedCube::NewL();
//    iTexturedCubeNode = CShapeNode::NewL( *iTexturedCube );
//    iTexturedCubeNode->SetName( KTexturedCubeNodeName );
//
    
    //##TODO##
    
    // create the camera and the camera node with no transformation
    iCamera = new NO_THROW Camera( GetCanvas() );
    User::LeaveIfNull( iCamera );
    iCameraNode = new NO_THROW CameraNode( *iCamera );
    User::LeaveIfNull( iCameraNode );
    User::LeaveIfError( iSceneGraph->AddChild( cameraNode ) );
        
    // create point light source to provide lighting
    PointLight* light = new NO_THROW PointLight();
    User::LeaveIfNull( light );
    light->SetAttenuation( 0.1, 0.0, 1.0 );
    User::LeaveIfError( iSceneGraph->Add( *light ) );
    
    // make the scene graph live
    iSceneGraph->SetSceneGraphLive( true );
    }

void CDemoEngine::ToggleObject()
    {
//    // toggle the current shape node
//    CShapeNode* oldShapeNode = iCurrentShapeNode;
//    if ( iCurrentShapeNode == iColorCubeNode ) {
//        iCurrentShapeNode = iTexturedCubeNode;
//    } else {
//        iCurrentShapeNode = iColorCubeNode;
//    }
//
//    // detach the old and attach the new one
//    iPolygonEngine->DetachSceneGraphL();
//    iRotationNode->RemoveChild( oldShapeNode );
//    iRotationNode->AddChild( iCurrentShapeNode );
//    iPolygonEngine->SetSceneGraphL( iSceneGraph );
    }

void CDemoEngine::RenderL()
    {
    // clear the background to black
    const RenderingCanvas& canvas = GetCanvas();
    Mem::FillZ( canvas.m_bufferPtr, 
                canvas.m_bytesPerScanline * canvas.iHeight );

    // adjust the rotation by one step
    Vector axis( 4, 4, 0 );
    iRotationNode->SetRotation( iRotationAngle++, axis );

    // render the scenegraph
    iCamera->Render();

    //##TODO## FPS display
    // create the info bitmap: show FPS count
//    TBuf<32> buf;
//    _LIT( KFpsFmt, "FPS: %.1f" );
//    buf.Format( KFpsFmt, iRenderTimer->GetCurrentFPS() );
//    iInfoBitGc->Clear();
//    iInfoBitGc->DrawText( buf, TPoint( 2, 13 ) );

    // draw the info bitmap on screen
//    ::BitBlt( canvas, *iInfoBitmap, 4, 4 );
    }

void CDemoEngine::CanvasUpdated() const
    {
    iCamera->CanvasWasUpdated();
    }
