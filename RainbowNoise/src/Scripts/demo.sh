#!/bin/tcsh -f

if ($#argv < 1) then
 echo "please specify the output directory"
 exit(1)
endif

set output_dir = $argv[1]

mkdir -p ${output_dir}

set huge_dir = ${output_dir}/huge;
mkdir -p ${huge_dir}

set exe_dir = ./bin;

if(1) then

set dimension = 2;
set domain_size = "1 1";
set num_class = 2;
set num_samples = "100 100";
set num_samples_text = 100n100;
set num_points = 9800;

set min_movement = 0.01;
set max_num_iterations = 100;

set class_combo = 0; # for visualizing samples

set dot_radius = 0.004; # for drawing samples

set base_name = lloyd_${dimension}d_${num_class}c_${num_samples_text};

@ num_class_plus_1 = ${num_class} + 1;
${exe_dir}/WhiteNoiseSamples ${dimension} ${num_class_plus_1} ${num_samples} ${num_points} ${domain_size} > ${output_dir}/${base_name}_init_points.txt

${exe_dir}/RandomPickPointsSites ${output_dir}/${base_name}_init_points.txt ${dimension} ${num_class} ${num_samples} > ${output_dir}/${base_name}_points.txt

foreach voronoi_scheme (lloyd)
foreach centroid_scheme (snap)
foreach class_weight_scheme (fair)

set output_base_name = ${base_name}_${voronoi_scheme}_${class_weight_scheme};

if(${class_combo} > 0) then
${exe_dir}/LloydRelaxationPoints ${output_dir}/${base_name}_points.txt ${dimension} ${domain_size} ${voronoi_scheme} ${centroid_scheme} ${class_weight_scheme} ${min_movement} ${max_num_iterations} ${class_combo} ${output_dir}/${output_base_name}_points.txt > ${output_dir}/${output_base_name}_samples.txt
else
${exe_dir}/LloydRelaxation ${output_dir}/${base_name}_points.txt ${dimension} ${domain_size} ${voronoi_scheme} ${centroid_scheme} ${class_weight_scheme} ${min_movement} ${max_num_iterations} > ${output_dir}/${output_base_name}_samples.txt
endif

foreach draw_name (${output_base_name}_samples) # ${output_base_name}_points)
${exe_dir}/DrawSamples ${output_dir}/${draw_name}.txt ${output_dir}/${draw_name}.fig 2 ${dot_radius} 1 ${domain_size}
${exe_dir}/fig2eps -f ${output_dir}/${draw_name}.fig
epstopdf ${output_dir}/${draw_name}.eps --outfile ${output_dir}/${draw_name}.pdf
end

end # foreach centroid_scheme
end # foreach voronoi_scheme
end # foreach class_weight_scheme

endif
