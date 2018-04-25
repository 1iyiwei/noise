#!/bin/tcsh -f

#echo $#argv

if ($#argv < 3) then
 echo "need input sample file, output dir, output file base name";
 exit(1);
endif

set input_sample_file = $argv[1]

set output_dir = $argv[2]

set output_base_name = $argv[3]

mkdir -p ${output_dir}

set bin_dir = ./bin;
set exe_dir = ./bin/Release;

set dimension = 2;

set sampling_method = "uniform";
set r_value = 0.01175;
set k_value = 0.70;
set domain_size = "1 1";
set boundary = "toroidal";
set sample_grid_resolution_per_unit_domain = 0.01

if(${input_sample_file} == "none") then
 set input_sample_file = "blue_4098.txt";
 ${exe_dir}/DartThrowing  ${sampling_method} ${dimension} ${r_value} ${k_value} ${domain_size} ${boundary} ${sample_grid_resolution_per_unit_domain} ${input_sample_file}
endif

# draw samples
if(0) then
set dot_radius = 0.005;
echo "0 0 0" > ${output_dir}/palette.txt

${exe_dir}/DrawSamples ${input_sample_file} ${output_dir}/${output_base_name}_samples.fig ${dimension} ${dot_radius} 1 ${domain_size} ${output_dir}/palette.txt
${bin_dir}/fig2eps -f ${output_dir}/${output_base_name}_samples.fig
epstopdf ${output_dir}/${output_base_name}_samples.eps --outfile ${output_dir}/${output_base_name}_samples.pdf
rm ${output_dir}/palette.txt
endif

set sim_infinite_domain = 0; # via 3x3 tiling

if(${sim_infinite_domain}) then
set sub_domain_min = "1 1";
set sub_domain_max = "2 2";
set num_tiles = "3 3";
set diff_max = 1;
else
set sub_domain_min = "0 0";
set sub_domain_max = "1 1";
set num_tiles = "1 1";
set diff_max = 0.2;
endif

set diff_domain_max = "${diff_max} ${diff_max}";

set kernel_option = "gaussian";
set kernel_size = 1;
set keep_dc = 0;
set output_resolution = 640;

set tiled_sample_file = "tiled.txt";

${exe_dir}/TileSamples ${input_sample_file} ${dimension} 1 ${num_tiles} ${domain_size} > ${output_dir}/${tiled_sample_file}

echo ${output_dir}/${tiled_sample_file} > ${output_dir}/all_sample_files_list.txt;

${exe_dir}/UniformDiffXform ${output_dir}/all_sample_files_list.txt ${dimension} ${sub_domain_min} ${sub_domain_max} ${diff_domain_max} ${kernel_option} ${kernel_size} ${keep_dc} ${output_resolution} ${output_dir}/${output_base_name}.f2m;

rm -f ${output_dir}/${tiled_sample_file};
rm -f ${output_dir}/all_sample_files_list.txt;

${exe_dir}/F2M2PFM ${output_dir}/${output_base_name}.f2m ${output_dir}/${output_base_name}.pfm;

${exe_dir}/PFM2PPM ${output_dir}/${output_base_name}.pfm ${output_dir}/${output_base_name}.ppm 1 1 0

magick ${output_dir}/${output_base_name}.ppm ${output_dir}/${output_base_name}.png
