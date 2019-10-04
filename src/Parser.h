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

#ifndef __PARSER_H_
#define __PARSER_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <map>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>

#include "Core.h"
#include "Netlist.h"
#include "Parameters.h"
#include "Coordinate.h"
#include "DEFDescriptor.h"
#include "DEFParser.h"
#include "LEFDescriptor.h"
#include "LEFParser.h"

class Parser {
        typedef boost::geometry::model::d2::point_xy<DBU> point;
        typedef boost::geometry::model::box<point> box;

       private:
        struct cellPin {
                std::string name;
                point position;
        };

        struct ioPin {
                std::string name;
                point position;
                std::string netName;
                box bounds;
                std::string direction;
                std::vector<cellPin> connections;
                std::string locationType;
        };

        std::map<std::string, NetDscp> mapIOPinToNet;
        std::map<std::string, point> mapInstToPosition;

        Parameters& _parms;
        Netlist& _netlist;
        Core& _core;
        box _dieArea;
        std::vector<ioPin> _ioPins;
        LEFParser _lefParser;
        DEFParser _defParser;
        LefDscp _lefDscp;
        DefDscp _defDscp;
        bool _hasLef = false;
        bool _hasDef = false;

        point getInstPosition(std::string);
        void readDieArea();
        void readConnections();
        void initNetlist();
        void initCore();
        void initMapIOtoNet();
        void initMapInstToPosition();

       public:
        Parser(Parameters&, Netlist&, Core&);
        void parseLef(const std::string& file);
        void parseDef(const std::string& file);

        bool hasDb() { return _hasLef && _hasDef; }  

        void initData();
        void getBlockages(std::string,
                          std::vector<std::pair<Coordinate, Coordinate>>&);
        bool isDesignPlaced();
        std::vector<std::string> getLayerNames();
        bool verifyRequiredData();
};

#endif /* __PARSER_H_ */
