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

#ifndef __NETLIST_H_
#define __NETLIST_H_

#include <string>
#include <vector>
#include <functional>
#include <limits>
#include <iostream>
#include "Coordinate.h"
#include "Box.h"

enum Orientation { NORTH, SOUTH, EAST, WEST };
enum Direction { IN, OUT, INOUT };

class InstancePin {
       protected:
        std::string _name;
        Coordinate _pos;

       public:
        InstancePin(const std::string& name, const Coordinate& pos)
            : _name(name), _pos(pos) {}
        std::string getName() const { return _name; }
        Coordinate getPos() const { return _pos; }
        DBU getX() const { return _pos.getX(); }
        DBU getY() const { return _pos.getY(); }
};

class IOPin : public InstancePin {
        Orientation _orientation;
        Direction _direction;
        Coordinate _lowerBound;
        Coordinate _upperBound;
        std::string _netName;

       public:
        IOPin(const std::string& name, Direction dir, Coordinate lowerBound,
              Coordinate upperBound, std::string netName)
            : InstancePin(name, Coordinate(0, 0)),
              _orientation(NORTH),
              _direction(dir),
              _lowerBound(lowerBound),
              _upperBound(upperBound),
              _netName(netName) {}

        void setOrientation(const Orientation orientation) {
                _orientation = orientation;
        }
        Orientation getOrientation() const { return _orientation; }
        void setX(const DBU x) { _pos.setX(x); }
        void setY(const DBU y) { _pos.setY(y); }
        void setPos(const Coordinate pos) { _pos = pos; }
        void setPos(const DBU x, const DBU y) { _pos.init(x, y); }
        Direction getDirection() const { return _direction; }
        Coordinate getLowerBound() const { return _lowerBound; };
        Coordinate getUpperBound() const { return _upperBound; };
        std::string getNetName() const { return _netName; }
};

class Netlist {
        std::vector<InstancePin> _instPins;
        std::vector<unsigned> _netPointer;
        std::vector<IOPin> _ioPins;

       public:
        Netlist();

        void addIONet(const IOPin& ioPin,
                      const std::vector<InstancePin>& instPins);

        void forEachIOPin(std::function<void(unsigned idx, IOPin&)> func);
        void forEachIOPin(
            std::function<void(unsigned idx, const IOPin&)> func) const;
        void forEachSinkOfIO(unsigned idx,
                             std::function<void(InstancePin&)> func);
        void forEachSinkOfIO(
            unsigned idx, std::function<void(const InstancePin&)> func) const;
        unsigned numSinksOfIO(unsigned idx);
        int numIOPins();

        DBU computeIONetHPWL(unsigned idx, Coordinate slotPos);
};

#endif /* __NETLIST_H_ */
