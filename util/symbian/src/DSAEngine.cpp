/*
 *  $Id: DSAEngine.cpp 10 2009-08-28 12:41:25Z matti $
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

#include "DSAEngine.h"
#include "novalogging.h"

//using namespace nova3d;

EXPORT_C CDSAEngine::CDSAEngine( RWsSession& aSession,
                                 CWsScreenDevice& aScreenDevice,
                                 RWindow& aWindow,
                                 TInt aDrawInterval )
    : iSession( aSession ),
      iScreenDevice( aScreenDevice ),
      iWindow( aWindow),
      iDrawInterval( aDrawInterval ),
      iIsRunning( EFalse )
    {
    }

EXPORT_C void CDSAEngine::ConstructL()
{
    // initialize DSA
    iDirectScreenAccess = CDirectScreenAccess::NewL( iSession, 
                                                     iScreenDevice, 
                                                     iWindow, 
                                                     *this );

#ifdef __WINSCW__
    ////////////////////////////////
    // EMULATOR CODE
    ////////////////////////////////
    // create offscreen buffer for drawing to
    iScreenBufferBitmap = new (ELeave) CWsBitmap( iSession );
    TSize size = iScreenDevice.SizeInPixels();
    User::LeaveIfError(iScreenBufferBitmap->Create(size, KNovaDisplayMode));
    iScreenBufferAddrBase = (TPixel*)iScreenBufferBitmap->DataAddress();
#else
    ////////////////////////////////
    // DEVICE CODE
    ////////////////////////////////
    // find out the frame buffer address
    TScreenInfoV01 info;
    TPckg<TScreenInfoV01> screen_info(info);
    UserSvr::ScreenInfo(screen_info);
    TUint8* frame_buf_addr = NULL;

    if ( info.iScreenAddressValid ) 
        {
        frame_buf_addr = (TUint8*)info.iScreenAddress;
        } 
    else 
        {
        User::Leave( KErrNotFound );
        }

    //skip the palette data
    frame_buf_addr += 32;
    iScreenBufferAddrBase = (TPixel*)frame_buf_addr;

    // initialize screen redraw event
    iRedrawEvent.Set(TRawEvent::ERedraw);
#endif

    iScreenBufferAddr = iScreenBufferAddrBase;

    // create render timer
    iRenderTimer = CPeriodic::NewL( CActive::EPriorityStandard );
}

EXPORT_C CDSAEngine::~CDSAEngine()
{
    if ( iRenderTimer != NULL ) 
        {
        iRenderTimer->Cancel();
        delete iRenderTimer;
        }

    delete iDirectScreenAccess;

#ifdef __WINSCW__
    delete iScreenBufferBitmap;
#endif
}

EXPORT_C void CDSAEngine::StartL()
    {
    LOG_DEBUG("CDSAEngine::StartL()");
    
    if( iIsRunning ) 
        {
        // already running; stop first
        Stop();
        }

    // start the DSA (validates a drawable region)
    TRAPD( err, iDirectScreenAccess->StartL() );
    LOG_DEBUG(" - dsa StartL() = %d", err);
    User::LeaveIfError( err );

    // get graphics context
    iGc = iDirectScreenAccess->Gc();

    // get drawing region
    iRegion = iDirectScreenAccess->DrawingRegion();

    // update iDrawRect and drawing canvas
    iDrawRect = iRegion->BoundingRect();
    UpdateCanvas();

    // set clipping region
    iGc->SetClippingRegion(iRegion);

    // mark as running
    iIsRunning = ETrue;

    // start the render timer
    TCallBack cb( RenderTimerCallback, this );
    iRenderTimer->Start( 0, iDrawInterval, cb );
    }

TInt CDSAEngine::RenderTimerCallback( TAny* aParam )
    {
    CDSAEngine* me = static_cast<CDSAEngine*>(aParam);
    me->RenderL();
    me->UpdateScreen();
    }

EXPORT_C void CDSAEngine::Stop()
    {
    LOG_DEBUG("CDSAEngine::Stop()");
    
    // pause DSA
    iDirectScreenAccess->Cancel();
    iIsRunning = EFalse;

    // stop the render timer
    iRenderTimer->Cancel();
    }

EXPORT_C void CDSAEngine::DisplayScreen()   
    {
#ifdef __WINSCW__
    ////////////////////////////////
    // EMULATOR CODE
    ////////////////////////////////
    // draw the offscreen buffer to screen
    iGc->BitBlt( iDrawPos, iScreenBufferBitmap, iDrawRect );

    // force screen update
    iDirectScreenAccess->ScreenDevice()->Update();
#else
    ////////////////////////////////
    // DEVICE CODE
    ////////////////////////////////
    // generate event to refresh the screen (will also keep backlight on)
    UserSvr::AddEvent( iRedrawEvent );
#endif 
    }

// default implementation
EXPORT_C void CDSAEngine::RenderL()
    {
    // base implementation: draw coloured test screen
    register uint_32 *p = iCanvas.m_bufferPtr;

    for( register TInt i = 0; i < iCanvas.m_right; i++ ) 
        {
        for( register TInt j = 0; j < iCanvas.m_bottom; j++ ) 
            {
            *p++ = PIXEL_888(i, j, i + j);
            }
        }
    }

EXPORT_C const RenderingCanvas& CDSAEngine::GetCanvas()
    {
    return iCanvas;
    }

void CDSAEngine::UpdateCanvas()
    {
    // set physical properties of the drawing canvas
    iCanvas.iTop = iDrawRect.iTl.iY;
    iCanvas.iLeft = iDrawRect.iTl.iX;
    iCanvas.iBottom = iDrawRect.iBr.iY;
    iCanvas.iRight = iDrawRect.iBr.iX;
    iCanvas.iWidth = iCanvas.iRight - iCanvas.iLeft;
    iCanvas.iHeight = iCanvas.iBottom - iCanvas.iTop;
    iCanvas.iCenterX = iCanvas.iLeft + (iCanvas.iWidth / 2);
    iCanvas.iCenterY = iCanvas.iTop + (iCanvas.iHeight / 2);
    iCanvas.iPixelsPerScanline = iCanvas.iWidth;
    iCanvas.iBytesPerScanline = iCanvas.iPixelsPerScanline * sizeof(TPixel);

#ifndef __WINSCW__
    //===========================================================
    // DEVICE CODE
    //===========================================================

    // need to adjust the buffer pointer+the canvas
    // so that we only draw to the current area. on the emulator
    // we draw to the fullscreen -sized bitmap and then use
    // DSA clipping.
    TInt shiftAmount = iCanvas.iTop;
    iCanvas.iTop -= shiftAmount;
    iCanvas.iBottom -= shiftAmount;
    iCanvas.iCenterY -= shiftAmount;
    iScreenBufferAddr = iScreenBufferAddrBase + 
                        shiftAmount * iCanvas.iPixelsPerScanline;
#endif

    // update the canvas's screen buffer pointer also
    iCanvas.m_bufferPtr = iScreenBufferAddr;

    CanvasUpdated();
    }

void CDSAEngine::CanvasUpdated() const
    {
    // base implementation does nothing
    }

EXPORT_C TBool CDSAEngine::HandleKeyEvent( const TKeyEvent& aKeyEvent, 
                                           TEventCode aType )
    {
    // nevermind "keypress" (keydown+keyup) events
    if ( aType == EEventKey ) 
        {
        return EFalse;
        }

    TBool keyState = (aType == EEventKeyDown);

    switch ( aKeyEvent.iScanCode ) 
        {
        case EStdKeyDevice0:
            iKeyJoystickButtonActive = keyState;
            return ETrue;
            break;
        case EStdKeyDownArrow:
            iKeyDownArrowActive = keyState;
            return ETrue;
            break;
        case EStdKeyUpArrow:
            iKeyUpArrowActive = keyState;
            return ETrue;
            break;
        case EStdKeyLeftArrow:
            iKeyLeftArrowActive = keyState;
            return ETrue;
            break;
        case EStdKeyRightArrow:
            iKeyRightArrowActive = keyState;
            return ETrue;
            break;
        default:
            // do nothing
            break;
        }

    return EFalse;
    }

void CDSAEngine::Redraw()
    {
    RenderL();
    DisplayScreen();
    }

EXPORT_C void CDSAEngine::Restart(
        RDirectScreenAccess::TTerminationReasons /*aReason*/)
    {
    // start drawing again. clipping area might get adjusted
    StartL();
    }

EXPORT_C void CDSAEngine::AbortNow(
        RDirectScreenAccess::TTerminationReasons /*aReason*/)
    {
    Stop();
    }
