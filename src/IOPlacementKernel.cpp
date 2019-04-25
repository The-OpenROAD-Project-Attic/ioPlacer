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

#include "IOPlacementKernel.h"
#include "Parser.h"
#include "WriterIOPins.h"

IOPlacementKernel::IOPlacementKernel(Parameters& parms) : _parms(&parms) {}

void IOPlacementKernel::initNetlistAndCore() {
        Parser parser(*_parms, _netlist, _core);
        parser.run();
}

void IOPlacementKernel::initIOLists() {
        _netlist.forEachIOPin([&](unsigned idx, IOPin& ioPin) {
                std::vector<InstancePin> instPinsVector;
                /* TODO:  <23-04-19, do we need this check to remove pins
                 * without sinks? TBD > */
                if (_netlist.numSinksOfIO(idx) != 0) {
                        _netlist.forEachSinkOfIO(
                            idx, [&](InstancePin& instPin) {
                                    instPinsVector.push_back(instPin);
                            });
                        _netlistIOPins.addIONet(ioPin, instPinsVector);
                }
        });
}

void IOPlacementKernel::defineSlots() {
        Coordinate lb = _core.getLowerBound();
        Coordinate ub = _core.getUpperBound();
        unsigned minDstPinsX = _core.getMinDstPinsX();
        unsigned minDstPinsY = _core.getMinDstPinsY();
        unsigned initTracksX = _core.getInitTracksX();
        unsigned initTracksY = _core.getInitTracksY();

        bool use = false;
        bool firstRight = true;
        bool firstUp = true;
        bool firstLeft = true;
        DBU currX = lb.getX() + initTracksX;
        DBU currY = lb.getY();
        DBU totalNumSlots = 0;
        totalNumSlots += (ub.getX() - lb.getX()) * 2 / minDstPinsX;
        totalNumSlots += (ub.getY() - lb.getY()) * 2 / minDstPinsY;
        unsigned numPins = _netlist.numIOPins();

        unsigned interval = std::floor(totalNumSlots / getKValue() * numPins);
        if (std::floor(totalNumSlots / interval) <= numPins) {
                interval = std::floor(totalNumSlots / numPins);
        }

        /*******************************************
         * How the for bellow follows core boundary *
         ********************************************
         *                 <----                    *
         *                                          *
         *                 3st edge     upperBound  *
         *           *------------------x           *
         *           |                  |           *
         *   |       |                  |      ^    *
         *   |  4th  |                  | 2nd  |    *
         *   |  edge |                  | edge |    *
         *   V       |                  |      |    *
         *           |                  |           *
         *           x------------------*           *
         *   lowerBound    1st edge                 *
         *                 ---->                    *
         *******************************************/
        for (unsigned i = 0; i < totalNumSlots; i++) {
                if (i % interval) {
                        use = false;
                } else {
                        use = true;
                }
                _slots.push_back({use, false, Coordinate(currX, currY)});
                // get slots for 1st edge
                if (currX < ub.getX() && currY == lb.getY()) {
                        currX += minDstPinsX;
                }
                // get slots for 2nd edge
                else if (currY < ub.getY() && currX >= ub.getX()) {
                        if (firstRight) {
                                currX = ub.getX();
                                currY += initTracksY;
                                firstRight = false;
                        } else {
                                currX = ub.getX();
                                currY += minDstPinsY;
                        }
                }
                // get slots for 3rd edge
                else if (currX > lb.getX()) {
                        if (firstUp) {
                                currY = ub.getY();
                                currX -= initTracksX;
                                firstUp = false;
                        } else {
                                currY = ub.getY();
                                currX -= minDstPinsX;
                        }
                }
                // get slots for 4th  edge
                else if (currY > lb.getY()) {
                        if (firstLeft) {
                                currX = lb.getX();
                                currY -= initTracksY;
                                firstLeft = false;
                        } else {
                                currX = lb.getX();
                                currY -= minDstPinsY;
                        }
                }
                // is at the lowerBound again, break loop
                else if (currX < lb.getX() && currY == lb.getY()) {
                        break;
                }
        }
}

inline Orientation IOPlacementKernel::checkOrientation(
    const DBU x, const DBU y, Orientation currentOrient) {
        DBU lowerXBound = _core.getLowerBound().getX();
        DBU lowerYBound = _core.getLowerBound().getY();
        DBU upperXBound = _core.getUpperBound().getX();
        DBU upperYBound = _core.getUpperBound().getY();

        if (x == lowerXBound) {
                if (y == upperYBound) return Orientation::SOUTH;
                return Orientation::EAST;
        }
        if (x == upperXBound) {
                if (y == lowerYBound) return Orientation::NORTH;
                return Orientation::WEST;
        }
        if (y == lowerYBound) return Orientation::NORTH;
        if (y == upperYBound) return Orientation::SOUTH;

        return currentOrient;
}

void IOPlacementKernel::run() {
        initNetlistAndCore();

#ifdef DEBUG
        _netlist.forEachIOPin([&](unsigned idx, const IOPin& ioPin) {
                std::cout << "IO Pin: " << ioPin.getName() << "\n";
                std::cout << "N Pins: " << _netlist.numSinksOfIO(idx) << "\n";
                _netlist.forEachSinkOfIO(idx, [&](const InstancePin& instPin) {
                        std::cout << "\tinstPin " << instPin.getName() << "\n";
                });
        });
#endif

        initIOLists();
        defineSlots();
        HungarianMatching hgMatching(_netlistIOPins, _core, _slots);
        hgMatching.run();

        std::vector<std::tuple<unsigned, Coordinate>> assignment;
        hgMatching.getFinalAssignment(assignment);

        _netlistIOPins.forEachIOPin([&](unsigned idx, IOPin& ioPin) {
                DBU _x = 0, _y = 0;
                for (std::tuple<unsigned, Coordinate> pinAssignment :
                     assignment) {
                        if (idx == std::get<0>(pinAssignment)) {
                                _x = std::get<1>(pinAssignment).getX();
                                _y = std::get<1>(pinAssignment).getY();
                                break;
                        }
                }
                Orientation orient =
                    checkOrientation(_x, _y, ioPin.getOrientation());
                ioPin.setOrientation(orient);
        });

        WriterIOPins writer(_netlistIOPins, assignment,
                            _parms->getOutputDefFile());

        writer.run();
}
