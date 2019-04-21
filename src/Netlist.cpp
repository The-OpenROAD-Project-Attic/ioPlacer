////////////////////////////////////////////////////////////////////////////////////
// Authors: Vitor Bandeira, Mateus Fogaça, Eder Matheus Monteiro e Isadora Oliveira
//          (Advisor: Ricardo Reis)
//
// BSD 3-Clause License
//
// Copyright (c) 2018, The Regents of the University of California
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
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
////////////////////////////////////////////////////////////////////////////////////


#include "Netlist.h"

Netlist::Netlist() {
	_netPointer.push_back(0);
}

void Netlist::addIONet(const IOPin& ioPin, const std::vector<InstancePin>& instPins) {
	_ioPins.push_back(ioPin);
	_instPins.insert(_instPins.end(), instPins.begin(), instPins.end());
	_netPointer.push_back(_instPins.size());
}

void Netlist::forEachIOPin(std::function<void(unsigned idx, IOPin&)> func) {
	for (unsigned idx = 0; idx < _ioPins.size(); ++idx) {
		func(idx, _ioPins[idx]);
	}
}

void Netlist::forEachIOPin(std::function<void(unsigned idx, const IOPin&)> func) const {
	for (unsigned idx = 0; idx < _ioPins.size(); ++idx) {
		func(idx, _ioPins[idx]);
	}
}

void Netlist::forEachSinkOfIO(unsigned idx, std::function<void(InstancePin&)> func) {
	unsigned netStart = _netPointer[idx];
	unsigned netEnd = _netPointer[idx + 1];
	for (unsigned idx = netStart; idx < netEnd; ++idx) {
		func(_instPins[idx]);
	}
}

void Netlist::forEachSinkOfIO(unsigned idx, std::function<void(const InstancePin&)> func) const {
	unsigned netStart = _netPointer[idx];
	unsigned netEnd = _netPointer[idx + 1];
	for (unsigned idx = netStart; idx < netEnd; ++idx) {
		func(_instPins[idx]);
	}
}
