/*
 *  $Id: Camera.cpp,v 1.42 2005/03/20 18:26:48 matti Exp $
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

// INCLUDE FILES
#include <avkon.hrh>
#include <aknmessagequerydialog.h>
#include <aknnotewrappers.h>
#include <stringloader.h>
#include <f32file.h>
#include <s32file.h>
#include <hlplch.h>

#include <SimpleCube_0xE7BA8E00.rsg>

#include "SimpleCube.hrh"
#include "SimpleCube.pan"
#include "SimpleCubeApplication.h"
#include "SimpleCubeAppUi.h"
#include "SimpleCubeAppView.h"

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CSimpleCubeAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSimpleCubeAppUi::ConstructL()
	{
	// Initialise app UI with standard value.
	BaseConstructL(CAknAppUi::EAknEnableSkin);

	// Create view object
	iAppView = CSimpleCubeAppView::NewL(ClientRect());

	//##TODO##
	}
// -----------------------------------------------------------------------------
// CSimpleCubeAppUi::CSimpleCubeAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CSimpleCubeAppUi::CSimpleCubeAppUi()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CSimpleCubeAppUi::~CSimpleCubeAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CSimpleCubeAppUi::~CSimpleCubeAppUi()
	{
	if (iAppView)
		{
		delete iAppView;
		iAppView = NULL;
		}

	}

// -----------------------------------------------------------------------------
// CSimpleCubeAppUi::HandleCommandL()
// Takes care of command handling.
// -----------------------------------------------------------------------------
//
void CSimpleCubeAppUi::HandleCommandL(TInt aCommand)
	{
	switch (aCommand)
		{
		case EEikCmdExit:
		case EAknSoftkeyExit:
			Exit();
			break;

		}
	}
// -----------------------------------------------------------------------------
//  Called by the framework when the application status pane
//  size is changed.  Passes the new client rectangle to the
//  AppView
// -----------------------------------------------------------------------------
//
void CSimpleCubeAppUi::HandleStatusPaneSizeChange()
	{
	iAppView->SetRect(ClientRect());
	}

