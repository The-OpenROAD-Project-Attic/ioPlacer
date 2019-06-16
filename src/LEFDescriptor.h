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

static const std::string INVALID_LEF_NAME = "*<INVALID_LEF_NAME>*";
static const std::string DEFAULT_PIN_DIRECTION = "INPUT";

class LefUnitsDscp {
       public:
        bool _clsHasTime = false;
        bool _clsHasCapacitance = false;
        bool _clsHasResitance = false;
        bool _clsHasPower = false;
        bool _clsHasCurrent = false;
        bool _clsHasVoltage = false;
        bool _clsHasDatabase = false;
        bool _clsHasFrequency = false;

        int _clsTime = 0;
        int _clsCapacitance = 0;
        int _clsResitance = 0;
        int _clsPower = 0;
        int _clsCurrent = 0;
        int _clsVoltage = 0;
        int _clsDatabase = 100;  // default value at LEF/DEF reference
        int _clsFrequency = 0;

        LefUnitsDscp() = default;
};  // end class

class LefSpacingRuleDscp {
       public:
        double _clsSpacing = 0.0;
        double _clsEOL = 0.0;  // End of line
        double _clsEOLWithin = 0.0;
        LefSpacingRuleDscp() = default;
};  // end class

//! Descriptor for LEF Layer

class LefLayerDscp {
       public:
        std::string _clsName = INVALID_LEF_NAME;
        std::string _clsType = INVALID_LEF_NAME;
        std::string _clsDirection = INVALID_LEF_NAME;
        double _clsPitch[2] = {0.0, 0.0};
        double _clsOffset = 0.0;
        double _clsWidth = 0.0;
        double _clsMinWidth = 0.0;
        double _clsArea = 0.0;
        std::vector<LefSpacingRuleDscp> _clsSpacingRules;
        LefLayerDscp() = default;
};  // end class

//! Descriptor for LEF Spacing

class LefSpacingDscp {
       public:
        std::string _clsLayer1 = INVALID_LEF_NAME;
        std::string _clsLayer2 = INVALID_LEF_NAME;
        double _clsDistance = 0.0;
        LefSpacingDscp() = default;
};  // end class

class LefDscp {
       public:
        int _clsMajorVersion = 0;
        int _clsMinorVersion = 0;
        std::string _clsCaseSensitive = INVALID_LEF_NAME;
        std::string _clsBusBitChars = INVALID_LEF_NAME;
        std::string _clsDivideChar = INVALID_LEF_NAME;
        double _clsManufactGrid = 0.0;
        LefUnitsDscp _clsLefUnitsDscp;
        std::vector<LefLayerDscp> _clsLefLayerDscps;
        std::vector<LefSpacingDscp> _clsLefSpacingDscps;
        LefDscp() = default;
};  // end class

// -----------------------------------------------------------------------------

#endif /* PHYSICALDESCRIPTOR_H */
