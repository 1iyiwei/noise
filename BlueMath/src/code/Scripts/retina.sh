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

set dimension = 2;

set num_runs = 10;

set domain_size = "1 1";

set method = "poisson"
set measure = "isotropic"
set max_importance_arg = "";

set dot_radius = 0.005;
set r_value = 0.03;
set r_text = 0p03;
set k_value = 0.70;
#set conflict_metric = 0; # mean
set boundary = 1; # toroidal
set sample_grid_resolution_per_unit_domain = 20;
set max_importance = 1;

set sft_freq = 128;
@ sft_size = ${sft_freq} * 2;
set sft_scale = ${sft_size};

set clamp_radius = 2.5;
set normalization_option = "sqrtsamplecount";

foreach domain_option ("uniform" "1blob")

if(${domain_option} == "uniform") then

set domain_res = 1x1;
set importance_cell_size = 1;
echo "F2 2 1 1 1" > ${output_dir}/${domain_option}.f2m;

${exe_dir}/F2M2PFM ${output_dir}/${domain_option}.f2m ${output_dir}/${domain_option}.pfm

else if(${domain_option} == "1blob") then

echo "1 2 1 0.5 0.5 0.2 0.2" > ${output_dir}/${domain_option}.txt

set domain_res_1d = 128;
set importance_cell_size = 0.0078125; # 1.0/128

set domain_res = ${domain_res_1d}x${domain_res_1d};

${exe_dir}/MakeBlobs ${output_dir}/${domain_option}.txt ${output_dir}/${domain_option}.pfm ${domain_res_1d} 1 ${domain_size};

${exe_dir}/PFM2F2M ${output_dir}/${domain_option}.pfm ${output_dir}/${domain_option}.f2m

if(0) then
${exe_dir}/PFM2PPM ${output_dir}/${domain_option}.pfm ${output_dir}/${domain_option}.ppm 1 1 2
${convert} ${output_dir}/${domain_option}.ppm ${output_dir}/${domain_option}.png
endif

else

echo unknown domain option;
exit(1);

endif

set base_name = ${domain_option}_${r_text};

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

set final_samples = `${exe_dir}/SampleSelector ${huge_dir}/${base_name}_${i}.txt ${dimension} cout 0`;
@ num_output_samples = $#final_samples / ${dimension};

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

${exe_dir}/PFM3PPM ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}.ppm 1 1 0 ${clamp_radius}
${convert} ${output_dir}/${base_name}_average${num_runs}.ppm ${output_dir}/${base_name}_average${num_runs}.png
echo ${output_dir}/${base_name}_average${num_runs}.png

if(${normalization_option} == 'samplecount') then
set hist_normalization_const = ${num_output_samples};
else if(${normalization_option} == 'sqrtsamplecount') then
set hist_normalization_const = 1;
else
echo unknown normalization option;
exit(1);
endif

./${exe_dir}/txt2dat ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt ${hist_normalization_const} ${output_dir}/${base_name}_average${num_runs}_mean.dat ${output_dir}/${base_name}_average${num_runs}_variance.dat 1

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

${gnuplot} foo.gp
rm foo.gp

foreach option (mean variance)
set final_name = ${base_name}_average${num_runs}_${option};
epstopdf ${output_dir}/${final_name}.eps --outfile ${output_dir}/${final_name}.pdf;
echo ${output_dir}/${final_name}.pdf;
end

end # domain_option
