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

#ifndef LEFDESCRIPTOR_H
#define LEFDESCRIPTOR_H

#include <map>
#include <deque>
#include <vector>

//#include "util/DoubleRectangle.h"
//#include "util/Double2.h"

static const std::string INVALID_LEF_NAME = "*<INVALID_LEF_NAME>*";
static const std::string DEFAULT_PIN_DIRECTION = "INPUT";

class LefUnitsDscp {
       public:
        bool clsHasTime = false;
        bool clsHasCapacitance = false;
        bool clsHasResitance = false;
        bool clsHasPower = false;
        bool clsHasCurrent = false;
        bool clsHasVoltage = false;
        bool clsHasDatabase = false;
        bool clsHasFrequency = false;

        int clsTime = 0;
        int clsCapacitance = 0;
        int clsResitance = 0;
        int clsPower = 0;
        int clsCurrent = 0;
        int clsVoltage = 0;
        int clsDatabase = 100;  // default value at LEF/DEF reference
        int clsFrequency = 0;

        LefUnitsDscp() = default;
};  // end class

class LefSpacingRuleDscp {
       public:
        double clsSpacing = 0.0;
        double clsEOL = 0.0;  // End of line
        double clsEOLWithin = 0.0;
        LefSpacingRuleDscp() = default;
};  // end class

//! Descriptor for LEF Layer

class LefLayerDscp {
       public:
        std::string clsName = INVALID_LEF_NAME;
        std::string clsType = INVALID_LEF_NAME;
        std::string clsDirection = INVALID_LEF_NAME;
        double clsPitch[2] = {0.0, 0.0};
        double clsOffset = 0.0;
        double clsWidth = 0.0;
        double clsMinWidth = 0.0;
        double clsArea = 0.0;
        std::vector<LefSpacingRuleDscp> clsSpacingRules;
        LefLayerDscp() = default;
};  // end class

//! Descriptor for LEF Spacing

class LefSpacingDscp {
       public:
        std::string clsLayer1 = INVALID_LEF_NAME;
        std::string clsLayer2 = INVALID_LEF_NAME;
        double clsDistance = 0.0;
        LefSpacingDscp() = default;
};  // end class

class LefDscp {
       public:
        int clsMajorVersion = 0;
        int clsMinorVersion = 0;
        std::string clsCaseSensitive = INVALID_LEF_NAME;
        std::string clsBusBitChars = INVALID_LEF_NAME;
        std::string clsDivideChar = INVALID_LEF_NAME;
        double clsManufactGrid = 0.0;
        LefUnitsDscp clsLefUnitsDscp;
        std::vector<LefLayerDscp> clsLefLayerDscps;
        std::vector<LefSpacingDscp> clsLefSpacingDscps;
        LefDscp() = default;
};  // end class

// -----------------------------------------------------------------------------

#endif /* PHYSICALDESCRIPTOR_H */
