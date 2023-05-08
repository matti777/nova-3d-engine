/*
 ============================================================================
 Name		: SimpleCube.pan
 Author	  : 
 Copyright   : 
 Description : This file contains panic codes.
 ============================================================================
 */

#ifndef __SIMPLECUBE_PAN__
#define __SIMPLECUBE_PAN__

/** SimpleCube application panic codes */
enum TSimpleCubePanics
	{
	ESimpleCubeUi = 1
	// add further panics here
	};

inline void Panic(TSimpleCubePanics aReason)
	{
	_LIT(applicationName, "SimpleCube");
	User::Panic(applicationName, aReason);
	}

#endif // __SIMPLECUBE_PAN__
