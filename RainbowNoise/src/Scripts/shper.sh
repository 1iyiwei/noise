#!/bin/tcsh -f
# the equivalence of paper.sh for dart shuffling

set output_dir = data;
set huge_dir = data/huge;

set dot_radius = 0.008;
set no_compute = 0;

set peak_method = "default";
set shuffle_method = "maxpeak";

# match the lloyd setting
set num_samples_per_class = 100;
set num_points_per_sample = 1024;
set sample_grid_res = 10; # roughly sqrt(num_samples)
set trough_grid_res = 100; # roughly sqrt(num_points)

@ num_points_per_class = ${num_points_per_sample} * ${num_samples_per_class};

# match up with lloyd
if(0) then

set dimension = 2;
set domain_size = "1 1";
set num_class = 2;
set energy_store_method = grid${sample_grid_res};
set trough_method = grid${trough_grid_res};
if(1) then # better result
@ num_samples = ${num_class} * ${num_samples_per_class};
set trough_method = random${num_samples};
#set trough_method = random${num_points_per_sample};
set shuffle_method = "gradualpeak";
endif
if(0) then # slow
set energy_store_method = grid${trough_grid_res};
set trough_method = union;
endif

set kernel_type = "gaussian";
set relative_kernel_size = 0.5;
 
@ num_points = ${num_class} * ${num_points_per_class};

set r_value = 0.0075;
set k_value = 0.70;

set moving_window_size = 100;
set max_num_iterations = 1000;

set num_samples = "";
set i = 0;
while(${i} < ${num_class})
set num_samples = "${num_samples_per_class} ${num_samples_per_class}";
@ i = ${i} + 1;
end

set base_name = ${dimension}d_${num_class}c_${num_samples_per_class};

set init = white;

set init_base_name = shuffle_${init}_${base_name};

if(${no_compute}) then
else
if(${init} == poisson) then
./src/Main/Release/DartThrowing ${dimension} 1 "0" ${r_value} ${k_value} ${domain_size} > ${output_dir}/${init_base_name}_init_points.txt
else
./src/Main/Release/WhiteNoiseSamples ${dimension} 1 ${num_points} ${domain_size} > ${output_dir}/${init_base_name}_init_points.txt
endif

./src/Main/Release/RandomPickPointsSites ${output_dir}/${init_base_name}_init_points.txt ${dimension} ${num_class} ${num_samples} > ${output_dir}/${init_base_name}_points.txt
endif #if(${no_compute})

foreach class_weighting_scheme (fair)

set output_base_name = ${init_base_name}_${energy_store_method}_${trough_method}_${class_weighting_scheme};

if(${no_compute}) then
else
./src/Main/Release/DartShuffling ${output_dir}/${init_base_name}_points.txt ${dimension} ${domain_size} ${energy_store_method} ${peak_method} ${trough_method} ${shuffle_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} > ${output_dir}/${output_base_name}_samples.txt
endif #if(${no_compute})

# visualizing all samples in one color
cp ${output_dir}/${output_base_name}_samples.txt ${output_dir}/${output_base_name}_samples_all.txt;
set current_class = 1;
while(${current_class} < ${num_class})
set i = ${current_class};
while(${i} > 0)
./src/Main/Release/ProcessLloydSites ${output_dir}/${output_base_name}_samples_all.txt ${output_dir}/${output_base_name}_samples_all.txt ${dimension} "decrement" ${i}
@ i = ${i} - 1;
end
@ current_class = ${current_class} + 1;
end

# draw samples
foreach draw_name (${output_base_name}_samples ${output_base_name}_samples_all)

if(${draw_name} == ${output_base_name}_samples_all) then
set color_palette_name = "palette";
set color_palette_file_name = ${output_dir}/${color_palette_name}.txt;
echo "0 0 0" > ${color_palette_file_name};
else
set color_palette_file_name = "";
endif

./src/Main/Release/DrawSamples ${output_dir}/${draw_name}.txt ${output_dir}/${draw_name}.fig 2 ${dot_radius} 1 ${domain_size} ${color_palette_file_name}
./src/Scripts/fig2eps -f ${output_dir}/${draw_name}.fig
epstopdf ${output_dir}/${draw_name}.eps --outfile ${output_dir}/${draw_name}.pdf
end

# measure min spacing
set boundary_condition = 1;
set input_name = ${output_base_name}_samples;
set filtered_name = ${input_name}_filtered;

foreach class_option (0 1 'all')

if(${class_option} == 'all') then
cp ${output_dir}/${input_name}.txt ${output_dir}/${filtered_name}.txt 
else
grep "^${class_option}" ${output_dir}/${input_name}.txt > ${output_dir}/${filtered_name}.txt 
endif

echo ${class_option} ${input_name}
./src/Main/Release/MinSampleSpacing ${output_dir}/${filtered_name}.txt ${dimension} ${domain_size} ${boundary_condition}
end

end # foreach class_weighting_scheme

endif

# measure min distance
if(0) then

set dimension = 2;
set domain_size = "1 1";
set boundary_condition = 1;

foreach class_option (0 1 'all')
foreach input_name (lloyd_2d_2c_100n100_lloyd_none_fair_samples lloyd_lock0_2d_2c_100n100_lloyd_none_fair_samples lloyd_joint_2d_2c_100n100_lloyd_none_fair_samples shuffle_white_2d_2c_100_grid5_grid400_fair_samples lloyd_2d_1c_200_lloyd_none_fair_samples)

set filtered_name = ${input_name}_filtered;

if(${class_option} == 'all') then
cp ${output_dir}/${input_name}.txt ${output_dir}/${filtered_name}.txt 
else
grep "^${class_option}" ${output_dir}/${input_name}.txt > ${output_dir}/${filtered_name}.txt 
endif

echo ${class_option} ${input_name}
./src/Main/Release/MinSampleSpacing ${output_dir}/${filtered_name}.txt ${dimension} ${domain_size} ${boundary_condition}

end
end

endif

# R1G2B1 sensor layout
if(0) then

set dimension = 2;
set num_class = 3;

set domain_size = "1 1";

set grid_size_1d = 20;
set num_samples = "100 200 100";

set energy_store_method = grid${grid_size_1d};
set trough_method = union;
set shuffle_method = "peak0";
set class_weighting_scheme = "fair";
set kernel_type = "gaussian";
set relative_kernel_size = 0.5;
set swap_method = "random";

set moving_window_size = 100;
set max_num_iterations = 2000;

set dot_radius = 0.02;
set grid_size = "${grid_size_1d} ${grid_size_1d}";

set init_base_name = r1g2b1_grid_${grid_size_1d}x${grid_size_1d};
set random_base_name = r1g2b1_random_${grid_size_1d}x${grid_size_1d};
set shuffle_base_name = r1g2b1_shuffle_${grid_size_1d}x${grid_size_1d};
set swap_base_name = r1g2b1_swap_${grid_size_1d}x${grid_size_1d};

# initial tiling
./src/Tiling/Release/R1G2B1GridSampling ${domain_size} ${grid_size} > ${output_dir}/${init_base_name}.txt

# random input
./src/Main/Release/RandomPickPointsSites ${output_dir}/${init_base_name}.txt ${dimension} ${num_class} ${num_samples} > ${output_dir}/${random_base_name}.txt

# shuffle
./src/Main/Release/DartShuffling ${output_dir}/${random_base_name}.txt ${dimension} ${domain_size} ${energy_store_method} ${peak_method} ${trough_method} ${shuffle_method} ${shuffle_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} > ${output_dir}/${shuffle_base_name}.txt

# swap
if(${swap_method} != "") then
./src/Main/Release/DartSwapping ${output_dir}/${shuffle_base_name}.txt ${dimension} ${domain_size} ${energy_store_method} ${swap_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} > ${output_dir}/${swap_base_name}.txt

foreach draw_name (${shuffle_base_name} ${swap_base_name})
foreach id (0 1 2)
if(${id} == "all") then
cp ${output_dir}/${draw_name}.txt ${output_dir}/${draw_name}_${id}.txt;
else
grep "^${id}" ${output_dir}/${draw_name}.txt > ${output_dir}/${draw_name}_${id}.txt;
endif
echo ${draw_name}_${id};
./src/Main/Release/MinSampleSpacing ${output_dir}/${draw_name}_${id}.txt ${dimension} ${domain_size} 1;
rm ${output_dir}/${draw_name}_${id}.txt;
end
end

endif

foreach name (${init_base_name} ${random_base_name} ${shuffle_base_name} ${swap_base_name})

src/Main/Release/DrawSamples ${output_dir}/${name}.txt ${output_dir}/${name}.fig ${dimension} ${dot_radius} 1 ${domain_size}
src/Scripts/fig2eps -f ${output_dir}/${name}.fig
epstopdf ${output_dir}/${name}.eps --outfile ${output_dir}/${name}.pdf

end # foreach name

endif

# R1G2B1 zoneplate
if(0) then

set dimension = 2;
set num_class = 3;
set domain_size = "1 1";

set grid_size_1d = 80;
set num_samples = "1600 3200 1600";
set grid_size_1d = 40;
set num_samples = "400 800 400";

set energy_store_method = grid${grid_size_1d};
set trough_method = union;
set shuffle_method = "peak0";
set class_weighting_scheme = "fair";
set kernel_type = "gaussian";
set relative_kernel_size = 0.5;
set swap_method = "random";

set moving_window_size = 200;
set max_num_iterations = 100000;

set dot_radius = 0.01;

# for tiling, must match grid_size_1d and num_samples above
if(${grid_size_1d} == 80) then
set num_tile_per_dim = 4;
set scaling = 0.25;
else if(${grid_size_1d} == 40) then
set num_tile_per_dim = 8;
set scaling = 0.125;
else
echo unknown grid_size_1d;
exit(1);
endif

set num_tiles = "${num_tile_per_dim} ${num_tile_per_dim}";

# for zone plate
set function_offset = 0;
set function_scale = 10;
set output_resolution = 320;
set filter_gaussian_std = 1.5;

set grid_size = "${grid_size_1d} ${grid_size_1d}";

set init_base_name = r1g2b1_grid_${grid_size_1d}x${grid_size_1d};
set random_base_name = r1g2b1_random_${grid_size_1d}x${grid_size_1d};
set shuffle_base_name = r1g2b1_shuffle_${grid_size_1d}x${grid_size_1d};
set swap_base_name = r1g2b1_swap_${grid_size_1d}x${grid_size_1d};

# initial tiling
./src/Tiling/Release/R1G2B1GridSampling ${domain_size} ${grid_size} > ${output_dir}/${init_base_name}.txt

# random input
./src/Main/Release/RandomPickPointsSites ${output_dir}/${init_base_name}.txt ${dimension} ${num_class} ${num_samples} > ${output_dir}/${random_base_name}.txt

# shuffle
./src/Main/Release/DartShuffling ${output_dir}/${random_base_name}.txt ${dimension} ${domain_size} ${energy_store_method} ${peak_method} ${trough_method} ${shuffle_method} ${shuffle_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} > ${output_dir}/${shuffle_base_name}.txt

# swap
if(${swap_method} != "") then
./src/Main/Release/DartSwapping ${output_dir}/${shuffle_base_name}.txt ${dimension} ${domain_size} ${energy_store_method} ${swap_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} > ${output_dir}/${swap_base_name}.txt

foreach draw_name (${shuffle_base_name} ${swap_base_name})
foreach id (0 1 2)
if(${id} == "all") then
cp ${output_dir}/${draw_name}.txt ${output_dir}/${draw_name}_${id}.txt;
else
grep "^${id}" ${output_dir}/${draw_name}.txt > ${output_dir}/${draw_name}_${id}.txt;
endif
echo ${draw_name}_${id};
./src/Main/Release/MinSampleSpacing ${output_dir}/${draw_name}_${id}.txt ${dimension} ${domain_size} 1;
rm ${output_dir}/${draw_name}_${id}.txt;
end
end

endif

foreach name (${init_base_name} ${random_base_name} ${shuffle_base_name} ${swap_base_name})

src/Main/Release/DrawSamples ${output_dir}/${name}.txt ${output_dir}/${name}.fig ${dimension} ${dot_radius} 1 ${domain_size}
src/Scripts/fig2eps -f ${output_dir}/${name}.fig
epstopdf ${output_dir}/${name}.eps --outfile ${output_dir}/${name}.pdf

end # foreach name

foreach base_name (${init_base_name} ${random_base_name} ${shuffle_base_name} ${swap_base_name})

set output_name = ${base_name};
set output_tiling_name = ${base_name}_tiling${num_tile_per_dim};

src/Main/Release/TileSamples ${output_dir}/${output_name}.txt ${dimension} ${scaling} ${num_tiles} ${domain_size} > ${output_dir}/${output_tiling_name}.txt;

foreach class (0 1 2)
set select_ids = ${class};

src/Main/Release/SampleSelector ${output_dir}/${output_tiling_name}.txt ${dimension} ${select_ids} > ${output_dir}/${output_tiling_name}_${class}.txt

src/Sain/Release/ZonePlate ${dimension} ${output_dir}/${output_tiling_name}_${class}.txt ${output_dir}/${output_tiling_name}_${class}_zoneplate.ppm ${function_offset} ${function_scale} ${output_resolution} ${filter_gaussian_std}

../BlueNoise/src/Main/Release/PPM2PFM ${output_dir}/${output_tiling_name}_${class}_zoneplate.ppm ${output_dir}/${output_tiling_name}_${class}_zoneplate.pfm
end

# merge
src/Sain/Release/Merge3PFM ${output_dir}/${output_tiling_name}_0_zoneplate.pfm ${output_dir}/${output_tiling_name}_1_zoneplate.pfm ${output_dir}/${output_tiling_name}_2_zoneplate.pfm ${output_dir}/${output_tiling_name}_merge_zoneplate.pfm 

echo ${output_tiling_name}_merge_zoneplate.ppm
src/Sain/Release/PFM2PPM ${output_dir}/${output_tiling_name}_merge_zoneplate.pfm ${output_dir}/${output_tiling_name}_merge_zoneplate.ppm 1 1 2

end # foreach base_name

endif

# 3 class for teaser
if(0) then

set dimension = 2;
set domain_size = "1 1";

set num_class = 3;
set num_samples_per_class = "88 353 1414"; # match up 0p080p040p02
set num_samples_text = 88n353n1414;

set num_trials = 1855;
set energy_store_method = grid40;
set trough_method = random${num_trials};
set shuffle_method = "gradualpeak";
set kernel_type = "gaussian";
set relative_kernel_size = 0.5;
set moving_window_size = 100;
set max_num_iterations = 4000;

set energy_class_id = 0;
set energy_array_size = 16; # visualization and debug

set dot_radius = 0.004;

foreach class_weighting_scheme (fair)

set output_base_name = shuffle_${dimension}d_${num_class}c_${num_samples_text}_${energy_store_method}_${trough_method}_${shuffle_method}_${class_weighting_scheme};

if(${no_compute}) then
else
./src/Main/Release/WhiteNoiseSamples ${dimension} ${num_class} ${num_samples_per_class} ${domain_size} > ${output_dir}/${output_base_name}_init_samples.txt

if(${energy_array_size} > 0) then
./src/Main/Release/DartShufflingEnergy ${output_dir}/${output_base_name}_init_samples.txt ${dimension} ${domain_size} ${energy_store_method} ${peak_method} ${trough_method} ${shuffle_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} ${energy_class_id} ${energy_array_size} ${output_dir}/${output_base_name}_energy.pfm  ${output_dir}/${output_base_name}_history.txt > ${output_dir}/${output_base_name}_samples.txt
./src/Sain/Release/PFM2PPM ${output_dir}/${output_base_name}_energy.pfm ${output_dir}/${output_base_name}_energy.ppm 1 1 2
else
./src/Main/Release/DartShuffling ${output_dir}/${output_base_name}_init_samples.txt ${dimension} ${domain_size} ${energy_store_method} ${peak_method} ${trough_method} ${shuffle_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} > ${output_dir}/${output_base_name}_samples.txt
endif
endif

# measure min spacing
set boundary_condition = 1;
set input_name = ${output_base_name}_samples;
set filtered_name = ${input_name}_filtered;

foreach class_option (0 1 2 'all')

if(${class_option} == 'all') then
cp ${output_dir}/${input_name}.txt ${output_dir}/${filtered_name}.txt 
else
grep "^${class_option}" ${output_dir}/${input_name}.txt > ${output_dir}/${filtered_name}.txt 
endif

echo ${class_option} ${input_name}
./src/Main/Release/MinSampleSpacing ${output_dir}/${filtered_name}.txt ${dimension} ${domain_size} ${boundary_condition}
end

# draw samples
foreach class_option (0 1 2 'all')

set draw_name = ${output_base_name}_${class_option};

if(${class_option} == 'all') then
cp ${output_dir}/${output_base_name}_samples.txt ${output_dir}/${draw_name}.txt 
else
grep "^${class_option}" ${output_dir}/${output_base_name}_samples.txt > ${output_dir}/${draw_name}.txt 
endif

src/Main/Release/DrawSamples ${output_dir}/${draw_name}.txt ${output_dir}/${draw_name}.fig ${dimension} ${dot_radius} 1 ${domain_size}
src/Scripts/fig2eps -f ${output_dir}/${draw_name}.fig
epstopdf ${output_dir}/${draw_name}.eps --outfile ${output_dir}/${draw_name}.pdf

end # foreach name

end # foreach class_weighting_scheme

endif

# power spectrum analysis for multiple classes with identical parameters
if(0) then

set dimension = 2;
set domain_size = "1 1";
set num_samples_one_class = 1300;

# set peak_method = random;
set refine_peak_method = random;

@ num_trials = ${num_samples_one_class};
set trough_method = random${num_trials};
set refine_trough_method = sphererandom${num_trials};
# set refine_trough_method = "";

set shuffle_method = "maxpeak";
set k_number = 2;
set shuffle_method = "throw";
set threshold_energy = "0.001";
# set shuffle_method = "gradualpeak0";
set refine_shuffle_method = "gradualpeak0";

set class_weighting_scheme = "fair";

set kernel_type = "gaussian";
set kernel_size_option = "constant"; # or optimal
set relative_kernel_size = 0.5;

set num_runs = 10;
set sft_size = 512;
set sft_scale = ${sft_size};

foreach num_class (1 2 3 4 9 16)

if(${num_class} == 1) then
set grid_res = 36;
set num_classes_sqrt = 1;
else if(${num_class} == 2) then
set grid_res = 50;
set num_classes_sqrt = 1;
else if(${num_class} == 3) then
set grid_res = 62;
set num_classes_sqrt = 1;
else if(${num_class} == 4) then
set grid_res = 75;
set num_classes_sqrt = 2;
else if(${num_class} == 8) then
set grid_res = 100;
set num_classes_sqrt = 2;
else if(${num_class} == 9) then
set grid_res = 100;
set num_classes_sqrt = 3;
else if(${num_class} == 16) then
set grid_res = 150;
set num_classes_sqrt = 4;
else if(${num_class} == 32) then
set grid_res = 200;
set num_classes_sqrt = 5;
else if(${num_class} == 64) then
set grid_res = 300;
set num_classes_sqrt = 8;
else
echo unsupported num_class;
exit(1);
endif

@ total_num_samples = ${num_class} * ${num_samples_one_class};

@ throw_num_trials = ${k_number} * ${total_num_samples};
@ throw_num_trials = ${k_number} * ${num_samples_one_class};
set throw_trough_method = random${throw_num_trials};

set energy_store_method = grid${grid_res};

@ moving_window_size = ${num_samples_one_class};
@ max_num_iterations = 2 * ${num_class} * ${num_samples_one_class};

set num_samples_per_class = "";
set i = 0;
while(${i} < ${num_class})
set num_samples_per_class = "${num_samples_per_class} ${num_samples_one_class}";
@ i = ${i} + 1;
end

set all_class_ids = "";
set i = 0;
while(${i} < ${num_class})
set all_class_ids = "${all_class_ids} ${i}";
@ i = ${i} + 1;
end

@ sft_scale_all = ${sft_size} * ${num_classes_sqrt};

set base_name = shuffle_${dimension}d_${num_class}c_${num_samples_one_class}_${energy_store_method}_${trough_method}_${class_weighting_scheme};
if(${kernel_size_option} == "optimal") then
set base_name = ${base_name}_ksizeopt;
endif

# optimal relative kernel size
if(${kernel_size_option} == "optimal") then
if(${num_class} < 2) then
set relative_kernel_size_opt = 52;
else if(${num_class} <= 4) then
@ relative_kernel_size_opt = 42 + ${num_class};
else
set relative_kernel_size_opt = 46;
endif
set relative_kernel_size = 0.${relative_kernel_size_opt};
endif

# sample set generation
set i = 0;
while(${i} < ${num_runs})

sleep 1;

./src/Main/Release/WhiteNoiseSamples ${dimension} ${num_class} ${num_samples_per_class} ${domain_size} > ${huge_dir}/${base_name}_init.txt

if(${shuffle_method} == "throw") then
time ./src/Main/Release/DartShuffling ${huge_dir}/${base_name}_init.txt ${dimension} ${domain_size} ${energy_store_method} ${peak_method} ${throw_trough_method} ${shuffle_method}${threshold_energy} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${total_num_samples} ${total_num_samples} > ${huge_dir}/${base_name}_${i}.txt
else
time ./src/Main/Release/DartShuffling ${huge_dir}/${base_name}_init.txt ${dimension} ${domain_size} ${energy_store_method} ${peak_method} ${trough_method} ${shuffle_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} > ${huge_dir}/${base_name}_${i}.txt
endif

if(${refine_trough_method} != "") then
time ./src/Main/Release/DartShuffling ${huge_dir}/${base_name}_${i}.txt ${dimension} ${domain_size} ${energy_store_method} ${refine_peak_method} ${refine_trough_method} ${refine_shuffle_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} > ${huge_dir}/${base_name}_refine_${i}.txt
endif

# measure min spacing
#if(${num_runs} == 1) then
if(${i} == 0) then
set boundary_condition = 1;
set input_names = ${base_name}_${i};
if(${refine_trough_method} != "") then
set input_names = "${input_names} ${base_name}_refine_${i}";
endif

foreach input_name (${input_names})
set filtered_name = ${input_name}_filtered;

foreach class_option (0 all)

if(${class_option} == 'all') then
cp ${huge_dir}/${input_name}.txt ${huge_dir}/${filtered_name}.txt 
else
grep "^${class_option}" ${huge_dir}/${input_name}.txt > ${huge_dir}/${filtered_name}.txt 
endif

echo ${class_option} ${input_name}
./src/Main/Release/MinSampleSpacing ${huge_dir}/${filtered_name}.txt ${dimension} ${domain_size} ${boundary_condition}
end # foreach class_option
end # foreach input_name
endif

if(${refine_trough_method} != "") then
mv ${huge_dir}/${base_name}_refine_${i}.txt ${huge_dir}/${base_name}_${i}.txt;
endif

@ i = ${i} + 1;
end # while

# power spectrum
if(${sft_size} > 0) then
foreach class (0 all)

set all_pfm_files = "";
set i = 0;
while(${i} < ${num_runs})

set input_name = ${base_name}_${i};
set output_name = ${input_name}_c${class};

if(${class} == "all") then
src/Main/Release/SampleSelector ${huge_dir}/${input_name}.txt ${dimension} ${all_class_ids} > ${huge_dir}/${output_name}_selected.txt
set my_sft_scale = ${sft_scale_all};
else
src/Main/Release/SampleSelector ${huge_dir}/${input_name}.txt ${dimension} ${class} > ${huge_dir}/${output_name}_selected.txt
set my_sft_scale = ${sft_scale};
endif

./src/Sain/Release/SFT ${huge_dir}/${output_name}_selected.txt ${huge_dir}/${output_name}.pfm ${dimension} 1 1 ${sft_size} ${my_sft_scale}

set all_pfm_files = "${all_pfm_files} ${huge_dir}/${output_name}.pfm";

@ i = ${i} + 1;
end

echo ${all_pfm_files} > ${huge_dir}/all_pfm_files_list.txt;

./src/Sain/Release/RadialPlot ${huge_dir}/all_pfm_files_list.txt ${output_dir}/${base_name}_c${class}_average${num_runs}.pfm ${output_dir}/${base_name}_c${class}_average${num_runs}_mean.txt ${output_dir}/${base_name}_c${class}_average${num_runs}_variance.txt
rm ${huge_dir}/all_pfm_files_list.txt;

./src/Sain/Release/PFM2PPM ${output_dir}/${base_name}_c${class}_average${num_runs}.pfm ${output_dir}/${base_name}_c${class}_average${num_runs}.ppm 1 1 1

end # foreach class
endif # (${sft_size} > 0) then

end # foreach num_class

endif

# same total sample count comparison
if(0) then

set dimension = 2;
set domain_size = "1 1";
set shuffle_method = "maxpeak";
set shuffle_method = "gradualpeak0";
set kernel_type = "gaussian";
set relative_kernel_size = 0.5;

set grid_res = 36;
set energy_store_method = grid${grid_res};

foreach num_class (1 2)

if(${num_class} == 1) then
set num_samples_one_class = 1300;
else if(${num_class} == 2) then
set num_samples_one_class = 650;
else
echo unknown number of class;
exit(1);
endif

@ total_num_samples = ${num_class} * ${num_samples_one_class};
@ num_trials = ${total_num_samples};
set trough_method = random${num_trials};

@ moving_window_size = ${total_num_samples};
@ max_num_iterations = 2 * ${moving_window_size};

set num_samples_per_class = "";
set i = 0;
while(${i} < ${num_class})
set num_samples_per_class = "${num_samples_per_class} ${num_samples_one_class}";
@ i = ${i} + 1;
end

foreach class_weighting_scheme (fair all)

set base_name = shuffle_${dimension}d_${num_class}c_${num_samples_one_class}_${energy_store_method}_${trough_method}_${class_weighting_scheme};

./src/Main/Release/WhiteNoiseSamples ${dimension} ${num_class} ${num_samples_per_class} ${domain_size} > ${huge_dir}/${base_name}_init.txt

time ./src/Main/Release/DartShuffling ${huge_dir}/${base_name}_init.txt ${dimension} ${domain_size} ${energy_store_method} ${peak_method} ${trough_method} ${shuffle_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} > ${huge_dir}/${base_name}.txt

# measure min spacing
if(0) then
set boundary_condition = 1;
set input_name = ${base_name};
set filtered_name = ${input_name}_filtered;

foreach class_option (all)

if(${class_option} == 'all') then
cp ${huge_dir}/${input_name}.txt ${huge_dir}/${filtered_name}.txt 
else
grep "^${class_option}" ${huge_dir}/${input_name}.txt > ${huge_dir}/${filtered_name}.txt 
endif

echo ${class_option} ${input_name}
./src/Main/Release/MinSampleSpacing ${huge_dir}/${filtered_name}.txt ${dimension} ${domain_size} ${boundary_condition}
end
endif

end # foreach class weighting scheme
end # foreach num_class 

endif

# 3 x 400 result to compare with color halftoning
if(0) then

set dimension = 2;
set domain_size = "1 1";

set num_class = 3;
set num_samples_per_class = 400;

set num_samples = "";
set i = 0;
while(${i} < ${num_class})
set num_samples = "${num_samples} ${num_samples_per_class}";
@ i = ${i} + 1;
end

set num_samples_text = ${num_samples_per_class};

@ total_num_samples = ${num_class} * ${num_samples_per_class};
set k_number = 8;

@ num_trials = ${k_number} * ${total_num_samples};
set energy_store_method = grid40;

set trough_method = random${num_trials};
set shuffle_method = "thrower";
set threshold_energy = "0.001";

set refine_num_trials = ${num_samples_per_class};
set refine_trough_method = sphererandom${num_trials};
set refine_shuffle_method = "gradualpeak0";

set kernel_type = "gaussian";
set relative_kernel_size = 0.5;
set moving_window_size = ${total_num_samples};
@ max_num_iterations = 2 * ${total_num_samples};

set dot_radius = 0.005;

foreach class_weighting_scheme (fair)

set output_base_name = shuffle_${dimension}d_${num_class}c_${num_samples_text}_${energy_store_method}_${trough_method}_${shuffle_method}_${class_weighting_scheme};

if(${no_compute}) then
else
./src/Main/Release/WhiteNoiseSamples ${dimension} ${num_class} ${num_samples} ${domain_size} > ${output_dir}/${output_base_name}_init_samples.txt

./src/Main/Release/DartShuffling ${output_dir}/${output_base_name}_init_samples.txt ${dimension} ${domain_size} ${energy_store_method} ${peak_method} ${trough_method} ${shuffle_method}${threshold_energy} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${total_num_samples} ${total_num_samples} > ${output_dir}/${output_base_name}_samples.txt

./src/Main/Release/DartShuffling ${output_dir}/${output_base_name}_samples.txt ${dimension} ${domain_size} ${energy_store_method} ${peak_method} ${refine_trough_method} ${refine_shuffle_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} > ${output_dir}/${output_base_name}_refined_samples.txt

endif

# measure min spacing
set boundary_condition = 1;
foreach input_name (${output_base_name}_samples ${output_base_name}_refined_samples)
set filtered_name = ${input_name}_filtered;

foreach class_option (0 1 2 'all')

if(${class_option} == 'all') then
cp ${output_dir}/${input_name}.txt ${output_dir}/${filtered_name}.txt 
else
grep "^${class_option}" ${output_dir}/${input_name}.txt > ${output_dir}/${filtered_name}.txt 
endif

echo ${class_option} ${input_name}
./src/Main/Release/MinSampleSpacing ${output_dir}/${filtered_name}.txt ${dimension} ${domain_size} ${boundary_condition}

end # foreach class_option
end # foreach input_name

# draw samples
foreach class_option ('all')

set draw_name = ${output_base_name}_${class_option};

if(${class_option} == 'all') then
cp ${output_dir}/${output_base_name}_samples.txt ${output_dir}/${draw_name}.txt 
else
grep "^${class_option}" ${output_dir}/${output_base_name}_samples.txt > ${output_dir}/${draw_name}.txt 
endif

src/Main/Release/DrawSamples ${output_dir}/${draw_name}.txt ${output_dir}/${draw_name}.fig ${dimension} ${dot_radius} 1 ${domain_size}
src/Scripts/fig2eps -f ${output_dir}/${draw_name}.fig
epstopdf ${output_dir}/${draw_name}.eps --outfile ${output_dir}/${draw_name}.pdf

end # foreach name

end # foreach class_weighting_scheme

endif

# 2c x 100 comparison with lloyd relaxation
# 3 cases: initial soft dart throwing, shuffle refinement, and lloyd refinement
if(0) then

set dimension = 2;
set domain_size = "1 1";
set num_class = 2;
set num_samples_one_class = 100;
set num_samples_text = ${num_samples_one_class};
set num_samples = "100 100";

# parameters for soft disk sampling
set energy_store_method = grid10;

set init_peak_method = default; # doesn't matter
set refine_peak_method = default;

set k_number = 32;

@ total_num_samples = ${num_class} * ${num_samples_one_class};

@ num_init_trials = ${k_number} * ${num_samples_one_class};
set num_refine_trials = ${total_num_samples};

set init_trough_method = random${num_init_trials};
set refine_trough_method = sphererandom${num_refine_trials};
# set refine_trough_method = "";

set init_shuffle_method = "throw";
set threshold_energy = "0.001";
set refine_shuffle_method = "gradualpeak";

set class_weighting_scheme = "fair";
set lloyd_class_weighting_scheme = "voronoi";

set kernel_type = "gaussian";
set relative_kernel_size = 0.5;

set moving_window_size = ${total_num_samples};
@ max_num_iterations = 10 * ${total_num_samples};

# parameters for lloyd relaxation
set voronoi_scheme = lloyd;
set centroid_scheme = none;

set num_points_per_sample = 128; #1024;
@ num_points_per_class = ${num_samples_one_class} * ${num_points_per_sample};
@ num_points = ${num_points_per_class} * ${num_class};

set min_movement = 0.000001; # roughly smaller than 1/(num_points * num_samples)
set max_num_relaxations = 100;

set dot_radius = 0.008;

set init_base_name = shuffle_${dimension}d_${num_class}c_${num_samples_text}_${energy_store_method}_${init_trough_method}_${init_shuffle_method}_${class_weighting_scheme};

set refine_base_name = shuffle_${dimension}d_${num_class}c_${num_samples_text}_${energy_store_method}_${refine_trough_method}_${refine_shuffle_method}_${class_weighting_scheme};

set init = "throw";
set lloyd_base_name = lloyd_${dimension}d_${num_class}c_${num_samples_text}_${voronoi_scheme}_${centroid_scheme}_${init}_${lloyd_class_weighting_scheme};

if(${no_compute}) then
else
# white noise
./src/Main/Release/WhiteNoiseSamples ${dimension} ${num_class} ${num_samples} ${domain_size} > ${output_dir}/${init_base_name}_init_samples.txt

# initial soft dart throwing
echo initial soft dart throwing
./src/Main/Release/DartShuffling ${output_dir}/${init_base_name}_init_samples.txt ${dimension} ${domain_size} ${energy_store_method} ${init_peak_method} ${init_trough_method} ${init_shuffle_method}${threshold_energy} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${total_num_samples} ${total_num_samples} > ${output_dir}/${init_base_name}_samples.txt

# refine by dart shuffling
echo refine by dart shuffling
./src/Main/Release/DartShuffling ${output_dir}/${init_base_name}_samples.txt ${dimension} ${domain_size} ${energy_store_method} ${refine_peak_method} ${refine_trough_method} ${refine_shuffle_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} > ${output_dir}/${refine_base_name}_samples.txt

# refine by relaxation
echo refine by relaxation
@ num_remaining_points = ${num_points} - ${total_num_samples};

./src/Main/Release/WhiteNoiseSamples ${dimension} 1 ${num_remaining_points} ${domain_size} > ${output_dir}/${lloyd_base_name}_init_points.txt

./src/Main/Release/ProcessLloydSites ${output_dir}/${lloyd_base_name}_init_points.txt ${output_dir}/${lloyd_base_name}_init_points.txt ${dimension} "decrement" 0;

cat ${output_dir}/${init_base_name}_samples.txt ${output_dir}/${lloyd_base_name}_init_points.txt > ${output_dir}/${lloyd_base_name}_points.txt

./src/Main/Release/LloydRelaxation ${output_dir}/${lloyd_base_name}_points.txt ${dimension} ${domain_size} ${voronoi_scheme} ${centroid_scheme} ${lloyd_class_weighting_scheme} ${min_movement} ${max_num_relaxations} > ${output_dir}/${lloyd_base_name}_samples.txt
endif # if no_compute

# draw samples and measure min spacing
foreach output_name (${init_base_name}_samples ${refine_base_name}_samples ${lloyd_base_name}_samples)

set draw_name = ${output_name};
./src/Main/Release/DrawSamples ${output_dir}/${draw_name}.txt ${output_dir}/${draw_name}.fig 2 ${dot_radius} 1 ${domain_size}
./src/Scripts/fig2eps -f ${output_dir}/${draw_name}.fig
epstopdf ${output_dir}/${draw_name}.eps --outfile ${output_dir}/${draw_name}.pdf

# measure min spacing
set boundary_condition = 1;

set input_name = ${output_name};
set filtered_name = ${input_name}_filtered;

foreach class_option (0 1 'all')

if(${class_option} == 'all') then
cp ${output_dir}/${input_name}.txt ${output_dir}/${filtered_name}.txt 
else
grep "^${class_option}" ${output_dir}/${input_name}.txt > ${output_dir}/${filtered_name}.txt 
endif

echo ${class_option} ${input_name}
./src/Main/Release/MinSampleSpacing ${output_dir}/${filtered_name}.txt ${dimension} ${domain_size} ${boundary_condition}
end # foreach class_option

end # foreach output_name

endif

# energy function visualization
if(0) then

set dimension = 2;
set domain_size = "1 1";

set num_class = 3;
set num_samples_per_class = "8 32 196";
set num_samples_text = 8n32n196;
set total_num_samples = 236;

set energy_store_method = grid15;
set peak_method = "default";

set k_number = 8;
@ num_init_trials = ${k_number} * ${total_num_samples};
set init_trough_method = random${num_init_trials};

set num_refine_trials = ${total_num_samples};
set refine_trough_method = sphererandom${num_refine_trials};
set refine_trough_method = "";

set init_shuffle_method = "throw";
set threshold_energy = "0.01";

set refine_shuffle_method = "gradualpeak";

set kernel_type = "gaussian";
set relative_kernel_size = 0.5;

set moving_window_size = ${total_num_samples};
@ max_num_iterations = 10 * ${moving_window_size};

set energy_class_id = -1;
set energy_array_size = 256; # visualization and debug

set dot_radius = 0.01;

foreach class_weighting_scheme (fair)

set output_base_name = shuffle_${dimension}d_${num_class}c_${num_samples_text}_${energy_store_method}_${init_trough_method}_${class_weighting_scheme};

./src/Main/Release/WhiteNoiseSamples ${dimension} ${num_class} ${num_samples_per_class} ${domain_size} > ${output_dir}/${output_base_name}_init_samples.txt

./src/Main/Release/DartShuffling ${output_dir}/${output_base_name}_init_samples.txt ${dimension} ${domain_size} ${energy_store_method} ${peak_method} ${init_trough_method} ${init_shuffle_method}${threshold_energy} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${total_num_samples} ${total_num_samples} > ${output_dir}/${output_base_name}_samples.txt

if(${refine_trough_method} != "") then
./src/Main/Release/DartShuffling ${output_dir}/${output_base_name}_samples.txt ${dimension} ${domain_size} ${energy_store_method} ${peak_method} ${refine_trough_method} ${refine_shuffle_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} > ${output_dir}/${output_base_name}_refined_samples.txt
endif

# measure min spacing
set boundary_condition = 1;

set input_names = ${output_base_name}_samples;

if(${refine_trough_method} != "") then
set input_names = "${input_names} ${output_base_name}_refined_samples";
endif

foreach input_name (${input_names})
set filtered_name = ${input_name}_filtered;

foreach class_option (0 1 2 'all')

if(${class_option} == 'all') then
cp ${output_dir}/${input_name}.txt ${output_dir}/${filtered_name}.txt 
else
grep "^${class_option}" ${output_dir}/${input_name}.txt > ${output_dir}/${filtered_name}.txt 
endif

echo ${class_option} ${input_name}
./src/Main/Release/MinSampleSpacing ${output_dir}/${filtered_name}.txt ${dimension} ${domain_size} ${boundary_condition}
end # foreach class_option
end # foreach input_name

if(${refine_trough_method} != "") then
mv ${output_dir}/${output_base_name}_refined_samples.txt ${output_dir}/${output_base_name}_samples.txt;
endif

./src/Main/Release/DrawSamples ${output_dir}/${output_base_name}_samples.txt ${output_dir}/${output_base_name}.fig 2 ${dot_radius} 1 ${domain_size}
./src/Scripts/fig2eps -f ${output_dir}/${output_base_name}.fig
epstopdf ${output_dir}/${output_base_name}.eps --outfile ${output_dir}/${output_base_name}.pdf

if(${energy_array_size} > 0) then

if(${energy_class_id} >= 0) then
set energy_class_ids = ${energy_class_id};
else
set i = 0;
set energy_class_ids = "";
while(${i} < ${num_class})
set energy_class_ids = "${energy_class_ids} ${i}";
@ i = ${i} + 1;
end
endif

foreach current_class_id (${energy_class_ids})
./src/Main/Release/DartShufflingEnergy ${output_dir}/${output_base_name}_samples.txt ${dimension} ${domain_size} ${energy_store_method} ${peak_method} ${init_trough_method} ${shuffle_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} 0 0 ${current_class_id} ${energy_array_size} ${output_dir}/${output_base_name}_energy_${current_class_id}.pfm ${output_dir}/${output_base_name}_history.txt > /dev/null;
end

if(${energy_class_id} >= 0) then
mv ${output_dir}/${output_base_name}_energy_${energy_class_id}.pfm ${output_dir}/${output_base_name}_energy.pfm;
else
if(${num_class} != 3) then
echo num_class must be 3 for merging 3 pfm;
exit(1);
endif

./src/Sain/Release/Merge3PFM ${output_dir}/${output_base_name}_energy_0.pfm ${output_dir}/${output_base_name}_energy_1.pfm ${output_dir}/${output_base_name}_energy_2.pfm  ${output_dir}/${output_base_name}_energy.pfm
endif

./src/Sain/Release/PFM2PPM ${output_dir}/${output_base_name}_energy.pfm ${output_dir}/${output_base_name}_energy.ppm 1 1 2
endif

end # foreach class_weighting_scheme

endif

# nonuniform 2 class with different number of samples
# match up with the hard dart throwing case
if(0) then

set dimension = 2;
set domain_size = "1 1";

set num_class = 2;
set all_class_ids = "0 1";

set total_num_samples = 10400;

set grid_res = 100;
set energy_store_method = grid${grid_res};

set num_samples_one_class = 1300; # nothing real, just for computation equivalence to the uniform multi class

set init_peak_method = default;
set refine_peak_method = random;

set k_number = 2;
@ init_num_trials = ${k_number} * ${num_samples_one_class};
set init_trough_method = random${init_num_trials};

@ refine_num_trials = ${num_samples_one_class};
set refine_trough_method = sphererandom${refine_num_trials};
# set refine_trough_method = "";

set init_shuffle_method = "throw";
set threshold_energy = "0.001";
# set shuffle_method = "gradualpeak0";
set refine_shuffle_method = "gradualpeak0";

set class_weighting_scheme = "fair";

set kernel_type = "gaussian";
set relative_kernel_size = 0.5;

@ moving_window_size = ${num_samples_one_class};
@ max_num_iterations = 2 * ${total_num_samples};

set num_runs = 10;
set sft_size = 512;
set sft_scale = ${sft_size};

foreach r_option (0p011 0p01sqrt2 0p02 0p04 0p08)# 0p16)

if(${r_option} == '0p01') then
set num_samples = "5200 5200";
else if(${r_option} == '0p011') then
set num_samples = "4297 6103";
else if(${r_option} == '0p01sqrt2') then
set num_samples = "2600 7800";
else if(${r_option} == '0p02') then
set num_samples = "1300 9100";
else if(${r_option} == '0p04') then
set num_samples = "325 10075";
else if(${r_option} == '0p08') then
set num_samples = "81 10319";
else if(${r_option} == '0p16') then
set num_samples = "20 10380";
else
echo unknown r value;
exit(1);
endif

set r_text = ${r_option}m0p01osqrt2;

set base_name = shuffle_${dimension}d_${num_class}c_${r_text}_${energy_store_method}_${refine_trough_method}_${class_weighting_scheme};

# sample set generation
set i = 0;
while(${i} < ${num_runs})

sleep 1;

./src/Main/Release/WhiteNoiseSamples ${dimension} ${num_class} ${num_samples} ${domain_size} > ${huge_dir}/${base_name}_init.txt

if(${init_shuffle_method} == "throw") then
time ./src/Main/Release/DartShuffling ${huge_dir}/${base_name}_init.txt ${dimension} ${domain_size} ${energy_store_method} ${init_peak_method} ${init_trough_method} ${init_shuffle_method}${threshold_energy} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${total_num_samples} ${total_num_samples} > ${huge_dir}/${base_name}_${i}.txt
else
time ./src/Main/Release/DartShuffling ${huge_dir}/${base_name}_init.txt ${dimension} ${domain_size} ${energy_store_method} ${init_peak_method} ${init_trough_method} ${init_shuffle_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} > ${huge_dir}/${base_name}_${i}.txt
endif

if(${refine_trough_method} != "") then
time ./src/Main/Release/DartShuffling ${huge_dir}/${base_name}_${i}.txt ${dimension} ${domain_size} ${energy_store_method} ${refine_peak_method} ${refine_trough_method} ${refine_shuffle_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} > ${huge_dir}/${base_name}_refine_${i}.txt
endif

# measure min spacing
#if(${num_runs} == 1) then
if(${i} == 0) then
set boundary_condition = 1;
set input_names = ${base_name}_${i};
if(${refine_trough_method} != "") then
set input_names = "${input_names} ${base_name}_refine_${i}";
endif

foreach input_name (${input_names})
set filtered_name = ${input_name}_filtered;

foreach class_option (0 1 all)

if(${class_option} == 'all') then
cp ${huge_dir}/${input_name}.txt ${huge_dir}/${filtered_name}.txt 
else
grep "^${class_option}" ${huge_dir}/${input_name}.txt > ${huge_dir}/${filtered_name}.txt 
endif

echo ${class_option} ${input_name}
./src/Main/Release/MinSampleSpacing ${huge_dir}/${filtered_name}.txt ${dimension} ${domain_size} ${boundary_condition}
end # foreach class_option
end # foreach input_name
endif

if(${refine_trough_method} != "") then
mv ${huge_dir}/${base_name}_refine_${i}.txt ${huge_dir}/${base_name}_${i}.txt;
endif

@ i = ${i} + 1;
end # while

# power spectrum
if(${sft_size} > 0) then
foreach class (0 1 all)

set all_pfm_files = "";
set i = 0;
while(${i} < ${num_runs})

set input_name = ${base_name}_${i};
set output_name = ${input_name}_c${class};

if(${class} == "all") then
src/Main/Release/SampleSelector ${huge_dir}/${input_name}.txt ${dimension} ${all_class_ids} > ${huge_dir}/${output_name}_selected.txt
else
src/Main/Release/SampleSelector ${huge_dir}/${input_name}.txt ${dimension} ${class} > ${huge_dir}/${output_name}_selected.txt
endif

./src/Sain/Release/SFT ${huge_dir}/${output_name}_selected.txt ${huge_dir}/${output_name}.pfm ${dimension} 1 1 ${sft_size} ${sft_scale}

set all_pfm_files = "${all_pfm_files} ${huge_dir}/${output_name}.pfm";

@ i = ${i} + 1;
end

echo ${all_pfm_files} > ${huge_dir}/all_pfm_files_list.txt;

./src/Sain/Release/RadialPlot ${huge_dir}/all_pfm_files_list.txt ${output_dir}/${base_name}_c${class}_average${num_runs}.pfm ${output_dir}/${base_name}_c${class}_average${num_runs}_mean.txt ${output_dir}/${base_name}_c${class}_average${num_runs}_variance.txt
rm ${huge_dir}/all_pfm_files_list.txt;

./src/Sain/Release/PFM2PPM ${output_dir}/${base_name}_c${class}_average${num_runs}.pfm ${output_dir}/${base_name}_c${class}_average${num_runs}.ppm 1 1 1

end # foreach class
endif # (${sft_size} > 0) then

end # foreach r_option

endif

# class priority, sequential generation, matching 0p040p020p01
if(0) then

set dimension = 2;
set domain_size = "1 1";
set num_class = 3;
set all_class_ids = "0 1 2";
set num_samples = "323 1295 5183";
set num_samples_array = (323 1295 5183);
set num_samples_text = 323n1295n5183;
set total_num_samples = 6801;

set num_samples_one_class = 1300; # nothing real, just for computation equivalence to the uniform multi class

set init_peak_method = default;
set refine_peak_method = random;

set k_number = 2;
@ init_num_trials = ${k_number} * ${num_samples_one_class};
set init_trough_method = random${init_num_trials};

@ refine_num_trials = ${num_samples_one_class};
set refine_trough_method = sphererandom${refine_num_trials};
# set refine_trough_method = "";

set init_shuffle_method = "throw";
set threshold_energy = "0.001";
set refine_shuffle_method = "gradualpeak0";

set class_weighting_scheme = "fair";

set kernel_type = "gaussian";
set relative_kernel_size = 0.5;

@ moving_window_size = ${num_samples_one_class};
@ max_num_iterations = 2 * ${total_num_samples};

set num_runs = 10;
set sft_size = 512;
set sft_scale = ${sft_size};

set which_run = 0;

while(${which_run} < ${num_runs})

# sample generation
set current_num_class = 1;
while(${current_num_class} <= ${num_class})

set num_samples = "";
set class_text = "";
set last_class_text = "";
set i = 0;
while(${i} < ${current_num_class})
@ i = ${i} + 1;
set num_samples = "${num_samples} $num_samples_array[${i}]";
set class_text = "${class_text}1c";
if(${i} < ${current_num_class}) then
set last_class_text = "${last_class_text}1c";
endif
end

set grid_size = 20;
set i = 1;
while(${i} < ${current_num_class})
@ i = ${i} + 1;
@ grid_size = 2 * ${grid_size};
end

set energy_store_method = grid${grid_size};

set base_name = shuffle_${dimension}d_${class_text}_${num_samples_text};
set lase_name = shuffle_${dimension}d_${last_class_text}_${num_samples_text};

set num_samples_current_class = $num_samples_array[${current_num_class}];

./src/Main/Release/WhiteNoiseSamples ${dimension} 1 ${num_samples_current_class} ${domain_size} > ${huge_dir}/${base_name}_init.txt

@ current_num_class_minus_one = ${current_num_class} - 1;
set i = 0;
while(${i} < ${current_num_class_minus_one})
./src/Main/Release/ProcessLloydSites ${huge_dir}/${base_name}_init.txt ${huge_dir}/${base_name}_init.txt ${dimension} "increment" ${i}
@ i = ${i} + 1;
end

if(${refine_trough_method} != "") then
set tail_name = ${refine_trough_method}_${init_shuffle_method}_${class_weighting_scheme};
else
set tail_name = ${init_trough_method}_${init_shuffle_method}_${class_weighting_scheme};
endif

set output_base_name = ${base_name}_${tail_name};

if(${current_num_class} > 1) then
set last_base_name = ${lase_name}_${tail_name};

cat ${huge_dir}/${last_base_name}_${which_run}.txt ${huge_dir}/${base_name}_init.txt > ${huge_dir}/${base_name}_init_cat.txt;
mv ${huge_dir}/${base_name}_init_cat.txt ${huge_dir}/${base_name}_init.txt;
endif

./src/Main/Release/DartShuffling ${huge_dir}/${base_name}_init.txt ${dimension} ${domain_size} ${energy_store_method} ${init_peak_method} ${init_trough_method} ${init_shuffle_method}${threshold_energy} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${num_samples_current_class} ${num_samples_current_class} > ${huge_dir}/${output_base_name}_${which_run}.txt

if(${refine_trough_method} != "") then
time ./src/Main/Release/DartShuffling ${huge_dir}/${output_base_name}_${which_run}.txt ${dimension} ${domain_size} ${energy_store_method} ${refine_peak_method} ${refine_trough_method} ${refine_shuffle_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} > ${huge_dir}/${output_base_name}_refine_${which_run}.txt
endif

if(${refine_trough_method} != "") then
mv ${huge_dir}/${output_base_name}_refine_${which_run}.txt ${huge_dir}/${output_base_name}_${which_run}.txt;
endif

./src/Main/Release/ProcessLloydSites ${huge_dir}/${output_base_name}_${which_run}.txt ${huge_dir}/${output_base_name}_${which_run}.txt ${dimension} "fasten" 

@ current_num_class = ${current_num_class} + 1;
end # while

./src/Main/Release/ProcessLloydSites ${huge_dir}/${output_base_name}_${which_run}.txt ${huge_dir}/${output_base_name}_${which_run}.txt ${dimension} "loosen" 

# measure min spacing
#if(${num_runs} == 1) then
if(${which_run} == 0) then
set boundary_condition = 1;
set input_names = ${output_base_name}_${which_run};

foreach input_name (${input_names})
set filtered_name = ${input_name}_filtered;

foreach class_option (0 1 2 all)

if(${class_option} == 'all') then
cp ${huge_dir}/${input_name}.txt ${huge_dir}/${filtered_name}.txt 
else
grep "^${class_option}" ${huge_dir}/${input_name}.txt > ${huge_dir}/${filtered_name}.txt 
endif

echo ${class_option} ${input_name}
./src/Main/Release/MinSampleSpacing ${huge_dir}/${filtered_name}.txt ${dimension} ${domain_size} ${boundary_condition}
end # foreach class_option
end # foreach input_name

endif #if(${which_run} == 0)

@ which_run = ${which_run} + 1;
end # while

# power spectrum
if(${sft_size} > 0) then
foreach class (0 1 2 all)

set all_pfm_files = "";
set i = 0;
while(${i} < ${num_runs})

set input_name = ${output_base_name}_${i};
set output_name = ${input_name}_c${class};

if(${class} == "all") then
src/Main/Release/SampleSelector ${huge_dir}/${input_name}.txt ${dimension} ${all_class_ids} > ${huge_dir}/${output_name}_selected.txt
else
src/Main/Release/SampleSelector ${huge_dir}/${input_name}.txt ${dimension} ${class} > ${huge_dir}/${output_name}_selected.txt
endif

./src/Sain/Release/SFT ${huge_dir}/${output_name}_selected.txt ${huge_dir}/${output_name}.pfm ${dimension} 1 1 ${sft_size} ${sft_scale}

set all_pfm_files = "${all_pfm_files} ${huge_dir}/${output_name}.pfm";

@ i = ${i} + 1;
end

echo ${all_pfm_files} > ${huge_dir}/all_pfm_files_list.txt;

./src/Sain/Release/RadialPlot ${huge_dir}/all_pfm_files_list.txt ${output_dir}/${base_name}_c${class}_average${num_runs}.pfm ${output_dir}/${base_name}_c${class}_average${num_runs}_mean.txt ${output_dir}/${base_name}_c${class}_average${num_runs}_variance.txt
rm ${huge_dir}/all_pfm_files_list.txt;

./src/Sain/Release/PFM2PPM ${output_dir}/${base_name}_c${class}_average${num_runs}.pfm ${output_dir}/${base_name}_c${class}_average${num_runs}.ppm 1 1 1

end # foreach class
endif # (${sft_size} > 0) then

endif 

# class priority, concurrent generation, matching 0p040p020p01
if(0) then

set dimension = 2;
set domain_size = "1 1";
set num_class = 3;
set all_class_ids = "0 1 2";
set num_samples = "323 1295 5183";
set num_samples_text = 323n1295n5183;
set total_num_samples = 6801;

set grid_size = 80;
set energy_store_method = grid${grid_size};

set num_samples_one_class = 1300; # nothing real, just for computation equivalence to the uniform multi class

set init_peak_method = default;
set refine_peak_method = random;

set k_number = 2;
@ init_num_trials = ${k_number} * ${num_samples_one_class};
set init_trough_method = random${init_num_trials};

@ refine_num_trials = ${num_samples_one_class};
set refine_trough_method = sphererandom${refine_num_trials};
# set refine_trough_method = "";

set init_shuffle_method = "throw";
set threshold_energy = "0.001";
set refine_shuffle_method = "gradualpeak0";

set class_weighting_scheme = "fair";

set relative_kernel_size = 0.5;

@ moving_window_size = ${num_samples_one_class};
@ max_num_iterations = 2 * ${total_num_samples};

set num_runs = 10;
set sft_size = 512;
set sft_scale = ${sft_size};

foreach kernel_type ("gaussian" "uniform")

set which_run = 0;
while(${which_run} < ${num_runs})

# sample generation
set base_name = shuffle_${dimension}d_${num_class}c_${num_samples_text};

./src/Main/Release/WhiteNoiseSamples ${dimension} ${num_class} ${num_samples} ${domain_size} > ${huge_dir}/${base_name}_init.txt

if(${refine_trough_method} != "") then
set tail_name = ${refine_trough_method}_${init_shuffle_method}_${class_weighting_scheme}_${kernel_type};
else
set tail_name = ${init_trough_method}_${init_shuffle_method}_${class_weighting_scheme}_${kernel_type};
endif

set output_base_name = ${base_name}_${tail_name};

./src/Main/Release/DartShuffling ${huge_dir}/${base_name}_init.txt ${dimension} ${domain_size} ${energy_store_method} ${init_peak_method} ${init_trough_method} ${init_shuffle_method}${threshold_energy} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${total_num_samples} ${total_num_samples} > ${huge_dir}/${output_base_name}_${which_run}.txt

if(${refine_trough_method} != "") then
./src/Main/Release/DartShuffling ${huge_dir}/${output_base_name}_${which_run}.txt ${dimension} ${domain_size} ${energy_store_method} ${refine_peak_method} ${refine_trough_method} ${refine_shuffle_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} > ${huge_dir}/${output_base_name}_refine_${which_run}.txt
endif

if(${refine_trough_method} != "") then
mv ${huge_dir}/${output_base_name}_refine_${which_run}.txt ${huge_dir}/${output_base_name}_${which_run}.txt;
endif

# measure min spacing
#if(${num_runs} == 1) then
if(${which_run} == 0) then
set boundary_condition = 1;
set input_names = ${output_base_name}_${which_run};

foreach input_name (${input_names})
set filtered_name = ${input_name}_filtered;

foreach class_option (0 1 2 all)

if(${class_option} == 'all') then
cp ${huge_dir}/${input_name}.txt ${huge_dir}/${filtered_name}.txt 
else
grep "^${class_option}" ${huge_dir}/${input_name}.txt > ${huge_dir}/${filtered_name}.txt 
endif

echo ${class_option} ${input_name}
./src/Main/Release/MinSampleSpacing ${huge_dir}/${filtered_name}.txt ${dimension} ${domain_size} ${boundary_condition}
end # foreach class_option
end # foreach input_name

endif #if(${which_run} == 0)

@ which_run = ${which_run} + 1;
end # while

# power spectrum
if(${sft_size} > 0) then
foreach class (0 1 2 all)

set all_pfm_files = "";
set i = 0;
while(${i} < ${num_runs})

set input_name = ${output_base_name}_${i};
set output_name = ${input_name}_c${class};

if(${class} == "all") then
src/Main/Release/SampleSelector ${huge_dir}/${input_name}.txt ${dimension} ${all_class_ids} > ${huge_dir}/${output_name}_selected.txt
else
src/Main/Release/SampleSelector ${huge_dir}/${input_name}.txt ${dimension} ${class} > ${huge_dir}/${output_name}_selected.txt
endif

./src/Sain/Release/SFT ${huge_dir}/${output_name}_selected.txt ${huge_dir}/${output_name}.pfm ${dimension} 1 1 ${sft_size} ${sft_scale}

set all_pfm_files = "${all_pfm_files} ${huge_dir}/${output_name}.pfm";

@ i = ${i} + 1;
end

echo ${all_pfm_files} > ${huge_dir}/all_pfm_files_list.txt;

./src/Sain/Release/RadialPlot ${huge_dir}/all_pfm_files_list.txt ${output_dir}/${output_base_name}_c${class}_average${num_runs}.pfm ${output_dir}/${output_base_name}_c${class}_average${num_runs}_mean.txt ${output_dir}/${output_base_name}_c${class}_average${num_runs}_variance.txt
rm ${huge_dir}/all_pfm_files_list.txt;

./src/Sain/Release/PFM2PPM ${output_dir}/${output_base_name}_c${class}_average${num_runs}.pfm ${output_dir}/${output_base_name}_c${class}_average${num_runs}.ppm 1 1 1

end # foreach class
endif # (${sft_size} > 0) then

end # foreach kernel_type

endif 

# 1 class ground truth
if(0) then

set dimension = 2;
set domain_size = "1 1";
set num_class = 1;

set all_class_ids = 0;

set num_samples_one_class = 1300; # nothing real, just for computation equivalence to the uniform multi class

set init_peak_method = default;
set refine_peak_method = random;

set k_number = 2;
@ init_num_trials = ${k_number} * ${num_samples_one_class};
set init_trough_method = random${init_num_trials};

@ refine_num_trials = ${num_samples_one_class};
set refine_trough_method = sphererandom${refine_num_trials};
# set refine_trough_method = "";

set init_shuffle_method = "throw";
set threshold_energy = "0.001";
set refine_shuffle_method = "gradualpeak0";

set class_weighting_scheme = "fair";

set kernel_type = "gaussian";
set relative_kernel_size = 0.5;

@ moving_window_size = ${num_samples_one_class};

set num_runs = 10;
set sft_size = 512;

foreach num_samples (323 1295 5183 4297 6103 2600 7800 1300 9100 325 10075 81 10319 10400 5200 11700 20800)

set sft_scale = ${sft_size};

if(${num_samples} == 323) then
set grid_size = 18;
else if(${num_samples} == 1295) then
set grid_size = 36;
else if(${num_samples} == 5183) then
set grid_size = 36;
else if(${num_samples} == 4297) then
set grid_size = 72;
else if(${num_samples} == 6103) then
set grid_size = 78;
else if(${num_samples} == 2600) then
set grid_size = 51;
else if(${num_samples} == 7800) then
set grid_size = 88;
else if(${num_samples} == 1300) then
set grid_size = 36;
else if(${num_samples} == 9100) then
set grid_size = 95;
else if(${num_samples} == 325) then
set grid_size = 18;
else if(${num_samples} == 10075) then
set grid_size = 100;
else if(${num_samples} == 81) then
set grid_size = 9;
else if(${num_samples} == 10319) then
set grid_size = 102;
else if(${num_samples} == 10400) then
set grid_size = 102;
else if(${num_samples} == 5200) then
set grid_size = 72;
set sft_scale = 1024;
else if(${num_samples} == 11700) then
set grid_size = 108;
set sft_scale = 1536;
else if(${num_samples} == 20800) then
set grid_size = 144;
set sft_scale = 2048;
else
echo unknown num_samples
exit(1);
endif

@ max_num_iterations = 2 * ${num_samples};

set energy_store_method = grid${grid_size};

set which_run = 0;
while(${which_run} < ${num_runs})

# sample generation
set base_name = shuffle_${dimension}d_${num_class}c_${num_samples};

./src/Main/Release/WhiteNoiseSamples ${dimension} ${num_class} ${num_samples} ${domain_size} > ${huge_dir}/${base_name}_init.txt

if(${refine_trough_method} != "") then
set tail_name = ${refine_trough_method}_${init_shuffle_method}_${class_weighting_scheme};
else
set tail_name = ${init_trough_method}_${init_shuffle_method}_${class_weighting_scheme};
endif

set output_base_name = ${base_name}_${tail_name};

./src/Main/Release/DartShuffling ${huge_dir}/${base_name}_init.txt ${dimension} ${domain_size} ${energy_store_method} ${init_peak_method} ${init_trough_method} ${init_shuffle_method}${threshold_energy} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${num_samples} ${num_samples} > ${huge_dir}/${output_base_name}_${which_run}.txt

if(${refine_trough_method} != "") then
./src/Main/Release/DartShuffling ${huge_dir}/${output_base_name}_${which_run}.txt ${dimension} ${domain_size} ${energy_store_method} ${refine_peak_method} ${refine_trough_method} ${refine_shuffle_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} > ${huge_dir}/${output_base_name}_refine_${which_run}.txt
endif

if(${refine_trough_method} != "") then
mv ${huge_dir}/${output_base_name}_refine_${which_run}.txt ${huge_dir}/${output_base_name}_${which_run}.txt;
endif

# measure min spacing
#if(${num_runs} == 1) then
if(${which_run} == 0) then
set boundary_condition = 1;
set input_names = ${output_base_name}_${which_run};

foreach input_name (${input_names})
set filtered_name = ${input_name}_filtered;

foreach class_option (all)

if(${class_option} == 'all') then
cp ${huge_dir}/${input_name}.txt ${huge_dir}/${filtered_name}.txt 
else
grep "^${class_option}" ${huge_dir}/${input_name}.txt > ${huge_dir}/${filtered_name}.txt 
endif

echo ${class_option} ${input_name}
./src/Main/Release/MinSampleSpacing ${huge_dir}/${filtered_name}.txt ${dimension} ${domain_size} ${boundary_condition}
end # foreach class_option
end # foreach input_name

endif #if(${which_run} == 0)

@ which_run = ${which_run} + 1;
end # while

# power spectrum
if(${sft_size} > 0) then
foreach class (all)

set all_pfm_files = "";
set i = 0;
while(${i} < ${num_runs})

set input_name = ${output_base_name}_${i};
set output_name = ${input_name}_c${class};

if(${class} == "all") then
src/Main/Release/SampleSelector ${huge_dir}/${input_name}.txt ${dimension} ${all_class_ids} > ${huge_dir}/${output_name}_selected.txt
else
src/Main/Release/SampleSelector ${huge_dir}/${input_name}.txt ${dimension} ${class} > ${huge_dir}/${output_name}_selected.txt
endif

./src/Sain/Release/SFT ${huge_dir}/${output_name}_selected.txt ${huge_dir}/${output_name}.pfm ${dimension} 1 1 ${sft_size} ${sft_scale}

set all_pfm_files = "${all_pfm_files} ${huge_dir}/${output_name}.pfm";

@ i = ${i} + 1;
end

echo ${all_pfm_files} > ${huge_dir}/all_pfm_files_list.txt;

./src/Sain/Release/RadialPlot ${huge_dir}/all_pfm_files_list.txt ${output_dir}/${output_base_name}_c${class}_average${num_runs}.pfm ${output_dir}/${output_base_name}_c${class}_average${num_runs}_mean.txt ${output_dir}/${output_base_name}_c${class}_average${num_runs}_variance.txt
rm ${huge_dir}/all_pfm_files_list.txt;

./src/Sain/Release/PFM2PPM ${output_dir}/${output_base_name}_c${class}_average${num_runs}.pfm ${output_dir}/${output_base_name}_c${class}_average${num_runs}.ppm 1 1 1

end # foreach class
endif # (${sft_size} > 0) then

end # foreach num_samples

endif

# timing measurement for soft dart throwing
if(0) then

set dimension = 2;
set domain_size = "1 1";

set num_samples_one_class = 1300;

set init_peak_method = default;
set refine_peak_method = random;

set k_number = 1;
@ init_num_trials = ${k_number} * ${num_samples_one_class};
set init_num_trials = 300;
set init_trough_method = random${init_num_trials};

@ refine_num_trials = ${num_samples_one_class};
set refine_num_trials = ${init_num_trials};
set refine_trough_method = sphererandom${refine_num_trials};
#set refine_trough_method = "";

set init_shuffle_method = "throw";
set threshold_energy = "0.01";
set refine_shuffle_method = "gradualpeak0";

set class_weighting_scheme = "fair";

set kernel_type = "gaussian";
set relative_kernel_size = 0.5;

foreach num_class (1 2 3 4 5 6 7 8)

if(${num_class} == 1) then
set grid_size = 36;
else if(${num_class} == 2) then
set grid_size = 51;
else if(${num_class} == 3) then
set grid_size = 62;
else if(${num_class} == 4) then
set grid_size = 72;
else if(${num_class} == 5) then
set grid_size = 81;
else if(${num_class} == 6) then
set grid_size = 88;
else if(${num_class} == 7) then
set grid_size = 95;
else if(${num_class} == 8) then
set grid_size = 102;
else
echo unknown num_class
exit(1);
endif

set energy_store_method = grid${grid_size};

set i = 0;
set num_samples = "";
while(${i} < ${num_class})
set num_samples = "${num_samples} ${num_samples_one_class}";
@ i = ${i} + 1;
end

@ total_num_samples = ${num_class} * ${num_samples_one_class};

@ moving_window_size = ${num_samples_one_class};
@ max_num_iterations = 2 * ${total_num_samples};

# sample generation
set base_name = shuffle_${dimension}d_${num_class}c_${num_samples_one_class};

./src/Main/Release/WhiteNoiseSamples ${dimension} ${num_class} ${num_samples} ${domain_size} > ${output_dir}/${base_name}_init.txt

if(${refine_trough_method} != "") then
set tail_name = ${refine_trough_method}_${init_shuffle_method}_${class_weighting_scheme};
else
set tail_name = ${init_trough_method}_${init_shuffle_method}_${class_weighting_scheme};
endif

set output_base_name = ${base_name}_${tail_name};

./src/Main/Release/DartShuffling ${output_dir}/${base_name}_init.txt ${dimension} ${domain_size} ${energy_store_method} ${init_peak_method} ${init_trough_method} ${init_shuffle_method}${threshold_energy} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${total_num_samples} ${total_num_samples} > ${output_dir}/${output_base_name}.txt

if(${refine_trough_method} != "") then
./src/Main/Release/DartShuffling ${output_dir}/${output_base_name}.txt ${dimension} ${domain_size} ${energy_store_method} ${refine_peak_method} ${refine_trough_method} ${refine_shuffle_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} > ${output_dir}/${output_base_name}_refine.txt
endif

# measure min spacing
set boundary_condition = 1;
set input_names = ${output_base_name};

if(${refine_trough_method} != "") then
#mv ${output_dir}/${output_base_name}_refine.txt ${output_dir}/${output_base_name}.txt;
set input_names = "${input_names} ${output_base_name}_refine";
endif

foreach input_name (${input_names})
set filtered_name = ${input_name}_filtered;

foreach class_option (0 all)

if(${class_option} == 'all') then
cp ${output_dir}/${input_name}.txt ${output_dir}/${filtered_name}.txt 
else
grep "^${class_option}" ${output_dir}/${input_name}.txt > ${output_dir}/${filtered_name}.txt 
endif

echo ${class_option} ${input_name}
./src/Main/Release/MinSampleSpacing ${output_dir}/${filtered_name}.txt ${dimension} ${domain_size} ${boundary_condition}

end # foreach class_option
end # foreach input_name

end # foreach num_class

endif

# determine optimal kernel size from multiple classes with identical parameters
if(0) then

set dimension = 2;
set domain_size = "1 1";
set num_samples_one_class = 130;

# set peak_method = random;
set refine_peak_method = random;

@ num_trials = ${num_samples_one_class};
set trough_method = random${num_trials};
set refine_trough_method = sphererandom${num_trials};
set refine_trough_method = "";

set shuffle_method = "maxpeak";
set k_number = 2;
set throw_num_trials_per_k_number = 1300;
set shuffle_method = "throw";
set threshold_energy = "0.001";
# set shuffle_method = "gradualpeak0";
set refine_shuffle_method = "gradualpeak0";

set class_weighting_scheme = "fair";

set kernel_type = "gaussian";

# below 3 all in multiple of 0.01
#set relative_kernel_size_min = 40;
#set relative_kernel_size_max = 60;
set relative_kernel_size_opt_dev = 4; # search range deviation from optimal
set relative_kernel_size_inc = 1;

set num_runs = 5;
set sft_size = 0;
set sft_scale = ${sft_size};

foreach num_class (1 2 3 4 5 6 9 12 16)

if(${num_class} == 1) then
set grid_res = 36;
set num_classes_sqrt = 1;
else if(${num_class} == 2) then
set grid_res = 50;
set num_classes_sqrt = 1;
else if(${num_class} == 3) then
set grid_res = 62;
set num_classes_sqrt = 1;
else if(${num_class} == 4) then
set grid_res = 75;
set num_classes_sqrt = 2;
else if(${num_class} == 5) then
set grid_res = 81;
set num_classes_sqrt = 2;
else if(${num_class} == 6) then
set grid_res = 88;
set num_classes_sqrt = 2;
else if(${num_class} == 8) then
set grid_res = 100;
set num_classes_sqrt = 2;
else if(${num_class} == 9) then
set grid_res = 100;
set num_classes_sqrt = 3;
else if(${num_class} == 12) then
set grid_res = 125;
set num_classes_sqrt = 3;
else if(${num_class} == 16) then
set grid_res = 150;
set num_classes_sqrt = 4;
else if(${num_class} == 32) then
set grid_res = 200;
set num_classes_sqrt = 5;
else if(${num_class} == 64) then
set grid_res = 300;
set num_classes_sqrt = 8;
else
echo unsupported num_class;
exit(1);
endif

@ total_num_samples = ${num_class} * ${num_samples_one_class};

#@ throw_num_trials = ${k_number} * ${total_num_samples};
#@ throw_num_trials = ${k_number} * ${num_samples_one_class};
@ throw_num_trials = ${k_number} * ${throw_num_trials_per_k_number};

set throw_trough_method = random${throw_num_trials};

set energy_store_method = grid${grid_res};

@ moving_window_size = ${num_samples_one_class};
@ max_num_iterations = 2 * ${num_class} * ${num_samples_one_class};

set num_samples_per_class = "";
set i = 0;
while(${i} < ${num_class})
set num_samples_per_class = "${num_samples_per_class} ${num_samples_one_class}";
@ i = ${i} + 1;
end

set all_class_ids = "";
set i = 0;
while(${i} < ${num_class})
set all_class_ids = "${all_class_ids} ${i}";
@ i = ${i} + 1;
end

@ sft_scale_all = ${sft_size} * ${num_classes_sqrt};

set base_name = shuffle_${dimension}d_${num_class}c_${num_samples_one_class}_${energy_store_method}_${trough_method}_${class_weighting_scheme};

# optimal relative kernel size
if(${num_class} < 2) then
set relative_kernel_size_opt = 52;
else if(${num_class} <= 4) then
@ relative_kernel_size_opt = 42 + ${num_class};
else
set relative_kernel_size_opt = 46;
endif

@ relative_kernel_size_min = ${relative_kernel_size_opt} - ${relative_kernel_size_opt_dev};
@ relative_kernel_size_max = ${relative_kernel_size_opt} + ${relative_kernel_size_opt_dev};

set relative_kernel_size_now = ${relative_kernel_size_min};

while(${relative_kernel_size_now} <= ${relative_kernel_size_max})

set relative_kernel_size = 0.${relative_kernel_size_now};

echo relative_kernel_size ${relative_kernel_size}

# sample set generation
set i = 0;
while(${i} < ${num_runs})

sleep 1;

./src/Main/Release/WhiteNoiseSamples ${dimension} ${num_class} ${num_samples_per_class} ${domain_size} > ${huge_dir}/${base_name}_init.txt

if(${shuffle_method} == "throw") then
./src/Main/Release/DartShuffling ${huge_dir}/${base_name}_init.txt ${dimension} ${domain_size} ${energy_store_method} ${peak_method} ${throw_trough_method} ${shuffle_method}${threshold_energy} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${total_num_samples} ${total_num_samples} > ${huge_dir}/${base_name}_${i}.txt
else
./src/Main/Release/DartShuffling ${huge_dir}/${base_name}_init.txt ${dimension} ${domain_size} ${energy_store_method} ${peak_method} ${trough_method} ${shuffle_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} > ${huge_dir}/${base_name}_${i}.txt
endif

if(${refine_trough_method} != "") then
./src/Main/Release/DartShuffling ${huge_dir}/${base_name}_${i}.txt ${dimension} ${domain_size} ${energy_store_method} ${refine_peak_method} ${refine_trough_method} ${refine_shuffle_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} > ${huge_dir}/${base_name}_refine_${i}.txt
endif

# measure min spacing
#if(${num_runs} == 1) then
#if(${i} == 0) then
if(1) then
set boundary_condition = 1;
set input_names = ${base_name}_${i};
if(${refine_trough_method} != "") then
set input_names = "${input_names} ${base_name}_refine_${i}";
endif

foreach input_name (${input_names})
set filtered_name = ${input_name}_filtered;

foreach class_option (0 all)

if(${class_option} == 'all') then
cp ${huge_dir}/${input_name}.txt ${huge_dir}/${filtered_name}.txt 
else
grep "^${class_option}" ${huge_dir}/${input_name}.txt > ${huge_dir}/${filtered_name}.txt 
endif

echo ${class_option} ${input_name}
./src/Main/Release/MinSampleSpacing ${huge_dir}/${filtered_name}.txt ${dimension} ${domain_size} ${boundary_condition}
end # foreach class_option
end # foreach input_name
endif

if(${refine_trough_method} != "") then
mv ${huge_dir}/${base_name}_refine_${i}.txt ${huge_dir}/${base_name}_${i}.txt;
endif

@ i = ${i} + 1;
end # while

# power spectrum
if(${sft_size} > 0) then
foreach class (0 all)

set all_pfm_files = "";
set i = 0;
while(${i} < ${num_runs})

set input_name = ${base_name}_${i};
set output_name = ${input_name}_c${class};

if(${class} == "all") then
src/Main/Release/SampleSelector ${huge_dir}/${input_name}.txt ${dimension} ${all_class_ids} > ${huge_dir}/${output_name}_selected.txt
set my_sft_scale = ${sft_scale_all};
else
src/Main/Release/SampleSelector ${huge_dir}/${input_name}.txt ${dimension} ${class} > ${huge_dir}/${output_name}_selected.txt
set my_sft_scale = ${sft_scale};
endif

./src/Sain/Release/SFT ${huge_dir}/${output_name}_selected.txt ${huge_dir}/${output_name}.pfm ${dimension} 1 1 ${sft_size} ${my_sft_scale}

set all_pfm_files = "${all_pfm_files} ${huge_dir}/${output_name}.pfm";

@ i = ${i} + 1;
end

echo ${all_pfm_files} > ${huge_dir}/all_pfm_files_list.txt;

./src/Sain/Release/RadialPlot ${huge_dir}/all_pfm_files_list.txt ${output_dir}/${base_name}_c${class}_average${num_runs}.pfm ${output_dir}/${base_name}_c${class}_average${num_runs}_mean.txt ${output_dir}/${base_name}_c${class}_average${num_runs}_variance.txt
rm ${huge_dir}/all_pfm_files_list.txt;

./src/Sain/Release/PFM2PPM ${output_dir}/${base_name}_c${class}_average${num_runs}.pfm ${output_dir}/${base_name}_c${class}_average${num_runs}.ppm 1 1 1

end # foreach class
endif # (${sft_size} > 0) then

@ relative_kernel_size_now = ${relative_kernel_size_now} + ${relative_kernel_size_inc};
end # while(${relative_kernel_size_now} <= ${relative_kernel_size_max})

end # foreach num_class

endif

# spatial layout for single-class soft disk
# now used for comparison with halftoning
if(0) then

set dimension = 2;
set domain_size = "1 1";
set num_class = 1;

set all_class_ids = 0;

set num_samples_one_class = 400;
set grid_size = 20;
set dot_radius = 0.005;

set energy_store_method = grid${grid_size};

set init_peak_method = default;
set refine_peak_method = random;

set k_number = 2;
@ init_num_trials = ${k_number} * ${num_samples_one_class};
set init_trough_method = random${init_num_trials};

@ refine_num_trials = ${num_samples_one_class};
set refine_trough_method = sphererandom${refine_num_trials};
set refine_trough_method = "";

set init_shuffle_method = "throw";
set threshold_energy = "0.001";
set refine_shuffle_method = "gradualpeak0";

set class_weighting_scheme = "fair";

set kernel_type = "gaussian";
set relative_kernel_size = 0.5;

@ moving_window_size = ${num_samples_one_class};

set num_samples = ${num_samples_one_class};
@ total_num_samples = ${num_class} * ${num_samples_one_class};

@ moving_window_size = ${num_samples_one_class};
@ max_num_iterations = 2 * ${total_num_samples};

# sample generation
set base_name = shuffle_${dimension}d_${num_class}c_${num_samples_one_class};

./src/Main/Release/WhiteNoiseSamples ${dimension} ${num_class} ${num_samples} ${domain_size} > ${output_dir}/${base_name}_init.txt

if(${refine_trough_method} != "") then
set tail_name = ${refine_trough_method}_${init_shuffle_method}_${class_weighting_scheme};
else
set tail_name = ${init_trough_method}_${init_shuffle_method}_${class_weighting_scheme};
endif

set output_base_name = ${base_name}_${tail_name};

./src/Main/Release/DartShuffling ${output_dir}/${base_name}_init.txt ${dimension} ${domain_size} ${energy_store_method} ${init_peak_method} ${init_trough_method} ${init_shuffle_method}${threshold_energy} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${total_num_samples} ${total_num_samples} > ${output_dir}/${output_base_name}.txt

if(${refine_trough_method} != "") then
./src/Main/Release/DartShuffling ${output_dir}/${output_base_name}.txt ${dimension} ${domain_size} ${energy_store_method} ${refine_peak_method} ${refine_trough_method} ${refine_shuffle_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} > ${output_dir}/${output_base_name}_refine.txt
endif

# measure min spacing
set boundary_condition = 1;
set input_names = ${output_base_name};

if(${refine_trough_method} != "") then
#mv ${output_dir}/${output_base_name}_refine.txt ${output_dir}/${output_base_name}.txt;
set input_names = "${input_names} ${output_base_name}_refine";
endif

foreach input_name (${input_names})

echo ${input_name}
./src/Main/Release/MinSampleSpacing ${output_dir}/${input_name}.txt ${dimension} ${domain_size} ${boundary_condition}

end # foreach input_name

# draw
set color_palette_name = "palette";
set color_palette_file_name = ${output_dir}/${color_palette_name}.txt;
echo "0 0 0" > ${color_palette_file_name};
foreach draw_name (${output_base_name})
./src/Main/Release/DrawSamples ${output_dir}/${draw_name}.txt ${output_dir}/${draw_name}.fig 2 ${dot_radius} 1 ${domain_size} ${color_palette_file_name}
./src/Scripts/fig2eps -f ${output_dir}/${draw_name}.fig
epstopdf ${output_dir}/${draw_name}.eps --outfile ${output_dir}/${draw_name}.pdf
end # foreach draw_name

endif

