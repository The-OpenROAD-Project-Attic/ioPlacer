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

#include "IOPlacement.h"
#include "IOPlacementKernel.h"
#include "Coordinate.h"
#include <vector>

namespace ioPlacer {

IOPlacementKernel ioKernel;

void IOPlacement::initCore(point lowerBounds, point upperBounds,
                           DBU minSpacingX, DBU minSpacingY, DBU initTrackX,
                           DBU initTrackY) {
        Coordinate lowerBound(lowerBounds.x(), lowerBounds.y());
        Coordinate upperBound(upperBounds.x(), upperBounds.y());
        ioKernel._core = Core(lowerBound, upperBound, minSpacingX, minSpacingY,
                              initTrackX, initTrackY);
}

void IOPlacement::setMetalLayers(int horizontalMetalLayer,
                                 int verticalMetalLayer) {
        ioKernel._horizontalMetalLayer =
            "Metal" + std::to_string(horizontalMetalLayer);
        ioKernel._verticalMetalLayer =
            "Metal" + std::to_string(verticalMetalLayer);
}

void IOPlacement::addIOPin(std::string name, std::string netName, box bounds,
                           std::string direction) {
        ioPin pin;
        pin.name = name;
        pin.position = point(0, 0);
        pin.netName = netName;
        pin.bounds = bounds;
        pin.direction = direction;
        _ioPins.push_back(pin);
        size_t idx = _ioPins.size() - 1;
        _mapNetToIo[netName].push_back(idx);
}

void IOPlacement::addInstPin(std::string net, std::string pinName, point pos) {
        cellPin pin;
        pin.name = pinName;
        pin.position = pos;

        std::vector<size_t>& index = _mapNetToIo[net];
        for (size_t& i : index) {
                _ioPins[i].connections.push_back(pin);
        }
}

void IOPlacement::addBlockage(point initialPos, point finalPos) {
        DBU initialX = initialPos.x();
        DBU initialY = initialPos.y();
        DBU finalX = finalPos.x();
        DBU finalY = finalPos.y();
        Coordinate coreLowerBound = ioKernel._core.getLowerBound();
        Coordinate coreUpperBound = ioKernel._core.getUpperBound();
        if (initialX != finalX && initialY != finalY) {
                std::cout << "ERROR: Blockage should consider only one edge\n";
                exit(-1);
        }
        initialX = std::max(initialX, coreLowerBound.getX());
        initialY = std::max(initialY, coreLowerBound.getY());
        finalX = std::min(finalX, coreUpperBound.getX());
        finalY = std::min(finalY, coreUpperBound.getY());
        Coordinate initialCoord(initialX, initialY);
        Coordinate finalCoord(finalX, finalY);
        std::pair<Coordinate, Coordinate> block(initialCoord, finalCoord);
        ioKernel._blockagesArea.push_back(block);
}

void IOPlacement::initNetlist() {
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
                ioKernel._netlist.addIONet(ioPin, instPins);
                io.connections.clear();
        }
        _ioPins.clear();
}

char IOPlacement::getOrientationString(int orient) {
        if (orient == SOUTH) return 'S';
        if (orient == NORTH) return 'N';
        if (orient == EAST) return 'E';
        return 'W';
}

void IOPlacement::getResults(std::vector<Pin_t>& pinAssignment) {
        for (IOPin& io : ioKernel._assignment) {
                Pin_t p;
                p.name = io.getName();
                p.pos = point(io.getX(), io.getY());
                const Orientation& orient = io.getOrientation();
                p.orientation = getOrientationString(orient);
                pinAssignment.push_back(p);
        }
}

void IOPlacement::setSlotsPerSection(unsigned slotsPerSection,
                                     float increaseFactor) {
        ioKernel._slotsPerSection = slotsPerSection;
        ioKernel._slotsIncreaseFactor = increaseFactor;
}

void IOPlacement::setSlotsUsagePerSection(float usagePerSection,
                                          float increaseFactor) {
        ioKernel._usagePerSection = usagePerSection;
        ioKernel._usageIncreaseFactor = increaseFactor;
}

void IOPlacement::forcePinSpread(bool force) {
        ioKernel._forcePinSpread = force;
}

void IOPlacement::setRandomMode(int randomMode) {
        ioKernel._randomMode = (RandomMode)randomMode;
}

std::vector<Pin_t> IOPlacement::run(bool returnHPWL) {
        initNetlist();
        ioKernel._returnHPWL = returnHPWL;
        ioKernel.run();
        std::vector<Pin_t> pinAssignment;
        getResults(pinAssignment);
        return pinAssignment;
}

}  // namespace ioPlacer
