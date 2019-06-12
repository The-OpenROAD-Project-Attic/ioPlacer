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
        std::string _name = INVALID;
        std::string _macroName = INVALID;
        std::string _locationType =
            INVALID;  // {FIXED | COVER | PLACED | UNPLACED }
        Coordinate _position = Coordinate(0, 0);
        std::string _orientation = INVALID;
        bool _isFixed = false;
        bool _isPlaced = false;
        ComponentDscp() = default;
};

class IOPinDscp {
       public:
        std::string _name = INVALID;
        std::string _netName = INVALID;
        std::string _direction = INVALID;  // {INPUT | OUTPUT | INOUT | FEEDTHRU}
        std::string _locationType = INVALID;  // {FIXED | COVER | PLACED }
        std::string _orientation = INVALID;
        std::string _layerName = INVALID;
        std::string _use = INVALID;  // { SIGNAL | POWER | GROUND |
        // CLOCK | TIEOFF | ANALOG}
        Coordinate _position = Coordinate(0, 0);
        Box _layerBounds;
        IOPinDscp() = default;
};

class NetConnection {
       public:
        std::string _pinName = INVALID;
        std::string _componentName = INVALID;
        NetConnection() = default;
};

class NetDscp {
       public:
        std::string _name = INVALID;
        std::string _use = INVALID;
        std::vector<NetConnection> _connections;
        NetDscp() = default;
};

class TrackDscp {
       public:
        bool _hasRect : 1;
        bool _hasPolygon : 1;
        std::string _direction = INVALID;
        DBU _location = 0;
        int _numTracks = 0;
        std::vector<std::string> _layers;
        DBU _space = 0;

        TrackDscp() {
                _hasRect = false;
                _hasPolygon = false;
        }
};

class DefDscp {
       public:
        bool _clsHasVersion : 1;
        bool _clsHasDevideChar : 1;
        bool _clsHasBusBitChar : 1;
        bool _clsHasDieBounds : 1;
        bool _clsHasDatabaseUnits : 1;
        double _clsVersion = 0.0;
        std::string _clsDeviderChar = INVALID;
        std::string _clsBusBitChars = INVALID;
        std::string _clsDesignName = INVALID;
        Box _clsDieBounds;
        int _clsDatabaseUnits = 0;
        std::vector<ComponentDscp> _Comps;
        std::vector<IOPinDscp> _IOPins;
        std::vector<NetDscp> _Nets;
        std::vector<TrackDscp> _clsTracks;
        DefDscp() {
                _clsHasVersion = false;
                _clsHasDevideChar = false;
                _clsHasBusBitChar = false;
                _clsHasDieBounds = false;
                _clsHasDatabaseUnits = false;
        }
};

#endif /* DEFDESCRIPTOR_H */
