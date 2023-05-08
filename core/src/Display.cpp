/*
 *  $Id: Display.cpp 22 2009-09-09 17:25:01Z matti $
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

#include "Display.h"
#include "FixedPoint.h"

namespace nova3d {

// returns maximum values for color components in the given pixel format
void GetColorLimits( NovaPixelFormat pixelFormat, 
		     int_32& red, int_32& green, int_32& blue )
{
    switch ( pixelFormat )
    {
    case PixelFormat888:
	red = 255;
	green = 255;
	blue = 255;
	break;
    case PixelFormat444:
	red = 15;
	green = 15;
	blue = 15;
	break;
    case PixelFormat555:
	red = 31;
	green = 31;
	blue = 31;
	break;
    case PixelFormat565:
	red = 31;
	green = 63;
	blue = 31;
	break;
    case PixelFormat666:
	red = 63;
	green = 63;
	blue = 63;	
	break;
    case PixelFormatUndefined:
    default: // bad pixel format
	break;
    };
}

NOVA_EXPORT uint_32 CreateColor( NovaPixelFormat pixelFormat, 
                                 int red, int green, int blue )
{
    switch ( pixelFormat )
    {
    case PixelFormat888:
	return PIXEL_888( red, green, blue );
	break;
    case PixelFormat444:
	return PIXEL_444( red, green, blue );
	break;
    case PixelFormat565:
	return PIXEL_565( red, green, blue );
	break;
    case PixelFormat555:
	return PIXEL_555( red, green, blue );
	break;
    case PixelFormat666:
	return PIXEL_666( red, green, blue );
	break;
    case PixelFormatUndefined:
    default: // bad pixel format
	return 0;
	break;
    };
}

NOVA_EXPORT uint_32 ConvertColor( uint_32 color, NovaPixelFormat fromFormat, 
				  NovaPixelFormat toFormat )
{
    int_32 red, green, blue;
    SplitColor( fromFormat, color, red, green, blue );
    return CreateColor( toFormat, red, green, blue );
}

NOVA_EXPORT void SplitColor( NovaPixelFormat pixelFormat, uint_32 color, 
			     int_32& red, int_32& green, int_32& blue )
{
    SplitColorIntoFixedPoint( pixelFormat, color, red, green, blue );
    red >>= FixedPointPrec;
    green >>= FixedPointPrec;
    blue >>= FixedPointPrec;
}

NOVA_EXPORT void SplitColorIntoFixedPoint( NovaPixelFormat pixelFormat, 
					   uint_32 color, 
					   int& red, int& green, int& blue )
{
    switch ( pixelFormat )
    {
    case PixelFormat888:
	red = color & 0xff0000;
	green = (color << (FixedPointPrec - 8)) & 0xff0000;
	blue = (color << FixedPointPrec) & 0xff0000;
	break;
    case PixelFormat444:
	red = (color << (FixedPointPrec - 8)) & 0xf0000;
	green = (color << (FixedPointPrec - 4)) & 0xf0000;
	blue = (color << FixedPointPrec) & 0xf0000;
	break;
    case PixelFormat565:
	red = (color << (FixedPointPrec - 11)) & 0x1f0000;
	green = (color << (FixedPointPrec - 5)) & 0x3f0000;
	blue = (color << FixedPointPrec) & 0x1f0000;
	break;
    case PixelFormat555:
	red = (color << (FixedPointPrec - 10)) & 0x1f0000;
	green = (color << (FixedPointPrec - 5)) & 0x1f0000;
	blue = (color << FixedPointPrec) & 0x1f0000;
	break;
    case PixelFormat666:
	red = (color << (FixedPointPrec - 12)) & 0x3f0000;
	green = (color << (FixedPointPrec - 6)) & 0x3f0000;
	blue = (color << FixedPointPrec) & 0x3f0000;
	break;
    case PixelFormatUndefined:
    default: // bad pixel format
	red = -1;
	green = -1;
	blue = -1;
	break;
    };
}

NOVA_EXPORT void ScaleColor( NovaPixelFormat pixelFormat, 
			     int_32& red, int_32& green, int_32& blue, 
			     int_32 scaler )
{
    red = ::FixedLargeMul( red, scaler );
    green = ::FixedLargeMul( green, scaler );
    blue = ::FixedLargeMul( blue, scaler );

    int_32 redLimit, greenLimit, blueLimit;
    GetColorLimits( pixelFormat, redLimit, greenLimit, blueLimit );

    if ( red > redLimit ) red = redLimit;
    if ( green > greenLimit ) green = greenLimit;
    if ( blue > blueLimit ) blue = blueLimit;
}

}; // namespace
