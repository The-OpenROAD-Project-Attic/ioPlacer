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

#include <cstdio>
#include <istream>
#include <fstream>
#include <iostream>
#include "Parser.h"
#include <boost/algorithm/string.hpp>

Parser::Parser(Parameters& parms, Netlist& netlist, Core& core)
    : _parms(parms), _netlist(netlist), _core(core) {}

void Parser::run() {
        _defParser.parseDEF(_parms.getInputDefFile(), _defDscp);
        initMapIOtoNet();
        initMapInstToPosition();
        readDieArea();
        readConnections();
        initNetlist();
        initCore();
}

Parser::point Parser::getInstPosition(std::string instName) {
        point position = point(-1, -1);
        for (ComponentDscp comp : _defDscp._Comps) {
                if (instName == comp._name) {
                        DBU x = comp._position.getX();
                        DBU y = comp._position.getY();
                        position = point(x, y);
                        break;
                }
        }
        return position;
}

void Parser::readDieArea() {
        point lower = point(_defDscp._clsDieBounds.getLowerBound().getX(),
                            _defDscp._clsDieBounds.getLowerBound().getY());
        point upper = point(_defDscp._clsDieBounds.getUpperBound().getX(),
                            _defDscp._clsDieBounds.getUpperBound().getY());

        _dieArea = box(lower, upper);
}  // end method

void Parser::readConnections() {
        int ioCounter = -1;
        for (IOPinDscp io : _defDscp._IOPins) {
                ioPin pin;
                pin.name = io._name;
                pin.position = point(io._position.getX(), io._position.getY());
                pin.netName = io._netName;
                pin.bounds = box(point(io._layerBounds.getLowerBound().getX(),
                                       io._layerBounds.getLowerBound().getY()),
                                 point(io._layerBounds.getUpperBound().getX(),
                                       io._layerBounds.getUpperBound().getY()));
                pin.direction = io._direction;
                _ioPins.push_back(pin);
                ioCounter++;

                NetDscp net = mapIOPinToNet[io._name];
                for (NetConnection conn : net._connections) {
                        if (conn._componentName == "PIN") {
                                continue;
                        }

                        cellPin cPin;
                        cPin.name = conn._componentName + ":" + conn._pinName;

                        cPin.position = mapInstToPosition[conn._componentName];
                        _ioPins[ioCounter].connections.push_back(cPin);
                }
        }
}  // end method

void Parser::initNetlist() {
        for (unsigned i = 0; i < _ioPins.size(); ++i) {
                ioPin& io = _ioPins[i];
                Direction dir = IN;
                if (io.direction == "OUTPUT") {
                        dir = OUT;
                } else if (io.direction == "INOUT") {
                        dir = INOUT;
                }

                Coordinate lowerBound(_ioPins[i].bounds.min_corner().x(),
                                      _ioPins[i].bounds.min_corner().y());
                Coordinate upperBound(_ioPins[i].bounds.max_corner().x(),
                                      _ioPins[i].bounds.max_corner().y());

                std::string netName = _ioPins[i].netName;
                Coordinate pos(_ioPins[i].position.x(),
                               _ioPins[i].position.y());

                IOPin ioPin(io.name, pos, dir, lowerBound, upperBound, netName);
                std::vector<InstancePin> instPins;
                for (unsigned j = 0; j < io.connections.size(); ++j) {
                        cellPin& cellPin = io.connections[j];
                        instPins.push_back(InstancePin(
                            cellPin.name, Coordinate(cellPin.position.x(),
                                                     cellPin.position.y())));
                }
                _netlist.addIONet(ioPin, instPins);
        }
}

void Parser::initCore() {
        Coordinate lowerBound(_dieArea.min_corner().x(),
                              _dieArea.min_corner().y());
        Coordinate upperBound(_dieArea.max_corner().x(),
                              _dieArea.max_corner().y());
        DBU minSpacingX = 0;
        DBU minSpacingY = 0;
        DBU initTrackX = 0;
        DBU initTrackY = 0;

        for (TrackDscp track : _defDscp._clsTracks) {
                if (boost::iequals(track._layers[0],
                    "Metal" +
                        std::to_string(_parms.getHorizontalMetalLayer()))) {
                        if (boost::iequals(track._direction, "Y")) {
                                minSpacingY = track._space;
                                initTrackY = track._location;
                        }
                } else if (boost::iequals(track._layers[0],
                            "Metal" + 
                            std::to_string(_parms.getVerticalMetalLayer()))) {
                        if (boost::iequals(track._direction, "X")) {
                                minSpacingX = track._space;
                                initTrackX = track._location;
                        }
                }
        }

        DBU IOWidth = _ioPins[0].bounds.max_corner().x() -
                      _ioPins[0].bounds.min_corner().x();
        if (minSpacingX <= IOWidth) {
                minSpacingX *= 2;
        }
        if (minSpacingY <= IOWidth) {
                minSpacingY *= 2;
        }

        /* TODO:  <23-05-19, here we multiply the spacing by two to comply with
         * minimum spacing in the same metal layer, to position two pins in
         * neighbour track one need to consider changing metal layers between
         * them > */
        _core = Core(lowerBound, upperBound, minSpacingX * 2, minSpacingY * 2,
                      initTrackX, initTrackY);
}

void Parser::initMapIOtoNet() {
        for (NetDscp net : _defDscp._Nets) {
                for (NetConnection conn : net._connections) {
                        if (conn._componentName != "PIN") continue;

                        mapIOPinToNet[conn._pinName] = net;
                }
        }
}

void Parser::initMapInstToPosition() {
        for (ComponentDscp comp : _defDscp._Comps) {
                point p = point(comp._position.getX(), comp._position.getY());
                mapInstToPosition[comp._name] = p;
        }
}

void Parser::getBlockages(
    std::string filename,
    std::vector<std::pair<Coordinate, Coordinate>>& blockages) {
        std::ifstream f;
        f.open(filename);
        DBU initialX;
        DBU initialY;
        DBU finalX;
        DBU finalY;
        Coordinate coreLowerBound = _core.getLowerBound();
        Coordinate coreUpperBound = _core.getUpperBound();
        if (f.is_open()) {
                while (f >> initialX >> initialY >> finalX >> finalY) {
                        if (initialX != finalX && initialY != finalY) {
                                std::cout << "ERROR: Blockage should begin and "
                                             "end on the same edge";
                                exit(-1);
                        }
                        initialX = std::max(initialX, coreLowerBound.getX());
                        initialY = std::max(initialY, coreLowerBound.getY());
                        finalX = std::min(finalX, coreUpperBound.getX());
                        finalY = std::min(finalY, coreUpperBound.getY());
                        Coordinate initialCoord(initialX, initialY);
                        Coordinate finalCoord(finalX, finalY);
                        std::pair<Coordinate, Coordinate> block(initialCoord,
                                                                finalCoord);
                        blockages.push_back(block);
                }
        }
        f.close();
}

bool Parser::isDesignPlaced() {
        for (ComponentDscp compDscp : _defDscp._Comps) {
                    if (!compDscp._isPlaced && !compDscp._isFixed) {
                        return false;
                }
        }

        return true;
}

std::string Parser::getMetalWrittenStyle() {
        std::string metal;
        for (TrackDscp track : _defDscp._clsTracks) {
                metal = track._layers[0].substr(0, 5);
        }

        return metal;
}
