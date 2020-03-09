# ////////////////////////////////////////////////////////////////////////////////
# // Authors: Vitor Bandeira, Mateus Fogaça, Eder Matheus Monteiro e Isadora
# // Oliveira
# //          (Advisor: Ricardo Reis)
# //
# // BSD 3-Clause License
# //
# // Copyright (c) 2019, Federal University of Rio Grande do Sul (UFRGS)
# // All rights reserved.
# //
# // Redistribution and use in source and binary forms, with or without
# // modification, are permitted provided that the following conditions are met:
# //
# // * Redistributions of source code must retain the above copyright notice, this
# //   list of conditions and the following disclaimer.
# //
# // * Redistributions in binary form must reproduce the above copyright notice,
# //   this list of conditions and the following disclaimer in the documentation
# //   and/or other materials provided with the distribution.
# //
# // * Neither the name of the copyright holder nor the names of its
# //   contributors may be used to endorse or promote products derived from
# //   this software without specific prior written permission.
# //
# // THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# // AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# // IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# // ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# // LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# // CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# // SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# // INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# // CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# // ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# // POSSIBILITY OF SUCH DAMAGE.
# ////////////////////////////////////////////////////////////////////////////////


sta::define_cmd_args "io_placer" {[-hor_layer h_layer] \ 
                                  [-ver_layer v_layer] \
                                  [-random_seed seed]  \
                       	          [-random]            \
                                 }

proc io_placer { args } {
  sta::parse_key_args "io_placer" args \
  keys {-hor_layer -ver_layer -random_seed} flags {-random}

  if { [info exists flags(-random)] } {
    ioPlacer::set_random_mode 2
  }

  set seed 42
  if [info exists keys(-random_seed)] {
    set seed $keys(-random_seed)
  }
  ioPlacer::set_rand_seed $seed

  set hor_layer 3
  if [info exists keys(-hor_layer)] {
    set hor_layer $keys(-hor_layer)
    ioPlacer::set_hor_metal_layer $hor_layer
  } else {
    puts "Warning: use -hor_layer to set the horizontal layer."
  }       
  
  set ver_layer 2
  if [info exists keys(-ver_layer)] {
    set ver_layer $keys(-ver_layer)
    ioPlacer::set_ver_metal_layer $ver_layer
  } else {
    puts "Warning: use -ver_layer to set the vertical layer."
  }       

  ioPlacer::run_io_placement
}
