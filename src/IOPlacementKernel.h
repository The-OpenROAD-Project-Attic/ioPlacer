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

#ifndef __IOPLACEMENTKERNEL_H_
#define __IOPLACEMENTKERNEL_H_

#include "Core.h"
#include "Parameters.h"
#include "HungarianMatching.h"
#include "Netlist.h"
#include "Slots.h"

class IOPlacementKernel {
       private:
        Parameters* _parms;
        Netlist _netlist;
        Netlist _netlistIOPins;
        Core _core;
        slotVector_t _slots;
        sectionVector_t _sections;
        std::vector<IOPin> _zeroSinkIOs;
        std::string _horizontalMetalLayer;
        std::string _verticalMetalLayer;

        unsigned _slotsPerSection = 100;
        float _slotsIncreaseFactor = 0.1f;

        float _usagePerSection = .1f;
        float _usageIncreaseFactor = 0.1f;

        bool _forcePinSpread = true;

        void randomPlacement(std::vector<IOPin>&);
        void initNetlistAndCore();
        void initIOLists();
        void defineSlots();
        void createSections();
        void setupSections();
        bool assignPinsSections();
        DBU returnIONetsHPWL(Netlist&);

        inline void updateOrientation(IOPin&);

        int getKValue() { return 1; }

       public:
        IOPlacementKernel(Parameters&);
        void run();
};

#endif /* __IOPLACEMENTKERNEL_H_ */
