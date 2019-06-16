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

#include <random>

#include "IOPlacementKernel.h"
#include "WriterIOPins.h"

#ifdef STANDALONE_MODE
#include "Parser.h"

void IOPlacementKernel::initNetlistAndCore() {
        Parser parser(*_parms, _netlist, _core);
        parser.run();

        std::cout << "Verifying required data\n";
        if (!parser.verifyRequiredData()) {
                std::cout << "*****\nRequired data is not provided by current DEF!\n";
                std::cout << "Check if DEF have tracks and components\n*****\n";
                std::exit(0);
        }

        std::string metal = parser.getMetalWrittenStyle();

        _horizontalMetalLayer =
            metal + std::to_string(_parms->getHorizontalMetalLayer());
        _verticalMetalLayer =
            metal + std::to_string(_parms->getVerticalMetalLayer());


        if (_parms->returnBlockagesFile().size() != 0) {
                _blockagesFile = _parms->returnBlockagesFile();
                parser.getBlockages(_blockagesFile, _blockagesArea);
        }

        if (!parser.isDesignPlaced()) {
                _cellsPlaced = false;
        }
}

IOPlacementKernel::IOPlacementKernel(Parameters& parms) : _parms(&parms) {
        initNetlistAndCore();
        if (_parms->returnHPWL()) {
                _returnHPWL = true;
        }
        if (_parms->returnForceSpread()) {
                _forcePinSpread = true;
        } else {
                _forcePinSpread = false;
        }
        if (_parms->returnNslots() > -1) {
                _slotsPerSection = _parms->returnNslots();
        }
        if (_parms->returnSlotsFactor() > -1) {
                _slotsIncreaseFactor = _parms->returnSlotsFactor();
        }
        if (_parms->returnUsage() > -1) {
                _usagePerSection = _parms->returnUsage();
        }
        if (_parms->returnUsageFactor() > -1) {
                _usageIncreaseFactor = _parms->returnUsageFactor();
        }
        if (_parms->returnRandomMode() > -1) {
                _randomMode = (RandomMode)_parms->returnRandomMode();
        }
}
#endif  // STANDALONE_MODE

void IOPlacementKernel::randomPlacement(const RandomMode mode) {
        unsigned numIOs = _netlist.numIOPins();
        unsigned numSlots = _slots.size();
        unsigned shift = _slots.size() / numIOs;
        unsigned mid1 = numSlots * 1 / 8 - numIOs / 8;
        unsigned mid2 = numSlots * 3 / 8 - numIOs / 8;
        unsigned mid3 = numSlots * 5 / 8 - numIOs / 8;
        unsigned mid4 = numSlots * 7 / 8 - numIOs / 8;
        unsigned idx = 0;
        unsigned slotsPerEdge = numIOs / 4;
        unsigned lastSlots = (numIOs - slotsPerEdge * 3);
        std::vector<int> vSlots(_slots.size());
        std::vector<int> vIOs(numIOs);
        switch (mode) {
                case RandomMode::Full:
                        for (size_t i = 0; i < vSlots.size(); ++i) {
                                vSlots[i] = i;
                        }
                        std::shuffle(vSlots.begin(), vSlots.end(),
                                     std::default_random_engine(42));
                        _netlist.forEachIOPin([&](unsigned idx, IOPin& ioPin) {
                                unsigned b = vSlots[0];
                                ioPin.setPos(_slots.at(b).pos);
                                _assignment.push_back(ioPin);
                                vSlots.erase(vSlots.begin());
                        });
                        break;
                case RandomMode::Even:
                        for (size_t i = 0; i < vIOs.size(); ++i) vIOs[i] = i;
                        std::shuffle(vIOs.begin(), vIOs.end(),
                                     std::default_random_engine(42));
                        _netlist.forEachIOPin([&](unsigned idx, IOPin& ioPin) {
                                unsigned b = vIOs[0];
                                ioPin.setPos(_slots.at(b * shift).pos);
                                _assignment.push_back(ioPin);
                                vIOs.erase(vIOs.begin());
                        });
                        break;
                case RandomMode::Group:
                        for (size_t i = mid1; i < mid1 + slotsPerEdge; i++) {
                                vIOs[idx++] = i;
                        }
                        for (size_t i = mid2; i < mid2 + slotsPerEdge; i++) {
                                vIOs[idx++] = i;
                        }
                        for (size_t i = mid3; i < mid3 + slotsPerEdge; i++) {
                                vIOs[idx++] = i;
                        }
                        for (size_t i = mid4; i < mid4 + lastSlots; i++) {
                                vIOs[idx++] = i;
                        }
                        for (auto i : vIOs) {
                                std::cout << i << std::endl;
                        }
                        std::shuffle(vIOs.begin(), vIOs.end(),
                                     std::default_random_engine(42));
                        _netlist.forEachIOPin([&](unsigned idx, IOPin& ioPin) {
                                unsigned b = vIOs[0];
                                ioPin.setPos(_slots.at(b).pos);
                                _assignment.push_back(ioPin);
                                vIOs.erase(vIOs.begin());
                        });
                        break;
                default:
                        std::cout << "ERROR: Random mode not found\n";
                        exit(-1);
                        break;
        }
}

void IOPlacementKernel::initIOLists() {
        _netlist.forEachIOPin([&](unsigned idx, IOPin& ioPin) {
                std::vector<InstancePin> instPinsVector;
                if (_netlist.numSinksOfIO(idx) != 0) {
                        _netlist.forEachSinkOfIO(
                            idx, [&](InstancePin& instPin) {
                                    instPinsVector.push_back(instPin);
                            });
                        _netlistIOPins.addIONet(ioPin, instPinsVector);
                } else {
                        _zeroSinkIOs.push_back(ioPin);
                }
        });
}

inline bool IOPlacementKernel::checkBlocked(DBU currX, DBU currY) {
        DBU blockedBeginX;
        DBU blockedBeginY;
        DBU blockedEndX;
        DBU blockedEndY;
        for (std::pair<Coordinate, Coordinate> blockage : _blockagesArea) {
                blockedBeginX = std::get<0>(blockage).getX();
                blockedBeginY = std::get<0>(blockage).getY();
                blockedEndX = std::get<0>(blockage).getX();
                blockedEndY = std::get<0>(blockage).getY();
                if (currX >= blockedBeginX)
                        if (currY >= blockedBeginY)
                                if (currX <= blockedEndX)
                                        if (currY <= blockedEndY) return true;
        }
        return false;
}

void IOPlacementKernel::defineSlots() {
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

        DBU totalNumSlots = 0;
        totalNumSlots += (ubX - lbX) * 2 / minDstPinsX;
        totalNumSlots += (ubY - lbY) * 2 / minDstPinsY;

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
                currX = pos.getX();
                currY = pos.getY();
                bool blocked = checkBlocked(currX, currY);
                _slots.push_back({blocked, false, Coordinate(currX, currY)});
                i++;
        }

        for (Coordinate pos : slotsEdge2) {
                currX = pos.getX();
                currY = pos.getY();
                bool blocked = checkBlocked(currX, currY);
                _slots.push_back({blocked, false, Coordinate(currX, currY)});
                i++;
        }

        for (Coordinate pos : slotsEdge3) {
                currX = pos.getX();
                currY = pos.getY();
                bool blocked = checkBlocked(currX, currY);
                _slots.push_back({blocked, false, Coordinate(currX, currY)});
                i++;
        }

        for (Coordinate pos : slotsEdge4) {
                currX = pos.getX();
                currY = pos.getY();
                bool blocked = checkBlocked(currX, currY);
                _slots.push_back({blocked, false, Coordinate(currX, currY)});
                i++;
        }
}

void IOPlacementKernel::createSections() {
        slotVector_t& slots = _slots;
        _sections.clear();
        unsigned numSlots = slots.size();
        unsigned beginSlot = 0;
        unsigned endSlot = 0;
        while (endSlot < numSlots) {
                int blockedSlots = 0;
                endSlot = beginSlot + _slotsPerSection - 1;
                if (endSlot > numSlots) {
                        endSlot = numSlots;
                }
                for (unsigned i = beginSlot; i < endSlot; ++i) {
                        if (slots[i].blocked) {
                                blockedSlots++;
                        }
                }
                unsigned midPoint = (endSlot - beginSlot) / 2;
                Section_t nSec = {slots.at(beginSlot + midPoint).pos};
                if (_usagePerSection > 1.f) {
                        std::cout << "WARNING: section usage exeeded max\n";
                        _usagePerSection = 1.;
                        std::cout << "Forcing slots per section to increase\n";
                        if (_slotsIncreaseFactor != 0.0f) {
                                _slotsPerSection *= (1 + _slotsIncreaseFactor);
                        } else if (_usageIncreaseFactor != 0.0f) {
                                _slotsPerSection *= (1 + _usageIncreaseFactor);
                        } else {
                                _slotsPerSection *= 1.1;
                        }
                }
                nSec.numSlots = endSlot - beginSlot - blockedSlots;
                if (nSec.numSlots < 0) {
                        std::cout << "ERROR: negative number of slots\n";
                        exit(-1);
                }
                nSec.beginSlot = beginSlot;
                nSec.endSlot = endSlot;
                nSec.maxSlots = nSec.numSlots * _usagePerSection;
                nSec.curSlots = 0;
                _sections.push_back(nSec);
                beginSlot = ++endSlot;
        }
}

bool IOPlacementKernel::assignPinsSections() {
        Netlist& net = _netlistIOPins;
        sectionVector_t& sections = _sections;
        createSections();
        int totalPinsAssigned = 0;
        net.forEachIOPin([&](unsigned idx, IOPin& ioPin) {
                bool pinAssigned = false;
                std::vector<DBU> dst(sections.size());
                std::vector<InstancePin> instPinsVector;
#pragma omp parallel for
                for (unsigned i = 0; i < sections.size(); i++) {
                        dst[i] = net.computeDstIOtoPins(idx, sections[i].pos);
                }
                net.forEachSinkOfIO(idx, [&](InstancePin& instPin) {
                        instPinsVector.push_back(instPin);
                });
                for (auto i : sort_indexes(dst)) {
                        if (sections[i].curSlots < sections[i].maxSlots) {
                                sections[i].net.addIONet(ioPin, instPinsVector);
                                sections[i].curSlots++;
                                pinAssigned = true;
                                totalPinsAssigned++;
                                break;
                        }
                        // Try to add pin just to first
                        if (not _forcePinSpread) break;
                }
                if (!pinAssigned) {
                        return;  // "break" forEachIOPin
                }
        });
        // if forEachIOPin ends or returns/breaks goes here
        if (totalPinsAssigned == net.numIOPins()) {
                return true;
        } else {
                return false;
        }
}

void IOPlacementKernel::printConfig() {
        std::cout << "Slots Per Section     " << _slotsPerSection << "\n";
        std::cout << "Slots Increase Factor " << _slotsIncreaseFactor << "\n";
        std::cout << "Usage Per Section     " << _usagePerSection << "\n";
        std::cout << "Usage Increase Factor " << _usageIncreaseFactor << "\n";
        std::cout << "Force Pin Spread      " << _forcePinSpread << "\n\n";
}

void IOPlacementKernel::setupSections() {
        bool allAssigned;
        unsigned i = 0;
        if (!(_slotsPerSection > 1)) {
                std::cout << "_slotsPerSection must be grater than one\n";
                exit(1);
        }
        if (!(_usagePerSection > 0.0f)) {
                std::cout << "_usagePerSection must be grater than zero\n";
                exit(1);
        }
        if (not _forcePinSpread && _usageIncreaseFactor == 0.0f &&
            _slotsIncreaseFactor == 0.0f) {
                std::cout << "WARNING: if _forcePinSpread = false than either "
                             "_usageIncreaseFactor or _slotsIncreaseFactor "
                             "must be != 0\n";
        }
        do {
                std::cout << "Tentative " << i++ << " to setup sections\n";
                printConfig();

                allAssigned = assignPinsSections();

                _usagePerSection *= (1 + _usageIncreaseFactor);
                _slotsPerSection *= (1 + _slotsIncreaseFactor);
                if (_sections.size() > MAX_SECTIONS_RECOMMENDED) {
                        std::cout
                            << "WARNING: number of sections is "
                            << _sections.size()
                            << " while the maximum recommended value is "
                            << MAX_SECTIONS_RECOMMENDED
                            << " this may negatively affect performance\n";
                }
                if (_slotsPerSection > MAX_SLOTS_RECOMMENDED) {
                        std::cout
                            << "WARNING: number of slots per sections is "
                            << _slotsPerSection
                            << " while the maximum recommended value is "
                            << MAX_SLOTS_RECOMMENDED
                            << " this may negatively affect performance\n";
                }
        } while (not allAssigned);
}

inline void IOPlacementKernel::updateOrientation(IOPin& pin) {
        const DBU x = pin.getX();
        const DBU y = pin.getY();
        DBU lowerXBound = _core.getLowerBound().getX();
        DBU lowerYBound = _core.getLowerBound().getY();
        DBU upperXBound = _core.getUpperBound().getX();
        DBU upperYBound = _core.getUpperBound().getY();

        if (x == lowerXBound) {
                if (y == upperYBound) {
                        pin.setOrientation(Orientation::SOUTH);
                        return;
                } else {
                        pin.setOrientation(Orientation::EAST);
                        return;
                }
        }
        if (x == upperXBound) {
                if (y == lowerYBound) {
                        pin.setOrientation(Orientation::NORTH);
                        return;
                } else {
                        pin.setOrientation(Orientation::WEST);
                        return;
                }
        }
        if (y == lowerYBound) {
                pin.setOrientation(Orientation::NORTH);
                return;
        }
        if (y == upperYBound) {
                pin.setOrientation(Orientation::SOUTH);
                return;
        }
}

inline void IOPlacementKernel::updatePinArea(IOPin& pin) {
        const DBU x = pin.getX();
        const DBU y = pin.getY();
        DBU lowerXBound = _core.getLowerBound().getX();
        DBU lowerYBound = _core.getLowerBound().getY();
        DBU upperXBound = _core.getUpperBound().getX();
        DBU upperYBound = _core.getUpperBound().getY();

        if (x == lowerXBound || x == upperXBound) {
//                int halfWidth = int( ceil((curLayer->width() * env.defDbu)/2.0) + 0.5f);
//                int height = (curLayer->hasArea())?
//                int( ceil((curLayer->area() * env.defDbu * env.defDbu)/(2.0*halfWidth)) + 0.5f) :
//                        2 * halfWidth;
//                curPin.addLayerPts( -1*halfWidth, 0, halfWidth, height );
                DBU halfWidth = DBU (ceil(_core.getMinWidthX()/2.0));
                DBU height = _core.getMinAreaX() != 0.0 ?
                        DBU (ceil(_core.getMinAreaX() / (2.0*halfWidth))) :
                        2 * halfWidth;
                pin.setLowerBound(-1*halfWidth, 0);
                pin.setUpperBound(halfWidth, height);
        }


        if (y == lowerYBound || y == upperYBound) {
//                int halfWidth = int( ceil((curLayer->width() * env.defDbu)/2.0) + 0.5f);
//                int height = (curLayer->hasArea())?
//                int( ceil((curLayer->area() * env.defDbu * env.defDbu)/(2.0*halfWidth)) + 0.5f) :
//                        2 * halfWidth;
//                curPin.addLayerPts( -1*halfWidth, 0, halfWidth, height );
                DBU halfWidth = DBU (ceil(_core.getMinWidthY()/2.0));
                DBU height = _core.getMinAreaY() != 0.0 ?
                        DBU (ceil(_core.getMinAreaY() / (2.0*halfWidth))) :
                        2 * halfWidth;
                pin.setLowerBound(-1*halfWidth, 0);
                pin.setUpperBound(halfWidth, height);
        }
}

DBU IOPlacementKernel::returnIONetsHPWL(Netlist& netlist) {
        unsigned pinIndex = 0;
        DBU hpwl = 0;
        netlist.forEachIOPin([&](unsigned idx, IOPin& ioPin) {
                hpwl += netlist.computeIONetHPWL(idx, ioPin.getPosition());
                pinIndex++;
        });

        return hpwl;
}

void IOPlacementKernel::run() {
        std::vector<HungarianMatching> hgVec;

        initIOLists();
        defineSlots();

        setupSections();

        if (_returnHPWL) {
                std::cout << "***HPWL before IOPlacement: "
                          << returnIONetsHPWL(_netlist) << "***\n";
        }
        if (_cellsPlaced) {
                for (unsigned idx = 0; idx < _sections.size(); idx++) {
                        if (_sections[idx].net.numIOPins() > 0) {
                                HungarianMatching hg(_sections[idx], _slots);
                                hgVec.push_back(hg);
                        }
                }

#pragma omp parallel for
                for (unsigned idx = 0; idx < hgVec.size(); idx++) {
                        hgVec[idx].run();
                }

                for (unsigned idx = 0; idx < hgVec.size(); idx++) {
                        hgVec[idx].getFinalAssignment(_assignment);
                }

                /* TODO:  <28-05-19, for some reason the first 3 slots/rows
                 * always have overlap violations if used > */
                unsigned i = 3;
                while (_zeroSinkIOs.size() > 0 && i < _slots.size()) {
                        if (not _slots[i].used && not _slots[i].blocked) {
                                _slots[i].used = true;
                                _zeroSinkIOs[0].setPos(_slots[i].pos);
                                _assignment.push_back(_zeroSinkIOs[0]);
                                _zeroSinkIOs.erase(_zeroSinkIOs.begin());
                        }
                        i++;
                }
        } else {
                std::cout << "WARNING: cells are not placed, running random\n";
                randomPlacement(_randomMode);
        }
#pragma omp parallel for
        for (unsigned i = 0; i < _assignment.size(); ++i) {
                updateOrientation(_assignment[i]);
                updatePinArea(_assignment[i]);
        }

        if (_returnHPWL) {
                DBU totalHPWL = 0;
#pragma omp parallel for reduction(+ : totalHPWL)
                for (unsigned idx = 0; idx < _sections.size(); idx++) {
                        totalHPWL += returnIONetsHPWL(_sections[idx].net);
                }
                std::cout << "***HPWL after IOPlacement: " << totalHPWL
                          << "***\n";
        }
}

void IOPlacementKernel::getResults() {
        WriterIOPins writer(_netlistIOPins, _assignment, _horizontalMetalLayer,
                            _verticalMetalLayer, _parms->getInputDefFile(),
                            _parms->getOutputDefFile());

        writer.run();
}
