/*
 *  $Id: VectorMath.cpp 23 2009-09-10 13:57:16Z matti $
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

#include <math.h>
#include <string.h>

#include "VectorMath.h"
#include "FixedPoint.h"
#include "novalogging.h"

namespace nova3d {

//////////////////////////////////////////////
// implementation of Vector
//////////////////////////////////////////////

NOVA_EXPORT Vector::Vector( real_64 x, real_64 y, real_64 z )
    : m_x( ::RealToFixed( x ) ),
      m_y( ::RealToFixed( y ) ),
      m_z( ::RealToFixed( z ) )
{
}

NOVA_EXPORT Vector::Vector()
    : m_x( 0 ),
      m_y( 0 ),
      m_z( 0 )
{
}

NOVA_EXPORT Vector::Vector( const Vector& vector )
    : m_x( vector.m_x ),
      m_y( vector.m_y ),
      m_z( vector.m_z )
{
}

NOVA_EXPORT Vector::Vector( const Vector& vector, real_64 scaler )
    : m_x( (int_32)(vector.m_x * scaler) ),
      m_y( (int_32)(vector.m_y * scaler) ),
      m_z( (int_32)(vector.m_z * scaler) )
{
}

NOVA_EXPORT Vector::Vector( const Vector& v1, const Vector& v2 )
    : m_x( v1.m_x - v2.m_x ),
      m_y( v1.m_y - v2.m_y ),
      m_z( v1.m_z - v2.m_z )
{
}

NOVA_EXPORT void Vector::SubstractAndSet( const Vector& v1, const Vector& v2 )
{
    m_x = v1.m_x - v2.m_x;
    m_y = v1.m_y - v2.m_y;
    m_z = v1.m_z - v2.m_z;
}

NOVA_EXPORT void Vector::SetReal( real_64 x, real_64 y, real_64 z )
{
    m_x = ::RealToFixed( x );
    m_y = ::RealToFixed( y );
    m_z = ::RealToFixed( z );
}

NOVA_EXPORT void Vector::SetFixed( int_32 x, int_32 y, int_32 z )
{
    m_x = x;
    m_y = y;
    m_z = z;
}

NOVA_EXPORT void Vector::Add( const Vector& vector )
{
    m_x += vector.m_x;
    m_y += vector.m_y;
    m_z += vector.m_z;
}

NOVA_EXPORT void Vector::Substract( const Vector& vector )
{
    m_x -= vector.m_x;
    m_y -= vector.m_y;
    m_z -= vector.m_z;
}

NOVA_EXPORT void Vector::CrossProductAndSet( const Vector& v1, 
                                             const Vector& v2 )
{
    int_32 i = ::FixedLargeMul( v1.m_y, v2.m_z ) - 
	::FixedLargeMul( v1.m_z, v2.m_y );
    int_32 j = -(::FixedLargeMul( v1.m_x, v2.m_z ) - 
		 ::FixedLargeMul( v1.m_z, v2.m_x ));
    int_32 k = ::FixedLargeMul( v1.m_x, v2.m_y ) - 
	::FixedLargeMul( v1.m_y, v2.m_x );
    
    m_x = i;
    m_y = j;
    m_z = k;
}

NOVA_EXPORT void Vector::Set( const Vector& vector )
{
    m_x = vector.m_x;
    m_y = vector.m_y;
    m_z = vector.m_z;
}

NOVA_EXPORT void Vector::Inverse()
{
    m_x = -m_x;
    m_y = -m_y;
    m_z = -m_z;
}

NOVA_EXPORT void Vector::CheckPrecision()
{
    // artificial check to make sure we have enough precision
    if ( ((m_x < 300) && (m_x > -300)) && 
         ((m_y < 300) && (m_y > -300)) && 
         ((m_z < 300) && (m_z > -300)) ) 
    {
        // no - multiply all components by a constant
        m_x <<= 5;
        m_y <<= 5;
        m_z <<= 5;
    } 
    else 
    {
        // make sure we dont have too big numbers to avoid overflows in multiplys
        while ( (m_x >= (250 << FixedPointPrec)) || 
                (m_y >= (250 << FixedPointPrec)) || 
                (m_z >= (250 << FixedPointPrec)) ) 
	{
            m_x >>= 1;
            m_y >>= 1;
            m_z >>= 1;
	}
    }
}

NOVA_EXPORT void Vector::Normalize()
{
    CheckPrecision();

    uint_32 len = ::FastSqrt( DotProductFixed( *this ) );

    // after the sqrt() the fixed frac. prec. will only be 8 bits so compensate
    uint_32 inv_len = ((FixedPointOne - 1) << (FixedPointPrec / 2)) / len;

    m_x = ::FixedLargeMul( m_x, inv_len );    
    m_y = ::FixedLargeMul( m_y, inv_len );    
    m_z = ::FixedLargeMul( m_z, inv_len );    
}

NOVA_EXPORT bool Vector::operator==( const Vector& vector ) const
{
    return ((m_x == vector.m_x) && (m_y == vector.m_y) && (m_z == vector.m_z));
}

NOVA_EXPORT void Vector::TransformAndSet( const Matrix& matrix, 
                                          const Vector& vector )
{
    // apply the rotational component
    register int_32 x = ::FixedTripleMul( matrix.m_data[0][0], vector.m_x,
                                          matrix.m_data[0][1], vector.m_y,
                                          matrix.m_data[0][2], vector.m_z );
    register int_32 y = ::FixedTripleMul( matrix.m_data[1][0], vector.m_x,
                                          matrix.m_data[1][1], vector.m_y,
                                          matrix.m_data[1][2], vector.m_z );
    register int_32 z = ::FixedTripleMul( matrix.m_data[2][0], vector.m_x,
                                          matrix.m_data[2][1], vector.m_y,
                                          matrix.m_data[2][2], vector.m_z );
    
    // add the translational component
    m_x = x + matrix.m_data[0][3];
    m_y = y + matrix.m_data[1][3];
    m_z = z + matrix.m_data[2][3];
}

NOVA_EXPORT void Vector::RotateAndSet( const Matrix& matrix, 
                                       const Vector& vector )
{
    // apply the rotational component
    register int_32 x = ::FixedTripleMul( matrix.m_data[0][0], vector.m_x,
                                          matrix.m_data[0][1], vector.m_y,
                                          matrix.m_data[0][2], vector.m_z );
    register int_32 y = ::FixedTripleMul( matrix.m_data[1][0], vector.m_x,
                                          matrix.m_data[1][1], vector.m_y,
                                          matrix.m_data[1][2], vector.m_z );
    register int_32 z = ::FixedTripleMul( matrix.m_data[2][0], vector.m_x,
                                          matrix.m_data[2][1], vector.m_y,
                                          matrix.m_data[2][2], vector.m_z );
    
    m_x = x;
    m_y = y;
    m_z = z;
}

NOVA_EXPORT real_64 Vector::AngleBetweenRadReal( const Vector& vector ) const
{
    real_64 lengths = LengthReal() * vector.LengthReal();
    real_64 angleCosine = DotProductReal( vector ) / lengths;
    if ( angleCosine > 1 )
    {
	// fix possible precision errors
	angleCosine = 1.0;
    }

    real_64 angle = acos( angleCosine );

    return angle;
}

NOVA_EXPORT real_64 Vector::LengthReal() const
{
    return ::FixedToReal( LengthFixed() );
}

NOVA_EXPORT int_32 Vector::LengthFixed() const
{
    // get dot product with self
    uint_32 dotp = DotProductFixed( *this );

    // after the sqrt() the fixed frac. prec. will only be 8 bits so compensate
    return (::FastSqrt( dotp ) << (FixedPointPrec / 2));
}

//////////////////////////////////////////////
// implementation of Matrix
//////////////////////////////////////////////

NOVA_EXPORT Matrix::Matrix()
{
    // set to identity
    SetIdentity();

    // create the trigonometric table
    for( int i = 0; i < TrigTableSize; i++ ) 
    {
        real_64 angleDeg = (real_64)i;
        real_64 angleRad = M_PI * angleDeg / 180.0;
        real_64 s = sin( angleRad );

        m_trigTable[i] = (int_32)(s * FixedPointFactor);
    }
}

NOVA_EXPORT Matrix::Matrix( const Matrix& matrix )
    : m_multiplications( matrix.m_multiplications )
{
    // just make a brute force memory copy of the another matrix's data
    memcpy( this->m_data, matrix.m_data, sizeof(m_data) );
    memcpy( this->m_trigTable, matrix.m_trigTable, sizeof(m_trigTable) );
}

NOVA_EXPORT void Matrix::CreateLookAt( const Vector& origin, 
                                       const Vector& target )
{
    // calculate "forward" vector and normalize it
    Vector forward( target );
    forward.Substract( origin );
    forward.Normalize();

    // calculate dot production of "world up" & "forward"
    Vector worldUp( 0.0, 1.0, 0.0 );
    real_64 dotProd = forward.DotProductReal( worldUp );

    // calculate the projection of "forward" on "world up"
    Vector projection( forward, dotProd );

    // calculate "up" and normalize it
    Vector up( worldUp );
    up.Substract( projection );

    // if "forward" == "world up", "up" becomes {0}. therefore we cant
    // proceed. 
    // ##TODO## figure out a workaround (use another axis?)
    if ( up.LengthReal() < 0.0000001 ) 
    {
        return;
    }
    
    up.Normalize();

    // calculate "right" (already unit vector as "up" and "forward" are too)
    Vector right;
    right.CrossProductAndSet( up, forward );
    
    // construct the orientation part of the matrix from the three vectors
    SetIdentity();
    FromVectors( right, up, forward );

    // set the translation part of the matrix from the origin vector
    m_data[0][3] = origin.GetFixedX();
    m_data[1][3] = origin.GetFixedY();
    m_data[2][3] = origin.GetFixedZ();
}

NOVA_EXPORT void Matrix::CreateRotation( int angle, const Vector& axis )
{
    // reset to identity
    SetIdentity();

    // create a normalized copy of the vector
    Vector normVector( axis );
    normVector.Normalize();

    // check angle value
    while( angle < 0 ) angle += 360;
    while( angle >= 360 ) angle -= 360;
    int_32 sin = m_trigTable[angle];
    int_32 cos = m_trigTable[angle + TrigTableCosOffset];
    int_32 negcos = FixedPointOne - cos;

    int_32 x = normVector.GetFixedX();
    int_32 y = normVector.GetFixedY();
    int_32 z = normVector.GetFixedZ();

    int_32 xx = ::FixedLargeMul( x, x );
    int_32 yy = ::FixedLargeMul( y, y );
    int_32 zz = ::FixedLargeMul( z, z );
    int_32 z_negcos = ::FixedLargeMul( z, negcos );
    int_32 y_negcos = ::FixedLargeMul( y, negcos );

    m_data[0][0] = xx + ::FixedLargeMul( (FixedPointOne - xx), cos );
    m_data[0][1] = ::FixedLargeMul( x, y_negcos ) - ::FixedLargeMul( z, sin );
    m_data[0][2] = ::FixedLargeMul( x, z_negcos ) + ::FixedLargeMul( y, sin );
    m_data[1][0] = ::FixedLargeMul( x, y_negcos ) + ::FixedLargeMul( z, sin );
    m_data[1][1] = yy + ::FixedLargeMul( (FixedPointOne - yy), cos);
    m_data[1][2] = ::FixedLargeMul( y, z_negcos ) - ::FixedLargeMul( x, sin );
    m_data[2][0] = ::FixedLargeMul( x, z_negcos ) - ::FixedLargeMul( y, sin );
    m_data[2][1] = ::FixedLargeMul( y, z_negcos ) + ::FixedLargeMul( x, sin ); 
    m_data[2][2] = zz + ::FixedLargeMul( (FixedPointOne - zz), cos );

    m_multiplications = 0;    
}

NOVA_EXPORT void Matrix::CreateTranslation( const Vector& translation )
{
    // reset to identity
    SetIdentity();

    // set the translation component
    m_data[0][3] = translation.GetFixedX();
    m_data[1][3] = translation.GetFixedY();
    m_data[2][3] = translation.GetFixedZ();
}

NOVA_EXPORT void Matrix::SetIdentity()
{
    for( int i = 0; i < MatrixDim; i++ ) 
    {
        for( int j = 0; j < MatrixDim; j++ ) 
	{
            m_data[j][i] = (i == j) ? FixedPointOne : 0;
	}
    }
    
    m_multiplications = 0;
}

NOVA_EXPORT void Matrix::Set( const Matrix& matrix )
{
    // brute force memcpy() the other matrix'es data
    memcpy( this->m_data, matrix.m_data, sizeof( matrix.m_data ) );
    m_multiplications = 0;
}

NOVA_EXPORT void Matrix::GetTranslation( Vector& vector ) const
{
    vector.SetFixed( m_data[0][3], m_data[1][3], m_data[2][3] );
}

NOVA_EXPORT void Matrix::MultiplyAndSet( const Matrix& m1, const Matrix& m2 )
{
    int_32 res[MatrixDim][MatrixDim];

    // create a temporary result; this matrix might be one of the arguments
    // and therefore would mess up the calculations
    for ( int i = 0; i < MatrixDim; i++ ) 
    {
        for( int j = 0; j < MatrixDim; j++ ) 
	{
            res[j][i] = ::FixedLargeMul( m1.m_data[0][i], m2.m_data[j][0] ) + 
		::FixedLargeMul( m1.m_data[1][i], m2.m_data[j][1] ) + 
		::FixedLargeMul( m1.m_data[2][i], m2.m_data[j][2] ) +
		::FixedLargeMul( m1.m_data[3][i], m2.m_data[j][3] );
	}
    }

    // when all done, copy over this matrix'es data
    memcpy( this->m_data, res, sizeof( m_data ) );

    if ( (&m1 == this) || (&m2 == this) ) 
    {
        // this is one of the arguments; increase amount of multiplications
        m_multiplications++;
    } 
    else 
    {
        // inherit the amount of multiplications from the matrix which 
        // has the greater amount
        m_multiplications = MAX( m1.m_multiplications, m2.m_multiplications );
        m_multiplications++;
    }
    
    if ( m_multiplications >= NormalizeFreq ) 
    {
        Normalize();
    }
}

NOVA_EXPORT void Matrix::Normalize()
{
    Vector v1, v2, v3;

    AsVectors( v1, v2, v3 );
    v1.Normalize();
    v2.Normalize();
    v3.Normalize();
    FromVectors( v1, v2, v3 );

    m_multiplications = 0;
}

NOVA_EXPORT void Matrix::Transpose()
{
    int_32 tmp[MatrixDim][MatrixDim];

    for( int i = 0; i < MatrixDim; i++ ) 
    {
        for( int j = 0; j < MatrixDim; j++ ) 
	{
            tmp[i][j] = m_data[j][i];
	}
    }

    memcpy( this->m_data, tmp, sizeof(m_data) );
}

NOVA_EXPORT void Matrix::ClearTranslation()
{
    m_data[0][3] = 0;
    m_data[1][3] = 0;
    m_data[2][3] = 0;
}

NOVA_EXPORT void Matrix::InvertTransformation()
{
    int_32 tmp[MatrixDim][MatrixDim];

    // transpose the rotation submatrix
    for( int i = 0; i < RotSubMatrixDim; i++ ) 
    {
        for( int j = 0; j < RotSubMatrixDim; j++ ) 
	{
            tmp[i][j] = m_data[j][i];
	}
    }

    // negate the translation components
    tmp[0][3] = -m_data[0][3];
    tmp[1][3] = -m_data[1][3];
    tmp[2][3] = -m_data[2][3];

    // copy the remaining values
    tmp[3][0] = m_data[3][0];
    tmp[3][1] = m_data[3][1];
    tmp[3][2] = m_data[3][2];

    memcpy( this->m_data, tmp, sizeof(m_data) );
}

NOVA_EXPORT void Matrix::AsVectors( Vector& v1, 
                                    Vector& v2, 
                                    Vector& v3 ) const
{
    v1.SetFixed( m_data[0][0], m_data[0][1], m_data[0][2] );
    v2.SetFixed( m_data[1][0], m_data[1][1], m_data[1][2] );
    v3.SetFixed( m_data[2][0], m_data[2][1], m_data[2][2] );
}

NOVA_EXPORT void Matrix::FromVectors( const Vector& v1, 
                                      const Vector& v2, 
                                      const Vector& v3 )
{
    m_data[0][0] = v1.GetFixedX();
    m_data[0][1] = v1.GetFixedY();
    m_data[0][2] = v1.GetFixedZ();
    m_data[1][0] = v2.GetFixedX();
    m_data[1][1] = v2.GetFixedY();
    m_data[1][2] = v2.GetFixedZ();
    m_data[2][0] = v3.GetFixedX();
    m_data[2][1] = v3.GetFixedY();
    m_data[2][2] = v3.GetFixedZ();
}

//////////////////////////////////////////////
// implementation of PlaneEquation
//////////////////////////////////////////////

NOVA_EXPORT PlaneEquation::PlaneEquation()
    : m_normal( 0, 0, 0 ),
      m_fixedD( 0 )
{
}

NOVA_EXPORT PlaneEquation::PlaneEquation( const Vector& v1, const Vector& v2, 
                                          const Vector& v3 )
{
    Calculate( v1, v2, v3 );
}

NOVA_EXPORT PlaneEquation::~PlaneEquation()
{
}

NOVA_EXPORT void PlaneEquation::Calculate( const Vector& v1, const Vector& v2, 
                                           const Vector& v3 )
{
    // calculate the plane normal by cross product of the two vectors
    // (v3-v1) x (v2-v1), named u and v here
    Vector u( v3, v1 );
    Vector v( v2, v1 );
    u.CheckPrecision();
    v.CheckPrecision();
    
    m_normal.CrossProductAndSet( v, u );
    m_normal.Normalize();

    // calculate the 'D' component from plane equation Ax + By + Cz + D = 0 
    // as D = -(Ax + By + Cz), assigning v1 into the equation
    m_fixedD = -m_normal.DotProductFixed( v1 );
}

}; // namespace
