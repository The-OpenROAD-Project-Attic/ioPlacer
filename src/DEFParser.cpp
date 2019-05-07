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

#include "DEFParser.h"
#include <iostream>
#include <fstream>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../3rdparty/def/defiComponent.hpp"
#include "../3rdparty/def/defiNet.hpp"
#include "../3rdparty/def/defiRowTrack.hpp"

#include "../3rdparty/def/defwWriter.hpp"

#include "../3rdparty/def/defrReader.hpp"

extern void freeCB(void* name);
extern void* mallocCB(size_t size);
extern void* reallocCB(void* name, size_t size);

DEFParser::DEFParser() {}

DEFParser::~DEFParser() {}

void defCheckType(defrCallbackType_e c);
int defComp(defrCallbackType_e c, defiComponent* co, defiUserData ud);
int defComponentStart(defrCallbackType_e c, int num, defiUserData ud);
int defDesignName(defrCallbackType_e c, const char* string, defiUserData ud);
int defEndFunc(defrCallbackType_e c, void* dummy, defiUserData ud);
int defExt(defrCallbackType_e t, const char* c, defiUserData ud);
int defNetStart(defrCallbackType_e c, int num, defiUserData ud);
int defNet(defrCallbackType_e c, defiNet* net, defiUserData ud);
int defUnits(defrCallbackType_e c, double d, defiUserData ud);
int defVersion(defrCallbackType_e c, double d, defiUserData ud);
int defRow(defrCallbackType_e type, defiRow* rowInfo, defiUserData userData);
char* defOrientStr(int orient);
int defOrient(std::string orient);
int defDieArea(defrCallbackType_e typ, defiBox* box, defiUserData ud);
int defTrack(defrCallbackType_e typ, defiTrack* track, defiUserData data);
int defPin(defrCallbackType_e, defiPin* pin, defiUserData ud);

DefDscp& getDesignFromUserData(defiUserData userData) {
        return *((DefDscp*)userData);
} 

void DEFParser::parseDEF(const std::string& filename, DefDscp& defDscp) {
        defrInit();
        defrReset();

        defrSetComponentCbk(defComp);
        defrSetDesignCbk(defDesignName);
        defrSetPinCbk(defPin);
        defrSetUnitsCbk(defUnits);
        defrSetVersionCbk(defVersion);
        defrSetComponentStartCbk(defComponentStart);
        defrSetDieAreaCbk(defDieArea);
        //        defrSetMallocFunction(mallocCB);
        //        defrSetReallocFunction(reallocCB);
        //        defrSetFreeFunction(freeCB);
        defrSetNetStartCbk(defNetStart);
        defrSetNetCbk(defNet);

        // register track call back
        defrSetTrackCbk(defTrack);

        defrSetAddPathToNet();

        FILE* f;
        int res;

        (void)defrSetOpenLogFileAppend();

        if ((f = fopen(filename.c_str(), "r")) == 0) {
                printf("Couldn't open input file '%s'\n", filename.c_str());
                return;
        }
        // Set case sensitive to 0 to start with, in History &
        // PropertyDefinition
        // reset it to 1.
        res = defrRead(f, filename.c_str(), (void*)&defDscp, 1);

        if (res) printf("Reader returns bad status. %s\n", filename.c_str());

        (void)defrReleaseNResetMemory();
        (void)defrUnsetNonDefaultCbk();
        (void)defrUnsetNonDefaultStartCbk();
        (void)defrUnsetNonDefaultEndCbk();
        defrClear();
}

char* defAddress(const char* in) { return ((char*)in); }

void defCheckType(defrCallbackType_e c) {
        if (c >= 0 && c <= defrDesignEndCbkType) {
                // OK
        } else {
                printf("ERROR: callback type is out of bounds!\n");
        }
}

int defPin(defrCallbackType_e, defiPin* pin, defiUserData userData) {
        DefDscp& defDscp = getDesignFromUserData(userData);
        defDscp.IOPins.resize(defDscp.IOPins.size() + 1);
        IOPinDscp& defPin = defDscp.IOPins.back();

        defPin.name = pin->pinName();
        defPin.netName = pin->netName();
        defPin.direction = pin->direction();
        defPin.position.setX(pin->placementX());
        defPin.position.setY(pin->placementY());
        defPin.orientation = pin->orientStr();
        if (pin->hasLayer()) {
                defPin.layerName = pin->layer(0);
                int xl, yl, xh, yh;
                pin->bounds(0, &xl, &yl, &xh, &yh);
                Coordinate lowerBound(xl, yl);
                Coordinate upperBound(xh, yh);
                defPin.layerBounds = Box(lowerBound, upperBound);
        }

        if (pin->isPlaced()) {
                defPin.locationType = "PLACED";
        } else if (pin->isCover()) {
                defPin.locationType = "COVER";
        } else if (pin->isFixed()) {
                defPin.locationType = "FIXED";
        }

        defPin.use = pin->use();

        return 0;
}

int defComp(defrCallbackType_e c, defiComponent* co, defiUserData ud) {
        DefDscp& defDscp = getDesignFromUserData(ud);
        defDscp.Comps.push_back(ComponentDscp());
        ComponentDscp& defComp = defDscp.Comps.back();

        defComp.name = DEFParser::unescape(co->id());
        defComp.macroName = co->name();
        defComp.isFixed = co->isFixed();
        defComp.isPlaced = co->isPlaced();
        defComp.position.setX(co->placementX());
        defComp.position.setY(co->placementY());
        defComp.orientation = co->placementOrientStr();

        return 0;
}

int defComponentStart(defrCallbackType_e c, int num, defiUserData ud) {
        DefDscp& defDscp = getDesignFromUserData(ud);
        defDscp.Comps.reserve(num);
        return 0;
}

int defNetStart(defrCallbackType_e c, int num, defiUserData ud) {
        DefDscp& defDscp = getDesignFromUserData(ud);
        defDscp.Nets.reserve(num);
        return 0;
}

int defDesignName(defrCallbackType_e c, const char* string, defiUserData ud) {
        DefDscp& defDscp = getDesignFromUserData(ud);
        defDscp.clsDesignName = string;
        return 0;
}

int defNet(defrCallbackType_e c, defiNet* net, defiUserData ud) {
        DefDscp& defDscp = getDesignFromUserData(ud);
        defDscp.Nets.push_back(NetDscp());
        NetDscp& netDscp = defDscp.Nets.back();
        netDscp.name = net->name();
        const char* use = net->use();
        if (use) {
                netDscp.use = use;
        }
        netDscp.connections.resize(net->numConnections());
        for (int i = 0; i < net->numConnections(); i++) {
                NetConnection& connection = netDscp.connections[i];
                connection.pinName = net->pin(i);
                connection.componentName =
                    DEFParser::unescape(net->instance(i));
        }

        return 0;
}

int defTrack(defrCallbackType_e typ, defiTrack* track, defiUserData ud) {
        DefDscp& defDscp = getDesignFromUserData(ud);
        defDscp.clsTracks.push_back(TrackDscp());
        TrackDscp& trackDscp = defDscp.clsTracks.back();
        trackDscp.direction = track->macro();
        trackDscp.location = static_cast<DBU>(std::round(track->x()));
        trackDscp.numTracks = static_cast<DBU>(std::round(track->xNum()));
        trackDscp.space = static_cast<DBU>(std::round(track->xStep()));
        trackDscp.layers.reserve(track->numLayers());
        for (int i = 0; i < track->numLayers(); i++) {
                trackDscp.layers.push_back(track->layer(i));
        }
        return 0;
}

char* defOrientStr(int orient) {
        switch (orient) {
                case 0:
                        return ((char*)"N");
                case 1:
                        return ((char*)"W");
                case 2:
                        return ((char*)"S");
                case 3:
                        return ((char*)"E");
                case 4:
                        return ((char*)"FN");
                case 5:
                        return ((char*)"FW");
                case 6:
                        return ((char*)"FS");
                case 7:
                        return ((char*)"FE");
        };
        return ((char*)"BOGUS");
}

int defOrient(std::string orient) {
        if (orient.compare("N") == 0) return 0;
        if (orient.compare("W") == 0) return 1;
        if (orient.compare("S") == 0) return 2;
        if (orient.compare("E") == 0) return 3;
        if (orient.compare("FN") == 0) return 4;
        if (orient.compare("FW") == 0) return 5;
        if (orient.compare("FS") == 0) return 6;
        if (orient.compare("FE") == 0) return 7;
        return -1;
}

int defDieArea(defrCallbackType_e typ, defiBox* box, defiUserData ud) {
        DefDscp& defDscp = getDesignFromUserData(ud);
        Coordinate lowerBound(box->defiBox::xl(), box->defiBox::yl());
        Coordinate upperBound(box->defiBox::xh(), box->defiBox::yh());
        defDscp.clsDieBounds = Box(lowerBound, upperBound);

        return 0;
}

#define CHECK_STATUS(status)            \
        if (status) {                   \
                defwPrintError(status); \
        }

std::string DEFParser::unescape(const std::string& str) {
        std::string result;

        bool scapeNext = false;
        for (char ch : str) {
                if (scapeNext) {
                        result += ch;
                        scapeNext = false;
                } else if (ch == '\\') {
                        scapeNext = true;
                } else {
                        result += ch;
                        scapeNext = false;
                }
        }

        return result;
}

int defUnits(defrCallbackType_e c, double d, defiUserData ud) {
        DefDscp& defDscp = getDesignFromUserData(ud);
        defDscp.clsDatabaseUnits = static_cast<int>(d);
        return 0;
}

int defVersion(defrCallbackType_e c, double d, defiUserData ud) {
        DefDscp& defDscp = getDesignFromUserData(ud);
        defDscp.clsVersion = d;
        return 0;
}