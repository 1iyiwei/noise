#!/bin/tcsh -f

#echo $#argv

if ($#argv < 1) then
 echo "please specify the output directory"
 exit(1)
endif

set outputdir = $argv[1]

mkdir -p $outputdir

# basic dart throwing with 2/3 classes
if(1) then

set dimension = 2;

foreach exe_option (0 1 2)

if(${exe_option} == 0) then
set exe_name = DartThrowing;
set output_name = grid;
else if(${exe_option} == 1) then
set exe_name = TreeThrowing;
set output_name = tree;
else
set exe_name = MultiTreeUniformSampling;
set output_name = multitree;
endif

foreach option (0 1)

if(${option} == 0) then
set num_classes = 2;
set priority = "0 0";
set r_values = "0.02 0.02";
set r_text = 0p02;
else if(${option} == 1) then
set num_classes = 3;
set priority = "0 0 0";
set r_values = "0.04 0.02 0.01";
set r_text = 0p040p020p01;
else
echo unknown option;
exit(1);
endif

set dot_radius = 0.002;

set k_value = 8;
set domain_size = "1 1";

echo src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size
src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size > ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${r_text}.txt

if(1) then
src/Main/Release/DrawSamples ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${r_text}.txt ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${r_text}.fig 2 ${dot_radius} 1 ${domain_size}
src/Scripts/fig2eps -f ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${r_text}.fig
epstopdf ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${r_text}.eps --outfile ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${r_text}.pdf
endif

end # foreach option

end # foreach exe_option
endif

# different r matrix computation
if(1) then

set dimension = 2;

set num_classes = 2;
set priority = "0 0";
set r_values = "0.02 0.02";
set r_text = 0p02;

set dot_radius = 0.002;

set k_value = 30;
set domain_size = "1 1";

foreach sign ('0' '+' '-')

echo src/Main/Release/DartThrowing $dimension $sign$num_classes $priority $r_values $k_value $domain_size
src/Main/Release/DartThrowing $dimension $sign$num_classes $priority $r_values $k_value $domain_size > ${outputdir}/poisson_${dimension}d_${sign}${num_classes}c_${r_text}.txt

if(1) then
src/Main/Release/DrawSamples ${outputdir}/poisson_${dimension}d_${sign}${num_classes}c_${r_text}.txt ${outputdir}/poisson_${dimension}d_${sign}${num_classes}c_${r_text}.fig 2 ${dot_radius} 1 ${domain_size}
src/Scripts/fig2eps -f ${outputdir}/poisson_${dimension}d_${sign}${num_classes}c_${r_text}.fig
epstopdf ${outputdir}/poisson_${dimension}d_${sign}${num_classes}c_${r_text}.eps --outfile ${outputdir}/poisson_${dimension}d_${sign}${num_classes}c_${r_text}.pdf
endif

end #foreach sign

endif

# rho control for more uniform relative number of samples across different classes
if(1) then

set dimension = 2;

set num_classes = 3;
set r_values = "0.04 0.02 0.01";
set r_text = 0p040p020p01;

#set k_value = 100;
set k_value = 30;
#set rho_number = 0.696400653;
set rho_number = 0.67;
set domain_size = "1 1";

set sft_size = 512;
set sft_scale = 512;

foreach exe_option (0 1)

if(${exe_option} == 0) then
set exe_name = DartThrowing;
set output_name = grid;
else
set exe_name = TreeThrowing;
set output_name = tree;
endif

foreach k_option (0 1)

if(${k_option} == 0) then
set k_value_arg = ${k_value};
else
set k_value_arg = ${rho_number};
endif

foreach p_option (0 1)

if(${p_option} == 0) then
set priority = "0 0 0";
set p_text = 000;
else
set priority = "0 1 2";
set p_text = 012;
endif

src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value_arg $domain_size > ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${p_text}_${r_text}_${k_option}k.txt

echo number of samples for ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${p_text}_${r_text}_${k_option}k.txt
set id = 0;
while(${id} < ${num_classes})

grep "^${id}" ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${p_text}_${r_text}_${k_option}k.txt | wc -l

@ id = ${id} + 1
end # while

if(0) then
set id = 0;
while(${id} < ${num_classes})

src/Main/Release/SampleSelector ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${p_text}_${r_text}_${k_option}k.txt ${dimension} ${id} > ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${p_text}_${r_text}_${k_option}k_selected${id}.txt

../BlueNoise/src/Main/Release/SFT ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${p_text}_${r_text}_${k_option}k_selected${id}.txt ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${p_text}_${r_text}_${k_option}k_selected${id}.pfm ${dimension} ${domain_size} ${sft_size} ${sft_scale}

../BlueNoise/src/Main/Release/PFM2PPM ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${p_text}_${r_text}_${k_option}k_selected${id}.pfm ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${p_text}_${r_text}_${k_option}k_selected${id}.ppm 1 1 0

@ id = ${id} + 1
end # while
endif

end # foreach p_option
end # foreach k_option
end # foreach exe_option

endif

# sampling history
if(1) then

set dimension = 2;

set num_classes = 2;
set r_values = "0.02 0.00755929";
set r_text = 0p02m0p01osqrt2;
set priority = "0.425 0.575";

set rho_number = 0.67;
set target_samples = 10400;
set domain_size = "1 1";

set normalize = ${rho_number};
set max_num_time_samples = 1000000;

foreach k_option (0 1 2)

if(${k_option} == 0) then
set k_value = -${target_samples};
set exe_name = DartThrowingHistory;
else if(${k_option} == 1) then
set k_value = ${rho_number};
set exe_name = DartThrowingClasslessRhoHistory;
else if(${k_option} == 2) then
set k_value = ${rho_number};
set exe_name = DartThrowingHistory;
else
echo unknown k_option;
exit(1);
endif

src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size ${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}_${k_option}k_history.txt > ${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}_${k_option}k.txt

src/Main/Release/AnalyzeHistory cdf ${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}_${k_option}k_history.txt  ${dimension} ${num_classes} ${r_values} ${normalize} ${max_num_time_samples} ${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}_${k_option}k_history_id0.txt ${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}_${k_option}k_history_id1.txt


echo "" > foo.gp;

echo set terminal postscript eps >> foo.gp
echo set output \'${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}_${k_option}k_history.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"trials\" >> foo.gp
echo set ylabel \"cdf for % of target \# of samples\" >> foo.gp
#echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
echo plot "[0:1] [0:1.2]" \"${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}_${k_option}k_history_id0.txt\" title \'class 0\' with lines linetype rgb \"red\", \"${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}_${k_option}k_history_id1.txt\" title \'class 1\' with lines linetype rgb \"blue\" >> foo.gp

wgnuplot foo.gp
rm foo.gp;

epstopdf ${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}_${k_option}k_history.eps --outfile foo.pdf;
#cp ${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}_${k_option}k_history.pdf foo.pdf
ps2pdf13 -dPDFSETTINGS=/prepress foo.pdf ${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}_${k_option}k_history.pdf;
rm foo.pdf;

end # foreach k_option

endif

# adaptive sampling
if(1) then

set dimension = 2;

set num_classes = 2;
set priority = "0 0";

set r_values = "0.02 0.02";
set r_text = 0p02;
set dot_radius = 0.002;

set k_value = 16.67;
set domain_size = "1 1";

set conflict_metric = 0; # mean

#set field_name = "uniform";
set field_name = "blob";
set field_name = "2blob";
#set field_name = "stripe";

set output_name = adaptive_${dimension}d_${num_classes}c_${r_text};

# create distance fields
echo build fields
foreach class (0 1)
set output_field_name = ${field_name}_${class}

if(${field_name} == 'uniform') then
echo "1 2 1 0.5 0.5 10000000000 10000000000" > ${outputdir}/${output_field_name}.txt
else if(${field_name} == 'blob') then
echo "1 2 1 0.5 0.5 0.3 0.3" > ${outputdir}/${output_field_name}.txt
else if(${field_name} == '2blob') then
set domain_size = "1 1.5";
if(${class} == 0) then
echo "1 2 1 0.5 0.5 0.3 0.3 " > ${outputdir}/${output_field_name}.txt
else
echo "1 2 1 0.5 1.0 0.3 0.3 " > ${outputdir}/${output_field_name}.txt
endif
else if(${field_name} == 'stripe') then
if(${class} == 0) then
echo "1 2 1 0.5 0.5 10000000000 0.2" > ${outputdir}/${output_field_name}.txt
else
echo "1 2 1 0.5 0.5 0.2 10000000000" > ${outputdir}/${output_field_name}.txt
endif
else
echo unknown field name
exit(1);
endif

src/Sain/Release/MakeBlobs ${outputdir}/${output_field_name}.txt ${outputdir}/${output_field_name}.pfm 512 1 ${domain_size};
src/Sain/Release/PFM2PPM ${outputdir}/${output_field_name}.pfm ${outputdir}/${output_field_name}.ppm 1 1 2;
end # foreach class

foreach exe_option (0 1)

if(${exe_option} == 0) then
set exe_name = AdaptiveDartThrowing;
set output_name_exe = grid;
else if(${exe_option} == 1) then
set exe_name = MultiTreeAdaptiveSampling;
set output_name_exe = multitree;
else
endif

set output_name_exe = ${output_name_exe}_${output_name}

echo src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size ${conflict_metric} ${outputdir}/${field_name}_0.pfm ${outputdir}/${field_name}_1.pfm
src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size ${conflict_metric} ${outputdir}/${field_name}_0.pfm ${outputdir}/${field_name}_1.pfm > ${outputdir}/${output_name_exe}.txt

foreach id ('' 0 1)

if(${id} == '') then
set output_name_final = ${output_name_exe};
else
set output_name_final = ${output_name_exe}_${id};
grep "^${id}" ${outputdir}/${output_name_exe}.txt > ${outputdir}/${output_name_final}.txt
endif

src/Main/Release/DrawSamples ${outputdir}/${output_name_final}.txt ${outputdir}/${output_name_final}.fig ${dimension} ${dot_radius} 1 ${domain_size}
src/Scripts/fig2eps -f ${outputdir}/${output_name_final}.fig
epstopdf ${outputdir}/${output_name_final}.eps --outfile ${outputdir}/${output_name_final}.pdf
end # foreach id

end #foreach exe_option

endif


# object placement test
if(1) then

set exe_name = DartThrowing;
set output_name = grid;

set dimension = 2;
set domain_size = "1 1";

set num_classes = 2;
set priority = "0 0";
set r_values = "0.02 0.02";
set r_text = 0p02;

set k_value = 8;

set object_scale = 0.028;

set field_name = 'blob';
set texture_size = 32;
set window_size = 512;

set background_color = -1;
set batch_mode = 1;
set boundary_condition = 0; # none or toroidal

set fig_or_ppm = 1; # 0 for fig, 1 for ppm

if(${fig_or_ppm} == 0) then
set r_values = "0.2 0.2";
set r_text = 0p2;
set object_scale = 0.28;
endif

if(${background_color} < 0) then
set background_color = "background_texture";
set color = "0 0 1";
echo "1 2 1 0.5 0.5 0.3 0.3" > ${outputdir}/${background_color}.txt
src/Sain/Release/MakeBlobs ${outputdir}/${background_color}.txt ${outputdir}/${background_color}.pfm ${texture_size} 0 ${domain_size} ${color}
src/Sain/Release/PFM2PPM ${outputdir}/${background_color}.pfm ${outputdir}/${background_color}.ppm 1 1 2
set background_color = ${outputdir}/${background_color}.ppm
endif

set class = 0;
set object_files = "";
set object_sizes = "";
while(${class} < ${num_classes})

set output_field_name = ${field_name}_${class}

if(${field_name} == 'blob') then
echo "1 2 1 0.5 0.5 0.1 0.2" > ${outputdir}/${output_field_name}.txt
else
echo unknown field name;
exit(1);
endif

if(${class} == 0) then
set color = "1 0 0";
else if(${class} == 1) then
set color = "0 1 0";
else
set color = "0 0 1";
endif

src/Sain/Release/MakeBlobs ${outputdir}/${output_field_name}.txt ${outputdir}/${output_field_name}.pfm ${texture_size} 0 ${domain_size} ${color}
src/Sain/Release/PFM2PPM ${outputdir}/${output_field_name}.pfm ${outputdir}/${output_field_name}.ppm 1 1 2

if(${fig_or_ppm} == 0) then
set object_files = "${object_files} ${output_field_name}.ppm";
else
set object_files = "${object_files} ${outputdir}/${output_field_name}.ppm";
endif

set object_sizes = "${object_sizes} ${object_scale}";

@ class = ${class} + 1;
end #while(${class} < ${num_classes})

set output_base_name = ${output_name}_${dimension}d_${num_classes}c_${r_text};
echo src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size
src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size > ${outputdir}/${output_base_name}.txt

if(${fig_or_ppm} == 0) then
set file_ext = "fig";
else
set file_ext = "ppm";
endif

set output_object_name = ${output_base_name}_objects;
set output_file_name = ${outputdir}/${output_object_name}.${file_ext};

echo src/GPU/Release/DrawObjects ${outputdir}/${output_base_name}.txt ${output_file_name} ${dimension} ${domain_size} ${num_classes} ${object_files} ${object_sizes} ${window_size} ${background_color} ${batch_mode} ${boundary_condition}
src/GPU/Release/DrawObjects ${outputdir}/${output_base_name}.txt ${output_file_name} ${dimension} ${domain_size} ${num_classes} ${object_files} ${object_sizes} ${window_size} ${background_color} ${batch_mode} ${boundary_condition}

if(${fig_or_ppm} == 0) then
src/Scripts/fig2eps -f ${outputdir}/${output_object_name}.fig
epstopdf ${outputdir}/${output_object_name}.eps --outfile ${outputdir}/${output_object_name}.pdf
else

if(0) then
if(${background_color} == 0) then
set transparent_color = 'black';
else
set transparent_color = 'white';
endif

convert ${outputdir}/${output_object_name}.ppm -transparent ${transparent_color} ${outputdir}/${output_object_name}.gif
endif

endif

endif

# discrete dart throwing
if(1) then

set dimension = 2;
set num_classes = 2;

set c1_r_value = 0.014;
set c1_r_text = 0p014;

set r_values = "0.02 0.02";
set r_text = 0p02;

set priority = "0 0";
set p_text = 00;

set dot_radius = 0.002;

set k_value = 10;
set k_value_discrete = 100;

set domain_size = "1 1";

# single class dart throwing
echo 1-class dart throwing
src/Main/Release/DartThrowing $dimension 1 0 $c1_r_value $k_value $domain_size > ${outputdir}/poisson_${dimension}d_1c_0_${c1_r_text}.txt

src/Main/Release/ResetClassLabel ${dimension} ${outputdir}/poisson_${dimension}d_1c_0_${c1_r_text}.txt > ${outputdir}/poisson_${dimension}d_1c_0_${c1_r_text}_reset.txt

# discrete dart throwing
echo ${num_classes}-class discrete dart throwing
src/Main/Release/DiscreteDartThrowing $dimension $num_classes $priority $r_values $k_value_discrete $domain_size ${outputdir}/poisson_${dimension}d_1c_0_${c1_r_text}_reset.txt > ${outputdir}/discrete_${dimension}d_${num_classes}c_${p_text}_${r_text}.txt

src/Main/Release/DrawSamples ${outputdir}/discrete_${dimension}d_${num_classes}c_${p_text}_${r_text}.txt ${outputdir}/discrete_${dimension}d_${num_classes}c_${p_text}_${r_text}.fig 2 ${dot_radius} 1 ${domain_size}
src/Scripts/fig2eps -f ${outputdir}/discrete_${dimension}d_${num_classes}c_${p_text}_${r_text}.fig
epstopdf ${outputdir}/discrete_${dimension}d_${num_classes}c_${p_text}_${r_text}.eps --outfile ${outputdir}/discrete_${dimension}d_${num_classes}c_${p_text}_${r_text}.pdf
endif

# R1G2B1
if(1) then

set dimension = 2;
set num_classes = 3;
set priority = "0 0 0";

set num_iterations = 3;

set domain_size = "1 1";

set grid_size_1d = 40;
set dot_radius = 0.01;
set grid_size = "${grid_size_1d} ${grid_size_1d}";

set input_base_name = r1g2b1_grid_${grid_size_1d}x${grid_size_1d};
set output_base_name = r1g2b1_dart_${grid_size_1d}x${grid_size_1d};

src/Tiling/Release/R1G2B1GridSampling ${domain_size} ${grid_size} > ${outputdir}/${input_base_name}.txt

set iteration = 0;
src/Main/Release/ResetClassLabel ${dimension} ${outputdir}/${input_base_name}.txt > ${outputdir}/${output_base_name}_${iteration}.txt

while(${iteration} < ${num_iterations})

set previous_iteration = ${iteration};
@ iteration = ${iteration} + 1;

if(${iteration} == 1) then
set r_values = "0.049 0.02 0.02 0.035 0.02 0.049"; # 1:1/sqrt(2):1
set k_value = 0.63;
else if(${iteration} == 2) then
set r_values = "0.035 0.014 0.014 0.025 0.014 0.035"; # 1:1/sqrt(2):1
set k_value = 0.80;
else if(${iteration} == 3) then
set r_values = "0.02 0.008 0.008 0.014 0.008 0.02"; # 1:1/sqrt(2):1
set k_value = 0.9999;
else
echo unhandled iteration
exit(1);
endif

src/Main/Release/DiscreteDartThrowing $dimension $num_classes $priority $r_values $k_value $domain_size ${outputdir}/${output_base_name}_${previous_iteration}.txt > ${outputdir}/${output_base_name}_${iteration}.txt

foreach name (${output_base_name}_${iteration})

src/Main/Release/DrawSamples ${outputdir}/${name}.txt ${outputdir}/${name}.fig 2 ${dot_radius} 1 ${domain_size}
src/Scripts/fig2eps -f ${outputdir}/${name}.fig
epstopdf ${outputdir}/${name}.eps --outfile ${outputdir}/${name}.pdf

end # foreach name

end # while(${iteration} < ${num_iterations})

endif

# DFT 
if(1) then

set dimension = 2;
set domain_size = "1 1";
set num_pairs = 1600; #640000; #1600;

set dft_size = 256;
set dft_scale = ${dft_size};

foreach option ("hat" "needle")

if(${option} == "hat") then
else if(${option} == "needle") then
set needle_dimension = ${dimension};
set needle_length = 0.02;
else
echo unknown option
exit(1);
endif

set diff_file_base_name = diff_${option}_${dimension}d_${num_pairs}

if(${option} == "hat") then
src/Theory/Release/DiffThrowing ${option} ${dimension} ${num_pairs} ${domain_size} > ${outputdir}/${diff_file_base_name}.txt
else if(${option} == "needle") then
src/Theory/Release/DiffThrowing ${option} ${dimension} ${needle_dimension} ${needle_length} ${num_pairs} ${domain_size} > ${outputdir}/${diff_file_base_name}.txt
endif

src/Sain/Release/DFT ${outputdir}/${diff_file_base_name}.txt ${outputdir}/${diff_file_base_name}.pfm ${dimension} 1 1 ${dft_size} ${dft_scale} 

src/Sain/Release/PFM2PPM ${outputdir}/${diff_file_base_name}.pfm ${outputdir}/${diff_file_base_name}.ppm 1 1 0

echo ${outputdir}/${diff_file_base_name}.pfm > ${outputdir}/all_pfm_files_list.txt;
src/Sain/Release/RadialPlot ${outputdir}/all_pfm_files_list.txt ${outputdir}/${diff_file_base_name}_all.pfm ${outputdir}/${diff_file_base_name}_mean.txt ${outputdir}/${diff_file_base_name}_variance.txt 
rm ${outputdir}/all_pfm_files_list.txt;

src/Scripts/txt2dat ${outputdir}/${diff_file_base_name}_mean.txt ${outputdir}/${diff_file_base_name}_variance.txt ${num_pairs} ${outputdir}/${diff_file_base_name}_mean.dat ${outputdir}/${diff_file_base_name}_variance.dat 1

echo set terminal postscript eps >> foo.gp
echo set output \'${outputdir}/${diff_file_base_name}_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp
echo set autoscale y >> foo.gp
echo plot "[1:128]" \"${outputdir}/${diff_file_base_name}_mean.dat\" with line linetype rgb \"red\" >> foo.gp

wgnuplot foo.gp
rm foo.gp

set output_name = ${diff_file_base_name}_mean;
epstopdf ${outputdir}/${output_name}.eps --outfile ${outputdir}/${output_name}.pdf;

end # foreach option

endif

# diff histogram
if(1) then

set dimension = 2;
set domain_size = "1 1";
set num_pairs = 640000;

set option = "hybrid";
set needle_length = 02;
set needle_variance = 002;

# for histogram
set which_dimension = "0 1";
set hist_freq = 1;
set num_bins = 800;

foreach length_sign ('-' '+')
foreach variance_sign ('-' '+')

set diff_file_base_name = diff_${option}_${dimension}d_${length_sign}0p${needle_length}_${variance_sign}0p${needle_variance}_${num_pairs}

echo ${diff_file_base_name}

src/Theory/Release/DiffThrowing ${option} ${dimension} ${length_sign}0.${needle_length} ${variance_sign}0.${needle_variance} ${num_pairs} ${domain_size} > ${outputdir}/${diff_file_base_name}.txt

echo ${outputdir}/${diff_file_base_name}.txt > ${outputdir}/all_sample_file_list.txt;

src/Theory/Release/DiffHistogram ${outputdir}/all_sample_file_list.txt $dimension $which_dimension ${hist_freq} $num_bins > ${outputdir}/${diff_file_base_name}_histogram.txt

rm ${outputdir}/all_sample_file_list.txt;

set output_name = ${diff_file_base_name}_histogram;

echo "" > foo.gp;

echo set terminal postscript eps >> foo.gp
echo set output \'${outputdir}/${output_name}.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"\|coordinate difference\|\" >> foo.gp
echo set ylabel \"density\" >> foo.gp
#echo set key off >> foo.gp
echo set autoscale y >> foo.gp
set max_bin = "";
@ max_bin = $num_bins - 1;
echo plot "[0:1.41245]" \"${outputdir}/${output_name}.txt\" title \'Poisson disk\' with lines linetype rgb \"red\" >> foo.gp

wgnuplot foo.gp
rm foo.gp;

epstopdf ${outputdir}/${output_name}.eps --outfile ${outputdir}/${output_name}.pdf;

end # foreach variance_sign
end # foreach length_sign

endif

