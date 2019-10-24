import_lef input.lef
import_def input.def

set_hor_metal_layer 2
set_ver_metal_layer 3

set_num_threads 5

run_io_placement

export_def "result.def"

exit

