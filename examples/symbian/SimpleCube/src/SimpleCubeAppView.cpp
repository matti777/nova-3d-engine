/*
 ============================================================================
 Name		: SimpleCubeAppView.cpp
 Author	  : 
 Copyright   : 
 Description : Application view implementation
 ============================================================================
 */

// INCLUDE FILES
#include <coemain.h>
#include "SimpleCubeAppView.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSimpleCubeAppView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSimpleCubeAppView* CSimpleCubeAppView::NewL(const TRect& aRect)
	{
	CSimpleCubeAppView* self = CSimpleCubeAppView::NewLC(aRect);
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// CSimpleCubeAppView::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSimpleCubeAppView* CSimpleCubeAppView::NewLC(const TRect& aRect)
	{
	CSimpleCubeAppView* self = new (ELeave) CSimpleCubeAppView;
	CleanupStack::PushL(self);
	self->ConstructL(aRect);
	return self;
	}

// -----------------------------------------------------------------------------
// CSimpleCubeAppView::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSimpleCubeAppView::ConstructL(const TRect& aRect)
	{
	// Create a window for this application view
	CreateWindowL();

	// Set the windows size
	SetRect(aRect);

	// Activate the window, which makes it ready to be drawn
	ActivateL();
	}

// -----------------------------------------------------------------------------
// CSimpleCubeAppView::CSimpleCubeAppView()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CSimpleCubeAppView::CSimpleCubeAppView()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CSimpleCubeAppView::~CSimpleCubeAppView()
// Destructor.
// -----------------------------------------------------------------------------
//
CSimpleCubeAppView::~CSimpleCubeAppView()
	{
	// No implementation required
	}

// -----------------------------------------------------------------------------
// CSimpleCubeAppView::Draw()
// Draws the display.
// -----------------------------------------------------------------------------
//
void CSimpleCubeAppView::Draw(const TRect& /*aRect*/) const
	{
	// Get the standard graphics context
	CWindowGc& gc = SystemGc();

	// Gets the control's extent
	TRect drawRect(Rect());

	// Clears the screen
	gc.Clear(drawRect);

	}

// -----------------------------------------------------------------------------
// CSimpleCubeAppView::SizeChanged()
// Called by framework when the view size is changed.
// -----------------------------------------------------------------------------
//
void CSimpleCubeAppView::SizeChanged()
	{
	DrawNow();
	}

// -----------------------------------------------------------------------------
// CSimpleCubeAppView::HandlePointerEventL()
// Called by framework to handle pointer touch events.
// Note: although this method is compatible with earlier SDKs, 
// it will not be called in SDKs without Touch support.
// -----------------------------------------------------------------------------
//
void CSimpleCubeAppView::HandlePointerEventL(const TPointerEvent& aPointerEvent)
	{

	// Call base class HandlePointerEventL()
	CCoeControl::HandlePointerEventL(aPointerEvent);
	}

// End of File
