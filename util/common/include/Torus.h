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

#ifndef __TORUS_H
#define __TORUS_H

// FILE INFO
// This is a simple torus object. This code can be used for
// quickly plugging a simple object into a Nova3D scene graph for testing
// purposes, or just as an example of how to use the APIs.

#include "Shape.h"
#include "NovaTypes.h"
#include "Display.h"
#include "Texture.h"

namespace nova3d {

/**
 * A simple torus ("donut") for testing out the polygon engine.
 * The torus will be created along the XZ plane, the hole pointing 
 * towards the positive Y axis.<p />
 *
 * @author Matti Dahlbom
 * @version $Name:  $, $Revision: 17 $
 */
class Torus : public Shape
{
 public: // Constructors and destructor
    NOVA_IMPORT Torus( NovaPixelFormat pixelFormat, 
		       real_64 torusRadius, 
		       real_64 bodyRadius, 
		       int torusSections, 
		       int bodySections );
    NOVA_IMPORT virtual ~Torus();
    
 protected: // New methods
    int Create( real_64 torusRadius, 
		real_64 bodyRadius, 
		int torusSections, 
		int bodySections);
    
 private: // Data
    
};

}; // namespace

#endif
