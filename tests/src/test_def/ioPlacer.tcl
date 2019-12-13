read_lef input.lef
read_def input.def

io_placer -hor_layer 2 -ver_layer 3

write_def "result.def"

exit

