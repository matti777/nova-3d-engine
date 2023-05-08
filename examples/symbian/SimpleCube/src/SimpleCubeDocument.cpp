/*
 ============================================================================
 Name		: SimpleCubeDocument.cpp
 Author	  : 
 Copyright   : 
 Description : CSimpleCubeDocument implementation
 ============================================================================
 */

// INCLUDE FILES
#include "SimpleCubeAppUi.h"
#include "SimpleCubeDocument.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSimpleCubeDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSimpleCubeDocument* CSimpleCubeDocument::NewL(CEikApplication& aApp)
	{
	CSimpleCubeDocument* self = NewLC(aApp);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSimpleCubeDocument::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSimpleCubeDocument* CSimpleCubeDocument::NewLC(CEikApplication& aApp)
	{
	CSimpleCubeDocument* self = new (ELeave) CSimpleCubeDocument(aApp);

	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// -----------------------------------------------------------------------------
// CSimpleCubeDocument::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSimpleCubeDocument::ConstructL()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CSimpleCubeDocument::CSimpleCubeDocument()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CSimpleCubeDocument::CSimpleCubeDocument(CEikApplication& aApp) :
	CAknDocument(aApp)
	{
	// No implementation required
	}

// ---------------------------------------------------------------------------
// CSimpleCubeDocument::~CSimpleCubeDocument()
// Destructor.
// ---------------------------------------------------------------------------
//
CSimpleCubeDocument::~CSimpleCubeDocument()
	{
	// No implementation required
	}

// ---------------------------------------------------------------------------
// CSimpleCubeDocument::CreateAppUiL()
// Constructs CreateAppUi.
// ---------------------------------------------------------------------------
//
CEikAppUi* CSimpleCubeDocument::CreateAppUiL()
	{
	// Create the application user interface, and return a pointer to it;
	// the framework takes ownership of this object
	return new (ELeave) CSimpleCubeAppUi;
	}

// End of File
