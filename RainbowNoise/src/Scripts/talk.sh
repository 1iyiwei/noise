#!/bin/tcsh -f

set outputdir = data;
set output_dir = ${outputdir};

# presentation 3 class example with objects
if(0) then

set outputdir = data;

set dimension = 2;

set exe_name = DartThrowing;
set output_name = grid;

set num_classes = 3;
set priority = "0 0 0";

set r_values = "0.04 0.02 0.01";
set r_text = 0p040p020p01;
set dot_radius = 0.002;

if(1) then
set r_values = "0.24 0.12 0.06";
set r_text = 0p240p120p06;
set dot_radius = 0.004;
endif

set k_value = 0.67;
set domain_size = "1 1";

set object_name = "object";
set field_name = "uniform";

set window_size = 1024; #768;
set background_color = "data/free_grass_texture.ppm"; #1;
set batch_mode = 1;
set boundary_condition = 1;

set object0 = flower_red_rose;
#set flower0 = flower_dahlia_pink;
set object1 = flower_buttercup_yellow;
set object2 = flower_cactus_white;
#set object2 = flower_daisy_white;

set object_files = " ${outputdir}/${object0}.ppm ${outputdir}/${object1}.ppm ${outputdir}/${object2}.ppm";
set output_file_ext = "ppm";

set object_scale = 0.05;
set object_sizes = "${object_scale} ${object_scale} ${object_scale}";

# sample generation
echo src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size
src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size > ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${r_text}.txt

# draw samples
foreach id () #(all)

set output_root_name = ${output_name}_${dimension}d_${num_classes}c_${r_text}_${id}

if(${id} == 'all') then
cp ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${r_text}.txt ${outputdir}/${output_root_name}.txt
else
grep "^${id}" ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${r_text}.txt > ${outputdir}/${output_root_name}.txt
endif

src/Main/Release/DrawSamples ${outputdir}/${output_root_name}.txt ${outputdir}/${output_root_name}.fig ${dimension} ${dot_radius} 1 ${domain_size}
src/Scripts/fig2eps -f ${outputdir}/${output_root_name}.fig
epstopdf ${outputdir}/${output_root_name}.eps --outfile ${outputdir}/${output_root_name}.pdf

echo ${outputdir}/${output_root_name}.pdf
end

# draw objects
if(1) then
set output_object_root_name = ${object_name}_${field_name}_${dimension}d_${num_classes}c_${r_text};

src/GPU/Release/DrawObjects ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${r_text}.txt ${outputdir}/${output_object_root_name}_objects.${output_file_ext} ${dimension} ${domain_size} ${num_classes} ${object_files} ${object_sizes} ${window_size} ${background_color} ${batch_mode} ${boundary_condition}

echo ${outputdir}/${output_object_root_name}_objects.${output_file_ext}
endif

endif

# presentation 1 class example
if(0) then

set outputdir = data;

set dimension = 2;

set exe_name = DartThrowing;
#set exe_name = AdaptiveDartThrowing;
set output_name = grid;

set num_classes = 1;
set priority = "0";

set r_values = "0.03";
set r_text = 0p03;
set dot_radius = 0.004;

set k_value = 0.75;
set domain_size = "1 1";

set object_name = "object";
set field_name = "uniform";

set window_size = 1024; #768;
set background_color = "data/free_grass_texture.ppm"; #1;
set batch_mode = 1;
set boundary_condition = 1;

set object0 = flower_red_rose;
#set flower0 = flower_dahlia_pink;
set object0 = flower_buttercup_yellow;
set object2 = flower_cactus_white;
#set object2 = flower_daisy_white;

set object_files = " ${outputdir}/${object0}.ppm";
set output_file_ext = "ppm";

set object_scale = 0.025;
set object_sizes = "${object_scale}";

# sample generation
echo src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size
src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size > ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${r_text}.txt

# draw samples
foreach id () #(all)

set output_root_name = ${output_name}_${dimension}d_${num_classes}c_${r_text}_${id}

if(${id} == 'all') then
cp ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${r_text}.txt ${outputdir}/${output_root_name}.txt
else
grep "^${id}" ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${r_text}.txt > ${outputdir}/${output_root_name}.txt
endif

src/Main/Release/DrawSamples ${outputdir}/${output_root_name}.txt ${outputdir}/${output_root_name}.fig ${dimension} ${dot_radius} 1 ${domain_size}
src/Scripts/fig2eps -f ${outputdir}/${output_root_name}.fig
epstopdf ${outputdir}/${output_root_name}.eps --outfile ${outputdir}/${output_root_name}.pdf

echo ${outputdir}/${output_root_name}.pdf
end

# draw objects
if(1) then
set output_object_root_name = ${object_name}_${field_name}_${dimension}d_${num_classes}c_${r_text};

src/GPU/Release/DrawObjects ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${r_text}.txt ${outputdir}/${output_object_root_name}_objects.${output_file_ext} ${dimension} ${domain_size} ${num_classes} ${object_files} ${object_sizes} ${window_size} ${background_color} ${batch_mode} ${boundary_condition}

echo ${outputdir}/${output_object_root_name}_objects.${output_file_ext}
endif

endif

# presentation 3 class with different density to illustrate r matrix
if(0) then

set outputdir = data;

set dimension = 2;

set exe_name = DartThrowing;
set output_name = grid;

set num_classes = 3;
set priority = "0 0 0";

set r_values = "0.04 0.02 0.01";
set r_text = 0p040p020p01;
set dot_radius = 0.002;

if(1) then
set r_values = "0.24 0.12 0.06";
set r_text = 0p240p120p06;
set dot_radius = 0.01;
endif

if(1) then
set r_values = "0.4 0.2 0.1";
set r_text = 0p40p20p1;
set dot_radius = 0.01;
endif

set k_value = 0.67;
set domain_size = "1 1";

set field_name = "uniform";

# sample generation
echo src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size
src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size > ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${r_text}.txt

# draw samples
foreach id (all)

set output_root_name = ${output_name}_${dimension}d_${num_classes}c_${r_text}_${id}

if(${id} == 'all') then
cp ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${r_text}.txt ${outputdir}/${output_root_name}.txt
else
grep "^${id}" ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${r_text}.txt > ${outputdir}/${output_root_name}.txt
endif

src/Main/Release/DrawSamples ${outputdir}/${output_root_name}.txt ${outputdir}/${output_root_name}.fig ${dimension} ${dot_radius} -1 ${domain_size}
src/Scripts/fig2eps -f ${outputdir}/${output_root_name}.fig
epstopdf ${outputdir}/${output_root_name}.eps --outfile ${outputdir}/${output_root_name}.pdf

echo ${outputdir}/${output_root_name}.pdf
end

endif

# energy function visualization
if(0) then

set peak_method = "default";
set shuffle_method = "maxpeak";

set dimension = 2;
set domain_size = "1 1";

set num_class = 3;
set num_samples_per_class = "8 32 196";
set num_samples_text = 8n32n196;
set total_num_samples = 236;

if(1) then # one class
set num_class = 1;
set num_samples_per_class = "32";
set num_samples_text = 32;
set total_num_samples = 32;
endif

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

set energy_class_id = 0;
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

foreach class_option ('all')

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

foreach output_vis_name (${output_base_name}_samples) #${output_base_name}_init_samples

foreach current_class_id (${energy_class_ids})
./src/Main/Release/DartShufflingEnergy ${output_dir}/${output_vis_name}.txt ${dimension} ${domain_size} ${energy_store_method} ${peak_method} ${init_trough_method} ${shuffle_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} 0 0 ${current_class_id} ${energy_array_size} ${output_dir}/${output_vis_name}_energy_${current_class_id}.pfm ${output_dir}/${output_vis_name}_history.txt > /dev/null;
end

if(${energy_class_id} >= 0) then
mv ${output_dir}/${output_vis_name}_energy_${energy_class_id}.pfm ${output_dir}/${output_vis_name}_energy.pfm;
else
if(${num_class} != 3) then
echo num_class must be 3 for merging 3 pfm;
exit(1);
endif

./src/Sain/Release/Merge3PFM ${output_dir}/${output_vis_name}_energy_0.pfm ${output_dir}/${output_vis_name}_energy_1.pfm ${output_dir}/${output_vis_name}_energy_2.pfm  ${output_dir}/${output_vis_name}_energy.pfm
endif

./src/Sain/Release/PFM2PPM ${output_dir}/${output_vis_name}_energy.pfm ${output_dir}/${output_vis_name}_energy.ppm 1 1 2

echo ${output_dir}/${output_vis_name}_energy.ppm

endif

end # foreach output_vis_name

end # foreach class_weighting_scheme

endif
