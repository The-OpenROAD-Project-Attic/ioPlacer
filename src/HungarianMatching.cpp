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
        defineSlots();
        createMatrix();
        for (int i = 0; i < 3; ++i) {
                _hungarianSolver.solve(_hungarianMatrix);
                updateNeighborhood(false);
                createMatrix();
        }
        updateNeighborhood(true);
#ifdef DEBUG
        /* print final assignment, i.e., pin index and position */
        assignmentVec_t v;
        unsigned name;
        Coordinate coor(0, 0);
        getFinalAssignment(v);
        for (auto i : v) {
                name = std::get<0>(i);
                coor = std::get<1>(i);
                std::cout << name << ": (" << coor.getX() << ", " << coor.getY()
                          << ")\n";
        }
        std::cout << _hungarianMatrix.rows() << ','
                  << _hungarianMatrix.columns() << "\n";
#endif
}

void HungarianMatching::defineSlots() {
        Coordinate lb = _core->getLowerBound();
        Coordinate ub = _core->getUpperBound();
        unsigned minDstPinsX = _core->getMinDstPinsX();
        unsigned minDstPinsY = _core->getMinDstPinsY();
        unsigned initTracksX = _core->getInitTracksX();
        unsigned initTracksY = _core->getInitTracksY();

        bool use = false;
        bool firstRight = true;
        bool firstUp = true;
        bool firstLeft = true;
        DBU currX = lb.getX() + initTracksX;
        DBU currY = lb.getY();
        DBU totalNumSlots = 0;
        totalNumSlots += (ub.getX() - lb.getX()) * 2 / minDstPinsX;
        totalNumSlots += (ub.getY() - lb.getY()) * 2 / minDstPinsY;
        unsigned numPins = getNumIOPins();

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
                        _numSlots++;
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

void HungarianMatching::createMatrix() {
        _numSlots = 0;

        for (auto i : _slots) {
                if (i.current && i.visited) {
                        i.current = false;
                        std::cout << "here\n";
                } else if (i.current) {
                        _numSlots++;
                }
        }

        _hungarianMatrix = Matrix<DBU>(getNumIOPins(), _numSlots);

        unsigned slotIndex = 0;
        for (auto i : _slots) {
                unsigned pinIndex = 0;
                if (i.current && i.visited) {
                        i.current = false;
                } else if (not i.current) {
                        continue;
                }
                Coordinate newPos = i.pos;
                _netlist->forEachIOPin([&](unsigned idx, IOPin& ioPin) {
                        DBU hpwl = _netlist->computeIONetHPWL(idx, newPos);
                        _hungarianMatrix(pinIndex, slotIndex) = hpwl;
                        pinIndex++;
                });
                slotIndex++;
        }
}

bool HungarianMatching::updateNeighborhood(bool last_pass) {
        Coordinate pos(0, 0);
        bool will_remove;
        std::vector<unsigned> to_remove;
        std::vector<unsigned> to_explore;
        /* TODO:  <23-04-19, transpose io pins and slots in matrix> */
        for (size_t col = 0; col < _hungarianMatrix.columns(); col++) {
                will_remove = true;
                for (size_t row = 0; row < _hungarianMatrix.rows(); row++) {
                        if (_hungarianMatrix(row, col) == 0) {
                                will_remove = false;
                                break;
                        }
                }
                if (will_remove) {
                        to_remove.push_back(col);
                } else {
                        to_explore.push_back(col);
                }
        }
        markRemove(to_remove);
        if (last_pass) {
                markExplore(to_explore);
        }
        return false;
}

void HungarianMatching::markExplore(std::vector<unsigned> v) {
        unsigned curr = 0;
        for (unsigned i = 0; i < _slots.size(); ++i) {
                if (_slots.at(i).current) {
                        if (v.size() > 0 && v.at(0) == curr) {
                                if (i > 1 && not _slots.at(i - 1).visited) {
                                        if (not _slots.at(i - 1).current) {
                                                _slots.at(i - 1).current = true;
                                                _numSlots++;
                                        }
                                }
                                if (i < _slots.size() &&
                                    not _slots.at(i + 1).visited) {
                                        if (not _slots.at(i + 1).current) {
                                                _slots.at(i + 1).current = true;
                                                _numSlots++;
                                        }
                                }
                                v.erase(v.begin());
                        }
                        curr++;
                }
        }
}

void HungarianMatching::markRemove(std::vector<unsigned> v) {
        unsigned curr = 0;
        for (auto i : _slots) {
                if (i.current) {
                        if (v.size() > 0 && v.at(0) == curr) {
                                i.visited = true;
                                v.erase(v.begin());
                                _numSlots--;
                        }
                        curr++;
                }
        }
}

void HungarianMatching::getFinalAssignment(assignmentVec_t& v) {
        unsigned idx = 0;
        for (auto i : _slots) {
                if (i.current && not i.visited) {
                        v.push_back(
                            std::tuple<unsigned, Coordinate>(idx++, i.pos));
                }
        }
}
