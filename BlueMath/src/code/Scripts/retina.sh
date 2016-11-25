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

set gnuplot = "gnuplot";
set convert = "convert";

# sample and spectrum

set base_name = "retina";

set dimension = 2;

set num_runs = 10;

set domain_size = "1 1";

set domain_option = "1blob";

set method = "poisson"
set measure = "isotropic"
set max_importance_arg = "";

set dot_radius = 0.005;
set r_value = 0.04;
set r_text = 0p04;
set k_value = 0.70;
#set conflict_metric = 0; # mean
set boundary = 1; # toroidal
set sample_grid_resolution_per_unit_domain = 20;
set max_importance = 1;

set sft_freq = 128;
@ sft_size = ${sft_freq} * 2;
set sft_scale = ${sft_size};

set normalization_option = "sqrtsamplecount";

echo "1 2 1 0.5 0.5 0.3 0.3" > ${output_dir}/${domain_option}.txt

set domain_res_1d = 32;
set importance_cell_size = 0.03125; # 1.0/32

set domain_res_1d = 8;
set importance_cell_size = 0.125; # 1.0/8

set domain_res = ${domain_res_1d}x${domain_res_1d};

${exe_dir}/MakeBlobs ${output_dir}/${domain_option}.txt ${output_dir}/${domain_option}.pfm ${domain_res_1d} 1 ${domain_size};

${exe_dir}/PFM2F2M ${output_dir}/${domain_option}.pfm ${output_dir}/${domain_option}.f2m

# generate samples
set all_sample_files = "";
# spectrum
set all_pfm_files = "";

set i = 0;
while(${i} < ${num_runs})

${exe_dir}/DartThrowing ${measure} ${dimension} ${r_value} ${k_value} ${domain_size} ${boundary} ${sample_grid_resolution_per_unit_domain} ${output_dir}/${domain_option}.f2m ${max_importance_arg} ${importance_cell_size} > ${huge_dir}/${base_name}_${i}.txt

${exe_dir}/SampleSelector ${huge_dir}/${base_name}_${i}.txt ${dimension} "cout" 0 > ${huge_dir}/${base_name}_${i}_selected.txt
${exe_dir}/SamplePFT ${huge_dir}/${base_name}_${i}_selected.txt ${huge_dir}/${base_name}_${i}.pfm ${dimension} 1 1 ${sft_size} ${sft_scale} ${normalization_option}

set all_pfm_files = "${all_pfm_files} ${huge_dir}/${base_name}_${i}.pfm";

if(${i} == 0) then
# draw samples
echo "0 0 0" > ${output_dir}/palette.txt

${exe_dir}/DrawSamples ${huge_dir}/${base_name}_${i}.txt ${output_dir}/${base_name}_samples.fig ${dimension} ${dot_radius} 1 ${domain_size} ${output_dir}/palette.txt
${exe_dir}/fig2eps -f ${output_dir}/${base_name}_samples.fig
epstopdf ${output_dir}/${base_name}_samples.eps --outfile ${output_dir}/${base_name}_samples.pdf
rm ${output_dir}/palette.txt
endif

sleep 1;
@ i = ${i} + 1;
end

# radial computation

echo ${all_pfm_files} > ${output_dir}/all_pfm_files_list.txt;
${exe_dir}/RadialPlot ${output_dir}/all_pfm_files_list.txt ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt
rm ${output_dir}/all_pfm_files_list.txt;

${exe_dir}/PFM2PPM ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}.ppm 1 1 0
${convert} ${output_dir}/${base_name}_average${num_runs}.ppm ${output_dir}/${base_name}_average${num_runs}.png
echo ${output_dir}/${base_name}_average${num_runs}.png
