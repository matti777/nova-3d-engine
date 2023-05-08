/*
 *  $Id: ColorCube.h 17 2009-09-02 09:03:47Z matti $
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

#ifndef __TEXTUREDCUBE_H
#define __TEXTUREDCUBE_H

// FILE INFO
// This is a simple cube object with textures. This code can be used for
// quickly plugging a simple object into a Nova3D scene graph for testing
// purposes, or just as an example of how to use the APIs.

#include "Shape.h"
#include "NovaTypes.h"
#include "Display.h"
#include "ColorCube.h"
#include "Texture.h"

namespace nova3d {

/**
 * A simple colored cube for testing out the polygon engine.<p />
 *
 * @author Matti Dahlbom
 * @version $Name:  $, $Revision: 17 $
 */
class TexturedCube : public ColorCube
    {
    public: // Constructors and destructor
        NOVA_IMPORT TexturedCube( NovaPixelFormat pixelFormat, 
				  real_64 radius = DefaultCubeRadius );
        NOVA_IMPORT virtual ~TexturedCube();

    protected: // New methods
	int Create();
        
    private: // Data
	Texture* m_texture;
    };

}; // namespace

#endif
