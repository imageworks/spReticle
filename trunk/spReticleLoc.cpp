///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2009, Sony Pictures Imageworks
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


/*
 * @file  spReticleLoc.cpp
 * This plug-in creates a custom locator to display a customizable reticle.
 * The reticle has the ability to display any number of aspect ratios as
 * well as pan-and-scan, projection masks, filmback data, etc. The look of
 * the reticle is customizable in regards to color and transparency for
 * masks, lines and text.There is also an option to specify whether the
 * reticle should respect the show/hide locator display options. Finally,
 * the reticle can be filtered to only display on cameras which contain
 * an attribute (see defines.h) set to true or are connected to the reticle.
 */

#include "defines.h"

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

#include <maya/MPxLocatorNode.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MColor.h>
#include <maya/M3dView.h>
#include <maya/MFnPlugin.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MIOStream.h>
#include <maya/MDagPath.h>
#include <maya/MFnCamera.h>
#include <maya/MVector.h>
#include <maya/MMatrix.h>
#include <maya/MFnMatrixData.h>
#include <maya/MFnStringData.h>
#include <maya/MTime.h>
#include <maya/MGlobal.h>
#include <maya/MFileIO.h>
#include <maya/MFileObject.h>
#include <maya/MFnMessageAttribute.h>

#if (MAYA_API_VERSION>=201200)
// Viewport 2.0 includes
#include <maya/MDrawRegistry.h>
#include <maya/MPxDrawOverride.h>
#include <maya/MUserData.h>
#include <maya/MDrawContext.h>
#include <maya/MGlobal.h>
#include <maya/MSelectionList.h>
#endif

#include "spReticleLoc.h"

#define McheckStatus(stat,msg)  \
    if (!stat) {                \
        stat.perror(msg);       \
        return stat;            \
    }

#define McheckVoid(stat,msg)    \
    if (!stat) {                \
        stat.perror(msg);       \
        return;                 \
    }

MTypeId spReticleLoc::id( 0x00000502 );

#if (MAYA_API_VERSION>=201200)
	MString	spReticleLoc::drawDbClassification("drawdb/geometry/spReticleLoc");
	MString	spReticleLoc::drawRegistrantId("spReticleLoc");
#endif

MObject spReticleLoc::DrawingEnabled;
MObject spReticleLoc::EnableTextDrawing;
MObject spReticleLoc::FilmbackAperture;
MObject spReticleLoc::HorizontalFilmAperture;
MObject spReticleLoc::VerticalFilmAperture;
MObject spReticleLoc::RelativeFilmback;
MObject spReticleLoc::SoundTrackWidth;
MObject spReticleLoc::DisplayFilmGate;
MObject spReticleLoc::ProjectionGate;
MObject spReticleLoc::HorizontalProjectionGate;
MObject spReticleLoc::VerticalProjectionGate;
MObject spReticleLoc::DisplayProjectionGate;
MObject spReticleLoc::SafeAction;
MObject spReticleLoc::HorizontalSafeAction;
MObject spReticleLoc::VerticalSafeAction;
MObject spReticleLoc::DisplaySafeAction;
MObject spReticleLoc::SafeTitle;
MObject spReticleLoc::HorizontalSafeTitle;
MObject spReticleLoc::VerticalSafeTitle;
MObject spReticleLoc::DisplaySafeTitle;
MObject spReticleLoc::AspectRatios;
MObject spReticleLoc::AspectRatio;
MObject spReticleLoc::DisplayMode;
MObject spReticleLoc::AspectMaskColor;
MObject spReticleLoc::AspectMaskTrans;
MObject spReticleLoc::AspectLineColor;
MObject spReticleLoc::AspectLineTrans;
MObject spReticleLoc::AspectDisplaySafeAction;
MObject spReticleLoc::AspectDisplaySafeTitle;
MObject spReticleLoc::PanScanAttr;
MObject spReticleLoc::PanScanAspectRatio;
MObject spReticleLoc::PanScanDisplayMode;
MObject spReticleLoc::PanScanDisplaySafeTitle;
MObject spReticleLoc::PanScanDisplaySafeAction;
MObject spReticleLoc::PanScanRatio;
MObject spReticleLoc::PanScanOffset;
MObject spReticleLoc::PanScanMaskColor;
MObject spReticleLoc::PanScanMaskTrans;
MObject spReticleLoc::PanScanLineColor;
MObject spReticleLoc::PanScanLineTrans;
MObject spReticleLoc::FilmGateMaskColor;
MObject spReticleLoc::FilmGateMaskTrans;
MObject spReticleLoc::FilmGateLineColor;
MObject spReticleLoc::FilmGateLineTrans;
MObject spReticleLoc::ProjGateMaskColor;
MObject spReticleLoc::ProjGateMaskTrans;
MObject spReticleLoc::ProjGateLineColor;
MObject spReticleLoc::ProjGateLineTrans;
MObject spReticleLoc::HideLocator;

// Deprecating this attribute in favor of CameraFilterMode, using an alias temporarily
//MObject spReticleLoc::UseSpReticle;
MObject spReticleLoc::CameraFilterMode;
MObject spReticleLoc::Cameras;

MObject spReticleLoc::DisplayLineH;
MObject spReticleLoc::DisplayLineV;
MObject spReticleLoc::DisplayThirdsH;
MObject spReticleLoc::DisplayThirdsV;
MObject spReticleLoc::DisplayCrosshair;
MObject spReticleLoc::DisplayFieldGuide;
MObject spReticleLoc::MiscTextColor;
MObject spReticleLoc::MiscTextTrans;
MObject spReticleLoc::LineColor;
MObject spReticleLoc::LineTrans;
MObject spReticleLoc::Time;
MObject spReticleLoc::DriveCameraAperture;
MObject spReticleLoc::MaximumDistance;
MObject spReticleLoc::UseOverscan;
MObject spReticleLoc::Pad;
MObject spReticleLoc::UsePad;
MObject spReticleLoc::PadAmount;
MObject spReticleLoc::PadAmountX;
MObject spReticleLoc::PadAmountY;
MObject spReticleLoc::PadDisplayMode;
MObject spReticleLoc::PadMaskColor;
MObject spReticleLoc::PadMaskTrans;
MObject spReticleLoc::PadLineColor;
MObject spReticleLoc::PadLineTrans;
MObject spReticleLoc::Text;
MObject spReticleLoc::TextType;
MObject spReticleLoc::TextStr;
MObject spReticleLoc::TextAlign;
MObject spReticleLoc::TextVAlign;
MObject spReticleLoc::TextPos;
MObject spReticleLoc::TextPosX;
MObject spReticleLoc::TextPosY;
MObject spReticleLoc::TextPosRel;
MObject spReticleLoc::TextLevel;
MObject spReticleLoc::TextARLevel;
MObject spReticleLoc::TextColor;
MObject spReticleLoc::TextTrans;
MObject spReticleLoc::TextEnabled;
MObject spReticleLoc::TextBold;
MObject spReticleLoc::TextSize;
MObject spReticleLoc::TextScale;
MObject spReticleLoc::Tag;

spReticleLoc::spReticleLoc() {}
spReticleLoc::~spReticleLoc() {}

// This method will retrieve the individual r,g,b and alpha values from
// various plugs and store them in an MColor object.
//
MStatus spReticleLoc::getColor(MObject colorObj, MObject transObj, MColor & color)
{
    MPlug p,c;
    MStatus stat;

    p = MPlug( thisNode, colorObj );

    // Get red
    c = p.child(0, &stat);
    McheckStatus (stat, ("spReticleLoc::getColor getting red plug from "+p.name() ) );
    McheckStatus ( c.getValue ( color.r  ), ("spReticleLoc::getColor getting red from "+p.name() ) );

    // Get green
    c = p.child(1,&stat);
    McheckStatus (stat, ("spReticleLoc::getColor getting green plug from "+p.name() ) );
    McheckStatus ( c.getValue ( color.g  ), ("spReticleLoc::getColor getting green from "+p.name() ) );

    // Get blue
    c = p.child(2, &stat);
    McheckStatus (stat, ("spReticleLoc::getColor getting blue plug from "+p.name() ) );
    McheckStatus ( c.getValue ( color.b  ), ("spReticleLoc::getColor getting blue from "+p.name() ) );

    p = MPlug( thisNode, transObj );
    McheckStatus ( p.getValue ( color.a ), ("spReticleLoc::getColor gettings transparency from "+p.name() ) );

    return MS::kSuccess;
}

//This method returns the specified matrix.
//
MMatrix spReticleLoc::getMatrix(MString matrixStr)
{
    MStatus stat;
    MMatrix matrix;

    // Extract the matrix attribute that is inherited from 'dagNode'
    MFnDependencyNode fnThisNode( thisNode );
    MObject matrixAttribute = fnThisNode.attribute( matrixStr, &stat );
    if (!stat)
    {
        stat.perror("spReticleLoc::getMatrix getting attribute : "+matrixStr);
        return matrix.identity;
    }

    MPlug matrixPlug( thisNode, matrixAttribute);
    if (matrixPlug.isNull())
    {
        stat.perror("spReticleLoc::getting plug for attribute : "+matrixStr);
        return matrix.identity;
    }

    // If matrix is an array so we must specify which element the plug refers to
    if (matrixPlug.isArray())
    {
        matrixPlug = matrixPlug.elementByLogicalIndex (0, &stat);
        if (!stat)
        {
            stat.perror("spReticleLoc::getMatrix setting logical index to 0 for "+matrixStr);
            return matrix.identity;
        }
    }

    // Get the value of the matrix attribute
    MObject matObject;
    stat = matrixPlug.getValue(matObject);
    if (!stat)
    {
        stat.perror("spReticleLoc::getMatrix getting value for "+matrixStr);
        return matrix.identity;
    }

    MFnMatrixData matrixData(matObject, &stat);
    if (!stat)
    {
        stat.perror("spReticleLoc::getMatrix getting MfnMatrixData");
        return matrix.identity;
    }

    matrix = matrixData.matrix( &stat );
    if (!stat)
    {
        stat.perror("spReticleLoc::getMatrix getting matrix");
        return matrix.identity;
    }

    return matrix;
}

// This method outputs to cerr the attributes of an Aspect_Ratio
// class instance.
//
void spReticleLoc::printAspectRatio(Aspect_Ratio & ar)
{
    cerr << "-------------------------------------------------" << endl;
    cerr << "aspect ratio      : " << ar.aspectRatio << endl;
    cerr << "displayMode       : " << ar.displayMode << endl;
    cerr << "maskColor         : " << ar.aspectGeom.maskColor << endl;
    cerr << "lineColor         : " << ar.aspectGeom.lineColor << endl;
}

// This method outputs to cerr the attributes of an PanScan class instance.
//
void spReticleLoc::printPanScan(PanScan & ps)
{
    printAspectRatio( ps );

    cerr << "panScanRatio      : " << ps.panScanRatio << endl;
    cerr << "panScanOffset     : " << ps.panScanOffset << endl;
    cerr << "displaySafeTitle  : " << ps.displaySafeTitle << endl;
    cerr << "displaySafeAction : " << ps.displaySafeAction << endl;
}

// This method outputs to cerr the attributes of an TextData class instance.
//
void spReticleLoc::printText(TextData & td)
{
    cerr << "-------------------------------------------------" << endl;
    cerr << "text type         : " << td.textType << endl;
    cerr << "text string       : " << td.textStr << endl;
    cerr << "text align        : " << td.textAlign << endl;
    cerr << "text pos          : " << td.textPosX << ", " << td.textPosY << endl;
    cerr << "text pos rel      : " << td.textPosRel << endl;
    cerr << "text level        : " << td.textLevel << endl;
    cerr << "text ar level     : " << td.textARLevel << endl;
}

// This method outputs to cerr the attributes of an Geom class instance.
//
void spReticleLoc::printGeom(Geom & g)
{
    cerr << "-------------------------------------------------" << endl;
    cerr << "x  : " << g.x << endl;
    cerr << "y  : " << g.y << endl;
    cerr << "x1 : " << g.x1 << endl;
    cerr << "x2 : " << g.x2 << endl;
    cerr << "y1 : " << g.y1 << endl;
    cerr << "y2 : " << g.y2 << endl;
    cerr << "mc : " << g.maskColor << endl;
    cerr << "lc : " << g.lineColor << endl;
    cerr << "iv : " << g.isValid << endl;
}

// This method outputs to cerr the various options that have been specified.
//
void spReticleLoc::printOptions()
{
    cerr << "-------------------------------------------------" << endl;
    cerr << "cameraFilterMode   : " << options.cameraFilterMode << endl;
    cerr << "displayLineH        : " << options.displayLineH << endl;
    cerr << "displayLineV        : " << options.displayLineV << endl;
    cerr << "displayThirdsH      : " << options.displayThirdsH << endl;
    cerr << "displayThirdsV      : " << options.displayThirdsV << endl;
    cerr << "displayCrosshair    : " << options.displayCrosshair << endl;
    cerr << "displayFieldGuide   : " << options.displayFieldGuide << endl;
    cerr << "driveCameraAperture : " << options.driveCameraAperture << endl;
    cerr << "maximumDistance     : " << options.maximumDistance << endl;
    cerr << "useOverscan         : " << options.useOverscan << endl;
}

// This method gets the pad data.
//
MStatus spReticleLoc::getPadData()
{
    MPlug p;
    MStatus stat;

    // Get the pad attribute;
    p = MPlug ( thisNode, UsePad );
    McheckStatus ( p.getValue ( pad.usePad ), "spReticleLoc::getPadData pad");

    // Get the padAmountX attribute;
    p = MPlug ( thisNode, PadAmountX );
    McheckStatus ( p.getValue ( pad.padAmountX  ), "spReticleLoc::getPadData padAmountX");

    // Get the padAmountY attribute;
    p = MPlug ( thisNode, PadAmountY );
    McheckStatus ( p.getValue ( pad.padAmountY  ), "spReticleLoc::getPadData padAmountX");

    // Set whether the filmback is padded
    pad.isPadded = (pad.padAmountX > EPSILON || pad.padAmountY > EPSILON );

    if (pad.usePad && pad.isPadded)
    {
        // Get the pad display mode
        p = MPlug ( thisNode, PadDisplayMode );
        McheckStatus ( p.getValue ( pad.displayMode ), "spReticleLoc::getPadData padDisplayMode");

        // Get the pad mask color
        stat = getColor( PadMaskColor, PadMaskTrans, pad.padGeom.maskColor );
        McheckStatus ( stat, "spReticleLoc::getPadData get padMaskColor");

        // Get the pad line color
        stat = getColor( PadLineColor, PadLineTrans, pad.padGeom.lineColor );
        McheckStatus ( stat, "spReticleLoc::getPadData get padLineColor");
    }
    else
    {
        pad.padAmountX = 0;
        pad.padAmountY = 0;
    }

    return MS::kSuccess;
}

// This method retrieves all of the data related to the filmback from
// various plugs.
//
MStatus spReticleLoc::getFilmbackData()
{
    MPlug p;
    MStatus stat;

    //Get horizontal film aperture
    p = MPlug( thisNode, HorizontalFilmAperture );
    McheckStatus ( p.getValue ( filmback.horizontalFilmAperture  ), "spReticleLoc::getFilmbackData get horizontalFilmAperture");

    //Get vertical film aperture
    p = MPlug( thisNode, VerticalFilmAperture );
    McheckStatus ( p.getValue ( filmback.verticalFilmAperture  ), "spReticleLoc::getFilmbackData get verticalFilmAperture");

    //Get whether the film aperture is relative or absolute
    p = MPlug( thisNode, RelativeFilmback );
    McheckStatus ( p.getValue ( filmback.relativeFilmback  ), "spReticleLoc::getFilmbackData get relativeFilmback");

    //Get sound track width
    p = MPlug( thisNode, SoundTrackWidth );
    McheckStatus ( p.getValue ( filmback.soundTrackWidth  ), "spReticleLoc::getFilmbackData get soundTrackWidth");

    //Get whether to display the film gate
    p = MPlug( thisNode, DisplayFilmGate );
    McheckStatus ( p.getValue ( filmback.displayFilmGate  ), "spReticleLoc::getFilmbackData get displayFilmGate");
    
    //Get the filmback mask color
    stat = getColor( FilmGateMaskColor, FilmGateMaskTrans, filmback.filmbackGeom.maskColor );
    McheckStatus ( stat, "spReticleLoc::getFilmbackData get filmGateMaskColor");
    
    //Get the filmback line color
    stat = getColor( FilmGateLineColor, FilmGateLineTrans, filmback.filmbackGeom.lineColor );
    McheckStatus ( stat, "spReticleLoc::getFilmbackData get filmGateLineColor");

    return MS::kSuccess;
}

// This method retrieves all of the data related to the projection mask from
// various plugs and then calls calcProjGeom to calculate the various
// projection geometry instances.
//
MStatus spReticleLoc::getProjectionData()
{
    MPlug p;
    MStatus stat;

    //Get whether to display the projection gate
    p = MPlug( thisNode, DisplayProjectionGate );
    McheckStatus ( p.getValue ( filmback.displayProjGate  ), "spReticleLoc::getProjectionData get displayProjectionGate");

    if ( filmback.displayProjGate )
    {
        //Get horizontal projection gate
        p = MPlug( thisNode, HorizontalProjectionGate );
        McheckStatus ( p.getValue ( filmback.horizontalProjectionGate  ), "spReticleLoc::getProjectionData get horizontalProjectionGate");

        //Get vertical projection gate
        p = MPlug( thisNode, VerticalProjectionGate );
        McheckStatus ( p.getValue ( filmback.verticalProjectionGate  ), "spReticleLoc::getProjectionData get verticalProjectionGate");

        //Get the projection gate mask color
        stat = getColor( ProjGateMaskColor, ProjGateMaskTrans, filmback.projGeom.maskColor );
        McheckStatus ( stat, "spReticleLoc::getProjectionData get projGateMaskColor");

        //Get the projection gate line color
        stat = getColor( ProjGateLineColor, ProjGateLineTrans, filmback.projGeom.lineColor );
        McheckStatus ( stat, "spReticleLoc::getProjectionData get projGateLineColor");
    }

    return MS::kSuccess;
}

// This method returns the data related to the filmback's safe action area.
//
MStatus spReticleLoc::getSafeActionData()
{
    MPlug p;

    //Get whether to display safe action
    p = MPlug( thisNode, DisplaySafeAction );
    McheckStatus ( p.getValue ( filmback.displaySafeAction  ), "spReticleLoc::getSafeActionData get displaySafeAction");

    //Get horizontal safe action
    p = MPlug( thisNode, HorizontalSafeAction );
    McheckStatus ( p.getValue ( filmback.horizontalSafeAction  ), "spReticleLoc::getSafeActionData get horizontalSafeAction");

    //Get vertical safe action
    p = MPlug( thisNode, VerticalSafeAction );
    McheckStatus ( p.getValue ( filmback.verticalSafeAction  ), "spReticleLoc::getSafeActionData get verticalSafeAction");

    //Set the safe action line color to the filmback line color
    filmback.safeActionGeom.lineColor = filmback.filmbackGeom.lineColor;

    return MS::kSuccess;
}

// This method returns the data relating to the filmback's safe title area.
//
MStatus spReticleLoc::getSafeTitleData()
{
    MPlug p;

    //Get whether to display safe title
    p = MPlug( thisNode, DisplaySafeTitle );
    McheckStatus ( p.getValue ( filmback.displaySafeTitle  ), "spReticleLoc::getSafeTitleData get displaySafeTitle");

    //Get horizontal safe title
    p = MPlug( thisNode, HorizontalSafeTitle );
    McheckStatus ( p.getValue ( filmback.horizontalSafeTitle  ), "spReticleLoc::getSafeTitleData get horizontalSafeTitle");

    //Get vertical safe title
    p = MPlug( thisNode, VerticalSafeTitle );
    McheckStatus ( p.getValue ( filmback.verticalSafeTitle  ), "spReticleLoc::getSafeTitleData get verticalSafeTitle");

    //Set the safe title line color to the filmback line color
    filmback.safeTitleGeom.lineColor = filmback.filmbackGeom.lineColor;

    return MS::kSuccess;
}

// This method gets the data for a specified aspect ratio. The aspectratios
// attribute is effectively a complex compound attribute where each index
// is the data for a specific aspect ratio.
//
MStatus spReticleLoc::getAspectRatioChildren(MPlug arPlug, Aspect_Ratio & ar)
{
    MStatus stat;
    MPlug p, c;

    p = arPlug.child( 0 , &stat );
    McheckStatus( p.getValue( ar.aspectRatio ), "spReticleLoc::getAspectRatioChildren - aspectRatio" );

    p = arPlug.child( 1 , &stat );
    McheckStatus( p.getValue( ar.displayMode ), "spReticleLoc::getAspectRatioChildren - displayMode" );

    if (ar.displayMode != 0)
    {
        p = arPlug.child( 2 , &stat );
        c = p.child( 0, &stat );
        McheckStatus( c.getValue( ar.aspectGeom.maskColor.r ), "spReticleLoc::getAspectRatioChildren - aspectMaskColorR" );

        c = p.child( 1, &stat );
        McheckStatus( c.getValue( ar.aspectGeom.maskColor.g ), "spReticleLoc::getAspectRatioChildren - aspectMaskColorG" );

        c = p.child( 2, &stat );
        McheckStatus( c.getValue( ar.aspectGeom.maskColor.b ), "spReticleLoc::getAspectRatioChildren - aspectMaskColorB" );

        p = arPlug.child( 3 , &stat );
        McheckStatus( p.getValue( ar.aspectGeom.maskColor.a ), "spReticleLoc::getAspectRatioChildren - aspectMaskTrans" );

        p = arPlug.child( 4 , &stat );
        c = p.child( 0, &stat );
        McheckStatus( c.getValue( ar.aspectGeom.lineColor.r ), "spReticleLoc::getAspectRatioChildren - aspectLineColorR" );

        c = p.child( 1, &stat );
        McheckStatus( c.getValue( ar.aspectGeom.lineColor.g ), "spReticleLoc::getAspectRatioChildren - aspectLineColorG" );

        c = p.child( 2, &stat );
        McheckStatus( c.getValue( ar.aspectGeom.lineColor.b ), "spReticleLoc::getAspectRatioChildren - aspectLineColorB" );

        p = arPlug.child( 5 , &stat );
        McheckStatus( p.getValue( ar.aspectGeom.lineColor.a ), "spReticleLoc::getAspectRatioChildren - aspectLineTrans" );

        p = arPlug.child( 6 , &stat );
        McheckStatus( p.getValue( ar.displaySafeAction ), "spReticleLoc::getAspectRatioChildren - displaySafeAction" );

        p = arPlug.child( 7 , &stat );
        McheckStatus( p.getValue( ar.displaySafeTitle ), "spReticleLoc::getAspectRatioChildren - displaySafeTitle" );
    }

    return MS::kSuccess;
}

bool spReticleLoc::aspectRatioSortPredicate(const Aspect_Ratio & lhs,
                                             const Aspect_Ratio & rhs)
{
    return lhs.aspectRatio <= rhs.aspectRatio;
}

// This retrieves the data for all of the various aspect ratios.
//
MStatus spReticleLoc::getAspectRatioData()
{
    // Get the aspectRatios data
    MPlug arsPlug = MPlug( thisNode, AspectRatios );
    int numPlugs = arsPlug.numElements();

    MStatus stat;

    // Clear the aspect ratio vector
    ars.clear();

    for (int i = 0; i < numPlugs; i++)
    {
        MPlug p = arsPlug.elementByPhysicalIndex( i, &stat );
        McheckStatus( stat, "spReticleLoc::getAspectRatioData - cannot get index" );

        Aspect_Ratio ar;
        stat = getAspectRatioChildren( p, ar );
        McheckStatus( stat, "spReticleLoc::getAspectRatioData - cannot get children" );

        //printAspectRatio( ar );

        ars.push_back( ar );
    }

    numAspectRatios = (int)ars.size();

    // Sort aspect ratios
    std::sort(ars.begin(),ars.end(),aspectRatioSortPredicate);

    return MS::kSuccess;
}

bool spReticleLoc::needToUpdateAspectRatios()
{
    MPlug arsPlug = MPlug( thisNode, AspectRatios );
    return (ars.size() != arsPlug.numElements() );
}

// This method gets the data for a PanScan class instance.
//
MStatus spReticleLoc::getPanScanData ( PanScan & ps )
{
    MPlug psPlug( thisNode, PanScanAttr );

    MStatus stat = getAspectRatioChildren( psPlug, ps );
    McheckStatus( stat, "spReticleLoc::getPanScanData - cannot get children" );

    if (ps.displayMode != 0)
    {
        MPlug p;

        p = psPlug.child( 8 , &stat );
        McheckStatus( p.getValue( ps.panScanRatio ), "spReticleLoc::getPanScanData - panScanRatio" );

        p = psPlug.child( 9 , &stat );
        McheckStatus( p.getValue( ps.panScanOffset ), "spReticleLoc::getPanScanData - panScanOffset" );
    }

    //printPanScan( ps );

    return MS::kSuccess;
}

// This method gets the data for a specified text entry. The text attribute
// is a complex compound attribute where each index is the data for a specific
// text element.
//
MStatus spReticleLoc::getTextChildren(MPlug tPlug, TextData & td)
{
    MStatus stat;
    MPlug p, c;

    p = tPlug.child( 0 , &stat );
    McheckStatus( p.getValue( td.textType ), "spReticleLoc::getTextChildren - textType" );

    p = tPlug.child( 1 , &stat );
    McheckStatus( p.getValue( td.textStr ), "spReticleLoc::getTextChildren - textStr" );

    p = tPlug.child( 2 , &stat );
    McheckStatus( p.getValue( td.textAlign ), "spReticleLoc::getTextChildren - textAlign" );

    p = tPlug.child( 3 , &stat );
    c = p.child( 0, &stat );
    McheckStatus( c.getValue( td.textPosX ), "spReticleLoc::getTextChildren - textPosX" );

    c = p.child( 1, &stat );
    McheckStatus( c.getValue( td.textPosY ), "spReticleLoc::getTextChildren - textPosY" );

    p = tPlug.child( 4 , &stat );
    McheckStatus( p.getValue( td.textPosRel ), "spReticleLoc::getTextChildren - textPosRel" );

    p = tPlug.child( 5 , &stat );
    McheckStatus( p.getValue( td.textLevel ), "spReticleLoc::getTextChildren - textLevel" );

    p = tPlug.child( 6 , &stat );
    McheckStatus( p.getValue( td.textARLevel ), "spReticleLoc::getTextChildren - textARLevel" );

    p = tPlug.child( 7 , &stat );
    c = p.child( 0, &stat );
    McheckStatus( c.getValue( td.textColor.r ), "spReticleLoc::getTextChildren - textColorR" );

    c = p.child( 1, &stat );
    McheckStatus( c.getValue( td.textColor.g ), "spReticleLoc::getTextChildren - textColorG" );

    c = p.child( 2, &stat );
    McheckStatus( c.getValue( td.textColor.b ), "spReticleLoc::getTextChildren - textColorB" );

    p = tPlug.child( 8 , &stat );
    McheckStatus( p.getValue( td.textColor.a ), "spReticleLoc::getTextChildren - textTrans" );

    p = tPlug.child( 9, &stat );
    McheckStatus( p.getValue( td.textEnabled ), "spReticleLoc::getTextChildren - textEnabled" );

    p = tPlug.child( 10 , &stat );
    McheckStatus( p.getValue( td.textBold ), "spReticleLoc::getTextChildren - textBold" );
        
    p = tPlug.child( 11 , &stat );
    McheckStatus( p.getValue( td.textSize ), "spReticleLoc::getTextChildren - textSize" );
        
    p = tPlug.child( 12 , &stat );
    McheckStatus( p.getValue( td.textScale ), "spReticleLoc::getTextChildren - textScale" );
        
    p = tPlug.child( 13 , &stat );
    McheckStatus( p.getValue( td.textVAlign ), "spReticleLoc::getTextChildren - textVAlign" );

    return MS::kSuccess;
}

MStatus spReticleLoc::getTextData()
{
    // Clear the text vector
    text.clear();

    MStatus stat;
    MPlug p,tp,cp;

    p = MPlug( thisNode, Text );
    int numPlugs = p.numElements();

    for (int i = 0; i < numPlugs && i < 10; i++ )
    {
        TextData td;

        tp = p.elementByPhysicalIndex( i, &stat );
        McheckStatus( stat, "spReticleLoc::getTextData - cannot get index" );

        stat = getTextChildren( tp, td );
        McheckStatus( stat, "spReticleLoc::getTextData - cannot get children" );

        //printText( td );

        text.push_back( td );
    }

    return MS::kSuccess;
}

// This method retrieves all of the options settings.
//
MStatus spReticleLoc::getOptions()
{
    MPlug p;
    MStatus stat;

    // Check to see if drawing is enabled
    p = MPlug ( thisNode, DrawingEnabled );
    McheckStatus ( p.getValue ( options.drawingEnabled ), "spReticleLoc::getOptions drawingEnabled" );

    if (options.drawingEnabled)
    {
        // Check to see if the text should be rendered;
        p = MPlug ( thisNode, EnableTextDrawing );
        McheckStatus ( p.getValue( options.enableTextDrawing), "spReticleLoc::getOptions enableTextDrawing" );

        // Get the camera filter mode
        p = MPlug ( thisNode, CameraFilterMode );
        McheckStatus ( p.getValue ( options.cameraFilterMode  ), "spReticleLoc::getOptions cameraFilterMode");

        // Display horizontal line option;
        p = MPlug ( thisNode, DisplayLineH );
        McheckStatus ( p.getValue ( options.displayLineH  ), "spReticleLoc::getOptions displayLineH");

        // Display vertical line option;
        p = MPlug ( thisNode, DisplayLineV );
        McheckStatus ( p.getValue ( options.displayLineV  ), "spReticleLoc::getOptions displayLineV");

        // Display horizontal thirds option;
        p = MPlug ( thisNode, DisplayThirdsH );
        McheckStatus ( p.getValue ( options.displayThirdsH  ), "spReticleLoc::getOptions displayThirdsH");

        // Display vertical thirds option;
        p = MPlug ( thisNode, DisplayThirdsV );
        McheckStatus ( p.getValue ( options.displayThirdsV  ), "spReticleLoc::getOptions displayThirdsV");

        // Display crosshair option;
        p = MPlug ( thisNode, DisplayCrosshair );
        McheckStatus ( p.getValue ( options.displayCrosshair  ), "spReticleLoc::getOptions displayCrosshair");
        
        // Display crosshair option;
        p = MPlug ( thisNode, DisplayFieldGuide );
        McheckStatus ( p.getValue ( options.displayFieldGuide  ), "spReticleLoc::getOptions displayFieldGuide");
        
        // Text Color;
        stat = getColor (MiscTextColor, MiscTextTrans, options.textColor );
        McheckStatus ( stat, "spReticleLoc::getOptions textColor");

        // Line Color;
        stat = getColor ( LineColor, LineTrans, options.lineColor );
        McheckStatus ( stat, "spReticleLoc::getOptions lineColor");

        // Get whether to drive a camera or not;
        p = MPlug ( thisNode, DriveCameraAperture );
        McheckStatus ( p.getValue ( options.driveCameraAperture  ), "spReticleLoc::getOptions driveCameraAperture");

        // Get the maximum distance attribute;
        p = MPlug ( thisNode, MaximumDistance );
        McheckStatus ( p.getValue ( options.maximumDistance  ), "spReticleLoc::getOptions maximumDistance");

        // Get whether to respect overscan or not;
        p = MPlug ( thisNode, UseOverscan );
        McheckStatus ( p.getValue ( options.useOverscan  ), "spReticleLoc::getOptions useOverscan");
    }

    // Print the options to cerr
    //printOptions();

    return MS::kSuccess;
}

// This method calculates the geometry of the current window. This data is
// stored in a instance of the Geom class.
//
void spReticleLoc::calcPortGeom()
{
    portGeom.x1 = 0;
    portGeom.x2 = portWidth;
    portGeom.y1 = 0;
    portGeom.y2 = portHeight;

    portGeom.x = portWidth / 2;
    portGeom.y = portHeight / 2;
    portGeom.isValid = true;
}

// This method calculates the geometry for the filmback. There are multiple
// Geom instances that are used for displaying all of the filmback options.
// This method takes the filmback data and calculates the appropriate settings
// for the filmback Geom instances.
//
void spReticleLoc::calcFilmbackGeom()
{
    filmback = oFilmback;

    // See if we are using the camera's filmback data
    bool useCameraFilmback = filmback.horizontalFilmAperture < 0;

    // If the horizontalFilmAperture equals -1, then use the cameras settings
    if (useCameraFilmback)
    {
        filmback.horizontalFilmAperture = camera.horizontalFilmAperture();
        filmback.verticalFilmAperture = camera.verticalFilmAperture();
    }
    // If drive camera aperture is on, then set camera values
    else if (options.driveCameraAperture)
    {
        if (fabs(camera.horizontalFilmAperture() - filmback.horizontalFilmAperture) > EPSILON)
            McheckVoid( camera.setHorizontalFilmAperture(filmback.horizontalFilmAperture), "spReticleLoc::calcFilmbackGeom - setting "+camera.name()+".horizontalFilmAperture");
        if (fabs(camera.verticalFilmAperture() - filmback.verticalFilmAperture) > EPSILON)
            McheckVoid( camera.setVerticalFilmAperture(filmback.verticalFilmAperture), "spReticleLoc::calcFilmbackGeom - setting "+camera.name()+".verticalFilmAperture");
    }

    // Calculate the aspect ratio of the cameras filmback
    double cameraAspectRatio = camera.horizontalFilmAperture() / camera.verticalFilmAperture();

    // Calculate the aspect ratio of the viewport and determine if it has a horizontal or vertical orientation
    double portAspectRatio = portWidth/portHeight;
    bool portHoriz = portAspectRatio > cameraAspectRatio;

    MFnCamera::FilmFit filmFit = camera.filmFit();
    if (filmFit == MFnCamera::kFillFilmFit)
    {
        if (portHoriz)
            filmFit = MFnCamera::kHorizontalFilmFit;
        else
            filmFit = MFnCamera::kVerticalFilmFit;
    }
    else if (filmFit == MFnCamera::kOverscanFilmFit)
    {
        if (portHoriz)
            filmFit = MFnCamera::kVerticalFilmFit;
        else
            filmFit = MFnCamera::kHorizontalFilmFit;
    }

    double panX = 0.0;
    double panY = 0.0;
    double zoom = 1.0;

#if MAYA_API_VERSION >= 201100
    if (camera.panZoomEnabled() && !camera.renderPanZoom())
    {
        zoom = camera.zoom();
        panX = camera.horizontalPan();
        panY = camera.verticalPan();
    }
#endif

    double pixelScale = 1.0;

    // Calculate the pixel scale value to use when drawing the filmback
    switch ( filmFit )
    {
        case MFnCamera::kInvalid :
        case MFnCamera::kHorizontalFilmFit :
        {
            pixelScale = portWidth / overscan / camera.horizontalFilmAperture() / zoom;
            portGeom.x -= (panX * pixelScale);
            portGeom.y -= (panY * pixelScale);
            break;
        }
        case MFnCamera::kVerticalFilmFit :
        {
            pixelScale = portHeight / overscan / camera.verticalFilmAperture() / zoom;
            portGeom.x -= (panX * pixelScale);
            portGeom.y -= (panY * pixelScale);
            break;
        }
        default:
        {
            MGlobal::displayError( name() + " invalid camera film fit (" + filmFit + ")");
            break;
        }
    }

    // If the reticle is in relativeFilmback mode, then scale the reticle filmback to fit the camera filmback
    if (filmback.relativeFilmback)
    {
        double aspectRatio = filmback.horizontalFilmAperture / filmback.verticalFilmAperture;
        if (aspectRatio > cameraAspectRatio)
            pixelScale *= camera.horizontalFilmAperture() / filmback.horizontalFilmAperture;
        else
            pixelScale *= camera.verticalFilmAperture() / filmback.verticalFilmAperture;
    }

    // Account for lens squeeze for the filmback
    double pixelScaleX = pixelScale * camera.lensSqueezeRatio();

    // Calculate the filmback width and height
    filmback.filmbackGeom.x = filmback.horizontalFilmAperture * pixelScaleX;
    filmback.filmbackGeom.y = filmback.verticalFilmAperture * pixelScale;
    
    // Calculate the actual filmback geometry corner values    
    filmback.filmbackGeom.x1 = portGeom.x - (filmback.filmbackGeom.x / 2);
    filmback.filmbackGeom.x2 = portGeom.x + (filmback.filmbackGeom.x / 2);
    filmback.filmbackGeom.y1 = portGeom.y - (filmback.filmbackGeom.y / 2);
    filmback.filmbackGeom.y2 = portGeom.y + (filmback.filmbackGeom.y / 2);
    filmback.filmbackGeom.isValid = true;

    // Set the image area to match the filmback    
    filmback.imageGeom = filmback.filmbackGeom;
    filmback.horizontalImageAperture	= filmback.horizontalFilmAperture;
    filmback.verticalImageAperture		= filmback.verticalFilmAperture;

    // Calculate the pad area width and height
    if (pad.usePad && pad.isPadded)
    {
        pad.padGeom.x = (filmback.horizontalFilmAperture - pad.padAmountX) * pixelScaleX;
        pad.padGeom.y = (filmback.verticalFilmAperture - pad.padAmountY) * pixelScale;

        // Calculate rest of the pad area
        pad.padGeom.x1 = portGeom.x - (pad.padGeom.x / 2);
        pad.padGeom.x2 = portGeom.x + (pad.padGeom.x / 2);
        pad.padGeom.y1 = portGeom.y - (pad.padGeom.y / 2);
        pad.padGeom.y2 = portGeom.y + (pad.padGeom.y / 2);
        pad.padGeom.isValid = true;

        //Update the image area
        filmback.horizontalImageAperture -= pad.padAmountX;
        filmback.verticalImageAperture -= pad.padAmountY;
    }

    // Adjust for sound track if necessary
    double imageOffsetX = 0;
    if (filmback.soundTrackWidth > EPSILON)
    {
        filmback.horizontalImageAperture -= filmback.soundTrackWidth;
        imageOffsetX = (filmback.soundTrackWidth * pixelScaleX) / 2.0;
    }

    // Calculate the image area width & height
    filmback.imageGeom.x = filmback.horizontalImageAperture * pixelScaleX;
    filmback.imageGeom.y = filmback.verticalImageAperture * pixelScale;
    
    // Calculate the actual filmback geometry corner values    
    filmback.imageGeom.x1 = (portGeom.x + imageOffsetX) - (filmback.imageGeom.x / 2.0f);
    filmback.imageGeom.x2 = (portGeom.x + imageOffsetX) + (filmback.imageGeom.x / 2.0f);
    filmback.imageGeom.y1 = portGeom.y - (filmback.imageGeom.y / 2.0f);
    filmback.imageGeom.y2 = portGeom.y + (filmback.imageGeom.y / 2.0f);
    filmback.imageGeom.isValid = true;

    // reset safe action/title    
    filmback.safeActionGeom.isValid = false;
    filmback.safeTitleGeom.isValid = false;
}

// This method calculates the safe action area for the filmback.
//
void spReticleLoc::calcFilmbackSafeActionGeom()
{
    calcMaskGeom(filmback.safeActionGeom,filmback.horizontalSafeAction,filmback.verticalSafeAction,filmback.imageGeom,filmback.horizontalImageAperture,filmback.verticalImageAperture);
    filmback.safeActionGeom.isValid = true;
}

// This method calculates the safe title area for the filmback.
//
void spReticleLoc::calcFilmbackSafeTitleGeom(){
    calcMaskGeom(filmback.safeTitleGeom,filmback.horizontalSafeTitle,filmback.verticalSafeTitle,filmback.imageGeom,filmback.horizontalImageAperture,filmback.verticalImageAperture);
    filmback.safeTitleGeom.isValid = true;
}

// This method calculates the actual mask x,y values for a given Geom
// instance.
//
void spReticleLoc::calcMaskGeom(Geom & g, double w, double h, Geom & gSrc,
                                 double wSrc, double hSrc)
{
    double pw = (w >= 0)?((wSrc-w)/2.0)/wSrc:(1.0-wSrc)/2.0;
    double ph = (h >= 0)?((hSrc-h)/2.0)/hSrc:(1.0-hSrc)/2.0;

    g.x = gSrc.x*pw*camera.lensSqueezeRatio();
    g.y = gSrc.y*ph;

    g.x1 = gSrc.x1+g.x;
    g.x2 = gSrc.x2-g.x;
    g.y1 = gSrc.y1+g.y;
    g.y2 = gSrc.y2-g.y;
}

// Calculates the safe action geometry for an Aspect Ratio.
//
void spReticleLoc::calcSafeActionGeom( Aspect_Ratio & ar )
{
    if ( ar.aspectGeom.isValid )
    {
        ar.safeActionGeom = ar.aspectGeom;
        calcMaskGeom(ar.safeActionGeom, -1, -1, ar.aspectGeom, 0.9, 0.9);
        ar.safeActionGeom.isValid = true;
    }
    else
    {
        ar.safeActionGeom.isValid = false;
    }
}

// Calculates the safe title geometry for an Aspect Ratio.
//
void spReticleLoc::calcSafeTitleGeom( Aspect_Ratio & ar )
{
    if (ar.aspectGeom.isValid)
    {
        ar.safeTitleGeom = ar.aspectGeom;
        calcMaskGeom(ar.safeTitleGeom, -1, -1, ar.aspectGeom, 0.8, 0.8);
        ar.safeTitleGeom.isValid = true;
    }
    else
    {
        ar.safeTitleGeom.isValid = false;
    }
}

// This calculates the various Geom instances for a particular aspectRatio
// instance.
//
void spReticleLoc::calcAspectGeom( Aspect_Ratio & ar )
{
    ar.aspectGeom.x = filmback.imageGeom.x / camera.lensSqueezeRatio();
    ar.aspectGeom.y = ar.aspectGeom.x / ar.aspectRatio;

    ar.aspectGeom.x1 = filmback.imageGeom.x1;
    ar.aspectGeom.x2 = filmback.imageGeom.x2;
    ar.aspectGeom.y1 = portGeom.y - (ar.aspectGeom.y / 2);
    ar.aspectGeom.y2 = portGeom.y + (ar.aspectGeom.y / 2);

    ar.aspectGeom.isValid = true;
    ar.safeActionGeom.isValid = false;
    ar.safeTitleGeom.isValid = false;

    if (ar.displaySafeAction)
        calcSafeActionGeom(ar);

    if (ar.displaySafeTitle)
        calcSafeTitleGeom(ar);
}

// This calculates the PanScan Geom instances.
//
void spReticleLoc::calcPanScanGeom( PanScan & ps )
{
    //Calculate the aspect ratio of the filmback to later determine the fit of the pan/scan area    
    float aspectRatio = filmback.horizontalImageAperture / filmback.verticalImageAperture;
    
    //If the aspect ratio of the pan/scan area is not set, use the filmback's aspect ratio    
    if (ps.aspectRatio < 0)
        ps.aspectRatio = aspectRatio;

    //Determine the fit of the pan/scan & pan/scan area against the filmback    
    if (ps.aspectRatio > aspectRatio && ps.panScanRatio < ps.aspectRatio)
    {
        ps.aspectGeom.y = (filmback.imageGeom.x / camera.lensSqueezeRatio()) / ps.aspectRatio;
        ps.aspectGeom.x = ps.aspectGeom.y * ps.panScanRatio;
    }
    else if (ps.panScanRatio > aspectRatio)
    {
        ps.aspectGeom.x = filmback.imageGeom.x / camera.lensSqueezeRatio();
        ps.aspectGeom.y = ps.aspectGeom.x / ps.panScanRatio;
    }
    else
    {
        ps.aspectGeom.y = filmback.imageGeom.y;
        ps.aspectGeom.x = ps.aspectGeom.y * ps.panScanRatio;
    }
    
    //Adjust for lens squeeze
    ps.aspectGeom.x *= camera.lensSqueezeRatio();

    ps.aspectGeom.x1 = filmback.imageGeom.x1+ ( ((ps.panScanOffset+1)/2)*(filmback.imageGeom.x-ps.aspectGeom.x) );
    ps.aspectGeom.x2 = ps.aspectGeom.x1+ps.aspectGeom.x;
    ps.aspectGeom.y1 = portGeom.y - (ps.aspectGeom.y / 2.0f);
    ps.aspectGeom.y2 = portGeom.y + (ps.aspectGeom.y / 2.0f);

    ps.aspectGeom.isValid = true;
    ps.safeActionGeom.isValid = false;
    ps.safeTitleGeom.isValid = false;

    if (ps.displaySafeAction)
        calcSafeActionGeom( ps );

    if (ps.displaySafeTitle)
        calcSafeTitleGeom( ps );
}

bool spReticleLoc::setInternalValueInContext(const  MPlug & plug,
        const MDataHandle & dataHandle,  MDGContext & ctx)
{
    if (plug == worldInverseMatrix || plug == isTemplated)
        return false;

    needRefresh = true;

    return false;
}

bool spReticleLoc::calcDynamicText(TextData *td, const int i)
{
    char buff[255];
    
    switch (td->textType)
    {
        case 0:						//String
            break;
        case 1:						//Lens
            if (td->textStr == "")
                td->textStr = MString("%1.2f mm");
            
            sprintf(buff,td->textStr.asChar(),camera.focalLength() );
            td->textStr = MString(buff);
            break;
        case 2:						//Camera
            if (td->textStr == "")
                td->textStr = camera.name();
            else
            {
                sprintf(buff,td->textStr.asChar(),camera.name().asChar() );
                td->textStr = MString(buff);
            }
            break;
        case 3:						//Frame
        {
            MTime time;
            MPlug p = MPlug ( thisNode, Time );
            if (p.isNull() == false && p.isConnected() == false)
            {
                MString cmd = "connectAttr time1.o "+p.name();
                MGlobal::executeCommand(cmd);
            }
            
            MStatus status = p.getValue(time);
            if (!status)
            {
                status.perror("spReticleLoc::calcDynamicText get time");
                return false;
            }
            
            if (td->textStr == "")
                td->textStr = MString("%04.0f");
            
            sprintf(buff,td->textStr.asChar(),time.value() );
            td->textStr = MString(buff);
            break;
        }
        case 4:						//Aspect Ratio
        {
            int level = td->textARLevel;
            if (level < 0 || level >= numAspectRatios)
            {
                MGlobal::displayError( name() + " invalid text level (" + level + ") for text item " + i);
                return false;
            }
            
            if (td->textStr == "")
                td->textStr = MString("%1.3f");
            
            sprintf(buff,td->textStr.asChar(),ars[level].aspectRatio );
            td->textStr = MString(buff);
            break;
        }
        case 5:						//Maximum Distance
            if (options.maximumDistance <= 0)
                return false;
            
            if (td->textStr == "")
                td->textStr = MString("max. dist %1.0f");
            
            //textColor = (maximumDist >= options.maximumDistance) ? MColor(1,1,1,0) : textColor;
            sprintf(buff, td->textStr.asChar(), maximumDist );
            td->textStr = MString(buff);
            break;
        case 6:						//Projection Gate
        {
            if (!filmback.displayProjGate)
                return false;
            
            double aspectRatio = (filmback.horizontalProjectionGate/filmback.verticalProjectionGate);
            
            if (td->textStr == "")
                td->textStr = MString("%1.3f");
            
            sprintf(buff, td->textStr.asChar(), aspectRatio);
            td->textStr = MString(buff);
            break;
        }
        case 7:						//Show
        {
            if (td->textStr == "")
                td->textStr = MString("%s");
            
            sprintf(buff, td->textStr.asChar(), getenv(SHOW_ENV_VAR));
            td->textStr = MString(buff);
            break;
        }
        case 8:						//Show
        {
            if (td->textStr == "")
                td->textStr = MString("%s");
            
            sprintf(buff, td->textStr.asChar(),  getenv(SHOT_ENV_VAR));
            td->textStr = MString(buff);
            break;
        }
        case 9:						//Show/Shot
        {
            if (td->textStr == "")
                td->textStr = MString("%s/%s");
            
            sprintf(buff, td->textStr.asChar(), getenv(SHOW_ENV_VAR),getenv(SHOT_ENV_VAR));
            td->textStr = MString(buff);
            break;
        }
        case 10:						//Frame Start
        {
            if (td->textStr == "")
                td->textStr = MString("%s");
            
            sprintf(buff, td->textStr.asChar(),  getenv(FRAME_START_ENV_VAR));
            td->textStr = MString(buff);
            break;
        }
        case 11:						//Frame End
        {
            if (td->textStr == "")
                td->textStr = MString("%s");
            
            sprintf(buff, td->textStr.asChar(),  getenv(FRAME_END_ENV_VAR));
            td->textStr = MString(buff);
            break;
        }
        case 12:						//Frame Range
        {
            if (td->textStr == "")
                td->textStr = MString("%s-%s");
            
            sprintf(buff, td->textStr.asChar(), getenv(FRAME_START_ENV_VAR), getenv(FRAME_END_ENV_VAR));
            td->textStr = MString(buff);
            break;
        }
        case 13:						//User
        {
            if (td->textStr == "")
                td->textStr = MString("%s");
            
            sprintf(buff, td->textStr.asChar(), getenv("USER"));
            td->textStr = MString(buff);
            break;
        }
        case 14:						//Current File
        {
            if (td->textStr == "")
                td->textStr = MString("%s");
            
            sprintf(buff, td->textStr.asChar(), MFileIO::currentFile().asChar() );
            td->textStr = MString(buff);
            break;
        }
        case 15:						//Path
        {
            if (td->textStr == "")
                td->textStr = MString("%s");
            
            MFileObject fo;
            fo.setFullName(MFileIO::currentFile());
            sprintf(buff, td->textStr.asChar(), fo.path().asChar() );
            td->textStr = MString(buff);
            break;
        }
        case 16:						//Path
        {
            if (td->textStr == "")
                td->textStr = MString("%s");
            
            MFileObject fo;
            fo.setFullName(MFileIO::currentFile());
            sprintf(buff, td->textStr.asChar(), fo.name().asChar() );
            td->textStr = MString(buff);
            break;
        }
        case 17:						//Pan Scan Aspect Ratio
            if (td->textStr == "")
                td->textStr = MString("%1.2f");
            
            sprintf(buff, td->textStr.asChar(), panScan.panScanRatio );
            td->textStr = MString(buff);
            break;
        case 18:						//Pan Scan Offset
            if (td->textStr == "")
                td->textStr = MString("%1.2f");
            
            sprintf(buff, td->textStr.asChar(), panScan.panScanOffset );
            td->textStr = MString(buff);
            break;
        case 19:						//Safe Action
            if (td->textStr == "")
                td->textStr = "safe action";
            break;
        case 20:						//Safe Title
            if (td->textStr == "")
                td->textStr = "safe title";
            break;
        default:
            MGlobal::displayError( name() + " invalid text type for text item " + i);
            return false;
    }
    
    return true;
}

bool spReticleLoc::getTextLevelGeometry(TextData *td, Geom &g, const int i)
{
    switch (td->textLevel)
    {
        case 0:
            g = portGeom;
            break;
        case 1:
            g = pad.padGeom;
            break;
        case 2:
            {
                switch (td->textType)
                {
                    case 19:
                        if (!filmback.safeActionGeom.isValid)
                            calcFilmbackSafeActionGeom();
                        g = filmback.safeActionGeom;
                        break;
                    case 20:
                        if (!filmback.safeTitleGeom.isValid)
                            calcFilmbackSafeTitleGeom();
                        g = filmback.safeTitleGeom;
                        break;
                    default:
                        g = filmback.filmbackGeom;
                        break;
                }
            }
            break;
        case 3:
            if (!filmback.displayProjGate)
            {
                MGlobal::displayError( name() + " cannot anchor text item " + i + " to undisplayed projection mask");
                return false;
            }
            g = filmback.projGeom;
            break;
        case 4:
            {
                int level = td->textARLevel;
                if (level < 0 || level >= numAspectRatios)
                {
                    MGlobal::displayError( name() + " invalid aspect ratio level (" + td->textARLevel + ") for text item " + i);
                    return false;
                }
                
                if (!ars[level].aspectGeom.isValid)
                    return false;
                
                switch (td->textType)
                {
                    case 19:
                        if (!ars[level].safeActionGeom.isValid)
                            calcSafeActionGeom(ars[level]);
                        g = ars[level].safeActionGeom;
                        break;
                    case 20:
                        if (!ars[level].safeTitleGeom.isValid)
                            calcSafeTitleGeom(ars[level]);
                        g = ars[level].safeTitleGeom;
                        break;
                    default:
                        g = ars[level].aspectGeom;
                        break;
                }
            }
            break;
        case 5:
            {
                if (!panScan.aspectGeom.isValid)
                    calcPanScanGeom( panScan );
                
                switch (td->textType)
                {
                    case 19:
                        if (!panScan.safeActionGeom.isValid)
                            calcSafeActionGeom(panScan);
                        g = panScan.safeActionGeom;
                        break;
                    case 20:
                        if (!panScan.safeTitleGeom.isValid)
                            calcSafeTitleGeom(panScan);
                        g = panScan.safeTitleGeom;
                        break;
                    default:
                        g = panScan.aspectGeom;
                        break;
                }
            }
            break;
        default:
            MGlobal::displayError( name() + " invalid text anchor for text item " + i);
            break;
    }
    
    return true;
}

bool spReticleLoc::calcTextPosition(TextData *td, const Geom &g, double &x, double &y, const int i)
{
    switch (td->textPosRel)
    {
        case 0:						//Bottom Left
            x = g.x1;
            y = g.y1;
            break;
        case 1:						//Bottom Center
            x = (g.x1 + g.x2 ) / 2;
            y = g.y1;
            break;
        case 2:						//Bottom Right
            x = g.x2;
            y = g.y1;
            break;
        case 3:						//Middle Left
            x = g.x1;
            y = (g.y1 + g.y2 ) / 2;
            break;
        case 4:						//Center
            x = (g.x1 + g.x2 ) / 2;
            y = (g.y1 + g.y2 ) / 2;
            break;
        case 5:						//Middle Right
            x = g.x2;
            y = (g.y1 + g.y2 ) / 2;
            break;
        case 6:						//Top Left
            x = g.x1;
            y = g.y2;
            break;
        case 7:						//Top Center
            x = (g.x1 + g.x2 ) / 2;
            y = g.y2;
            break;
        case 8:						//Top Right
            x = g.x2;
            y = g.y2;
            break;
        default:
            MGlobal::displayError( name() + " invalid text relative position (" + td->textPosRel + ") for text item " + i);
            return false;
    }
    
    return true;
}

void spReticleLoc::drawCustomTextElements(GPURenderer* renderer)
{
    if (!text.size())
        return;

    TextData *td;

    // Make sure everything is ready for drawing text
    renderer->enableTextRendering();

    Geom g;
    for (int i = 0; i < (int)text.size(); i++)
    {
        td = &text[i];

        // If the text is not enabled, skip it
        if (!td->textEnabled)
            continue;

        MColor textColor = options.textColor;

        // Process dynamic text
        if (td->textType && !calcDynamicText(td, i))
            continue;

        // Determine the level geometry
        if (!getTextLevelGeometry(td, g, i))
            continue;

        // Determine the position
        double x,y;
        if (!calcTextPosition(td,g,x,y,i))
            continue;

        // Actually draw the text
        renderer->drawText( td, x, y);
    }

    // Disable text rendering
    renderer->disableTextRendering();
}

// This updates the data in order to get things ready for drawing
//
bool spReticleLoc::prepForDraw(const MObject & node, const MDagPath & path, const MDagPath & cameraPath)
{
    MStatus stat;
    MPlug p;

    // Initialize maximumDist
    maximumDist = 0;

#if SOURCE_MEL_SCRIPT
    // If this is the first time it's being draw, load the default values
    if (loadDefault)
    {
        MString tag;
        p = MPlug ( thisNode, Tag );
        McheckStatus ( p.getValue ( tag  ), "spReticleLoc::draw get tag");
        
        MString cmd = "if (exists(\"" SOURCE_MEL_METHOD "\")) "SOURCE_MEL_METHOD"(\""+path.partialPathName()+"\",\""+tag+"\")";
        MGlobal::executeCommand(cmd);
        loadDefault = false;
    }
#endif

    // Get options
    if (needRefresh)
        getOptions();

    // Drawing not enabled, return
    if (!options.drawingEnabled)
        return false;

    // Set the MFnCamera to the current camera
    camera.setObject( cameraPath );

    // If camera is orthographic, then return
    if (camera.isOrtho()) 
        return false;

    //Default value to false
    bool useReticle = false;

    //
    switch (options.cameraFilterMode)
    {
        // Draw in all cameras
        case 0:
            break;

        // Using CAMERA_ATTR Filter, check camera for CAMERA_ATTR attribute and draw if true
        case 1:
        {
            // Find the CAMERA_ATTR plug on the camera node
            MPlug useReticlePlug = camera.findPlug(CAMERA_ATTR, &stat);

            // If the plug is valid (not null) get the current value
            if (!useReticlePlug.isNull())
            {
                McheckStatus ( useReticlePlug.getValue( useReticle ), "spReticleLoc::draw get CAMERA_ATTR plug");
            }

            // If camera is not set to use reticle then return
            if (!useReticle)
            {
                return false;
            }

            break;
        }

        // Only display in connected cameras
        case 2:
        {
            MPlug cameras(thisNode, Cameras);
        
            MPlugArray cameraPlugs;

            MObject cameraObj = camera.object();
            for (unsigned i = 0; i < cameras.numElements() && !useReticle; i++)
            {
                cameras[i].connectedTo(cameraPlugs,true,false,&stat);
                if ( cameraPlugs.length() == 0)
                    continue;

                for(unsigned j = 0; j < cameraPlugs.length(); j++)
                {
                    if ( cameraPlugs[j].node() == cameraObj )
                    {
                        useReticle = true;
                        break;
                    }
                }
            }

            if ( !useReticle )
                return false;

            break;
        }
    }

    // Get the camera position
    MMatrix wm = cameraPath.inclusiveMatrix();

    // Find the largest component
    if (options.maximumDistance > 0)
    {
        // original: largest value
        //maximumDist = std::max(wm[3][0],std::max(wm[3][1],wm[3][2]));

        // largest absolute value (if negative, returns positive number)
        //maximumDist = std::max(fabs(wm[3][0]),std::max(fabs(wm[3][1]),fabs(wm[3][2])));

        // largest value (if negative, returns negative number)
        double maxDist = std::max(wm[3][0],std::max(wm[3][1],wm[3][2]));
        double minDist = std::min(wm[3][0],std::min(wm[3][1],wm[3][2]));
        maximumDist = (fabs(maxDist) > fabs(minDist)) ? maxDist : minDist;
    }

    // Get the camera's overscan value
    overscan = (options.useOverscan) ? 1.0 : camera.overscan();
    ncp = camera.nearClippingPlane() + 0.001;

    // Get the worldInverseMatrix
    wim = getMatrix("worldInverseMatrix");

    // Get the text data
    getTextData();

    // Update aspect ratios
    if (needToUpdateAspectRatios() || needRefresh)
    {
        // Get the aspect ratio data
        getAspectRatioData();
    }

    // Check to see if we need to refresh the data
    if (needRefresh)
    {
        // Get the pad attribute data
        getPadData();

        // Get the filmback attribute data
        getFilmbackData();

        // Get the projection gate attribute data
        getProjectionData();

        // Get safe title and safe action data
        getSafeActionData();
        getSafeTitleData();

        // Get pan and scan data
        getPanScanData( panScan );

        // Hold this copy
        oFilmback = filmback;

        // Reset need refresh
        needRefresh = false;
    }
	
	return true;
}

// This is the main function which draws the locator.
//
void spReticleLoc::drawBase(int width, int height, GPURenderer* renderer)
{
	portWidth = double(width);
	portHeight = double(height);
	
    // Calculate the port geometry
    calcPortGeom();

    // Peform the calculations necessary to define the filmback geometry.
    calcFilmbackGeom();

    // Calculate the projection gate geometry
    if (filmback.displayProjGate)
        calcMaskGeom(filmback.projGeom,filmback.horizontalProjectionGate,filmback.verticalProjectionGate,filmback.imageGeom,filmback.horizontalImageAperture,filmback.verticalImageAperture);

    // Calculate the safe action geometry
    if (filmback.displaySafeAction)
        calcFilmbackSafeActionGeom();

    // Calculate the safe title geometry
    if (filmback.displaySafeTitle)
        calcFilmbackSafeTitleGeom();

    // Calculate the projection gate geometry
    if (panScan.displayMode != 0)
        calcPanScanGeom( panScan );

    // Calculate the aspect ratio geometry
    for (int i = 0; i < numAspectRatios; i++ )
    {
        // Only calculate the aspect geom if the display mode != 0
        if (ars[i].displayMode != 0)
        {
            calcAspectGeom( ars[i] );
        }
    }
    
    // Set the filmback for the renderer
    renderer->setFilmback(&filmback);

    // Get everything setup for rendering
    renderer->prepareForDraw(portWidth, portHeight);
    
    // create variable to store what the first geometry object to draw aspect ratios to
    Geom aspectContainerGeom = portGeom;
    Geom filmbackGeom = filmback.filmbackGeom;

    // Draw the filmback
    if ( filmback.displayFilmGate )
    {
        // Draw filmback Mask
        if ( filmback.displayFilmGate == 3 )
        {
            renderer->drawMask(aspectContainerGeom, filmbackGeom, filmback.filmbackGeom.maskColor, 1);
            aspectContainerGeom = filmback.filmbackGeom;
        }
        
        // Draw filmback Line
        renderer->drawLines(filmback.filmbackGeom, filmback.filmbackGeom.lineColor, 1, filmback.displayFilmGate == 2);
        
        // Draw Sound Area Line
        if ( filmback.soundTrackWidth > EPSILON )
            renderer->drawLine(filmback.imageGeom.x1, filmback.imageGeom.x1, filmback.imageGeom.y1, filmback.imageGeom.y2, filmback.imageGeom.lineColor, 0 );
    }

    // Draw the padGeomMask
    if (pad.usePad && pad.isPadded)
    {
        if (pad.displayMode == 3)
            renderer->drawMask(aspectContainerGeom, pad.padGeom, pad.padGeom.maskColor, true);
        aspectContainerGeom = pad.padGeom;
    }
    
    // Draw all the aspectRatios

    // Draw the masks first
    Aspect_Ratio ar;
    for (int i = 0; i < numAspectRatios; i++ )
    {
        ar = ars[i];
        // Draw the masks as Quads
        if (ar.displayMode == 3)
        {
            Geom g = (i == 0)?aspectContainerGeom:ars[i-1].aspectGeom;
            MColor maskColor = ar.aspectGeom.maskColor;
            // Draw the mask in red if over the max distance
            if (options.maximumDistance > 0 &&
                fabs(maximumDist) >= options.maximumDistance)
            {
                if (i == 0)
                {
                    maskColor = MColor(1,0,0,maskColor.a);
                }
                else
                {
                    maskColor *= MColor(1.5,0.5,0.5,1);
                }
            }
            //drawMask(g, ar.aspectGeom, maskColor, (i == 0 && !(pad.usePad && pad.isPadded)) );
            renderer->drawMask(g, ar.aspectGeom, maskColor, i == 0);
            
            if ( ar.displaySafeAction == 3 )
            {
                float sf = (ar.displaySafeTitle == 3) ? 0.66 : 0.5;
                MColor c = MColor(maskColor.r,maskColor.g,maskColor.b,1+((maskColor.a-1) * sf));
                renderer->drawMask(ar.aspectGeom,ar.safeActionGeom,c,true);
            }
            if ( ar.displaySafeTitle == 3 )
            {
                float sf = (panScan.displaySafeAction == 3) ? 0.33 : 0.5;
                MColor c = MColor(maskColor.r,maskColor.g,maskColor.b,1+((maskColor.a-1) * sf));
                if ( ar.displaySafeAction == 3 )
                    renderer->drawMask(ar.safeActionGeom,ar.safeTitleGeom,c,true);
                else
                    renderer->drawMask(ar.aspectGeom,ar.safeTitleGeom,c,true);
            }
        }
    }

    // Draw lines
    for (int i = 0; i < numAspectRatios; i++ )
    {
        ar = ars[i];

        if (ar.displayMode != 0)
        {
            bool sides = (i == 0 || (fabs(ar.aspectGeom.x - ars[i].aspectGeom.x) > EPSILON) );
            renderer->drawLines(ar.aspectGeom, ar.aspectGeom.lineColor, sides, ar.displayMode == 2);
            // Draw safe action
            if (ar.displaySafeAction)
                renderer->drawLines( ar.safeActionGeom, ar.safeActionGeom.lineColor, true, ar.displaySafeAction == 2);

            // Draw safe title
            if (ar.displaySafeTitle)
                renderer->drawLines( ar.safeTitleGeom, ar.safeTitleGeom.lineColor, true, ar.displaySafeTitle == 2);
        }
    }

    // Draw the panScan
    if ( panScan.displayMode != 0 )
    {
        if (panScan.displayMode == 2)
        {
            renderer->drawMask(filmback.imageGeom, panScan.aspectGeom, panScan.aspectGeom.maskColor, true, false );
            
            Geom g = panScan.aspectGeom;
            
            if ( panScan.displaySafeAction == 3 )
            {
                float sf = (panScan.displaySafeTitle == 3) ? 0.66 : 0.5;
                MColor c = MColor(panScan.aspectGeom.maskColor.r,panScan.aspectGeom.maskColor.g,panScan.aspectGeom.maskColor.b,1+((panScan.aspectGeom.maskColor.a-1) * sf));
                renderer->drawMask(panScan.aspectGeom,panScan.safeActionGeom, c, true );
                g = panScan.safeActionGeom;
            }
            
            if ( panScan.displaySafeTitle == 3 )
            {
                float sf = (panScan.displaySafeAction == 3) ? 0.33 : 0.5;
                MColor c = MColor(panScan.aspectGeom.maskColor.r,panScan.aspectGeom.maskColor.g,panScan.aspectGeom.maskColor.b,1+((panScan.aspectGeom.maskColor.a-1) * sf));
                renderer->drawMask(g,panScan.safeTitleGeom, c, true );
            }
        }
        
        renderer->drawLines(panScan.aspectGeom, panScan.aspectGeom.lineColor, 1, 0);
        
        // Draw safe action
        if (panScan.displaySafeAction)
        {
            renderer->drawLines( panScan.safeActionGeom, panScan.safeActionGeom.lineColor, true, panScan.displaySafeAction == 2);
        }

        // Draw safe title
        if (panScan.displaySafeTitle)
        {
            renderer->drawLines( panScan.safeTitleGeom, panScan.safeTitleGeom.lineColor, true, panScan.displaySafeTitle == 2);
        }
    }

    // Draw the projection gate
    if ( filmback.displayProjGate )
    {
        if ( filmback.displayProjGate == 3 )
            renderer->drawMask(filmback.filmbackGeom, filmback.projGeom, filmback.projGeom.maskColor, 1);
        
        renderer->drawLines(filmback.projGeom, filmback.projGeom.lineColor, 1, filmback.displayProjGate == 2);
    }

    // Draw filmback safe action
    if (filmback.displaySafeAction)
    {
        renderer->drawLines( filmback.safeActionGeom, filmback.safeActionGeom.lineColor, 1, filmback.displaySafeAction == 2);
    }

    // Draw filmback safe title
    if (filmback.displaySafeTitle)
    {
        renderer->drawLines( filmback.safeTitleGeom, filmback.safeTitleGeom.lineColor, 1, filmback.displaySafeTitle == 2);
    }

    // Display horizontal line
    if ( options.displayLineH )
    {
        renderer->drawLine( filmback.imageGeom.x1, filmback.imageGeom.x2, portGeom.y, portGeom.y, options.lineColor, 0 );
    }

    // Display vertical line
    if ( options.displayLineV )
    {
        double cx = ( filmback.imageGeom.x1 + filmback.imageGeom.x2 ) / 2;
        renderer->drawLine( cx, cx, filmback.imageGeom.y1, filmback.imageGeom.y2, options.lineColor, 0 );
    }

    // Display Horizontal Thirds
    if ( options.displayThirdsH)
    {
        Geom g = aspectContainerGeom;
        double y1 = g.y1+ ( ( g.y2 - g.y1 ) * 0.33 );
        double y2 = g.y1+ ( ( g.y2 - g.y1 ) * 0.66 );
        renderer->drawLine( g.x1, g.x2, y1, y1, options.lineColor, 0 );
        renderer->drawLine( g.x1, g.x2, y2, y2, options.lineColor, 0 );
    }

    // Display Vertical Thirds
    if ( options.displayThirdsV)
    {
        Geom g = aspectContainerGeom;
        double x1 = g.x1+ ( ( g.x2 - g.x1 ) * 0.33 );
        double x2 = g.x1+ ( ( g.x2 - g.x1 ) * 0.66 );
        renderer->drawLine( x1, x1, g.y1, g.y2, options.lineColor, 0 );
        renderer->drawLine( x2, x2, g.y1, g.y2, options.lineColor, 0 );
    }

    // Display crosshair
    if ( options.displayCrosshair )
    {
        double cx = ( filmback.imageGeom.x1 + filmback.imageGeom.x2 ) / 2;
        renderer->drawLine( cx-25, cx-5, portGeom.y, portGeom.y, options.lineColor, 0 );
        renderer->drawLine( cx+25, cx+5, portGeom.y, portGeom.y, options.lineColor, 0 );
        renderer->drawLine( cx, cx, portGeom.y-25, portGeom.y-5, options.lineColor, 0 );
        renderer->drawLine( cx, cx, portGeom.y+25, portGeom.y+5, options.lineColor, 0 );
    }
    
    // Display Field Guide
    if ( options.displayFieldGuide)
    {
        //Calculate constants
        int numLines = FIELDGUIDE_NUM_LINES;

        double cx = ( filmback.imageGeom.x1 + filmback.imageGeom.x2 ) / 2.0;
        double cy = ( filmback.imageGeom.y1 + filmback.imageGeom.y2 ) / 2.0;

        double sx = (filmback.imageGeom.x / 2.0) / double(numLines+1);
        double sy = (filmback.imageGeom.y / 2.0) / double(numLines+1);
        
        //If the filmback is not being drawn, then draw lines for it
        if ( !filmback.displayFilmGate )
            renderer->drawLines(filmback.filmbackGeom, options.lineColor, 1, 0);

        //Draw field guide lines
        for (int i = 1; i <= numLines; i++)
        {
            //Draw horizontal lines
            double lx = sx * i;
            double lx1 = filmback.imageGeom.x1 + lx;
            double lx2 = filmback.imageGeom.x2 - lx;
            
            renderer->drawLine( lx1, lx1, filmback.imageGeom.y1, filmback.imageGeom.y2, options.lineColor, 0);
            renderer->drawLine( lx2, lx2, filmback.imageGeom.y1, filmback.imageGeom.y2, options.lineColor, 0);
            
            //Draw vertical lines
            double ly = sy * i;
            double ly1 = filmback.imageGeom.y1 + ly;
            double ly2 = filmback.imageGeom.y2 - ly;
            
            renderer->drawLine( filmback.imageGeom.x1, filmback.imageGeom.x2, ly1, ly1, options.lineColor, 0);
            renderer->drawLine( filmback.imageGeom.x1, filmback.imageGeom.x2, ly2, ly2, options.lineColor, 0);
        }

        //Draw center lines
        renderer->drawLine( cx, cx, filmback.imageGeom.y1, filmback.imageGeom.y2, options.lineColor, 0);
        renderer->drawLine( filmback.imageGeom.x1, filmback.imageGeom.x2, cy, cy, options.lineColor, 0);
        
        //Draw Diagonal lines
        renderer->drawLine( filmback.imageGeom.x1, filmback.imageGeom.x2, filmback.imageGeom.y1, filmback.imageGeom.y2, options.lineColor, 0);
        renderer->drawLine( filmback.imageGeom.x1, filmback.imageGeom.x2, filmback.imageGeom.y2, filmback.imageGeom.y1, options.lineColor, 0);

        //Draw Numbers
        TextData td;
        td.textAlign = 1;
        td.textVAlign = 1;
        td.textSize = 12;
        td.textBold = false;
        td.textColor = options.textColor;
        td.textPosX = 0;
        td.textPosY = 0;

        renderer->enableTextRendering();
        for (int i = 1; i <= numLines; i++)
        {
            double lx = sx * i;
            double lx1 = filmback.imageGeom.x1 + lx;
            double lx2 = filmback.imageGeom.x2 - lx;

            double ly = sy * i;
            double ly1 = filmback.imageGeom.y1 + ly;
            double ly2 = filmback.imageGeom.y2 - ly;

            //Draw text
            td.textStr.set((numLines - i)+1);
            renderer->drawText(&td,lx1,ly1);
            renderer->drawText(&td,lx2,ly1);
            renderer->drawText(&td,lx1,ly2);
            renderer->drawText(&td,lx2,ly2);
        }
        renderer->disableTextRendering();
    }

    // Display the pad area
    if (pad.usePad && pad.isPadded && pad.displayMode > 0)
    {
        renderer->drawLines(pad.padGeom, pad.padGeom.lineColor, 1, pad.displayMode == 2);
    }

    // Draw custom text elements
    if ( options.enableTextDrawing )
        drawCustomTextElements(renderer);

    // Clean-up after draw
    renderer->postDraw();
}

// This is the main function which draws the locator. It is called
// by Maya whenever a 3D view needs to be refreshed.
//
void spReticleLoc::draw(M3dView & view, const MDagPath & path, 
                         M3dView::DisplayStyle style,
                         M3dView::DisplayStatus status)
{
    MStatus stat;
    MPlug p;

    // Get the dagPath, node and function set for the currently displaying camera
    MDagPath cameraPath;
    view.getCamera(cameraPath);

    // Prep the data for drawing
    if (!prepForDraw(thisMObject(), path, cameraPath))
        return;
	
    // Get the view ready for drawing
    view.beginGL();

    // Perform the draw
    drawBase(view.portWidth(), view.portHeight(), &oglRenderer);
	
    // End of openGL calls
    view.endGL();
}

// This function returns whether this locator should be effected by the
// Display locators toggle in Maya. It is dependent upon the hideLocator
// option.
//
bool spReticleLoc::excludeAsLocator() const
{
    // Check to see locator should be hidden with other locators
    MObject thisNode = thisMObject();
    MPlug p(thisNode, HideLocator);
    bool hideLocator;
    MStatus stat = p.getValue(hideLocator);
    if (!stat)
        stat.perror("spReticleLoc::excludeAsLocator hideLocator");

    return hideLocator;
}

// This method is called just after a particular instance of the node has
// been created. It automatically sources the proper scripts and sets the
// loadDefault argument to true, indicating that the first time the locator
// is drawn that it should first load the default settings for the locator.
// This functionality allows either the show or facility to set standards
// on the locator.
//
void spReticleLoc::postConstructor()
{
    MStatus stat;
    MPlug p;

    // Tell Maya to draw this in the transparent queue
#if MAYA_API_VERSION < 850
#ifndef WIN32
    setTranspHandler( (TranspHandler)(&spReticleLoc::isTransparent) );
#endif
#endif

    // Load defaults
    loadDefault = SOURCE_MEL_SCRIPT;

    // Set Refresh
    needRefresh = true;

    // Initialize thisNode
    thisNode = thisMObject();

    // Create aliases for deprecated node attributes
    MFnDependencyNode fnThisNode( thisNode );

    // Create an alias for the useSpReticle attribute which has been deprecated for the cameraFilterMode attribute
    p = MPlug( thisNode, CameraFilterMode );
    fnThisNode.setAlias("useSpReticle","cameraFilterMode",p, true, &stat);
    McheckVoid ( stat, "spReticleLoc::postConstructor, unable to alias useSpReticle attribute");

    // Create an alias for the FilmGateColor and FilmGateTrans attributes
    p = MPlug( thisNode, FilmGateLineColor );
    fnThisNode.setAlias("filmGateColor","filmGateLineColor",p, true, &stat);
    McheckVoid ( stat, "spReticleLoc::postConstructor, unable to alias filmGateColor attribute");

    p = MPlug( thisNode, FilmGateLineTrans );
    fnThisNode.setAlias("filmGateTrans","filmGateLineTrans",p, true, &stat);
    McheckVoid ( stat, "spReticleLoc::postConstructor, unable to alias filmGateTrans attribute");

    // Create an alias for the ProjGateColor and ProjGateTrans attributes
    p = MPlug( thisNode, ProjGateLineColor );
    fnThisNode.setAlias("projGateColor","projGateLineColor",p, true, &stat);
    McheckVoid ( stat, "spReticleLoc::postConstructor, unable to alias projGateColor attribute");

    p = MPlug( thisNode, ProjGateLineTrans );
    fnThisNode.setAlias("projGateTrans","projGateLineTrans",p, true, &stat);
    McheckVoid ( stat, "spReticleLoc::postConstructor, unable to alias projGateTrans attribute");
}

void * spReticleLoc::creator()
{
    return new spReticleLoc();
}

MStatus spReticleLoc::initialize()
{
    MStatus              stat;
    MFnNumericAttribute  nAttr;
    MFnEnumAttribute     eAttr;
    MFnCompoundAttribute cAttr;
    MFnUnitAttribute     uAttr;
    MFnTypedAttribute    tAttr;
    MFnMessageAttribute  mAttr;

    // Define a default empty string
    MString defaultString("");
    MFnStringData defaultTextData;
    MObject defaultTextAttr = defaultTextData.create(defaultString);

    DrawingEnabled = nAttr.create( "drawingEnabled", "en", MFnNumericData::kBoolean, 1, &stat );
    McheckStatus(stat,"create drawingEnabled attribute");
    nAttr.setInternal(true);

    EnableTextDrawing = nAttr.create( "enableTextDrawing", "etd", MFnNumericData::kBoolean, 1, &stat );
    McheckStatus(stat,"create enableTextDrawing attribute");
    nAttr.setInternal(true);

    HorizontalFilmAperture = nAttr.create( "horizontalFilmAperture", "hfa", MFnNumericData::kFloat, 0.864, &stat );
    McheckStatus(stat,"create horizontalFilmAperture attribute");
    nAttr.setInternal(true);

    VerticalFilmAperture = nAttr.create( "verticalFilmAperture", "vfa", MFnNumericData::kFloat, 0.630, &stat );
    McheckStatus(stat,"create verticalFilmAperture attribute");
    nAttr.setInternal(true);

    FilmbackAperture = nAttr.create( "filmbackAperture", "cap", HorizontalFilmAperture, VerticalFilmAperture, MObject::kNullObj, &stat );
    nAttr.setDefault( 0.864, 0.630 );

    RelativeFilmback = nAttr.create( "relativeFilmback", "rfb", MFnNumericData::kBoolean, 1, &stat );
    McheckStatus(stat,"create relativeFilmback attribute");
    nAttr.setInternal(true);

    SoundTrackWidth = nAttr.create( "soundTrackWidth", "stw", MFnNumericData::kFloat, 0.0, &stat );
    McheckStatus(stat,"create soundTrackWidth attribute");
    nAttr.setInternal(true);
    
    DisplayFilmGate = eAttr.create( "displayFilmGate", "dfg", 0, &stat );
    McheckStatus(stat,"create displayFilmGate attribute");
    eAttr.addField("off", 0);
    eAttr.addField("lines", 1);
    eAttr.addField("dashed lines", 2);
    eAttr.addField("Transparent Mask", 3);
    eAttr.setInternal(true);
    
    HorizontalProjectionGate = nAttr.create( "horizontalProjectionGate", "hpg", MFnNumericData::kFloat, 0.825, &stat );
    McheckStatus(stat,"create horizontalProjectionGate attribute");
    nAttr.setInternal(true);

    VerticalProjectionGate = nAttr.create( "verticalProjectionGate", "vpg", MFnNumericData::kFloat, 0.446, &stat );
    McheckStatus(stat,"create verticalProjectionGate attribute");
    nAttr.setInternal(true);

    ProjectionGate = nAttr.create( "projectionGate", "pg", HorizontalProjectionGate, VerticalProjectionGate, MObject::kNullObj, &stat );
    nAttr.setDefault( 0.825, 0.446 );
    
    DisplayProjectionGate = eAttr.create( "displayProjGate", "dpg", 0, &stat );
    McheckStatus(stat,"create displayProjGate attribute");
    eAttr.addField("off", 0);
    eAttr.addField("lines", 1);
    eAttr.addField("dashed lines", 2);
    eAttr.addField("Transparent Mask", 3);
    eAttr.setInternal(true);
    
    HorizontalSafeAction = nAttr.create( "horizontalSafeAction", "hsa", MFnNumericData::kFloat, 0.713, &stat );
    McheckStatus(stat,"create horizontalSafeAction attribute");
    nAttr.setInternal(true);

    VerticalSafeAction = nAttr.create( "verticalSafeAction", "vsa", MFnNumericData::kFloat, 0.535, &stat );
    McheckStatus(stat,"create verticalSafeAction attribute");
    nAttr.setInternal(true);

    SafeAction = nAttr.create( "safeAction", "sa", HorizontalSafeAction, VerticalSafeAction, MObject::kNullObj, &stat );
    nAttr.setDefault( 0.713, 0.535 );

    DisplaySafeAction = eAttr.create( "displaySafeAction", "dsa", 0, &stat );
    McheckStatus(stat,"create displaySafeAction attribute");
    eAttr.addField("off", 0);
    eAttr.addField("lines", 1);
    eAttr.addField("dashed lines", 2);
    eAttr.setInternal(true);

    HorizontalSafeTitle = nAttr.create( "horizontalSafeTitle", "hst", MFnNumericData::kFloat, 0.630, &stat );
    McheckStatus(stat,"create horizontalSafeTitle attribute");
    nAttr.setInternal(true);

    VerticalSafeTitle = nAttr.create( "verticalSafeTitle", "vst", MFnNumericData::kFloat, 0.475, &stat );
    McheckStatus(stat,"create verticalSafeTitle attribute");
    nAttr.setInternal(true);

    SafeTitle = nAttr.create( "safeTitle", "st", HorizontalSafeTitle, VerticalSafeTitle, MObject::kNullObj, &stat );
    nAttr.setDefault( 0.630, 0.475 );

    DisplaySafeTitle = eAttr.create( "displaySafeTitle", "dst", 0, &stat );
    McheckStatus(stat,"create displaySafeTitle attribute");
    eAttr.addField("off", 0);
    eAttr.addField("lines", 1);
    eAttr.addField("dashed lines", 2);
    eAttr.setInternal(true);

    AspectRatio = nAttr.create( "aspectRatio", "ar", MFnNumericData::kFloat, 1.85, &stat );
    McheckStatus(stat,"create aspectRatio attribute");
    nAttr.setInternal(true);

    DisplayMode = eAttr.create( "displayMode", "dm", 0, &stat );
    McheckStatus(stat, "create displayMode attribute");
    eAttr.addField("none", 0);
    eAttr.addField("lines", 1);
    eAttr.addField("dashed lines", 2);
    eAttr.addField("Transparent Mask", 3);
    eAttr.setInternal(true);

    AspectMaskColor = nAttr.createColor( "aspectMaskColor", "amc", &stat );
    nAttr.setDefault( 0.0, 0.0, 0.0 );
    nAttr.setUsedAsColor( true );
    nAttr.setInternal(true);

    AspectMaskTrans = nAttr.create( "aspectMaskTrans", "amt", MFnNumericData::kFloat, 0.75, &stat );
    McheckStatus(stat,"create aspectMaskTrans attribute");
    nAttr.setMin(0.0);
    nAttr.setMax(1.0);
    nAttr.setInternal(true);

    AspectLineColor = nAttr.createColor( "aspectLineColor", "alc", &stat );
    nAttr.setDefault( 0.8, 0.8, 0.8 );
    nAttr.setUsedAsColor( true );
    nAttr.setInternal(true);

    AspectLineTrans = nAttr.create( "aspectLineTrans", "alt", MFnNumericData::kFloat, 0.0, &stat );
    McheckStatus(stat,"create aspectLineTrans attribute");
    nAttr.setMin(0.0);
    nAttr.setMax(1.0);
    nAttr.setInternal(true);

    AspectDisplaySafeTitle = eAttr.create( "aspectDisplaySafeTitle", "adst", 0, &stat );
    McheckStatus(stat,"create aspectDisplaySafeTitle attribute");
    eAttr.addField("off", 0);
    eAttr.addField("lines", 1);
    eAttr.addField("dashed lines", 2);
    eAttr.addField("Transparent Mask", 3);
    eAttr.setInternal(true);

    AspectDisplaySafeAction = eAttr.create( "aspectDisplaySafeAction", "adsa", 0, &stat );
    McheckStatus(stat,"create aspectDisplaySafeAction attribute");
    eAttr.addField("off", 0);
    eAttr.addField("lines", 1);
    eAttr.addField("dashed lines", 2);
    eAttr.addField("Transparent Mask", 3);
    eAttr.setInternal(true);

    AspectRatios = cAttr.create( "aspectRatios", "ars", &stat );
    McheckStatus(stat,"create aspectRatios attribute");
    cAttr.addChild( AspectRatio );
    cAttr.addChild( DisplayMode );
    cAttr.addChild( AspectMaskColor );
    cAttr.addChild( AspectMaskTrans );
    cAttr.addChild( AspectLineColor );
    cAttr.addChild( AspectLineTrans );
    cAttr.addChild( AspectDisplaySafeAction );
    cAttr.addChild( AspectDisplaySafeTitle );
    cAttr.setArray( true );
    cAttr.setIndexMatters( true );
    cAttr.setStorable(true);

    PanScanAspectRatio = nAttr.create( "panScanAspectRatio", "psar", MFnNumericData::kFloat, -1, &stat );
    McheckStatus(stat,"create panScanAspectRatio attribute");
    nAttr.setInternal(true);

    PanScanDisplayMode = eAttr.create( "panScanDisplayMode", "psdm", 0, &stat );
    McheckStatus(stat, "create panScanDisplayMode attribute");
    eAttr.addField("none", 0);
    eAttr.addField("lines", 1);
    eAttr.addField("Transparent Mask", 2);
    eAttr.setInternal(true);

    PanScanDisplaySafeTitle = eAttr.create( "panScanDisplaySafeTitle", "psdst", 0, &stat );
    McheckStatus(stat,"create panScanDisplaySafeTitle attribute");
    eAttr.addField("off", 0);
    eAttr.addField("lines", 1);
    eAttr.addField("dashed lines", 2);
    eAttr.addField("Transparent Mask", 3);
    eAttr.setInternal(true);

    PanScanDisplaySafeAction = eAttr.create( "panScanDisplaySafeAction", "psdsa", 0, &stat );
    McheckStatus(stat,"create panScanDisplaySafeAction attribute");
    eAttr.addField("off", 0);
    eAttr.addField("lines", 1);
    eAttr.addField("dashed lines", 2);
    eAttr.addField("Transparent Mask", 3);
    eAttr.setInternal(true);

    PanScanMaskColor = nAttr.createColor( "panScanMaskColor", "psmc", &stat );
    nAttr.setDefault( 1.0, 1.0, 0.8 );
    nAttr.setUsedAsColor( true );
    nAttr.setInternal(true);

    PanScanMaskTrans = nAttr.create( "panScanMaskTrans", "psmt", MFnNumericData::kFloat, 0.85, &stat );
    McheckStatus(stat,"create panScanMaskTrans attribute");
    nAttr.setMin(0.0);
    nAttr.setMax(1.0);
    nAttr.setInternal(true);

    PanScanLineColor = nAttr.createColor( "panScanLineColor", "pslc", &stat );
    nAttr.setDefault( 1.0, 1.0, 0.0 );
    nAttr.setUsedAsColor( true );
    nAttr.setInternal(true);

    PanScanLineTrans = nAttr.create( "panScanLineTrans", "pslt", MFnNumericData::kFloat, 0.5, &stat );
    McheckStatus(stat,"create panScanLineTrans attribute");
    nAttr.setMin(0.0);
    nAttr.setMax(1.0);
    nAttr.setInternal(true);

    PanScanRatio = nAttr.create( "panScanRatio", "psr", MFnNumericData::kFloat, 1.33, &stat );
    McheckStatus(stat, "create panScanRatio attribute");
    nAttr.setKeyable(true);
    nAttr.setMin(0);
    nAttr.setInternal(true);

    PanScanOffset = nAttr.create( "panScanOffset", "pso", MFnNumericData::kFloat, 0.0, &stat );
    McheckStatus(stat, "create panScanOffset attribute");
    nAttr.setKeyable(true);
    nAttr.setMin(-1.0);
    nAttr.setMax(1.0);
    nAttr.setInternal(true);

    PanScanAttr = cAttr.create( "panScan", "ps", &stat );
    McheckStatus(stat,"create panScan attribute");
    cAttr.addChild( PanScanAspectRatio );
    cAttr.addChild( PanScanDisplayMode );
    cAttr.addChild( PanScanMaskColor );
    cAttr.addChild( PanScanMaskTrans );
    cAttr.addChild( PanScanLineColor );
    cAttr.addChild( PanScanLineTrans );
    cAttr.addChild( PanScanDisplaySafeAction );
    cAttr.addChild( PanScanDisplaySafeTitle );
    cAttr.addChild( PanScanRatio );
    cAttr.addChild( PanScanOffset );

    TextType = eAttr.create( "textType", "ttyp", 0, &stat );
    McheckStatus(stat, "create textType attribute");
    eAttr.addField("String", 0);
    eAttr.addField("Lens", 1);
    eAttr.addField("Camera", 2);
    eAttr.addField("Frame", 3);
    eAttr.addField("Aspect Ratio", 4);
    eAttr.addField("Max. Distance", 5);
    eAttr.addField("Proj. Gate", 6);
    eAttr.addField("Show", 7);
    eAttr.addField("Shot", 8);
    eAttr.addField("Show/Shot",9);
    eAttr.addField("Frame Start", 10);
    eAttr.addField("Frame End", 11);
    eAttr.addField("Frame Range",12);
    eAttr.addField("User",13);
    eAttr.addField("Current File",14);
    eAttr.addField("File Path",15);
    eAttr.addField("File Name",16);
    eAttr.addField("Pan/Scan Aspect Ratio",17);
    eAttr.addField("Pan/Scan Offset",18);
    eAttr.addField("safe action",19);
    eAttr.addField("safe title",20);
    eAttr.setInternal(true);

    TextStr = tAttr.create( "textStr", "tstr", MFnStringData::kString );
    McheckStatus(stat,"create textStr attribute");
    tAttr.setDefault(defaultTextAttr);
    tAttr.setInternal(true);

    TextAlign = eAttr.create( "textAlign", "ta", 0, &stat );
    McheckStatus(stat, "create textAlign attribute");
    eAttr.addField("Left", 0);
    eAttr.addField("Center", 1);
    eAttr.addField("Right", 2);
    eAttr.setInternal(true);

    TextPosX = nAttr.create( "textPosX", "tpx", MFnNumericData::kFloat, 0.0, &stat );
    McheckStatus(stat, "create textPosX attribute");
    nAttr.setInternal(true);

    TextPosY = nAttr.create( "textPosY", "tpy", MFnNumericData::kFloat, 0.0, &stat );
    McheckStatus(stat, "create textPosX attribute");
    nAttr.setInternal(true);

    TextPos = nAttr.create( "textPos", "tp", TextPosX, TextPosY, MObject::kNullObj, &stat );
    McheckStatus(stat, "create textPos attribute");
    nAttr.setDefault( 0.0, 0.0 );

    TextPosRel = eAttr.create( "textPosRel", "tpr", 0, &stat );
    McheckStatus(stat, "create textPosRel attribute");
    eAttr.addField("Bottom Left", 0);
    eAttr.addField("Bottom Center", 1);
    eAttr.addField("Bottom Right", 2);
    eAttr.addField("Middle Left", 3);
    eAttr.addField("Center", 4);
    eAttr.addField("Middle Right", 5);
    eAttr.addField("Top Left", 6);
    eAttr.addField("Top Center", 7);
    eAttr.addField("Top Right", 8);
    eAttr.setInternal(true);

    TextLevel = eAttr.create( "textLevel", "tl", 0, &stat );
    McheckStatus(stat, "create textLevel attribute");
    eAttr.addField("Window", 0);
    eAttr.addField("Pad/Filmback", 1);
    eAttr.addField("Filmback", 2);
    eAttr.addField("Proj. Mask", 3);
    eAttr.addField("Aspect Ratio", 4);
    eAttr.addField("Pan/Scan",5);
    eAttr.setInternal(true);

    TextARLevel = nAttr.create( "textARLevel", "tarl", MFnNumericData::kInt, 0, &stat );
    McheckStatus(stat, "create textARLevel attribute");
    nAttr.setInternal(true);

    TextColor = nAttr.createColor( "textColor", "tc", &stat );
    McheckStatus(stat,"create textColor attribute");
    nAttr.setDefault( 1.0, 1.0, 1.0 );
    nAttr.setUsedAsColor( true );
    nAttr.setInternal(true);

    TextTrans = nAttr.create( "textTrans", "tt", MFnNumericData::kFloat, 0.5, &stat );
    McheckStatus(stat,"create textTrans attribute");
    nAttr.setMin(0.0);
    nAttr.setMax(1.0);
    nAttr.setInternal(true);

    TextEnabled = nAttr.create( "textEnabled", "ten", MFnNumericData::kBoolean, true, &stat );
    McheckStatus( stat, "create textEnabled attribute");

    TextBold = nAttr.create( "textBold", "tbld", MFnNumericData::kBoolean, false, &stat );
    McheckStatus(stat, "create textBold attribute");
    nAttr.setInternal(true);

    TextSize = nAttr.create( "textSize", "tsiz", MFnNumericData::kInt, 12, &stat );
    nAttr.setMin(MINFONT);
    nAttr.setSoftMax(MAXFONT);
    nAttr.setMax(MAXFONT*2);
    McheckStatus(stat, "create textSize attribute");
    nAttr.setInternal(true);

    TextScale = nAttr.create( "textScale", "tscl", MFnNumericData::kBoolean, false, &stat );
    McheckStatus(stat, "create textScale attribute");
    nAttr.setInternal(true);

    TextVAlign = eAttr.create( "textVAlign", "tva", 0, &stat );
    McheckStatus(stat, "create textVAlign attribute");
    eAttr.addField("Bottom", 0);
    eAttr.addField("Middle", 1);
    eAttr.addField("Top", 2);
    eAttr.setInternal(true);

    Text = cAttr.create( "text", "txt", &stat );
    McheckStatus(stat,"create text attribute");
    cAttr.addChild( TextType );
    cAttr.addChild( TextStr );
    cAttr.addChild( TextAlign );
    cAttr.addChild( TextPos );
    cAttr.addChild( TextPosRel );
    cAttr.addChild( TextLevel );
    cAttr.addChild( TextARLevel );
    cAttr.addChild( TextColor );
    cAttr.addChild( TextTrans );
    cAttr.addChild( TextEnabled );
    cAttr.addChild( TextBold );
    cAttr.addChild( TextSize );
    cAttr.addChild( TextScale );
    cAttr.addChild( TextVAlign );
    cAttr.setArray( true );
    cAttr.setIndexMatters( true );

    MiscTextColor = nAttr.createColor( "miscTextColor", "mtc", &stat );
    McheckStatus(stat,"create miscTextColor attribute");
    nAttr.setDefault( 1.0, 1.0, 1.0 );
    nAttr.setUsedAsColor( true );
    nAttr.setInternal(true);

    MiscTextTrans = nAttr.create( "miscTextTrans", "mtt", MFnNumericData::kFloat, 0.5, &stat );
    McheckStatus(stat,"create miscTextTrans attribute");
    nAttr.setMin(0.0);
    nAttr.setMax(1.0);
    nAttr.setInternal(true);

    LineColor = nAttr.createColor( "lineColor", "lc", &stat );
    nAttr.setDefault( 1.0, 1.0, 0.0 );
    nAttr.setUsedAsColor( true );
    nAttr.setInternal(true);

    LineTrans = nAttr.create( "lineTrans", "lt", MFnNumericData::kFloat, 0.5, &stat );
    McheckStatus(stat,"create lineTrans attribute");
    nAttr.setMin(0.0);
    nAttr.setMax(1.0);
    nAttr.setInternal(true);

    FilmGateMaskColor = nAttr.createColor( "filmGateMaskColor", "fgmc", &stat );
    McheckStatus(stat,"create filmGateMaskColor attribute");
    nAttr.setDefault( 0.0, 0.0, 0.0 );
    nAttr.setUsedAsColor( true );
    nAttr.setInternal(true);

    FilmGateMaskTrans = nAttr.create( "filmGateMaskTrans", "fgmt", MFnNumericData::kFloat, 0.25, &stat );
    McheckStatus(stat,"create filmGateMaskTrans attribute");
    nAttr.setMin(0.0);
    nAttr.setMax(1.0);
    nAttr.setInternal(true);

    FilmGateLineColor = nAttr.createColor( "filmGateLineColor", "fglc", &stat );
    McheckStatus(stat,"create filmGateLineColor attribute");
    nAttr.setDefault( 1.0, 1.0, 1.0 );
    nAttr.setUsedAsColor( true );
    nAttr.setInternal(true);

    FilmGateLineTrans = nAttr.create( "filmGateLineTrans", "fglt", MFnNumericData::kFloat, 0.25, &stat );
    McheckStatus(stat,"create filmGateLineTrans attribute");
    nAttr.setMin(0.0);
    nAttr.setMax(1.0);
    nAttr.setInternal(true);

    ProjGateMaskColor = nAttr.createColor( "projGateMaskColor", "pgmc", &stat );
    McheckStatus(stat,"create projGateMaskColor attribute");
    nAttr.setDefault( 1.0, 1.0, 1.0 );
    nAttr.setUsedAsColor( true );
    nAttr.setInternal(true);

    ProjGateMaskTrans = nAttr.create( "projGateMaskTrans", "pgmt", MFnNumericData::kFloat, 0.75, &stat );
    McheckStatus(stat,"create projGateMaskTrans attribute");
    nAttr.setMin(0.0);
    nAttr.setMax(1.0);
    nAttr.setInternal(true);

    ProjGateLineColor = nAttr.createColor( "projGateLineColor", "pglc", &stat );
    McheckStatus(stat,"create projGateLineColor attribute");
    nAttr.setDefault( 1.0, 1.0, 1.0 );
    nAttr.setUsedAsColor( true );
    nAttr.setInternal(true);

    ProjGateLineTrans = nAttr.create( "projGateLineTrans", "pglt", MFnNumericData::kFloat, 0.25, &stat );
    McheckStatus(stat,"create projGateLineTrans attribute");
    nAttr.setMin(0.0);
    nAttr.setMax(1.0);
    nAttr.setInternal(true);

#if MAYA_API_VERSION < 201100
    HideLocator = nAttr.create( "hideLocator", "hl", MFnNumericData::kBoolean, false, &stat );
#else
    HideLocator = nAttr.create( "hideLocator", "hloc", MFnNumericData::kBoolean, false, &stat );
#endif
    McheckStatus(stat,"create hideLocator attribute");
    nAttr.setInternal(true);

    CameraFilterMode = eAttr.create( "cameraFilterMode", "cfm", 0, &stat );
    McheckStatus(stat, "create cameraFilterMode attribute");
    eAttr.addField("All Cameras", 0);
    eAttr.addField("Cameras with " CAMERA_ATTR " attribute", 1);
    eAttr.addField("Connected Cameras", 2);
    eAttr.setInternal(true);

    Cameras = mAttr.create( "cameras", "cams", &stat );
    McheckStatus(stat, "create cameras attribute");
    mAttr.setArray( true );
    mAttr.setIndexMatters( false );

    DisplayLineH = nAttr.create( "displayLineH", "dlh", MFnNumericData::kBoolean, false, &stat );
    McheckStatus(stat,"create displayLineH attribute");
    nAttr.setInternal(true);

    DisplayLineV = nAttr.create( "displayLineV", "dlv", MFnNumericData::kBoolean, false, &stat );
    McheckStatus(stat,"create displayLineV attribute");
    nAttr.setInternal(true);

    DisplayThirdsH = nAttr.create( "displayThirdsH", "dth", MFnNumericData::kBoolean, false, &stat );
    McheckStatus(stat,"create displayThirdsH attribute");
    nAttr.setInternal(true);

    DisplayThirdsV = nAttr.create( "displayThirdsV", "dtv", MFnNumericData::kBoolean, false, &stat );
    McheckStatus(stat,"create displayThirdsV attribute");
    nAttr.setInternal(true);

    DisplayCrosshair = nAttr.create( "displayCrosshair", "dlc", MFnNumericData::kBoolean, false, &stat );
    McheckStatus(stat,"create displayCrosshair attribute");
    nAttr.setInternal(true);

    DisplayFieldGuide = nAttr.create( "displayFieldGuide", "dfgd", MFnNumericData::kBoolean, false, &stat );
    McheckStatus(stat,"create displayFieldGuide attribute");
    nAttr.setInternal(true);

    Time = uAttr.create( "time", "tm", MFnUnitAttribute::kTime, 0.0, &stat );
    McheckStatus(stat,"create time attribute");

    DriveCameraAperture = nAttr.create( "driveCameraAperture", "dca", MFnNumericData::kBoolean, false, &stat );
    McheckStatus(stat,"create driveCameraAperture attribute");
    nAttr.setInternal(true);

    MaximumDistance = nAttr.create( "maximumDistance", "mad", MFnNumericData::kFloat, 10000, &stat );
    McheckStatus(stat,"create maximumDistance attribute");
    nAttr.setInternal(true);

    UseOverscan = nAttr.create( "useOverscan", "uo", MFnNumericData::kBoolean, false, &stat );
    McheckStatus(stat,"create useOverscan attribute");
    nAttr.setInternal(true);

    UsePad = nAttr.create( "usePad", "up", MFnNumericData::kBoolean, false, &stat );
    McheckStatus(stat,"create usePad attribute");
    nAttr.setInternal(true);

    PadAmountX = nAttr.create( "padAmountX", "pax", MFnNumericData::kFloat, 0.0, &stat );
    McheckStatus(stat,"create padAmountX attribute");
    nAttr.setInternal(true);

    PadAmountY = nAttr.create( "padAmountY", "pay", MFnNumericData::kFloat, 0.0, &stat );
    McheckStatus(stat,"create padAmountY attribute");
    nAttr.setInternal(true);

    PadAmount = nAttr.create( "padAmount", "paa", PadAmountX, PadAmountY, MObject::kNullObj, &stat );
    nAttr.setDefault( 0.0, 0.0 );

    PadDisplayMode = eAttr.create( "padDisplayMode", "pdm", 3, &stat );
    McheckStatus(stat, "create panScanDisplayMode attribute");
    eAttr.addField("none", 0);
    eAttr.addField("lines", 1);
    eAttr.addField("dashed lines", 2);
    eAttr.addField("Transparent Mask", 3);
    eAttr.setInternal(true);

    PadMaskColor = nAttr.createColor( "padMaskColor", "pmc", &stat );
    McheckStatus(stat,"create padMaskColor attribute");
    nAttr.setDefault( 0.0, 0.0, 0.0 );
    nAttr.setUsedAsColor( true );
    nAttr.setInternal(true);

    PadMaskTrans = nAttr.create( "padMaskTrans", "pmt", MFnNumericData::kFloat, 0.25, &stat );
    McheckStatus(stat,"create padMaskTrans attribute");
    nAttr.setMin(0.0);
    nAttr.setMax(1.0);
    nAttr.setInternal(true);

    PadLineColor = nAttr.createColor( "padLineColor", "plc", &stat );
    McheckStatus(stat,"create padLineColor attribute");
    nAttr.setDefault( 1.0, 0.0, 0.0 );
    nAttr.setUsedAsColor( true );
    nAttr.setInternal(true);

    PadLineTrans = nAttr.create( "padLineTrans", "plt", MFnNumericData::kFloat, 0.0, &stat );
    McheckStatus(stat,"create padLineTrans attribute");
    nAttr.setMin(0.0);
    nAttr.setMax(1.0);
    nAttr.setInternal(true);

    Pad = cAttr.create( "pad", "pad", &stat );
    McheckStatus(stat,"create pad attribute");
    cAttr.addChild( UsePad );
    cAttr.addChild( PadAmount );
    cAttr.addChild( PadDisplayMode );
    cAttr.addChild( PadMaskColor );
    cAttr.addChild( PadMaskTrans );
    cAttr.addChild( PadLineColor );
    cAttr.addChild( PadLineTrans );

    Tag = tAttr.create( "type", "typ", MFnStringData::kString );
    McheckStatus(stat,"create tag attribute");
    tAttr.setHidden(true);
    tAttr.setConnectable(false);
    tAttr.setDefault(defaultTextAttr);
    
    stat = addAttribute(EnableTextDrawing);
        McheckStatus(stat,"addAttribute enableTextDrawing");
    stat = addAttribute (DrawingEnabled);
        McheckStatus(stat,"addAttribute drawingEnabled");
    stat = addAttribute (FilmbackAperture);
        McheckStatus(stat,"addAttribute filmbackAperture");
    stat = addAttribute (RelativeFilmback);
        McheckStatus(stat,"addAttribute relativeFilmback");
    stat = addAttribute (SoundTrackWidth);
        McheckStatus(stat,"addAttribute soundTrackWidth");
    stat = addAttribute (DisplayFilmGate);
        McheckStatus(stat,"addAttribute displayFilmGate");
    stat = addAttribute (ProjectionGate);
        McheckStatus(stat,"addAttribute projectionGate");
    stat = addAttribute (DisplayProjectionGate);
        McheckStatus(stat,"addAttribute displayProjectionGate");
    stat = addAttribute (SafeAction);
        McheckStatus(stat,"addAttribute safeAction");
    stat = addAttribute (DisplaySafeAction);
        McheckStatus(stat,"addAttribute displaySafeAction");
    stat = addAttribute (SafeTitle);
        McheckStatus(stat,"addAttribute safeTitle");
    stat = addAttribute (DisplaySafeTitle);
        McheckStatus(stat,"addAttribute displaySafeTitle");
    stat = addAttribute (PanScanAttr);
        McheckStatus(stat,"addAttribute panScan");
    stat = addAttribute (AspectRatios);
        McheckStatus(stat,"addAttribute aspectRatios");
    stat = addAttribute (Text);
        McheckStatus(stat,"addAttribute text");
    stat = addAttribute (MiscTextColor);
        McheckStatus(stat,"addAttribute miscTextColor");
    stat = addAttribute (MiscTextTrans);
        McheckStatus(stat,"addAttribute miscTextTrans");
    stat = addAttribute (LineColor);
        McheckStatus(stat,"addAttribute lineColor");
    stat = addAttribute (LineTrans);
        McheckStatus(stat,"addAttribute lineTrans");
    stat = addAttribute (FilmGateMaskColor);
        McheckStatus(stat,"addAttribute filmGateMaskColor");
    stat = addAttribute (FilmGateMaskTrans);
        McheckStatus(stat,"addAttribute filmGateMaskTrans");
    stat = addAttribute (FilmGateLineColor);
        McheckStatus(stat,"addAttribute filmGateLineColor");
    stat = addAttribute (FilmGateLineTrans);
        McheckStatus(stat,"addAttribute filmGateLineTrans");
    stat = addAttribute (ProjGateMaskColor);
        McheckStatus(stat,"addAttribute projGateMaskColor");
    stat = addAttribute (ProjGateMaskTrans);
        McheckStatus(stat,"addAttribute projGateMaskTrans");
    stat = addAttribute (ProjGateLineColor);
        McheckStatus(stat,"addAttribute projGateLineColor");
    stat = addAttribute (ProjGateLineTrans);
        McheckStatus(stat,"addAttribute projGateLineTrans");
    stat = addAttribute (HideLocator);
        McheckStatus(stat,"addAttribute hideLocator");
    stat = addAttribute (CameraFilterMode);
        McheckStatus(stat,"addAttribute cameraFilterMode");
    stat = addAttribute (Cameras);
        McheckStatus(stat,"addAttribute cameras");
    stat = addAttribute (DisplayLineH);
        McheckStatus(stat,"addAttribute displayLineH");
    stat = addAttribute (DisplayLineV);
        McheckStatus(stat,"addAttribute displayLineV");
    stat = addAttribute (DisplayThirdsH);
        McheckStatus(stat,"addAttribute displayThirdsH");
    stat = addAttribute (DisplayThirdsV);
        McheckStatus(stat,"addAttribute displayThirdsV");
    stat = addAttribute (DisplayCrosshair);
        McheckStatus(stat,"addAttribute displayCrosshair");
    stat = addAttribute (DisplayFieldGuide);
        McheckStatus(stat,"addAttribute displayFieldGuide");
    stat = addAttribute (Time);
        McheckStatus(stat,"addAttribute time");
    stat = addAttribute (DriveCameraAperture);
        McheckStatus(stat,"addAttribute driveCameraAperture");
    stat = addAttribute (MaximumDistance);
        McheckStatus(stat,"addAttribute maximumDistance");
    stat = addAttribute (UseOverscan);
        McheckStatus(stat,"addAttribute useOverscan");
    stat = addAttribute (Pad);
        McheckStatus(stat,"addAttribute pad");
    stat = addAttribute (Tag);
        McheckStatus(stat,"addAttribute tag");

    return MS::kSuccess;
}

//---------------------------------------------------------------------------
// Viewport 2.0 override implementation
//---------------------------------------------------------------------------
#if (MAYA_API_VERSION>=201200)
spReticleLocDrawOverride::spReticleLocDrawOverride(const MObject& obj) : MHWRender::MPxDrawOverride(obj, spReticleLocDrawOverride::draw)
{
}

spReticleLocDrawOverride::~spReticleLocDrawOverride()
{
}

#if (MAYA_API_VERSION>=201400 && USE_MUIDRAWMANAGER)
MHWRender::DrawAPI spReticleLocDrawOverride::supportedDrawAPIs() const
{
    // this plugin supports both GL and DX
    return (MHWRender::kOpenGL | MHWRender::kDirectX11);
}
#elif (MAYA_API_VERSION>=201300)
MHWRender::DrawAPI spReticleLocDrawOverride::supportedDrawAPIs() const
{
    // this plugin supports OpenGL
    return (MHWRender::kOpenGL);
}
#endif

#if (MAYA_API_VERSION>=201300)
bool spReticleLocDrawOverride::isBounded(const MDagPath& /*objPath*/,
                                      const MDagPath& /*cameraPath*/) const
{
    return false;
}

bool spReticleLocDrawOverride::disableInternalBoundingBoxDraw() const
{
    return true;
}
#endif

MBoundingBox spReticleLocDrawOverride::boundingBox( const MDagPath& objPath,const MDagPath& cameraPath ) const
{
    return MBoundingBox(MPoint(-1000000,-1000000,-1000000),
                        MPoint(1000000,1000000,1000000));
}

#if (MAYA_API_VERSION < 201400)
MUserData* spReticleLocDrawOverride::prepareForDraw(
                                                    const MDagPath& objPath,
                                                    const MDagPath& cameraPath,
                                                    MUserData* oldData)
#else
MUserData* spReticleLocDrawOverride::prepareForDraw(
                                  const MDagPath& objPath,
                                  const MDagPath& cameraPath,
                                  const MHWRender::MFrameContext& frameContext,
                                  MUserData* oldData)
#endif
{
    MObject obj = objPath.node();
    MFnDependencyNode node(obj);

    spReticleLoc* reticle = static_cast<spReticleLoc*>(node.userNode());

    bool draw = reticle->prepForDraw(obj,objPath,cameraPath);

    spReticleLocData* data = new spReticleLocData();

    data->reticle = reticle;
    data->renderer = &renderer;
    data->draw = draw;

    return data;
}

#if (MAYA_API_VERSION < 201400)
void spReticleLocDrawOverride::draw(const MHWRender::MDrawContext& context, const MUserData* data)
{
    int portWidth, portHeight;

    context.getRenderTargetSize (portWidth, portHeight);

    const spReticleLocData* drawData = static_cast<const spReticleLocData*>(data);
    if (drawData)
    {
        if (drawData->draw)
        {
            drawData->reticle->drawBase(portWidth, portHeight, drawData->renderer);
        }
    }
}
#else
bool spReticleLocDrawOverride::hasUIDrawables() const
{
    return USE_MUIDRAWMANAGER;
}

void spReticleLocDrawOverride::addUIDrawables( const MDagPath& objPath, MHWRender::MUIDrawManager& drawManager, const MHWRender::MFrameContext& frameContext, const MUserData* data )
{
#if (USE_MUIDRAWMANAGER)
    //Get the view attributes that impact drawing
    int oX,oy,portWidth,portHeight;
    frameContext.getViewportDimensions(oX,oy,portWidth,portHeight);

    const spReticleLocData* drawData = static_cast<const spReticleLocData*>(data);
    if (drawData)
    {
        if (drawData->draw)
        {
            renderer.setDrawManager(&drawManager);
            drawData->reticle->drawBase(portWidth, portHeight, &renderer);
        }
    }
#endif
}

void spReticleLocDrawOverride::draw(const MHWRender::MDrawContext& context, const MUserData* data)
{
#if (!USE_MUIDRAWMANAGER)
    int portWidth, portHeight;

    context.getRenderTargetSize (portWidth, portHeight);

    const spReticleLocData* drawData = static_cast<const spReticleLocData*>(data);
    if (drawData)
    {
        if (drawData->draw)
        {
            drawData->reticle->drawBase(portWidth, portHeight, drawData->renderer);
        }
    }
#endif
}
#endif

#endif


//---------------------------------------------------------------------------
// Plugin Registration
//---------------------------------------------------------------------------

MStatus initializePlugin(MObject obj)
{
    MFnPlugin plugin(obj, "SPI", PLUGIN_VERSION, "Any");

#if (MAYA_API_VERSION<201200)
    MStatus status = plugin.registerNode( "spReticleLoc", spReticleLoc::id, 
                         &spReticleLoc::creator, &spReticleLoc::initialize,
                         MPxNode::kLocatorNode );
#else
    MStatus status = plugin.registerNode( "spReticleLoc", spReticleLoc::id, 
                         &spReticleLoc::creator, &spReticleLoc::initialize,
                         MPxNode::kLocatorNode,
                         &spReticleLoc::drawDbClassification);
#endif
    if (!status)
    {
        status.perror("registerNode");
        return status;
    }

#if (MAYA_API_VERSION>=201200)
    status = MHWRender::MDrawRegistry::registerDrawOverrideCreator(
    spReticleLoc::drawDbClassification,
    spReticleLoc::drawRegistrantId,
    spReticleLocDrawOverride::Creator);
    if (!status)
    {
        status.perror("registerDrawOverrideCreator");
        return status;
    }
#endif

#if SOURCE_MEL_SCRIPT
    MGlobal::sourceFile(SOURCE_MEL_SCRIPT_PATH);
#endif

    return status;
}

MStatus uninitializePlugin(MObject obj)
{
    MFnPlugin plugin( obj );
    MStatus status;

#if (MAYA_API_VERSION>=201200)
    status = MHWRender::MDrawRegistry::deregisterDrawOverrideCreator(
        spReticleLoc::drawDbClassification,
        spReticleLoc::drawRegistrantId);
    if (!status)
    {
        status.perror("deregisterDrawOverrideCreator");
        return status;
    }
#endif

    status = plugin.deregisterNode( spReticleLoc::id );
    if (!status)
    {
        status.perror("deregisterNode");
        return status;
    }

    return status;
}

