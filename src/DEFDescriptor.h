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

#ifndef DEFDESCRIPTOR_H
#define DEFDESCRIPTOR_H

#include <vector>
#include "Box.h"
#include "Coordinate.h"
#include "Netlist.h"

static const std::string INVALID = "INVALID_STRING*";

class ComponentDscp {
       public:
        std::string name = INVALID;
        std::string macroName = INVALID;
        std::string locationType =
            INVALID;  // {FIXED | COVER | PLACED | UNPLACED }
        Coordinate position = Coordinate(0, 0);
        std::string orientation = INVALID;
        bool isFixed = false;
        bool isPlaced = false;
        ComponentDscp() = default;
};

class IOPinDscp {
       public:
        std::string name = INVALID;
        std::string netName = INVALID;
        std::string direction = INVALID;  // {INPUT | OUTPUT | INOUT | FEEDTHRU}
        std::string locationType = INVALID;  // {FIXED | COVER | PLACED }
        std::string orientation = INVALID;
        std::string layerName = INVALID;
        std::string use = INVALID;  // { SIGNAL | POWER | GROUND |
        // CLOCK | TIEOFF | ANALOG}
        Coordinate position = Coordinate(0, 0);
        Box layerBounds;
        IOPinDscp() = default;
};

class NetConnection {
       public:
        std::string pinName = INVALID;
        std::string componentName = INVALID;
        NetConnection() = default;
};

class NetDscp {
       public:
        std::string name = INVALID;
        std::string use = INVALID;
        std::vector<NetConnection> connections;
        NetDscp() = default;
};

class TrackDscp {
       public:
        bool hasRect : 1;
        bool hasPolygon : 1;
        std::string direction = INVALID;
        DBU location = 0;
        int numTracks = 0;
        std::vector<std::string> layers;
        DBU space = 0;

        TrackDscp() {
                hasRect = false;
                hasPolygon = false;
        }
};

class DefDscp {
       public:
        bool clsHasVersion : 1;
        bool clsHasDevideChar : 1;
        bool clsHasBusBitChar : 1;
        bool clsHasDieBounds : 1;
        bool clsHasDatabaseUnits : 1;
        double clsVersion = 0.0;
        std::string clsDeviderChar = INVALID;
        std::string clsBusBitChars = INVALID;
        std::string clsDesignName = INVALID;
        Box clsDieBounds;
        int clsDatabaseUnits = 0;
        std::vector<ComponentDscp> Comps;
        std::vector<IOPinDscp> IOPins;
        std::vector<NetDscp> Nets;
        std::vector<TrackDscp> clsTracks;
        DefDscp() {
                clsHasVersion = false;
                clsHasDevideChar = false;
                clsHasBusBitChar = false;
                clsHasDieBounds = false;
                clsHasDatabaseUnits = false;
        }
};

#endif /* DEFDESCRIPTOR_H */
