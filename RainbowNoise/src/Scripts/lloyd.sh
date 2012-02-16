#!/bin/tcsh -f

set output_dir = data;

# first test
if(0) then

set dimension = 2;
set domain_size = "1 1";
set num_class = 3;
set num_samples = "10 20 40";
set num_samples_text = 10n20n40;
set num_points = 9930;

set min_movement = 0.01;
set max_num_iterations = 100;

set class_combo = 3; # for visualizing samples

set voronoi_scheme = lloyd;
set centroid_scheme = none;
set class_weight_scheme = fair;

set output_base_name = lloyd_${dimension}d_${num_class}c_${num_samples_text}_${voronoi_scheme}_${class_weight_scheme};

@ num_class_plus_1 = ${num_class} + 1;
./src/Main/Release/WhiteNoiseSamples ${dimension} ${num_class_plus_1} ${num_samples} ${num_points} ${domain_size} > ${output_dir}/${output_base_name}_init_points.txt

./src/Main/Release/RandomPickPointsSites ${output_dir}/${output_base_name}_init_points.txt ${dimension} ${num_class} ${num_samples} > ${output_dir}/${output_base_name}_points.txt

if(${class_combo} > 0) then
./src/Main/Release/LloydRelaxationPoints ${output_dir}/${output_base_name}_points.txt ${dimension} ${domain_size} ${voronoi_scheme} ${centroid_scheme} ${class_weight_scheme} ${min_movement} ${max_num_iterations} ${class_combo} ${output_dir}/${output_base_name}_points.txt > ${output_dir}/${output_base_name}_samples.txt
else
./src/Main/Release/LloydRelaxation ${output_dir}/${output_base_name}_points.txt ${dimension} ${domain_size} ${voronoi_scheme} ${centroid_scheme} ${class_weight_scheme} ${min_movement} ${max_num_iterations} > ${output_dir}/${output_base_name}_samples.txt
endif

set dot_radius = 0.004;

foreach draw_name (${output_base_name}_samples ${output_base_name}_points)
./src/Main/Release/DrawSamples ${output_dir}/${draw_name}.txt ${output_dir}/${draw_name}.fig 2 ${dot_radius} 1 ${domain_size}
./src/Scripts/fig2eps -f ${output_dir}/${draw_name}.fig
epstopdf ${output_dir}/${draw_name}.eps --outfile ${output_dir}/${draw_name}.pdf
end

endif

# different initial conditions
if(0) then

set dimension = 2;
set domain_size = "1 1";
set num_class = 2;
set num_samples = "100 100";
set num_samples_text = 100n100;

set num_points = 10058; # match the PD setting right below
set r_value = 0.0075;
set k_value = 0.70;

set min_movement = 0.01;
set max_num_iterations = 100;

set class_combo = 1; # for visualizing samples

set voronoi_scheme = lloyd;
set centroid_scheme = none;
set class_weight_scheme = fair;

set base_name = ${dimension}d_${num_class}c_${num_samples_text};

foreach init (poisson white)

set init_base_name = lloyd_${init}_${base_name};

if(${init} == poisson) then
./src/Main/Release/DartThrowing ${dimension} 1 "0" ${r_value} ${k_value} ${domain_size} > ${output_dir}/${init_base_name}_init_points.txt
else
./src/Main/Release/WhiteNoiseSamples ${dimension} 1 ${num_points} ${domain_size} > ${output_dir}/${init_base_name}_init_points.txt
endif

./src/Main/Release/RandomPickPointsSites ${output_dir}/${init_base_name}_init_points.txt ${dimension} ${num_class} ${num_samples} > ${output_dir}/${init_base_name}_points.txt

set output_base_name = ${init_base_name}_${voronoi_scheme}_${class_weight_scheme};

if(${class_combo} > 0) then
./src/Main/Release/LloydRelaxationPoints ${output_dir}/${init_base_name}_points.txt ${dimension} ${domain_size} ${voronoi_scheme} ${centroid_scheme} ${class_weight_scheme} ${min_movement} ${max_num_iterations} ${class_combo} ${output_dir}/${output_base_name}_points.txt > ${output_dir}/${output_base_name}_samples.txt
else
./src/Main/Release/LloydRelaxation ${output_dir}/${init_base_name}_points.txt ${dimension} ${domain_size} ${voronoi_scheme} ${centroid_scheme} ${class_weight_scheme} ${min_movement} ${max_num_iterations} > ${output_dir}/${output_base_name}_samples.txt
endif

set dot_radius = 0.004;

foreach draw_name (${output_base_name}_samples ${output_base_name}_points ${init_base_name}_init_points)
./src/Main/Release/DrawSamples ${output_dir}/${draw_name}.txt ${output_dir}/${draw_name}.fig 2 ${dot_radius} 1 ${domain_size}
./src/Scripts/fig2eps -f ${output_dir}/${draw_name}.fig
epstopdf ${output_dir}/${draw_name}.eps --outfile ${output_dir}/${draw_name}.pdf
end

end # foreach init 

endif

# different Voronoi algorithm
if(0) then

set dimension = 2;
set domain_size = "1 1";
set num_class = 2;
set num_samples = "100 100";
set num_samples_text = 100n100;
set num_points = 10000;

set min_movement = 0.00;
set max_num_iterations = 100;

set class_combo = 11; # for visualizing samples

foreach class_weight_scheme (fair) #(fair rainbow single all)
foreach voronoi_scheme (lloyd balzer)
foreach centroid_scheme (none)

set output_base_name = lloyd_${dimension}d_${num_class}c_${num_samples_text}_${voronoi_scheme}_${class_weight_scheme};

./src/Main/Release/WhiteNoiseSamples ${dimension} 1 ${num_points} ${domain_size} > ${output_dir}/${output_base_name}_init_points.txt

./src/Main/Release/RandomPickPointsSites ${output_dir}/${output_base_name}_init_points.txt ${dimension} ${num_class} ${num_samples} > ${output_dir}/${output_base_name}_points.txt

if(${class_combo} > 0) then
./src/Main/Release/LloydRelaxationPoints ${output_dir}/${output_base_name}_points.txt ${dimension} ${domain_size} ${voronoi_scheme} ${centroid_scheme} ${class_weight_scheme} ${min_movement} ${max_num_iterations} ${class_combo} ${output_dir}/${output_base_name}_points.txt > ${output_dir}/${output_base_name}_samples.txt
else
./src/Main/Release/LloydRelaxation ${output_dir}/${output_base_name}_points.txt ${dimension} ${domain_size} ${voronoi_scheme} ${centroid_scheme} ${class_weight_scheme} ${min_movement} ${max_num_iterations} > ${output_dir}/${output_base_name}_samples.txt
endif

set dot_radius = 0.004;

foreach draw_name (${output_base_name}_samples ${output_base_name}_points)
./src/Main/Release/DrawSamples ${output_dir}/${draw_name}.txt ${output_dir}/${draw_name}.fig 2 ${dot_radius} 1 ${domain_size}
./src/Scripts/fig2eps -f ${output_dir}/${draw_name}.fig
epstopdf ${output_dir}/${draw_name}.eps --outfile ${output_dir}/${draw_name}.pdf
end

end # foreach centroid_scheme
end # foreach voronoi_scheme
end # foreach class_weight_scheme

endif

# sequential relaxation
if(0) then

set dimension = 2;
set domain_size = "1 1";
set num_class = 2;
set num_samples_per_class = 100;
set num_points_per_class = 10000;

set min_movement = 0.00;
set max_num_iterations = 100;

set dot_radius = 0.004;

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
foreach voronoi_scheme (lloyd balzer)
foreach centroid_scheme (none)

set output_base_name = lloyd_lock${lock}_${dimension}d_${num_class}c_${num_samples_text}_${voronoi_scheme}_${class_weight_scheme};

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

if(${lock}) then
./src/Main/Release/ProcessLloydSites ${output_dir}/${output_base_name}_samples.txt ${output_dir}/${output_base_name}_samples.txt ${dimension} "loosen";
endif

if(${draw_intermediate}) then
else
foreach draw_name (${output_base_name}_samples)
./src/Main/Release/DrawSamples ${output_dir}/${draw_name}.txt ${output_dir}/${draw_name}.fig 2 ${dot_radius} 1 ${domain_size}
./src/Scripts/fig2eps -f ${output_dir}/${draw_name}.fig
epstopdf ${output_dir}/${draw_name}.eps --outfile ${output_dir}/${draw_name}.pdf
end
endif

end # foreach centroid_scheme
end # foreach voronoi_scheme
end # foreach class_weight_scheme
end # foreach lock
endif
