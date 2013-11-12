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

#include "defines.h"
#include "util.h"
#include "OpenGLRenderer.h"

#if(MAYA_API_VERSION>=201400 && USE_MUIDRAWMANAGER)
#include "V2Renderer.h"
#endif

class spReticleLoc : public MPxLocatorNode
{
public:
    spReticleLoc();
    virtual                 ~spReticleLoc();

    virtual void            draw( M3dView & view, const MDagPath & path,
                            M3dView::DisplayStyle style,
                            M3dView::DisplayStatus status );

    virtual bool            setInternalValueInContext( const MPlug &,
                                                       const MDataHandle &,
                                                       MDGContext &);

    static  void            *creator();
    static  MStatus         initialize();

    virtual bool            excludeAsLocator() const;
    virtual void            postConstructor();

    virtual bool            isTransparent() const {return true;}
    virtual bool            drawLast() const      {return true;}
    virtual bool            isBounded() const     {return false;}

    // Get node ready for drawing
    bool                    prepForDraw(const MObject & thisNode, const MDagPath & path, const MDagPath & cameraPath);
    
    // Base draw method
    void                    drawBase(int width, int height, GPURenderer* renderer);

public:
    static MTypeId id;
    static MString drawDbClassification;
    static MString drawRegistrantId;
    static MObject DrawingEnabled;
    static MObject EnableTextDrawing; // added by sjt@sjt.is (May 2010).
    static MObject FilmbackAperture;
    static MObject HorizontalFilmAperture;
    static MObject VerticalFilmAperture;
    static MObject SoundTrackWidth;
    static MObject DisplayFilmGate;
    static MObject ProjectionGate;
    static MObject HorizontalProjectionGate;
    static MObject VerticalProjectionGate;
    static MObject DisplayProjectionGate;
    static MObject SafeAction;
    static MObject HorizontalSafeAction;
    static MObject VerticalSafeAction;
    static MObject DisplaySafeAction;
    static MObject SafeTitle;
    static MObject HorizontalSafeTitle;
    static MObject VerticalSafeTitle;
    static MObject DisplaySafeTitle;
    static MObject RelativeFilmback;
    static MObject AspectRatios;
    static MObject AspectRatio;
    static MObject DisplayMode;
    static MObject AspectMaskColor;
    static MObject AspectMaskTrans;
    static MObject AspectLineColor;
    static MObject AspectLineTrans;
    static MObject AspectDisplaySafeAction;
    static MObject AspectDisplaySafeTitle;
    static MObject PanScanAttr;
    static MObject PanScanAspectRatio;
    static MObject PanScanDisplayMode;
    static MObject PanScanDisplaySafeTitle;
    static MObject PanScanDisplaySafeAction;
    static MObject PanScanRatio;
    static MObject PanScanOffset;
    static MObject PanScanMaskColor;
    static MObject PanScanMaskTrans;
    static MObject PanScanLineColor;
    static MObject PanScanLineTrans;
    static MObject FilmGateMaskColor;
    static MObject FilmGateMaskTrans;
    static MObject FilmGateLineColor;
    static MObject FilmGateLineTrans;
    static MObject ProjGateMaskColor;
    static MObject ProjGateMaskTrans;
    static MObject ProjGateLineColor;
    static MObject ProjGateLineTrans;
    static MObject HideLocator;
    static MObject CameraFilterMode;
    static MObject Cameras;
    static MObject DisplayLineH;
    static MObject DisplayLineV;
    static MObject DisplayThirdsH;
    static MObject DisplayThirdsV;
    static MObject DisplayCrosshair;
    static MObject DisplayFieldGuide;
    static MObject MiscTextColor;
    static MObject MiscTextTrans;
    static MObject LineColor;
    static MObject LineTrans;
    static MObject Time;
    static MObject DriveCameraAperture;
    static MObject MaximumDistance;
    static MObject UseOverscan;
    static MObject Pad;
    static MObject UsePad;
    static MObject PadAmount;
    static MObject PadAmountX;
    static MObject PadAmountY;
    static MObject PadDisplayMode;
    static MObject PadMaskColor;
    static MObject PadMaskTrans;
    static MObject PadLineColor;
    static MObject PadLineTrans;
    static MObject Text;
    static MObject TextType;
    static MObject TextStr;
    static MObject TextAlign;
    static MObject TextVAlign;
    static MObject TextPos;
    static MObject TextPosX;
    static MObject TextPosY;
    static MObject TextPosRel;
    static MObject TextLevel;
    static MObject TextARLevel;
    static MObject TextColor;
    static MObject TextTrans;
    static MObject TextEnabled;
    static MObject TextBold;
    static MObject TextSize;
    static MObject TextScale;
    static MObject Tag;

private:
    MStatus getPadData();
    MStatus getFilmbackData();
    MStatus getProjectionData();
    MStatus getSafeActionData();
    MStatus getSafeTitleData();
    MStatus getAspectRatioChildren ( MPlug arPlug, Aspect_Ratio & ar );
    static bool aspectRatioSortPredicate( const Aspect_Ratio &, const Aspect_Ratio &);
    MStatus getAspectRatioData ();
    bool needToUpdateAspectRatios();
    MStatus getPanScanData ( PanScan & ps );
    MStatus getTextChildren ( MPlug tPlug, TextData & td );
    MStatus generateTextBuffer(TextData &td);
    MStatus getTextData();
    MStatus getOptions();

    MStatus getColor ( MObject colorObj, MObject transObj, MColor & color );
    MMatrix getMatrix( MString matrixStr );
    void printAspectRatio ( Aspect_Ratio & ar );
    void printPanScan ( PanScan & ps );
    void printText ( TextData & td );
    void printGeom ( Geom & g );
    void printOptions ();
    void calcPortGeom();
    void calcFilmbackGeom();
    void calcMaskGeom( Geom & g, double w, double h, Geom & gSrc, double wSrc, double hSrc );
    void calcFilmbackSafeActionGeom();
    void calcFilmbackSafeTitleGeom();
    void calcSafeActionGeom( Aspect_Ratio & ar );
    void calcSafeTitleGeom( Aspect_Ratio & ar );
    void calcAspectGeom( Aspect_Ratio & ar);
    void calcPanScanGeom( PanScan & ps );
    bool calcDynamicText(TextData *td, const int i);
    bool getTextLevelGeometry(TextData *td, Geom &g, const int i);
    bool calcTextPosition(TextData *td, const Geom &g, double &x, double &y, const int i);

    void drawCustomTextElements(GPURenderer* renderer);

    Filmback   oFilmback,filmback;
    PadOptions pad;
    PanScan    panScan;
    Geom       portGeom;
    Options    options;

    double    portWidth;
    double    portHeight;
    double    scaleFactor;
    double    overscan;
    double    ncp;
    MMatrix   wim;
    MFnCamera camera;
    MObject   thisNode;

    int    numAspectRatios;
    bool   loadDefault;
    double maximumDist;
    bool   needRefresh;

    std::vector<Aspect_Ratio> ars;
    std::vector<TextData>     text;

    OpenGLRenderer oglRenderer;
};

//---------------------------------------------------------------------------
// Viewport 2.0 override implementation
//---------------------------------------------------------------------------
#if (MAYA_API_VERSION>=201200)

class spReticleLocData : public MUserData
{
public:
    spReticleLocData() : MUserData(false) {} // don't delete after draw
    virtual ~spReticleLocData() {}

    spReticleLoc* reticle;
    GPURenderer* renderer;
    bool draw;
};

class spReticleLocDrawOverride : public MHWRender::MPxDrawOverride
{
public:
    static MHWRender::MPxDrawOverride* Creator(const MObject& obj)
    {
            return new spReticleLocDrawOverride(obj);
    }

    virtual ~spReticleLocDrawOverride();

#if (MAYA_API_VERSION>=201300)
    virtual MHWRender::DrawAPI supportedDrawAPIs() const;
    
    virtual bool isBounded(
                           const MDagPath& objPath,
                           const MDagPath& cameraPath) const;
    
    virtual bool disableInternalBoundingBoxDraw() const;
#endif

    virtual MBoundingBox boundingBox(
                                     const MDagPath& objPath,
                                     const MDagPath& cameraPath) const;    

#if (MAYA_API_VERSION<201400)
    virtual MUserData* prepareForDraw(
                                      const MDagPath& objPath,
                                      const MDagPath& cameraPath,
                                      MUserData* oldData);
#else
    virtual MUserData* prepareForDraw(
                                      const MDagPath& objPath,
                                      const MDagPath& cameraPath,
                                      const MHWRender::MFrameContext& frameContext,
                                      MUserData* oldData);

    virtual bool hasUIDrawables() const;

    virtual void addUIDrawables(
                                const MDagPath& objPath,
                                MHWRender::MUIDrawManager& drawManager,
                                const MHWRender::MFrameContext& frameContext,
                                const MUserData* data);
#endif

    static void draw(const MHWRender::MDrawContext& context, const MUserData* data);

private:
    spReticleLocDrawOverride(const MObject& obj);
    spReticleLocData* data;

#if (MAYA_API_VERSION>=201400 && USE_MUIDRAWMANAGER)
    V2Renderer renderer;
#else
    OpenGLRenderer renderer;
#endif
};

#endif
