////////////////////////////////////////////////////////////////////////////////
// Authors: Vitor Bandeira, Mateus Fogaça, Eder Matheus Monteiro e Isadora
// Oliveira
//          (Advisor: Ricardo Reis)
//
// BSD 3-Clause License
//
// Copyright (c) 2019, Federal University of Rio Grande do Sul (UFRGS)
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
////////////////////////////////////////////////////////////////////////////////

#include "LEFParser.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <map>
#include <iostream>

//#include "util/DoubleRectangle.h"
//#include "util/Double2.h"

// LEF headers
#include "lef/lefrReader.hpp"
#include "lef/lefwWriter.hpp"
#include "lef/lefiDebug.hpp"
#include "lef/lefiUtil.hpp"

#define BOOST_POLYGON_NO_DEPS
#include <boost/polygon/polygon.hpp>

// -----------------------------------------------------------------------------

// =============================================================================
// LEF/DEF Common Function for parser
// =============================================================================

// void freeCB(void* name);
// void* mallocCB(size_t size);
// void* reallocCB(void* name, int size);

void* mallocCB(size_t size) {  // DEF Parser version 5.8
        return malloc(size);
}

void* mallocCB(int size) {  // LEF Parser version 5.8
        return malloc(size);
}

// -----------------------------------------------------------------------------

void* reallocCB(void* name, size_t size) {  // DEF Parser version 5.8
        return realloc(name, size);
}

void* reallocCB(void* name, int size) {  // LEF Parser version 5.8
        return realloc(name, size);
}

// -----------------------------------------------------------------------------

void freeCB(void* name) {
        free(name);
        return;
}

// -----------------------------------------------------------------------------

// LEF CALLBACKS
void lefCheckType(lefrCallbackType_e c);
int lefMacroBeginCB(lefrCallbackType_e c, const char* macroName,
                    lefiUserData ud);
int lefMacroEndCB(lefrCallbackType_e c, const char* macroName, lefiUserData ud);
int lefMacroCB(lefrCallbackType_e c, lefiMacro* macro, lefiUserData ud);
int lefPinCB(lefrCallbackType_e c, lefiPin* pin, lefiUserData ud);
int lefSiteCB(lefrCallbackType_e c, lefiSite* site, lefiUserData ud);
int lefUnits(lefrCallbackType_e c, lefiUnits* units, lefiUserData ud);
int lefObstructionCB(lefrCallbackType_e c, lefiObstruction* obs,
                     lefiUserData ud);
int lefLayerCB(lefrCallbackType_e c, lefiLayer* layer, lefiUserData ud);
int lefSpacingCB(lefrCallbackType_e c, lefiSpacing* spacing, lefiUserData ud);
int lefViaCb(lefrCallbackType_e typ, lefiVia* via, lefiUserData data);
int lefViaRuleCb(lefrCallbackType_e typ, lefiViaRule* via, lefiUserData data);

LefDscp& getLibraryFromUserData(lefiUserData userData) {
        return *((LefDscp*)userData);
}  // end function

// =============================================================================
// LEF Function Implementation
// =============================================================================

void LEFParser::parseLEF(const std::string& filename, LefDscp& dscp) {
        //	int retStr = 0;  unused variable

        FILE* lefFile;
        int res;

        lefrInit();

        lefrReset();

        (void)lefrSetOpenLogFileAppend();

        //(void) lefrSetShiftCase(); // will shift name to uppercase if
        // caseinsensitive
        // is set to off or not set

        lefrSetUnitsCbk(lefUnits);
        lefrSetLayerCbk(lefLayerCB);
        lefrSetSpacingCbk(lefSpacingCB);
        lefrSetRegisterUnusedCallbacks();

        // Open the lef file for the reader to read
        if ((lefFile = fopen(filename.c_str(), "r")) == 0) {
                printf("Couldn’t open input file ’%s’\n", filename.c_str());
                exit(1);
        }

        // Invoke the parser
        res = lefrRead(lefFile, filename.c_str(), (void*)&dscp);
        if (res != 0) {
                printf("LEF parser returns an error. #: %d \n", res);

                return;
        }
        //(void) lefrPrintUnusedCallbacks(fout);
        (void)lefrReleaseNResetMemory();
        fclose(lefFile);
}  // end method

int lefUnits(lefrCallbackType_e c, lefiUnits* units, lefiUserData ud) {
        LefDscp& dscp = getLibraryFromUserData(ud);
        LefUnitsDscp& lefUnits = dscp.clsLefUnitsDscp;

        if (units->hasDatabase()) {
                lefUnits.clsDatabase = (int)units->databaseNumber();
                lefUnits.clsHasDatabase = true;
        }

        return 0;
}  // end function

void lefCheckType(lefrCallbackType_e c) {
        if (c >= 0 && c <= lefrLibraryEndCbkType) {
                // OK
        } else {
                printf("ERROR: callback type is out of bounds!\n");
        }
}  // end call back

int lefLayerCB(lefrCallbackType_e c, lefiLayer* layer, lefiUserData ud) {
        static const int X = 0;
        static const int Y = 0;

        LefDscp& dscp = getLibraryFromUserData(ud);
        dscp.clsLefLayerDscps.resize(dscp.clsLefLayerDscps.size() + 1);
        LefLayerDscp& lefLayer = dscp.clsLefLayerDscps.back();
        lefLayer.clsName = layer->lefiLayer::name();
        if (layer->lefiLayer::hasType())
                lefLayer.clsType = layer->lefiLayer::type();
        if (layer->lefiLayer::hasPitch()) {
                lefLayer.clsPitch[X] = layer->lefiLayer::pitch();
                lefLayer.clsPitch[Y] = layer->lefiLayer::pitch();
        }
        if (layer->lefiLayer::hasXYPitch()) {
                lefLayer.clsPitch[X] = layer->lefiLayer::pitchX();
                lefLayer.clsPitch[Y] = layer->lefiLayer::pitchY();
        }
        if (layer->lefiLayer::hasWidth())
                lefLayer.clsWidth = layer->lefiLayer::width();
        if (layer->lefiLayer::hasDirection())
                lefLayer.clsDirection = layer->lefiLayer::direction();
        if (layer->lefiLayer::hasSpacingNumber()) {
                int numSpacing = layer->lefiLayer::numSpacing();
                lefLayer.clsSpacingRules.resize(numSpacing);
                for (int i = 0; i < numSpacing; ++i) {
                        LefSpacingRuleDscp& spcRule =
                            lefLayer.clsSpacingRules[i];
                        spcRule.clsSpacing = layer->lefiLayer::spacing(i);
                        spcRule.clsEOL = layer->lefiLayer::spacingEolWidth(i);
                        spcRule.clsEOLWithin =
                            layer->lefiLayer::spacingEolWithin(i);
                }  // end for
        }          // end if
        if (layer->lefiLayer::hasArea())
                lefLayer.clsArea = layer->lefiLayer::area();
        return 0;
}  // end method

int lefSpacingCB(lefrCallbackType_e c, lefiSpacing* spacing, lefiUserData ud) {
        LefDscp& dscp = getLibraryFromUserData(ud);
        dscp.clsLefSpacingDscps.resize(dscp.clsLefSpacingDscps.size() + 1);
        LefSpacingDscp& lefSpacing = dscp.clsLefSpacingDscps.back();
        lefSpacing.clsLayer1 = spacing->lefiSpacing::name1();
        lefSpacing.clsLayer2 = spacing->lefiSpacing::name2();
        lefSpacing.clsDistance = spacing->lefiSpacing::distance();
        return 0;
}  // end method
