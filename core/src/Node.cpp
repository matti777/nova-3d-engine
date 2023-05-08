/*
 *  $Id: Node.cpp 18 2009-09-04 10:49:50Z matti $
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

#include "Node.h"
#include "Lights.h"
#include "Camera.h"
#include "Shape.h"
#include "novalogging.h"
#include "NovaErrors.h"

namespace nova3d {

//////////////////////////////////////////////
// implementation of Node
//////////////////////////////////////////////

Node::Node( Node::NodeType nodeType )
    : m_isLive( false ),
      m_type( nodeType ),
      m_parent( NULL )
{
    m_name[0] = '\0';
}

Node::~Node()
{
}

NOVA_EXPORT bool Node::IsGroupNode()
{
    return ((m_type == TypeGroup) || 
            (m_type == TypeRoot) ||
            (m_type == TypeTransformation));
}

NOVA_EXPORT int Node::DetachFromParent()
{
    if ( m_isLive ) 
    {
        return NovaErrLiveSceneGraph;
    }
    else if ( m_parent == NULL )
    {
        return NovaErrNotSet;
    }
    else 
    {
        GroupNode* parent = m_parent;
        m_parent = NULL;
        
        parent->RemoveChild( this );
        return NovaErrNone;
    }
}

int Node::SetParent( GroupNode* parent )
{
    if ( m_isLive ) 
    {
        return NovaErrLiveSceneGraph;
    }
    else if ( m_parent == parent )
    {
        // give node already set as the parent
        return NovaErrNone;
    }
    else if ( m_parent != NULL )
    {
        return NovaErrAlreadySet;
    }
    else
    {
        m_parent = parent;
        return NovaErrNone;
    }
}

NOVA_EXPORT int Node::SetName( const char* name )
{
    if ( strlen( name ) > MaxNodeNameLength )
    {
        return NovaErrOverflow;
    }
    else
    {
        strcpy( m_name, name );
        return NovaErrNone;
    }
}

void Node::TransformMatrixBySceneGraph( Matrix* matrix )
{
    //LOG_DEBUG_F("TransformMatrixBySceneGraph() node = %s", GetName());

    // initialize the matrix to identity
    matrix->SetIdentity();

    // transforms the matrix by all transform nodes in the scene
    // graph branch above this node
    const GroupNode* node = GetParent();
    while ( node != NULL ) 
    {
        if ( node->GetType() == TypeTransformation ) 
	{
            const TransformationNode* tNode = 
                static_cast<const TransformationNode*>(node);
	    //    LOG_DEBUG_F("-transforming with node = %s",node->GetName());
            matrix->MultiplyAndSet( *matrix, tNode->GetMatrix() );
	}

        node = node->GetParent();
    }
}

//////////////////////////////////////////////
// implementation of GroupNode
//////////////////////////////////////////////

GroupNode::GroupNode( NodeType nodeType )
    : Node( nodeType )
{
}

NOVA_EXPORT GroupNode::GroupNode()
    : Node( TypeGroup )
{
}

NOVA_EXPORT GroupNode::~GroupNode()
{
    // delete all the children
    for ( int i = 0; i < m_children.Count(); i++ )
    {
        Node* node = NULL;
        if ( m_children.Get( i, node ) == NovaErrNone )
	{
	    LOG_DEBUG_F("deleting node %s", node->GetName());
            delete node;           
	}
    }

}

NOVA_EXPORT int GroupNode::AddChild( Node* node )
{
    LOG_DEBUG_F("GroupNode::AddChild() %s -> %s", node->GetName(), GetName());

    if ( m_isLive ) 
    {
	LOG_DEBUG("GroupNode::AddChild() - cannot add to live graph!");
        return NovaErrLiveSceneGraph;
    }
    else if ( node->GetType() == TypeRoot )
    {
        // cant add a root node as child
	LOG_DEBUG("GroupNode::AddChild() - cannot root node as child!");
        return NovaErrInvalidArgument;
    }
    else 
    {
        int ret = node->SetParent( this );
        if ( ret != NovaErrNone )
	{
	    LOG_DEBUG_F("GroupNode::AddChild() - SetParent() failed=%d", ret);
            return ret;
	}
        else
	{
            ret = m_children.Append( node ); 
            if ( ret != NovaErrNone )
	    {
                node->DetachFromParent();
	    }
            
            return ret;
	}
    }
}

NOVA_EXPORT int GroupNode::RemoveChild( Node* node )
{
    if ( m_isLive ) 
    {
        return NovaErrLiveSceneGraph;
    }
    else
    {
        for ( int i = 0; i < m_children.Count(); i++ )
	{
            Node* child = NULL;
            if ( m_children.Get( i, child ) == NovaErrNone )
	    {
                if ( child == node ) 
		{
                    m_children.Remove( i );
                    child->DetachFromParent();
                    
                    return NovaErrNone;
		}
	    }
	}
    }
    
    return NovaErrNotFound;
}

//////////////////////////////////////////////
// implementation of RootNode
//////////////////////////////////////////////

NOVA_EXPORT RootNode::RootNode()
    : GroupNode( TypeRoot )
{
}

NOVA_EXPORT RootNode::~RootNode()
{
    LOG_DEBUG("RootNode::~GroupNode() done.");
}

void RootNode::SetSceneGraphLive( bool isLive )
{
    LOG_DEBUG_F("RootNode::SetSceneGraphLive() = %d", isLive);

    // reset all node lists
    m_shapeNodeList.Reset();
    m_lightNodeList.Reset();
    m_cameraNodeList.Reset();

    // recursively set all nodes live, forming the node lists as we go
    SetNodeLive( this, isLive );

    LOG_DEBUG_F("RootNode::SetSceneGraphLive() #shapes = %d, #lights = %d, " \
		"#cameras = %d", 
		m_shapeNodeList.Count(), m_lightNodeList.Count(), 
		m_cameraNodeList.Count());

    // set the shape/light node lists to all cameras
    CameraNode* cameraNode;
    for ( int i = 0; i < m_cameraNodeList.Count(); i++ )
    {
	if ( m_cameraNodeList.Get( i, cameraNode ) == NovaErrNone )
	{
	    if ( isLive )
	    {
		cameraNode->GetCamera().SetShapeNodeList( &m_shapeNodeList );
		cameraNode->GetCamera().SetLightNodeList( &m_lightNodeList );
	    }
	    else
	    {
		cameraNode->GetCamera().SceneGraphDetached();
	    }
	}
    }

    LOG_DEBUG("RootNode::SetSceneGraphLive() done.");
}

void RootNode::SetNodeLive( Node* node, bool isLive )
{
    node->SetLive( isLive );

    if ( node->IsGroupNode() )
    {
        // call recursively for all children
        GroupNode* groupNode = static_cast<GroupNode*>(node);
        const List<Node*>& children = groupNode->Children();
        for ( int i = 0; i < children.Count(); i++ )
	{
            Node* child = NULL;
            if ( children.Get( i, child ) == NovaErrNone )
	    {
                SetNodeLive( child, isLive );

		// if the node is a shape node, increase the polygon count
		if ( child->GetType() == Node::TypeShape )
		{
		    m_shapeNodeList.Append( (ShapeNode*&)child );
		} 
		else if ( child->GetType() == Node::TypeLight )
		{
		    m_lightNodeList.Append( (LightNode*&)child );
		}
		else if ( child->GetType() == Node::TypeCamera )
		{
		    m_cameraNodeList.Append( (CameraNode*&)child );
		}
	    }
	}
    }
}

//////////////////////////////////////////////
// implementation of TransformationNode
//////////////////////////////////////////////

NOVA_EXPORT TransformationNode::TransformationNode()
    : GroupNode( TypeTransformation )
{
}

NOVA_EXPORT TransformationNode::~TransformationNode()
{
}

NOVA_EXPORT void TransformationNode::SetRotation( int_32 angle, 
						  const Vector& axis )
{
    m_matrix.CreateRotation( angle, axis );
}

NOVA_EXPORT void TransformationNode::SetTranslation( const Vector& translation )
{
    m_matrix.CreateTranslation( translation );
}

NOVA_EXPORT void TransformationNode::SetLookAt( const Vector& origin, 
						const Vector& target )
{
    m_matrix.CreateLookAt( origin, target );
}

//////////////////////////////////////////////
// implementation of ShapeNode
//////////////////////////////////////////////

NOVA_EXPORT ShapeNode::ShapeNode( Shape& shape )
    : Node( TypeShape ),
      m_shape( shape )
{
}

NOVA_EXPORT ShapeNode::~ShapeNode()
{
}

void ShapeNode::TransformBySceneGraph()
{
    TransformMatrixBySceneGraph( &m_objectMatrix );
}

void ShapeNode::TransformByCamera( const Matrix& inverseCameraMatrix )
{
    m_objectMatrix.MultiplyAndSet( m_objectMatrix, inverseCameraMatrix );
}

void ShapeNode::GetBoundingSphere( BoundingSphere& boundingSphere )
{
    m_objectMatrix.GetTranslation( boundingSphere.m_location );
    boundingSphere.m_radius = m_shape.GetBoundingSphereRadius();
}

//////////////////////////////////////////////
// implementation of CameraNode
//////////////////////////////////////////////

NOVA_EXPORT CameraNode::CameraNode( Camera& camera )
    : Node( TypeCamera ),
      m_camera( camera )
{
    m_camera.SetNode( this );
}

NOVA_EXPORT CameraNode::~CameraNode()
{
}

void CameraNode::TransformBySceneGraph()
{
    //LOG_DEBUG("CameraNode::TransformBySceneGraph()");

    TransformMatrixBySceneGraph( &m_cameraMatrix );

    // if "look at" target has been set, turn camera to look at it
    Vector lookAt;
    if ( m_camera.GetLookAt( lookAt ) ) 
    {
        Vector cameraPos;
        m_cameraMatrix.GetTranslation( cameraPos );
        m_cameraMatrix.CreateLookAt( cameraPos, lookAt );
    }

    //LOG_DEBUG("CameraNode::TransformBySceneGraph() done.");
}

//////////////////////////////////////////////
// implementation of LightNode
//////////////////////////////////////////////

NOVA_EXPORT LightNode::LightNode( Light& light )
    : Node( TypeLight ),
      m_light( light )
{
}

NOVA_EXPORT LightNode::~LightNode()
{
}

void LightNode::TransformBySceneGraph()
{
    if ( m_light.GetType() == Light::TypeAmbient ) 
    {
        // ambient light has no position/orientation
        return;
    }

    // only done for light sources with position
    if ( m_light.GetType() == Light::TypePoint )
    {
        PointLight& pointLight = static_cast<PointLight&>(m_light);
        
        TransformMatrixBySceneGraph( &m_lightMatrix );
    
        // get the transformed light position and update it to the light object
        Vector pos; 
        m_lightMatrix.GetTranslation( pos );
        pos.RotateAndSet( m_lightMatrix, pos );
        pointLight.GetPosition().Set( pos );
    }
}

}; // namespace
