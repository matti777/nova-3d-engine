/*
 *  $Id: Node.h 17 2009-09-02 09:03:47Z matti $
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
#ifndef __NODE_H_
#define __NODE_H_

// This file contains definitions for all the different kinds of scene graph 
// nodes.

#include "NovaTypes.h"
#include "List.h"
#include "VectorMath.h"

namespace nova3d {

// forward declarations
class Shape;
class GroupNode;
class Light;
class Camera;
class ShapeNode;
class LightNode;
class CameraNode;

// maximum node name length
const uint_32 MaxNodeNameLength = 63;

/**
 * Represents a node in a scene graph. This is an abstract class and is 
 * intended to be instantiated.<p />
 * 
 * @author Matti Dahlbom
 * @version $Revision: 17 $
 */
class Node
{
 public:
    /** node types */
    enum NodeType 
    {
	TypeGroup = 1,
	TypeRoot,
	TypeTransformation,
	TypeShape,
	TypeCamera,
	TypeLight
    };

 public: // Constructors and destructor
    virtual ~Node();

 public: // New methods (Public API)
    NOVA_IMPORT bool IsGroupNode();
    NOVA_IMPORT int DetachFromParent();
    NOVA_IMPORT int SetName( const char* name );
    int SetParent( GroupNode* parent );
    inline const GroupNode* GetParent() const;
    inline NodeType GetType() const;
    inline const char* GetName() const;
        
    /** 
     * Transform the given matrix by the scene graph, applying all the
     * transformations above this node.<p />
     */
    void TransformMatrixBySceneGraph( Matrix* matrix );
        
 protected: // New methods
        
 private: // New methods
    inline void SetLive( bool isLive );
        
 protected: // Constructors
    Node( NodeType nodeType );
        
 protected: // Data
    // this node's name
    char m_name[MaxNodeNameLength + 1];
        
    // whether this node is part of a live scene graph
    bool m_isLive;
        
 private: // Data
    // this node's type
    NodeType m_type;
        
    // this node's parent
    GroupNode* m_parent;
        
    // friend declarations
    friend class RootNode;
};

/**
 * Represents a node that may have children.<p />
 * 
 * @author Matti Dahlbom
 * @version $Revision: 17 $
 */
class GroupNode : public Node
{
 public: // Constructors and destructor
    NOVA_IMPORT GroupNode();
        
    /**
     * Will destroy all child nodes.<p />
     */
    NOVA_IMPORT virtual ~GroupNode();
        
 public: // New methods (Public API)
    NOVA_IMPORT int AddChild( Node* node );
    NOVA_IMPORT int RemoveChild( Node* node );
    inline const List<Node*>& Children() const;
    inline int NumChildren() const;

 protected: // Constructors
    GroupNode( NodeType nodeType );
        
 private: // Data
    List<Node*> m_children;
};

/**
 * Represents a root node in a scene graph.<p />
 * 
 * @author Matti Dahlbom
 * @version $Revision: 17 $
 */
class RootNode : public GroupNode
{
 public:  // Constructors and destructor
    NOVA_IMPORT RootNode();
    NOVA_IMPORT virtual ~RootNode();
        
 public: // New methods
    void SetSceneGraphLive( bool isLive );
    void SetNodeLive( Node* node, bool isLive );
        
 private: // Data
    List<ShapeNode*> m_shapeNodeList;
    List<LightNode*> m_lightNodeList;
    List<CameraNode*> m_cameraNodeList;
};

/**
 * Transformation node. This node owns a transformation matrix.<P>
 *
 * @author Matti Dahlbom
 * @version $Name:  $, $Revision: 17 $
 */
class TransformationNode : public GroupNode
{
 public: // Constructors and destructor
    NOVA_IMPORT TransformationNode();
    NOVA_IMPORT ~TransformationNode();
        
 public: // New methods (Public API)
    inline const Matrix& GetMatrix() const;
    NOVA_IMPORT void SetRotation( int_32 angle, 
				  const Vector& axis );
    NOVA_IMPORT void SetTranslation( const Vector& translation );
    NOVA_IMPORT void SetLookAt( const Vector& origin, 
				const Vector& target );

 private: // Data
    Matrix m_matrix;
};

/**
 * Node that holds visual object.<p />
 *
 * @author Matti Dahlbom
 * @version $Name:  $, $Revision: 17 $
 */
class ShapeNode : public Node
{
 public: // Constructors and destructor
    NOVA_IMPORT ShapeNode( Shape& shape );
    NOVA_IMPORT ~ShapeNode();
        
 public: // New methods 

    /**
     * Returns the shape (visual object) for this node.
     */
    inline Shape& GetShape();

    /**
     * Transforms the object matrix of this node by the transforms in
     * the scene graph branch above this node.
     */
    void TransformBySceneGraph();

    /** 
     * Transforms the object matrix by the inverse camera transform.
     */
    void TransformByCamera( const Matrix& inverseCameraMatrix );

    /** 
     * Returns the object matrix.
     */
    inline const Matrix& GetObjectMatrix() const;

    /**
     * Returns the bounding sphere for the shape.
     */
    void GetBoundingSphere( BoundingSphere& boundingSpehere );
        
 private: // Data
    // shape this node relates to
    Shape& m_shape;

    // the object matrix
    Matrix m_objectMatrix;
};

/**
 * Node that holds a camera.<p />
 *
 * @author Matti Dahlbom
 * @version $Name:  $, $Revision: 17 $
 */
class CameraNode : public Node
{
 public: // Constructors and destructor
    NOVA_IMPORT CameraNode( Camera& camera );
    NOVA_IMPORT ~CameraNode();
        
 public: // New methods
    /** Returns the associated Camera */
    inline Camera& GetCamera() const;
        
    /** Returns the Camera Matrix of the associated Camera */
    inline const Matrix& GetCameraMatrix() const;
        
    /**
     * Transforms the camera of this node by the transforms in the 
     * scene graph branch above this node.
     */
    void TransformBySceneGraph();
        
 private: // Data
    // the Camera associated with this node
    Camera& m_camera;
        
    // the Camera Matrix
    Matrix m_cameraMatrix;
};

/**
 * Node that holds a light source.<p />
 *
 * @author Matti Dahlbom
 * @version $Name:  $, $Revision: 17 $
 */
class LightNode : public Node
{
 public: // Constructors and destructor
    NOVA_IMPORT LightNode( Light& light );
    NOVA_IMPORT ~LightNode();
        
 public: // New methods
    /** Returns the Light associated with this node */
    inline Light& GetLight() const;
        
    /** Transforms this node by the scene graph */
    void TransformBySceneGraph();
        
 private: // Data
    // the Light associated with this node
    Light& m_light;
        
    // the Light Matrix 
    Matrix m_lightMatrix;
};

/////////////////////////////////////////
// inline method definitions
/////////////////////////////////////////

// Node inline method definitions
const GroupNode* Node::GetParent() const
{
    return m_parent;
}

Node::NodeType Node::GetType() const
{
    return m_type; 
}

const char* Node::GetName() const
{
    return m_name;
}

void Node::SetLive( bool isLive )
{
    m_isLive = isLive;
}

// GroupNode inline method definitions
int GroupNode::NumChildren() const
{
    return m_children.Count();
}

const List<Node*>& GroupNode::Children() const
{
    return m_children;
}

// ShapeNode inline method definitions
const Matrix& ShapeNode::GetObjectMatrix() const
{
    return m_objectMatrix;
}

Shape& ShapeNode::GetShape()
{
    return m_shape;
}

// TransformationNode inline method definitions
const Matrix& TransformationNode::GetMatrix() const
{
    return m_matrix;
}

// CameraNode inline method definitions
Camera& CameraNode::GetCamera() const
{
    return m_camera;
}

const Matrix& CameraNode::GetCameraMatrix() const
{
    return m_cameraMatrix;
}

// LightNode inline method definitions
Light& LightNode::GetLight() const
{
    return m_light;
}

}; // namespace

#endif 

