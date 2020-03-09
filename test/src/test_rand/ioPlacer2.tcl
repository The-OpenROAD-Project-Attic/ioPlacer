read_lef input.lef
read_def input.def

io_placer -hor_layer 2 -ver_layer 3 -random -random_seed 56
write_def "result2.def"

exit

