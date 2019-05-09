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
        /* TODO:  <25-04-19, this parameter list needs revisoin> */
        dscp.add_options()("idf,i", po::value<std::string>(),
                           "Input DEF file (mandatory)")(
            "def,o", po::value<std::string>(), "Output DEF file (mandatory)")(
            "mlh,h", po::value<int>(),
            "Horizontal metal layer (mandatory)")(
            "mlv,v", po::value<int>(),
            "Vertical metal layer (mandatory)");

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

                if (vm.count("help") || !vm.count("mlh") || !vm.count("mlv") ||
                    !vm.count("def") || !vm.count("idf")) {
                        std::cout << "\n" << dscp;
                        std::exit(1);
                }

                _inputDefFile = vm["idf"].as<std::string>();
                _outputDefFile = vm["def"].as<std::string>();
                _horizontalMetalLayer = vm["mlh"].as<int>();
                _verticalMetalLayer = vm["mlv"].as<int>();
        }
        catch (const po::error& ex) {
                std::cerr << ex.what() << '\n';
        }

        printAll();
}

void Parameters::printAll() const {
        std::cout << "\nOptions: \n";
        std::cout << std::setw(20) << std::left << "Input DEF file: ";
        std::cout << _inputDefFile << "\n";
        std::cout << std::setw(20) << std::left << "Output DEF file: ";
        std::cout << _outputDefFile << "\n";
        std::cout << std::setw(20) << std::left << "Horizontal metal layer: ";
        std::cout << "Metal" << _horizontalMetalLayer+1 << "\n";
        std::cout << std::setw(20) << std::left << "Vertical metal layer: ";
        std::cout << "Metal" << _verticalMetalLayer+1 << "\n";

        std::cout << "\n";
}
