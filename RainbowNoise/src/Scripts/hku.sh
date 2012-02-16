#!/bin/tcsh -f

set output_dir = data;
set huge_dir = data/huge;

# relationship with spectrum and number of classes
if(0) then

set has_c1 = 0;

set dimension = 2;

set ids = 0;
set r_value = 0.02;
set r_text = 0p02;

set target_samples = 1300;
set k_value = 100.67;
set domain_size = "1 1";

set sft_size = 512;

foreach num_classes (2) #(1 2 4 8 9 13 16 17 32 64 128 256)
#foreach num_classes_sqrt (3) #(2 4 8 16)

if(1) then
if(${num_classes} <= 2) then
set num_classes_sqrt = 1;
else if(${num_classes} <= 6) then
set num_classes_sqrt = 2;
else if(${num_classes} <= 12) then
set num_classes_sqrt = 3;
else if(${num_classes} <= 20) then
set num_classes_sqrt = 4;
else
echo unhandled num_classes;
exit(1);
endif
else
set num_classes = 0;
@ num_classes = ${num_classes_sqrt} * ${num_classes_sqrt};
endif

set sft_scale_all = ${sft_size};
@ sft_scale_all = ${sft_size} * ${num_classes_sqrt};

if(${num_classes} == 2) then
set c1_r_value = 0.02;
set c1_r_text = 0p02;
else if(${num_classes} == 4) then
set c1_r_value = 0.01;
set c1_r_text = 0p01;
else if(${num_classes} == 8) then
set c1_r_value = 0.00707106781;
set c1_r_text = 0p00707;
else if(${num_classes} == 9) then
set c1_r_value = 0.00666666667;
set c1_r_text = 0p0067;
else if(${num_classes} == 13) then
set c1_r_value = 0.00554700196;
set c1_r_text = 0p0055;
else if(${num_classes} == 16) then
set c1_r_value = 0.005;
set c1_r_text = 0p005;
else if(${num_classes} == 17) then
set c1_r_value = 0.0048507125;
set c1_r_text = 0p0049;
else if(${num_classes} == 64) then
set c1_r_value = 0.0025;
set c1_r_text = 0p0025;
else if(${num_classes} == 256) then
set c1_r_value = 0.00125;
set c1_r_text = 0p00125;
else
echo unhandled num_classes;
exit(1);
endif

if(${target_samples} > 0) then
@ k_value = ${target_samples} * ${num_classes} * -1;
endif

set priority = "";
set r_values = "";
set foo = 0;

while($foo < $num_classes)
set priority = "$priority 0";
set r_values = "$r_values $r_value";
@ foo = ${foo} + 1;
end

set all_ids = "";
set i = 0;
while(${i} < ${num_classes})
set all_ids = "${all_ids} ${i}";
@ i = ${i} + 1;
end

@ total_samples = ${target_samples} * ${num_classes};

set all_pfm_files_selected = "";
set all_pfm_files_all = "";
set all_pfm_files_c1 = "";
set all_pfm_files_count = 0;

foreach i (0 1 2 3 4 5 6 7 8 9)

echo ${num_classes} class dart throwing

src/Main/Release/DartThrowing $dimension $num_classes $priority $r_values $k_value $domain_size > ${output_dir}/poisson_${dimension}d_${num_classes}c_${r_text}_${i}.txt

src/Main/Release/SampleSelector ${output_dir}/poisson_${dimension}d_${num_classes}c_${r_text}_${i}.txt ${dimension} ${ids} > ${output_dir}/poisson_${dimension}d_${num_classes}c_${r_text}_${i}_selected.txt

#src/Main/Release/LloydRelaxation2D ${output_dir}/poisson_${dimension}d_${num_classes}c_${r_text}_${i}.txt ${dimension} ${domain_size} ${num_relaxations} > ${output_dir}/poisson_${dimension}d_${num_classes}c_${r_text}_${num_relaxations}e_${i}.txt

../BlueNoise/src/Main/Release/SFT ${output_dir}/poisson_${dimension}d_${num_classes}c_${r_text}_${i}_selected.txt ${output_dir}/poisson_${dimension}d_${num_classes}c_${r_text}_${i}_selected.pfm ${dimension} 1 1 ${sft_size} ${sft_size}

src/Main/Release/SampleSelector ${output_dir}/poisson_${dimension}d_${num_classes}c_${r_text}_${i}.txt ${dimension} ${all_ids} > ${output_dir}/poisson_${dimension}d_${num_classes}c_${r_text}_${i}_all.txt

../BlueNoise/src/Main/Release/SFT ${output_dir}/poisson_${dimension}d_${num_classes}c_${r_text}_${i}_all.txt ${output_dir}/poisson_${dimension}d_${num_classes}c_${r_text}_${i}_all.pfm ${dimension} 1 1 ${sft_size} ${sft_scale_all}

if(${has_c1}) then
echo 1 class dart throwing

src/Main/Release/DartThrowing $dimension 1 0 $c1_r_value $k_value $domain_size > ${output_dir}/poisson_${dimension}d_1c_${c1_r_text}_${i}.txt

src/Main/Release/SampleSelector ${output_dir}/poisson_${dimension}d_1c_${c1_r_text}_${i}.txt ${dimension} 0 > ${output_dir}/poisson_${dimension}d_1c_${c1_r_text}_${i}_selected.txt

../BlueNoise/src/Main/Release/SFT ${output_dir}/poisson_${dimension}d_1c_${c1_r_text}_${i}_selected.txt ${output_dir}/poisson_${dimension}d_1c_${c1_r_text}_${i}_selected.pfm ${dimension} 1 1 ${sft_size} ${sft_scale_all}
endif

set all_pfm_files_selected = "$all_pfm_files_selected ${output_dir}/poisson_${dimension}d_${num_classes}c_${r_text}_${i}_selected.pfm";
set all_pfm_files_all = "$all_pfm_files_all ${output_dir}/poisson_${dimension}d_${num_classes}c_${r_text}_${i}_all.pfm";
if(${has_c1}) then
set all_pfm_files_c1 = "$all_pfm_files_c1 ${output_dir}/poisson_${dimension}d_1c_${c1_r_text}_${i}_selected.pfm";
endif
@ all_pfm_files_count = ${all_pfm_files_count} + 1;
end # foreach i

echo ${all_pfm_files_selected} > ${output_dir}/all_pfm_files_selected_list.txt;
src/Sain/Release/RadialPlot ${output_dir}/all_pfm_files_selected_list.txt ${output_dir}/poisson_${dimension}d_${num_classes}c_${r_text}_selected_average${all_pfm_files_count}.pfm ${output_dir}/poisson_${dimension}d_${num_classes}c_${r_text}_selected_average${all_pfm_files_count}_mean.txt ${output_dir}/poisson_${dimension}d_${num_classes}c_${r_text}_selected_average${all_pfm_files_count}_variance.txt
rm ${output_dir}/all_pfm_files_selected_list.txt;

../BlueNoise/src/Main/Release/PFM2PPM ${output_dir}/poisson_${dimension}d_${num_classes}c_${r_text}_selected_average${all_pfm_files_count}.pfm  ${output_dir}/poisson_${dimension}d_${num_classes}c_${r_text}_selected_average${all_pfm_files_count}.ppm 1 1 1

echo ${all_pfm_files_all} > ${output_dir}/all_pfm_files_all_list.txt;
src/Sain/Release/RadialPlot ${output_dir}/all_pfm_files_all_list.txt ${output_dir}/poisson_${dimension}d_${num_classes}c_${r_text}_all_average${all_pfm_files_count}.pfm ${output_dir}/poisson_${dimension}d_${num_classes}c_${r_text}_all_average${all_pfm_files_count}_mean.txt ${output_dir}/poisson_${dimension}d_${num_classes}c_${r_text}_all_average${all_pfm_files_count}_variance.txt
rm ${output_dir}/all_pfm_files_all_list.txt;

../BlueNoise/src/Main/Release/PFM2PPM ${output_dir}/poisson_${dimension}d_${num_classes}c_${r_text}_all_average${all_pfm_files_count}.pfm  ${output_dir}/poisson_${dimension}d_${num_classes}c_${r_text}_all_average${all_pfm_files_count}.ppm 1 1 1

# plot stuff
foreach choice (selected all)

if(${choice} == selected) then
set norm_sample_count = ${target_samples};
else
set norm_sample_count = ${total_samples};
endif

./src/Scripts/txt2dat ${output_dir}/poisson_${dimension}d_${num_classes}c_${r_text}_${choice}_average${all_pfm_files_count}_mean.txt ${output_dir}/poisson_${dimension}d_${num_classes}c_${r_text}_${choice}_average${all_pfm_files_count}_variance.txt ${norm_sample_count} ${output_dir}/poisson_${dimension}d_${num_classes}c_${r_text}_${choice}_average${all_pfm_files_count}_mean.dat ${output_dir}/poisson_${dimension}d_${num_classes}c_${r_text}_${choice}_average${all_pfm_files_count}_variance.dat 1

echo "" > foo.gp
echo set terminal postscript eps >> foo.gp
echo set output \'${output_dir}/poisson_${dimension}d_${num_classes}c_${r_text}_${choice}_average${all_pfm_files_count}_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp

if(${num_classes} == 1) then
echo plot "[1:256] [0:1.6]" \"${output_dir}/poisson_${dimension}d_${num_classes}c_${r_text}_${choice}_average${all_pfm_files_count}_mean.dat\" with line linetype rgb \"\#009900\" >> foo.gp
else
echo plot "[1:256] [0:1.6]" \"${output_dir}/poisson_${dimension}d_${num_classes}c_${r_text}_${choice}_average${all_pfm_files_count}_mean.dat\" with line linetype rgb \"red\", \"data/poisson_2d_1c_0p02_mean.dat\" with line linetype rgb \"\#009900\" >> foo.gp
endif

echo "" >> foo.gp

echo set terminal postscript eps >> foo.gp
echo set output \'${output_dir}/poisson_${dimension}d_${num_classes}c_${r_text}_${choice}_average${all_pfm_files_count}_variance.eps\' >> foo.gp
echo set size 0.5, 0.3375 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"anisotrophy\" >> foo.gp
echo set key off >> foo.gp
echo plot "[1:256] [-20:0]" \"${output_dir}/poisson_${dimension}d_${num_classes}c_${r_text}_${choice}_average${all_pfm_files_count}_variance.dat\" with line linetype rgb \"blue\" >> foo.gp

echo "" >> foo.gp

wgnuplot foo.gp

rm foo.gp

foreach option (mean variance)
set final_name = poisson_${dimension}d_${num_classes}c_${r_text}_${choice}_average${all_pfm_files_count}_${option};
epstopdf ${output_dir}/${final_name}.eps --outfile ${output_dir}/${final_name}.pdf;
echo ${output_dir}/${final_name}.pdf;
end

end

if(${has_c1}) then
echo ${all_pfm_files_c1} > ${output_dir}/all_pfm_files_c1_list.txt;
src/Sain/Release/RadialPlot ${output_dir}/all_pfm_files_c1_list.txt ${output_dir}/poisson_${dimension}d_${num_classes}c_${r_text}_c1_average${all_pfm_files_count}.pfm ${output_dir}/poisson_${dimension}d_${num_classes}c_${r_text}_c1_average${all_pfm_files_count}_mean.txt ${output_dir}/poisson_${dimension}d_${num_classes}c_${r_text}_c1_average${all_pfm_files_count}_variance.txt
rm ${output_dir}/all_pfm_files_c1_list.txt;

../BlueNoise/src/Main/Release/PFM2PPM ${output_dir}/poisson_${dimension}d_${num_classes}c_${r_text}_c1_average${all_pfm_files_count}.pfm  ${output_dir}/poisson_${dimension}d_${num_classes}c_${r_text}_c1_average${all_pfm_files_count}.ppm 1 1 1
endif

end #foreach num_classes

endif


# nonuniform 2 class with different number of samples
# match up with the hard dart throwing case
if(0) then

set dimension = 2;
set domain_size = "1 1";

set num_class = 2;
set all_class_ids = "0 1";

set total_num_samples = 10400;

set grid_res = 100;
set energy_store_method = grid${grid_res};

set num_samples_one_class = 1300; # nothing real, just for computation equivalence to the uniform multi class

set init_peak_method = default;
set refine_peak_method = random;

set k_number = 2;
@ init_num_trials = ${k_number} * ${num_samples_one_class};
set init_trough_method = random${init_num_trials};

@ refine_num_trials = ${num_samples_one_class};
set refine_trough_method = sphererandom${refine_num_trials};
# set refine_trough_method = "";

set init_shuffle_method = "throw";
set threshold_energy = "0.001";
# set shuffle_method = "gradualpeak0";
set refine_shuffle_method = "gradualpeak0";

set class_weighting_scheme = "fair";

set kernel_type = "gaussian";
set relative_kernel_size = 0.5;

@ moving_window_size = ${num_samples_one_class};
@ max_num_iterations = 2 * ${total_num_samples};

set num_runs = 10;
set sft_size = 512;
set sft_scale = ${sft_size};

foreach r_option (0p011 0p01sqrt2 0p02 0p04 0p08)# 0p16)

if(${r_option} == '0p01') then
set num_samples = "5200 5200";
else if(${r_option} == '0p011') then
set num_samples = "4297 6103";
else if(${r_option} == '0p01sqrt2') then
set num_samples = "2600 7800";
else if(${r_option} == '0p02') then
set num_samples = "1300 9100";
else if(${r_option} == '0p04') then
set num_samples = "325 10075";
else if(${r_option} == '0p08') then
set num_samples = "81 10319";
else if(${r_option} == '0p16') then
set num_samples = "20 10380";
else
echo unknown r value;
exit(1);
endif

set r_text = ${r_option}m0p01osqrt2;

set base_name = shuffle_${dimension}d_${num_class}c_${r_text}_${energy_store_method}_${refine_trough_method}_${class_weighting_scheme};

# sample set generation
set i = 0;
while(${i} < ${num_runs})

sleep 1;

./src/Main/Release/WhiteNoiseSamples ${dimension} ${num_class} ${num_samples} ${domain_size} > ${huge_dir}/${base_name}_init.txt

if(${init_shuffle_method} == "throw") then
time ./src/Main/Release/DartShuffling ${huge_dir}/${base_name}_init.txt ${dimension} ${domain_size} ${energy_store_method} ${init_peak_method} ${init_trough_method} ${init_shuffle_method}${threshold_energy} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${total_num_samples} ${total_num_samples} > ${huge_dir}/${base_name}_${i}.txt
else
time ./src/Main/Release/DartShuffling ${huge_dir}/${base_name}_init.txt ${dimension} ${domain_size} ${energy_store_method} ${init_peak_method} ${init_trough_method} ${init_shuffle_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} > ${huge_dir}/${base_name}_${i}.txt
endif

if(${refine_trough_method} != "") then
time ./src/Main/Release/DartShuffling ${huge_dir}/${base_name}_${i}.txt ${dimension} ${domain_size} ${energy_store_method} ${refine_peak_method} ${refine_trough_method} ${refine_shuffle_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} > ${huge_dir}/${base_name}_refine_${i}.txt
endif

# measure min spacing
#if(${num_runs} == 1) then
if(${i} == 0) then
set boundary_condition = 1;
set input_names = ${base_name}_${i};
if(${refine_trough_method} != "") then
set input_names = "${input_names} ${base_name}_refine_${i}";
endif

foreach input_name (${input_names})
set filtered_name = ${input_name}_filtered;

foreach class_option (0 1 all)

if(${class_option} == 'all') then
cp ${huge_dir}/${input_name}.txt ${huge_dir}/${filtered_name}.txt 
else
grep "^${class_option}" ${huge_dir}/${input_name}.txt > ${huge_dir}/${filtered_name}.txt 
endif

echo ${class_option} ${input_name}
./src/Main/Release/MinSampleSpacing ${huge_dir}/${filtered_name}.txt ${dimension} ${domain_size} ${boundary_condition}
end # foreach class_option
end # foreach input_name
endif

if(${refine_trough_method} != "") then
mv ${huge_dir}/${base_name}_refine_${i}.txt ${huge_dir}/${base_name}_${i}.txt;
endif

@ i = ${i} + 1;
end # while

# power spectrum
if(${sft_size} > 0) then
foreach class (0 1 all)

set all_pfm_files = "";
set i = 0;
while(${i} < ${num_runs})

set input_name = ${base_name}_${i};
set output_name = ${input_name}_c${class};

if(${class} == "all") then
src/Main/Release/SampleSelector ${huge_dir}/${input_name}.txt ${dimension} ${all_class_ids} > ${huge_dir}/${output_name}_selected.txt
else
src/Main/Release/SampleSelector ${huge_dir}/${input_name}.txt ${dimension} ${class} > ${huge_dir}/${output_name}_selected.txt
endif

./src/Sain/Release/SFT ${huge_dir}/${output_name}_selected.txt ${huge_dir}/${output_name}.pfm ${dimension} 1 1 ${sft_size} ${sft_scale}

set all_pfm_files = "${all_pfm_files} ${huge_dir}/${output_name}.pfm";

@ i = ${i} + 1;
end

echo ${all_pfm_files} > ${huge_dir}/all_pfm_files_list.txt;

./src/Sain/Release/RadialPlot ${huge_dir}/all_pfm_files_list.txt ${output_dir}/${base_name}_c${class}_average${num_runs}.pfm ${output_dir}/${base_name}_c${class}_average${num_runs}_mean.txt ${output_dir}/${base_name}_c${class}_average${num_runs}_variance.txt
rm ${huge_dir}/all_pfm_files_list.txt;

./src/Sain/Release/PFM2PPM ${output_dir}/${base_name}_c${class}_average${num_runs}.pfm ${output_dir}/${base_name}_c${class}_average${num_runs}.ppm 1 1 1

end # foreach class
endif # (${sft_size} > 0) then

end # foreach r_option

endif

# class priority, sequential generation, matching 0p040p020p01
if(0) then

set dimension = 2;
set domain_size = "1 1";
set num_class = 3;
set all_class_ids = "0 1 2";
set num_samples = "323 1295 5183";
set num_samples_array = (323 1295 5183);
set num_samples_text = 323n1295n5183;
set total_num_samples = 6801;

set num_samples_one_class = 1300; # nothing real, just for computation equivalence to the uniform multi class

set init_peak_method = default;
set refine_peak_method = random;

set k_number = 2;
@ init_num_trials = ${k_number} * ${num_samples_one_class};
set init_trough_method = random${init_num_trials};

@ refine_num_trials = ${num_samples_one_class};
set refine_trough_method = sphererandom${refine_num_trials};
# set refine_trough_method = "";

set init_shuffle_method = "throw";
set threshold_energy = "0.001";
set refine_shuffle_method = "gradualpeak0";

set class_weighting_scheme = "fair";

set kernel_type = "gaussian";
set relative_kernel_size = 0.5;

@ moving_window_size = ${num_samples_one_class};
@ max_num_iterations = 2 * ${total_num_samples};

set num_runs = 10;
set sft_size = 512;
set sft_scale = ${sft_size};

set which_run = 0;

while(${which_run} < ${num_runs})

# sample generation
set current_num_class = 1;
while(${current_num_class} <= ${num_class})

set num_samples = "";
set class_text = "";
set last_class_text = "";
set i = 0;
while(${i} < ${current_num_class})
@ i = ${i} + 1;
set num_samples = "${num_samples} $num_samples_array[${i}]";
set class_text = "${class_text}1c";
if(${i} < ${current_num_class}) then
set last_class_text = "${last_class_text}1c";
endif
end

set grid_size = 20;
set i = 1;
while(${i} < ${current_num_class})
@ i = ${i} + 1;
@ grid_size = 2 * ${grid_size};
end

set energy_store_method = grid${grid_size};

set base_name = shuffle_${dimension}d_${class_text}_${num_samples_text};
set lase_name = shuffle_${dimension}d_${last_class_text}_${num_samples_text};

set num_samples_current_class = $num_samples_array[${current_num_class}];

./src/Main/Release/WhiteNoiseSamples ${dimension} 1 ${num_samples_current_class} ${domain_size} > ${huge_dir}/${base_name}_init.txt

@ current_num_class_minus_one = ${current_num_class} - 1;
set i = 0;
while(${i} < ${current_num_class_minus_one})
./src/Main/Release/ProcessLloydSites ${huge_dir}/${base_name}_init.txt ${huge_dir}/${base_name}_init.txt ${dimension} "increment" ${i}
@ i = ${i} + 1;
end

if(${refine_trough_method} != "") then
set tail_name = ${refine_trough_method}_${init_shuffle_method}_${class_weighting_scheme};
else
set tail_name = ${init_trough_method}_${init_shuffle_method}_${class_weighting_scheme};
endif

set output_base_name = ${base_name}_${tail_name};

if(${current_num_class} > 1) then
set last_base_name = ${lase_name}_${tail_name};

cat ${huge_dir}/${last_base_name}_${which_run}.txt ${huge_dir}/${base_name}_init.txt > ${huge_dir}/${base_name}_init_cat.txt;
mv ${huge_dir}/${base_name}_init_cat.txt ${huge_dir}/${base_name}_init.txt;
endif

./src/Main/Release/DartShuffling ${huge_dir}/${base_name}_init.txt ${dimension} ${domain_size} ${energy_store_method} ${init_peak_method} ${init_trough_method} ${init_shuffle_method}${threshold_energy} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${num_samples_current_class} ${num_samples_current_class} > ${huge_dir}/${output_base_name}_${which_run}.txt

if(${refine_trough_method} != "") then
time ./src/Main/Release/DartShuffling ${huge_dir}/${output_base_name}_${which_run}.txt ${dimension} ${domain_size} ${energy_store_method} ${refine_peak_method} ${refine_trough_method} ${refine_shuffle_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} > ${huge_dir}/${output_base_name}_refine_${which_run}.txt
endif

if(${refine_trough_method} != "") then
mv ${huge_dir}/${output_base_name}_refine_${which_run}.txt ${huge_dir}/${output_base_name}_${which_run}.txt;
endif

./src/Main/Release/ProcessLloydSites ${huge_dir}/${output_base_name}_${which_run}.txt ${huge_dir}/${output_base_name}_${which_run}.txt ${dimension} "fasten" 

@ current_num_class = ${current_num_class} + 1;
end # while

./src/Main/Release/ProcessLloydSites ${huge_dir}/${output_base_name}_${which_run}.txt ${huge_dir}/${output_base_name}_${which_run}.txt ${dimension} "loosen" 

# measure min spacing
#if(${num_runs} == 1) then
if(${which_run} == 0) then
set boundary_condition = 1;
set input_names = ${output_base_name}_${which_run};

foreach input_name (${input_names})
set filtered_name = ${input_name}_filtered;

foreach class_option (0 1 2 all)

if(${class_option} == 'all') then
cp ${huge_dir}/${input_name}.txt ${huge_dir}/${filtered_name}.txt 
else
grep "^${class_option}" ${huge_dir}/${input_name}.txt > ${huge_dir}/${filtered_name}.txt 
endif

echo ${class_option} ${input_name}
./src/Main/Release/MinSampleSpacing ${huge_dir}/${filtered_name}.txt ${dimension} ${domain_size} ${boundary_condition}
end # foreach class_option
end # foreach input_name

endif #if(${which_run} == 0)

@ which_run = ${which_run} + 1;
end # while

# power spectrum
if(${sft_size} > 0) then
foreach class (0 1 2 all)

set all_pfm_files = "";
set i = 0;
while(${i} < ${num_runs})

set input_name = ${output_base_name}_${i};
set output_name = ${input_name}_c${class};

if(${class} == "all") then
src/Main/Release/SampleSelector ${huge_dir}/${input_name}.txt ${dimension} ${all_class_ids} > ${huge_dir}/${output_name}_selected.txt
else
src/Main/Release/SampleSelector ${huge_dir}/${input_name}.txt ${dimension} ${class} > ${huge_dir}/${output_name}_selected.txt
endif

./src/Sain/Release/SFT ${huge_dir}/${output_name}_selected.txt ${huge_dir}/${output_name}.pfm ${dimension} 1 1 ${sft_size} ${sft_scale}

set all_pfm_files = "${all_pfm_files} ${huge_dir}/${output_name}.pfm";

@ i = ${i} + 1;
end

echo ${all_pfm_files} > ${huge_dir}/all_pfm_files_list.txt;

./src/Sain/Release/RadialPlot ${huge_dir}/all_pfm_files_list.txt ${output_dir}/${base_name}_c${class}_average${num_runs}.pfm ${output_dir}/${base_name}_c${class}_average${num_runs}_mean.txt ${output_dir}/${base_name}_c${class}_average${num_runs}_variance.txt
rm ${huge_dir}/all_pfm_files_list.txt;

./src/Sain/Release/PFM2PPM ${output_dir}/${base_name}_c${class}_average${num_runs}.pfm ${output_dir}/${base_name}_c${class}_average${num_runs}.ppm 1 1 1

end # foreach class
endif # (${sft_size} > 0) then

endif 

# class priority, concurrent generation, matching 0p040p020p01
if(0) then

set dimension = 2;
set domain_size = "1 1";
set num_class = 3;
set all_class_ids = "0 1 2";
set num_samples = "323 1295 5183";
set num_samples_text = 323n1295n5183;
set total_num_samples = 6801;

set grid_size = 80;
set energy_store_method = grid${grid_size};

set num_samples_one_class = 1300; # nothing real, just for computation equivalence to the uniform multi class

set init_peak_method = default;
set refine_peak_method = random;

set k_number = 2;
@ init_num_trials = ${k_number} * ${num_samples_one_class};
set init_trough_method = random${init_num_trials};

@ refine_num_trials = ${num_samples_one_class};
set refine_trough_method = sphererandom${refine_num_trials};
# set refine_trough_method = "";

set init_shuffle_method = "throw";
set threshold_energy = "0.001";
set refine_shuffle_method = "gradualpeak0";

set class_weighting_scheme = "fair";

set relative_kernel_size = 0.5;

@ moving_window_size = ${num_samples_one_class};
@ max_num_iterations = 2 * ${total_num_samples};

set num_runs = 10;
set sft_size = 512;
set sft_scale = ${sft_size};

foreach kernel_type ("gaussian" "uniform")

set which_run = 0;
while(${which_run} < ${num_runs})

# sample generation
set base_name = shuffle_${dimension}d_${num_class}c_${num_samples_text};

./src/Main/Release/WhiteNoiseSamples ${dimension} ${num_class} ${num_samples} ${domain_size} > ${huge_dir}/${base_name}_init.txt

if(${refine_trough_method} != "") then
set tail_name = ${refine_trough_method}_${init_shuffle_method}_${class_weighting_scheme}_${kernel_type};
else
set tail_name = ${init_trough_method}_${init_shuffle_method}_${class_weighting_scheme}_${kernel_type};
endif

set output_base_name = ${base_name}_${tail_name};

./src/Main/Release/DartShuffling ${huge_dir}/${base_name}_init.txt ${dimension} ${domain_size} ${energy_store_method} ${init_peak_method} ${init_trough_method} ${init_shuffle_method}${threshold_energy} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${total_num_samples} ${total_num_samples} > ${huge_dir}/${output_base_name}_${which_run}.txt

if(${refine_trough_method} != "") then
./src/Main/Release/DartShuffling ${huge_dir}/${output_base_name}_${which_run}.txt ${dimension} ${domain_size} ${energy_store_method} ${refine_peak_method} ${refine_trough_method} ${refine_shuffle_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} > ${huge_dir}/${output_base_name}_refine_${which_run}.txt
endif

if(${refine_trough_method} != "") then
mv ${huge_dir}/${output_base_name}_refine_${which_run}.txt ${huge_dir}/${output_base_name}_${which_run}.txt;
endif

# measure min spacing
#if(${num_runs} == 1) then
if(${which_run} == 0) then
set boundary_condition = 1;
set input_names = ${output_base_name}_${which_run};

foreach input_name (${input_names})
set filtered_name = ${input_name}_filtered;

foreach class_option (0 1 2 all)

if(${class_option} == 'all') then
cp ${huge_dir}/${input_name}.txt ${huge_dir}/${filtered_name}.txt 
else
grep "^${class_option}" ${huge_dir}/${input_name}.txt > ${huge_dir}/${filtered_name}.txt 
endif

echo ${class_option} ${input_name}
./src/Main/Release/MinSampleSpacing ${huge_dir}/${filtered_name}.txt ${dimension} ${domain_size} ${boundary_condition}
end # foreach class_option
end # foreach input_name

endif #if(${which_run} == 0)

@ which_run = ${which_run} + 1;
end # while

# power spectrum
if(${sft_size} > 0) then
foreach class (0 1 2 all)

set all_pfm_files = "";
set i = 0;
while(${i} < ${num_runs})

set input_name = ${output_base_name}_${i};
set output_name = ${input_name}_c${class};

if(${class} == "all") then
src/Main/Release/SampleSelector ${huge_dir}/${input_name}.txt ${dimension} ${all_class_ids} > ${huge_dir}/${output_name}_selected.txt
else
src/Main/Release/SampleSelector ${huge_dir}/${input_name}.txt ${dimension} ${class} > ${huge_dir}/${output_name}_selected.txt
endif

./src/Sain/Release/SFT ${huge_dir}/${output_name}_selected.txt ${huge_dir}/${output_name}.pfm ${dimension} 1 1 ${sft_size} ${sft_scale}

set all_pfm_files = "${all_pfm_files} ${huge_dir}/${output_name}.pfm";

@ i = ${i} + 1;
end

echo ${all_pfm_files} > ${huge_dir}/all_pfm_files_list.txt;

./src/Sain/Release/RadialPlot ${huge_dir}/all_pfm_files_list.txt ${output_dir}/${output_base_name}_c${class}_average${num_runs}.pfm ${output_dir}/${output_base_name}_c${class}_average${num_runs}_mean.txt ${output_dir}/${output_base_name}_c${class}_average${num_runs}_variance.txt
rm ${huge_dir}/all_pfm_files_list.txt;

./src/Sain/Release/PFM2PPM ${output_dir}/${output_base_name}_c${class}_average${num_runs}.pfm ${output_dir}/${output_base_name}_c${class}_average${num_runs}.ppm 1 1 1

end # foreach class
endif # (${sft_size} > 0) then

end # foreach kernel_type

endif 
