/*
 *  $Id: Lights.h 17 2009-09-02 09:03:47Z matti $
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

#ifndef __LIGHTS_H
#define __LIGHTS_H

// FILE INFO
// This file contains declarations for classes that represent lights.

#include "NovaTypes.h"
#include "FixedPoint.h"
#include "VectorMath.h"

namespace nova3d {

// forward declarations
//class LightNode;

/**
 * Abstract base class for all lights.<p />
 *
 * @author Matti Dahlbom
 * @version $Name:  $, $Revision: 17 $
 */
class Light 
{
 public: // public enums
    enum Type
    {
	TypeAmbient, 
	TypePoint
    };

 public: // Constructors and destructor
    NOVA_IMPORT virtual ~Light();

 public: // New methods
    inline Type GetType() const;
        
 protected: // Constructors
    Light( Type type );
        
 private: // Data
    // light type
    Type m_type;
        
    // friend declarations
    //        friend class LightNode;
};

/**
 * Ambient light describes the amount of light received throughout the scene.
 * <p />
 *
 * @author Matti Dahlbom
 * @version $Name:  $, $Revision: 17 $
 */
class AmbientLight : public Light
{
 public: // Constructors and destructor
    NOVA_IMPORT AmbientLight( real_64 intensity = 0.0 );
    NOVA_IMPORT AmbientLight( const AmbientLight& other );
    NOVA_IMPORT virtual ~AmbientLight();

 public: // New methods
    /** Returns the ambient light intensity as fixed point */
    inline int_32 IntensityFixed() const;

    /** Returns the ambient light intensity as real */
    inline real_64 IntensityReal() const;

 private: // Data
    // ambient light intensity as fixed point
    int_32 m_intensity;
};

/**
 * Point light is a light source which has a location but no direction.
 * This class supports attenuation settings to control how the light
 * intensity diminishes with distance from the light source.<p /> 
 *
 * @author Matti Dahlbom
 * @version $Name:  $, $Revision: 17 $
 */
class PointLight : public Light
{
 public: // Constructors and destructor
    NOVA_IMPORT PointLight();
    NOVA_IMPORT virtual ~PointLight();

 public: // new methods (public API)
    /**
     * Sets the light attenuation coefficients. The attenuation 
     * (as a function of distance between this point light source and 
     * a location) is:<p />
     * <pre>
     * f(d) = 1/(att0 + att1*d + att2*d^2)
     * </pre>
     * <p />
     *
     * By setting att1 = att2 = 0 and placing the light source "infinitely" 
     * far the point light will act as a directional light source.<p>
     *
     * All the arguments must greater than or equal to zero. 
     * The default values are 0.0, 0.0, 1.0, respectively.<p />
     */
    NOVA_IMPORT int SetAttenuation( real_64 att0, real_64 att1, 
				    real_64 att2 );

    /**
     * Sets whether this point light source is attenuated by the distance 
     * to the illuminated vertex. The default is true.
     */
    inline void SetAttenuated( bool attenuated );

    /**
     * Returns the vaue of is attenuated -property.
     */
    inline bool IsAttenuated() const; 

    /**
     * Calculates the light intensity as the function of the distance.<P>
     *
     * @param distance as fixed point
     * @return attenuation factor as fixed point
     */
    inline int_32 CalculateAttenuationFactor( int_32 distance ) const;

    /** Returns the position of the light source */
    inline Vector& GetPosition();
        
    /** Sets the position of this point light */
    inline void SetPosition( const Vector& position );
        
 private: // Data
    // whether this light is attenuated
    bool m_isAttenuated;

    // attenuation coefficients as fixed point values
    int_32 m_att0;
    int_32 m_att1;
    int_32 m_att2;

    // light position in the coordinate system it was last transformed to.
    // the position is extracted from the light node's light matrix every
    // time it gets transformed
    Vector m_position;
};

// inline method definitions
// Light inline method definitions
Light::Type Light::GetType() const
{
    return m_type;
}

// AmbientLight inline method definitions
int_32 AmbientLight::IntensityFixed() const
{
    return m_intensity;
}

real_64 AmbientLight::IntensityReal() const
{
    return ::FixedToReal( m_intensity );
}

// PointLight inline method definitions
Vector& PointLight::GetPosition()
{
    return m_position;
}

void PointLight::SetPosition( const Vector& position )
{
    m_position.Set( position );
}

void PointLight::SetAttenuated( bool attenuated )
{
    m_isAttenuated = attenuated;
}

bool PointLight::IsAttenuated() const
{
    return m_isAttenuated;
}

int_32 PointLight::CalculateAttenuationFactor( int_32 distance ) const
{
    //##TODO##
    return 0;
}

}; // namespace

#endif
