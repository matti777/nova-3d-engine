/*
 *  $Id: DSAEngine.h 10 2009-08-28 12:41:25Z matti $
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

#ifndef __DSAENGINE_H
#define __DSAENGINE_H

// FILE INFO
// This file holds DSAEngine class which may be inherted by an application
// side class to use the Direct Screen Access functionality.

#include <w32std.h>

#include "Display.h"

//namespace nova3d {

/**
 * Direct Screen Access -engine that contains handling for the basic
 * functionality. Intended to be inherited by an application side class 
 * (implement RenderL() to draw screens).<p />
 *
 * @author Matti Dahlbom
 * @version $Name:  $, $Revision: 10 $
 */
class CDSAEngine : public CBase, 
                   public MDirectScreenAccess/*,
                   public MRenderable*/
{
    public: // Constructors and destructor
        IMPORT_C virtual ~CDSAEngine();

    public: // new methods (exported API)
        /**
         * Starts the engine.<P>
         */
        IMPORT_C void StartL();

        /**
         * Stops (pauses) the engine.<P>
         */
        IMPORT_C void Stop();

        /**
         * Returns a TCanvas matching the current draw rect.
         */
        IMPORT_C const RenderingCanvas& GetCanvas();

        /**
         * A key event was received. This method sets/unsets the <code>iKey*</code>
         * variables, to be used by the deriving classes.<P>
         *
         * NOTE: the application using a derivative of this class (and hence this
         * class) must call this method from its own key event handling methods
         * in order to maintain the state of the key state variables correctly.<P>
         *
         * Also the application might want to call 
         * <code>CAknAppUi::SetKeyBlockMode( ENoKeyBlock )</code> to make key events
         * nonblocking (allowing multiple keys to produce key events simultaneously).<P>
         *
         * @return ETrue if the key was consumed
         */
        IMPORT_C virtual TBool HandleKeyEvent( const TKeyEvent& aKeyEvent, 
                                               TEventCode aType );

        void Redraw();

    protected: // New methods (Public API)
        /**
         * Inheriting class needs to override this method to render frames.
         */
        IMPORT_C virtual void RenderL();

        /**
         * Flushes the double buffer to the screen.<p />
         */
        IMPORT_C void DisplayScreen();

    protected: // New methods (Public API)
        /**
         * Client may override this method to receive notifications about the 
         * rendering canvas being updated. In the overridden method, 
         * client should call 
         * <code>CanvasWasUpdatedL()</code> for each of its cameras.
         */
        IMPORT_C virtual void CanvasUpdated() const;

    protected: // Constructors
        /**
         * Constructs.<p />
         *
         * @param aSession window server session
         * @param aScreenDevice screen device
         * @param aWindow current window
         * @param aDrawInterval time (in us) to sleep between frames. Default
         * is 0us.
         */
        IMPORT_C CDSAEngine( RWsSession &aSession,
                             CWsScreenDevice &aScreenDevice,
                             RWindow &aWindow,
                             TInt aDrawInterval = 0 );
        /**
         * 2nd phase constructor.<P>
         */
        IMPORT_C void ConstructL();

    private: // New methods
        static TInt RenderTimerCallback( TAny* aParam );
        
    private: // from MDirectScreenAccess
        //NOTE: these are exported to allow inheritance of this class!
        IMPORT_C void Restart(RDirectScreenAccess::TTerminationReasons aReason);
        IMPORT_C void AbortNow(RDirectScreenAccess::TTerminationReasons aReason);

    protected: // Data
        // key statuses
        TBool iKeyUpArrowActive;
        TBool iKeyDownArrowActive;
        TBool iKeyLeftArrowActive;
        TBool iKeyRightArrowActive;
        TBool iKeyJoystickButtonActive;

        // render timer
        CPeriodic* iRenderTimer;

    private: // new methods
        /**
         * Updates the rendering canvas from the draw rect.
         */
        void UpdateCanvas();

    private: // data
        // window server handles
        RWsSession &iSession;
        CWsScreenDevice &iScreenDevice;
        RWindow &iWindow;

        // direct screen access handles
        CDirectScreenAccess* iDirectScreenAccess;
        CFbsBitGc* iGc;
        RRegion* iRegion;

        // drawing resources
        TPoint iDrawPos;
        TRect iDrawRect;

        // base address of the offscreen buffer
        void* iScreenBufferAddrBase;

        // pointer to offscreen buffer (adjusts to screen size)
        void* iScreenBufferAddr;

#ifdef __WINSCW__
        // actual device code writes directly to the screen 
        CFbsBitmap* iScreenBufferBitmap;
#else
        // screen redraw event (when running on device)
        TRawEvent iRedrawEvent;
#endif

        // Whether the engine is currently 'running'
        TBool iIsRunning;

        // time to sleep between frames
        TInt iDrawInterval;

        // canvas for current draw rect
        RenderingCanvas iCanvas;
    };

//}; // namespace

#endif

