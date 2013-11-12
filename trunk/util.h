///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2013, Sony Pictures Imageworks
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the
// distribution.  Neither the name of Sony Pictures Imageworks nor the
// names of its contributors may be used to endorse or promote
// products derived from this software without specific prior written
// permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////
//
//  util.h
//  spReticle.1.7
//
//  Created by Henry Vera on 4/23/13.
//
//

#ifndef spReticle_util_h
#define spReticle_util_h

#include <maya/MColor.h>
#include <maya/MString.h>

class Geom
{
public:
    double  x1, x2;
    double  y1, y2;
    double  x, y;
    MColor  lineColor;
    MColor  maskColor;
    bool    isValid;
};

class Aspect_Ratio
{
public:
    double aspectRatio;
    int    displayMode;
    int    displaySafeAction;
    int    displaySafeTitle;
    
    Geom   aspectGeom;
    Geom   safeActionGeom;
    Geom   safeTitleGeom;
};

class PanScan : public Aspect_Ratio
{
public:
    double panScanRatio;
    double panScanOffset;
};

class Filmback
{
public:
    double horizontalFilmAperture;
    double verticalFilmAperture;
    int    relativeFilmback;
    double soundTrackWidth;
    double horizontalImageAperture;
    double verticalImageAperture;
    int    displayFilmGate;
    double horizontalProjectionGate;
    double verticalProjectionGate;
    double horizontalSafeAction;
    double verticalSafeAction;
    double horizontalSafeTitle;
    double verticalSafeTitle;
    int    displayProjGate;
    int    displaySafeAction;
    int    displaySafeTitle;
    
    Geom   filmbackGeom;
    Geom   imageGeom;
    Geom   projGeom;
    Geom   safeActionGeom;
    Geom   safeTitleGeom;
};

class PadOptions
{
public:
    bool   usePad;
    bool   isPadded;
    double padAmountX;
    double padAmountY;
    int    displayMode;
    
    Geom   padGeom;
};

class Options
{
public:
    bool   drawingEnabled;
    bool   enableTextDrawing;
    short  cameraFilterMode;
    bool   displayLineH;
    bool   displayLineV;
    bool   displayThirdsH;
    bool   displayThirdsV;
    bool   displayCrosshair;
    bool   displayFieldGuide;
    bool   driveCameraAperture;
    bool   useOverscan;
    double maximumDistance;
    MColor textColor;
    MColor lineColor;
};

class TextData
{
public:
    int     textType;
    MString textStr;
    int     textAlign;
    int     textVAlign;
    double  textPosX;
    double  textPosY;
    int     textPosRel;
    int     textLevel;
    int     textARLevel;
    MColor  textColor;
    bool    textEnabled;
    int     textSize;
    bool    textBold;
    bool    textScale;
};

#endif
