#!/bin/tcsh -f
# the equivalence of paper.sh for lloyd

set output_dir = data;

set dot_radius = 0.008;
set no_compute = 0;

set num_samples_per_class = 100;
set num_points_per_sample = 1024;
@ num_points_per_class = ${num_samples_per_class} * ${num_points_per_sample};

set min_movement = 0.000001; # roughly smaller than 1/(num_points * num_samples)
set max_num_iterations = 100;

# concurrent relaxation
if(0) then

set dimension = 2;
set domain_size = "1 1";
set num_class = 2;
set num_samples = "${num_samples_per_class} ${num_samples_per_class}";
set num_samples_text = ${num_samples_per_class}n${num_samples_per_class};

@ num_points = ${num_class} * ${num_points_per_class};

foreach init (white throw)
foreach class_weight_scheme (fair) #(fair rainbow single all)
foreach voronoi_scheme (lloyd) # balzer)
foreach centroid_scheme (none) # snap)

set output_base_name = lloyd_${dimension}d_${num_class}c_${num_samples_text}_${voronoi_scheme}_${centroid_scheme}_${init}_${class_weight_scheme};

if(${no_compute}) then
else

if(${init} == "white") then

./src/Main/Release/WhiteNoiseSamples ${dimension} 1 ${num_points} ${domain_size} > ${output_dir}/${output_base_name}_init_points.txt

./src/Main/Release/RandomPickPointsSites ${output_dir}/${output_base_name}_init_points.txt ${dimension} ${num_class} ${num_samples} > ${output_dir}/${output_base_name}_points.txt

else if(${init} == "throw") then

set energy_store_method = grid10;
set peak_method = "maxpeak";
set k_number = 8;
@ total_num_samples = ${num_class} * ${num_samples_per_class};
@ num_trials = ${k_number} * ${total_num_samples};
set trough_method = random${num_trials};
set shuffle_method = throw;
set energy_threshold = 0.01;
set class_weighting_scheme = fair;
set kernel_type = gaussian;
set relative_kernel_size = 0.5;

./src/Main/Release/WhiteNoiseSamples ${dimension} ${num_class} ${num_samples} ${domain_size} > ${output_dir}/${output_base_name}_init_samples.txt

./src/Main/Release/DartShuffling ${output_dir}/${output_base_name}_init_samples.txt ${dimension} ${domain_size} ${energy_store_method} ${peak_method} ${trough_method} ${shuffle_method}${energy_threshold} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${total_num_samples} ${total_num_samples} > ${output_dir}/${output_base_name}_throw_samples.txt

@ num_remaining_points = ${num_points} - ${total_num_samples};

./src/Main/Release/WhiteNoiseSamples ${dimension} 1 ${num_remaining_points} ${domain_size} > ${output_dir}/${output_base_name}_init_points.txt

./src/Main/Release/ProcessLloydSites ${output_dir}/${output_base_name}_init_points.txt ${output_dir}/${output_base_name}_init_points.txt ${dimension} "decrement" 0;

cat ${output_dir}/${output_base_name}_throw_samples.txt ${output_dir}/${output_base_name}_init_points.txt > ${output_dir}/${output_base_name}_points.txt

else
echo unknown initial condition
exit(1);
endif

./src/Main/Release/LloydRelaxation ${output_dir}/${output_base_name}_points.txt ${dimension} ${domain_size} ${voronoi_scheme} ${centroid_scheme} ${class_weight_scheme} ${min_movement} ${max_num_iterations} > ${output_dir}/${output_base_name}_samples.txt

endif

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
set input_names = ${output_base_name}_samples;
if(${init} == "throw") then
set input_names = "${input_names} ${output_base_name}_throw_samples";
endif

foreach input_name (${input_names})
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
end # foreach input_name 

end # foreach voronoi_scheme
end # foreach class_weight_scheme
end # foreach init
endif

# sequential relaxation
if(0) then

set dimension = 2;
set domain_size = "1 1";
set num_class = 2;

set draw_intermediate = 0;

@ num_points = ${num_points_per_class} * ${num_class};
set class_count = 1;
set num_samples_text = "${num_samples_per_class}";
while(${class_count} < ${num_class})
set num_samples_text = "${num_samples_text}n${num_samples_per_class}";
@ class_count = ${class_count} + 1;
end # while

foreach lock (0 1) # lock existing samples
foreach class_weight_scheme (fair)# single all) #(fair rainbow single all)
foreach voronoi_scheme (lloyd)# balzer)
foreach centroid_scheme (none) # snap)

set output_base_name = lloyd_lock${lock}_${dimension}d_${num_class}c_${num_samples_text}_${voronoi_scheme}_${centroid_scheme}_${class_weight_scheme};

if(${no_compute}) then
else
set class_count = 0;
set num_samples = "";
set existing_class_ids = "";
set remaining_num_points = ${num_points};
echo "" > ${output_dir}/${output_base_name}_samples.txt;

while(${class_count} < ${num_class})

./src/Main/Release/WhiteNoiseSamples ${dimension} 1 ${num_points_per_class} ${domain_size} > ${output_dir}/${output_base_name}_init_points.txt

./src/Main/Release/RandomPickPointsSites ${output_dir}/${output_base_name}_init_points.txt ${dimension} 1 ${num_samples_per_class} > ${output_dir}/${output_base_name}_new_points.txt

set i = 0;
while(${i} < ${class_count})
./src/Main/Release/ProcessLloydSites ${output_dir}/${output_base_name}_new_points.txt ${output_dir}/${output_base_name}_new_points.txt ${dimension} "increment" ${i};
@ i = ${i} + 1;
end

cat ${output_dir}/${output_base_name}_samples.txt ${output_dir}/${output_base_name}_new_points.txt > ${output_dir}/${output_base_name}_points.txt

#set num_samples = "${num_samples} ${num_samples_per_class}";

./src/Main/Release/LloydRelaxation ${output_dir}/${output_base_name}_points.txt ${dimension} ${domain_size} ${voronoi_scheme} ${centroid_scheme} ${class_weight_scheme} ${min_movement} ${max_num_iterations} > ${output_dir}/${output_base_name}_samples.txt

set existing_class_ids = "${existing_class_ids} ${class_count}";

if(${lock}) then
./src/Main/Release/ProcessLloydSites ${output_dir}/${output_base_name}_samples.txt ${output_dir}/${output_base_name}_samples.txt ${dimension} "fasten" ${existing_class_ids};
endif

if(${draw_intermediate}) then
./src/Main/Release/ProcessLloydSites ${output_dir}/${output_base_name}_samples.txt ${output_dir}/${output_base_name}_${class_count}_samples.txt ${dimension} "loosen";
set draw_name = ${output_base_name}_${class_count}_samples;
./src/Main/Release/DrawSamples ${output_dir}/${draw_name}.txt ${output_dir}/${draw_name}.fig 2 ${dot_radius} 1 ${domain_size}
./src/Scripts/fig2eps -f ${output_dir}/${draw_name}.fig
epstopdf ${output_dir}/${draw_name}.eps --outfile ${output_dir}/${draw_name}.pdf
endif

@ remaining_num_points = ${remaining_num_points} - ${num_samples_per_class};
@ class_count = ${class_count} + 1;
end # while
endif # if(${no_compute})

if(${lock}) then
./src/Main/Release/ProcessLloydSites ${output_dir}/${output_base_name}_samples.txt ${output_dir}/${output_base_name}_samples.txt ${dimension} "loosen";
endif

if(${draw_intermediate}) then
else
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
endif

end # foreach centroid_scheme
end # foreach voronoi_scheme
end # foreach class_weight_scheme
end # foreach lock
endif

# multile classes with already relaxed initial condition per class
if(0) then
endif

# multiple classes start with a good joint distribution
if(0) then

set dimension = 2;
set domain_size = "1 1";
set num_class = 2;

@ total_num_samples = ${num_samples_per_class} * ${num_class};
@ num_points = ${num_points_per_class} * ${num_class};

set class_count = 1;
set num_samples = "${num_samples_per_class}";
set num_samples_text = "${num_samples_per_class}";
while(${class_count} < ${num_class})
set num_samples = "${num_samples} ${num_samples_per_class}";
set num_samples_text = "${num_samples_text}n${num_samples_per_class}";
@ class_count = ${class_count} + 1;
end # while

foreach class_weight_scheme (fair) #(fair rainbow single all)
foreach voronoi_scheme (lloyd)# balzer)
foreach centroid_scheme (none) # snap)

set output_base_name = lloyd_joint_${dimension}d_${num_class}c_${num_samples_text}_${voronoi_scheme}_${centroid_scheme}_${class_weight_scheme};

if(${no_compute}) then
else

# generate the initial joint distribution
./src/Main/Release/WhiteNoiseSamples ${dimension} 1 ${num_points} ${domain_size} > ${output_dir}/${output_base_name}_init_points.txt

./src/Main/Release/RandomPickPointsSites ${output_dir}/${output_base_name}_init_points.txt ${dimension} 1 ${total_num_samples} > ${output_dir}/${output_base_name}_points.txt

./src/Main/Release/LloydRelaxation ${output_dir}/${output_base_name}_points.txt ${dimension} ${domain_size} ${voronoi_scheme} ${centroid_scheme} ${class_weight_scheme} ${min_movement} ${max_num_iterations} > ${output_dir}/${output_base_name}_samples.txt

# generate the final distribution 
@ num_points_left = ${num_points} - ${total_num_samples};

./src/Main/Release/WhiteNoiseSamples ${dimension} 1 ${num_points_left} ${domain_size} > ${output_dir}/${output_base_name}_init_points.txt

./src/Main/Release/ProcessLloydSites ${output_dir}/${output_base_name}_init_points.txt ${output_dir}/${output_base_name}_init_points.txt ${dimension} "decrement" 0

./src/Main/Release/RandomPickPointsSites ${output_dir}/${output_base_name}_samples.txt ${dimension} ${num_class} ${num_samples} > ${output_dir}/${output_base_name}_init_samples.txt

cat ${output_dir}/${output_base_name}_init_samples.txt ${output_dir}/${output_base_name}_init_points.txt > ${output_dir}/${output_base_name}_points.txt;

./src/Main/Release/LloydRelaxation ${output_dir}/${output_base_name}_points.txt ${dimension} ${domain_size} ${voronoi_scheme} ${centroid_scheme} ${class_weight_scheme} ${min_movement} ${max_num_iterations} > ${output_dir}/${output_base_name}_samples.txt
endif

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
foreach draw_name (${output_base_name}_init_samples ${output_base_name}_samples ${output_base_name}_samples_all)

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

end # foreach centroid_scheme
end # foreach voronoi_scheme
end # foreach class_weight_scheme

endif

# one class relaxation matching the total count above
if(0) then

set dimension = 2;
set domain_size = "1 1";
set num_class = 2;
set one_class = 1;

@ num_samples = ${num_samples_per_class} * ${num_class};
set num_samples_text = ${num_samples};

@ num_points = ${num_points_per_class} * ${num_class};

foreach class_weight_scheme (fair) #(fair rainbow single all)
foreach voronoi_scheme (lloyd)# balzer)
foreach centroid_scheme (none)

set output_base_name = lloyd_${dimension}d_${one_class}c_${num_samples_text}_${voronoi_scheme}_${centroid_scheme}_${class_weight_scheme};

if(${no_compute}) then
else
./src/Main/Release/WhiteNoiseSamples ${dimension} 1 ${num_points} ${domain_size} > ${output_dir}/${output_base_name}_init_points.txt

./src/Main/Release/RandomPickPointsSites ${output_dir}/${output_base_name}_init_points.txt ${dimension} ${one_class} ${num_samples} > ${output_dir}/${output_base_name}_points.txt

./src/Main/Release/LloydRelaxation ${output_dir}/${output_base_name}_points.txt ${dimension} ${domain_size} ${voronoi_scheme} ${centroid_scheme} ${class_weight_scheme} ${min_movement} ${max_num_iterations} > ${output_dir}/${output_base_name}_samples.txt
endif

foreach draw_name (${output_base_name}_samples)
set color_palette_name = "palette";
echo "0 0 0" > ${output_dir}/${color_palette_name}.txt;
./src/Main/Release/DrawSamples ${output_dir}/${draw_name}.txt ${output_dir}/${draw_name}.fig 2 ${dot_radius} 1 ${domain_size} ${output_dir}/${color_palette_name}.txt
./src/Scripts/fig2eps -f ${output_dir}/${draw_name}.fig
epstopdf ${output_dir}/${draw_name}.eps --outfile ${output_dir}/${draw_name}.pdf
end

end # foreach centroid_scheme
end # foreach voronoi_scheme
end # foreach class_weight_scheme

endif

# one class with 1300 samples to compare with soft dart throwing
if(0) then

endif

# one class relaxation
if(1) then

set dimension = 2;
set domain_size = "1 1";
set num_class = 1;
set one_class = 1;

set num_samples_per_class = 130;
set num_points_per_sample = 256;
@ num_points_per_class = ${num_samples_per_class} * ${num_points_per_sample};

@ num_samples = ${num_samples_per_class} * ${num_class};
set num_samples_text = ${num_samples};

@ num_points = ${num_points_per_class} * ${num_class};

foreach class_weight_scheme (fair) #(fair rainbow single all)
foreach voronoi_scheme (lloyd balzer)
foreach centroid_scheme (none)

set output_base_name = lloyd_${dimension}d_${one_class}c_${num_samples_text}_${voronoi_scheme}_${centroid_scheme}_${class_weight_scheme};

if(${no_compute}) then
else
./src/Main/Release/WhiteNoiseSamples ${dimension} 1 ${num_points} ${domain_size} > ${output_dir}/${output_base_name}_init_points.txt

./src/Main/Release/RandomPickPointsSites ${output_dir}/${output_base_name}_init_points.txt ${dimension} ${one_class} ${num_samples} > ${output_dir}/${output_base_name}_points.txt

./src/Main/Release/LloydRelaxation ${output_dir}/${output_base_name}_points.txt ${dimension} ${domain_size} ${voronoi_scheme} ${centroid_scheme} ${class_weight_scheme} ${min_movement} ${max_num_iterations} > ${output_dir}/${output_base_name}_samples.txt
endif

# measure min spacing
set boundary_condition = 1;
set input_names = ${output_base_name}_samples;
foreach input_name (${input_names})
echo ${input_name}
./src/Main/Release/MinSampleSpacing ${output_dir}/${input_name}.txt ${dimension} ${domain_size} ${boundary_condition}
end # foreach input_name

# draw
foreach draw_name (${output_base_name}_samples)
set color_palette_name = "palette";
echo "0 0 0" > ${output_dir}/${color_palette_name}.txt;
./src/Main/Release/DrawSamples ${output_dir}/${draw_name}.txt ${output_dir}/${draw_name}.fig 2 ${dot_radius} 1 ${domain_size} ${output_dir}/${color_palette_name}.txt
./src/Scripts/fig2eps -f ${output_dir}/${draw_name}.fig
epstopdf ${output_dir}/${draw_name}.eps --outfile ${output_dir}/${draw_name}.pdf
end

end # foreach centroid_scheme
end # foreach voronoi_scheme
end # foreach class_weight_scheme

endif
