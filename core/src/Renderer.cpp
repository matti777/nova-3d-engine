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

#include "Renderer.h"
#include "FixedPoint.h"
#include "RenderingUtils.h"
#include "Texture.h"
#include "novalogging.h"

#define MIN(a,b) ((a)<(b) ? (a) : (b))

namespace nova3d {

Renderer::Renderer( RenderingCanvas& canvas )
    : m_canvas( canvas )
{
    for ( int i = 1; i <= 65535; i++ ) 
    {
        uint_32 result = MaxUint32 / i;
        m_fixedDivLookup[i] = (int_32)result;
    }
}

inline int_32 Renderer::DivLookup( int_32 fixedDivider )
{
    return m_fixedDivLookup[fixedDivider & 0xffff];
}

inline void Renderer::DrawGouraudSpan( int_32 x1, int_32 x2, 
				       int_32 red1, int_32 green1, 
				       int_32 blue1, 
				       int_32 red2, int_32 green2, 
				       int_32 blue2, 
				       uint_32* basePtr )
{
    // check that the span endpoints are ordered x1 < x2. if not, swap values
    if ( x1 > x2 ) 
    {
        ::Swap32( x1, x2 );
        ::Swap32( red1, red2 );
        ::Swap32( green1, green2 );
        ::Swap32( blue1, blue2 );
    }

    // check for 0-length span
    if ( (x2 - x1) < FixedPointOne ) 
    {
	return;
    }

    // extract integer span boundaries
    int_32 left = ::CeilFixed( x1 );
    int_32 right = ::CeilFixed( x2 );

    // check whether the span is completely outside the view port (canvas)
    if ( (left > m_canvas.m_right) || (right < m_canvas.m_left) ) 
    {
	return;
    }

    // check for <1-length span
    if ( (x2 - x1) < FixedPointOne ) 
    {
	return;
    }

    // setup 
    int_32 inv_len = (int_32)(MaxUint32 / (uint_32)(x2 - x1));
    register int_32 red_slope = ::FixedLargeMul( (red2 - red1), inv_len );
    register int_32 green_slope = ::FixedLargeMul( (green2 - green1), inv_len );
    register int_32 blue_slope = ::FixedLargeMul( (blue2 - blue1), inv_len );

    // clip to the rendering canvas'es left edge
    if ( left < m_canvas.m_left ) 
    {
        int_32 steps = (m_canvas.m_left - left);
        red1 += red_slope * steps;
        green1 += green_slope * steps;
        blue1 += blue_slope * steps;
        left = m_canvas.m_left;
    }

    // clip to the rendering canvas'es right edge
    if ( right >= m_canvas.m_right ) 
    {
	right = (m_canvas.m_right - 1);
    }

    //##TODO## well now. this wont work for <4byte pixel formats
    // due to the wide pointer.. now what
    // -> need to make another loop for lower modes with 16bit ptr

    int_32 len = right - left;
    uint_32* ptr = basePtr + left;

    // draw all pixels in span.
    for( int_32 i = 0; i < len; i++ ) 
    {
	*ptr++ = nova3d::CreateColor( m_canvas.m_pixelFormat, 
				      (red1 >> FixedPointPrec), 
				      (green1 >> FixedPointPrec),
				      (blue1 >> FixedPointPrec) );
        red1 += red_slope;
        green1 += green_slope;
        blue1 += blue_slope;
    }
}

void Renderer::DrawTriangle( ScreenPolygon* face )
{
    ScreenVertex *vertex1, *vertex2, *vertex3;

    // sort vertices in y direction so that v1 < v2 < v3
    nova3d::SelectVertexOrder( face, &vertex1, &vertex2, &vertex3 );
 
    int_32 y1 = ::CeilFixed( vertex1->m_y );
    int_32 y2 = ::CeilFixed( vertex2->m_y );
    int_32 y3 = ::CeilFixed( vertex3->m_y );

    // check for 0-height triangle
    if ( (vertex3->m_y - vertex1->m_y) < FixedPointOne ) 
    {
	return;
    }

    // setup
    int_32 topmost_y = m_canvas.m_top;
    int_32 lowest_y = MIN( y3, m_canvas.m_bottom ) - 1;
    int_32 cur_y = y1;
    //##TODO## check that this works
    uint_32* base_p = (uint_32*)((uint_32)m_canvas.m_bufferPtr + 
				 cur_y * m_canvas.m_bytesPerScanline);
    
    int_32 x1_slope = 0, x2_slope = 0, x1 = 0, x2 = 0;
    int_32 red1 = 0, red1_slope = 0, red2 = 0, red2_slope = 0;
    int_32 green1 = 0, green1_slope = 0, green2 = 0, green2_slope = 0;
    int_32 blue1 = 0, blue1_slope = 0, blue2 = 0, blue2_slope = 0;

    // calculate slope2 for v1-v3 (constant for whole scan)
    int_32 inv_len = 
	(int_32)(MaxUint32 / (uint_32)(vertex3->m_y - vertex1->m_y));
    x2 = vertex1->m_x;
    x2_slope = ::FixedLargeMul( (vertex3->m_x - x2), inv_len );
    red2 = vertex1->m_color.m_red;
    red2_slope = ::FixedLargeMul( (vertex3->m_color.m_red - red2), inv_len );
    green2 = vertex1->m_color.m_green;
    green2_slope = 
	::FixedLargeMul( (vertex3->m_color.m_green - green2), inv_len );
    blue2 = vertex1->m_color.m_blue;
    blue2_slope = ::FixedLargeMul( (vertex3->m_color.m_blue - blue2), inv_len );

    // if the edge v1-v2 has any height, calculate slope1 for it
    if ( y1 < y2 ) 
    {
        inv_len = (int_32)(MaxUint32 / (uint_32)(vertex2->m_y - vertex1->m_y));
        x1 = vertex1->m_x;
        x1_slope = ::FixedLargeMul( (vertex2->m_x - x1), inv_len );
        red1 = vertex1->m_color.m_red;
        red1_slope = 
	    ::FixedLargeMul( (vertex2->m_color.m_red - red1), inv_len );
        green1 = vertex1->m_color.m_green;
        green1_slope = 
	    ::FixedLargeMul( (vertex2->m_color.m_green - green1), inv_len );
        blue1 = vertex1->m_color.m_blue;
        blue1_slope = 
	    ::FixedLargeMul( (vertex2->m_color.m_blue - blue1), inv_len );
    }   

    // triangle edge scan loop
    while ( cur_y <= lowest_y ) 
    { 
        if ( cur_y == y2 ) 
	{
            // check if there is no v2-v3 slope (the triangle has flat bottom)
            if( y3 == y2 ) 
	    {
                return;
            }

            // need to recalculate slope1 for v2-v3
            inv_len = 
		(int_32)(MaxUint32 / (uint_32)(vertex3->m_y - vertex2->m_y));
            x1 = vertex2->m_x;
            x1_slope = ::FixedLargeMul( (vertex3->m_x - x1), inv_len );
            red1 = vertex2->m_color.m_red;
            red1_slope = 
		::FixedLargeMul( (vertex3->m_color.m_red - red1), inv_len );
            green1 = vertex2->m_color.m_green;
            green1_slope = 
		::FixedLargeMul( (vertex3->m_color.m_green - green1), inv_len );
            blue1 = vertex2->m_color.m_blue;
            blue1_slope = 
		::FixedLargeMul( (vertex3->m_color.m_blue - blue1), inv_len );
        }

        if ( cur_y >= topmost_y ) 
	{
            DrawGouraudSpan( x1, x2, red1, green1, blue1, 
                             red2, green2, blue2, base_p );
        }

        cur_y++;
        x1 += x1_slope;
        x2 += x2_slope;
        red1 += red1_slope;
        green1 += green1_slope;
        blue1 += blue1_slope;
        red2 += red2_slope;
        green2 += green2_slope;
        blue2 += blue2_slope;
	
        base_p = (uint_32*)((uint_32)base_p + m_canvas.m_bytesPerScanline);
    }
}

inline void Renderer::DrawTexturedSpan( int_32 leftX, int_32 rightX, 
					int_32 leftU, int_32 leftV, 
					int_32 leftZ,
					int_32 dudx, int_32 dvdx, int_32 dzdx,
					uint_32 scanlinePtr, 
					Texture* texture )
{
    uint_8* tex_data = texture->GetData();
    uint_32* tex_palette = texture->GetPalette();
    uint_32 u_mask = texture->GetUMask();
    uint_32 v_mask = texture->GetVMask();
    int_32 texshift = texture->GetShift();

    // ceil() span endpoint Xs to integers
    int_32 left = ::CeilFixed( leftX );
    int_32 right = ::CeilFixed( rightX );

    //##TODO## can this be skipped if left side is clipped?
    
    // apply subtexel accuracy
    int_32 prestep = (left << FixedPointPrec) - leftX;
    leftU += ::FixedLargeMul( prestep, dudx );
    leftV += ::FixedLargeMul( prestep, dvdx );
    leftZ += ::FixedLargeMul( prestep, dzdx );

    // check whether the span is completely outside the view port (canvas)
    if ( (left > m_canvas.m_right) || (right < m_canvas.m_left) ) 
    {
	return;
    }

    // calculate span length
    int_32 len = right - left + 1;

    //##TODO## remove all this 2d clipping crap when frustum clipping is done

    // clip to the left side of the rendering canvas
    if ( left < m_canvas.m_left ) 
    {
        // clip u,v,z values 
        int_32 diff = (m_canvas.m_left << FixedPointPrec) - leftX;
        leftU += ::FixedLargeMul( diff, dudx );
        leftV += ::FixedLargeMul( diff, dvdx );
        leftZ += ::FixedLargeMul( diff, dzdx );

        // adjust the left side X coordinate and length accordingly
        len -= m_canvas.m_left - left;
        left = m_canvas.m_left;
    }

    // clip to the right side of the rendering canvas
    if ( right >= m_canvas.m_right ) 
    {
        len = m_canvas.m_right - left;
    }

    // check whether still something left to draw
    if ( len <= 0 ) 
    {
	return;
    }

    //##TODO## 2-byte pixel mode support

    // calculate the address to start writing from
    uint_32* p = (uint_32*)scanlinePtr + left;

    while( len > 0 ) 
    {
	int_64 real_z = DivLookup( leftZ );
	int_32 real_u = (int_32)( ((int_64)leftU * real_z) >> 32);
	int_32 real_v = (int_32)( ((int_64)leftV * real_z) >> 32);

        uint_8 value = tex_data[(real_u & u_mask) + 
				(((real_v & v_mask) << texshift))];
        uint_32 color = tex_palette[value];
        *p++ = color;

        leftU += dudx;
        leftV += dvdx;
        leftZ += dzdx;
        len--;
    }
}

void Renderer::DrawTexturedTriangle( ScreenPolygon* face )
{
    ScreenVertex *vertex1, *vertex2, *vertex3;

    // sort vertices in y direction so that vertex1 < vertex2 < vertex3
    nova3d::SelectVertexOrder( face, &vertex1, &vertex2, &vertex3 );

    // calculate dudx, dvdx, dzdx (constant through whole polygon)
    int_32 dudx, dvdx, dzdx, didx = -1;
    nova3d::CalculatePolygonGradients( *vertex1, *vertex2, *vertex3, 
				       dudx, dvdx, dzdx, didx );

    // get the vertex y coordinates by ceil()ing from the accurate values
    int_32 y1 = ::CeilFixed( vertex1->m_y );
    int_32 y2 = ::CeilFixed( vertex2->m_y );
    int_32 y3 = ::CeilFixed( vertex3->m_y );
    
    // calculate gradients for v1-v3 (constant for whole scan) and check 
    // whether the long edge (v1-v3) is "on left" 
    int_32 long_inv_len, long_x = 0, long_dxdy = 0;
    bool long_on_left = nova3d::IsLongOnLeft( *vertex1, *vertex2, *vertex3, 
					      long_inv_len, long_x, long_dxdy );

    // the interpolated values start from v1 (unless len(v2-v1)=0)
    int_32 left_x = long_x;
    int_32 left_u = vertex1->m_textureCoordinates.m_u;
    int_32 left_v = vertex1->m_textureCoordinates.m_v;
    int_32 left_z = vertex1->m_z;

    int_32 left_dxdy = 0, left_dudy = 0, left_dvdy = 0, left_dzdy = 0;
    int_32 right_x = 0, right_dxdy = 0;

    if ( long_on_left ) 
    {
        left_dxdy = long_dxdy;
        left_dudy = ::FixedLargeMul( (vertex3->m_textureCoordinates.m_u - 
				      left_u), long_inv_len );
        left_dvdy = ::FixedLargeMul( (vertex3->m_textureCoordinates.m_v - 
				      left_v), long_inv_len );
        left_dzdy = ::FixedLargeMul( (vertex3->m_z - left_z), long_inv_len );

        if ( y2 > y1 ) 
	{
            // calculate 'right' values for v1-v2
            int_32 len = (vertex2->m_y - vertex1->m_y) >> 8;
            if ( len > 0 ) 
	    {
                right_x = vertex1->m_x;
                right_dxdy = ((vertex2->m_x - vertex1->m_x) / len) << 8;
            }
        }
    } 
    else 
    {
        // long edge is on right
        right_x = long_x;
        right_dxdy = long_dxdy;

        if ( y2 > y1 ) 
	{
            // calculate 'left' values for v1-v2
            int_32 inv_len = (int_32)(MaxUint32 / 
				      (uint_32)(vertex2->m_y - vertex1->m_y));
            left_dxdy = ::FixedLargeMul( (vertex2->m_x - left_x), inv_len );
            left_dudy = ::FixedLargeMul( (vertex2->m_textureCoordinates.m_u - 
					  left_u), inv_len );
            left_dvdy = ::FixedLargeMul( (vertex2->m_textureCoordinates.m_v - 
					  left_v), inv_len );
            left_dzdy = ::FixedLargeMul( (vertex2->m_z - left_z), inv_len );
        }
    }

    // apply subpixel accuracy
    int_32 prestep = (y1 << FixedPointPrec) - vertex1->m_y;
    left_x += ::FixedLargeMul( prestep, left_dxdy );
    left_u += ::FixedLargeMul( prestep, left_dudy );
    left_v += ::FixedLargeMul( prestep, left_dvdy );
    left_z += ::FixedLargeMul( prestep, left_dzdy );
    right_x += ::FixedLargeMul( prestep, right_dxdy );

    // setup for drawing
    int_32 lowest_y = MIN( y3, m_canvas.m_bottom ) - 1;
    uint_32 scanline_ptr = (uint_32)((uint_32)m_canvas.m_bufferPtr + 
				     y1 * m_canvas.m_bytesPerScanline);
    int_32 cur_y = y1;

    // triangle edge scan loop
    while ( cur_y <= lowest_y ) 
    { 
        if( cur_y == y2 ) 
	{
            // check if there is no v2-v3 slope (the triangle has flat bottom)
	    //##TODO## move out of the loop?
            if ( y3 == y2 ) 
	    {
                return;
            }

            int_32 prestep = (y2 << FixedPointPrec) - vertex2->m_y;

            if ( long_on_left ) 
	    {
                // update right side stuff for v2-v3
                int_32 len = (vertex3->m_y - vertex2->m_y) >> 8;
                if ( len <= 0 ) 
		{
		    // there is no right side
		    return;
		}

                right_x = vertex2->m_x;
                right_dxdy = ((vertex3->m_x - vertex2->m_x) / len) << 8;

                // apply subpixel accuracy
                right_x += ::FixedLargeMul( prestep, right_dxdy );
            } 
	    else 
	    {
                // update left side stuff for v2-v3
                int_32 inv_len = (int_32)(MaxUint32 / (uint_32)(vertex3->m_y - 
								vertex2->m_y));
                left_x = vertex2->m_x;
                left_u = vertex2->m_textureCoordinates.m_u;
                left_v = vertex2->m_textureCoordinates.m_v;
                left_z = vertex2->m_z;
                left_dxdy = ::FixedLargeMul( (vertex3->m_x - left_x), inv_len );
                left_dudy = 
		    ::FixedLargeMul( (vertex3->m_textureCoordinates.m_u - 
				      left_u), inv_len );
                left_dvdy = 
		    ::FixedLargeMul( (vertex3->m_textureCoordinates.m_v - 
				      left_v), inv_len );
                left_dzdy = ::FixedLargeMul( (vertex3->m_z - left_z), inv_len );

                // apply subpixel accuracy
                left_x += ::FixedLargeMul( prestep, left_dxdy );
                left_u += ::FixedLargeMul( prestep, left_dudy );
                left_v += ::FixedLargeMul( prestep, left_dvdy );
                left_z += ::FixedLargeMul( prestep, left_dzdy );
            }
        }

        if ( cur_y >= m_canvas.m_top ) 
	{
            DrawTexturedSpan( left_x, right_x, left_u, left_v, left_z, 
			      dudx, dvdx, dzdx, 
                              scanline_ptr, face->m_texture );
        }

        // increment values for next scanline
        left_x += left_dxdy;
        left_u += left_dudy;
        left_v += left_dvdy;
        left_z += left_dzdy;
        right_x += right_dxdy;
        scanline_ptr += m_canvas.m_bytesPerScanline;
        cur_y++;
    }
}

inline void Renderer::DrawLightedTexturedSpan( int_32 leftX, int_32 rightX, 
					       int_32 leftU, int_32 leftV, 
					       int_32 leftZ,
					       int_32 intensityLeft, 
					       int_32 dudx, int_32 dvdx, 
					       int_32 dzdx, int_32 didx, 
					       uint_32 scanlinePtr, 
					       Texture* texture )
{
    uint_8* tex_data = texture->GetData();
    uint_32* tex_palettes = texture->GetPalette();
    uint_32 u_mask = texture->GetUMask();
    uint_32 v_mask = texture->GetVMask();
    int_32 texshift = texture->GetShift();

    // ceil() span endpoint Xs to integers
    int_32 left = ::CeilFixed( leftX );
    int_32 right = ::CeilFixed( rightX );

    //##TODO## can this be skipped if left side is clipped?
    
    // apply subtexel accuracy
    int_32 prestep = (left << FixedPointPrec) - leftX;
    leftU += ::FixedLargeMul( prestep, dudx );
    leftV += ::FixedLargeMul( prestep, dvdx );
    leftZ += ::FixedLargeMul( prestep, dzdx );

    // check whether the span is completely outside the view port (canvas)
    if ( (left > m_canvas.m_right) || (right < m_canvas.m_left) ) 
    {
	return;
    }

    // calculate span length
    int_32 len = right - left + 1;

    //##TODO## remove all this 2d clipping crap when frustum clipping is done

    // clip to the left side of the rendering canvas
    if ( left < m_canvas.m_left ) 
    {
        // clip u,v,z values 
        int_32 diff = (m_canvas.m_left << FixedPointPrec) - leftX;
        leftU += ::FixedLargeMul( diff, dudx );
        leftV += ::FixedLargeMul( diff, dvdx );
        leftZ += ::FixedLargeMul( diff, dzdx );
	intensityLeft += ::FixedLargeMul( diff, didx );

        // adjust the left side X coordinate and length accordingly
        len -= m_canvas.m_left - left;
        left = m_canvas.m_left;
    }

    // clip to the right side of the rendering canvas
    if ( right >= m_canvas.m_right ) 
    {
        len = m_canvas.m_right - left;
    }

    // check whether still something left to draw
    if ( len <= 0 ) 
    {
	return;
    }

    //##TODO## 2-byte pixel mode support

    // calculate the address to start writing from
    uint_32* p = (uint_32*)scanlinePtr + left;

    while( len > 0 ) 
    {
	int_64 real_z = DivLookup( leftZ );
	int_32 real_u = (int_32)( ((int_64)leftU * real_z) >> 32);
	int_32 real_v = (int_32)( ((int_64)leftV * real_z) >> 32);

        uint_8 value = tex_data[(real_u & u_mask) + 
				(((real_v & v_mask) << texshift))];
	uint_32* palette = 
	    tex_palettes + (intensityLeft >> FixedPointPrec) * 
	    Texture::NumPaletteEntries;
        uint_32 color = palette[value];
        *p++ = color;

        leftU += dudx;
        leftV += dvdx;
        leftZ += dzdx;
	intensityLeft += didx;
        len--;
    }
}

void Renderer::DrawLightedTexturedTriangle( ScreenPolygon* face )
{
    ScreenVertex *vertex1, *vertex2, *vertex3;

    // sort vertices in y direction so that vertex1 < vertex2 < vertex3
    nova3d::SelectVertexOrder( face, &vertex1, &vertex2, &vertex3 );

    // calculate dudx, dvdx, dzdx (constant through whole polygon)
    int_32 dudx, dvdx, dzdx, didx = 0;
    nova3d::CalculatePolygonGradients( *vertex1, *vertex2, *vertex3, 
				       dudx, dvdx, dzdx, didx );

    // get the vertex y coordinates by ceil()ing from the accurate values
    int_32 y1 = ::CeilFixed( vertex1->m_y );
    int_32 y2 = ::CeilFixed( vertex2->m_y );
    int_32 y3 = ::CeilFixed( vertex3->m_y );
    
    // calculate gradients for v1-v3 (constant for whole scan) and check 
    // whether the long edge (v1-v3) is "on left" 
    int_32 long_inv_len, long_x, long_dxdy;
    bool long_on_left = nova3d::IsLongOnLeft( *vertex1, *vertex2, *vertex3, 
					      long_inv_len, long_x, long_dxdy );

    // the interpolated values start from v1 (unless len(v2-v1)=0)
    int_32 left_x = long_x;
    int_32 left_u = vertex1->m_textureCoordinates.m_u;
    int_32 left_v = vertex1->m_textureCoordinates.m_v;
    int_32 left_z = vertex1->m_z;
    int_32 left_intensity = vertex1->m_textureCoordinates.m_intensity;

    int_32 left_dxdy, left_dudy, left_dvdy, left_dzdy, left_didy;
    int_32 right_x, right_dxdy;

    if ( long_on_left ) 
    {
        left_dxdy = long_dxdy;
        left_dudy = ::FixedLargeMul( (vertex3->m_textureCoordinates.m_u - 
				      left_u), long_inv_len );
        left_dvdy = ::FixedLargeMul( (vertex3->m_textureCoordinates.m_v - 
				      left_v), long_inv_len );
        left_dzdy = ::FixedLargeMul( (vertex3->m_z - left_z), long_inv_len );
	left_didy = 
	    ::FixedLargeMul( (vertex3->m_textureCoordinates.m_intensity - 
			      left_intensity), long_inv_len );

        if ( y2 > y1 ) 
	{
            // calculate 'right' values for v1-v2
            int_32 len = (vertex2->m_y - vertex1->m_y) >> 8;
            if ( len > 0 ) 
	    {
                right_x = vertex1->m_x;
                right_dxdy = ((vertex2->m_x - vertex1->m_x) / len) << 8;
            }
        }
    } 
    else 
    {
        // long edge is on right
        right_x = long_x;
        right_dxdy = long_dxdy;

        if ( y2 > y1 ) 
	{
            // calculate 'left' values for v1-v2
            int_32 inv_len = (int_32)(MaxUint32 / 
				      (uint_32)(vertex2->m_y - vertex1->m_y));
            left_dxdy = ::FixedLargeMul( (vertex2->m_x - left_x), inv_len );
            left_dudy = ::FixedLargeMul( (vertex2->m_textureCoordinates.m_u - 
					  left_u), inv_len );
            left_dvdy = ::FixedLargeMul( (vertex2->m_textureCoordinates.m_v - 
					  left_v), inv_len );
            left_dzdy = ::FixedLargeMul( (vertex2->m_z - left_z), inv_len );
	    left_didy = 
		::FixedLargeMul( (vertex2->m_textureCoordinates.m_intensity - 
				  left_intensity), inv_len );
        }
    }

    // apply subpixel accuracy
    int_32 prestep = (y1 << FixedPointPrec) - vertex1->m_y;
    left_x += ::FixedLargeMul( prestep, left_dxdy );
    left_u += ::FixedLargeMul( prestep, left_dudy );
    left_v += ::FixedLargeMul( prestep, left_dvdy );
    left_z += ::FixedLargeMul( prestep, left_dzdy );
    left_intensity += ::FixedLargeMul( prestep, left_didy );
    right_x += ::FixedLargeMul( prestep, right_dxdy );

    // setup for drawing
    int_32 lowest_y = MIN( y3, m_canvas.m_bottom ) - 1;
    uint_32 scanline_ptr = (uint_32)((uint_32)m_canvas.m_bufferPtr + 
				     y1 * m_canvas.m_bytesPerScanline);
    int_32 cur_y = y1;

    // triangle edge scan loop
    while ( cur_y <= lowest_y ) 
    { 
        if( cur_y == y2 ) 
	{
            // check if there is no v2-v3 slope (the triangle has flat bottom)
	    //##TODO## move out of the loop?
            if ( y3 == y2 ) 
	    {
                return;
            }

            int_32 prestep = (y2 << FixedPointPrec) - vertex2->m_y;

            if ( long_on_left ) 
	    {
                // update right side stuff for v2-v3
                int_32 len = (vertex3->m_y - vertex2->m_y) >> 8;
                if ( len <= 0 ) 
		{
		    // there is no right side
		    return;
		}

                right_x = vertex2->m_x;
                right_dxdy = ((vertex3->m_x - vertex2->m_x) / len) << 8;

                // apply subpixel accuracy
                right_x += ::FixedLargeMul( prestep, right_dxdy );
            } 
	    else 
	    {
                // update left side stuff for v2-v3
                int_32 inv_len = (int_32)(MaxUint32 / (uint_32)(vertex3->m_y - 
								vertex2->m_y));
                left_x = vertex2->m_x;
                left_u = vertex2->m_textureCoordinates.m_u;
                left_v = vertex2->m_textureCoordinates.m_v;
                left_z = vertex2->m_z;
		left_intensity = vertex2->m_textureCoordinates.m_intensity;
                left_dxdy = ::FixedLargeMul( (vertex3->m_x - left_x), inv_len );
                left_dudy = 
		    ::FixedLargeMul( (vertex3->m_textureCoordinates.m_u - 
				      left_u), inv_len );
                left_dvdy = 
		    ::FixedLargeMul( (vertex3->m_textureCoordinates.m_v - 
				      left_v), inv_len );
                left_dzdy = ::FixedLargeMul( (vertex3->m_z - left_z), inv_len );
                left_didy = 
		    ::FixedLargeMul( (vertex3->m_textureCoordinates.m_intensity - 
				      left_intensity), inv_len );

                // apply subpixel accuracy
                left_x += ::FixedLargeMul( prestep, left_dxdy );
                left_u += ::FixedLargeMul( prestep, left_dudy );
                left_v += ::FixedLargeMul( prestep, left_dvdy );
                left_z += ::FixedLargeMul( prestep, left_dzdy );
		left_intensity += ::FixedLargeMul( prestep, left_didy );
            }
        }

        if ( cur_y >= m_canvas.m_top ) 
	{
            DrawLightedTexturedSpan( left_x, right_x, left_u, left_v, left_z, 
				     left_intensity, 
				     dudx, dvdx, dzdx, didx, 
				     scanline_ptr, face->m_texture );
        }

        // increment values for next scanline
        left_x += left_dxdy;
        left_u += left_dudy;
        left_v += left_dvdy;
        left_z += left_dzdy;
	left_intensity += left_didy;
        right_x += right_dxdy;
        scanline_ptr += m_canvas.m_bytesPerScanline;
        cur_y++;
    }
}

}; // namespace
