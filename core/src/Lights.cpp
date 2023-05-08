/*
 *  $Id: Lights.cpp 18 2009-09-04 10:49:50Z matti $
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

#include "Lights.h"
#include "NovaErrors.h"

namespace nova3d {

//////////////////////////////////////////////
// implementation of Light
//////////////////////////////////////////////

Light::Light( Light::Type type )
    : m_type ( type )
{
}

NOVA_EXPORT Light::~Light()
{
}

//////////////////////////////////////////////
// implementation of AmbientLight
//////////////////////////////////////////////

NOVA_EXPORT AmbientLight::AmbientLight( real_64 intensity )
    : Light( Light::TypeAmbient ),
      m_intensity( ::RealToFixed( intensity ) )
{
}

NOVA_EXPORT AmbientLight::AmbientLight( const AmbientLight& other )
    : Light( Light::TypeAmbient ),
      m_intensity( other.m_intensity )
{
}

NOVA_EXPORT AmbientLight::~AmbientLight()
{
}

//////////////////////////////////////////////
// implementation of PointLight
//////////////////////////////////////////////

NOVA_EXPORT PointLight::PointLight()
    : Light( Light::TypePoint ), 
      m_isAttenuated( true ), 
      m_att0( 0 ), 
      m_att1( 0 ),
      m_att2( FixedPointOne )
{
}

NOVA_EXPORT PointLight::~PointLight()
{
}

NOVA_EXPORT int PointLight::SetAttenuation( real_64 att0, real_64 att1, 
                                            real_64 att2 )
{
    if ( (att0 < 0) || (att1 < 0) || (att2 < 0) )
    {
        return NovaErrInvalidArgument;
    }
    
    m_att0 = ::RealToFixed( att0 );
    m_att1 = ::RealToFixed( att1 );
    m_att2 = ::RealToFixed( att2 );
    
    return NovaErrNone;
}

}; // namespace
