/*
 *  $Id: Texture.h 23 2009-09-10 13:57:16Z matti $
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

#ifndef __TEXTURE_H
#define __TEXTURE_H

// FILE INFO
// This file describes the class used to contain textures.

#include "NovaTypes.h"
#include "Display.h"

namespace nova3d {

/**
 * Represents a texture used by the texture mapping routines. The texture 
 * is a 8bit indexed image, with a palette indexable by the values in 
 * the texture map. The palette itself, although limited to 256 entries,
 * may contain any colors within the current color scheme.<p />
 *
 * @author Matti Dahlbom
 * @version $Name:  $, $Revision: 23 $
 */
class Texture 
{
 public: 
    // number of entries per palette
    static const int NumPaletteEntries = 256;

    // texture filter mode (NOTE: must not change these values!)
    enum FilterMode
    {
        EFilterNone = 0x00, 
        EFilterBilinear = 0x01
    };

 public: // Constructors and destructor
    NOVA_IMPORT Texture();
    NOVA_IMPORT ~Texture();
        
 public: // New methods (Public API)
    /**
     * Creates the texture. The texture object will make its own copy
     * of the palette and data and they can be deallocated after this call
     * returns. The texture data must be organized
     * in memory linearly from left to right, top to bottom. The data
     * values are indices to the palette.<p />
     */
    NOVA_IMPORT int Create( NovaPixelFormat pixelFormat, 
			    int width, int height,
			    uint_32* palette, uint_8* data );

    NOVA_IMPORT int CreateLinearPalettes( real_64 gain = 1.0 );
        
    /**
     * Convenience method for creating a linear array of palettes for 
     * illumination. The linear multiplier goes from 0.0 to 
     * <code>aGain</code>. The color components are capped to their 
     * respective maximim values so there is no possibility of
     * overflows. Use gain values of >1.0 to produce a palette array with 
     * "highlight" effect. The default palette is used as the base of the 
     * calculations.<p />
     *
     * @param aGain gain value. default is 1.0
     */
    //        NOVA_IMPORT int CreateLinearPalettes( real_64 aGain = 1.0 );
  
    /** Returns the width of the texture. */
    inline int GetWidth() const;
        
    /** Returns the height of the texture. */
    inline int GetHeight() const;
        
    /**
     * This is amount of bits to shift the v-component (vertical) of the 
     * texture coefficients to address a given pixel in the texture: 
     * iData[u + v*iWidth] = iData[u + (v << iShift)]
     */
    inline uint_32 GetShift() const;

    inline uint_32 GetUMask() const;
    inline uint_32 GetVMask() const;
    inline int GetNumPalettes() const;
    inline NovaPixelFormat GetPixelFormat() const;

    /**
     * Returns the texture palette(s). In case of multiple palette(s), the 
     * first palette should be the one with most lighting and the last with
     * the least lighting.<P>
     *
     * There are <code>GetNumPalettes() * KNumPaletteEntries</code> palette
     * entries in the array.<P>
     */
    inline uint_32* GetPalette() const;

    /** Returns texture pixel data, as 8-bit indexes to the palette */
    inline uint_8* GetData() const;

    /** Scales an intensity value to the texture's palette range */
    void ScaleIntensity( int_32& intensity ) const;
        
 private: // Data
    int m_width;
    int m_height;
    uint_32 m_shift;
    uint_32 m_umask;
    uint_32 m_vmask;

    // pixel format of this texture
    NovaPixelFormat m_pixelFormat;
        
    // number of palettes; each palette consists of 256 entries
    int m_numPalettes;
    int m_numPalettesShift;
    uint_32* m_palette;

    // indexed texture data
    uint_8* m_data;
};

///////////////////////////////////////////////////
// inline method definitions
///////////////////////////////////////////////////

int Texture::GetWidth() const
{
    return m_width;
}
        
int Texture::GetHeight() const
{
    return m_height;
}

uint_32 Texture::GetShift() const
{
    return m_shift;
}

uint_32 Texture::GetUMask() const
{
    return m_umask;
}

uint_32 Texture::GetVMask() const
{
    return m_vmask;
}

int Texture::GetNumPalettes() const
{
    return m_numPalettes;
}

uint_32* Texture::GetPalette() const
{
    return m_palette;
}

uint_8* Texture::GetData() const
{
    return m_data;
}

NovaPixelFormat Texture::GetPixelFormat() const
{
    return m_pixelFormat;
}

}; // namespace

#endif
