import_lef ispd18_test1.input.lef
import_def ispd18_test1.input.def

set_hor_metal_layer 2
set_ver_metal_layer 3

set_num_threads 1

run_io_placement

export_def "result.def"

exit

