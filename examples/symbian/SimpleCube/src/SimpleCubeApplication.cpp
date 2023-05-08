/*
 ============================================================================
 Name		: SimpleCubeApplication.cpp
 Author	  : 
 Copyright   : 
 Description : Main application class
 ============================================================================
 */

// INCLUDE FILES
#include "SimpleCube.hrh"
#include "SimpleCubeDocument.h"
#include "SimpleCubeApplication.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSimpleCubeApplication::CreateDocumentL()
// Creates CApaDocument object
// -----------------------------------------------------------------------------
//
CApaDocument* CSimpleCubeApplication::CreateDocumentL()
	{
	// Create an SimpleCube document, and return a pointer to it
	return CSimpleCubeDocument::NewL(*this);
	}

// -----------------------------------------------------------------------------
// CSimpleCubeApplication::AppDllUid()
// Returns application UID
// -----------------------------------------------------------------------------
//
TUid CSimpleCubeApplication::AppDllUid() const
	{
	// Return the UID for the SimpleCube application
	return KUidSimpleCubeApp;
	}

// End of File
