#!/bin/tcsh -f

set rainbow_dir = ../RainbowNoise;
set output_dir = data;
set huge_dir = data/huge;

# 2D dart throwing with white noise normalization
if(0) then

set dimension = 2;

set num_class = 1;
set priority = "0";

set r_values = "0.04";
set r_text = 0p04;

set k_value = 0.75;

# set num_samples = 405; # derived from dart throwing result

set domain_size = "1 1";

set num_runs = 10;

set sft_freq = 64;
@ sft_size = ${sft_freq} * 2;
set sft_scale = ${sft_size};

set normalization_option = "sqrtsamplecount";
set norm_pft_option = "subadd";
set invariance_const = 1;

set white_name = 'white';
set poisson_name = 'poisson';
set naive_name = 'poisson_naive';
set norm_name = 'poisson_norm';

set all_white_pfm_files = "";
set all_poisson_pfm_files = "";
set all_naive_pfm_files = "";
set all_norm_pfm_files = "";

set i = 0;
while(${i} < ${num_runs})

foreach output_name (${poisson_name} ${white_name}) # poisson must preceed white to figure out the relevant # of samples

if(${output_name} == ${poisson_name}) then
set exe_name = DartThrowing;
else if(${output_name} == ${white_name}) then
set exe_name = WhiteNoiseSamples;
else
echo unknown output_name;
exit(1);
endif

set base_name = ${output_name}_${dimension}d_${num_class}c_${r_text};

# sample set generation
if(${exe_name} == DartThrowing) then
${rainbow_dir}/src/Main/Release/${exe_name} $dimension $num_class $priority $r_values $k_value $domain_size > ${huge_dir}/${base_name}_${i}.txt
else if(${exe_name} == WhiteNoiseSamples) then
${rainbow_dir}/src/Main/Release/${exe_name} $dimension $num_class $num_poisson_samples $domain_size > ${huge_dir}/${base_name}_${i}.txt
else
echo unknown exe name;
exit(1);
endif

sleep 1;

# compute sft
${rainbow_dir}/src/Main/Release/SampleSelector ${huge_dir}/${base_name}_${i}.txt ${dimension} 0 > ${huge_dir}/${base_name}_${i}_selected.txt
./src/Main/Release/SamplePFT ${huge_dir}/${base_name}_${i}_selected.txt ${huge_dir}/${base_name}_${i}.pfm ${dimension} 1 1 ${sft_size} ${sft_scale} ${normalization_option}

# num samples
if(${output_name} == ${poisson_name}) then
set final_samples = `${rainbow_dir}/src/Main/Release/SampleSelector ${huge_dir}/${base_name}_${i}.txt ${dimension} 0`;
@ num_poisson_samples = $#final_samples / ${dimension};
endif

end #foreach output_name

# compute naive normalized power
./src/Main/Release/NormPFT ${norm_pft_option} ${invariance_const} ${huge_dir}/${poisson_name}_${dimension}d_${num_class}c_${r_text}_${i}.pfm ${huge_dir}/${white_name}_${dimension}d_${num_class}c_${r_text}_${i}.pfm ${huge_dir}/${naive_name}_${dimension}d_${num_class}c_${r_text}_${i}.pfm

# compute normalized power
./src/Main/Release/SyntheticPFT ${num_poisson_samples} "uniform" "infinite" ${output_dir}/${white_name}_${dimension}d_${num_class}c_${r_text}_averageinfty.pfm ${dimension} 1 1 ${sft_size} ${sft_scale} ${normalization_option}

./src/Main/Release/NormPFT ${norm_pft_option} ${invariance_const} ${huge_dir}/${poisson_name}_${dimension}d_${num_class}c_${r_text}_${i}.pfm ${output_dir}/${white_name}_${dimension}d_${num_class}c_${r_text}_averageinfty.pfm ${huge_dir}/${norm_name}_${dimension}d_${num_class}c_${r_text}_${i}.pfm

# book keeping
set all_white_pfm_files = "${all_white_pfm_files} ${huge_dir}/${white_name}_${dimension}d_${num_class}c_${r_text}_${i}.pfm";
set all_poisson_pfm_files = "${all_poisson_pfm_files} ${huge_dir}/${poisson_name}_${dimension}d_${num_class}c_${r_text}_${i}.pfm";
set all_naive_pfm_files = "${all_naive_pfm_files} ${huge_dir}/${naive_name}_${dimension}d_${num_class}c_${r_text}_${i}.pfm";
set all_norm_pfm_files = "${all_norm_pfm_files} ${huge_dir}/${norm_name}_${dimension}d_${num_class}c_${r_text}_${i}.pfm";

@ i = ${i} + 1;
end

# radial computation
foreach output_name (${poisson_name} ${white_name} ${naive_name} ${norm_name})

set base_name = ${output_name}_${dimension}d_${num_class}c_${r_text};

if(${output_name} == ${poisson_name}) then
#set all_pfm_files = ${all_poisson_pfm_files};
echo ${all_poisson_pfm_files} > ${output_dir}/all_pfm_files_list.txt;
else if(${output_name} == ${white_name}) then
#set all_pfm_files = ${all_white_pfm_files};
echo ${all_white_pfm_files} > ${output_dir}/all_pfm_files_list.txt;
else if(${output_name} == ${naive_name}) then
#set all_pfm_files = ${all_naive_pfm_files};
echo ${all_naive_pfm_files} > ${output_dir}/all_pfm_files_list.txt;
else if(${output_name} == ${norm_name}) then
#set all_pfm_files = ${all_norm_pfm_files};
echo ${all_norm_pfm_files} > ${output_dir}/all_pfm_files_list.txt;
else
echo unknown output_name
exit(1);
endif

#echo ${all_pfm_files} > ${output_dir}/all_pfm_files_list.txt;
./src/Main/Release/RadialPlot ${output_dir}/all_pfm_files_list.txt ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt
rm ${output_dir}/all_pfm_files_list.txt;

# radial plot
if(${normalization_option} == 'samplecount') then
set hist_normalization_const = ${num_samples};
else if(${normalization_option} == 'sqrtsamplecount') then
set hist_normalization_const = 1;
else
echo unknown normalization option;
exit(1);
endif

${rainbow_dir}/src/Scripts/txt2dat ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt ${hist_normalization_const} ${output_dir}/${base_name}_average${num_runs}_mean.dat ${output_dir}/${base_name}_average${num_runs}_variance.dat 1

echo set terminal postscript eps >> foo.gp
echo set output \'${output_dir}/${base_name}_average${num_runs}_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
echo plot "[1:${sft_freq}] [0:1.8]" \"${output_dir}/${base_name}_average${num_runs}_mean.dat\" with line linetype rgb \"red\" >> foo.gp

echo set terminal postscript eps >> foo.gp
echo set output \'${output_dir}/${base_name}_average${num_runs}_variance.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"anisotropy\" >> foo.gp
echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
echo plot "[1:${sft_freq}] [-20:20]" \"${output_dir}/${base_name}_average${num_runs}_variance.dat\" with line linetype rgb \"blue\" >> foo.gp

wgnuplot foo.gp
rm foo.gp

foreach option (mean variance)
set final_name = ${base_name}_average${num_runs}_${option};
epstopdf ${output_dir}/${final_name}.eps --outfile ${output_dir}/${final_name}.pdf;
echo ${output_dir}/${final_name}.pdf;
end

./src/Main/Release/PFM2PPM ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}.ppm 1 1 0
echo ${output_dir}/${base_name}_average${num_runs}.ppm

end # foreach output_name

# cleanup
if(1) then
rm -f ${output_dir}/huge/*
endif

endif
