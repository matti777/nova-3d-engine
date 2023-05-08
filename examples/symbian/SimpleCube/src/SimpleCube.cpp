/*
 ============================================================================
 Name		: SimpleCube.cpp
 Author	  : 
 Copyright   : 
 Description : Main application class
 ============================================================================
 */

// INCLUDE FILES
#include <eikstart.h>
#include "SimpleCubeApplication.h"

LOCAL_C CApaApplication* NewApplication()
	{
	return new CSimpleCubeApplication;
	}

GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication(NewApplication);
	}

