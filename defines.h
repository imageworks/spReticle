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
//  defines.h
//  spReticle.1.7
//
//  Created by Henry Vera on 4/23/13.
//
//

#ifndef spReticle_defines_h
#define spReticle_defines_h

#define PLUGIN_VERSION          "2.0"

//  Epsilon value to use when testing for equality with floating values
#define EPSILON                 0.0000001

// Define whether to source a MEL script upon instantiation, and if so, which script in the script path
#define SOURCE_MEL_SCRIPT       true
#define SOURCE_MEL_SCRIPT_PATH  "spReticleLoc.mel"
#define SOURCE_MEL_METHOD       "spReticleLocSetDefault"

// If identifying a camera to display based upon an attribute, use the following attribute
#define CAMERA_ATTR             "useSpReticle"

// Text standard defines
#define SHOW_ENV_VAR            "SHOW"
#define SEQ_ENV_VAR             "SEQ"
#define SHOT_ENV_VAR            "SHOT"
#define FRAME_START_ENV_VAR     "FS"
#define FRAME_END_ENV_VAR       "FE"

// Font Defines
#define MINFONT                 4
#define	MAXFONT                 120

// Field Guide
#define FIELDGUIDE_NUM_LINES    11

// Specifies whether the VP2.0 MUIDrawManager class should be used for rendering vs OpenGL.
// Currently, it has proper draw-order integration with image planes and support for DX11.
// Cons are that it is slower, fonts are aliased, and line rendering is sometimes occluded by masks.
// This only impacts Viewport 2.0.  Viewport 1.0 will use the OpenGLRenderer regardless.
#define USE_MUIDRAWMANAGER      false 

#endif
