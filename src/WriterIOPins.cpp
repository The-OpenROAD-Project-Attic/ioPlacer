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

#include "WriterIOPins.h"
#include <iostream>
#include <fstream>
#include <sstream>

WriterIOPins::WriterIOPins(Netlist& netlist, std::vector<IOPin>& av,
                           std::string horizontalMetalLayer,
                           std::string verticalMetalLayer,
                           std::string inFileName, std::string outFileName)
    : _netlist(netlist), _assignment(av) {
        _horizontalMetalLayer = horizontalMetalLayer;
        _verticalMetalLayer = verticalMetalLayer;
        _inFileName = inFileName;
        _outFileName = outFileName;
}

void WriterIOPins::run() {
        if (writeFile())
                std::cout << "File " << _outFileName << " created\n";
        else
                std::cout << "Error in writeFile!\n";
}

bool WriterIOPins::writeFile() {
        std::ofstream pinsFile;
        std::ifstream defFile;
        std::string line;
        std::string pins;
        std::string numPins;
        std::string buff;
        bool pinsSection = false;
        bool pinsSectionEnd = true;

        pinsFile.open(_outFileName);
        defFile.open(_inFileName);

        if (!defFile.is_open()) {
                std::cout << "DEF file \"" << _inFileName << "\" could not been open\n";
                std::exit(-1);
        }

        while (std::getline(defFile, line)) {
                std::istringstream iss(line);
                if (iss >> pins >> numPins >> buff) {
                        if (pins == "PINS") {
                                pinsFile << line << std::endl;
                                pinsSection = true;
                        }
                }

                if (pinsSection) {
                        std::vector<IOPin>& assignment = _assignment;

                        if (!pinsFile.is_open()) {
                                std::cout << "Could not open file pinsFile.\n";
                                pinsFile.close();
                                return false;
                        }

                        for (IOPin ioPin : assignment) {
                                std::string name = ioPin.getName();
                                std::string netName = ioPin.getNetName();
                                Direction direction = ioPin.getDirection();
                                std::string layer;

                                Coordinate position(ioPin.getX(), ioPin.getY());
                                Orientation orientation =
                                    ioPin.getOrientation();
                                Coordinate lowerBound = ioPin.getLowerBound();
                                Coordinate upperBound = ioPin.getUpperBound();

                                std::string dir = (direction == Direction::DIR_IN)
                                                      ? "INPUT"
                                                      : "OUTPUT";
                                std::string orient;
                                switch (orientation) {
                                        case Orientation::ORIENT_EAST:
                                                orient = "E";
                                                break;
                                        case Orientation::ORIENT_WEST:
                                                orient = "W";
                                                break;
                                        case Orientation::ORIENT_NORTH:
                                                orient = "N";
                                                break;
                                        case Orientation::ORIENT_SOUTH:
                                                orient = "S";
                                                break;
                                }

                                std::string locationType;
                                if (ioPin.getLocationType() != "INVALID_STRING*") {
                                        locationType = ioPin.getLocationType();
                                } else  {
                                        locationType = "FIXED";
                                }

                                if (orientation == Orientation::ORIENT_EAST ||
                                    orientation == Orientation::ORIENT_WEST) {
                                        layer = _horizontalMetalLayer;
                                } else {
                                        layer = _verticalMetalLayer;
                                }

                                pinsFile << "- " << name << " + NET " << netName
                                         << " + DIRECTION " << dir
                                         << " + USE SIGNAL" << std::endl;
                                pinsFile << "  + LAYER " << layer << " ( "
                                         << lowerBound.getX() << " "
                                         << lowerBound.getY() << " ) ( "
                                         << upperBound.getX() << " "
                                         << upperBound.getY() << " )"
                                         << std::endl;
                                pinsFile << "  + " << locationType
                                         << " ( " << position.getX()
                                         << " " << position.getY() << " ) "
                                         << orient << " ;" << std::endl;
                        }
                        pinsSection = false;
                        pinsSectionEnd = false;
                } else if (pinsSectionEnd) {
                        pinsFile << line << std::endl;
                } else {
                        if (line == "END PINS") {
                                pinsFile << line << std::endl;
                                pinsSectionEnd = true;
                        }
                }
        }

        pinsFile.close();
        defFile.close();

        return true;
}
