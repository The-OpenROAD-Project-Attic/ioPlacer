read_lef input.lef
read_def input.def

io_placer -hor_layer 2 -ver_layer 3 -boundaries_offset 0 -min_distance 1 -random
write_def "result1.def"

exit

