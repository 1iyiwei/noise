#!/bin/tcsh -f

set rainbow_dir = ../RainbowNoise;
set blue_dir = ../BlueNoise;
set output_dir = data;
set huge_dir = data/huge;

# 1D dart throwing
if(0) then

set dimension = 1;

set exe_name = DartLining;
set output_name = line;

if(1) then
set exe_name = DartThrowing;
set output_name = grid;
endif

if(0) then
set exe_name = WhiteNoiseSamples;
set output_name = white;
endif

set num_class = 1;
set priority = "0";

set r_values = "0.01";
set r_text = 0p01;

set k_value = 0.99;

if(${exe_name} == WhiteNoiseSamples) then
set num_samples = "63";
endif

set domain_size = "1";

set num_runs = 512;

set sft_freq = 256;
@ sft_size = ${sft_freq} * 2;
set sft_scale = ${sft_size};

set normalization_option = "sqrtsamplecount";

foreach diff_dist ("") #("uniform" "gaussian2" "gaussian1" "gaussianp75" "gaussianp5" "gaussianp25" "gaussianp125")

set base_name = ${output_name}_${dimension}d_${num_class}c_${r_text};
if(${diff_dist} != "") then
set base_name = ${base_name}_${diff_dist};
endif

set all_pfm_files = "";
set i = 0;
while(${i} < ${num_runs})

# sample set generation
if(${exe_name} == DartThrowing) then
${rainbow_dir}/src/Main/Release/${exe_name} $dimension $num_class $priority $r_values $k_value $domain_size > ${huge_dir}/${base_name}_${i}.txt
else if(${exe_name} == DartLining) then
src/Main/Release/${exe_name} $dimension $num_class $r_values $domain_size $diff_dist > ${huge_dir}/${base_name}_${i}.txt
else if(${exe_name} == WhiteNoiseSamples) then
${rainbow_dir}/src/Main/Release/${exe_name} $dimension $num_class $num_samples $domain_size > ${huge_dir}/${base_name}_${i}.txt
else
echo unknown exe name;
exit(1);
endif

sleep 1;

# compute sft
./src/Main/Release/SampleSelector ${huge_dir}/${base_name}_${i}.txt ${dimension} "cout" 0 > ${huge_dir}/${base_name}_${i}_selected.txt
./src/Main/Release/SamplePFT ${huge_dir}/${base_name}_${i}_selected.txt ${huge_dir}/${base_name}_${i}.pfm ${dimension} 1 ${sft_size} ${sft_scale} ${normalization_option}

set all_pfm_files = "${all_pfm_files} ${huge_dir}/${base_name}_${i}.pfm";

# num samples
set final_samples = `./src/Main/Release/SampleSelector ${huge_dir}/${base_name}_${i}.txt ${dimension} cout 0`;
@ num_samples = $#final_samples / ${dimension};

@ i = ${i} + 1;
end

# radial computation
echo ${all_pfm_files} > ${output_dir}/all_pfm_files_list.txt;
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

wgnuplot foo.gp
rm foo.gp

set final_name = ${base_name}_average${num_runs}_mean;
epstopdf ${output_dir}/${final_name}.eps --outfile ${output_dir}/${final_name}.pdf;
echo ${output_dir}/${final_name}.pdf;

end # foreach diff_dist

endif

# 1D diff histogram
if(0) then

set regenerate_samples = 1;

foreach distribution ('poisson' 'white') 

set dimension = 1;

set num_class = 1;
set priority = 0;
set ids = 0;

set r_value = "0.01";
set r_text = 0p01;

set k_value = 0.99;

if(${dimension} == 1) then
set target_num_samples = 63;
else if(${dimension} == 2) then
set target_num_samples = 1680;
endif

set which_dimension = "0";
set which_dimension_text = "0";

foreach which_dimension_domain_size (1 2)
set which_dimension_sub_domain_size = 1;

set max_diff = 1;
set max_zoom_diff = 0.1;
set num_bins = 1000;
set num_zoom_bins = 100; # match diff above

@ target_num_samples = ${target_num_samples} * ${which_dimension_domain_size};

set y_max = 2.5;
set max_frequency = 256;
set num_runs = 1280; #2560;

set all_sample_files = "";
set all_sample_files_count = 0;

set domain_size = "";
set sub_domain_size = "";
set i = 0;
while(${i} < ${dimension})
set domain_size = "$domain_size $which_dimension_domain_size";
set sub_domain_size = "$sub_domain_size $which_dimension_sub_domain_size";
@ i = ${i} + 1;
end

# generate samples
set i = 0;
while(${i} < ${num_runs})

set sample_file_name = ${huge_dir}/${distribution}_${dimension}d_${num_class}c_${r_text}_${i}.txt;

if(${regenerate_samples}) then
if(${distribution} == 'poisson') then

if($target_num_samples > 0) then
#set k_value = -$target_num_samples;
endif

${rainbow_dir}/src/Main/Release/DartThrowing $dimension $num_class $priority $r_value $k_value $domain_size > ${sample_file_name}
else
${rainbow_dir}/src/Main/Release/WhiteNoiseSamples ${dimension} ${num_class} ${target_num_samples} ${domain_size} > ${sample_file_name} 
endif
endif

set all_sample_files = "$all_sample_files ${sample_file_name}";
@ all_sample_files_count = ${all_sample_files_count} + 1;

@ i = ${i} + 1;
end

foreach hist_option (normal zoom) # normal must proceed zoom

# compute histogram
echo ${all_sample_files} > ${output_dir}/all_sample_file_list.txt;

set normal_histogram_name = ${distribution}_${dimension}d_${num_class}c_${r_text}_${which_dimension_text}_${which_dimension_domain_size}s_${num_runs}_histogram;

set histogram_name = ${normal_histogram_name};

if(${hist_option} == 'zoom') then
set histogram_name = ${histogram_name}_zoom;
set my_diff = ${max_zoom_diff};
set my_bins = ${num_zoom_bins};
else
set my_diff = ${max_diff};
set my_bins = ${num_bins};
endif

echo compute ${output_dir}/${histogram_name}

if(${hist_option} == 'normal') then
./src/Main/Release/DiffHistogram ${output_dir}/all_sample_file_list.txt $dimension $which_dimension ${my_bins} ${sub_domain_size} ${my_diff} > ${output_dir}/${histogram_name}.txt
else
head -n ${my_bins} ${output_dir}/${normal_histogram_name}.txt > ${output_dir}/${histogram_name}.txt
endif

rm ${output_dir}/all_sample_file_list.txt;

# plot histogram
if(1) then
echo "" > foo.gp;

echo set terminal postscript eps >> foo.gp
echo set output \'${output_dir}/${histogram_name}.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"\|d\|\" >> foo.gp
echo set ylabel \"histogram\" >> foo.gp
echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
set max_bin = "";
@ max_bin = $num_bins - 1; 
#[0:${y_max}]
echo plot "[0:${my_diff}] [0:${y_max}]" \"${output_dir}/${histogram_name}.txt\" with line linetype rgb \"red\" >> foo.gp

wgnuplot foo.gp
rm foo.gp;
endif

end # foreach which_dimension_domain_size

end #foreach hist_option

# cleanup
if(1) then
set i = 0;
while(${i} < ${num_runs})
rm ${huge_dir}/${distribution}_${dimension}d_${num_class}c_${r_text}_${i}.txt;
@ i = ${i} + 1;
end
endif

end #foreach distribution

# plot histogram with reference white noise
if(0) then

set output_name = ${output_dir}/poisson_white_${dimension}d_${num_class}c_${r_text}_${which_dimension_text}_histogram_freq${hist_freq};
echo plot ${output_name}.eps

echo "" > foo.gp;

echo set terminal postscript eps >> foo.gp
echo set output \'${output_name}.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"\|coordinate difference\|\" >> foo.gp
echo set ylabel \"density\" >> foo.gp
#echo set key off >> foo.gp
echo set autoscale y >> foo.gp
set max_bin = "";
@ max_bin = $num_bins - 1;
echo plot "[0:1]" \"${output_dir}/poisson_${dimension}d_${num_class}c_${r_text}_${which_dimension_text}_histogram.txt\" title \'Poisson disk\' with lines linetype rgb \"red\", \"${output_dir}/white_${dimension}d_${num_class}c_${r_text}_${which_dimension_text}_histogram.txt\"  title \'uniform random\' with lines linetype rgb \"blue\" >> foo.gp

wgnuplot foo.gp
rm foo.gp;

epstopdf ${output_name}.eps;
cp ${output_name}.pdf foo.pdf
ps2pdf13 -dPDFSETTINGS=/prepress foo.pdf ${output_name}.pdf;

endif

endif

# 2D dart throwing with naive/mean white noise normalization
if(0) then

set dimension = 2;

set num_class = 1;
set priority = "0";

set r_values = "0.04";
set r_text = 0p04;

set k_value = 0.75;

# set num_samples = 405; # derived from dart throwing result

set domain_size = "1 1";

set num_runs = 10; #100;

set sft_freq = 64;
@ sft_size = ${sft_freq} * 2;
set sft_scale = ${sft_size};

set normalization_option = "sqrtsamplecount";
set norm_pft_option = "subadd";
set invariance_const = 1;

set white_name = 'white';
set whity_name = 'whity'; # for comparing against itself, white
set poisson_name = 'poisson';
set poisson_norm_name = 'poisson_norm';
set poisson_naive_name = 'poisson_naive';
set whity_norm_name = 'white_norm';
set whity_naive_name = 'white_naive';

set all_white_pfm_files = "";
set all_whity_pfm_files = "";
set all_poisson_pfm_files = "";
set all_poisson_naive_pfm_files = "";
set all_whity_naive_pfm_files = "";

set i = 0;
while(${i} < ${num_runs})

foreach output_name (${poisson_name} ${white_name} ${whity_name}) # poisson must preceed white to figure out the relevant # of samples

if(${output_name} == ${poisson_name}) then
set exe_name = DartThrowing;
else if((${output_name} == ${white_name}) || (${output_name} == ${whity_name})) then
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
./src/Main/Release/SampleSelector ${huge_dir}/${base_name}_${i}.txt ${dimension} "cout" 0 > ${huge_dir}/${base_name}_${i}_selected.txt
./src/Main/Release/SamplePFT ${huge_dir}/${base_name}_${i}_selected.txt ${huge_dir}/${base_name}_${i}.pfm ${dimension} 1 1 ${sft_size} ${sft_scale} ${normalization_option}

# num samples
if(${output_name} == ${poisson_name}) then
set final_samples = `./src/Main/Release/SampleSelector ${huge_dir}/${base_name}_${i}.txt ${dimension} cout 0`;
@ num_poisson_samples = $#final_samples / ${dimension};
endif

end #foreach output_name

# compute naive normalized power
./src/Main/Release/NormPFT ${norm_pft_option} ${invariance_const} ${huge_dir}/${poisson_name}_${dimension}d_${num_class}c_${r_text}_${i}.pfm ${huge_dir}/${white_name}_${dimension}d_${num_class}c_${r_text}_${i}.pfm ${huge_dir}/${poisson_naive_name}_${dimension}d_${num_class}c_${r_text}_${i}.pfm

./src/Main/Release/NormPFT ${norm_pft_option} ${invariance_const} ${huge_dir}/${whity_name}_${dimension}d_${num_class}c_${r_text}_${i}.pfm ${huge_dir}/${white_name}_${dimension}d_${num_class}c_${r_text}_${i}.pfm ${huge_dir}/${whity_naive_name}_${dimension}d_${num_class}c_${r_text}_${i}.pfm

# book keeping
set all_white_pfm_files = "${all_white_pfm_files} ${huge_dir}/${white_name}_${dimension}d_${num_class}c_${r_text}_${i}.pfm";
set all_whity_pfm_files = "${all_whity_pfm_files} ${huge_dir}/${whity_name}_${dimension}d_${num_class}c_${r_text}_${i}.pfm";
set all_poisson_pfm_files = "${all_poisson_pfm_files} ${huge_dir}/${poisson_name}_${dimension}d_${num_class}c_${r_text}_${i}.pfm";
set all_poisson_naive_pfm_files = "${all_poisson_naive_pfm_files} ${huge_dir}/${poisson_naive_name}_${dimension}d_${num_class}c_${r_text}_${i}.pfm";
set all_whity_pfm_files = "${all_whity_pfm_files} ${huge_dir}/${whity_name}_${dimension}d_${num_class}c_${r_text}_${i}.pfm";
set all_whity_naive_pfm_files = "${all_whity_naive_pfm_files} ${huge_dir}/${whity_naive_name}_${dimension}d_${num_class}c_${r_text}_${i}.pfm";

@ i = ${i} + 1;
end

# white mean for norm computation
set white_base_name = ${white_name}_${dimension}d_${num_class}c_${r_text};

if(1) then
./src/Main/Release/SyntheticPFT ${num_poisson_samples} "uniform" "infinite" ${output_dir}/${white_base_name}_average${num_runs}.pfm ${dimension} 1 1 ${sft_size} ${sft_scale} ${normalization_option}
else
echo ${all_white_pfm_files} > ${output_dir}/all_pfm_files_list.txt;
./src/Main/Release/RadialPlot ${output_dir}/all_pfm_files_list.txt ${output_dir}/${white_base_name}_average${num_runs}.pfm ${output_dir}/${white_base_name}_average${num_runs}_mean.txt ${output_dir}/${white_base_name}_average${num_runs}_variance.txt
rm ${output_dir}/all_pfm_files_list.txt;
endif

# norm computation
set all_poisson_norm_pfm_files = "";
set all_whity_norm_pfm_files = "";

set i = 0;
while(${i} < ${num_runs})

# compute normalized power
./src/Main/Release/NormPFT ${norm_pft_option} ${invariance_const} ${huge_dir}/${poisson_name}_${dimension}d_${num_class}c_${r_text}_${i}.pfm ${output_dir}/${white_base_name}_average${num_runs}.pfm ${huge_dir}/${poisson_norm_name}_${dimension}d_${num_class}c_${r_text}_${i}.pfm

./src/Main/Release/NormPFT ${norm_pft_option} ${invariance_const} ${huge_dir}/${whity_name}_${dimension}d_${num_class}c_${r_text}_${i}.pfm ${output_dir}/${white_base_name}_average${num_runs}.pfm ${huge_dir}/${whity_norm_name}_${dimension}d_${num_class}c_${r_text}_${i}.pfm

# book keeping
set all_poisson_norm_pfm_files = "${all_poisson_norm_pfm_files} ${huge_dir}/${poisson_norm_name}_${dimension}d_${num_class}c_${r_text}_${i}.pfm";
set all_whity_norm_pfm_files = "${all_whity_norm_pfm_files} ${huge_dir}/${whity_norm_name}_${dimension}d_${num_class}c_${r_text}_${i}.pfm";

@ i = ${i} + 1;
end

# radial computation
foreach output_name (${poisson_name} ${white_name} ${poisson_naive_name} ${whity_naive_name} ${poisson_norm_name} ${whity_norm_name})

set base_name = ${output_name}_${dimension}d_${num_class}c_${r_text};

if(${output_name} == ${poisson_name}) then
#set all_pfm_files = ${all_poisson_pfm_files};
echo ${all_poisson_pfm_files} > ${output_dir}/all_pfm_files_list.txt;
else if(${output_name} == ${white_name}) then
#set all_pfm_files = ${all_white_pfm_files};
echo ${all_white_pfm_files} > ${output_dir}/all_pfm_files_list.txt;
else if(${output_name} == ${poisson_naive_name}) then
#set all_pfm_files = ${all_poisson_naive_pfm_files};
echo ${all_poisson_naive_pfm_files} > ${output_dir}/all_pfm_files_list.txt;
else if(${output_name} == ${whity_naive_name}) then
#set all_pfm_files = ${all_whity_naive_pfm_files};
echo ${all_whity_naive_pfm_files} > ${output_dir}/all_pfm_files_list.txt;
else if(${output_name} == ${poisson_norm_name}) then
#set all_pfm_files = ${all_poisson_norm_pfm_files};
echo ${all_poisson_norm_pfm_files} > ${output_dir}/all_pfm_files_list.txt;
else if(${output_name} == ${whity_norm_name}) then
#set all_pfm_files = ${all_whity_norm_pfm_files};
echo ${all_whity_norm_pfm_files} > ${output_dir}/all_pfm_files_list.txt;
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
echo plot "[1:${sft_freq}] [-30:30]" \"${output_dir}/${base_name}_average${num_runs}_variance.dat\" with line linetype rgb \"blue\" >> foo.gp

wgnuplot foo.gp
rm foo.gp

foreach option (mean variance)
set final_name = ${base_name}_average${num_runs}_${option};
epstopdf ${output_dir}/${final_name}.eps --outfile ${output_dir}/${final_name}.pdf;
echo ${output_dir}/${final_name}.pdf;
end

foreach tonemap_option (0 1)
./src/Main/Release/PFM2PPM ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}_${tonemap_option}t.ppm 1 1 ${tonemap_option}
echo ${output_dir}/${base_name}_average${num_runs}_${tonemap_option}t.ppm
end

end # foreach output_name

# cleanup
if(0) then
rm -f ${all_white_pfm_files} ${all_whity_pfm_files} ${all_poisson_pfm_files} ${all_poisson_naive_pfm_files} ${all_whity_naive_pfm_files} ${all_poisson_norm_pfm_files} ${all_whity_norm_pfm_files};
endif

endif

# adaptive sampling
if(0) then

set dimension = 2;
set domain_size = "1 1";
set domain_size_string = "1x1";

set num_class = 1;
set priority = "0";

set r_values = "0.04";
set r_text = 0p04;

set k_value = 0.75;
set conflict_metric = 0; # mean

#set num_samples = 888;
set infty_num_runs = "infty";
set num_runs = 10;

set sft_freq = 256;
@ sft_size = ${sft_freq} * 2;
set sft_scale = ${sft_size};

set normalization_option = "sqrtsamplecount";
set norm_pft_option = "subadd";
set invariance_const = 1;

foreach domain_option ("uniform" "1blob") #"uniform"

if(${domain_option} == "uniform") then

set domain_res = 1x1;
echo "F2 2 1 1 1" > ${output_dir}/${domain_option}.f2m;

./src/Main/Release/F2M2PFM ${output_dir}/${domain_option}.f2m ${output_dir}/${domain_option}.pfm

else if(${domain_option} == "1blob") then

echo "1 2 1 0.5 0.5 0.3 0.3" > ${output_dir}/${domain_option}.txt

set domain_res_1d = 32;
set domain_res = ${domain_res_1d}x${domain_res_1d};

${rainbow_dir}/src/Sain/Release/MakeBlobs ${output_dir}/${domain_option}.txt ${output_dir}/${domain_option}.pfm ${domain_res_1d} 1 ${domain_size};

./src/Main/Release/PFM2F2M ${output_dir}/${domain_option}.pfm ${output_dir}/${domain_option}.f2m

else

echo unknown domain option;
exit(1);

endif

# real samples
set real_base_name = ${domain_option}_${dimension}d_${r_text};

set i = 0;
while (${i} < ${num_runs})

${rainbow_dir}/src/Main/Release/AdaptiveDartThrowing $dimension $num_class $priority $r_values $k_value $domain_size ${conflict_metric} ${output_dir}/${domain_option}.pfm > ${huge_dir}/${real_base_name}_${i}.txt

sleep 1;

# num samples
set final_samples = `./src/Main/Release/SampleSelector ${huge_dir}/${real_base_name}_${i}.txt ${dimension} cout 0`;
@ num_samples = $#final_samples / ${dimension};

@ i = ${i} + 1;
end

# synthetic spectrum
set synthetic_base_name = ${domain_option}_${dimension}d_white;

./src/Main/Release/SyntheticPFT ${num_samples} ${output_dir}/${domain_option}.f2m ${domain_size_string} ${output_dir}/${synthetic_base_name}_average${infty_num_runs}.pfm ${dimension} 1 1 ${sft_size} ${sft_scale} ${normalization_option}

./src/Main/Release/PFM2PPM ${output_dir}/${synthetic_base_name}_average${infty_num_runs}.pfm ${output_dir}/${synthetic_base_name}_average${infty_num_runs}.ppm 1 1 0
echo ${output_dir}/${synthetic_base_name}_average${infty_num_runs}.ppm

# real spectrum

set norm_base_name = ${real_base_name}_norm;

set all_poisson_pfm_files = "";
set all_norm_pfm_files = "";
set i = 0;
while (${i} < ${num_runs})

# compute sft
./src/Main/Release/SampleSelector ${huge_dir}/${real_base_name}_${i}.txt ${dimension} "cout" 0 > ${huge_dir}/${real_base_name}_${i}_selected.txt

./src/Main/Release/SamplePFT ${huge_dir}/${real_base_name}_${i}_selected.txt ${huge_dir}/${real_base_name}_${i}.pfm ${dimension} 1 1 ${sft_size} ${sft_scale} ${normalization_option}

set all_poisson_pfm_files = "${all_poisson_pfm_files} ${huge_dir}/${real_base_name}_${i}.pfm";

# norm sft
./src/Main/Release/NormPFT ${norm_pft_option} ${invariance_const} ${huge_dir}/${real_base_name}_${i}.pfm ${output_dir}/${synthetic_base_name}_average${infty_num_runs}.pfm ${huge_dir}/${norm_base_name}_${i}.pfm

set all_norm_pfm_files = "${all_norm_pfm_files} ${huge_dir}/${norm_base_name}_${i}.pfm";

@ i = ${i} + 1;
end


# radial computation
foreach output_name (${real_base_name} ${norm_base_name})

set base_name = ${output_name};

if(${output_name} == ${real_base_name}) then
#set all_pfm_files = ${all_poisson_pfm_files};
echo ${all_poisson_pfm_files} > ${output_dir}/all_pfm_files_list.txt;
else if(${output_name} == ${norm_base_name}) then
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

end #foreach domain_option

endif

# cascade white noise
if(0) then

set dimension = 2;

set domain_size = "1 1";
set num_samples = 400;
set sub_domain_scale = 0.05;

set num_runs = 10;

set sft_freq = 64;
@ sft_size = ${sft_freq} * 2;
set sft_scale = ${sft_size};

set normalization_option = "sqrtsamplecount";

foreach boundary_condition (0 1)

set base_name = cascade_white_b${boundary_condition};

# generate samples
set i = 0;
while(${i} < ${num_runs})

./src/Main/Release/CascadeWhiteNoise ${dimension} ${num_samples} ${domain_size} ${sub_domain_scale} ${boundary_condition} > ${huge_dir}/${base_name}_${i}.txt

sleep 1
@ i = ${i} + 1;
end

# spectrum
set all_pfm_files = "";
set i = 0;
while (${i} < ${num_runs})

# compute sft
./src/Main/Release/SampleSelector ${huge_dir}/${base_name}_${i}.txt ${dimension} "cout" 0 > ${huge_dir}/${base_name}_${i}_selected.txt

./src/Main/Release/SamplePFT ${huge_dir}/${base_name}_${i}_selected.txt ${huge_dir}/${base_name}_${i}.pfm ${dimension} 1 1 ${sft_size} ${sft_scale} ${normalization_option}

set all_pfm_files = "${all_pfm_files} ${huge_dir}/${base_name}_${i}.pfm";

@ i = ${i} + 1;
end

# radial computation
echo ${all_pfm_files} > ${output_dir}/all_pfm_files_list.txt;

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

end #foreach boundary_condition 

endif

# diff transform, (simulated) infinite domain
if(0) then

set dimension = 2;

set r_value = 0.08;
set k_value = 0.75;

set domain_size = "3 3";
set sample_rate = "30 30"; # for regular grid

set sub_domain_min = "1 1";
set sub_domain_max = "2 2";

set diff_domain_max = "1 1";

set kernel_option = "gaussian";
set kernel_size = 1;
set keep_dc = 0;
set output_resolution = 64;
set sft_freq = ${output_resolution};

set num_runs = 10;

foreach method ("poisson" "white" "grid") # poisson must proceed white for sample count

set base_name = ${method}_diffxform;

# generate samples
set i = 0;
while(${i} < ${num_runs})

if(${method} == "poisson") then
${rainbow_dir}/src/Main/Release/DartThrowing ${dimension} 1 0 ${r_value} ${k_value} $domain_size > ${huge_dir}/${base_name}_${i}.txt;
else if(${method} == "white") then
./src/Main/Release/UniformWhiteNoise ${dimension} ${num_samples} ${domain_size} > ${huge_dir}/${base_name}_${i}.txt
else if(${method} == "grid") then
./src/Main/Release/RegularGrid ${dimension} ${domain_size} ${sample_rate} 0 > ${huge_dir}/${base_name}_${i}.txt
else
echo unknown sampling method;
exit(1);
endif

sleep 1;

# num samples
set final_samples = `./src/Main/Release/SampleSelector ${huge_dir}/${base_name}_${i}.txt ${dimension} cout 0`;
@ num_samples = $#final_samples / ${dimension};

@ i = ${i} + 1;
end

# diff transform
set all_pfm_files = "";

set i = 0;
while(${i} < ${num_runs})

echo ${huge_dir}/${base_name}_${i}.txt > ${output_dir}/all_sample_files_list.txt;
./src/Main/Release/UniformDiffXform ${output_dir}/all_sample_files_list.txt ${dimension} ${sub_domain_min} ${sub_domain_max} ${diff_domain_max} ${kernel_option} ${kernel_size} ${keep_dc} ${output_resolution} ${huge_dir}/${base_name}_${i}.f2m;

./src/Main/Release/F2M2PFM ${huge_dir}/${base_name}_${i}.f2m ${huge_dir}/${base_name}_${i}.pfm;

set all_pfm_files = "${all_pfm_files} ${huge_dir}/${base_name}_${i}.pfm";

@ i = ${i} + 1;
end

rm ${output_dir}/all_sample_files_list.txt;

# radial computation
echo ${all_pfm_files} > ${output_dir}/all_pfm_files_list.txt;

./src/Main/Release/RadialPlot ${output_dir}/all_pfm_files_list.txt ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt
rm ${output_dir}/all_pfm_files_list.txt;

# radial plot
set hist_normalization_const = 1;

${rainbow_dir}/src/Scripts/txt2dat ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt ${hist_normalization_const} ${output_dir}/${base_name}_average${num_runs}_mean.dat ${output_dir}/${base_name}_average${num_runs}_variance.dat 1

echo set terminal postscript eps >> foo.gp
echo set output \'${output_dir}/${base_name}_average${num_runs}_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
echo plot "[1:${sft_freq}] [0:3]" \"${output_dir}/${base_name}_average${num_runs}_mean.dat\" with line linetype rgb \"red\" >> foo.gp

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

end # foreach method

endif

# adaptive sampling + diff xform
if(0) then

set dimension = 2;

set r_value = 0.04;
set r_text = 0p04;

set k_value = 0.75;
set conflict_metric = 0; # mean

set domain_size = "1 1";

set sub_domain_min = "0 0";
set sub_domain_max = "1 1";

set diff_domain_max = "1 1";

set kernel_option = "gaussian";
set kernel_size = 1;
set keep_dc = 0;
set output_resolution = 64;
set sft_freq = ${output_resolution};

set num_runs = 10;

set norm_pft_option = "subadd" 
set invariance_const = 1;

foreach domain_option ("uniform" "1blob") #"uniform"

if(${domain_option} == "uniform") then

set domain_res = 1x1;
echo "F2 2 1 1 1" > ${output_dir}/${domain_option}.f2m;

./src/Main/Release/F2M2PFM ${output_dir}/${domain_option}.f2m ${output_dir}/${domain_option}.pfm

else if(${domain_option} == "1blob") then

echo "1 2 1 0.5 0.5 0.3 0.3" > ${output_dir}/${domain_option}.txt

set domain_res_1d = 32;
set domain_res = ${domain_res_1d}x${domain_res_1d};

${rainbow_dir}/src/Sain/Release/MakeBlobs ${output_dir}/${domain_option}.txt ${output_dir}/${domain_option}.pfm ${domain_res_1d} 1 ${domain_size};

./src/Main/Release/PFM2F2M ${output_dir}/${domain_option}.pfm ${output_dir}/${domain_option}.f2m

else

echo unknown domain option;
exit(1);

endif

foreach method ("poisson" "white") # poisson must proceed white for sample count

set base_name = ${domain_option}_${method}_${r_text}_diffxform;

# generate samples
set i = 0;
while(${i} < ${num_runs})

if(${method} == "poisson") then
${rainbow_dir}/src/Main/Release/AdaptiveDartThrowing ${dimension} 1 0 ${r_value} ${k_value} $domain_size ${conflict_metric} ${output_dir}/${domain_option}.pfm > ${huge_dir}/${base_name}_${i}.txt
else if(${method} == "white") then
./src/Main/Release/IsotropicWhiteNoise ${output_dir}/${domain_option}.f2m ${num_samples} ${domain_size} > ${huge_dir}/${base_name}_${i}.txt
else
echo unknown sampling method;
exit(1);
endif

sleep 1;

# num samples
set final_samples = `./src/Main/Release/SampleSelector ${huge_dir}/${base_name}_${i}.txt ${dimension} cout 0`;
@ num_samples = $#final_samples / ${dimension};

@ i = ${i} + 1;
end

# diff transform
set i = 0;
while(${i} < ${num_runs})

echo ${huge_dir}/${base_name}_${i}.txt > ${output_dir}/all_sample_files_list.txt;
./src/Main/Release/UniformDiffXform ${output_dir}/all_sample_files_list.txt ${dimension} ${sub_domain_min} ${sub_domain_max} ${diff_domain_max} ${kernel_option} ${kernel_size} ${keep_dc} ${output_resolution} ${huge_dir}/${base_name}_${i}.f2m;

./src/Main/Release/F2M2PFM ${huge_dir}/${base_name}_${i}.f2m ${huge_dir}/${base_name}_${i}.pfm;

@ i = ${i} + 1;
end

rm ${output_dir}/all_sample_files_list.txt;

end # foreach method

set white_name = ${domain_option}_white_${r_text}_diffxform;
set poisson_name = ${domain_option}_poisson_${r_text}_diffxform;

# normalization
set all_pfm_files = "";

set i = 0;
while(${i} < ${num_runs})

./src/Main/Release/NormPFT ${norm_pft_option} ${invariance_const} ${huge_dir}/${poisson_name}_${i}.pfm ${huge_dir}/${white_name}_${i}.pfm ${huge_dir}/${poisson_name}_${i}.pfm

set all_pfm_files = "${all_pfm_files} ${huge_dir}/${poisson_name}_${i}.pfm";

@ i = ${i} + 1;
end

# radial computation
echo ${all_pfm_files} > ${output_dir}/all_pfm_files_list.txt;

set base_name = ${domain_option}_poisson_${r_text}_norm_diffxform;

./src/Main/Release/RadialPlot ${output_dir}/all_pfm_files_list.txt ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt
rm ${output_dir}/all_pfm_files_list.txt;

# radial plot
set hist_normalization_const = 1;

${rainbow_dir}/src/Scripts/txt2dat ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt ${hist_normalization_const} ${output_dir}/${base_name}_average${num_runs}_mean.dat ${output_dir}/${base_name}_average${num_runs}_variance.dat 1

echo set terminal postscript eps >> foo.gp
echo set output \'${output_dir}/${base_name}_average${num_runs}_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
echo plot "[1:${sft_freq}] [-10:3]" \"${output_dir}/${base_name}_average${num_runs}_mean.dat\" with line linetype rgb \"red\" >> foo.gp

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

./src/Main/Release/PFM2PPM ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}.ppm 1 1 2
echo ${output_dir}/${base_name}_average${num_runs}.ppm

end # foreach domain_option

endif

# uniform diff transform with different sampling methods
if(0) then

set generate_samples = 1;

set dimension = 2;
set domain_size = "1 1";

# poisson
set r_value = 0.03;
set r_text = 0p03;
set k_value = 0.75;
set conflict_metric = 0; # mean

# white noise
set num_samples = 721;

# lloyd
set num_lloyd_iterations = 100;

# balzer
set num_points_per_site = 64;
set voronoi_scheme = balzer;
set centroid_scheme = none;
set class_weight_scheme = fair;
set min_movement = 0.01;
set max_num_iterations = 10;

# grid
# 729 samples
set sample_rate = "27 27";

# ppentomino
# 720 samples
set ppentomino_grid_size = 30;
set ppentomino_template_size = 10;
set ppentomino_num_template = 4;
set ppentomino_num_subdivision = 1;
set ppentomino_num_iteration = 100;

set domain_option = "uniform";

set isotropic_average_distance = ${r_value};
set max_relative_dd = -1; #0.05;
set max_relative_dd_text = inf; #0p05;
set local_diff_range = 10;
set diff_norm = "unit";
set diff_weight = "unit";
set max_cache_size = 1;

set sim_infinite_domain = 1; # via 3x3 tiling

if(${sim_infinite_domain}) then
set sub_domain_min = "1 1";
set sub_domain_max = "2 2";
else
set sub_domain_min = "0 0";
set sub_domain_max = "1 1";
endif

set diff_max = 0.2;
set diff_domain_max = "${diff_max} ${diff_max}";

set kernel_size = 1;
set diffxform_hist_normalization_const = 1; # related to kernel_size
#set keep_dc = 0;
set output_resolution = 640; # per unit domain
set diff_scale = 0.0015625; # 1/output_resolution

# sft stuff
set sft_freq = 128; #output_resolution * diff_max
@ sft_size = ${sft_freq} * 2;
set sft_scale = ${sft_size};
set normalization_option = "sqrtsamplecount";
set sft_hist_normalization_const = 1;

set num_runs = 10;

if(${domain_option} == "uniform") then

set domain_res = 1x1;
set importance_cell_size = 3;
echo "F2 2 1 1 1" > ${output_dir}/${domain_option}.f2m;

./src/Main/Release/F2M2PFM ${output_dir}/${domain_option}.f2m ${output_dir}/${domain_option}.pfm

else

echo unknown domain option;
exit(1);

endif

foreach method ("poisson" "white" "grid0" "grid1" "lloyd" "ppentomino" "balzer") # poisson must preceed white to count sample

set head_name = ${domain_option}_${r_text};
set base_name = ${head_name}_${method};

set white_base_name = ${head_name}_white;
set poisson_base_name = ${head_name}_poisson;

# generate samples
set all_sample_files = "";
set all_tiled_sample_files = "";

set i = 0;
while(${i} < ${num_runs})

if(${generate_samples}) then

if(${method} == "white") then

./src/Main/Release/IsotropicWhiteNoise ${output_dir}/${domain_option}.f2m ${num_samples} ${domain_size} > ${huge_dir}/${base_name}_${i}.txt

else if(${method} == "poisson") then

${rainbow_dir}/src/Main/Release/AdaptiveDartThrowing ${dimension} 1 0 ${r_value} ${k_value} ${domain_size} ${conflict_metric} ${output_dir}/${domain_option}.pfm > ${huge_dir}/${base_name}_${i}.txt

else if(${method} == "lloyd") then

./src/Main/Release/IsotropicWhiteNoise ${output_dir}/${domain_option}.f2m ${num_samples} ${domain_size} > ${huge_dir}/${white_base_name}_${i}.txt

${rainbow_dir}/src/Main/Release/LloydRelaxation2D ${huge_dir}/${white_base_name}_${i}.txt ${dimension} ${domain_size} ${num_lloyd_iterations} > ${huge_dir}/${base_name}_${i}.txt

else if(${method} == "balzer") then

@ num_points = ${num_samples} * ${num_points_per_site};

if(0) then

# random sites
./src/Main/Release/IsotropicWhiteNoise ${output_dir}/${domain_option}.f2m ${num_points} ${domain_size} > ${huge_dir}/${white_base_name}_${i}.txt

./src/Main/Release/RandomPickPointsSites ${huge_dir}/${white_base_name}_${i}.txt ${dimension} 1 ${num_samples} > ${huge_dir}/${white_base_name}_points_${i}.txt

else

# poisson sites
@ num_points_minus_sites = ${num_points} - ${num_samples}
./src/Main/Release/IsotropicWhiteNoise ${output_dir}/${domain_option}.f2m ${num_points_minus_sites} ${domain_size} > ${huge_dir}/${white_base_name}_${i}.txt

./src/Main/Release/RandomPickPointsSites ${huge_dir}/${white_base_name}_${i}.txt ${dimension} 1 0 > ${huge_dir}/${white_base_name}_only_points_${i}.txt

cat ${huge_dir}/${white_base_name}_only_points_${i}.txt ${huge_dir}/${poisson_base_name}_${i}.txt > ${huge_dir}/${white_base_name}_points_${i}.txt

endif

${rainbow_dir}/src/Main/Release/LloydRelaxation ${huge_dir}/${white_base_name}_points_${i}.txt ${dimension} ${domain_size} ${method} ${centroid_scheme} ${class_weight_scheme} ${min_movement} ${max_num_iterations} > ${huge_dir}/${base_name}_${i}.txt

else if((${method} == "grid0") || (${method} == "grid1")) then

if(${method} == "grid1") then
set jitter_scale = 1;
else
set jitter_scale = 0;
endif

./src/Main/Release/RegularGrid ${dimension} ${domain_size} ${sample_rate} ${jitter_scale} > ${huge_dir}/${base_name}_${i}.txt

else if(${method} == "ppentomino") then

set grid_size = ${ppentomino_grid_size};
set template_size = ${ppentomino_template_size};
set num_template = ${ppentomino_num_template};
set num_subdivision = ${ppentomino_num_subdivision};
set num_iteration = ${ppentomino_num_iteration};

${blue_dir}/src/Main/Release/PolyominoRelaxation ${method} ${grid_size} ${template_size} ${num_template} ${num_subdivision} ${num_iteration} > ${huge_dir}/${base_name}_${i}.txt

else

echo unknown sampling method;
exit(1);

endif

sleep 1;

endif #(${generate_samples}) then

set all_sample_files = "${all_sample_files} ${huge_dir}/${base_name}_${i}.txt";

# num samples
set final_samples = `./src/Main/Release/SampleSelector ${huge_dir}/${base_name}_${i}.txt ${dimension} cout 0`;
@ num_samples = $#final_samples / ${dimension};

# tiling
if(${sim_infinite_domain}) then

${rainbow_dir}//src/Main/Release/TileSamples ${huge_dir}/${base_name}_${i}.txt ${dimension} 1 3 3 ${domain_size} > ${huge_dir}/${base_name}_tiled_${i}.txt;

set all_tiled_sample_files = "${all_tiled_sample_files} ${huge_dir}/${base_name}_tiled_${i}.txt";

endif

# draw samples
if(${i} == 10) then

echo "0 0 0" > ${huge_dir}/palette.txt

set dot_radius = 0.005;

./src/Main/Release/DrawSamples ${huge_dir}/${base_name}_${i}.txt ${huge_dir}/${base_name}_${i}.fig ${dimension} ${dot_radius} 1 ${domain_size} ${huge_dir}/palette.txt
src/Scripts/fig2eps -f ${huge_dir}/${base_name}_${i}.fig
epstopdf ${huge_dir}/${base_name}_${i}.eps --outfile ${huge_dir}/${base_name}_${i}.pdf
rm ${huge_dir}/palette.txt

endif

@ i = ${i} + 1;
end

set sample_base_name = ${base_name};

# diff transform
foreach kernel_option ("gaussian" "sft") # "cos")

if(${kernel_option} == "gaussian") then
set keep_dc = 0;
set clamp_radius = 0;
else
set keep_dc = 1;
set clamp_radius = 1;
endif

if(${kernel_option} == "sft") then
set tail_name = sft;
else
set tail_name = diffxform_${kernel_option}_${local_diff_range}r_${max_relative_dd_text}dd_${diff_norm}_${diff_weight};
endif

set base_name = ${head_name}_${method}_${tail_name};

if(${kernel_option} == "sft") then

set all_pfm_files = "";
set i = 0;
while(${i} < ${num_runs})

# compute sft
./src/Main/Release/SampleSelector ${huge_dir}/${sample_base_name}_${i}.txt ${dimension} "cout" 0 > ${huge_dir}/${sample_base_name}_${i}_selected.txt

./src/Main/Release/SamplePFT ${huge_dir}/${sample_base_name}_${i}_selected.txt ${huge_dir}/${sample_base_name}_${i}.pfm ${dimension} 1 1 ${sft_size} ${sft_scale} ${normalization_option}

set all_pfm_files = "${all_pfm_files} ${huge_dir}/${sample_base_name}_${i}.pfm";

@ i = ${i} + 1;
end

echo ${all_pfm_files} > ${output_dir}/all_pfm_files_list.txt;

else

if(${sim_infinite_domain}) then
echo ${all_tiled_sample_files} > ${output_dir}/all_sample_files_list.txt;
else
echo ${all_sample_files} > ${output_dir}/all_sample_files_list.txt;
endif

./src/Main/Release/IsotropicDiffXform ${output_dir}/${domain_option}.f2m ${importance_cell_size} ${isotropic_average_distance} ${max_relative_dd} ${local_diff_range} ${diff_norm} ${diff_weight} ${max_cache_size} ${output_dir}/all_sample_files_list.txt ${dimension} ${sub_domain_min} ${sub_domain_max} ${diff_domain_max} ${kernel_option} ${kernel_size} ${keep_dc} ${output_resolution} ${output_dir}/${base_name}_average${num_runs}.f2m;

foreach core_weight_scale (1.0 1.2 1.4 1.6 1.8 2.0)
./src/Main/Release/Blueness ${output_dir}/${base_name}_average${num_runs}.f2m ${diff_scale} ${diff_domain_max} ${r_value} ${core_weight_scale}
end

rm ${output_dir}/all_sample_files_list.txt;

./src/Main/Release/F2M2PFM ${output_dir}/${base_name}_average${num_runs}.f2m ${output_dir}/${base_name}_average${num_runs}.pfm;

echo ${output_dir}/${base_name}_average${num_runs}.pfm > ${output_dir}/all_pfm_files_list.txt;

endif

# radial computation
./src/Main/Release/RadialPlot ${output_dir}/all_pfm_files_list.txt ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt
rm ${output_dir}/all_pfm_files_list.txt;

./src/Main/Release/PFM2PPM ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}.ppm 1 1 0

echo ${output_dir}/${base_name}_average${num_runs}.ppm

# radial plot

if(${kernel_option} == "sft") then
set scaling_factor = 1;
set norm_factor = ${sft_hist_normalization_const};
else
set scaling_factor = ${diff_scale};
set norm_factor = ${diffxform_hist_normalization_const};
endif

./src/Scripts/txt2dat ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt ${norm_factor} ${output_dir}/${base_name}_average${num_runs}_mean.dat ${output_dir}/${base_name}_average${num_runs}_variance.dat ${scaling_factor}

if((${method} == "poisson") || (${method} == "lloyd") || (${method} == "ppentomino") || (${method} == "balzer")) then
set behead_index_threshold = ${r_value};
else if(${method} == "grid1") then
set behead_index_threshold = 0.01;
else
set behead_index_threshold = -1;
endif

if(${kernel_option} == "gaussian") then
./src/Scripts/behead ${output_dir}/${base_name}_average${num_runs}_variance.dat ${output_dir}/${base_name}_average${num_runs}_variance.dat ${behead_index_threshold}
endif

echo set terminal postscript eps >> foo.gp
echo set output \'${output_dir}/${base_name}_average${num_runs}_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
if(${kernel_option} == "sft") then
echo set xlabel \"frequency\" >> foo.gp
else
echo set xlabel \"\|d\|\" >> foo.gp
endif
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
if(${kernel_option} == "sft") then
set x_min = 1;
set x_max = ${sft_freq};
else
set x_min = 0;
set x_max = ${diff_max};
endif
echo plot "[${x_min}:${x_max}] [0:3]" \"${output_dir}/${base_name}_average${num_runs}_mean.dat\" with line linetype rgb \"red\" >> foo.gp

echo set terminal postscript eps >> foo.gp
echo set output \'${output_dir}/${base_name}_average${num_runs}_variance.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
if(${kernel_option} == "sft") then
echo set xlabel \"frequency\" >> foo.gp
else
echo set xlabel \"\|d\|\" >> foo.gp
endif
echo set ylabel \"anisotropy\" >> foo.gp
echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
echo plot "[${x_min}:${x_max}] [-30:30]" \"${output_dir}/${base_name}_average${num_runs}_variance.dat\" with line linetype rgb \"blue\" >> foo.gp

wgnuplot foo.gp
rm foo.gp

foreach option (mean variance)
set final_name = ${base_name}_average${num_runs}_${option};
epstopdf ${output_dir}/${final_name}.eps --outfile ${output_dir}/${final_name}.pdf;
echo ${output_dir}/${final_name}.pdf;
end

end # foreach kernel_option 

end # foreach method

endif

# isotropic/adaptive diff transform
# use a fixed r_value, but change domain_size so as to control relative density with different domains
if(0) then

set generate_samples = 1;

set rho_only = 1;

#set poisson_exe = "AdaptiveDartThrowing";

set dimension = 2;

set default_domain_size = "1 1";

set default_domain_res_1d = 250;
set default_importance_cell_size = 0.004; # 1.0/250

# poisson
set r_value = 0.03;
set r_text = 0p03;
set k_value = 0.70; # 0.75 for uniform, 0.70 for adaptive
set conflict_metric = 0; # mean
set boundary = 1;
set sample_grid_resolution_per_unit_domain = 50;

# these number came from k_value = 0.75
# set num_samples = 721; # used when generating only white noise
# set num_samples = 2884;
# set num_samples = 11536;
# set num_samples = 46144;

set isotropic_average_distance = ${r_value};
set max_relative_dd = -1; #0.05;
set max_relative_dd_text = inf; #0p05;
set local_diff_range = 12;
if(${rho_only}) then
set local_diff_range = 3;
endif

set max_cache_size = 4;

set sim_infinite_domain = 1; # via 3x3 tiling
set sim_infinite_tiling = "3 3";

if(${sim_infinite_domain}) then
set default_sub_domain_min = "1 1";
set default_sub_domain_max = "2 2";
else
set default_sub_domain_min = "0 0";
set default_sub_domain_max = "1 1";
endif

set diff_max = 0.15;
set diff_domain_max = "${diff_max} ${diff_max}";

set kernel_size = 1;
set diffxform_hist_normalization_const = 1; # related to kernel_size
set keep_dc = 0;
set output_resolution = 640; # per unit domain
set diff_scale = 0.0015625; # 1/output_resolution

# sft stuff
set sft_freq = 96; #output_resolution * diff_max
@ sft_size = ${sft_freq} * 2;
set sft_scale = ${sft_size};
set normalization_option = "sqrtsamplecount";
set sft_hist_normalization_const = 1;
set clamp_radius = 1;

set default_num_runs = 10;

foreach domain_option ("1blob" "gauss" "balzer" "ostro" "uniostro" "portrait")# "uniform" "plant" "cos")
#foreach frequency (1 2 4 8)

echo ${domain_option}

# default set up
set domain_size = "${default_domain_size}";
set sub_domain_min = "${default_sub_domain_min}";
set sub_domain_max = "${default_sub_domain_max}";

set domain_res_1d = ${default_domain_res_1d};
set importance_cell_size = ${default_importance_cell_size};

set num_runs = ${default_num_runs};

if((${domain_option} == "uniform") || (${domain_option} == "uniostro")) then

set domain_res = 1x1;
set importance_cell_size = 1;
echo "F2 2 1 1 1" > ${output_dir}/${domain_option}.f2m;

if(0) then
set domain_size = "2 2";

if(${sim_infinite_domain}) then
set sub_domain_min = "2 2";
set sub_domain_max = "4 4";
else
set sub_domain_min = "0 0";
set sub_domain_max = "2 2";
endif

set importance_cell_size = 2; # 2/1;
endif

./src/Main/Release/F2M2PFM ${output_dir}/${domain_option}.f2m ${output_dir}/${domain_option}.pfm

else if(${domain_option} == "1blob") then

echo "1 2 1 0.5 0.5 0.3 0.3" > ${output_dir}/${domain_option}.txt

if(0) then
set domain_res_1d = 32;
set importance_cell_size = 0.03125; # 1.0/32

set domain_res_1d = 64;
set importance_cell_size = 0.015625; # 1.0/64

endif

if(1) then
set domain_size = "2 2";

echo "1 2 1 1.0 1.0 0.6 0.6" > ${output_dir}/${domain_option}.txt

if(${sim_infinite_domain}) then
set sub_domain_min = "2 2";
set sub_domain_max = "4 4";
else
set sub_domain_min = "0 0";
set sub_domain_max = "2 2";
endif

#set domain_res_1d = 32;
#set importance_cell_size = 0.008; # 2/250;
endif

set domain_res = ${domain_res_1d}x${domain_res_1d};

${rainbow_dir}/src/Sain/Release/MakeBlobs ${output_dir}/${domain_option}.txt ${output_dir}/${domain_option}.pfm ${domain_res_1d} 1 ${domain_size};

./src/Main/Release/PFM2F2M ${output_dir}/${domain_option}.pfm ${output_dir}/${domain_option}.f2m

else if(${domain_option} == "gauss") then

echo "1 2 1 0.5 0.5 1000000 0.3" > ${output_dir}/${domain_option}.txt

if(0) then
set domain_res_1d = 32;
set importance_cell_size = 0.03125; # 1.0/32

set domain_res_1d = 64;
set importance_cell_size = 0.015625; # 1.0/64
endif

if(1) then
set domain_size = "2 2";

echo "1 2 1 1.0 1.0 2000000 0.6" > ${output_dir}/${domain_option}.txt

if(${sim_infinite_domain}) then
set sub_domain_min = "2 2";
set sub_domain_max = "4 4";
else
set sub_domain_min = "0 0";
set sub_domain_max = "2 2";
endif

# set domain_res_1d = 32;
# set importance_cell_size = 0.008; # 2/250;
endif

set domain_res = ${domain_res_1d}x${domain_res_1d};

${rainbow_dir}/src/Sain/Release/MakeBlobs ${output_dir}/${domain_option}.txt ${output_dir}/${domain_option}.pfm ${domain_res_1d} 1 ${domain_size};

./src/Main/Release/PFM2F2M ${output_dir}/${domain_option}.pfm ${output_dir}/${domain_option}.f2m

else if(${domain_option} == "balzer") then

set importance_domain_size = "2 2";
set importance_domain_offset = "-1 -1";
set importance_range_offset = 0.01;

set exp_value = -20;
set sin_weight = 0.2;
set sin_freq = 1;

if(0) then
set domain_res_1d = 32;
set importance_cell_size = 0.015625; # 1.0/64; note that the effective domain_res_1d is 64 due to importance_domain_size
endif

if(1) then
set domain_size = "2 2";

if(${sim_infinite_domain}) then
set sub_domain_min = "2 2";
set sub_domain_max = "4 4";
else
set sub_domain_min = "0 0";
set sub_domain_max = "2 2";
endif

set importance_cell_size = 0.004; # 2.0/500;
endif

if(1) then
set domain_size = "4 4";

if(${sim_infinite_domain}) then
set sub_domain_min = "4 4";
set sub_domain_max = "8 8";
else
set sub_domain_min = "0 0";
set sub_domain_max = "4 4";
endif

set importance_cell_size = 0.008; # 4.0/550;

set num_runs = 4;
endif

set domain_res = ${domain_res_1d}x${domain_res_1d};

./src/Main/Release/MakeImportance ${dimension} ${domain_res_1d} ${importance_domain_size} ${importance_domain_offset} ${importance_range_offset} ${output_dir}/${domain_option}.f2m ${domain_option} ${exp_value} ${sin_weight} ${sin_freq};

./src/Main/Release/F2M2PFM ${output_dir}/${domain_option}.f2m ${output_dir}/${domain_option}.pfm;

else if(${domain_option} == "ostro") then

set importance_domain_size = "1 1";
set importance_domain_offset = "0 0";
set importance_range_offset = 0.01;

if(0) then
set domain_res_1d = 64;
set importance_cell_size = 0.015625; # 1.0/64;
endif

if(0) then
set domain_size = "2 2";

if(${sim_infinite_domain}) then
set sub_domain_min = "2 2";
set sub_domain_max = "4 4";
else
set sub_domain_min = "0 0";
set sub_domain_max = "2 2";
endif

set importance_cell_size = 0.03125; # 2.0/64;
endif

set domain_res = ${domain_res_1d}x${domain_res_1d};

if(1) then
convert ${output_dir}/${domain_option}.png -negate -compress none -resize ${domain_res_1d}x${domain_res_1d} ${output_dir}/${domain_option}.pgm
endif

./src/Main/Release/PGM2F2M ${output_dir}/${domain_option}.pgm ${output_dir}/${domain_option}.f2m

./src/Main/Release/MakeImportance ${dimension} ${domain_res_1d} ${importance_domain_size} ${importance_domain_offset} ${importance_range_offset} ${output_dir}/${domain_option}.f2m ${output_dir}/${domain_option}.f2m;

./src/Main/Release/F2M2PFM ${output_dir}/${domain_option}.f2m ${output_dir}/${domain_option}.pfm;

else if(${domain_option} == "plant") then

set importance_domain_size = "2 2";
set importance_domain_offset = "0 0";
set importance_range_offset = 0.01;

set domain_res_1d = 64;
set domain_res_1d_total = 128;

set importance_cell_size = 0.0078125; # 1.0/128; note that the effective domain_res_1d is 64 due to importance_domain_size

if(1) then
set domain_size = "2 2";

if(${sim_infinite_domain}) then
set sub_domain_min = "2 2";
set sub_domain_max = "4 4";
else
set sub_domain_min = "0 0";
set sub_domain_max = "2 2";
endif

set importance_cell_size = 0.015625; # 2.0/128;
endif

if(0) then
set domain_size = "4 4";

if(${sim_infinite_domain}) then
set sub_domain_min = "4 4";
set sub_domain_max = "8 8";
else
set sub_domain_min = "0 0";
set sub_domain_max = "4 4";
endif

set importance_cell_size = 0.03125; # 4.0/128;
endif

set domain_res = ${domain_res_1d}x${domain_res_1d};

if(1) then
convert -size 1024x1024 xc:none -fill white -draw "rectangle 1, 1, 1024, 1024" ${output_dir}/background.png;

composite -gravity center ${output_dir}/${domain_option}.png ${output_dir}/background.png ${output_dir}/${domain_option}_square.png

convert ${output_dir}/${domain_option}_square.png -negate -compress none -resize ${domain_res_1d_total}x${domain_res_1d_total} ${output_dir}/${domain_option}.pgm
endif

./src/Main/Release/PGM2F2M ${output_dir}/${domain_option}.pgm ${output_dir}/${domain_option}.f2m

./src/Main/Release/MakeImportance ${dimension} ${domain_res_1d} ${importance_domain_size} ${importance_domain_offset} ${importance_range_offset} ${output_dir}/${domain_option}.f2m ${output_dir}/${domain_option}.f2m;

./src/Main/Release/F2M2PFM ${output_dir}/${domain_option}.f2m ${output_dir}/${domain_option}.pfm;

else if(${domain_option} == "portrait") then

set num_runs = 1;

set importance_domain_size = "1 1";
set importance_domain_offset = "0 0";
set importance_range_offset = 0.01;

set domain_res_1d = 500;

set importance_cell_size = 0.002; # 1.0/500;

if(0) then
set domain_size = "2 2";

if(${sim_infinite_domain}) then
set sub_domain_min = "2 2";
set sub_domain_max = "4 4";
else
set sub_domain_min = "0 0";
set sub_domain_max = "2 2";
endif

set importance_cell_size = 0.004; # 2.0/500;
endif

if(0) then
set domain_size = "4 4";

if(${sim_infinite_domain}) then
set sub_domain_min = "4 4";
set sub_domain_max = "8 8";
else
set sub_domain_min = "0 0";
set sub_domain_max = "4 4";
endif

set importance_cell_size = 0.008; # 4.0/500;
endif

if(1) then
set domain_size = "8 8";

if(${sim_infinite_domain}) then
set sub_domain_min = "8 8";
set sub_domain_max = "16 16";
else
set sub_domain_min = "0 0";
set sub_domain_max = "8 8";
endif

set importance_cell_size = 0.016; # 8.0/500;
endif

set domain_res = ${domain_res_1d}x${domain_res_1d};

if(1) then
convert ${output_dir}/${domain_option}.png -negate -compress none -crop 500x500+8+0 -resize ${domain_res_1d}x${domain_res_1d} ${output_dir}/${domain_option}.pgm
endif

./src/Main/Release/PGM2F2M ${output_dir}/${domain_option}.pgm ${output_dir}/${domain_option}.f2m

./src/Main/Release/MakeImportance ${dimension} ${domain_res_1d} ${importance_domain_size} ${importance_domain_offset} ${importance_range_offset} ${output_dir}/${domain_option}.f2m ${output_dir}/${domain_option}.f2m;

./src/Main/Release/F2M2PFM ${output_dir}/${domain_option}.f2m ${output_dir}/${domain_option}.pfm;

else if(${domain_option} == "cos") then

set importance_domain_size = "1 1";
set importance_domain_offset = "0 0";
set importance_range_offset = 0.02;

set cos_freq = ${frequency};

set domain_res_1d = 64;
set importance_cell_size = 0.015625; # 1.0/64; note that the effective domain_res_1d is 64 due to importance_domain_size

set domain_res = ${domain_res_1d}x${domain_res_1d};

./src/Main/Release/MakeImportance ${dimension} ${domain_res_1d} ${importance_domain_size} ${importance_domain_offset} ${importance_range_offset} ${output_dir}/${domain_option}.f2m ${domain_option} ${cos_freq};

./src/Main/Release/F2M2PFM ${output_dir}/${domain_option}.f2m ${output_dir}/${domain_option}.pfm;

else

echo unknown domain option;
exit(1);

endif

./src/Main/Release/PFM2PPM ${output_dir}/${domain_option}.pfm ${output_dir}/${domain_option}.ppm 1 1 2

if(${sim_infinite_domain}) then

./src/Main/Release/TileF2M ${output_dir}/${domain_option}.f2m ${sim_infinite_tiling} ${output_dir}/${domain_option}_tiled.f2m;

endif

set methods = ("poisson" "white");

if(${rho_only}) then
set methods = ("poisson");
set num_runs = 1;
endif

foreach method ($methods) # poisson must preceed white to count sample

set head_name = ${domain_option}_${r_text};
if(${domain_option} == "cos") then
set head_name = ${domain_option}${frequency}_${r_text};
endif

set base_name = ${head_name}_${method};

# generate samples
set all_sample_files = "";
set all_tiled_sample_files = "";
set i = 0;
while(${i} < ${num_runs})

if(${generate_samples}) then

if(${method} == "white") then
./src/Main/Release/IsotropicWhiteNoise ${output_dir}/${domain_option}.f2m ${num_samples} ${domain_size} ${huge_dir}/${base_name}_${i}.hex
else if(${method} == "poisson") then
#${rainbow_dir}/src/Main/Release/${poisson_exe} ${dimension} 1 0 ${r_value} ${k_value} ${domain_size} ${conflict_metric} ${output_dir}/${domain_option}.pfm > ${huge_dir}/${base_name}_${i}.txt
./src/Main/Release/DartThrowing "isotropic" ${dimension} ${r_value} ${k_value} ${domain_size} ${boundary} ${sample_grid_resolution_per_unit_domain} ${output_dir}/${domain_option}.f2m ${importance_cell_size} ${huge_dir}/${base_name}_${i}.hex
else if(${method} == "poissondivwhite") then
# do nothing
else
echo unknown sampling method;
exit(1);
endif

sleep 1;
endif #if(${generate_samples})

if(${method} !=  "poissondivwhite") then
set all_sample_files = "${all_sample_files} ${huge_dir}/${base_name}_${i}.hex";

# num samples
set final_samples = `./src/Main/Release/SampleSelector ${huge_dir}/${base_name}_${i}.hex ${dimension} cout 0`;
@ num_samples = $#final_samples / ${dimension};
endif

# tiling
if(${sim_infinite_domain}) then

./src/Main/Release/TileSamples ${huge_dir}/${base_name}_${i}.hex ${dimension} 1 ${sim_infinite_tiling} ${domain_size} ${huge_dir}/${base_name}_tiled_${i}.hex;

set all_tiled_sample_files = "${all_tiled_sample_files} ${huge_dir}/${base_name}_tiled_${i}.hex";

endif

# draw samples
if(${i} == 0) then

echo "0 0 0" > ${output_dir}/palette.txt;

set dot_radius = 0.01;

./src/Main/Release/DrawSamples ${huge_dir}/${base_name}_${i}.hex ${huge_dir}/${base_name}_${i}.fig ${dimension} ${dot_radius} 1 ${domain_size} ${output_dir}/palette.txt
./src/Scripts/fig2eps -f ${huge_dir}/${base_name}_${i}.fig
epstopdf ${huge_dir}/${base_name}_${i}.eps --outfile ${huge_dir}/${base_name}_${i}.pdf
endif

@ i = ${i} + 1;
end

set sample_base_name = ${base_name};

# diff transform
if(${rho_only}) then
set kernel_options = ("gaussian");
set diff_norms = ("symmetry" "unit");
else
set kernel_options = ("gaussian" "sft");
set diff_norms = ("symmetry");
endif

foreach kernel_option (${kernel_options})
foreach diff_norm (${diff_norms})
foreach diff_weight ("unit")

if(${kernel_option} == "sft") then
set tail_name = sft;
else
set tail_name = diffxform_${kernel_option}_${local_diff_range}r_${max_relative_dd_text}dd_${diff_norm}_${diff_weight};
endif

set base_name = ${head_name}_${method}_${tail_name};

if(${kernel_option} == "sft") then

set all_pfm_files = "";
set i = 0;
while(${i} < ${num_runs})

# compute sft
./src/Main/Release/SampleSelector ${huge_dir}/${sample_base_name}_${i}.hex ${dimension} ${huge_dir}/${sample_base_name}_${i}_selected.hex 0

./src/Main/Release/SamplePFT ${huge_dir}/${sample_base_name}_${i}_selected.hex ${huge_dir}/${sample_base_name}_${i}.pfm ${dimension} 1 1 ${sft_size} ${sft_scale} ${normalization_option}

set all_pfm_files = "${all_pfm_files} ${huge_dir}/${sample_base_name}_${i}.pfm";

@ i = ${i} + 1;
end

echo ${all_pfm_files} > ${output_dir}/all_pfm_files_list.txt;

else

if(${sim_infinite_domain}) then
echo ${all_tiled_sample_files} > ${output_dir}/all_sample_files_list.txt;
else
echo ${all_sample_files} > ${output_dir}/all_sample_files_list.txt;
endif

if(${sim_infinite_domain}) then
set domain_option_file = ${output_dir}/${domain_option}_tiled.f2m 
else
set domain_option_file = ${output_dir}/${domain_option}.f2m 
endif

if(${rho_only}) then

time ./src/Main/Release/IsotropicDiffRho ${domain_option_file} ${importance_cell_size} ${isotropic_average_distance} ${max_relative_dd} ${local_diff_range} ${diff_norm} ${diff_weight} ${max_cache_size} ${output_dir}/all_sample_files_list.txt ${dimension} ${sub_domain_min} ${sub_domain_max} ${diff_domain_max};

else

time ./src/Main/Release/IsotropicDiffXform ${domain_option_file} ${importance_cell_size} ${isotropic_average_distance} ${max_relative_dd} ${local_diff_range} ${diff_norm} ${diff_weight} ${max_cache_size} ${output_dir}/all_sample_files_list.txt ${dimension} ${sub_domain_min} ${sub_domain_max} ${diff_domain_max} ${kernel_option} ${kernel_size} ${keep_dc} ${output_resolution} ${output_dir}/${base_name}_average${num_runs}.f2m;

rm ${output_dir}/all_sample_files_list.txt;

./src/Main/Release/F2M2PFM ${output_dir}/${base_name}_average${num_runs}.f2m ${output_dir}/${base_name}_average${num_runs}.pfm;

echo ${output_dir}/${base_name}_average${num_runs}.pfm > ${output_dir}/all_pfm_files_list.txt;

endif

endif #if(${kernel_option} == "sft")

if(${rho_only}) then
# do nothing
else

# radial computation
./src/Main/Release/RadialPlot ${output_dir}/all_pfm_files_list.txt ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt
rm ${output_dir}/all_pfm_files_list.txt;

./src/Main/Release/PFM2PPM ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}.ppm 1 1 0

if(${kernel_option} == "sft") then
./src/Main/Release/PFM3PPM ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}_cr${clamp_radius}.ppm 1 1 0 ${clamp_radius}
endif

echo ${output_dir}/${base_name}_average${num_runs}.ppm

# radial plot
if(${kernel_option} == "sft") then
set scaling_factor = 1;
set norm_factor = ${sft_hist_normalization_const};
else
set scaling_factor = ${diff_scale};
set norm_factor = ${diffxform_hist_normalization_const};
endif

./src/Scripts/txt2dat ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt ${norm_factor} ${output_dir}/${base_name}_average${num_runs}_mean.dat ${output_dir}/${base_name}_average${num_runs}_variance.dat ${scaling_factor}

if(${method} == "poisson") then
./src/Scripts/behead ${output_dir}/${base_name}_average${num_runs}_variance.dat ${output_dir}/${base_name}_average${num_runs}_variance.dat ${r_value}
endif

echo set terminal postscript eps >> foo.gp
echo set output \'${output_dir}/${base_name}_average${num_runs}_mean.eps\' >> foo.gp
echo set size 0.5, 0.3375 >> foo.gp
echo set title \"\" >> foo.gp
if(${kernel_option} == "sft") then
echo set xlabel \"frequency\" >> foo.gp
else
echo set xlabel \"\|d\|\" >> foo.gp
endif
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
if(${kernel_option} == "sft") then
set x_min = 1;
set x_max = ${sft_freq};
else
set x_min = 0;
set x_max = ${diff_max};
endif
echo plot "[${x_min}:${x_max}] [0:2]" \"${output_dir}/${base_name}_average${num_runs}_mean.dat\" with line linetype rgb \"red\" >> foo.gp

echo set terminal postscript eps >> foo.gp
echo set output \'${output_dir}/${base_name}_average${num_runs}_variance.eps\' >> foo.gp
echo set size 0.5, 0.3375 >> foo.gp
echo set title \"\" >> foo.gp
if(${kernel_option} == "sft") then
echo set xlabel \"frequency\" >> foo.gp
else
echo set xlabel \"\|d\|\" >> foo.gp
endif
echo set ylabel \"anisotropy\" >> foo.gp
echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
echo plot "[${x_min}:${x_max}] [-30:0]" \"${output_dir}/${base_name}_average${num_runs}_variance.dat\" with line linetype rgb \"blue\" >> foo.gp

wgnuplot foo.gp
rm foo.gp

foreach option (mean variance)
set final_name = ${base_name}_average${num_runs}_${option};
epstopdf ${output_dir}/${final_name}.eps --outfile ${output_dir}/${final_name}.pdf;
echo ${output_dir}/${final_name}.pdf;
end

endif #if(!${rho_only})

end # foreach diff_norm
end # foreach diff_weight

end # foreach kernel_option 
end # foreach method

# end # foreach frequency
end # foreach domain_option

endif

# anisotropic diff transform
if(0) then

set generate_samples = 1;

set rho_only = 0;

set dimension = 2;

set default_domain_size = "1 1";

# poisson
set r_value = 0.03;
set r_text = 0p03;
set k_value = 100.70;
set boundary = 1; # toroidal
set sample_grid_resolution_per_unit_domain = 50;

# white
#set num_samples = 400;
set white_radius = 0.01;
set default_warp_cell_size = 0.02;

set anisotropic_average_distance = ${r_value};
set max_relative_dd = -1; #0.05;
set max_relative_dd_text = inf; #0p05;
set local_diff_range = 12;
if(${rho_only}) then
set local_diff_range = 3;
endif
#set diff_norm = "symmetry";
#set diff_weight = "unit";
set max_cache_size = 4;

set default_sim_infinite_domain = 1; # via 3x3 tiling
set sim_infinite_tiling = "3 3";

if(${default_sim_infinite_domain}) then
set default_sub_domain_min = "1 1";
set default_sub_domain_max = "2 2";
else
set default_sub_domain_min = "0 0";
set default_sub_domain_max = "1 1";
endif

set diff_max = 0.15;
set diff_domain_max = "${diff_max} ${diff_max}";

#set kernel_option = "gaussian";
set kernel_size = 1;
set hist_normalization_const = 1; # related to kernel_size
#set keep_dc = 0;
set output_resolution = 640; # per unit domain
set diff_scale = 0.0015625; # 1/output_resolution

set default_num_runs = 10;

foreach domain_option ("scale" "shear" "perspective" "terrain" "portrait")

# default set up
set domain_size = "${default_domain_size}";
set sim_infinite_domain = "${default_sim_infinite_domain}";
set sub_domain_min = "${default_sub_domain_min}";
set sub_domain_max = "${default_sub_domain_max}";
set warp_cell_size = ${default_warp_cell_size};
set num_runs = ${default_num_runs};

if(${domain_option} == "identity") then
echo "F2 2 3 3   1 0 0   0 1 0   0 0 1" > ${output_dir}/${domain_option}.f2m;
else if(${domain_option} == "scale") then
echo "F2 2 3 3   0.5 0   0 0 1 0   0 0 1" > ${output_dir}/${domain_option}.f2m;
else if(${domain_option} == "shear") then
echo "F2 2 3 3   0.5 0.5 0   0 1 0   0 0 1" > ${output_dir}/${domain_option}.f2m;
else if(${domain_option} == "perspective") then
echo "F2 2 3 3   3 1 0   0 2 0   2 1 1" > ${output_dir}/${domain_option}.f2m;
else if(${domain_option} == "terrain") then
# do nothing
else if((${domain_option} == "portrait") || (${domain_option} == "ostro")) then
# do nothing
else
echo unknown domain option
exit(1);
endif

if(${domain_option} == "ostro") then

set sim_infinite_domain = 0;

set importance_domain_size = "1 1";
set importance_domain_offset = "0 0";
set importance_range_offset = 0.01;
set max_importance = 1.0;

set domain_res_1d = 250;
set importance_cell_size = 0.004; # 1.0/250

if(1) then
convert ${output_dir}/${domain_option}.png -negate -compress none -resize ${domain_res_1d}x${domain_res_1d} ${output_dir}/${domain_option}.pgm
endif

./src/Main/Release/PGM2F2M ${output_dir}/${domain_option}.pgm ${output_dir}/${domain_option}.f2m

./src/Main/Release/MakeImportance ${dimension} ${domain_res_1d} ${importance_domain_size} ${importance_domain_offset} ${importance_range_offset} ${output_dir}/${domain_option}.f2m ${output_dir}/${domain_option}.f2m;

./src/Main/Release/Importance2JacobianField ${output_dir}/${domain_option}.f2m ${max_importance} ${importance_cell_size} ${r_value} ${output_dir}/${domain_option}.f2m;

else if(${domain_option} == "portrait") then

set sim_infinite_domain = 0;

set importance_domain_size = "1 1";
set importance_domain_offset = "0 0";
set importance_range_offset = 0.01;
set max_importance = 1.0;

set domain_res_1d = 500;
set importance_cell_size = 0.002; # 1.0/500;

if(1) then
convert ${output_dir}/${domain_option}.png -negate -compress none -crop 500x500+8+0 -resize ${domain_res_1d}x${domain_res_1d} ${output_dir}/${domain_option}.pgm
endif

./src/Main/Release/PGM2F2M ${output_dir}/${domain_option}.pgm ${output_dir}/${domain_option}.f2m

./src/Main/Release/MakeImportance ${dimension} ${domain_res_1d} ${importance_domain_size} ${importance_domain_offset} ${importance_range_offset} ${output_dir}/${domain_option}.f2m ${output_dir}/${domain_option}.f2m;

./src/Main/Release/Importance2JacobianField ${output_dir}/${domain_option}.f2m ${max_importance} ${importance_cell_size} ${r_value} ${output_dir}/${domain_option}.f2m;

else if(${domain_option} == "terrain") then

./src/Main/Release/Warp2JacobianField ${domain_option} ${dimension} ${domain_size} ${warp_cell_size} ${output_dir}/${domain_option}.f2m

else

./src/Main/Release/Warp2JacobianField ${output_dir}/${domain_option}.f2m ${dimension} ${domain_size} ${warp_cell_size} ${output_dir}/${domain_option}.f2m

endif

if(${sim_infinite_domain}) then
./src/Main/Release/TileF2M ${output_dir}/${domain_option}.f2m ${sim_infinite_tiling} ${output_dir}/${domain_option}_tiled.f2m;
endif

if(${domain_option} == "ostro") then

set domain_size = "1 1";

if(${sim_infinite_domain}) then
set sub_domain_min = "1 1";
set sub_domain_max = "2 2";
else
set sub_domain_min = "0 0";
set sub_domain_max = "1 1";
endif

set importance_cell_size = 0.004; # 1.0/250;

set warp_cell_size = ${importance_cell_size};

set num_runs = 10;

else if(${domain_option} == "portrait") then

set domain_size = "8 8";

if(${sim_infinite_domain}) then
set sub_domain_min = "8 8";
set sub_domain_max = "16 16";
else
set sub_domain_min = "0 0";
set sub_domain_max = "8 8";
endif

set importance_cell_size = 0.016; # 8.0/500;

set warp_cell_size = ${importance_cell_size};

set num_runs = 1;

else if(${domain_option} == "terrain") then

set domain_size = "8 8";

if(${sim_infinite_domain}) then
set sub_domain_min = "8 8";
set sub_domain_max = "16 16";
else
set sub_domain_min = "0 0";
set sub_domain_max = "8 8";
endif

set warp_cell_size = 0.16;

set num_runs = 1;

else if(${domain_option} == "perspective") then

set domain_size = "4 4";

if(${sim_infinite_domain}) then
set sub_domain_min = "4 4";
set sub_domain_max = "8 8";
else
set sub_domain_min = "0 0";
set sub_domain_max = "4 4";
endif

set warp_cell_size = 0.08;

set num_runs = 4;

else

set domain_size = "2 2";

if(${sim_infinite_domain}) then
set sub_domain_min = "2 2";
set sub_domain_max = "4 4";
else
set sub_domain_min = "0 0";
set sub_domain_max = "2 2";
endif

set warp_cell_size = 0.04; # need to be consistent with default_warp_cell_size;

endif

if(${rho_only}) then
set num_runs = 1;
set methods = ("poisson");
else
set methods = ("poisson" "white");
endif

foreach method (${methods})# "poissondivwhite") # poisson must preceed white to count sample, and both must preceed poissondivwhite

if((${domain_option} == "ostro") || (${domain_option} == "portrait")) then
set head_name = aniso_${domain_option}_${r_text};
else
set head_name = ${domain_option}_${r_text};
endif

set base_name = ${head_name}_${method};

# generate samples
set all_sample_files = "";
set all_tiled_sample_files = "";
set i = 0;
while(${i} < ${num_runs})

if(${generate_samples}) then

if(${method} == "white") then

./src/Main/Release/AnisotropicWhiteNoise ${output_dir}/${domain_option}.f2m ${dimension} ${num_samples} ${domain_size} ${white_radius} ${warp_cell_size} ${huge_dir}/${base_name}_${i}.hex ;

else if(${method} == "poisson") then

./src/Main/Release/DartThrowing "anisotropic" ${dimension} ${r_value} ${k_value} ${domain_size} ${boundary} ${sample_grid_resolution_per_unit_domain} ${output_dir}/${domain_option}.f2m ${warp_cell_size} ${white_radius} ${huge_dir}/${base_name}_${i}.hex;

else

echo unknown sampling method;
exit(1);

endif

sleep 1;
endif #if(${generate_samples})

set all_sample_files = "${all_sample_files} ${huge_dir}/${base_name}_${i}.hex";

# num samples
set final_samples = `./src/Main/Release/SampleSelector ${huge_dir}/${base_name}_${i}.hex ${dimension} cout 0`;
@ num_samples = $#final_samples / ${dimension};

# tiling
if(${sim_infinite_domain}) then

./src/Main/Release/TileSamples ${huge_dir}/${base_name}_${i}.hex ${dimension} 1 ${sim_infinite_tiling} ${domain_size} ${huge_dir}/${base_name}_tiled_${i}.hex;

set all_tiled_sample_files = "${all_tiled_sample_files} ${huge_dir}/${base_name}_tiled_${i}.hex";

endif

# draw samples
if(${i} == 0) then

echo "0 0 0" > ${output_dir}/palette.txt;

set dot_radius = 0.01;

./src/Main/Release/DrawSamples ${huge_dir}/${base_name}_${i}.hex ${huge_dir}/${base_name}_${i}.fig ${dimension} ${dot_radius} 1 ${domain_size} ${output_dir}/palette.txt
./src/Scripts/fig2eps -f ${huge_dir}/${base_name}_${i}.fig
epstopdf ${huge_dir}/${base_name}_${i}.eps --outfile ${huge_dir}/${base_name}_${i}.pdf

endif

@ i = ${i} + 1;
end

# diff transform
if(${rho_only}) then
set diff_norms = ("symmetry" "unit");
else
set diff_norms = ("symmetry");
endif

foreach kernel_option ("gaussian") # "cos") # cos is too slow

if(${kernel_option} == "cos") then
set keep_dc = 1;
else
set keep_dc = 0;
endif

foreach diff_norm (${diff_norms})
foreach diff_weight ("unit")

set tail_name = diffxform_${kernel_option}_${local_diff_range}r_${max_relative_dd_text}dd_${diff_norm}_${diff_weight};
set base_name = ${head_name}_${method}_${tail_name};

if(${sim_infinite_domain}) then
echo ${all_tiled_sample_files} > ${output_dir}/all_sample_files_list.txt;
else
echo ${all_sample_files} > ${output_dir}/all_sample_files_list.txt;
endif

if(${sim_infinite_domain}) then
set domain_option_file = ${output_dir}/${domain_option}_tiled.f2m 
else
set domain_option_file = ${output_dir}/${domain_option}.f2m 
endif

if(${rho_only}) then
./src/Main/Release/AnisotropicDiffRho ${domain_option_file} ${warp_cell_size} ${anisotropic_average_distance} ${max_relative_dd} ${local_diff_range} ${diff_norm} ${diff_weight} ${max_cache_size} ${output_dir}/all_sample_files_list.txt ${dimension} ${sub_domain_min} ${sub_domain_max};
else
./src/Main/Release/AnisotropicDiffXform ${domain_option_file} ${warp_cell_size} ${anisotropic_average_distance} ${max_relative_dd} ${local_diff_range} ${diff_norm} ${diff_weight} ${max_cache_size} ${output_dir}/all_sample_files_list.txt ${dimension} ${sub_domain_min} ${sub_domain_max} ${diff_domain_max} ${kernel_option} ${kernel_size} ${keep_dc} ${output_resolution} ${output_dir}/${base_name}_average${num_runs}.f2m;

rm ${output_dir}/all_sample_files_list.txt;

./src/Main/Release/F2M2PFM ${output_dir}/${base_name}_average${num_runs}.f2m ${output_dir}/${base_name}_average${num_runs}.pfm;

./src/Main/Release/PFM2PPM ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}.ppm 1 1 0

echo ${output_dir}/${base_name}_average${num_runs}.ppm

# radial computation
echo ${output_dir}/${base_name}_average${num_runs}.pfm > ${output_dir}/all_pfm_files_list.txt;

./src/Main/Release/RadialPlot ${output_dir}/all_pfm_files_list.txt ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt
rm ${output_dir}/all_pfm_files_list.txt;

# radial plot
set hist_norm_const = ${hist_normalization_const};

./src/Scripts/txt2dat ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt ${hist_norm_const} ${output_dir}/${base_name}_average${num_runs}_mean.dat ${output_dir}/${base_name}_average${num_runs}_variance.dat ${diff_scale}

if(${method} == "poisson") then
./src/Scripts/behead ${output_dir}/${base_name}_average${num_runs}_variance.dat ${output_dir}/${base_name}_average${num_runs}_variance.dat ${r_value}
endif

echo set terminal postscript eps >> foo.gp
echo set output \'${output_dir}/${base_name}_average${num_runs}_mean.eps\' >> foo.gp
echo set size 0.5, 0.3375 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"\|d\|\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
echo plot "[0:${diff_max}] [0:2.2]" \"${output_dir}/${base_name}_average${num_runs}_mean.dat\" with line linetype rgb \"red\" >> foo.gp

echo set terminal postscript eps >> foo.gp
echo set output \'${output_dir}/${base_name}_average${num_runs}_variance.eps\' >> foo.gp
echo set size 0.5, 0.3375 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"\|d\|\" >> foo.gp
echo set ylabel \"anisotropy\" >> foo.gp
echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
echo plot "[0:${diff_max}] [-30:0]" \"${output_dir}/${base_name}_average${num_runs}_variance.dat\" with line linetype rgb \"blue\" >> foo.gp

wgnuplot foo.gp
rm foo.gp

foreach option (mean variance)
set final_name = ${base_name}_average${num_runs}_${option};
epstopdf ${output_dir}/${final_name}.eps --outfile ${output_dir}/${final_name}.pdf;
echo ${output_dir}/${final_name}.pdf;
end

endif # if(${rho_only}) then

end # foreach diff_weight
end # foreach diff_norm
end # foreach kernel_option

end # foreach method

end # foreach domain_option

endif

# different sampling rates for the terrain warp
if(0) then

set generate_samples = 1;

set dimension = 2;

set default_domain_size = "1 1";

# poisson
set r_value = 0.03;
set r_text = 0p03;
set k_value = 0.70;
set boundary = 1; # toroidal
set sample_grid_resolution_per_unit_domain = 50;

# white
#set num_samples = 400;
set white_radius = 0.01;
set default_warp_cell_size = 0.02;

set anisotropic_average_distance = ${r_value};
set max_relative_dd = -1; #0.05;
set max_relative_dd_text = inf; #0p05;
set local_diff_range = 12;
#set diff_norm = "symmetry";
#set diff_weight = "unit";
set max_cache_size = 4;

set sim_infinite_domain = 1; # via 3x3 tiling
set sim_infinite_tiling = "3 3";

if(${sim_infinite_domain}) then
set default_sub_domain_min = "1 1";
set default_sub_domain_max = "2 2";
else
set default_sub_domain_min = "0 0";
set default_sub_domain_max = "1 1";
endif

set diff_max = 0.15;
set diff_domain_max = "${diff_max} ${diff_max}";

#set kernel_option = "gaussian";
set kernel_size = 1;
set diffxform_hist_normalization_const = 1; # related to kernel_size
#set keep_dc = 0;
set output_resolution = 640; # per unit domain
set diff_scale = 0.0015625; # 1/output_resolution

# sft stuff
set sft_freq = 96; #output_resolution * diff_max
@ sft_size = ${sft_freq} * 2;
set sft_scale = ${sft_size};
set normalization_option = "sqrtsamplecount";
set sft_hist_normalization_const = 1;

set default_num_runs = 10;

foreach domain_option ("terrain")

set domain_size = "${default_domain_size}";
set sub_domain_min = "${default_sub_domain_min}";
set sub_domain_max = "${default_sub_domain_max}";
set warp_cell_size = ${default_warp_cell_size};

if(${domain_option} == "identity") then
echo "F2 2 3 3   1 0 0   0 1 0   0 0 1" > ${output_dir}/${domain_option}.f2m;
else if(${domain_option} == "scale") then
echo "F2 2 3 3   0.5 0   0 0 1 0   0 0 1" > ${output_dir}/${domain_option}.f2m;
else if(${domain_option} == "shear") then
echo "F2 2 3 3   0.5 0.5 0   0 1 0   0 0 1" > ${output_dir}/${domain_option}.f2m;
else if(${domain_option} == "perspective") then
echo "F2 2 3 3   3 1 0   0 2 0   2 1 1" > ${output_dir}/${domain_option}.f2m;
else if(${domain_option} == "terrain") then
# do nothing
else
echo unknown domain option
exit(1);
endif

if(${domain_option} == "terrain") then
./src/Main/Release/Warp2JacobianField ${domain_option} ${dimension} ${domain_size} ${warp_cell_size} ${output_dir}/${domain_option}.f2m
else
./src/Main/Release/Warp2JacobianField ${output_dir}/${domain_option}.f2m ${dimension} ${domain_size} ${warp_cell_size} ${output_dir}/${domain_option}.f2m
endif

if(${sim_infinite_domain}) then
./src/Main/Release/TileF2M ${output_dir}/${domain_option}.f2m ${sim_infinite_tiling} ${output_dir}/${domain_option}_tiled.f2m;
endif

foreach domain_size_1d (1 2) 
# 4)

@ domain_size_1d_2 = ${domain_size_1d} * 2;

set domain_size = "${domain_size_1d} ${domain_size_1d}";

if(${sim_infinite_domain}) then
set sub_domain_min = "${domain_size}";
set sub_domain_max = "${domain_size_1d_2} ${domain_size_1d_2}";
else
set sub_domain_min = "0 0";
set sub_domain_max = "${domain_size}";
endif

if(${domain_size_1d} == 1) then
set inv_domain_size_1d = 1;
set warp_cell_size = 0.02;
set num_runs = 64;
else if(${domain_size_1d} == 2) then
set inv_domain_size_1d = 0.5;
set warp_cell_size = 0.04;
set num_runs = 16;
else if(${domain_size_1d} == 4) then
set inv_domain_size_1d = 0.25;
set warp_cell_size = 0.08;
set num_runs = 4;
else if(${domain_size_1d} == 8) then
set inv_domain_size_1d = 0.125;
set warp_cell_size = 0.16;
set num_runs = 1;
else
echo unknown domain_size_1d
exit(1);
endif
#set num_runs = 10;

foreach method ("poisson") # "white") # poisson must preceed white to count sample, and both must preceed poissondivwhite

set head_name = ${domain_option}_${r_text}_${domain_size_1d}ds;

set base_name = ${head_name}_${method};

# generate samples
set all_sample_files = "";
set all_tiled_sample_files = "";
set i = 0;
while(${i} < ${num_runs})

if(${generate_samples}) then

if(${method} == "white") then

./src/Main/Release/AnisotropicWhiteNoise ${output_dir}/${domain_option}.f2m ${dimension} ${num_samples} ${domain_size} ${white_radius} ${warp_cell_size} > ${huge_dir}/${base_name}_${i}.txt ;

else if(${method} == "poisson") then

./src/Main/Release/DartThrowing "anisotropic" ${dimension} ${r_value} ${k_value} ${domain_size} ${boundary} ${sample_grid_resolution_per_unit_domain} ${output_dir}/${domain_option}.f2m ${warp_cell_size} ${white_radius} > ${huge_dir}/${base_name}_${i}.txt;

else

echo unknown sampling method;
exit(1);

endif

sleep 1;
endif #if(${generate_samples})

set all_sample_files = "${all_sample_files} ${huge_dir}/${base_name}_${i}.txt";

# num samples
set final_samples = `./src/Main/Release/SampleSelector ${huge_dir}/${base_name}_${i}.txt ${dimension} cout 0`;
@ num_samples = $#final_samples / ${dimension};

# tiling
if(${sim_infinite_domain}) then

./src/Main/Release/TileSamples ${huge_dir}/${base_name}_${i}.txt ${dimension} 1 ${sim_infinite_tiling} ${domain_size} > ${huge_dir}/${base_name}_tiled_${i}.txt;

set all_tiled_sample_files = "${all_tiled_sample_files} ${huge_dir}/${base_name}_tiled_${i}.txt";

endif

@ i = ${i} + 1;
end

set sample_base_name = ${base_name};
set sample_inverse_name = ${base_name}_inverse;

# diff transform
foreach kernel_option ("gaussian" "sft") # cos is too slow

if(${kernel_option} == "cos") then
set keep_dc = 1;
else
set keep_dc = 0;
endif

foreach diff_norm ("symmetry")
foreach diff_weight ("unit")

if(${kernel_option} == "sft") then
set tail_name = sft;
else
set tail_name = diffxform_${kernel_option}_${local_diff_range}r_${max_relative_dd_text}dd_${diff_norm}_${diff_weight};
endif

set base_name = ${head_name}_${method}_${tail_name};

if(${kernel_option} == "sft") then

set all_pfm_files = "";
set i = 0;
while(${i} < ${num_runs})

# inverse warp
./src/Main/Release/ScaleSamples ${huge_dir}/${sample_base_name}_${i}.txt ${dimension} ${inv_domain_size_1d} > ${huge_dir}/${sample_base_name}_scaled_${i}.txt

if(${domain_option} == "terrain") then
./src/Main/Release/WarpSamples ${domain_option} 1 ${dimension} ${huge_dir}/${sample_base_name}_scaled_${i}.txt > ${huge_dir}/${sample_inverse_name}_scaled_${i}.txt
else
./src/Main/Release/WarpSamples ${output_dir}/${domain_option}.f2m 1 ${dimension} ${huge_dir}/${sample_base_name}_scaled_${i}.txt > ${huge_dir}/${sample_inverse_name}_scaled_${i}.txt
endif

./src/Main/Release/ScaleSamples ${huge_dir}/${sample_inverse_name}_scaled_${i}.txt ${dimension} ${domain_size_1d} > ${huge_dir}/${sample_inverse_name}_${i}.txt

if(1) then
./src/Main/Release/ClipSamples ${huge_dir}/${sample_inverse_name}_${i}.txt ${dimension} ${domain_size} > ${huge_dir}/${sample_inverse_name}_clipped_${i}.txt
cp ${huge_dir}/${sample_inverse_name}_clipped_${i}.txt ${huge_dir}/${sample_inverse_name}_${i}.txt
endif

# draw samples
if(${i} == 0) then

echo "0 0 0" > ${output_dir}/palette.txt;

set dot_radius = 0.01;

if(${kernel_option} == "sft") then
set draw_name = ${sample_inverse_name};
else
set draw_name = ${sample_base_name};
endif

./src/Main/Release/DrawSamples ${huge_dir}/${draw_name}_${i}.txt ${huge_dir}/${draw_name}_${i}.fig ${dimension} ${dot_radius} 1 ${domain_size} ${output_dir}/palette.txt
./src/Scripts/fig2eps -f ${huge_dir}/${draw_name}_${i}.fig
epstopdf ${huge_dir}/${draw_name}_${i}.eps --outfile ${huge_dir}/${draw_name}_${i}.pdf

endif

# compute sft
./src/Main/Release/SampleSelector ${huge_dir}/${sample_inverse_name}_${i}.txt ${dimension} "cout" 0 > ${huge_dir}/${sample_inverse_name}_${i}_selected.txt

./src/Main/Release/SamplePFT ${huge_dir}/${sample_inverse_name}_${i}_selected.txt ${huge_dir}/${sample_inverse_name}_${i}.pfm ${dimension} 1 1 ${sft_size} ${sft_scale} ${normalization_option}

set all_pfm_files = "${all_pfm_files} ${huge_dir}/${sample_inverse_name}_${i}.pfm";

@ i = ${i} + 1;
end

echo ${all_pfm_files} > ${output_dir}/all_pfm_files_list.txt;

else # gaussian kernel

if(${sim_infinite_domain}) then
echo ${all_tiled_sample_files} > ${output_dir}/all_sample_files_list.txt;
else
echo ${all_sample_files} > ${output_dir}/all_sample_files_list.txt;
endif

if(${sim_infinite_domain}) then
set domain_option_file = ${output_dir}/${domain_option}_tiled.f2m 
else
set domain_option_file = ${output_dir}/${domain_option}.f2m 
endif

./src/Main/Release/AnisotropicDiffXform ${domain_option_file} ${warp_cell_size} ${anisotropic_average_distance} ${max_relative_dd} ${local_diff_range} ${diff_norm} ${diff_weight} ${max_cache_size} ${output_dir}/all_sample_files_list.txt ${dimension} ${sub_domain_min} ${sub_domain_max} ${diff_domain_max} ${kernel_option} ${kernel_size} ${keep_dc} ${output_resolution} ${output_dir}/${base_name}_average${num_runs}.f2m;

rm ${output_dir}/all_sample_files_list.txt;

./src/Main/Release/F2M2PFM ${output_dir}/${base_name}_average${num_runs}.f2m ${output_dir}/${base_name}_average${num_runs}.pfm;

echo ${output_dir}/${base_name}_average${num_runs}.pfm > ${output_dir}/all_pfm_files_list.txt;

endif #(${kernel_option} == "sft")

# radial computation

./src/Main/Release/RadialPlot ${output_dir}/all_pfm_files_list.txt ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt
rm ${output_dir}/all_pfm_files_list.txt;

./src/Main/Release/PFM2PPM ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}.ppm 1 1 0

echo ${output_dir}/${base_name}_average${num_runs}.ppm

# radial plot

if(${kernel_option} == "sft") then
set scaling_factor = 1;
set norm_factor = ${sft_hist_normalization_const};
else
set scaling_factor = ${diff_scale};
set norm_factor = ${diffxform_hist_normalization_const};
endif

./src/Scripts/txt2dat ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt ${norm_factor} ${output_dir}/${base_name}_average${num_runs}_mean.dat ${output_dir}/${base_name}_average${num_runs}_variance.dat ${scaling_factor}

if(${method} == "poisson") then
set behead_index_threshold = ${r_value};
else
set behead_index_threshold = -1;
endif

if(${kernel_option} == "gaussian") then
./src/Scripts/behead ${output_dir}/${base_name}_average${num_runs}_variance.dat ${output_dir}/${base_name}_average${num_runs}_variance.dat ${behead_index_threshold}
endif

echo set terminal postscript eps >> foo.gp
echo set output \'${output_dir}/${base_name}_average${num_runs}_mean.eps\' >> foo.gp
echo set size 0.5, 0.3375 >> foo.gp
echo set title \"\" >> foo.gp
if(${kernel_option} == "sft") then
echo set xlabel \"frequency\" >> foo.gp
else
echo set xlabel \"\|d\|\" >> foo.gp
endif
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
if(${kernel_option} == "sft") then
set x_min = 1;
set x_max = ${sft_freq};
else
set x_min = 0;
set x_max = ${diff_max};
endif
echo plot "[${x_min}:${x_max}] [0:3]" \"${output_dir}/${base_name}_average${num_runs}_mean.dat\" with line linetype rgb \"red\" >> foo.gp

echo set terminal postscript eps >> foo.gp
echo set output \'${output_dir}/${base_name}_average${num_runs}_variance.eps\' >> foo.gp
echo set size 0.5, 0.3375 >> foo.gp
echo set title \"\" >> foo.gp
if(${kernel_option} == "sft") then
echo set xlabel \"frequency\" >> foo.gp
else
echo set xlabel \"\|d\|\" >> foo.gp
endif
echo set ylabel \"anisotropy\" >> foo.gp
echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
echo plot "[${x_min}:${x_max}] [-30:0]" \"${output_dir}/${base_name}_average${num_runs}_variance.dat\" with line linetype rgb \"blue\" >> foo.gp

wgnuplot foo.gp
rm foo.gp

foreach option (mean variance)
set final_name = ${base_name}_average${num_runs}_${option};
epstopdf ${output_dir}/${final_name}.eps --outfile ${output_dir}/${final_name}.pdf;
echo ${output_dir}/${final_name}.pdf;
end

end # foreach diff_weight
end # foreach diff_norm
end # foreach kernel_option

end # foreach method

end # foreach domain_size_1d

end # foreach domain_option

endif

# two phase sampling, trying to identify worst case scenario
if(0) then

set dimension = 2;
set domain_size = "1 1";

# grid
# 729 samples
# set sample_rate = "27 27";

# poisson
set r_value = 0.03;
set r_text = 0p03;
set k_value = 0.75;
set boundary = 1; # toroidal
set sample_grid_resolution_per_unit_domain = 20;

# xform
set domain_option = "uniform";

set isotropic_average_distance = ${r_value};
set max_relative_dd = -1; #0.05;
set max_relative_dd_text = inf; #0p05;
set local_diff_range = 10;
set diff_norm = "unit";
set diff_weight = "unit";
set max_cache_size = 1;

set sim_infinite_domain = 1; # via 3x3 tiling

if(${sim_infinite_domain}) then
set sub_domain_min = "1 1";
set sub_domain_max = "2 2";
else
set sub_domain_min = "0 0";
set sub_domain_max = "1 1";
endif

set diff_max = 0.2;
set diff_domain_max = "${diff_max} ${diff_max}";

set kernel_size = 1;
set diffxform_hist_normalization_const = 1; # related to kernel_size
#set keep_dc = 0;
set output_resolution = 640; # per unit domain
set diff_scale = 0.0015625; # 1/output_resolution

# sft stuff
set sft_freq = 128; #output_resolution * diff_max
@ sft_size = ${sft_freq} * 2;
set sft_scale = ${sft_size};
set normalization_option = "sqrtsamplecount";
set sft_hist_normalization_const = 1;

set num_runs = 10;

foreach init ("hex" "grid0")

foreach sample_rate_1d (1 2 3 4 5 6 7 8)

set init_name = ${domain_option}_${init}_${sample_rate_1d};

foreach method ("poisson")

set base_name = ${init_name}_${method}_${r_text};

# generate samples
set all_sample_files = "";
set all_tiled_sample_files = "";

set i = 0;
while(${i} < ${num_runs})

if((${init} == "grid0") || (${init} == "grid1")) then

if(${method} == "grid1") then
set jitter_scale = 1;
else
set jitter_scale = 0;
endif

set sample_rate = "${sample_rate_1d} ${sample_rate_1d}";

./src/Main/Release/RegularGrid ${dimension} ${domain_size} ${sample_rate} ${jitter_scale} > ${huge_dir}/${init_name}_${i}.txt

else if(${init} == "hex") then

./src/Main/Release/HexagonalGrid ${dimension} ${domain_size} ${sample_rate_1d} > ${huge_dir}/${init_name}_${i}.txt

else

echo unknown init ${init}
exit(1);

endif

# increment class id for init sample set
if(0) then
echo warning plot mode can ruin spectrum analysis!

${rainbow_dir}/src/Main/Release/ProcessLloydSites ${huge_dir}/${init_name}_${i}.txt ${huge_dir}/${init_name}_${i}.txt ${dimension} "increment"
endif

if(${method} == "poisson") then

./src/Main/Release/DartThrowing "uniform" ${dimension} ${r_value} ${k_value} ${domain_size} ${boundary} ${sample_grid_resolution_per_unit_domain} ${huge_dir}/${base_name}_${i}.txt ${huge_dir}/${init_name}_${i}.txt
else

echo unknown method ${method}
exit(1);

endif

set all_sample_files = "${all_sample_files} ${huge_dir}/${base_name}_${i}.txt";

# num samples
set final_samples = `./src/Main/Release/SampleSelector ${huge_dir}/${base_name}_${i}.txt ${dimension} cout 0`;
@ num_samples = $#final_samples / ${dimension};

# tiling
if(${sim_infinite_domain}) then

./src/Main/Release/TileSamples ${huge_dir}/${base_name}_${i}.txt ${dimension} 1 3 3 ${domain_size} > ${huge_dir}/${base_name}_tiled_${i}.txt;

set all_tiled_sample_files = "${all_tiled_sample_files} ${huge_dir}/${base_name}_tiled_${i}.txt";

endif

# draw samples
if(${i} == 0) then

echo "0 0 0 255 0 0" > ${output_dir}/palette.txt;

set dot_radius = 0.01;

./src/Main/Release/DrawSamples ${huge_dir}/${base_name}_${i}.txt ${huge_dir}/${base_name}_${i}.fig ${dimension} ${dot_radius} 1 ${domain_size} ${output_dir}/palette.txt
./src/Scripts/fig2eps -f ${huge_dir}/${base_name}_${i}.fig
epstopdf ${huge_dir}/${base_name}_${i}.eps --outfile ${huge_dir}/${base_name}_${i}.pdf
endif

@ i = ${i} + 1;
end # while

set sample_base_name = ${base_name};

# diff transform
foreach kernel_option ("gaussian" "sft") # "cos")

if(${kernel_option} == "gaussian") then
set keep_dc = 0;
set clamp_radius = 0;
else
set keep_dc = 1;
set clamp_radius = 1;
endif

if(${kernel_option} == "sft") then
set tail_name = sft;
else
set tail_name = diffxform_${kernel_option}_${local_diff_range}r_${max_relative_dd_text}dd_${diff_norm}_${diff_weight};
endif

set base_name = ${sample_base_name}_${tail_name};

if(${kernel_option} == "sft") then

set all_pfm_files = "";
set i = 0;
while(${i} < ${num_runs})

# compute sft
./src/Main/Release/SampleSelector ${huge_dir}/${sample_base_name}_${i}.txt ${dimension} "cout" 0 > ${huge_dir}/${sample_base_name}_${i}_selected.txt

./src/Main/Release/SamplePFT ${huge_dir}/${sample_base_name}_${i}_selected.txt ${huge_dir}/${sample_base_name}_${i}.pfm ${dimension} 1 1 ${sft_size} ${sft_scale} ${normalization_option}

set all_pfm_files = "${all_pfm_files} ${huge_dir}/${sample_base_name}_${i}.pfm";

@ i = ${i} + 1;
end

echo ${all_pfm_files} > ${output_dir}/all_pfm_files_list.txt;

else

if(${sim_infinite_domain}) then
echo ${all_tiled_sample_files} > ${output_dir}/all_sample_files_list.txt;
else
echo ${all_sample_files} > ${output_dir}/all_sample_files_list.txt;
endif

./src/Main/Release/UniformDiffXform ${output_dir}/all_sample_files_list.txt ${dimension} ${sub_domain_min} ${sub_domain_max} ${diff_domain_max} ${kernel_option} ${kernel_size} ${keep_dc} ${output_resolution} ${output_dir}/${base_name}_average${num_runs}.f2m;

rm ${output_dir}/all_sample_files_list.txt;

./src/Main/Release/F2M2PFM ${output_dir}/${base_name}_average${num_runs}.f2m ${output_dir}/${base_name}_average${num_runs}.pfm;

echo ${output_dir}/${base_name}_average${num_runs}.pfm > ${output_dir}/all_pfm_files_list.txt;

endif

# radial computation
./src/Main/Release/RadialPlot ${output_dir}/all_pfm_files_list.txt ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt
rm ${output_dir}/all_pfm_files_list.txt;

./src/Main/Release/PFM2PPM ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}.ppm 1 1 0

echo ${output_dir}/${base_name}_average${num_runs}.ppm

# radial plot

if(${kernel_option} == "sft") then
set scaling_factor = 1;
set norm_factor = ${sft_hist_normalization_const};
else
set scaling_factor = ${diff_scale};
set norm_factor = ${diffxform_hist_normalization_const};
endif

./src/Scripts/txt2dat ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt ${norm_factor} ${output_dir}/${base_name}_average${num_runs}_mean.dat ${output_dir}/${base_name}_average${num_runs}_variance.dat ${scaling_factor}

if((${method} == "poisson") || (${method} == "lloyd") || (${method} == "ppentomino") || (${method} == "balzer")) then
set behead_index_threshold = ${r_value};
else if(${method} == "grid1") then
set behead_index_threshold = 0.01;
else
set behead_index_threshold = -1;
endif

if(${kernel_option} == "gaussian") then
./src/Scripts/behead ${output_dir}/${base_name}_average${num_runs}_variance.dat ${output_dir}/${base_name}_average${num_runs}_variance.dat ${behead_index_threshold}
endif

echo set terminal postscript eps >> foo.gp
echo set output \'${output_dir}/${base_name}_average${num_runs}_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
if(${kernel_option} == "sft") then
echo set xlabel \"frequency\" >> foo.gp
else
echo set xlabel \"\|d\|\" >> foo.gp
endif
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
if(${kernel_option} == "sft") then
set x_min = 1;
set x_max = ${sft_freq};
else
set x_min = 0;
set x_max = ${diff_max};
endif
echo plot "[${x_min}:${x_max}] [0:3]" \"${output_dir}/${base_name}_average${num_runs}_mean.dat\" with line linetype rgb \"red\" >> foo.gp

echo set terminal postscript eps >> foo.gp
echo set output \'${output_dir}/${base_name}_average${num_runs}_variance.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
if(${kernel_option} == "sft") then
echo set xlabel \"frequency\" >> foo.gp
else
echo set xlabel \"\|d\|\" >> foo.gp
endif
echo set ylabel \"anisotropy\" >> foo.gp
echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
echo plot "[${x_min}:${x_max}] [-30:30]" \"${output_dir}/${base_name}_average${num_runs}_variance.dat\" with line linetype rgb \"blue\" >> foo.gp

wgnuplot foo.gp
rm foo.gp

foreach option (mean variance)
set final_name = ${base_name}_average${num_runs}_${option};
epstopdf ${output_dir}/${final_name}.eps --outfile ${output_dir}/${final_name}.pdf;
echo ${output_dir}/${final_name}.pdf;
end

end # foreach kernel_option 

end # foreach method

end # foreach sample_rate_1d

end #foreach init

endif

# haar warp
if(0) then

set dimension = 2;
set domain_size = "1 1";

# poisson
set r_value = 0.03;
set r_text = 0p03;
set k_value = 0.75;
set boundary = 1; # toroidal
set sample_grid_resolution_per_unit_domain = 20;

# xform
set domain_option = "uniform";

set isotropic_average_distance = ${r_value};
set max_relative_dd = -1; #0.05;
set max_relative_dd_text = inf; #0p05;
set local_diff_range = 10;
set diff_norm = "symmetry";
set diff_weight = "unit";
set max_cache_size = 4;

set sim_infinite_domain = 1; # via 3x3 tiling
set sim_infinite_tiling = "3 3";

if(${sim_infinite_domain}) then
set sub_domain_min = "1 1";
set sub_domain_max = "2 2";
else
set sub_domain_min = "0 0";
set sub_domain_max = "1 1";
endif

set diff_max = 0.2;
set diff_domain_max = "${diff_max} ${diff_max}";

set kernel_size = 1;
set diffxform_hist_normalization_const = 1; # related to kernel_size
#set keep_dc = 0;
set output_resolution = 640; # per unit domain
set diff_scale = 0.0015625; # 1/output_resolution

set dot_radius = 0.01;

set num_runs = 10;

set poisson_base_name = "poisson_${r_text}";
set importance_name = "haar_importance";

# haar importance
echo "F2 2 1 1 1" > ${output_dir}/${importance_name}.f2m;
set cell_size = 1;

echo "F2 2 2 2 0.6 0.2 0.05 0.15" > ${output_dir}/${importance_name}.f2m;
#echo "F2 2 2 2 1 1 1 1" > ${output_dir}/${importance_name}.f2m;
set cell_size = 0.5;

if(${sim_infinite_domain}) then
./src/Main/Release/TileF2M ${output_dir}/${importance_name}.f2m ${sim_infinite_tiling} ${output_dir}/${importance_name}_tiled.f2m;
endif

foreach method ("haarwarp" "haarpoisson")

set output_base_name = "${method}_${poisson_base_name}";

# generate samples
set all_sample_files = "";
set all_tiled_sample_files = "";

set i = 0;
while(${i} < ${num_runs})

if(${method} == "haarwarp") then

# poisson disk
./src/Main/Release/DartThrowing "uniform" ${dimension} ${r_value} ${k_value} ${domain_size} ${boundary} ${sample_grid_resolution_per_unit_domain} ${huge_dir}/${poisson_base_name}_${i}.txt

# haar warp
./src/Main/Release/WarpSamples "haar" 0 ${dimension} ${huge_dir}/${poisson_base_name}_${i}.txt ${output_dir}/${importance_name}.f2m ${cell_size} > ${huge_dir}/${output_base_name}_${i}.txt

else if(${method} == "haarpoisson") then

./src/Main/Release/DartThrowing "isotropic" ${dimension} ${r_value} ${k_value} ${domain_size} ${boundary} ${sample_grid_resolution_per_unit_domain} ${output_dir}/${importance_name}.f2m ${cell_size} ${huge_dir}/${output_base_name}_${i}.txt

else
echo unknown method
exit(1);
endif

set all_sample_files = "${all_sample_files} ${huge_dir}/${output_base_name}_${i}.txt";

# num samples
set final_samples = `./src/Main/Release/SampleSelector ${huge_dir}/${output_base_name}_${i}.txt ${dimension} cout 0`;
@ num_samples = $#final_samples / ${dimension};

# tiling
if(${sim_infinite_domain}) then
./src/Main/Release/TileSamples ${huge_dir}/${output_base_name}_${i}.txt ${dimension} 1 3 3 ${domain_size} > ${huge_dir}/${output_base_name}_tiled_${i}.txt;

set all_tiled_sample_files = "${all_tiled_sample_files} ${huge_dir}/${output_base_name}_tiled_${i}.txt";
endif

# plot
if(${i} == 0) then
echo "0 0 0" > ${output_dir}/palette.txt;

foreach draw_name (${poisson_base_name} ${output_base_name})

./src/Main/Release/DrawSamples ${huge_dir}/${draw_name}_${i}.txt ${huge_dir}/${draw_name}_${i}.fig ${dimension} ${dot_radius} 1 ${domain_size} ${output_dir}/palette.txt
./src/Scripts/fig2eps -f ${huge_dir}/${draw_name}_${i}.fig
epstopdf ${huge_dir}/${draw_name}_${i}.eps --outfile ${huge_dir}/${draw_name}_${i}.pdf
end
endif

@ i = ${i} + 1;
end

set sample_base_name = ${output_base_name};

# diff transform

foreach kernel_option ("gaussian") # "cos")

if(${kernel_option} == "gaussian") then
set keep_dc = 0;
set clamp_radius = 0;
else
set keep_dc = 1;
set clamp_radius = 1;
endif

if(${kernel_option} == "sft") then
set tail_name = sft;
else
set tail_name = diffxform_${kernel_option}_${local_diff_range}r_${max_relative_dd_text}dd_${diff_norm}_${diff_weight};
endif

set base_name = ${sample_base_name}_${tail_name};

if(${kernel_option} == "sft") then

set all_pfm_files = "";
set i = 0;
while(${i} < ${num_runs})

# compute sft
./src/Main/Release/SampleSelector ${huge_dir}/${sample_base_name}_${i}.txt ${dimension} "cout" 0 > ${huge_dir}/${sample_base_name}_${i}_selected.txt

./src/Main/Release/SamplePFT ${huge_dir}/${sample_base_name}_${i}_selected.txt ${huge_dir}/${sample_base_name}_${i}.pfm ${dimension} 1 1 ${sft_size} ${sft_scale} ${normalization_option}

set all_pfm_files = "${all_pfm_files} ${huge_dir}/${sample_base_name}_${i}.pfm";

@ i = ${i} + 1;
end

echo ${all_pfm_files} > ${output_dir}/all_pfm_files_list.txt;

else

if(${sim_infinite_domain}) then
echo ${all_tiled_sample_files} > ${output_dir}/all_sample_files_list.txt;
else
echo ${all_sample_files} > ${output_dir}/all_sample_files_list.txt;
endif
if(${sim_infinite_domain}) then
set importance_file = ${output_dir}/${importance_name}_tiled.f2m 
else
set importance_file = ${output_dir}/${importance_name}.f2m 
endif

./src/Main/Release/IsotropicDiffXform ${importance_file} ${cell_size} ${isotropic_average_distance} ${max_relative_dd} ${local_diff_range} ${diff_norm} ${diff_weight} ${max_cache_size} ${output_dir}/all_sample_files_list.txt ${dimension} ${sub_domain_min} ${sub_domain_max} ${diff_domain_max} ${kernel_option} ${kernel_size} ${keep_dc} ${output_resolution} ${output_dir}/${base_name}_average${num_runs}.f2m;

rm ${output_dir}/all_sample_files_list.txt;

./src/Main/Release/F2M2PFM ${output_dir}/${base_name}_average${num_runs}.f2m ${output_dir}/${base_name}_average${num_runs}.pfm;

echo ${output_dir}/${base_name}_average${num_runs}.pfm > ${output_dir}/all_pfm_files_list.txt;

endif

# radial computation
./src/Main/Release/RadialPlot ${output_dir}/all_pfm_files_list.txt ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt
rm ${output_dir}/all_pfm_files_list.txt;

./src/Main/Release/PFM2PPM ${output_dir}/${base_name}_average${num_runs}.pfm ${output_dir}/${base_name}_average${num_runs}.ppm 1 1 0

echo ${output_dir}/${base_name}_average${num_runs}.ppm

# radial plot

if(${kernel_option} == "sft") then
set scaling_factor = 1;
set norm_factor = ${sft_hist_normalization_const};
else
set scaling_factor = ${diff_scale};
set norm_factor = ${diffxform_hist_normalization_const};
endif

./src/Scripts/txt2dat ${output_dir}/${base_name}_average${num_runs}_mean.txt ${output_dir}/${base_name}_average${num_runs}_variance.txt ${norm_factor} ${output_dir}/${base_name}_average${num_runs}_mean.dat ${output_dir}/${base_name}_average${num_runs}_variance.dat ${scaling_factor}

set behead_index_threshold = ${r_value};

if(${kernel_option} == "gaussian") then
./src/Scripts/behead ${output_dir}/${base_name}_average${num_runs}_variance.dat ${output_dir}/${base_name}_average${num_runs}_variance.dat ${behead_index_threshold}
endif

echo set terminal postscript eps >> foo.gp
echo set output \'${output_dir}/${base_name}_average${num_runs}_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
if(${kernel_option} == "sft") then
echo set xlabel \"frequency\" >> foo.gp
else
echo set xlabel \"\|d\|\" >> foo.gp
endif
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
if(${kernel_option} == "sft") then
set x_min = 1;
set x_max = ${sft_freq};
else
set x_min = 0;
set x_max = ${diff_max};
endif
echo plot "[${x_min}:${x_max}] [0:3]" \"${output_dir}/${base_name}_average${num_runs}_mean.dat\" with line linetype rgb \"red\" >> foo.gp

echo set terminal postscript eps >> foo.gp
echo set output \'${output_dir}/${base_name}_average${num_runs}_variance.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
if(${kernel_option} == "sft") then
echo set xlabel \"frequency\" >> foo.gp
else
echo set xlabel \"\|d\|\" >> foo.gp
endif
echo set ylabel \"anisotropy\" >> foo.gp
echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
echo plot "[${x_min}:${x_max}] [-30:30]" \"${output_dir}/${base_name}_average${num_runs}_variance.dat\" with line linetype rgb \"blue\" >> foo.gp

wgnuplot foo.gp
rm foo.gp

foreach option (mean variance)
set final_name = ${base_name}_average${num_runs}_${option};
epstopdf ${output_dir}/${final_name}.eps --outfile ${output_dir}/${final_name}.pdf;
echo ${output_dir}/${final_name}.pdf;
end

end #foreach kernel_option

end #foreach method 

endif
