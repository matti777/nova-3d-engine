/*
 ============================================================================
 Name		: SimpleCubeDocument.h
 Author	  : 
 Copyright   : 
 Description : Declares document class for application.
 ============================================================================
 */

#ifndef __SIMPLECUBEDOCUMENT_h__
#define __SIMPLECUBEDOCUMENT_h__

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class CSimpleCubeAppUi;
class CEikApplication;

// CLASS DECLARATION

/**
 * CSimpleCubeDocument application class.
 * An instance of class CSimpleCubeDocument is the Document part of the
 * AVKON application framework for the SimpleCube example application.
 */
class CSimpleCubeDocument : public CAknDocument
	{
public:
	// Constructors and destructor

	/**
	 * NewL.
	 * Two-phased constructor.
	 * Construct a CSimpleCubeDocument for the AVKON application aApp
	 * using two phase construction, and return a pointer
	 * to the created object.
	 * @param aApp Application creating this document.
	 * @return A pointer to the created instance of CSimpleCubeDocument.
	 */
	static CSimpleCubeDocument* NewL(CEikApplication& aApp);

	/**
	 * NewLC.
	 * Two-phased constructor.
	 * Construct a CSimpleCubeDocument for the AVKON application aApp
	 * using two phase construction, and return a pointer
	 * to the created object.
	 * @param aApp Application creating this document.
	 * @return A pointer to the created instance of CSimpleCubeDocument.
	 */
	static CSimpleCubeDocument* NewLC(CEikApplication& aApp);

	/**
	 * ~CSimpleCubeDocument
	 * Virtual Destructor.
	 */
	virtual ~CSimpleCubeDocument();

public:
	// Functions from base classes

	/**
	 * CreateAppUiL
	 * From CEikDocument, CreateAppUiL.
	 * Create a CSimpleCubeAppUi object and return a pointer to it.
	 * The object returned is owned by the Uikon framework.
	 * @return Pointer to created instance of AppUi.
	 */
	CEikAppUi* CreateAppUiL();

private:
	// Constructors

	/**
	 * ConstructL
	 * 2nd phase constructor.
	 */
	void ConstructL();

	/**
	 * CSimpleCubeDocument.
	 * C++ default constructor.
	 * @param aApp Application creating this document.
	 */
	CSimpleCubeDocument(CEikApplication& aApp);

	};

#endif // __SIMPLECUBEDOCUMENT_h__
// End of File
