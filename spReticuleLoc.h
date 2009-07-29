///////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009, Sony Pictures Imageworks
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// 
// Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
// Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
// Neither the name of the organization Sony Pictures Imageworks nor the
// names of its contributors
// may be used to endorse or promote products derived from this software
// without specific prior written permission.
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
// PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER
// OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
///////////////////////////////////////////////////////////////////////////////

class spReticuleLoc : public MPxLocatorNode
{
public:
    spReticuleLoc();
    virtual                 ~spReticuleLoc();

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
    virtual bool            isBounded() const     {return true;}
    virtual MBoundingBox    boundingBox() const;

public:
    static MTypeId id;
    static MObject DrawingEnabled;
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
    static MObject FilmGateColor;
    static MObject FilmGateTrans;
    static MObject ProjGateColor;
    static MObject ProjGateTrans;
    static MObject HideLocator;
    static MObject UseSpReticule;
    static MObject DisplayLineH;
    static MObject DisplayLineV;
    static MObject DisplayThirdsH;
    static MObject DisplayThirdsV;
    static MObject DisplayCrosshair;
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
    static MObject TextPos;
    static MObject TextPosX;
    static MObject TextPosY;
    static MObject TextPosRel;
    static MObject TextLevel;
    static MObject TextARLevel;
    static MObject TextColor;
    static MObject TextTrans;
    static MObject Tag;

private:
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
            bool   displaySafeAction;
            bool   displaySafeTitle;
            
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
            double soundTrackWidth;
            double horizontalImageAperture;
            double verticalImageAperture;
            double displayFilmGate;
            double horizontalProjectionGate;
            double verticalProjectionGate;
            double horizontalSafeAction;
            double verticalSafeAction;
            double horizontalSafeTitle;
            double verticalSafeTitle;
            double displayProjGate;
            bool   displaySafeAction;
            bool   displaySafeTitle;
            
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
            bool   useSpRet;
            bool   displayLineH;
            bool   displayLineV;
            bool   displayThirdsH;
            bool   displayThirdsV;
            bool   displayCrosshair;
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
            double textPosX;
            double textPosY;
            int    textPosRel;
            int    textLevel;
            int    textARLevel;
            MColor textColor;
    };

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
    MStatus getTextData();
    bool needToUpdateTextData();
    MStatus getOptions();

    MPoint getPoint (float x, float y, M3dView & view);
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
    void drawMask3D( Geom g1, Geom g2, double z, MColor color, bool sides );
    void drawLine3D( double x1, double x2, double y1, double y2, double z, MColor color, bool stipple );
    void drawLines3D( Geom g, double z, MColor color, bool sides, bool stipple );
    void drawMask( Geom g1, Geom g2, MColor color, bool sides );
    void drawLine( double x1, double x2, double y1, double y2, MColor color, bool stipple );
    void drawLines( Geom g, MColor color, bool sides, bool stipple );
    void drawText( MString text, double tx, double ty, MColor color, M3dView::TextPosition, M3dView & view );
    void drawInternalTextElements(M3dView & view);
    void drawCustomTextElements(M3dView & view);

    Filmback   oFilmback,filmback;
    PadOptions pad;
    PanScan    panScan;
    Geom       portGeom;
    Options    options;

    double    portWidth;
    double    portHeight;
    double    overscan;
    double    ncp;
    MMatrix   wim;
    MFnCamera camera;
    MObject   thisNode;

    int    numAspectRatios;
    double loadDefault;
    double maximumDist;
    bool   needRefresh;

    std::vector<Aspect_Ratio> ars;
    std::vector<TextData>     text;
};

