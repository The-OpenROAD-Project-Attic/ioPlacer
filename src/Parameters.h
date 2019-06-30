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

#ifndef __PARAMETERS_H_
#define __PARAMETERS_H_

#include <string>

#include "Coordinate.h"

class Parameters {
       private:
        int _horizontalMetalLayer;
        int _verticalMetalLayer;
        std::string _inputLefFile;
        std::string _inputDefFile;
        std::string _outputDefFile;
        bool _returnHPWL = false;

        bool _forceSpread = true;
        int _nslots = -1;
        int _randomMode = 0;
        float _slotsFactor = -1;
        float _usage = -1;
        float _usageFactor = -1;
        std::string _blockagesFile;
	DBU _horizontalLength = -1;
	DBU _verticalLength = -1;

        void printAll() const;

       public:
        Parameters(int, char**);

        std::string getInputDefFile() const { return _inputDefFile; }
        std::string getInputLefFile() const { return _inputLefFile; }
        std::string getOutputDefFile() const { return _outputDefFile; }
        int getHorizontalMetalLayer() const { return _horizontalMetalLayer; };
        int getVerticalMetalLayer() const { return _verticalMetalLayer; };
        int returnHPWL() const { return _returnHPWL; };
        int returnNslots() const { return _nslots; };
        int returnRandomMode() const { return _randomMode; };
        float returnSlotsFactor() const { return _slotsFactor; };
        float returnUsage() const { return _usage; };
        float returnUsageFactor() const { return _usageFactor; };
        float returnForceSpread() const { return _forceSpread; };
        std::string returnBlockagesFile() const { return _blockagesFile; };
	DBU returnHorizontalLength() const { return _horizontalLength; };
	DBU returnVerticalLength() const { return _verticalLength; };
};

#endif /* __PARAMETERS_H_ */
