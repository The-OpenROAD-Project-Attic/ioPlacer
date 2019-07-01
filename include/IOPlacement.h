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

#ifndef __IOPLACEMENT_H_
#define __IOPLACEMENT_H_

#include <iostream>
#include <string>
#include <map>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>

namespace ioPlacer {

typedef long long int DBU;
typedef boost::geometry::model::d2::point_xy<DBU> point;
typedef boost::geometry::model::box<point> box;

struct _Pin {
        std::string name;
        point pos;
        char orientation;
} typedef Pin_t;

class IOPlacement {
       public:
        IOPlacement() = default;

        // the lower left and the upper right corners, minimum spacing between
        // IO pins, first track's position
        void initCore(point lowerBounds, point upperBounds, DBU minSpacingX,
                      DBU minSpacingY, DBU initTrackX, DBU initTrackY, 
                      DBU minAreaX, DBU minAreaY, DBU minWidthX, DBU minWidthY,
		      DBU databaseUnit);

        void setMetalLayers(int horizontalMetalLayer, int verticalMetalLayer);

        // ioPin name, position, name of the net that is connected to it,
        // bounds, direction: INPUT, OUTPUT, INOUT
        void addIOPin(std::string name, std::string netName, box bounds,
                      std::string direction);

        // net connected to the pin, pin name, pin position
        void addInstPin(std::string net, std::string pinName, point pos);
        void addBlockage(point initialPos, point finalPos);

        // optional
        void setSlotsPerSection(unsigned slotsPerSection, float increaseFactor);
        void setSlotsUsagePerSection(float usagePerSection,
                                     float increaseFactor);
        void forcePinSpread(bool force);

        void setRandomMode(int randomMode);

        // run IOPlacement, returns a vector with all a pin name, its new
        // position and its orientation
        std::vector<Pin_t> run(bool returnHPWL = false);

       private:
        struct cellPin {
                std::string name;
                point position;
        };

        struct ioPin {
                std::string name;
                point position;
                std::string netName;
                box bounds;
                std::string direction;
                std::vector<cellPin> connections;
                std::string locationType;
        };

        std::vector<ioPin> _ioPins;
        std::map<std::string, std::vector<size_t>> _mapNetToIo;

        void initNetlist();
        void getResults(std::vector<Pin_t>& pinAssignment);
        char getOrientationString(int orient);
};

}  // namespace ioPlacer

#endif /* __IOPLACEMENT_H_ */
