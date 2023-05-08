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

#include "NovaDemo.h"

#include <SDL/SDL_image.h>
#include <GL/gl.h>

#include "TextureFactory.h"
#include "Normalizer.h"

using namespace nova3d;

// rendering window constants
const int WindowWidth = 640;
const int WindowHeight = 480;
const int WindowColorDepth = 32;

NovaDemo::NovaDemo()
    : m_sdlSurface( NULL ),
      m_rotationNode( NULL ), 
      m_renderingCanvas( RenderingCanvas() ), 
      m_sceneGraph(), 
      m_rotationAngle( 0 ), 
      m_colorCube( NULL ), 
      m_colorCubeNode( NULL ), 
      m_texturedCube( NULL ), 
      m_texturedCubeNode( NULL ), 
      m_torus( NULL ), 
      m_torusNode( NULL ), 
      m_currentShapeNode( NULL ), 
      m_camera( NULL ),
      m_goldTexture( NULL )
{
}

NovaDemo::~NovaDemo()
{
    delete m_camera;
    delete m_colorCube;
    delete m_texturedCube;
    delete m_torus;
    delete m_goldTexture;
}

void NovaDemo::RenderFrame()
{ 
    if ( SDL_MUSTLOCK( m_sdlSurface ) ) 
    {
	if ( SDL_LockSurface( m_sdlSurface ) != 0 ) 
	{
	    return;
	}
    }

    // clear the canvas to black
    memset( m_renderingCanvas.m_bufferPtr, 0, 
	    m_renderingCanvas.m_bytesPerScanline * m_renderingCanvas.m_height );

    // adjust the rotation by one step
    Vector axis( 4, 3, 0 );
    m_rotationNode->SetRotation( m_rotationAngle++, axis );

    // render the scenegraph
    m_camera->Render();

    if ( SDL_MUSTLOCK( m_sdlSurface ) ) 
    {
	SDL_UnlockSurface( m_sdlSurface );
    }
  
    SDL_Flip( m_sdlSurface ); 
}

void NovaDemo::RenderOpenGLFrame()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    //##TODO##

    SDL_GL_SwapBuffers();
}

bool NovaDemo::SetupSDL( int flags )
{
    printf("Initializing SDL..\n");

    // try to init SDL
    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 ) 
    {
	printf("failed to init SDL\n");
	return false;
    }

    printf("Creating SDL window..\n");

    // create the surface
    m_sdlSurface = 
	SDL_SetVideoMode( WindowWidth, WindowHeight, WindowColorDepth, flags );

    if ( m_sdlSurface == NULL )
    {
	printf( "failed to create SDL surface: %s\n", SDL_GetError() );
	SDL_Quit();
	return false;
    }

    SDL_WM_SetCaption("Nova3D Demo", "nova3d demo");

    printf("SetupSDL() done.\n");

    return true;
}

void NovaDemo::SetupRenderingCanvas()
{
    // setup the rendering canvas based on the SDL surface
    m_renderingCanvas = RenderingCanvas();
    m_renderingCanvas.m_top = 0;
    m_renderingCanvas.m_bottom = m_sdlSurface->h - 1;
    m_renderingCanvas.m_left = 0;
    m_renderingCanvas.m_right = m_sdlSurface->w - 1;
    m_renderingCanvas.m_height = m_sdlSurface->h;
    m_renderingCanvas.m_width = m_sdlSurface->w;
    m_renderingCanvas.m_centerX = m_sdlSurface->w / 2;
    m_renderingCanvas.m_centerY = m_sdlSurface->h / 2;
    m_renderingCanvas.m_pixelFormat = PixelFormat888;
    m_renderingCanvas.m_bytesPerScanline = m_sdlSurface->pitch;
    m_renderingCanvas.m_bufferPtr = m_sdlSurface->pixels;
}

void ThrowIfError( int ret )
{
    if ( ret != NovaErrNone )
    {
	throw ret;
    }
}

void NovaDemo::CreateSceneGraph()
{
    m_sceneGraph.SetName("Root");

    // create translation + translation node for the object and 
    // attach it to the root
    TransformationNode* translationNode = new TransformationNode();
    translationNode->SetName("Translation");
    Vector translation( 0.0, 0.0, 2.8 );
    translationNode->SetTranslation( translation );
    m_sceneGraph.AddChild( translationNode );

    // create rotation + rotation node and attach it to the translation node
    m_rotationNode = new TransformationNode();
    m_rotationNode->SetName("Rotation");
    m_rotationAngle = 0;
    translationNode->AddChild( m_rotationNode );

    // create the color cube and set it as the default object
    m_colorCube = new ColorCube( m_renderingCanvas.m_pixelFormat );
    m_colorCubeNode = new ShapeNode( *m_colorCube );
    m_colorCubeNode->SetName("ColorCube");
    m_currentShapeNode = m_colorCubeNode;
    m_rotationNode->AddChild( m_colorCubeNode );

    // create the textured cube
    m_texturedCube = new TexturedCube( m_renderingCanvas.m_pixelFormat );
    m_texturedCubeNode = new ShapeNode( *m_texturedCube );
    m_texturedCubeNode->SetName( "TexturedCube" );

    // create the torus
    m_torus = new Torus( m_renderingCanvas.m_pixelFormat, 
			 1.3, 0.5, 20, 16 );
    m_torusNode = new ShapeNode( *m_torus );
    m_torusNode->SetName("Torus");
    ThrowIfError( Normalizer::CreateVertexNormals( *m_torus ) );
    ThrowIfError( Normalizer::SmoothenVertexNormals( *m_torus ) );
    Normalizer::OptimizeVertexNormals( *m_torus );
    ThrowIfError( m_torus->SetTexture( m_goldTexture ) );
    ThrowIfError( m_torus->SetEnvironmentMapped( true ) );
    
    // create the camera and the camera node with no transformation
    m_camera = new Camera( m_renderingCanvas );
    CameraNode* cameraNode = new CameraNode( *m_camera );
    cameraNode->SetName("Camera");
    m_sceneGraph.AddChild( cameraNode );

    // create point light source to provide lighting 
    PointLight* light = new PointLight();
    light->SetAttenuation( 0.1, 0.0, 1.0 );
    LightNode* lightNode = new LightNode( *light );
    lightNode->SetName("PointLight");
    m_sceneGraph.AddChild( lightNode );

    // make the scene graph live
    m_sceneGraph.SetSceneGraphLive( true );
}

void NovaDemo::ToggleShape()
{
    // toggle the current shape node
    ShapeNode* oldShapeNode = m_currentShapeNode;
    if ( m_currentShapeNode == m_colorCubeNode ) 
    {
        m_currentShapeNode = m_texturedCubeNode;
    } 
    else if ( m_currentShapeNode == m_texturedCubeNode ) 
    {
	m_currentShapeNode = m_torusNode;
    }
    else
    {
        m_currentShapeNode = m_colorCubeNode;
    }

    // detach the old and attach the new one
    m_sceneGraph.SetSceneGraphLive( false );
    m_rotationNode->RemoveChild( oldShapeNode );
    m_rotationNode->AddChild( m_currentShapeNode );
    m_sceneGraph.SetSceneGraphLive( true );
}

void NovaDemo::LoadTexture( const char* filename, nova3d::Texture*& texture )
{
    SDL_Surface* image = IMG_Load( filename );
    if ( image == NULL )
    {
	char buf[256];
	sprintf( buf, "IMG_Load() failed: %s\n", IMG_GetError() );
	throw buf;
    }
    printf("LoadTexture(): size = %d x %d, bits = %d, "	\
	   "bytes = %d, mustlock = %d\n", 
	   image->w, image->h,
	   image->format->BitsPerPixel, 
	   image->format->BytesPerPixel, 
	   SDL_MUSTLOCK( image ) );
    
    TextureFactory textureFactory;
    int ret = textureFactory.CreateTexture( PixelFormat888, image->w, 
					    image->h, 
					    (uint_8*)image->pixels, 
					    texture );
    delete image;

    if ( ret != NovaErrNone )
    {
	char buf[256];
	sprintf( buf, "CreateTexture() failed = %d\n", ret );
	throw buf;
    }
}

void NovaDemo::LoadTextures()
{
    LoadTexture( "data/gold.jpg", m_goldTexture );
}

void NovaDemo::SetupOpenGL()
{
    glEnable( GL_CULL_FACE );
    //  glEnable(GL_LIGHTING);
    //glEnable(GL_LIGHT0);
    glEnable( GL_DEPTH_TEST );
} 

int NovaDemo::OpenGLLoop()
{
    if ( !SetupSDL( SDL_OPENGL ) )
    {
	return -1;
    }

    SetupOpenGL();

    printf("Ready to render!\n");

    bool quit = false;
    while( !quit ) 
    {
	SDL_Event event;
	while ( SDL_PollEvent(&event) ) 
	{      
	    switch (event.type) 
	    {
	    case SDL_QUIT:
		quit = true;
		break;
	    case SDL_KEYDOWN:
// 		if ( (event.key.keysym.sym == SDLK_LEFT) ||
// 		     (event.key.keysym.sym == SDLK_RIGHT) )
// 		{
// 		} 
// 		else 
		{
		    quit = true;
		}
		break;
	    }
	}

	// render a frame
        RenderOpenGLFrame();
    }

    SDL_Quit();

    printf("done.\n");
}

int NovaDemo::RenderLoop()
{
    printf("Initializing SDL..\n");

    // initialize SDL
    if ( !SetupSDL() )
    {
	return -1;
    }

    // set up the rendering canvas
    SetupRenderingCanvas();

    printf("Loading textures..\n");
    try 
    {
	// load textures
	LoadTextures();
    } 
    catch ( char* s )
    {
	printf( "LoadTextures() failed = %s\n", s);
	return -1;
    }

    printf("Creating scene graph..\n");
    try 
    {
	CreateSceneGraph();
    }
    catch ( int i ) 
    {
	printf("CreateSceneGraph() failed with int = %d\n", i);
	return i;
    }

    printf("Ready to render!\n");

    bool quit = false;
    while( !quit ) 
    {
	SDL_Event event;
	while ( SDL_PollEvent(&event) ) 
	{      
	    switch (event.type) 
	    {
	    case SDL_QUIT:
		quit = true;
		break;
	    case SDL_KEYDOWN:
		if ( (event.key.keysym.sym == SDLK_LEFT) ||
		     (event.key.keysym.sym == SDLK_RIGHT) )
		{
		    ToggleShape();
		} 
		else 
		{
		    quit = true;
		}
		break;
	    }
	}

	// render a frame
        RenderFrame();
    }

    SDL_Quit();

    printf("done.\n");
}

int main(int argc, char** argv) 
{
    NovaDemo demo;
    //return demo.RenderLoop();
    return demo.OpenGLLoop();    
}

