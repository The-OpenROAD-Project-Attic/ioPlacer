read_lef input.lef
read_def input.def

run_io_placement -hor_layer 2 -ver_layer 3

write_def "result.def"

exit

