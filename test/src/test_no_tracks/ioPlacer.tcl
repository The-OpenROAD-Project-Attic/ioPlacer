read_lef ./NangateOpenCellLibrary.lef
read_lib ./NangateOpenCellLibrary_typical.lib
read_verilog ./gcd_nangate45.v
link_design gcd
read_sdc ./gcd.sdc

initialize_floorplan -site FreePDK45_38x28_10R_NP_162NW_34O \
-utilization 30

io_placer -random -hor_layer 2 -ver_layer 3
