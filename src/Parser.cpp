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
#include "Parser.h"

Parser::Parser(Parameters& parms, Netlist& netlist, Core& core)
    : _parms(&parms), _netlist(&netlist), _core(&core) {}

void Parser::run() {
        _defParser.parseDEF(_parms->getInputDefFile(), _defDscp);
		initMapIOtoNet();
		initMapInstToPosition();
        readDieArea();
        readConnections();
        initNetlist();
        initCore();
}

Parser::point Parser::getInstPosition(std::string instName) {
        point position = point(-1, -1);
        for (ComponentDscp comp : _defDscp.Comps) {
                if (instName == comp.name) {
                        DBU x = comp.position.getX();
                        DBU y = comp.position.getY();
                        position = point(x, y);
                        break;
                }
        }
        return position;
}

void Parser::readDieArea() {
        point lower = point(_defDscp.clsDieBounds.getLowerBound().getX(),
                            _defDscp.clsDieBounds.getLowerBound().getY());
        point upper = point(_defDscp.clsDieBounds.getUpperBound().getX(),
                            _defDscp.clsDieBounds.getUpperBound().getY());

        _dieArea = box(lower, upper);
}  // end method

void Parser::readConnections() {
        int ioCounter = -1;
        for (IOPinDscp io : _defDscp.IOPins) {
				ioPin pin;
                pin.name = io.name;
                pin.position = point(io.position.getX(), io.position.getY());
                pin.netName = io.netName;
                pin.bounds = box(point(io.layerBounds.getLowerBound().getX(),
                                       io.layerBounds.getLowerBound().getY()),
                                 point(io.layerBounds.getUpperBound().getX(),
                                       io.layerBounds.getUpperBound().getY()));
                pin.direction = io.direction;
                _ioPins.push_back(pin);
                ioCounter++;

                NetDscp net = mapIOPinToNet[io.name];
                for (NetConnection conn : net.connections) {
				        if (conn.componentName == "PIN") {
							continue;
						}

                        cellPin cPin;
                        cPin.name = conn.componentName + ":" +
							conn.pinName;

                        cPin.position = mapInstToPosition[conn.componentName];
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
                _netlist->addIONet(ioPin, instPins);
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

        for (TrackDscp track : _defDscp.clsTracks) {
                if (track.layers[0] == "Metal"+std::to_string(_parms->getHorizontalMetalLayer())) {
                        if (track.direction == "X") {
                                minSpacingX = track.space;
                                initTrackX = track.location;
                        } else if (track.direction == "Y") {
                                minSpacingY = track.space;
                                initTrackY = track.location;
                        }
                }
        }

        *_core = Core(lowerBound, upperBound, minSpacingX, minSpacingY,
                      initTrackX, initTrackY);
}

void Parser::initMapIOtoNet () {
		for (NetDscp net : _defDscp.Nets) {
                for (NetConnection conn : net.connections) {
                        if (conn.componentName != "PIN")
                                continue;

                        mapIOPinToNet[conn.pinName] = net;
                }
        }
		std::cout << "Map size: " << mapIOPinToNet.size() << "\n";
}

void Parser::initMapInstToPosition() {
		for (ComponentDscp comp : _defDscp.Comps) {
			point p = point(comp.position.getX(), comp.position.getY());
			mapInstToPosition[comp.name] = p;
		}
}