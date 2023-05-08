/*
 ============================================================================
 Name		: SimpleCubeApplication.h
 Author	  : 
 Copyright   : 
 Description : Declares main application class.
 ============================================================================
 */

#ifndef __SIMPLECUBEAPPLICATION_H__
#define __SIMPLECUBEAPPLICATION_H__

// INCLUDES
#include <aknapp.h>
#include "SimpleCube.hrh"

// UID for the application;
// this should correspond to the uid defined in the mmp file
const TUid KUidSimpleCubeApp =
	{
	_UID3
	};

// CLASS DECLARATION

/**
 * CSimpleCubeApplication application class.
 * Provides factory to create concrete document object.
 * An instance of CSimpleCubeApplication is the application part of the
 * AVKON application framework for the SimpleCube example application.
 */
class CSimpleCubeApplication : public CAknApplication
	{
public:
	// Functions from base classes

	/**
	 * From CApaApplication, AppDllUid.
	 * @return Application's UID (KUidSimpleCubeApp).
	 */
	TUid AppDllUid() const;

protected:
	// Functions from base classes

	/**
	 * From CApaApplication, CreateDocumentL.
	 * Creates CSimpleCubeDocument document object. The returned
	 * pointer in not owned by the CSimpleCubeApplication object.
	 * @return A pointer to the created document object.
	 */
	CApaDocument* CreateDocumentL();
	};

#endif // __SIMPLECUBEAPPLICATION_H__
// End of File
