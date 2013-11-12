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
//  V2Renderer.cpp
//  spReticle.1.7
//
//  Created by Henry Vera on 4/23/13.
//
//

#include <maya/MTypes.h>

#if (MAYA_API_VERSION >= 201400)

#include <stdio.h>

#include <maya/MGlobal.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MUintArray.h>

#include "V2Renderer.h"

V2Renderer::V2Renderer()
{
}

V2Renderer::~V2Renderer()
{
}

void V2Renderer::setDrawManager(MHWRender::MUIDrawManager* drawManager)
{
    this->drawManager = drawManager;
}

void V2Renderer::prepareForDraw(float portWidth, float portHeight)
{
    drawManager->beginDrawable();
}

void V2Renderer::postDraw()
{
    drawManager->endDrawable();
}

// Given two Geom instances, this calculates the mask area between them.
//
void V2Renderer::drawMask( Geom g1, Geom g2, MColor color, bool sides, bool top=true )
{
	drawManager->setColor(MColor(color.r, color.g, color.b, 1 - color.a));
    
	MUintArray index;
	index.append(0);
	index.append(1);
	index.append(3);
	index.append(2);
    
    if (top)
    {
        if ( (g2.y1 - g1.y1) > EPSILON )
        {
            // Bottom Mask
            MPointArray bottomMask;
            bottomMask.append(MPoint( g1.x1, g1.y1, 0.0));
            bottomMask.append(MPoint( g1.x2, g1.y1, 0.0));
            bottomMask.append(MPoint( g2.x2, g2.y1, 0.0));
            bottomMask.append(MPoint( g2.x1, g2.y1, 0.0));
            drawManager->mesh2d(MHWRender::MUIDrawManager::kTriStrip,bottomMask,NULL,&index);
        }

    
        if ( (g1.y2 - g2.y2) > EPSILON )
        {
            // Top Mask
            MPointArray topMask;
            topMask.append(MPoint( g2.x1, g2.y2, 0.0));
            topMask.append(MPoint( g2.x2, g2.y2, 0.0));
            topMask.append(MPoint( g1.x2, g1.y2, 0.0));
            topMask.append(MPoint( g1.x1, g1.y2, 0.0));
            drawManager->mesh2d(MHWRender::MUIDrawManager::kTriStrip,topMask,NULL,&index);
        }
    }
    else
    {
        g1.y1 = g2.y1;
        g1.y2 = g2.y2;
    }
    
	if (sides)
	{
		// Left side mask
        if ((g2.x1 - g1.x1) > EPSILON)
        {
            MPointArray leftMask;
            leftMask.append(MPoint( g1.x1, g1.y1, 0.0));
            leftMask.append(MPoint( g2.x1, g2.y1, 0.0));
            leftMask.append(MPoint( g2.x1, g2.y2, 0.0));
            leftMask.append(MPoint( g1.x1, g1.y2, 0.0));
            drawManager->mesh2d(MHWRender::MUIDrawManager::kTriStrip,leftMask,NULL,&index);
        }
        
		// right side mask
        if ((g1.x2 - g2.x2) > EPSILON)
        {
            MPointArray rightMask;
            rightMask.append(MPoint( g1.x2, g1.y1, 0.0));
            rightMask.append(MPoint( g2.x2, g2.y1, 0.0));
            rightMask.append(MPoint( g2.x2, g2.y2, 0.0));
            rightMask.append(MPoint( g1.x2, g1.y2, 0.0));
            drawManager->mesh2d(MHWRender::MUIDrawManager::kTriStrip,rightMask,NULL,&index);
        }
	}
}

// This draws a single line between the specified points.
//
void V2Renderer::drawLine(double x1, double x2, double y1, double y2,
                            MColor color, bool stipple)
{
	//drawManager->beginDrawable();
    
	drawManager->setColor(color);
    
	if(stipple) {
		drawManager->setLineStyle(2,0x00FF);
	}
    
	drawManager->line2d(MPoint(x1,y1),MPoint(x2,y2));
    
	//drawManager->endDrawable();
}

// Given a Geom instance, this will draw a line connecting the points.
// The argument side determines whether the sides will be drawn (the top
// will always be drawn). The stipple argument specifies whether the line
// should be solid or dashed/stippled.
//
void V2Renderer::drawLines( Geom g, MColor color, bool sides, bool stipple)
{
	//drawManager->beginDrawable();
    
	drawManager->setColor(MColor(color.r,color.g,color.b,1-color.a));
    
	if(stipple) {
		drawManager->setLineStyle(2,0x00FF);
	}
    
	if(sides) {
		drawManager->rect2d(MPoint ((g.x1 + g.x2)/2, (g.y1 + g.y2)/2),
                           MVector(0.0,1.0),
                           fabs(g.x1-g.x2)/2,fabs(g.y1-g.y2)/2);
	} else {
		drawManager->line2d(MPoint(g.x1,g.y1),MPoint(g.x2,g.y1));
		drawManager->line2d(MPoint(g.x2,g.y2),MPoint(g.x1,g.y2));
	}
    
	//drawManager->endDrawable();
}

// This function uses MUIDrawManager to draw text.
//
void V2Renderer::drawText(TextData *td, double tx, double ty)
{
    double screenScaleFactor = (td->textScale) ? filmback->filmbackGeom.x/1280.0f : 1.0f;

#if (MAYA_API_VERSION >= 201500)
    int fontSize = td->textSize * screenScaleFactor;
    drawManager->setFontName("Bitstream Charter");
	drawManager->setFontSize(fontSize);
#else
    int fontSize = 14;
#endif

    // Adjust ty for text alignment
    switch (td->textVAlign) {
        case 0:
            ty -= (fontSize * 0.22f);
            break;
        case 1:
            ty -= fontSize / 2.0f;
            break;
        case 2:
            ty -= fontSize;
            break;
    }
    
    // Adjust text position to account for screen scaling
    tx += td->textPosX*screenScaleFactor;
    ty += td->textPosY*screenScaleFactor;
    
	//drawManager->beginDrawable();
	
    //drawManager->setColor(td->textColor);
	drawManager->setColor(MColor(td->textColor.r,td->textColor.g,td->textColor.b,1-td->textColor.a));
	drawManager->text2d(MPoint(tx,ty),td->textStr,(MHWRender::MUIDrawManager::TextAlignment)td->textAlign);
    
	//drawManager->endDrawable();
}

#endif

