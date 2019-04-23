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

#include "HungarianMatching.h"

HungarianMatching::HungarianMatching(Netlist& netlist, Core& core) {
        _netlist = &netlist;
        _core = &core;
}

void HungarianMatching::run() {
        initIOLists();
        defineSlots();
        createMatrix();

        hungarianSolver.solve(hungarianMatrix);
}

void HungarianMatching::defineSlots() {
        Coordinate lb = _core->getLowerBound();
        Coordinate ub = _core->getUpperBound();
        DBU corePerimeter = _core->getPerimeter();
        unsigned int minDstPins = _core->getMinDstPins();
        slotVector_t* slots = _slots;

        DBU numSlots = corePerimeter / minDstPins;
        DBU currX = lb.getX() + minDstPins;
        DBU currY = lb.getY();

        int numPins = getNumIOPins();
        int interval = std::floor(numSlots / double(getKValue() * numPins));

        if (std::floor(numSlots / double(interval)) >= numPins) {
                interval = std::floor(numSlots / double(numPins));
        }

        bool use = false;

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
        for (unsigned int i = 0; i < numSlots; i++) {
                if (i % interval) {
                        use = false;
                } else {
                        use = true;
                }
                slots->push_back(std::tuple<bool, bool, Coordinate>(
                    use, false, Coordinate(currX, currY)));
                // get slots for 1st edge
                if (currX < ub.getX() && currY == lb.getY()) {
                        currX += minDstPins;
                }
                // get slots for 2nd edge
                else if (currY < ub.getY() && currX >= ub.getX()) {
                        currX = ub.getX();
                        currY += minDstPins;
                }
                // get slots for 3rd edge
                else if (currX > lb.getX()) {
                        currY = ub.getY();
                        currX -= minDstPins;
                }
                // get slots for 4th  edge
                else if (currY > lb.getY()) {
                        currX = lb.getX();
                        currY -= minDstPins;
                }
        }
}

int HungarianMatching::getKValue() { return 1; }

void HungarianMatching::initIOLists() {
        _netlist->forEachIOPin([&](unsigned idx, IOPin& ioPin) {
                std::vector<InstancePin> instPinsVector;
                /* TODO:  <23-04-19, do we need this check to remove pins
                 * without sinks? TBD > */
                if (_netlist->numSinksOfIO(idx) != 0) {
                        _netlist->forEachSinkOfIO(
                            idx, [&](InstancePin& instPin) {
                                    instPinsVector.push_back(instPin);
                            });
                        netlistIOPins.addIONet(ioPin, instPinsVector);
                }
        });
}

int HungarianMatching::getNumIOPins() { return netlistIOPins.numIOPins(); }

void HungarianMatching::createMatrix() {
        hungarianMatrix = Matrix<DBU>(getNumIOPins(), numSlots);

        for (int i = 0; i < numSlots; i++) {
                int pinIndex = 0;
                int y = 0;
                DBU slotBeginning = slotSize * i;
                DBU halfSlot = slotBeginning + std::floor(slotSize / double(2));
                Coordinate coreLowerBounds = _core->getLowerBound();
                Coordinate coreUpperBounds = _core->getUpperBound();
                int x = halfSlot;
                if (x > coreUpperBounds.getX()) {
                        y = x - coreUpperBounds.getX();
                        x = coreUpperBounds.getX();
                        if (y > coreUpperBounds.getY()) {
                                x = x - (y - coreUpperBounds.getY());
                                y = coreUpperBounds.getY();
                                if (x < coreLowerBounds.getX()) {
                                        y = y - std::abs(
                                                    coreLowerBounds.getX() - x);
                                        x = coreLowerBounds.getX();
                                }
                        }
                }

                Coordinate newPos = Coordinate(x, y);

                netlistIOPins.forEachIOPin([&](unsigned idx, IOPin& ioPin) {
                        DBU hpwl = netlistIOPins.computeIONetHPWL(idx, newPos);
                        hungarianMatrix(pinIndex, i) = hpwl;
                        pinIndex++;
                });
        }
}
