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
#include <istream>
#include <fstream>

HungarianMatching::HungarianMatching(Netlist& netlist, Core& core,
                                     slotVector_t& v) {
        _netlist = &netlist;
        _core = &core;
        _slots = &v;
        _numIOPins = _netlist->numIOPins();
}

void HungarianMatching::run() {
        bool stable = false;
        unsigned iterr = 4;
        while (not stable && iterr > 0) {
                iterr--;
                createMatrix();
                _hungarianSolver.solve(_hungarianMatrix);
                stable = updateNeighborhood(stable);
        }
        updateNeighborhood(stable);
}

void HungarianMatching::setNumSlots() {
        _numSlots = 0;
        for (auto i : *_slots) {
                if (i.current) {
                        if (i.visited) {
                                i.current = false;
                        } else {
                                _numSlots++;
                        }
                }
        }
}

void HungarianMatching::createMatrix() {
        setNumSlots();
        _hungarianMatrix = Matrix<DBU>(_numSlots, _numIOPins);

        unsigned slotIndex = 0;
        for (auto i : *_slots) {
                unsigned pinIndex = 0;
                if (i.current && i.visited) {
                        i.current = false;
                } else if (not i.current) {
                        continue;
                }
                Coordinate newPos = i.pos;
                _netlist->forEachIOPin([&](unsigned idx, IOPin& ioPin) {
                        DBU hpwl = _netlist->computeIONetHPWL(idx, newPos);
                        _hungarianMatrix(slotIndex, pinIndex) = hpwl;
                        pinIndex++;
                });
                slotIndex++;
        }
}

bool HungarianMatching::updateNeighborhood(bool last_pass) {
        Coordinate pos(0, 0);
        bool will_remove;
        bool stable;
        std::vector<unsigned> to_remove;
        std::vector<unsigned> to_explore;
        /* TODO:  <23-04-19, transpose io pins and slots in matrix> */
        for (size_t row = 0; row < _hungarianMatrix.rows(); row++) {
                will_remove = true;
                for (size_t col = 0; col < _hungarianMatrix.columns(); col++) {
                        if (_hungarianMatrix(row, col) == 0) {
                                will_remove = false;
                                break;
                        }
                }
                if (will_remove) {
                        to_remove.push_back(row);
                } else {
                        to_explore.push_back(row);
                }
        }
        if (to_remove.size()) {
                stable = false;
        } else {
                stable = true;
        }
        markRemove(to_remove);
        if (not last_pass) {
                markExplore(to_explore);
        }
        return stable;
}

inline void HungarianMatching::addSlot(slot_t& s) {
        if (not s.visited && not s.current) {
                s.current = true;
                _numSlots++;
        }
}

inline void HungarianMatching::markExplore(std::vector<unsigned> v) {
        unsigned curr = 0;
        slotVector_t slots = *_slots;
        for (unsigned i = 0; i < slots.size(); ++i) {
                if (slots.at(i).current && v.size() > 0 && v.at(0) == curr) {
                        if (i > 0) {
                                addSlot(slots.at(i - 1));
                        }
                        addSlot(slots.at(i + 1));
                        v.erase(v.begin());
                }
                curr++;
        }
}

inline void HungarianMatching::markRemove(std::vector<unsigned> v) {
        unsigned curr = 0;
        for (auto i : *_slots) {
                if (i.current && v.size() > 0 && v.at(0) == curr) {
                        i.visited = true;
                        v.erase(v.begin());
                        _numSlots--;
                }
                curr++;
        }
}

void HungarianMatching::getFinalAssignment(std::vector<IOPin>& v, std::vector<IOPin>& zeroSinkIOs) {
        unsigned id = 0;
        for (auto i : *_slots) {
			    bool pinPlaced = false;
                if (i.current && not i.visited) {
					_netlist->forEachIOPin([&](unsigned idx, IOPin& ioPin) {
							if (idx == id && !pinPlaced) {
								ioPin.setPos(i.pos);
								v.push_back(ioPin);
								pinPlaced = true;
								id++;
							}
					});					
                } else if (zeroSinkIOs.size() > 0) {
					zeroSinkIOs[0].setPos(i.pos);
					v.push_back(zeroSinkIOs[0]);
					zeroSinkIOs.erase(zeroSinkIOs.begin());
				}
        }
}
