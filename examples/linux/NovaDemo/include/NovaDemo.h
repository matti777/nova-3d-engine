/*
 *  $Id: Camera.cpp 10 2009-08-28 12:41:25Z matti $
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

#ifndef __NOVADEMO_H
#define __NOVADEMO_H

#include <stdio.h>
#include <SDL/SDL.h>

#include "Node.h"
#include "Display.h"
#include "ColorCube.h"
#include "TexturedCube.h"
#include "Torus.h"
#include "Camera.h"

/** 
 * Main class for the demo application.<p />
 *
 * @author Matti Dahlbom
 * @version $Revision$
 */
class NovaDemo
{
 public: // Constructors and destructor
    NovaDemo();
    ~NovaDemo();

 public: // New methods
    int RenderLoop();
    int OpenGLLoop();
    
 private: // New methods
    void CreateSceneGraph();
    bool SetupSDL( int flags = 0 );
    void SetupRenderingCanvas();
    void SetupOpenGL();
    void RenderOpenGLFrame();
    void RenderFrame();
    void ToggleShape();
    void LoadTextures();
    void LoadTexture( const char* filename, nova3d::Texture*& texture );

 private: // Data
    // SDL surface
    SDL_Surface* m_sdlSurface;

    // rendering canvas
    nova3d::RenderingCanvas m_renderingCanvas;

    // scene graph
    nova3d::RootNode m_sceneGraph;
    
    // rotation node for the object
    nova3d::TransformationNode* m_rotationNode;

    // current rotation angle
    int_32 m_rotationAngle;

    // color cube object
    nova3d::ColorCube* m_colorCube;

    // shape node for the color cube object
    nova3d::ShapeNode* m_colorCubeNode;

    // textured cube object
    nova3d::TexturedCube* m_texturedCube;

    // shape node for the textured cube object
    nova3d::ShapeNode* m_texturedCubeNode;

    // torus object
    nova3d::Torus* m_torus;

    // shape node for the torus object
    nova3d::ShapeNode* m_torusNode;
    
    // current shape node being shown
    nova3d::ShapeNode* m_currentShapeNode;

    // camera
    nova3d::Camera* m_camera;

    // textures
    nova3d::Texture* m_goldTexture;
};

#endif
