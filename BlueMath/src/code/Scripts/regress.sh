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

echo "uniform diff diff transform";

set dimension = 2;

set num_samples = 1024; # white noise
set sample_rate_1d = 10; # hex
set sample_rate = "32 32"; # regular grid

set domain_size = "3 3";

set sub_domain_min = "1 1";
set sub_domain_max = "2 2";

set diff_domain_max = "1 1";

set kernel_option = "gaussian";
set kernel_size = 1;
set keep_dc = 0;
set output_resolution = 64;

set num_runs = 1;

foreach method ("white" "grid" "hex")

set base_name = ${method}_diffxform;

# generate samples
set all_sample_files = "";
set i = 0;
while(${i} < ${num_runs})

if(${method} == "white") then
${exe_dir}/UniformWhiteNoise ${dimension} ${num_samples} ${domain_size} > ${huge_dir}/${base_name}_${i}.txt
else if(${method} == "grid") then
${exe_dir}/RegularGrid ${dimension} ${domain_size} ${sample_rate} 0 > ${huge_dir}/${base_name}_${i}.txt
else if(${method} == "hex") then
${exe_dir}/HexagonalGrid ${dimension} ${domain_size} ${sample_rate_1d} > ${huge_dir}/${base_name}_${i}.txt
else
echo unknown sampling method;
exit(1);
endif

sleep 1;

set all_sample_files = "${all_sample_files} ${huge_dir}/${base_name}_${i}.txt";

@ i = ${i} + 1;
end

# diff transform
echo ${all_sample_files} > ${output_dir}/all_sample_files_list.txt;

${exe_dir}/UniformDiffXform ${output_dir}/all_sample_files_list.txt ${dimension} ${sub_domain_min} ${sub_domain_max} ${diff_domain_max} ${kernel_option} ${kernel_size} ${keep_dc} ${output_resolution} ${output_dir}/${base_name}_average${num_runs}.f2m;

rm ${output_dir}/all_sample_files_list.txt;

${exe_dir}/F2M2PFM ${output_dir}/${base_name}_average${num_runs}.f2m ${output_dir}/${base_name}_average${num_runs}.pfm;

${exe_dir}/PFM2PPM ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}.ppm 1 1 0

convert ${output_dir}/${base_name}_average${num_runs}.ppm ${output_dir}/${base_name}_average${num_runs}.png

echo ${output_dir}/${base_name}_average${num_runs}.png

# radial mean (todo)

end # foreach method

endif

if(1) then
echo "2D blue noise";

set option = uniform;

set dimension = 2;

set domain_size = "1 1";

set dot_radius = 0.01;

set r_value = 0.04;
set r_text = 0p04;
set k_value = 0.70;
#set conflict_metric = 0; # mean
set boundary = 1;
set sample_grid_resolution_per_unit_domain = 20;

set base_name = ${option}_${dimension}d_${r_text};

${exe_dir}/DartThrowing ${option} ${dimension} ${r_value} ${k_value} ${domain_size} ${boundary} ${sample_grid_resolution_per_unit_domain} > ${output_dir}/${base_name}_samples.txt

# coverage
set normalization = 1;
set very_small = 0;
foreach vdpower (0 2 4 inf)
set coverage = `${exe_dir}/Coverage ${output_dir}/${base_name}_samples.txt ${dimension} ${vdpower} ${boundary} ${normalization} ${domain_size} ${very_small}`;

echo "${vdpower} : ${coverage} ";

end #foreach vdpower

# draw samples
foreach draw_option ("stipple" "voronoi_b0" "voronoi_b1")

if(${draw_option} == "stipple") then

set radius_sign = "";
set domain_sign = "";

else

set radius_sign = "-";
if(${draw_option} == "voronoi_b0") then
set domain_sign = "-";
else
set domain_sign = "";
endif

endif

set draw_name = ${base_name}_${draw_option};

${exe_dir}/DrawSamples ${output_dir}/${base_name}_samples.txt ${output_dir}/${draw_name}_samples.fig ${dimension} ${radius_sign}${dot_radius} 1 ${domain_sign}${domain_size}

${exe_dir}/fig2eps -f ${output_dir}/${draw_name}_samples.fig

epstopdf ${output_dir}/${draw_name}_samples.eps --outfile ${output_dir}/${draw_name}_samples.pdf

end # foreach draw_option

endif

if(1) then
echo "blue noise slicing";

set option = uniform;

set original_dimension = 3;
set project_dimension = 2;

set original_domain_size = "1 1 1";
set project_domain_size = "1 1";

set dot_radius = 0.01;

set r_value = 0.1;
set r_text = 0p1;
set k_value = 0.70;
#set conflict_metric = 0; # mean
set boundary = 1;
set sample_grid_resolution_per_unit_domain = 20;

set projection_dimensions = "0 1";

set original_base_name = ${option}_${original_dimension}d_${r_text};
set project_base_name = ${option}_${original_dimension}d_${project_dimension}d_${r_text};

# generation
${exe_dir}/DartThrowing ${option} ${original_dimension} ${r_value} ${k_value} ${original_domain_size} ${boundary} ${sample_grid_resolution_per_unit_domain} > ${output_dir}/${original_base_name}_samples.txt

# projection
${exe_dir}/ProjectSamples ${output_dir}/${original_base_name}_samples.txt ${original_dimension} ${projection_dimensions} > ${output_dir}/${project_base_name}_samples.txt

# draw samples
${exe_dir}/DrawSamples ${output_dir}/${project_base_name}_samples.txt ${output_dir}/${project_base_name}_samples.fig ${project_dimension} ${dot_radius} 1 ${project_domain_size}

${exe_dir}/fig2eps -f ${output_dir}/${project_base_name}_samples.fig

epstopdf ${output_dir}/${project_base_name}_samples.eps --outfile ${output_dir}/${project_base_name}_samples.pdf

endif

if(1) then

echo "Lloyd 2D";

set dimension = 2;
set domain_size = "1 1";
set num_samples = 300;
set dot_radius = 0.01;

set num_iterations = 20;
set boundary = 1;

set base_name = uniform_${num_samples};

set init_name = ${base_name}_white;
set output_name = ${base_name}_lloyd;

${exe_dir}/UniformWhiteNoise ${dimension} ${num_samples} ${domain_size} > ${output_dir}/${init_name}_samples.txt;

${exe_dir}/LloydRelaxation2D ${output_dir}/${init_name}_samples.txt ${dimension} ${boundary} ${domain_size} ${num_iterations} > ${output_dir}/${output_name}_samples.txt;


# draw samples
foreach draw_name (${init_name} ${output_name})

${exe_dir}/DrawSamples ${output_dir}/${draw_name}_samples.txt ${output_dir}/${draw_name}_samples.fig ${project_dimension} ${dot_radius} 1 ${project_domain_size}

${exe_dir}/fig2eps -f ${output_dir}/${draw_name}_samples.fig

epstopdf ${output_dir}/${draw_name}_samples.eps --outfile ${output_dir}/${draw_name}_samples.pdf

end #foreach draw_name
endif

if(1) then

echo "proximity 2D";

set dimension = 2;
set domain_size = "1 1";
set num_samples = 300;
set dot_radius = 0.01;

set boundary = 1;

set base_name = uniform_${num_samples};

set init_name = ${base_name}_white;
set output_name = ${base_name}_nearest;

${exe_dir}/UniformWhiteNoise ${dimension} ${num_samples} ${domain_size} > ${output_dir}/${init_name}_samples.txt;

${exe_dir}/Nearest2D ${output_dir}/${init_name}_samples.txt ${output_dir}/${output_name}_samples.fig ${dimension} ${domain_size} ${boundary} ${dot_radius};

# draw samples
${exe_dir}/fig2eps -f ${output_dir}/${output_name}_samples.fig

epstopdf ${output_dir}/${output_name}_samples.eps --outfile ${output_dir}/${output_name}_samples.pdf

endif
