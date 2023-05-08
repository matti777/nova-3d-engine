/*
 *  $Id: VectorMath.h 23 2009-09-10 13:57:16Z matti $
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

#ifndef __VECTOR_MATH_H
#define __VECTOR_MATH_H

// FILE INFO
// This file contains vector math definitions for entities such as
// a vector and a matrix 

#include <stdio.h>

#include "NovaTypes.h"
#include "FixedPoint.h"

namespace nova3d {

// forward declarations
//class Vector;
class Matrix;

/**
 * Represents a 4x1 vector. Vector components are x,y,z,w (w not used in 
 * current implementation). All the internal math are done with fixed point 
 * numbers.<P>
 *
 * @author Matti Dahlbom
 * @version $Name:  $, $Revision: 23 $
 */
class Vector
{
 public: // Constructors and destructor
    NOVA_IMPORT Vector( real_64 x, real_64 y, real_64 z );
    NOVA_IMPORT Vector();
    NOVA_IMPORT Vector( const Vector& vector );
    NOVA_IMPORT Vector( const Vector& vector, real_64 scaler );

    /** Constructs a new Vector as the difference (v1 - v2) of two vectors */ 
    NOVA_IMPORT Vector( const Vector& v1, const Vector& v2 );
                
 public: // New methods (Public API)
    NOVA_IMPORT void SetReal( real_64 x, real_64 y, real_64 z );
    NOVA_IMPORT void SetFixed( int_32 x, int_32 y, int_32 z );
    NOVA_IMPORT void Add( const Vector& vector );
    NOVA_IMPORT void Substract( const Vector& vector );
    inline real_64 DotProductReal( const Vector& vector ) const;
    inline int_32 DotProductFixed( const Vector& vector ) const;
        
    /**
     * Calculates a cross product between Vectors v1 and v2. The result is
     * stored in this Vector.
     */
    NOVA_IMPORT void CrossProductAndSet( const Vector& v1, 
					 const Vector& v2 );

    /** Sets equal to another vector */
    NOVA_IMPORT void Set( const Vector& vector );

    /** Sets this vector to the difference v1 - v2 */
    NOVA_IMPORT void SubstractAndSet( const Vector& v1, const Vector& v2 );
        
    NOVA_IMPORT void Inverse();

    /**
     * Improves the precision of the vector components if very small. This
     * may be used to eliminate rounding errors in multiplication/etc. 
     * operations.
     */
    NOVA_IMPORT void CheckPrecision();
        
    /** Normalizes the vector (to unit length), using precalculated length. */
    NOVA_IMPORT void Normalize();

    /** Compares for equality with another vector */
    NOVA_IMPORT bool operator==( const Vector& vector ) const;        

    /**
     * Transforms the argument vector by the argument matrix. The result is
     * stored in this Vector.
     */
    NOVA_IMPORT void TransformAndSet( const Matrix& matrix, 
				      const Vector& vector );

    /**
     * Rotates the argument vector by the argument matrix. The translational 
     * component is not used. The result is
     * stored in this Vector.
     */
    NOVA_IMPORT void RotateAndSet( const Matrix& matrix, 
				   const Vector& vector );

    /** 
     * Returns the angle between this vector and another vector in radians
     * as a real.
     */
    NOVA_IMPORT real_64 AngleBetweenRadReal( const Vector& vector ) const;

    /**
     * Returns the length of the vector as a real.
     */
    NOVA_IMPORT real_64 LengthReal() const;

    /**
     * Returns the length of the vector as fixed point.
     */
    NOVA_IMPORT int_32 LengthFixed() const;
        
    inline int_32 GetFixedX() const;
    inline int_32 GetFixedY() const;
    inline int_32 GetFixedZ() const;
    inline real_64 GetRealX() const;
    inline real_64 GetRealY() const;
    inline real_64 GetRealZ() const;
    inline void GetFixed( int_32& x, int_32& y, int_32& z ) const;
    inline void PrintToStdout() const;
    inline bool IsNull() const;

 private: // Data
    int_32 m_x;
    int_32 m_y;
    int_32 m_z;
};

/**
 * Represents a bounding sphere.<p />
 */
struct BoundingSphere
{
    /** Location of the sphere's central point as a vector */
    Vector m_location;
    
    /** Radius of the sphere as fixed point */
    int_32 m_radius;
};

// dimension of the matrix
const int MatrixDim = 4;

// dimension of the rotation submatrix
const int RotSubMatrixDim = 3;

// normalize matrix every N multiplications
const int NormalizeFreq = 10;

// offset of cosine values in the trigonometric table
const int TrigTableCosOffset = 90;

// size of the trigonometrix table
const int TrigTableSize = 360 + TrigTableCosOffset;

/**
 * Represents a 4x4 matrix. All internal math is calculated with 
 * fixed point numbers.<P>
 *
 * The matrix data (m_data) for a transformation matrix is organized as follows:
 * <pre>
 * | r11 r12 r13 tx |
 * | r21 r22 r23 ty |
 * | r31 r32 r33 tz |
 * |  0   0   0  1  |
 *
 * and is represented by:
 *
 * m_data[0][0] = r11
 * m_data[0][2] = r31
 * m_data[2][0] = r31
 *   ... etc
 * </pre> 
 *
 * r-entities represent a rotation matrix. t-entities represent 
 * a translation vector.<p />
 *
 * @author Matti Dahlbom
 * @version $Name:  $, $Revision: 23 $
 */
class Matrix 
{
 public: // Constructors and destructor
    NOVA_IMPORT Matrix();
        
    /** 
     * Copy constructor. Recommended way of construction of a Matrix, 
     * since it copies the lookup tables from the argument matrix and
     * doesnt generate them by itself.
     * */
    NOVA_IMPORT Matrix( const Matrix& matrix );
        
 public: // New methods (Public API)
    /**
     * Sets up an transformation matrix so that it "looks" at a 
     * given point from a given point in world space, defined by
     * vectors.
     */
    NOVA_IMPORT void CreateLookAt( const Vector& origin, 
				   const Vector& target );
        
    /** Creates a rotation matrix around a given axis */
    NOVA_IMPORT void CreateRotation( int angle, const Vector& axis );
        
    /** Creates a translation matrix from a given Vector */
    NOVA_IMPORT void CreateTranslation( const Vector& translation );

    /** Sets this matrix to identity */
    NOVA_IMPORT void SetIdentity();
        
    /** Sets this matrix from another */
    NOVA_IMPORT void Set( const Matrix& matrix );

    /**
     * Extracts the translation part of the matrix and sets it into the
     * given vector.
     */
    NOVA_IMPORT void GetTranslation( Vector& vector ) const;
        
    /** 
     * Multiplies the two matrices m1 * m2. The result is stored in
     * this Matrix.
     */
    NOVA_IMPORT void MultiplyAndSet( const Matrix& m1, const Matrix& m2 );
    NOVA_IMPORT void Normalize();
    NOVA_IMPORT void Transpose();

    /** Sets the translational component of the matrix to {0,0,0}. */
    NOVA_IMPORT void ClearTranslation();

    /**
     * Inverts a transformation matrix; the upper-left 3x3 submatrix is 
     * transposed, and the translational part is negated.
     */
    NOVA_IMPORT void InvertTransformation();

    /** 
     * Extracts the upper-left 3x3 submatrix of this matrix as 
     * horizontal vectors.
     */
    NOVA_IMPORT void AsVectors( Vector& v1, 
				Vector& v2, 
				Vector& v3 ) const;

    /**
     * Initializes the upper-left 3x3 submatrix of this matrix 
     * from horizontal vectors. The rest of the matrix is left untouched.
     */
    NOVA_IMPORT void FromVectors( const Vector& v1, 
				  const Vector& v2, 
				  const Vector& v3 );

    inline void PrintToStdout() const;

 private: // Data
    // actual matrix data
    int_32 m_data[MatrixDim][MatrixDim];

    // number of multiplications after last normalization
    int_32 m_multiplications;

    // sine/cosine table
    int_32 m_trigTable[TrigTableSize];
        
    // friend declarations
    friend class Vector;
};

/**
 * Represents a plane equation.<p />
 *
 * @author Matti Dahlbom
 * @version $Name:  $, $Revision: 23 $
 */
class PlaneEquation 
{
 public: // Constructors and destructor
    /** 
     * Constructs an uninitialized plane equation object.
     */
    NOVA_IMPORT PlaneEquation();

    /**
     * Constructs a plane equation from given 3 points (vectors) forming 
     * the plane.
     */
    NOVA_IMPORT PlaneEquation( const Vector& v1, const Vector& v2, 
			       const Vector& v3 );

    NOVA_IMPORT ~PlaneEquation();

 public: // New methods (Public API)
    /**
     * (Re)calculates the plane equation from given 3 points (vectors) forming 
     * the plane.
     */
    NOVA_IMPORT void Calculate( const Vector& v1, const Vector& v2, 
				const Vector& v3 );

    /**
     * Checks whether the given point (defined by a vector) is "outside" the 
     * plane. The "outside" property is defined as whether the 
     * point is on that side of the plane where the normal vector
     * points to. This is given by a dot product between the point
     * and the normal vector and by comparing the result to the 
     * D component of the plane equation.
     */
    inline bool IsOutside( const Vector& point ) const;

    /**
     * Returns the distance (in fixed point) for the given point from the
     * plane.<p />
     */
    inline int_32 DistanceFromPlaneFixed( const Vector& point ) const;

    inline const Vector& GetNormal() const;
    inline int_32 GetFixedD() const;

 private: // Data
    Vector m_normal;
    int_32 m_fixedD;
};

// inline method definitions

// inline method definitions for Vector
real_64 Vector::DotProductReal( const Vector& vector ) const
{
    return (real_64)(DotProductFixed( vector ) / FixedPointFactor);
}

int_32 Vector::DotProductFixed( const Vector& vector ) const
{
    return ::FixedTripleMul( m_x, vector.m_x,
                             m_y, vector.m_y, 
                             m_z, vector.m_z );
}

int_32 Vector::GetFixedX() const
{
    return m_x;
}

int_32 Vector::GetFixedY() const
{
    return m_y;
}

int_32 Vector::GetFixedZ() const
{
    return m_z;
}

real_64 Vector::GetRealX() const
{
    return ::FixedToReal( m_x );
}

real_64 Vector::GetRealY() const
{
    return ::FixedToReal( m_y );
}

real_64 Vector::GetRealZ() const
{
    return ::FixedToReal( m_z );
}

void Vector::GetFixed( int_32& x, int_32& y, int_32& z ) const
{
    x = m_x;
    y = m_y;
    z = m_z;
}

bool Vector::IsNull() const
{
    return ((m_x == 0) && (m_y == 0) && (m_z == 0));
}

void Vector::PrintToStdout() const
{
    printf("[%f,%f,%f]\n", 
	   ::FixedToReal(m_x), ::FixedToReal(m_y), ::FixedToReal(m_z) );
}

// inline method definitions for Matrix
void Matrix::PrintToStdout() const
{
    for ( int i = 0; i < MatrixDim; i++ )
    {
	printf("[%f,%f,%f,%f]\n",
 	       ::FixedToReal(m_data[i][0]), ::FixedToReal(m_data[i][1]),  
 	       ::FixedToReal(m_data[i][2]), ::FixedToReal(m_data[i][3])); 
    }
}

// inline method definitions for PlaneEquation
inline const Vector& PlaneEquation::GetNormal() const
{
    return m_normal;
}

inline int_32 PlaneEquation::GetFixedD() const
{
    return m_fixedD;
}

bool PlaneEquation::IsOutside( const Vector& point ) const
{
    int_32 d = -(m_normal.DotProductFixed( point ));
    return ( d < m_fixedD );
}

}; // namespace

#endif
