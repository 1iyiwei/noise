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

set exe_dir = ./bin/Release;

set dimension = 2;

set sub_domain_min = "0 0";
set sub_domain_max = "1 1";

set diff_domain_max = "1 1";

set kernel_option = "gaussian";
set kernel_size = 1;
set keep_dc = 0;
set output_resolution = 64;

echo ${input_sample_file} > ${output_dir}/all_sample_files_list.txt;

${exe_dir}/UniformDiffXform ${output_dir}/all_sample_files_list.txt ${dimension} ${sub_domain_min} ${sub_domain_max} ${diff_domain_max} ${kernel_option} ${kernel_size} ${keep_dc} ${output_resolution} ${output_dir}/${output_base_name}.f2m;

rm ${output_dir}/all_sample_files_list.txt;

${exe_dir}/F2M2PFM ${output_dir}/${output_base_name}.f2m ${output_dir}/${output_base_name}.pfm;

${exe_dir}/PFM2PPM ${output_dir}/${output_base_name}.pfm ${output_dir}/${output_base_name}.ppm 1 1 0
