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

#include <random>

IOPlacementKernel::IOPlacementKernel(Parameters& parms) : _parms(&parms) {}

void IOPlacementKernel::randomPlacement(std::vector<IOPin>& assignment) {
        static const int kMaxValue = _slots.size() - 1;
        std::vector<int> v(kMaxValue + 1);
        for (size_t i = 0; i < v.size(); ++i) v[i] = i;
        std::shuffle(v.begin(), v.end(), std::default_random_engine(42));
        _netlist.forEachIOPin([&](unsigned idx, IOPin& ioPin) {
                unsigned b = v[0];
                ioPin.setPos(_slots.at(b).pos);
                assignment.push_back(ioPin);
                v.erase(v.begin());
        });
}

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
                        _netlist.forEachSinkOfIO(idx,
                                                 [&](InstancePin& instPin) {
                                instPinsVector.push_back(instPin);
                        });
                        _netlistIOPins.addIONet(ioPin, instPinsVector);
                } else {
                        _zeroSinkIOs.push_back(ioPin);
                }
        });
}

void IOPlacementKernel::defineSlots() {
        Netlist& netlist = _netlistIOPins;
        Coordinate lb = _core.getLowerBound();
        Coordinate ub = _core.getUpperBound();
        DBU lbX = lb.getX();
        DBU lbY = lb.getY();
        DBU ubX = ub.getX();
        DBU ubY = ub.getY();
        unsigned minDstPinsX = _core.getMinDstPinsX();
        unsigned minDstPinsY = _core.getMinDstPinsY();
        unsigned initTracksX = _core.getInitTracksX();
        unsigned initTracksY = _core.getInitTracksY();

        bool use = false;
        DBU totalNumSlots = 0;
        totalNumSlots += (ubX - lbX) * 2 / minDstPinsX;
        totalNumSlots += (ubY - lbY) * 2 / minDstPinsY;

        unsigned numPins = netlist.numIOPins();

        unsigned interval = std::floor(totalNumSlots / (getKValue() * numPins));
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

        std::vector<Coordinate> slotsEdge1;
        DBU currX = lb.getX() + initTracksX;
        DBU currY = lb.getY();

        while (currX < ub.getX()) {
                Coordinate pos(currX, currY);
                slotsEdge1.push_back(pos);
                currX += minDstPinsX;
        }

        std::vector<Coordinate> slotsEdge2;
        currY = lb.getY() + initTracksY;
        currX = ub.getX();
        while (currY < ub.getY()) {
                Coordinate pos(currX, currY);
                slotsEdge2.push_back(pos);
                currY += minDstPinsY;
        }

        std::vector<Coordinate> slotsEdge3;
        currX = lb.getX() + initTracksX;
        currY = ub.getY();
        while (currX < ub.getX()) {
                Coordinate pos(currX, currY);
                slotsEdge3.push_back(pos);
                currX += minDstPinsX;
        }
        std::reverse(slotsEdge3.begin(), slotsEdge3.end());

        std::vector<Coordinate> slotsEdge4;
        currY = lb.getY() + initTracksY;
        currX = lb.getX();
        while (currY < ub.getY()) {
                Coordinate pos(currX, currY);
                slotsEdge4.push_back(pos);
                currY += minDstPinsY;
        }
        std::reverse(slotsEdge4.begin(), slotsEdge4.end());

        int i = 0;
        for (Coordinate pos : slotsEdge1) {
                if (i % interval) {
                        use = false;
                } else {
                        use = true;
                }
                currX = pos.getX();
                currY = pos.getY();
                _slots.push_back({use, false, Coordinate(currX, currY)});
                i++;
        }
        for (Coordinate pos : slotsEdge2) {
                if (i % interval) {
                        use = false;
                } else {
                        use = true;
                }

                currX = pos.getX();
                currY = pos.getY();
                _slots.push_back({use, false, Coordinate(currX, currY)});
                i++;
        }

        for (Coordinate pos : slotsEdge3) {
                if (i % interval) {
                        use = false;
                } else {
                        use = true;
                }

                currX = pos.getX();
                currY = pos.getY();
                _slots.push_back({use, false, Coordinate(currX, currY)});
                i++;
        }

        for (Coordinate pos : slotsEdge4) {
                if (i % interval) {
                        use = false;
                } else {
                        use = true;
                }

                currX = pos.getX();
                currY = pos.getY();
                _slots.push_back({use, false, Coordinate(currX, currY)});
                i++;
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
        std::vector<IOPin> assignment;

        initNetlistAndCore();
        initIOLists();
        defineSlots();
        _horizontalMetalLayer = _parms->getHorizontalMetalLayer();
        _verticalMetalLayer = _parms->getVerticalMetalLayer();

        bool random = false;
        if (random) {
                randomPlacement(assignment);
                /* TODO:  <25-04-19, remove hard coded> */
                std::cout << "!!!WARNING!!! hard coded run of random"
                          << std::endl;
        } else {
                HungarianMatching hgMatching(_netlistIOPins, _core, _slots);
                hgMatching.run();
                hgMatching.getFinalAssignment(assignment, _zeroSinkIOs);
        }

        for (IOPin& ioPin : assignment) {
                Orientation orient = checkOrientation(
                    ioPin.getX(), ioPin.getY(), ioPin.getOrientation());
                ioPin.setOrientation(orient);
        }

        WriterIOPins writer(_netlistIOPins, assignment, _horizontalMetalLayer,
                            _verticalMetalLayer, _parms->getInputDefFile(),
                            _parms->getOutputDefFile());

        writer.run();
}
