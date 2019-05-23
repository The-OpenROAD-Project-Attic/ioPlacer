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

#include <iostream>
#include <chrono>
#include <ctime>

#include "Parameters.h"
#include "IOPlacementKernel.h"

int main(int argc, char** argv) {
        std::cout << " ######################################\n";
        std::cout << " #      OpenROAD IO placement tool    #\n";
        std::cout << " #                                    #\n";
        std::cout << " # Authors:                           #\n";
        std::cout << " #    Vitor Bandeira (UFRGS)          #\n";
        std::cout << " #    Mateus Fogaça (UFRGS)           #\n";
        std::cout << " #    Eder Matheus Monteiro (UFRGS)   #\n";
        std::cout << " #    Isadora Oliveira (UFRGS)        #\n";
        std::cout << " #                                    #\n";
        std::cout << " #  Advisor:                          #\n";
        std::cout << " #    Ricardo Reis (UFRGS)            #\n";
        std::cout << " ######################################\n";
        std::cout << "\n";

        std::time_t date = std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now());
        std::cout << "Current time: " << std::ctime(&date);

        Parameters parms(argc, argv);
        IOPlacementKernel kernel(parms);
        kernel.run();
        kernel.getResults();

        return 0;
}
