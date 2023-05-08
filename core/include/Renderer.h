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

#ifndef __RENDERER_H
#define __RENDERER_H

#include "Display.h"

namespace nova3d {

/**
 * Contains rendering logic.<p />
 *
 * @author Matti Dahlbom
 * @version $Revision$
 */
class Renderer 
{
 public: // Constructors and destructor
    Renderer( RenderingCanvas& canvas );

 public: // New methods
    /** Renders a nontextured, vertex colored polygon */
    void DrawTriangle( ScreenPolygon* face );

    /** Renders a textured polygon */
    void DrawTexturedTriangle( ScreenPolygon* face );

    /** Renders a lighted, textured polygon */
    void DrawLightedTexturedTriangle( ScreenPolygon* face );

 private: // New methods
    int_32 DivLookup( int_32 fixedDivider );

    void DrawGouraudSpan( int_32 x1, int_32 x2, 
			  int_32 red1, int_32 green1, 
			  int_32 blue1, 
			  int_32 red2, int_32 green2, 
			  int_32 blue2, 
			  uint_32* basePtr );
    void DrawTexturedSpan( int_32 leftX, int_32 rightX, 
			   int_32 leftU, int_32 leftV, 
			   int_32 leftZ,
			   int_32 dudx, int_32 dvdx, int_32 dzdx,
			   uint_32 scanlinePtr, 
			   Texture* texture );
    void DrawLightedTexturedSpan( int_32 leftX, int_32 rightX, 
				  int_32 leftU, int_32 leftV, 
				  int_32 leftZ, int_32 intensityLeft, 
				  int_32 dudx, int_32 dvdx, int_32 dzdx,
				  int_32 didx,
				  uint_32 scanlinePtr, 
				  Texture* texture );

 private: // Data
    // reference to the rendering canvas to draw to
    RenderingCanvas& m_canvas;

    // fixed point division lookup table
    int_32 m_fixedDivLookup[65536];
};

}; // namespace

#endif
