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

#include "Parameters.h"

#include <iostream>
#include <iomanip>
#include <boost/program_options.hpp>

Parameters::Parameters(int argc, char** argv) {
        namespace po = boost::program_options;
        po::options_description dscp("Usage");
        // clang-format off
        dscp.add_options()
                ("input,i"        , po::value<std::string>() , "Input DEF file (mandatory)")
                ("output,o"       , po::value<std::string>() , "Output DEF file (mandatory)")
                ("hmetal,h"       , po::value<int>()         , "Horizontal metal layer (int) (mandatory)")
                ("vmetal,v"       , po::value<int>()         , "Vertical metal layer (int) (mandatory)")
                ("wirelen,w"      , po::value<int>()         , "Return IO nets HPWL (bool) (optional)")
                ("force-spread,f" , po::value<int>()         , "Force pins to be spread in core, i.e., try to respect number os slots (bool) (optional)")
                ("nslots,n"       , po::value<int>()         , "Number of slots per section (int) (optional)")
                ("slots-factor,s" , po::value<float>()       , "Increase factor (%) of slots per section (float) (optional)")
                ("max-slots,m"    , po::value<float>()       , "Percentage of usage for each section (float) (optional)")
                ("usage-factor,x" , po::value<float>()       , "Increase factor (%) of usage for each section (float) (optional)")
                ("block-area,b"   , po::value<std::string>() , "File containing areas to be blocked (optional)")
                ;
        // clang-format on

        po::variables_map vm;
        try {
                po::store(
                    po::command_line_parser(argc, argv)
                        .options(dscp)
                        .style(po::command_line_style::unix_style |
                               po::command_line_style::allow_long_disguise)
                        .run(),
                    vm);
                po::notify(vm);

                if (vm.count("help") || !vm.count("hmetal") ||
                    !vm.count("vmetal") || !vm.count("output") ||
                    !vm.count("input")) {
                        std::cout << "\n" << dscp;
                        std::exit(1);
                }

                _inputDefFile = vm["input"].as<std::string>();
                _outputDefFile = vm["output"].as<std::string>();
                _horizontalMetalLayer = vm["hmetal"].as<int>();
                _verticalMetalLayer = vm["vmetal"].as<int>();

                if (vm.count("wirelen")) {
                        _returnHPWL = vm["wirelen"].as<int>();
                }
                if (vm.count("nslots")) {
                        _nslots = vm["nslots"].as<int>();
                }
                if (vm.count("slots-factor")) {
                        _slotsFactor = vm["slots-factor"].as<float>();
                }
                if (vm.count("max-slots")) {
                        _usage = vm["max-slots"].as<float>();
                }
                if (vm.count("usage-factor")) {
                        _usageFactor = vm["usage-factor"].as<float>();
                }
                if (vm.count("force-spread")) {
                        _forceSpread = vm["force-spread"].as<int>();
                }
                if (vm.count("block-area")) {
                        _blockagesFile = vm["block-area"].as<std::string>();
                }
        } catch (const po::error& ex) {
                std::cerr << ex.what() << '\n';
        }

        printAll();
}

void Parameters::printAll() const {
        // clang-format off
        std::cout << "\nOptions: \n";
        std::cout << std::setw(20) << std::left << "Input DEF file: ";
        std::cout << _inputDefFile << "\n";
        std::cout << std::setw(20) << std::left << "Output DEF file: ";
        std::cout << _outputDefFile << "\n";
        std::cout << std::setw(20) << std::left << "Horizontal metal layer: ";
        std::cout << "Metal" << _horizontalMetalLayer << "\n";
        std::cout << std::setw(20) << std::left << "Vertical metal layer: ";
        std::cout << "Metal" << _verticalMetalLayer << "\n";
        std::cout << "Get IO nets HPWL: " << _returnHPWL << "\n";

        std::cout << "Number of slots per section: " << _nslots << "\n";
        std::cout << "Increase factor of slots per section: " << _slotsFactor << "\n";
        std::cout << "Percentage of usage for each section: " << _usage << "\n";
        std::cout << "Increase factor of usage for each section: " << _usageFactor << "\n";
        std::cout << "Force pin spread: " << _forceSpread << "\n";
        std::cout << "Blockage area file: " << _blockagesFile << "\n"; 

        std::cout << "\n";
        // clang-format on
}
