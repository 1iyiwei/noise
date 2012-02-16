#!/bin/tcsh -f

set outputdir = data;

# teaser 3 class example
if(0) then

set outputdir = data;

set dimension = 2;

set exe_name = DartThrowing;
set output_name = grid;

set num_classes = 3;
set priority = "0 0 0";

set r_values = "0.04 0.02 0.01";
set r_text = 0p040p020p01;
set dot_radius = 0.002;

if(1) then
set r_values = "0.08 0.04 0.02";
set r_text = 0p080p040p02;
set dot_radius = 0.004;
endif

set k_value = 0.70;
set domain_size = "1 1";

echo src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size
#src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size > ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${r_text}.txt

foreach id (0 1 2 all)

set output_root_name = ${output_name}_${dimension}d_${num_classes}c_${r_text}_${id}

if(${id} == 'all') then
cp ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${r_text}.txt ${outputdir}/${output_root_name}.txt
else
grep "^${id}" ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${r_text}.txt > ${outputdir}/${output_root_name}.txt
endif

src/Main/Release/DrawSamples ${outputdir}/${output_root_name}.txt ${outputdir}/${output_root_name}.fig ${dimension} ${dot_radius} 1 ${domain_size}
src/Scripts/fig2eps -f ${outputdir}/${output_root_name}.fig
epstopdf ${outputdir}/${output_root_name}.eps --outfile ${outputdir}/${output_root_name}.pdf
end

endif

# dart throwing spectrum
if(0) then

set dimension = 2;

if(0) then
set num_classes = 3;
set priority = "0 0 0";
set r_values = "0.02 0.02 0.02";
set ids = "0 1 2";
set ids = 0;
set r_text = 0p02;
endif

if(0) then # single category will rough similar number of samples
set num_classes = 1;
set priority = "0";
set r_values = "0.0225";
set ids = 0;
set r_text = 0p0225;
endif

if(1) then
set num_classes = 8;
set priority = "0 0 0 0 0 0 0 0";
set r_values = "0.02 0.02 0.02 0.02 0.02 0.02 0.02 0.02";
set ids = 0;
set r_text = 0p02;
endif

set k_value = 100;

set domain_size = "1 1";
set num_relaxations = 30;

set all_pfm_files = "";
set all_pfm_files_count = 0;

foreach i (0 1 2 3 4 5 6 7 8 9)

src/Main/Release/DartThrowing $dimension $num_classes $priority $r_values $k_value $domain_size > data/poisson_${dimension}d_${num_classes}c_${r_text}_${i}.txt

src/Main/Release/SampleSelector data/poisson_${dimension}d_${num_classes}c_${r_text}_${i}.txt ${dimension} ${ids} > data/poisson_${dimension}d_${num_classes}c_${r_text}_${i}_selected.txt

#src/Main/Release/LloydRelaxation2D data/poisson_${dimension}d_${num_classes}c_${r_text}_${i}.txt ${dimension} ${domain_size} ${num_relaxations} > data/poisson_${dimension}d_${num_classes}c_${r_text}_${num_relaxations}e_${i}.txt

../BlueNoise/src/Main/Release/SFT data/poisson_${dimension}d_${num_classes}c_${r_text}_${i}_selected.txt data/poisson_${dimension}d_${num_classes}c_${r_text}_${i}_selected.pfm ${dimension} 1 1 512 512

set all_pfm_files = "$all_pfm_files data/poisson_${dimension}d_${num_classes}c_${r_text}_${i}_selected.pfm";
@ all_pfm_files_count = ${all_pfm_files_count} + 1;
end

echo ${all_pfm_files} > data/all_pfm_files_list.txt;
src/Sain/Release/RadialPlot data/all_pfm_files_list.txt data/poisson_${dimension}d_${num_classes}c_${r_text}_selected_average${all_pfm_files_count}.pfm data/poisson_${dimension}d_${num_classes}c_${r_text}_selected_average${all_pfm_files_count}_mean.txt data/poisson_${dimension}d_${num_classes}c_${r_text}_selected_average${all_pfm_files_count}_variance.txt
rm data/all_pfm_files_list.txt;

../BlueNoise/src/Main/Release/PFM2PPM data/poisson_${dimension}d_${num_classes}c_${r_text}_selected_average${all_pfm_files_count}.pfm  data/poisson_${dimension}d_${num_classes}c_${r_text}_selected_average${all_pfm_files_count}.ppm 1 1 1

endif

# relationship with spectrum and number of classes
if(0) then

set dimension = 2;

set ids = 0;
set r_value = 0.02;
set r_text = 0p02;

set target_samples = 1300;
set k_value = 100;
set domain_size = "1 1";

set sft_size = 512;

foreach num_classes (13 17) #(1 2 4 8 9 16 32 64 128 256)
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

if(${num_classes} == 4) then
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

set all_pfm_files_selected = "";
set all_pfm_files_all = "";
set all_pfm_files_c1 = "";
set all_pfm_files_count = 0;

foreach i (0 1 2 3 4 5 6 7 8 9)

echo ${num_classes} class dart throwing

src/Main/Release/DartThrowing $dimension $num_classes $priority $r_values $k_value $domain_size > data/poisson_${dimension}d_${num_classes}c_${r_text}_${i}.txt

src/Main/Release/SampleSelector data/poisson_${dimension}d_${num_classes}c_${r_text}_${i}.txt ${dimension} ${ids} > data/poisson_${dimension}d_${num_classes}c_${r_text}_${i}_selected.txt

#src/Main/Release/LloydRelaxation2D data/poisson_${dimension}d_${num_classes}c_${r_text}_${i}.txt ${dimension} ${domain_size} ${num_relaxations} > data/poisson_${dimension}d_${num_classes}c_${r_text}_${num_relaxations}e_${i}.txt

../BlueNoise/src/Main/Release/SFT data/poisson_${dimension}d_${num_classes}c_${r_text}_${i}_selected.txt data/poisson_${dimension}d_${num_classes}c_${r_text}_${i}_selected.pfm ${dimension} 1 1 ${sft_size} ${sft_size}

src/Main/Release/SampleSelector data/poisson_${dimension}d_${num_classes}c_${r_text}_${i}.txt ${dimension} ${all_ids} > data/poisson_${dimension}d_${num_classes}c_${r_text}_${i}_all.txt

../BlueNoise/src/Main/Release/SFT data/poisson_${dimension}d_${num_classes}c_${r_text}_${i}_all.txt data/poisson_${dimension}d_${num_classes}c_${r_text}_${i}_all.pfm ${dimension} 1 1 ${sft_size} ${sft_scale_all}

echo 1 class dart throwing

src/Main/Release/DartThrowing $dimension 1 0 $c1_r_value $k_value $domain_size > data/poisson_${dimension}d_1c_${c1_r_text}_${i}.txt

src/Main/Release/SampleSelector data/poisson_${dimension}d_1c_${c1_r_text}_${i}.txt ${dimension} 0 > data/poisson_${dimension}d_1c_${c1_r_text}_${i}_selected.txt

../BlueNoise/src/Main/Release/SFT data/poisson_${dimension}d_1c_${c1_r_text}_${i}_selected.txt data/poisson_${dimension}d_1c_${c1_r_text}_${i}_selected.pfm ${dimension} 1 1 ${sft_size} ${sft_scale_all}

set all_pfm_files_selected = "$all_pfm_files_selected data/poisson_${dimension}d_${num_classes}c_${r_text}_${i}_selected.pfm";
set all_pfm_files_all = "$all_pfm_files_all data/poisson_${dimension}d_${num_classes}c_${r_text}_${i}_all.pfm";
set all_pfm_files_c1 = "$all_pfm_files_c1 data/poisson_${dimension}d_1c_${c1_r_text}_${i}_selected.pfm";
@ all_pfm_files_count = ${all_pfm_files_count} + 1;
end

echo ${all_pfm_files_selected} > data/all_pfm_files_selected_list.txt;
src/Sain/Release/RadialPlot data/all_pfm_files_selected_list.txt data/poisson_${dimension}d_${num_classes}c_${r_text}_selected_average${all_pfm_files_count}.pfm data/poisson_${dimension}d_${num_classes}c_${r_text}_selected_average${all_pfm_files_count}_mean.txt data/poisson_${dimension}d_${num_classes}c_${r_text}_selected_average${all_pfm_files_count}_variance.txt
rm data/all_pfm_files_selected_list.txt;

../BlueNoise/src/Main/Release/PFM2PPM data/poisson_${dimension}d_${num_classes}c_${r_text}_selected_average${all_pfm_files_count}.pfm  data/poisson_${dimension}d_${num_classes}c_${r_text}_selected_average${all_pfm_files_count}.ppm 1 1 1

echo ${all_pfm_files_all} > data/all_pfm_files_all_list.txt;
src/Sain/Release/RadialPlot data/all_pfm_files_all_list.txt data/poisson_${dimension}d_${num_classes}c_${r_text}_all_average${all_pfm_files_count}.pfm data/poisson_${dimension}d_${num_classes}c_${r_text}_all_average${all_pfm_files_count}_mean.txt data/poisson_${dimension}d_${num_classes}c_${r_text}_all_average${all_pfm_files_count}_variance.txt
rm data/all_pfm_files_all_list.txt;

../BlueNoise/src/Main/Release/PFM2PPM data/poisson_${dimension}d_${num_classes}c_${r_text}_all_average${all_pfm_files_count}.pfm  data/poisson_${dimension}d_${num_classes}c_${r_text}_all_average${all_pfm_files_count}.ppm 1 1 1

echo ${all_pfm_files_c1} > data/all_pfm_files_c1_list.txt;
src/Sain/Release/RadialPlot data/all_pfm_files_c1_list.txt data/poisson_${dimension}d_${num_classes}c_${r_text}_c1_average${all_pfm_files_count}.pfm data/poisson_${dimension}d_${num_classes}c_${r_text}_c1_average${all_pfm_files_count}_mean.txt data/poisson_${dimension}d_${num_classes}c_${r_text}_c1_average${all_pfm_files_count}_variance.txt
rm data/all_pfm_files_c1_list.txt;

../BlueNoise/src/Main/Release/PFM2PPM data/poisson_${dimension}d_${num_classes}c_${r_text}_c1_average${all_pfm_files_count}.pfm  data/poisson_${dimension}d_${num_classes}c_${r_text}_c1_average${all_pfm_files_count}.ppm 1 1 1

end #foreach num_classes

endif

# matching white noise for above
if(0) then

set dimension = 2;
set target_samples = 1300;

set all_pfm_files = "";
set all_pfm_files_count = 0;

foreach i (0 1 2 3 4 5 6 7 8 9)
 ../BlueNoise/src/Main/Release/RandomDart ${dimension} ${target_samples} > data/white_${dimension}d_${target_samples}s_${i}.txt;

../BlueNoise/src/Main/Release/SFT data/white_${dimension}d_${target_samples}s_${i}.txt data/white_${dimension}d_${target_samples}s_${i}.pfm ${dimension} 1 1 512 512;

set all_pfm_files = "$all_pfm_files data/white_${dimension}d_${target_samples}s_${i}.pfm";
@ all_pfm_files_count = ${all_pfm_files_count} + 1;
end

echo ${all_pfm_files} > data/all_pfm_files_list.txt;
src/Sain/Release/RadialPlot data/all_pfm_files_list.txt data/white_${dimension}d_${target_samples}s_average${all_pfm_files_count}.pfm data/white_${dimension}d_${target_samples}s_average${all_pfm_files_count}_mean.txt data/white_${dimension}d_${target_samples}s_average${all_pfm_files_count}_variance.txt
rm data/all_pfm_files_list.txt;

../BlueNoise/src/Main/Release/PFM2PPM data/white_${dimension}d_${target_samples}s_average${all_pfm_files_count}.pfm data/white_${dimension}d_${target_samples}s_average${all_pfm_files_count}.ppm 1 1 1

endif

# non-uniform r values
if(0) then

set dimension = 2;

if(0) then # too complex for analysis
set num_classes = 3;
set priority = "0 0 0";
set p_text = 000;
set r_values = "0.01 0.02 0.04";
set r_text = 0p010p020p04;
set target_samples = 2400;
endif

if(0) then # equivalent to above with respect to the dominating class
set num_classes = 2;
set priority = "0 0";
set p_text = 00;
set r_values = "0.01 0.0178885438";
set r_text = 0p010p0178885438;
set target_samples = 3600;
endif

if(1) then # more optimal r-matrix design
set num_classes = 3;
set priority = "2 1 0";
set p_text = 210;

if(1) then
set priority = "0 0 0";
set p_text = 000;
endif

set r_values = "0.01 0.00872871561 0.00872871561 0.02 0.0178885438 0.04";
set r_text = 0p010p020p04full;
set target_samples = -2400;
endif

set k_value = 100;
set domain_size = "1 1";

set num_runs = 10;

set sft_size = 512;

if(${target_samples} > 0) then
@ k_value = ${target_samples} * ${num_classes} * -1;
endif

set i = 0;
while(${i} < ${num_runs})
echo src/Main/Release/DartThrowing $dimension $num_classes $priority $r_values $k_value $domain_size to data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}.txt
src/Main/Release/DartThrowing $dimension $num_classes $priority $r_values $k_value $domain_size > data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}.txt
@ i = ${i} + 1;
end

set id = 0;

while(${id} < ${num_classes})

set all_pfm_files = "";
set all_pfm_files_count = 0;

set i = 0;
while(${i} < ${num_runs})

echo data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}_selected${id}.txt

src/Main/Release/SampleSelector data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}.txt ${dimension} ${id} > data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}_selected${id}.txt

../BlueNoise/src/Main/Release/SFT data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}_selected${id}.txt data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}_selected${id}.pfm ${dimension} 1 1 ${sft_size} ${sft_size} 

set all_pfm_files = "$all_pfm_files data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}_selected${id}.pfm";
@ all_pfm_files_count = ${all_pfm_files_count} + 1;

@ i = ${i} + 1;
end

echo ${all_pfm_files} > data/all_pfm_files_list.txt;
src/Sain/Release/RadialPlot data/all_pfm_files_list.txt data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_selected${id}_average${all_pfm_files_count}.pfm data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_selected${id}_average${all_pfm_files_count}_mean.txt data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_selected${id}_average${all_pfm_files_count}_variance.txt
rm data/all_pfm_files_list.txt;

../BlueNoise/src/Main/Release/PFM2PPM data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_selected${id}_average${all_pfm_files_count}.pfm  data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_selected${id}_average${all_pfm_files_count}.ppm 1 1 1

@ id = ${id} + 1;
end

endif

# 2 classes with different r values
if(0) then

set dimension = 2;
set num_classes = 2;
set priority = "0 0";
set p_text = 00;
set target_samples = 5200; # computed/estimated by the k_value below

set k_value = 100;
set k_text = ${k_value};
set rho_value = 0.671069;
set rho_text = 0p671069;
set domain_size = "1 1";

set num_runs = 10;

set sft_size = 512;

foreach k_option (0) #(0 1)
foreach option (0 1 2 3 4 5 6)

if($option == 0) then
set r_values = "0.01 0.01";
set r_text = 0p01m0p01osqrt2;
else if($option == 1) then
set r_values = "0.02 0.00755929";
set r_text = 0p02m0p01osqrt2;
else if($option == 2) then
set r_values = "0.04 0.00718421";
set r_text = 0p04m0p01osqrt2;
else if($option == 3) then
set r_values = "0.08 0.00709885";
set r_text = 0p08m0p01osqrt2;
else if($option == 4) then
set r_values = "0.16 0.00707798";
set r_text = 0p16m0p01osqrt2;
else if($option == 5) then
set r_values = "0.011 0.00923099";
set r_text = 0p011m0p01osqrt2;
else if($option == 6) then
set r_values = "0.0141421356 0.00816497";
set r_text = 0p01sqrt2m0p01osqrt2;
endif

if(${k_option} == 0) then
set k_value = ${rho_value};
set k_text = ${rho_text};
else
if(${target_samples} > 0) then
@ k_value = ${target_samples} * ${num_classes} * -1;
set k_text = ${target_samples};
@ k_text = -${k_value}
endif
endif

set k_text = ${k_text}k;

set i = 0;
while(${i} < ${num_runs})
echo src/Main/Release/TreeThrowing $dimension $num_classes $priority $r_values $k_value $domain_size to data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${k_text}_${i}.txt
src/Main/Release/TreeThrowing $dimension $num_classes $priority $r_values $k_value $domain_size > data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${k_text}_${i}.txt
@ i = ${i} + 1;
end

set id = 0;

while(${id} < ${num_classes})

set all_pfm_files = "";
set all_pfm_files_count = 0;

set i = 0;
while(${i} < ${num_runs})

echo data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${k_text}_${i}_selected${id}.txt

src/Main/Release/SampleSelector data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${k_text}_${i}.txt ${dimension} ${id} > data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${k_text}_${i}_selected${id}.txt

../BlueNoise/src/Main/Release/SFT data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${k_text}_${i}_selected${id}.txt data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${k_text}_${i}_selected${id}.pfm ${dimension} 1 1 ${sft_size} ${sft_size} 

set all_pfm_files = "$all_pfm_files data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${k_text}_${i}_selected${id}.pfm";
@ all_pfm_files_count = ${all_pfm_files_count} + 1;

@ i = ${i} + 1;
end

echo ${all_pfm_files} > data/all_pfm_files_list.txt;
src/Sain/Release/RadialPlot data/all_pfm_files_list.txt data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${k_text}_selected${id}_average${all_pfm_files_count}.pfm data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${k_text}_selected${id}_average${all_pfm_files_count}_mean.txt data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${k_text}_selected${id}_average${all_pfm_files_count}_variance.txt
rm data/all_pfm_files_list.txt;

../BlueNoise/src/Main/Release/PFM2PPM data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${k_text}_selected${id}_average${all_pfm_files_count}.pfm  data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${k_text}_selected${id}_average${all_pfm_files_count}.ppm 1 1 1

@ id = ${id} + 1;
end

end # foreach option
end # foreach k_option

endif

# ground truth, match up *2 classes with different r values* above
if(0) then

set dimension = 2;
set num_classes = 1;
set priority = "0";
set p_text = 0;
set k_value = 100;
set k_text = ${k_value};
set rho_value = 0.671069;
set rho_text = 0p671069;
set domain_size = "1 1";

set num_runs = 10;

set sft_size = 512;

foreach k_option (0) #(0 1)

set target_samples = 5200; # will be computed/estimated by the k_value below

if(${k_option} == 0) then
set k_value = ${rho_value};
set k_text = ${rho_text};
else
if(${target_samples} > 0) then
@ k_value = ${target_samples} * ${num_classes} * -1;
set k_text = ${target_samples};
@ k_text = -${k_value}
endif
endif

set k_text = ${k_text}k;

foreach option (1 2 3 4 5 6)
foreach side (0 1)

if($option == 0) then
set r_values = "0.01";
set target_samples = 5200;
set r_text = 0p01m0p01osqrt2;
else if($option == 1) then
if($side == 0) then
set r_values = "0.02";
if(${k_option} == 0) then
set target_samples = 1300;
else
set target_samples = 1346.2;
endif
else
set r_values =  "0.00755929";
if(${k_option} == 0) then
set target_samples = 9100;
else
set target_samples = 9053.8;
endif
endif
set r_text = 0p02m0p01osqrt2;
else if($option == 2) then
if($side == 0) then
set r_values = "0.04";
if(${k_option} == 0) then
set target_samples = 325;
else
set target_samples = 380.1;
endif
else
set r_values = "0.00718421";
if(${k_option} == 0) then
set target_samples = 10075;
else
set target_samples = 10019.9;
endif
endif
set r_text = 0p04m0p01osqrt2;
else if($option == 3) then
if($side == 0) then
set r_values = "0.08";
if(${k_option} == 0) then
set target_samples = 81;
else
set target_samples = 100.6;
endif
else
set r_values = "0.00709885";
if(${k_option} == 0) then
set target_samples = 10319;
else
set target_samples = 10299.4;
endif
endif
set r_text = 0p08m0p01osqrt2;
else if($option == 4) then
if($side == 0) then
set r_values = "0.16";
if(${k_option} == 0) then
set target_samples = 20;
else
set target_samples = 26.2;
endif
else
set r_values = "0.00707798";
if(${k_option} == 0) then
set target_samples = 10380;
else
set target_samples = 10373.8;
endif
endif
set r_text = 0p16m0p01osqrt2;
else if($option == 5) then
if($side == 0) then
set r_values = "0.011";
if(${k_option} == 0) then
set target_samples = 4297;
else
set target_samples = 4176.4;
endif
else
set r_values = "0.00923099";
if(${k_option} == 0) then
set target_samples = 6102;
else
set target_samples = 6223.6;
endif
endif
set r_text = 0p011m0p01osqrt2;
else if($option == 6) then
if($side == 0) then
set r_values = "0.0141421356";
if(${k_option} == 0) then
set target_samples = 2600;
else
set target_samples = 2553.9;
endif
else
set r_values = "0.00816497";
if(${k_option} == 0) then
set target_samples = 7799;
else
set target_samples = 7846.1;
endif
endif
set r_text = 0p01sqrt2m0p01osqrt2;
endif

if(0) then
if(${target_samples} > 0) then
@ k_value = -${target_samples} * ${num_classes} * -1;
endif
else
set k_value = -${target_samples};
endif

set id = 0;

set all_pfm_files_selected = "";
set all_pfm_files_count = 0;

set i = 0;
while(${i} < ${num_runs})

echo data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${k_text}_${i}.txt

src/Main/Release/DartThrowing $dimension $num_classes $priority $r_values $k_value $domain_size > data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${k_text}_${i}.txt

src/Main/Release/SampleSelector data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${k_text}_${i}.txt ${dimension} ${id} > data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${k_text}_${i}_selected${id}.txt

../BlueNoise/src/Main/Release/SFT data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${k_text}_${i}_selected${id}.txt data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${k_text}_${i}_selected${id}.pfm ${dimension} 1 1 ${sft_size} ${sft_size} 

set all_pfm_files_selected = "$all_pfm_files_selected data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${k_text}_${i}_selected${id}.pfm";
@ all_pfm_files_count = ${all_pfm_files_count} + 1;

@ i = ${i} + 1;
end

echo ${all_pfm_files_selected} > data/all_pfm_files_selected_list.txt;
src/Sain/Release/RadialPlot data/all_pfm_files_selected_list.txt data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${k_text}_selected${id}_average${all_pfm_files_count}.pfm data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${k_text}_selected${side}_average${all_pfm_files_count}_mean.txt data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${k_text}_selected${side}_average${all_pfm_files_count}_variance.txt
rm data/all_pfm_files_selected_list.txt;

end # foreach side
end # foreach option
end # foreach k_option

endif

# sft1d and histograms
if(0) then

set outputdir = data;
set hugedir = data/huge;

set regenerate_samples = 0;

foreach distribution ('poisson' 'white') #('poisson' 'white') # 'poisson' or 'white'

set dimension = 2;

set num_classes = 1;
set priority = 0;
set ids = 0;

if(${dimension} == 1) then
set target_num_samples = 36;
else if(${dimension} == 2) then
set target_num_samples = 1680;
endif

set k_value = 100;

set r_value = 0.02;
set r_text = 0p02

set which_dimension = "0 1";
set which_dimension_text = "01";

set which_dimension_domain_size = 1;
set num_bins = 800;
set y_max = 0.008;
set max_frequency = 256;
set num_runs = 1280; # 2560;

set hist_freq = 1; # 50 75 100 125), the rest not seem useful at all

set all_sample_files = "";
set all_sample_files_count = 0;

set domain_size = "";
set i = 0;
while(${i} < ${dimension})
set domain_size = "$domain_size $which_dimension_domain_size";
@ i = ${i} + 1;
end

set i = 0;
while(${i} < ${num_runs})

set sample_file_name = ${hugedir}/${distribution}_${dimension}d_${num_classes}c_${r_text}_${i}.txt;

if(${regenerate_samples}) then
if(${distribution} == 'poisson') then

if($target_num_samples > 0) then
#set k_value = -$target_num_samples;
endif

src/Main/Release/DartThrowing $dimension $num_classes $priority $r_value $k_value $domain_size > ${sample_file_name}
else
../BlueNoise/src/Main/Release/RandomDart ${dimension} ${target_num_samples} > ${sample_file_name} 
endif
endif

set all_sample_files = "$all_sample_files ${sample_file_name}";
@ all_sample_files_count = ${all_sample_files_count} + 1;

@ i = ${i} + 1;
end

echo ${all_sample_files} > ${outputdir}/all_sample_file_list.txt;
if(0) then
echo compute ${outputdir}/${distribution}_${dimension}d_${num_classes}c_${r_text}_sft1d
src/Main/Release/Math "sft1dsamples" ${outputdir}/all_sample_file_list.txt $dimension $which_dimension $max_frequency > ${outputdir}/${distribution}_${dimension}d_${num_classes}c_${r_text}_sft1d.txt;
endif
if(1) then
echo compute ${outputdir}/${distribution}_${dimension}d_${num_classes}c_${r_text}_${which_dimension_text}_histogram
src/Theory/Release/Histogram ${outputdir}/all_sample_file_list.txt $dimension $which_dimension ${hist_freq} $num_bins > ${outputdir}/${distribution}_${dimension}d_${num_classes}c_${r_text}_${which_dimension_text}_histogram.txt
endif
rm ${outputdir}/all_sample_file_list.txt;

# cleanup
if(0) then
set i = 0;
while(${i} < ${num_runs})
rm ${hugedir}/${distribution}_${dimension}d_${num_classes}c_${r_text}_${i}.txt;
@ i = ${i} + 1;
end
endif

# plot SFT1D
if(0) then
echo "" > foo.gp;

echo set terminal postscript eps >> foo.gp
echo set output \'${outputdir}/${distribution}_${dimension}d_${num_classes}c_${r_text}_sft1d.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp
echo set autoscale y >> foo.gp
set max_bin = "";
@ max_bin = $max_frequency - 1;
echo plot "[1:${max_bin}]" \"${outputdir}/${distribution}_${dimension}d_${num_classes}c_${r_text}_sft1d.txt\" with line linetype rgb \"red\" >> foo.gp

wgnuplot foo.gp
rm foo.gp;
endif

# plot histogram
if(0) then
echo "" > foo.gp;

echo set terminal postscript eps >> foo.gp
echo set output \'${outputdir}/${distribution}_${dimension}d_${num_classes}c_${r_text}_${which_dimension_text}_histogram.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"abs\(coordinate diff\) mod r\" >> foo.gp
echo set ylabel \"histogram\" >> foo.gp
echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
set max_bin = "";
@ max_bin = $num_bins - 1;
echo plot "[0:${r_value}] [0:${y_max}]" \"${outputdir}/${distribution}_${dimension}d_${num_classes}c_${r_text}_${which_dimension_text}_histogram.txt\" with line linetype rgb \"red\" >> foo.gp

wgnuplot foo.gp
rm foo.gp;
endif

end #foreach distribution

# plot histogram with reference white noise
if(1) then

set output_name = ${outputdir}/poisson_white_${dimension}d_${num_classes}c_${r_text}_${which_dimension_text}_histogram_freq${hist_freq};
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
echo plot "[0:1]" \"${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}_${which_dimension_text}_histogram.txt\" title \'Poisson disk\' with lines linetype rgb \"red\", \"${outputdir}/white_${dimension}d_${num_classes}c_${r_text}_${which_dimension_text}_histogram.txt\"  title \'uniform random\' with lines linetype rgb \"blue\" >> foo.gp

wgnuplot foo.gp
rm foo.gp;

epstopdf ${output_name}.eps;
cp ${output_name}.pdf foo.pdf
ps2pdf13 -dPDFSETTINGS=/prepress foo.pdf ${output_name}.pdf;

endif

endif

# matching power spectrum profile for above
if(0) then

foreach distribution ('poisson' 'white') # 'poisson' or 'white'

set dimension = 2;

set num_classes = 1;
set priority = 0;
set ids = 0;
set target_samples = 1680;

set k_value = 100;
set r_value = 0.02;
set r_text = 0p02

set domain_size = "1 1";

set num_runs = 10;

set sft_size = 512;

if(${target_samples} > 0) then
@ k_value = ${target_samples} * ${num_classes} * -1;
endif

set all_pfm_files = "";
set all_pfm_files_count = 0;

set i = 0;
set id = 0;
while(${i} < ${num_runs})

set sample_file_name = data/${distribution}_${dimension}d_${num_classes}c_${r_text}_${i}.txt;

echo data/${distribution}_${dimension}d_${num_classes}c_${r_text}_${i}.txt

if(${distribution} == 'poisson') then
src/Main/Release/DartThrowing $dimension $num_classes $priority $r_value $k_value $domain_size > ${sample_file_name}
else
../BlueNoise/src/Main/Release/RandomDart ${dimension} ${target_samples} > ${sample_file_name} 
endif

echo data/${distribution}_${dimension}d_${num_classes}c_${r_text}_${i}_selected.txt

src/Main/Release/SampleSelector data/${distribution}_${dimension}d_${num_classes}c_${r_text}_${i}.txt ${dimension} ${id} > data/${distribution}_${dimension}d_${num_classes}c_${r_text}_${i}_selected.txt

../BlueNoise/src/Main/Release/SFT data/${distribution}_${dimension}d_${num_classes}c_${r_text}_${i}_selected.txt data/${distribution}_${dimension}d_${num_classes}c_${r_text}_${i}_selected.pfm ${dimension} 1 1 ${sft_size} ${sft_size} 

set all_pfm_files = "$all_pfm_files data/${distribution}_${dimension}d_${num_classes}c_${r_text}_${i}_selected.pfm";
@ all_pfm_files_count = ${all_pfm_files_count} + 1;

@ i = ${i} + 1;
end

echo ${all_pfm_files} > data/all_pfm_files_list.txt;
src/Sain/Release/RadialPlot data/all_pfm_files_list.txt data/${distribution}_${dimension}d_${num_classes}c_${r_text}_selected_average${all_pfm_files_count}.pfm data/${distribution}_${dimension}d_${num_classes}c_${r_text}_selected_average${all_pfm_files_count}_mean.txt data/${distribution}_${dimension}d_${num_classes}c_${r_text}_selected_average${all_pfm_files_count}_variance.txt
rm data/all_pfm_files_list.txt;

#../BlueNoise/src/Main/Release/PFM2PPM data/${distribution}_${dimension}d_${num_classes}c_${r_text}_selected_average${all_pfm_files_count}.pfm data/${distribution}_${dimension}d_${num_classes}c_${r_text}_selected_average${all_pfm_files_count}.ppm 1 1 1

./src/Scripts/txt2dat data/${distribution}_${dimension}d_${num_classes}c_${r_text}_selected_average${all_pfm_files_count}_mean.txt data/${distribution}_${dimension}d_${num_classes}c_${r_text}_selected_average${all_pfm_files_count}_variance.txt ${target_samples} data/${distribution}_${dimension}d_${num_classes}c_${r_text}_selected_average${all_pfm_files_count}_mean.dat data/${distribution}_${dimension}d_${num_classes}c_${r_text}_selected_average${all_pfm_files_count}_variance.dat

end #foreach distribution

# plot stuff
if(1) then

echo plot data/poisson_white_${dimension}d_${num_classes}c_${r_text}_mean.eps

echo "" > foo.gp;

echo set terminal postscript eps >> foo.gp
echo set output \'data/poisson_white_${dimension}d_${num_classes}c_${r_text}_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
#echo set key off >> foo.gp
echo set autoscale y >> foo.gp
echo plot "[1:256]" \"data/poisson_${dimension}d_${num_classes}c_${r_text}_selected_average${all_pfm_files_count}_mean.dat\" title \'Poisson disk\' with lines linetype rgb \"red\", \"data/white_${dimension}d_${num_classes}c_${r_text}_selected_average${all_pfm_files_count}_mean.dat\"  title \'uniform random\' with lines linetype rgb \"blue\" >> foo.gp

wgnuplot foo.gp
rm foo.gp;
endif

endif


# histogram for 2 classes
if(0) then

foreach distribution ('poisson' 'white' 'discreterelax' 'discreterandom') # 'poisson' and 'white' must proceed all others

set dimension = 2;

set num_classes = 2;
set priority = "0 0";
set ids = 0;

set target_num_samples = -2600;

set k_value = 0.68;

set r_values = "0.04 0.01511858";
set r_text = 0p04m0p02osqrt2;

set num_continuous_iterations = 10;
set num_discrete_iterations = 5;
set relaxation_id = 0;

#set which_dimension = 0;
set which_dimension_domain_size = 1;
set num_bins = 800;
set num_runs = 100; #1280;

set hist_freq = 1; # 50 75 100 125), the rest not seem useful at all

set all_sample_files_0 = "";
set all_sample_files_1 = "";
set all_sample_files_count = 0;

set domain_size = "";
set i = 0;
while(${i} < ${dimension})
set domain_size = "$domain_size $which_dimension_domain_size";
@ i = ${i} + 1;
end

set i = 0;
while(${i} < ${num_runs})

set sample_file_name = data/huge/${distribution}_${dimension}d_${num_classes}c_${r_text}_${i}.txt;
set poisson_file_name = data/huge/poisson_${dimension}d_${num_classes}c_${r_text}_${i}.txt;

set sample_file_name_0 = data/huge/${distribution}_${dimension}d_${num_classes}c_${r_text}_${i}_selected0.txt;
set sample_file_name_1 = data/huge/${distribution}_${dimension}d_${num_classes}c_${r_text}_${i}_selected1.txt;

if(1) then

if(${distribution} == 'poisson') then

if($target_num_samples > 0) then
set k_value = -$target_num_samples;
endif

src/Main/Release/DartThrowing $dimension $num_classes $priority $r_values $k_value $domain_size > ${sample_file_name}

sleep 1;

# white
foreach id (0 1)
set all_samples = `src/Main/Release/SampleSelector ${sample_file_name} ${dimension} ${id}`;

@ num_samples = $#all_samples / ${dimension};
../BlueNoise/src/Main/Release/RandomDart ${dimension} ${num_samples} > data/huge/white_${dimension}d_${num_classes}c_${r_text}_${i}_selected${id}.txt;
end # foreach id

else if(${distribution} == 'discreterelax') then

src/Main/Release/DiscreteRelaxSamples ${poisson_file_name} ${dimension} ${domain_size} ${num_continuous_iterations} ${num_discrete_iterations} ${relaxation_id} 0 > ${sample_file_name}

else if(${distribution} == 'discreterandom') then

src/Main/Release/RandomizeSamples ${dimension} ${poisson_file_name} > ${sample_file_name}

else if(${distribution} == 'white') then
#echo white is handled with poisson;
#exit(1);
else
echo unknown distribution;
exit(1);
endif

if(${distribution} != 'white') then
foreach id (0 1)
grep "^${id}" ${sample_file_name} > data/huge/${distribution}_${dimension}d_${num_classes}c_${r_text}_${i}_selected${id}.txt;
end
endif

endif

set all_sample_files_0 = "$all_sample_files_0 ${sample_file_name_0}";
set all_sample_files_1 = "$all_sample_files_1 ${sample_file_name_1}";
@ all_sample_files_count = ${all_sample_files_count} + 1;

@ i = ${i} + 1;
end

echo ${all_sample_files_0} > data/all_sample_files_0_list.txt;
echo ${all_sample_files_1} > data/all_sample_files_1_list.txt;
if(1) then
foreach which_dimension_text ('0' '01')
if(${which_dimension_text} == '0') then
set which_dimension = '0';
else
set which_dimension = '0 1';
endif

foreach id (0 1)
echo compute data/${distribution}_${dimension}d_${num_classes}c_${r_text}_selected${id}_${which_dimension_text}_histogram
src/Theory/Release/Histogram data/all_sample_files_${id}_list.txt $dimension $which_dimension ${hist_freq} $num_bins > data/${distribution}_${dimension}d_${num_classes}c_${r_text}_selected${id}_${which_dimension_text}_histogram.txt
end

end # foreach which_dimension_text
endif
rm data/all_sample_files_0_list.txt;
rm data/all_sample_files_1_list.txt;

# cleanup
if(0) then
set i = 0;
while(${i} < ${num_runs})
rm data/${distribution}_${dimension}d_${num_classes}c_${r_text}_${i}.txt;
foreach id (0 1)
rm data/${distribution}_${dimension}d_${num_classes}c_${r_text}_${i}_selected${id}.txt;
end
@ i = ${i} + 1;
end
endif

# plot histogram
if(1) then

foreach which_dimension_text ('0' '01')

if(${which_dimension_text} == '0') then
set x_max = 1;
else
set x_max = 1.41245;
endif

foreach id (0 1)

foreach normalization (0 1)

if(${normalization}) then
set histogram = 'histogram_normalized';
src/Scripts/normalize_hist data/${distribution}_${dimension}d_${num_classes}c_${r_text}_selected${id}_${which_dimension_text}_histogram.txt data/white_${dimension}d_${num_classes}c_${r_text}_selected${id}_${which_dimension_text}_histogram.txt data/${distribution}_${dimension}d_${num_classes}c_${r_text}_selected${id}_${which_dimension_text}_histogram_normalized.txt
else
set histogram = 'histogram';
endif

echo plot data/${distribution}_white_${dimension}d_${num_classes}c_${r_text}_selected${id}_${which_dimension_text}_${histogram}_freq${hist_freq}.eps

echo "" > foo.gp;

echo set terminal postscript eps >> foo.gp
echo set output \'data/${distribution}_white_${dimension}d_${num_classes}c_${r_text}_selected${id}_${which_dimension_text}_${histogram}_freq${hist_freq}.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"\|coordinate difference\|\" >> foo.gp
if(${normalization}) then
echo set ylabel \"normalized density\" >> foo.gp
else
echo set ylabel \"density\" >> foo.gp
endif
#echo set key off >> foo.gp
echo set autoscale y >> foo.gp
set max_bin = "";
@ max_bin = $num_bins - 1;
if(${normalization}) then
echo plot "[0:${x_max}]" \"data/${distribution}_${dimension}d_${num_classes}c_${r_text}_selected${id}_${which_dimension_text}_${histogram}.txt\" title \'${distribution}\' with lines linetype rgb \"red\" >> foo.gp
else
echo plot "[0:${x_max}]" \"data/${distribution}_${dimension}d_${num_classes}c_${r_text}_selected${id}_${which_dimension_text}_${histogram}.txt\" title \'${distribution}\' with lines linetype rgb \"red\", \"data/white_${dimension}d_${num_classes}c_${r_text}_selected${id}_${which_dimension_text}_${histogram}.txt\" title \'uniform random\' with lines linetype rgb \"\#009900\" >> foo.gp
endif
#echo plot "[t=0:1]" \"data/${distribution}_${dimension}d_${num_classes}c_${r_text}_selected${id}_${which_dimension_text}_histogram.txt\" title \'Poisson disk\' with lines linetype rgb \"red\", "2*(1-t)" title \'uniform random\' with lines linetype rgb \"blue\" >> foo.gp
#, \"data/white_${dimension}d_${num_classes}c_${r_text}_selected${id}_histogram.txt\"  title \'uniform random\' with lines linetype rgb \"blue\"
wgnuplot foo.gp
rm foo.gp;

end # foreach normalization
end # foreach id
end # foreach which_dimension_text

end #foreach distribution

endif

endif

# plot radial mean for above
if(0) then

set dimension = 2;

set num_classes = 2;
set priority = "0 0";
set ids = 0;

set r_values = "0.04 0.01511858";
set r_text = 0p04m0p02osqrt2;

set sft_size = 512;

set num_runs = 10;

foreach distribution ('poisson' 'discreterelax' 'discreterandom') # 'poisson' must proceed all others
set id = 0;

while(${id} < ${num_classes})

set all_pfm_files = "";
set all_pfm_files_count = 0;

set i = 0;
while(${i} < ${num_runs})

echo data/huge/${distribution}_${dimension}d_${num_classes}c_${r_text}_${i}_selected${id}.txt

src/Main/Release/SampleSelector data/huge/${distribution}_${dimension}d_${num_classes}c_${r_text}_${i}_selected${id}.txt ${dimension} ${id} > data/huge/${distribution}_${dimension}d_${num_classes}c_${r_text}_${i}_selected${id}_selected.txt

../BlueNoise/src/Main/Release/SFT data/huge/${distribution}_${dimension}d_${num_classes}c_${r_text}_${i}_selected${id}_selected.txt data/huge/${distribution}_${dimension}d_${num_classes}c_${r_text}_${i}_selected${id}.pfm ${dimension} 1 1 ${sft_size} ${sft_size} 

set all_pfm_files = "$all_pfm_files data/huge/${distribution}_${dimension}d_${num_classes}c_${r_text}_${i}_selected${id}.pfm";
@ all_pfm_files_count = ${all_pfm_files_count} + 1;

@ i = ${i} + 1;
end

echo ${all_pfm_files} > data/all_pfm_files_list.txt;
src/Sain/Release/RadialPlot data/all_pfm_files_list.txt data/huge/${distribution}_${dimension}d_${num_classes}c_${r_text}_selected${id}_average${all_pfm_files_count}.pfm data/huge/${distribution}_${dimension}d_${num_classes}c_${r_text}_selected${id}_average${all_pfm_files_count}_mean.txt data/huge/${distribution}_${dimension}d_${num_classes}c_${r_text}_selected${id}_average${all_pfm_files_count}_variance.txt
rm data/all_pfm_files_list.txt;

../BlueNoise/src/Main/Release/PFM2PPM data/huge/${distribution}_${dimension}d_${num_classes}c_${r_text}_selected${id}_average${all_pfm_files_count}.pfm  data/huge/${distribution}_${dimension}d_${num_classes}c_${r_text}_selected${id}_average${all_pfm_files_count}.ppm 1 1 1

@ id = ${id} + 1;
end

end # foreach distribution

endif

# randomization of samples
if(0) then

set dimension = 2;
set num_classes = 2;
set priority = "0 0";
set p_text = 00;
set target_samples = 5200; # computed/estimated by the k_value below

set k_value = 100;
set domain_size = "1 1";

set num_runs = 10;

set sft_size = 512;

set r_values = "0.02 0.00755929";
set r_text = 0p02m0p01osqrt2;

set num_continuous_iterations = 15;
set num_discrete_iterations = 10;
set relaxation_id = 0;

foreach method (discreterelax) #(random discreterelax)

set i = 0;
while(${i} < ${num_runs})
#src/Main/Release/DartThrowing $dimension $num_classes $priority $r_values $k_value $domain_size > data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}.txt

set output_file_name = data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}_${method}.txt;

if(${method} == 'random') then
src/Main/Release/RandomizeSamples ${dimension} data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}.txt > ${output_file_name}
else if (${method} == 'discreterelax') then
src/Main/Release/DiscreteRelaxSamples data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}.txt ${dimension} ${domain_size} ${num_continuous_iterations} ${num_discrete_iterations} ${relaxation_id} 1 > ${output_file_name}
else

endif

@ i = ${i} + 1;
end

set id = 0;

while(${id} < ${num_classes})

set all_pfm_files = "";
set all_pfm_files_count = 0;

set i = 0;
while(${i} < ${num_runs})

echo data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}_${method}_selected${id}.txt

src/Main/Release/SampleSelector data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}_${method}.txt ${dimension} ${id} > data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}_${method}_selected${id}.txt

../BlueNoise/src/Main/Release/SFT data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}_${method}_selected${id}.txt data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}_${method}_selected${id}.pfm ${dimension} 1 1 ${sft_size} ${sft_size} 

set all_pfm_files = "$all_pfm_files data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}_${method}_selected${id}.pfm";
@ all_pfm_files_count = ${all_pfm_files_count} + 1;

@ i = ${i} + 1;
end

echo ${all_pfm_files} > data/all_pfm_files_list.txt;
src/Sain/Release/RadialPlot data/all_pfm_files_list.txt data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${method}_selected${id}_average${all_pfm_files_count}.pfm data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${method}_selected${id}_average${all_pfm_files_count}_mean.txt data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${method}_selected${id}_average${all_pfm_files_count}_variance.txt
rm data/all_pfm_files_list.txt;

../BlueNoise/src/Main/Release/PFM2PPM data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${method}_selected${id}_average${all_pfm_files_count}.pfm  data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${method}_selected${id}_average${all_pfm_files_count}.ppm 1 1 1

@ id = ${id} + 1;
end

end # foreach method
endif

# discrete relaxation of samples
if(0) then
echo this is not working
exit(1)

set dimension = 2;
set num_classes = 2;
set priority = "0 0";
set p_text = 00;
set target_samples = 5200; # computed/estimated by the k_value below

set k_value = 100;
set domain_size = "1 1";

set num_runs = 10;

set sft_size = 512;

set r_values = "0.02 0.00755929";
set r_text = 0p02m0p01osqrt2;

set num_discrete_iterations = 10;
set num_continuous_iterations = 10;

set id = 0;

while(${id} < ${num_classes})

set all_pfm_files = "";
set all_pfm_files_count = 0;

set i = 0;
while(${i} < ${num_runs})

echo data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}_discreterelax_selected${id}.txt

src/Main/Release/DiscreteRelaxSamples data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}.txt ${dimension} ${domain_size} ${num_continuous_iterations} ${num_discrete_iterations} ${id} 1 > data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}_discreterelax${id}.txt

src/Main/Release/SampleSelector data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}_discreterelax${id}.txt ${dimension} ${id} > data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}_discreterelax_selected${id}.txt

../BlueNoise/src/Main/Release/SFT data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}_discreterelax_selected${id}.txt data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}_discreterelax_selected${id}.pfm ${dimension} 1 1 ${sft_size} ${sft_size} 

set all_pfm_files = "$all_pfm_files data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}_discreterelax_selected${id}.pfm";
@ all_pfm_files_count = ${all_pfm_files_count} + 1;

@ i = ${i} + 1;
end

echo ${all_pfm_files} > data/all_pfm_files_list.txt;
src/Sain/Release/RadialPlot data/all_pfm_files_list.txt data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_discreterelax_selected${id}_average${all_pfm_files_count}.pfm data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_discreterelax_selected${id}_average${all_pfm_files_count}_mean.txt data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_discreterelax_selected${id}_average${all_pfm_files_count}_variance.txt
rm data/all_pfm_files_list.txt;

../BlueNoise/src/Main/Release/PFM2PPM data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_discreterelax_selected${id}_average${all_pfm_files_count}.pfm  data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_discreterelax_selected${id}_average${all_pfm_files_count}.ppm 1 1 1

@ id = ${id} + 1;
end

endif

# total sample set with 10400 target number of samples
if(0) then

set dimension = 2;

set target_samples = 10400;
set k_value = 100;

set domain_size = "1 1";

set num_runs = 10;

set sft_size = 512;

if(${target_samples} > 0) then
@ k_value = ${target_samples} * -1;
endif

foreach option (3 4 5 6)

if(${option} == 0) then
# ground truth with one class
set num_classes = 1;
set ids = "0";
set priority = "0";
set p_text = 0;
set r_values = "0.00707106781"; # 0.01/sqrt(2)
set r_text = 0p00707;
else if(${option} == 1) then
set num_classes = 8;
set ids = "0 1 2 3 4 5 6 7";
set priority = "0 0 0 0 0 0 0 0";
set p_text = 00000000;
set r_values = "0.02 0.02 0.02 0.02 0.02 0.02 0.02 0.02";
set r_text = 0p02;
else 

set num_classes = 2;
set ids = "0 1";
set priority = "0 0";
set p_text = 00;

if(${option} == 2) then
set r_values = "0.02 0.00755929";
set r_text = 0p02m0p01osqrt2;
else if($option == 3) then
set r_values = "0.04 0.00718421";
set r_text = 0p04m0p01osqrt2;
else if($option == 4) then
set r_values = "0.08 0.00709885";
set r_text = 0p08m0p01osqrt2;
else if($option == 5) then
set r_values = "0.011 0.00923099";
set r_text = 0p011m0p01osqrt2;
else if($option == 6) then
set r_values = "0.0141421356 0.00816497";
set r_text = 0p01sqrt2m0p01osqrt2;
else
echo unknown option;
exit(1);
endif

else
echo unknown option;
exit(1);
endif

set all_pfm_files = "";
set all_pfm_files_count = 0;

set i = 0;
while(${i} < ${num_runs})

# dart throwing
if((${option} == 0) || (${option} == 1)) then
echo src/Main/Release/DartThrowing $dimension $num_classes $priority $r_values $k_value $domain_size to data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}.txt
src/Main/Release/DartThrowing $dimension $num_classes $priority $r_values $k_value $domain_size > data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}.txt
endif

#SFT
echo data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}_all.txt

src/Main/Release/SampleSelector data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}.txt ${dimension} ${ids} > data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}_all.txt

../BlueNoise/src/Main/Release/SFT data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}_all.txt data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}_all.pfm ${dimension} 1 1 ${sft_size} ${sft_size} 

set all_pfm_files = "$all_pfm_files data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}_all.pfm";
@ all_pfm_files_count = ${all_pfm_files_count} + 1;

@ i = ${i} + 1;
end

echo ${all_pfm_files} > data/all_pfm_files_list.txt;
src/Sain/Release/RadialPlot data/all_pfm_files_list.txt data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_all_average${all_pfm_files_count}.pfm data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_all_average${all_pfm_files_count}_mean.txt data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_all_average${all_pfm_files_count}_variance.txt
rm data/all_pfm_files_list.txt;

../BlueNoise/src/Main/Release/PFM2PPM data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_all_average${all_pfm_files_count}.pfm  data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_all_average${all_pfm_files_count}.ppm 1 1 1

end # foreach option

endif

# different ways to compute the r matrix
if(0) then

set dimension = 2;

set num_classes = 3;
set r_values = "0.04 0.02 0.01";
set r_text = 0p040p020p01;
set ids = "0 1 2";

#set target_samples = 7330;
set target_samples = 6801;
set k_value = 100;
#set rho_number = 0.696400653;
set rho_number = 0.67;
set domain_size = "1 1";

set num_runs = 10;
set sft_size = 512;
set sft_scale = 512;

if(${target_samples} > 0) then
@ k_value = ${target_samples} * -1;
endif

foreach k_option (0) #(0 1)

if(${k_option} == 0) then
set k_value_arg = ${rho_number};
else
set k_value_arg = ${k_value};
endif

foreach p_option (0 1)

if(${p_option} == 0) then
set priority = "0 0 0";
set p_text = 000;
else
set priority = "0 1 2";
set p_text = 012;
endif

foreach sign (- +)

# sample sets
set i = 0;
while(${i} < ${num_runs})

src/Main/Release/DartThrowingHistory $dimension ${sign}$num_classes $priority $r_values $k_value_arg $domain_size data/poisson_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}_${k_option}k_history_${i}.txt > data/poisson_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}_${k_option}k_${i}.txt

sleep 1;
@ i = ${i} + 1;
end

# SFT
set id = 0;
while(${id} < ${num_classes})

set all_pfm_files = "";
set all_pfm_files_count = 0;

set i = 0;
while(${i} < ${num_runs})

src/Main/Release/SampleSelector data/poisson_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}_${k_option}k_${i}.txt ${dimension} ${id} > data/poisson_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}_${k_option}k_${i}_selected${id}.txt

../BlueNoise/src/Main/Release/SFT data/poisson_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}_${k_option}k_${i}_selected${id}.txt data/poisson_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}_${k_option}k_${i}_selected${id}.pfm ${dimension} ${domain_size} ${sft_size} ${sft_scale}

set all_pfm_files = "$all_pfm_files data/poisson_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}_${k_option}k_${i}_selected${id}.pfm";
@ all_pfm_files_count = ${all_pfm_files_count} + 1;

@ i = ${i} + 1;
end

echo ${all_pfm_files} > data/all_pfm_files_list.txt;
src/Sain/Release/RadialPlot data/all_pfm_files_list.txt data/poisson_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}_${k_option}k_selected${id}_average${all_pfm_files_count}.pfm data/poisson_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}_${k_option}k_selected${id}_average${all_pfm_files_count}_mean.txt data/poisson_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}_${k_option}k_selected${id}_average${all_pfm_files_count}_variance.txt
rm data/all_pfm_files_list.txt;

../BlueNoise/src/Main/Release/PFM2PPM data/poisson_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}_${k_option}k_selected${id}_average${all_pfm_files_count}.pfm  data/poisson_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}_${k_option}k_selected${id}_average${all_pfm_files_count}.ppm 1 1 1

@ id = ${id} + 1;
end

end # foreach sign

end # foreach p_option

end # foreach k_option

endif

# ground truth matching above: different ways to compute the r matrix
if(0) then

set dimension = 2;
set original_num_classes = 3;
set num_classes = 1;

set r_text = "0p040p020p01";

set priority = "0";
set p_text = any;

set domain_size = "1 1";

set num_runs = 10;

set sft_size = 512;

set id = 0;
while(${id} < ${original_num_classes})

if(${id} == 0) then
set r_values = 0.04;
set num_samples = 323;
else if(${id} == 1) then
set r_values = 0.02;
set num_samples = 1295;
else if(${id} == 2) then
set r_values = 0.01;
set num_samples = 5183;
else
echo unknown id
exit(1);
endif

set k_value = -${num_samples};

# sample sets

set all_pfm_files_selected = "";
set all_pfm_files_count = 0;

set i = 0;
while(${i} < ${num_runs})

echo data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_selected${id}_${i}.txt

src/Main/Release/DartThrowing $dimension $num_classes $priority $r_values $k_value $domain_size > data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_selected${id}_${i}.txt

src/Main/Release/SampleSelector data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_selected${id}_${i}.txt ${dimension} 0 > data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_selected${id}_${i}_selected.txt

../BlueNoise/src/Main/Release/SFT data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_selected${id}_${i}_selected.txt data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_selected${id}_${i}.pfm ${dimension} 1 1 ${sft_size} ${sft_size}


set all_pfm_files_selected = "$all_pfm_files_selected data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_selected${id}_${i}.pfm";
@ all_pfm_files_count = ${all_pfm_files_count} + 1;

@ i = ${i} + 1;
end

echo ${all_pfm_files_selected} > data/all_pfm_files_selected_list.txt;
src/Sain/Release/RadialPlot data/all_pfm_files_selected_list.txt data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_selected${id}_average${all_pfm_files_count}.pfm data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_selected${id}_average${all_pfm_files_count}_mean.txt data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_selected${id}_average${all_pfm_files_count}_variance.txt 
rm data/all_pfm_files_selected_list.txt;

@ id = ${id} + 1;
end

endif

# ground truth matching above: different ways to compute the r matrix
if(0) then

echo this is obsolete 
exit(1);

set dimension = 2;
set original_num_classes = 3;
set num_classes = 1;

set r_text = "0p040p020p01";

set priority = "0";

set domain_size = "1 1";

set num_runs = 10;

set sft_size = 512;

foreach p_option (0 1)

if(${p_option} == 0) then
set p_text = 000;
else
set p_text = 012;
endif

foreach sign_option ('negative' 'positive')

if(${sign_option} == 'negative') then
set sign = '-';
else if(${sign_option} == 'positive') then
set sign = '+';
endif

set id = 0;
while(${id} < ${original_num_classes})

if(${id} == 0) then
set r_values = 0.04;
else if(${id} == 1) then
set r_values = 0.02;
else if(${id} == 2) then
set r_values = 0.01;
else
echo unknown id
exit(1);
endif

if(${p_option} == 0) then

if(${sign_option} == 'negative') then

if(${id} == 0) then
set num_samples = 381;
else if(${id} == 1) then
set num_samples = 1381;
else #if(${id} == 2) then
set num_samples = 5568;
endif

else if(${sign_option} == "positive") then

if(${id} == 0) then
set num_samples = 318;
else if(${id} == 1) then
set num_samples = 1186;
else #if(${id} == 2) then
set num_samples = 5826;
endif

else
echo unknown sign
exit(1);
endif #(${sign_option} == 'negative')

else #if(${p_option} == 1) then

if(${id} == 0) then
set num_samples = 350;
else if(${id} == 1) then
set num_samples = 1397;
else #if(${id} == 2) then
set num_samples = 5585;
endif

endif #(${p_option} == 0)

set k_value = -${num_samples};

# sample sets

set all_pfm_files_selected = "";
set all_pfm_files_count = 0;

set i = 0;
while(${i} < ${num_runs})

echo data/poisson_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}_selected${id}_${i}.txt

src/Main/Release/DartThrowing $dimension $num_classes $priority $r_values $k_value $domain_size > data/poisson_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}_selected${id}_${i}.txt

src/Main/Release/SampleSelector data/poisson_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}_selected${id}_${i}.txt ${dimension} 0 > data/poisson_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}_selected${id}_${i}_selected.txt

../BlueNoise/src/Main/Release/SFT data/poisson_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}_selected${id}_${i}_selected.txt data/poisson_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}_selected${id}_${i}.pfm ${dimension} 1 1 ${sft_size} ${sft_size}


set all_pfm_files_selected = "$all_pfm_files_selected data/poisson_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}_selected${id}_${i}.pfm";
@ all_pfm_files_count = ${all_pfm_files_count} + 1;

@ i = ${i} + 1;
end

echo ${all_pfm_files_selected} > data/all_pfm_files_selected_list.txt;
src/Sain/Release/RadialPlot data/all_pfm_files_selected_list.txt data/poisson_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}_selected${id}_average${all_pfm_files_count}.pfm data/poisson_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}_selected${id}_average${all_pfm_files_count}_mean.txt data/poisson_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}_selected${id}_average${all_pfm_files_count}_variance.txt 
rm data/all_pfm_files_selected_list.txt;

@ id = ${id} + 1;
end

end # foreach sign

end # foreach p_option

endif

# initial study of the class probability
if(0) then

set dimension = 2;
set domain_size = "1 1";
set num_classes = 2;

set num_runs = 10;

if(${num_runs} != 10) then
echo need to reset all individual r values below
exit(1);
endif

foreach r_option (1 2 3 4 5 6)

if(${r_option} == 1) then
set r_values = "0.01 0.01";
set r_value_0_div_num_runs = 0.001;
set r_value_1_div_num_runs = 0.001;
set r_text = 0p01m0p01osqrt2;
else if(${r_option} == 2) then
set r_values = "0.02 0.00755929";
set r_value_0_div_num_runs = 0.002;
set r_value_1_div_num_runs = 0.000755929;
set r_text = 0p02m0p01osqrt2;
else if(${r_option} == 3) then
set r_values = "0.04 0.00718421";
set r_value_0_div_num_runs = 0.004;
set r_value_1_div_num_runs = 0.000718421;
set r_text = 0p04m0p01osqrt2;
else if(${r_option} == 4) then
set r_values = "0.08 0.00709885";
set r_value_0_div_num_runs = 0.008;
set r_value_1_div_num_runs = 0.000709885;
set r_text = 0p08m0p01osqrt2;
else if(${r_option} == 5) then
set r_values = "0.011 0.00923099";
set r_value_0_div_num_runs = 0.0011;
set r_value_1_div_num_runs = 0.000923099;
set r_text = 0p011m0p01osqrt2;
else if(${r_option} == 6) then
set r_values = "0.0141421356 0.00816497";
set r_value_0_div_num_runs = 0.00141421356;
set r_value_1_div_num_runs = 0.000816497;
set r_text = 0p01sqrt2m0p01osqrt2;
else
echo unknown r_option;
exit(1);
endif

foreach rho_option (0 2)

if(${rho_option} == 0) then
set rho_value = 0.65;
set rho_text = 0p65;
set target_samples = 9760;
else if(${rho_option} == 1) then
set rho_value = 0.66;
set rho_text = 0p66;
set target_samples = 10060;
else if(${rho_option} == 2) then
set rho_value = 0.67;
set rho_text = 0p67;
set target_samples = 10400;
else if(${rho_option} == 3) then
set rho_value = 0.68;
set rho_text = 0p68;
set target_samples = 10680;
else if(${rho_option} == 4) then
set rho_value = 0.69;
set rho_text = 0p69;
set target_samples = 11000;
else if(${rho_option} == 5) then
set rho_value = 0.70;
set rho_text = 0p70;
set target_samples = 11400;
else
echo unknown rho option;
exit(1);
endif

if(${target_samples} > 0) then
@ k_value = ${target_samples} * -1;
endif

set output_name = class_probability_${dimension}d_${num_classes}c_${r_text}_${rho_text}

foreach class (0 1)
#echo ${p_text} ${class}
set output_file = data/class_probability_${dimension}d_${num_classes}c_${r_text}_${rho_text}_${class}.txt
rm -f ${output_file}
end

set p0_min = 1;
set p0_max = 99;
set p0_inc = 1;

set p0 = ${p0_min};

while(${p0} <= ${p0_max})

set p1 = 0;
@ p1 = 100 - ${p0};

if(${p0} < 10) then
set p0 = 0${p0};
endif
if(${p1} < 10) then
set p1 = 0${p1};
endif

set priority = "0.${p0} 0.${p1}";
set p_text = "0p${p0}0p${p1}";

set i = 0;

while(${i} < ${num_runs})

#echo data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${rho_text}_${i}.txt

echo src/Main/Release/DartThrowing $dimension $num_classes $priority $r_values $k_value $domain_size

src/Main/Release/DartThrowing $dimension $num_classes $priority $r_values $k_value $domain_size > data/huge/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${rho_text}_${i}.txt

@ i = ${i} + 1;
end

foreach class (0 1)
#echo ${p_text} ${class}
set output_file = data/class_probability_${dimension}d_${num_classes}c_${r_text}_${rho_text}_${class}.txt
echo "0.${p0} : " >> ${output_file}
touch junk.sh; chmod u+x junk.sh;
echo "#\!/bin/tcsh -f" > junk.sh;
echo "set num_samples = \\" >> junk.sh;
grep "^${class}" data/huge/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${rho_text}_?.txt | wc -l >> junk.sh;
echo '@ num_samples = ${num_samples} *' "${num_runs}" >> junk.sh;
echo "src/Main/Release/Math rho 2 \\" >> junk.sh;
if(${class} == 0) then
echo "${r_value_0_div_num_runs} \\" >> junk.sh;
else
echo "${r_value_1_div_num_runs} \\" >> junk.sh;
endif
echo '${num_samples}' >> junk.sh;

#cat junk.sh; echo debug; exit(1);
./junk.sh >> ${output_file}
rm junk.sh;
echo " " >>  ${output_file}
end

@ p0 = ${p0} + ${p0_inc};
end

end # foreach rho_option

end # foreach r_option

endif

# measure the number of trials from sampling history
if(0) then

set outputdir = data;

set dimension = 2;

set num_classes = 2;
set r_values = "0.02 0.00755929";
set r_text = 0p02m0p01osqrt2;
set priority = "0.425 0.575";

set rho_number = 0.67;
set target_samples = 10400;
set domain_size = "1 1";

set num_runs = 10;

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

set i = 0;
while(${i} < ${num_runs})
src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size ${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}_${k_option}k_history_${i}.txt > ${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}_${k_option}k_${i}.txt

@ i = ${i} + 1
end

set output_file_name = ${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}_${k_option}k_history_analysis.txt;

echo "" > ${output_file_name}

echo "killed: " >> ${output_file_name}
grep ". 2" ${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}_${k_option}k_history_?.txt | wc -l >> ${output_file_name}

echo "rejected: " >> ${output_file_name}
grep ". 3" ${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}_${k_option}k_history_?.txt | wc -l >> ${output_file_name}

echo "accepted: " >> ${output_file_name}
grep ". 4" ${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}_${k_option}k_history_?.txt | wc -l >> ${output_file_name}

echo "total: " >> ${output_file_name}
wc -l ${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}_${k_option}k_history_?.txt >> ${output_file_name}

end # foreach k_option

endif

# showing that treating samples as geometric disks won't be good for building r matrix
if(0) then

set outputdir = data;

set dimension = 2;

set num_classes = 2;
set priority = "0 0";
set r_values = "0.02 0.02";
set r_text = 0p02;

set dot_radius = 0.005;

set k_value = 100;
set domain_size = "1 1";

foreach sign ('0' '+')

echo src/Main/Release/DartThrowing $dimension $sign$num_classes $priority $r_values $k_value $domain_size
src/Main/Release/DartThrowing $dimension $sign$num_classes $priority $r_values $k_value $domain_size > ${outputdir}/poisson_${dimension}d_${sign}${num_classes}c_${r_text}.txt

foreach id (0 1 all)

set output_root_name = poisson_${dimension}d_${sign}${num_classes}c_${r_text}_${id}

if(${id} == 'all') then
cp ${outputdir}/poisson_${dimension}d_${sign}${num_classes}c_${r_text}.txt ${outputdir}/${output_root_name}.txt
else
grep "^${id}" ${outputdir}/poisson_${dimension}d_${sign}${num_classes}c_${r_text}.txt > ${outputdir}/${output_root_name}.txt
endif

src/Main/Release/DrawSamples ${outputdir}/${output_root_name}.txt ${outputdir}/${output_root_name}.fig ${dimension} ${dot_radius} 1 ${domain_size}
src/Scripts/fig2eps -f ${outputdir}/${output_root_name}.fig
epstopdf ${outputdir}/${output_root_name}.eps --outfile ${outputdir}/${output_root_name}.pdf
end

end #foreach sign

endif

# comparing single class Poisson disk sampling (applied to individual classes or the entire collection) to our method
if(0) then

set outputdir = data;

set dimension = 2;

set num_classes = 2;
set priority = "0 0";

set dot_radius = 0.005;

set target_samples_per_class = 650;

set k_value = 100;
@ k_value = -${target_samples_per_class} * ${num_classes};
set domain_size = "1 1";

foreach option (0 1 2)

if(${option} == 0) then # individual class single-class Poisson
set r_value = 0.0282842712;
set r_values = "${r_value} 0 ${r_value}";
set r_text = 0p02sqrt20p000p02sqrt2;

else if(${option} == 1) then # entire set single-class Poisson

set r_values = "0.02 0.02 0.02";
set r_text = 0p020p020p02;

else if(${option} == 2) then # our method

set r_values = "0.0282842712 0.02 0.0282842712";
set r_text = 0p02sqrt20p020p02sqrt2;

else
echo unknown option;
exit(1);
endif

if(${option} == 0) then

set list_file_name = ${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}_list.txt

echo "" > ${list_file_name}

foreach id (0 1 all)
set output_root_name = poisson_${dimension}d_${num_classes}c_${r_text}_${id}

if(${id} == 'all') then

src/Main/Release/MergeSamples ${list_file_name} ${dimension} > ${outputdir}/${output_root_name}.txt

rm ${list_file_name};

else
sleep 1;

src/Main/Release/DartThrowing $dimension 1 0 $r_value -${target_samples_per_class} $domain_size > ${outputdir}/${output_root_name}.txt

echo ${outputdir}/${output_root_name}.txt >> ${list_file_name}
endif

end

else

echo src/Main/Release/DartThrowing $dimension $num_classes $priority $r_values $k_value $domain_size
src/Main/Release/DartThrowing $dimension $num_classes $priority $r_values $k_value $domain_size > ${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}.txt

foreach id (0 1 all)

set output_root_name = poisson_${dimension}d_${num_classes}c_${r_text}_${id}

if(${id} == 'all') then
cp ${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}.txt ${outputdir}/${output_root_name}.txt
else
#grep "^${id}" ${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}.txt > ${outputdir}/${output_root_name}.txt
endif

end

endif # ${option} == 0

foreach id (0 1 all)

set output_root_name = poisson_${dimension}d_${num_classes}c_${r_text}_${id}

if(${id} != 'all') then
grep "^${id}" ${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}_all.txt > ${outputdir}/${output_root_name}.txt
endif

src/Main/Release/DrawSamples ${outputdir}/${output_root_name}.txt ${outputdir}/${output_root_name}.fig ${dimension} ${dot_radius} 1 ${domain_size}
src/Scripts/fig2eps -f ${outputdir}/${output_root_name}.fig
epstopdf ${outputdir}/${output_root_name}.eps --outfile ${outputdir}/${output_root_name}.pdf

end

end #foreach option

endif

# comparisons of the patience factor
if(0) then

set dimension = 2;

set num_classes = 3;
set r_values = "0.04 0.02 0.01";
set r_text = 0p040p020p01;
set ids = "0 1 2";

#set target_samples = 7330;
set target_samples = 6801;
set k_value = 100;
#set rho_number = 0.696400653;
set rho_number = 0.67;
set domain_size = "1 1";

set num_runs = 10;
set sft_size = 512;
set sft_scale = 512;

if(${target_samples} > 0) then
@ k_value = ${target_samples} * -1;
endif

set exe_option = 1;

if(${exe_option} == 0) then
set exe_name = DartThrowingPatientKilling;
set output_name = grid;
else
set exe_name = TreeThrowingPatientKilling;
set output_name = tree;
endif

foreach patience (0 1 100)

foreach k_option (0)

if(${k_option} == 0) then
set k_value_arg = ${rho_number};
else
set k_value_arg = ${k_value};
endif

foreach p_option (0)

if(${p_option} == 0) then
set priority = "0 0 0";
set p_text = 000;
else
set priority = "0 1 2";
set p_text = 012;
endif

foreach sign (+)

# sample sets
set i = 0;
while(${i} < ${num_runs})

src/Main/Release/${exe_name} $dimension ${sign}$num_classes $priority $r_values $k_value_arg $domain_size ${patience} > data/${output_name}_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}_${k_option}k_${patience}p_${i}.txt

sleep 1;
@ i = ${i} + 1;
end

# SFT
set id = 0;
while(${id} < ${num_classes})

set all_pfm_files = "";
set all_pfm_files_count = 0;

set i = 0;
while(${i} < ${num_runs})

src/Main/Release/SampleSelector data/${output_name}_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}_${k_option}k_${patience}p_${i}.txt ${dimension} ${id} > data/${output_name}_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}_${k_option}k_${patience}p_${i}_selected${id}.txt

../BlueNoise/src/Main/Release/SFT data/${output_name}_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}_${k_option}k_${patience}p_${i}_selected${id}.txt data/${output_name}_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}_${k_option}k_${patience}p_${i}_selected${id}.pfm ${dimension} ${domain_size} ${sft_size} ${sft_scale}

set all_pfm_files = "$all_pfm_files data/${output_name}_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}_${k_option}k_${patience}p_${i}_selected${id}.pfm";
@ all_pfm_files_count = ${all_pfm_files_count} + 1;

@ i = ${i} + 1;
end

echo ${all_pfm_files} > data/all_pfm_files_list.txt;
src/Sain/Release/RadialPlot data/all_pfm_files_list.txt data/${output_name}_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}_${k_option}k_${patience}p_selected${id}_average${all_pfm_files_count}.pfm data/${output_name}_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}_${k_option}k_${patience}p_selected${id}_average${all_pfm_files_count}_mean.txt data/${output_name}_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}_${k_option}k_${patience}p_selected${id}_average${all_pfm_files_count}_variance.txt
rm data/all_pfm_files_list.txt;

../BlueNoise/src/Main/Release/PFM2PPM data/${output_name}_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}_${k_option}k_${patience}p_selected${id}_average${all_pfm_files_count}.pfm  data/${output_name}_${dimension}d_${sign}${num_classes}c_${p_text}_${r_text}_${k_option}k_${patience}p_selected${id}_average${all_pfm_files_count}.ppm 1 1 1

@ id = ${id} + 1;
end

end # foreach sign

end # foreach p_option

end # foreach k_option

end # foreach patience

endif

# zone plate sampling pattern
if(0) then

set dimension = 2;

set num_classes = 2;
set priority = "0 0";
set p_text = 00;
set target_samples = 5200; # computed/estimated by the k_value below

set k_value = 100;
set k_text = ${k_value};
set rho_value = 0.671069;
set rho_text = 0p671069;
set k_value = ${rho_value};
set k_text = ${rho_text};

set domain_size = "1 1";

set r_value_0 = 02;
set r_text_0 = 02;
set r_value_1 = 00755929;
set r_text_1 = 01osqrt2;

if(1) then
set r_value_0 = 01;
set r_text_0 = 01;
set r_value_1 = 00353553391;
set r_text_1 = 005osqrt2;
endif

if(0) then
set r_value_0 = 005;
set r_text_0 = 005;
set r_value_1 = 00176776695;
set r_text_1 = 0025osqrt2;
endif

set r_values = "0.${r_value_0} 0.${r_value_1}";
set r_text = 0p${r_text_0}m0p${r_text_1};
set select_id = 1;

# for relaxation
set num_continuous_iterations = 10;
set num_discrete_iterations = 10;
set relaxation_id = 0;

# for zone plate
set function_offset = 0;
set function_scale = 10;
set output_resolution = 204;
set filter_gaussian_std = 1.5;

if(${r_value_0} == 01) then
set function_scale = 10;
set output_resolution = 204;
else if(${r_value_0} == 005) then
set function_scale = 16;
set output_resolution = 408;
endif

# for SFT
set sft_size = 256;
set sft_scale = 1024;

# ground truth
set one_class = 1;
set one_priority = 0;
set one_id = 0;

src/Main/Release/DartThrowing $dimension $one_class $one_priority 0.$r_value_1 $k_value $domain_size > data/poisson_${dimension}d_${one_class}c_${r_text}.txt

src/Main/Release/SampleSelector data/poisson_${dimension}d_${one_class}c_${r_text}.txt ${dimension} ${one_id} > data/poisson_${dimension}d_${one_class}c_${r_text}_selected.txt

src/Sain/Release/ZonePlate ${dimension} data/poisson_${dimension}d_${one_class}c_${r_text}_selected.txt data/poisson_${dimension}d_${one_class}c_${r_text}_zoneplate.ppm ${function_offset} ${function_scale} ${output_resolution} ${filter_gaussian_std}

foreach post_process ('' 'random' 'discreterelax')

set output_base_name = poisson_${dimension}d_${num_classes}c_${r_text};

if(${post_process} == '') then
# original sample set
src/Main/Release/DartThrowing $dimension $num_classes $priority $r_values $k_value $domain_size > data/${output_base_name}.txt

set output_name = ${output_base_name};

else if(${post_process} == 'random') then

set output_name = ${output_base_name}_${post_process};

src/Main/Release/RandomizeSamples ${dimension} data/${output_base_name}.txt > data/${output_name}.txt

else if (${post_process} == 'discreterelax') then

set output_name = ${output_base_name}_${post_process};

src/Main/Release/DiscreteRelaxSamples data/${output_base_name}.txt ${dimension} ${domain_size} ${num_continuous_iterations} ${num_discrete_iterations} ${relaxation_id} 0 > data/${output_name}.txt

else
echo unknown post process
exit(1);
endif

src/Main/Release/SampleSelector data/${output_name}.txt ${dimension} ${select_id} > data/${output_name}_selected.txt

src/Sain/Release/ZonePlate ${dimension} data/${output_name}_selected.txt data/${output_name}_zoneplate.ppm ${function_offset} ${function_scale} ${output_resolution} ${filter_gaussian_std}

if(0) then
../BlueNoise/src/Main/Release/SFT data/${output_name}_selected.txt data/${output_name}.pfm ${dimension} 1 1 ${sft_size} ${sft_scale}
../BlueNoise/src/Main/Release/PFM2PPM data/${output_name}.pfm data/${output_name}.ppm 1 1 0
endif

end # foreach post_process

# pair-wise comparison
foreach process () #('random' 'discreterelax')

if(${process} == 'random') then
set function_offset = 0;
set function_scale = 100;
else if(${process} == 'discreterelax') then
set function_offset = 76;
set function_scale = 100;
else
echo unknown post process
exit(1);
endif

src/Sain/Release/ZonePlate ${dimension} data/${output_base_name}_selected.txt data/${output_base_name}_focus_zoneplate.ppm ${function_offset} ${function_scale} ${output_resolution} ${filter_gaussian_std}

set output_name = ${output_base_name}_${process};

src/Sain/Release/ZonePlate ${dimension} data/${output_name}_selected.txt data/${output_name}_focus_zoneplate.ppm ${function_offset} ${function_scale} ${output_resolution} ${filter_gaussian_std}

end # foreach process

endif

# multi-tree adaptive sampling
if(0) then

set outputdir = data;

set draw_objects = 1;

set dimension = 2;

set num_classes = 2;
set priority = "0 0";

set r_values = "0.02 0.02";
set r_text = 0p02;
set dot_radius = 0.002;

if(${draw_objects}) then
set r_values = "0.04 0.04";
set r_text = 0p04;
set dot_radius = 0.004;
endif

set field_name = "uniform";
#set field_name = "tog";
#set field_name = "blob";
#set field_name = "2blob";
#set field_name = "stripe";

if((${field_name} == "uniform") || (${field_name} == "tog")) then
set object_scale = 0.025;
set boundary_condition = 1; # none or toroidal
else
set object_scale = 0.015;
set boundary_condition = 0; # none or toroidal
endif

set window_size = 1024; #768;
set background_color = "data/free_grass_texture.ppm"; #1;
set batch_mode = 1;
set fig_or_ppm = 1; # 0 for fig, 1 for ppm

set flower0 = red_flower;
set flower1 = yellow_flower;

set flower0 = flower_red_rose;
#set flower0 = flower_dahlia_pink;
set flower1 = flower_buttercup_yellow;
#set flower1 = flower_cactus_white;
#set flower1 = flower_daisy_white;

if(${fig_or_ppm} == 0) then
set object_files = " ${flower0}.gif ${flower1}.gif";
set output_file_ext = "fig";
else
set object_files = " ${outputdir}/${flower0}.ppm ${outputdir}/${flower1}.ppm";
set output_file_ext = "ppm";
endif

set object_name = "flower";

set object_sizes = "${object_scale} ${object_scale}";

set k_value = 16.67;
set domain_size = "1 1";

set conflict_metric = 0; # mean

set output_name = adaptive_${dimension}d_${num_classes}c_${r_text};

# create distance fields
echo build fields
foreach class (0 1)
set output_field_name = ${field_name}_${class}

if(${field_name} == 'uniform') then
echo "1 2 1 0.5 0.5 10000000000 10000000000" > ${outputdir}/${output_field_name}.txt
else if(${field_name} == 'tog') then
# aspect ratio 4:3 for TOG image
set domain_size = "0.75 1";
echo "1 2 1 0.375 0.5 10000000000 10000000000" > ${outputdir}/${output_field_name}.txt
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

foreach exe_option (0) #(0 1)

if(${exe_option} == 0) then
set exe_name = AdaptiveDartThrowing;
set output_name_exe = grid;
else if(${exe_option} == 1) then
set exe_name = MultiTreeAdaptiveSampling;
set output_name_exe = multitree;
else
endif

set output_name_exe = ${output_name_exe}_${output_name}
if(1) then
if(${num_classes} == 2) then
echo src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size ${conflict_metric} ${outputdir}/${field_name}_0.pfm ${outputdir}/${field_name}_1.pfm
src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size ${conflict_metric} ${outputdir}/${field_name}_0.pfm ${outputdir}/${field_name}_1.pfm > ${outputdir}/${output_name_exe}.txt
else
echo src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size ${conflict_metric} ${outputdir}/${field_name}_0.pfm
src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size ${conflict_metric} ${outputdir}/${field_name}_0.pfm > ${outputdir}/${output_name_exe}.txt
endif
endif

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

if(1) then
set output_object_root_name = ${object_name}_${field_name}_${dimension}d_${num_classes}c_${r_text};

src/GPU/Release/DrawObjects ${outputdir}/${output_name_exe}.txt ${outputdir}/${output_object_root_name}_objects.${output_file_ext} ${dimension} ${domain_size} ${num_classes} ${object_files} ${object_sizes} ${window_size} ${background_color} ${batch_mode} ${boundary_condition}

echo ${outputdir}/${output_object_root_name}_objects.${output_file_ext}
endif

end #foreach exe_option

endif

# Lenna and Mandril
# both from http://en.wikipedia.org/wiki/Standard_test_image
if(0) then

set outputdir = data;

set generate_samples = 0;

set dimension = 2;

#set num_classes = 3;

set r_value = 02; 
set dot_radius = 0.004; # about 1/5 #0.0${r_value};

if(0) then
set r_value = 01;
set dot_radius = 0.002;
endif

if(0) then
set r_value = 005;
set dot_radius = 0.001;
endif

if(0) then # for 3 class
set r_value = 002;
set dot_radius = 0.00051;
#set dot_radius = 0.001;
endif

if(0) then # for 6 and 7 class
set r_value = 003;
set dot_radius = 0.0006;
#set dot_radius = 0.0007;
endif

set k_value = 128.67; #16;
set domain_size = "1 1";

set conflict_metric = 0; # mean

#set field_name = "Lenna";
#set field_name = "Mandril";
#set field_name = "lena";

foreach field_name ("van_gogh_bandaged") # ("Lenna") ("Mandril") ("Monarch") ("Pepper") ("van_gogh_bandaged")

foreach negation (0) #(1 0)

foreach num_classes (7) #(6 7 3 4) 

set priority = "";
set r_values = "";
set field_names = "";

set class = 0;
while(${class} < ${num_classes})
set priority = "${priority} 0";
set r_values = "${r_values} 0.${r_value}";
set field_names = "${field_names} ${outputdir}/${field_name}_${class}.pfm";

@ class = ${class} + 1;
end

set r_text = 0p${r_value};

if((${num_classes} == 3) || (${num_classes} == 6) || (${num_classes} == 7)) then

if(${negation} == 0) then
set conversion_source = ${outputdir}/${field_name}.ppm;
else
set conversion_source = ${outputdir}/${field_name}_negate.ppm;
convert ${outputdir}/${field_name}.ppm -negate -compress none ${conversion_source}
endif

else if(${num_classes} == 4) then

if(${negation} == 0) then
set conversion_source = ${outputdir}/${field_name}.tif;
convert -colorspace RGB ${outputdir}/${field_name}.ppm -colorspace CMYK ${conversion_source}
else
set conversion_source = ${outputdir}/${field_name}_negate.tif;
convert -colorspace RGB ${outputdir}/${field_name}.ppm -negate -colorspace CMYK ${conversion_source}
endif

else
echo unknown number of classes
exit(1);
endif

set class = 0;
while(${class} < ${num_classes})

if((${num_classes} == 3) || (${num_classes} == 6) || (${num_classes} == 7)) then

if(${class} == 0) then
set channel = 'R';
else if(${class} == 1) then
set channel = 'G';
else if(${class} == 2) then
set channel = 'B';
else
set channel = '';
endif

else if(${num_classes} == 4) then

if(${class} == 0) then
set channel = 'C';
else if(${class} == 1) then
set channel = 'M';
else if(${class} == 2) then
set channel = 'Y';
else if(${class} == 3) then
set channel = 'K';
else
set channel = '';
endif

else
echo unknown number of classes;
exit(1);
endif

if(${channel} != '') then
convert -separate -channel ${channel} -compress none ${conversion_source} ${outputdir}/${field_name}_${class}.ppm
./src/Sain/Release/PPM2PFM ${outputdir}/${field_name}_${class}.ppm ${outputdir}/${field_name}_${class}.pfm
endif

@ class = ${class} + 1;
end # while

if(${num_classes} > 4) then

if(${num_classes} == 6) then
if(${negation} == 0) then
set conversion_source = ${outputdir}/${field_name}.ppm;
set output_basis = "1 0 0  0 1 0  0 0 1  0 1 1  1 0 1  1 1 0";
else
set conversion_source = ${outputdir}/${field_name}_negate.ppm;
set output_basis = "0 1 1  1 0 1  1 1 0  1 0 0  0 1 0  0 0 1";
endif
set output_basis = "1 0 0  0 1 0  0 0 1  0 1 1  1 0 1  1 1 0";
else if(${num_classes} == 7) then
if(${negation} == 0) then
set conversion_source = ${outputdir}/${field_name}.ppm;
set output_basis = "1 0 0  0 1 0  0 0 1  0 1 1  1 0 1  1 1 0  1 1 1";
else
set conversion_source = ${outputdir}/${field_name}_negate.ppm;
set output_basis = "0 1 1  1 0 1  1 1 0  1 0 0  0 1 0  0 0 1  0 0 0";
endif
set output_basis = "1 0 0  0 1 0  0 0 1  0 1 1  1 0 1  1 1 0  1 1 1";
else
echo unknown number of classes > 4;
exit(1);
endif

./src/Sain/Release/PPM2PFM ${conversion_source} ${outputdir}/${field_name}.pfm;
./src/Main/Release/ColorTransform ${outputdir}/${field_name}.pfm ${num_classes} ${output_basis} ${field_names}
endif
# echo debug; exit(1);

set exe_name = MultiTreeAdaptiveSampling;
set output_name_exe = multitree;

set output_name = adaptive_${field_name}_${dimension}d_${num_classes}c_${r_text}_n${negation};

set output_name_exe = ${output_name_exe}_${output_name}

if(${generate_samples}) then
echo src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size ${conflict_metric} ${field_names}
time src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size ${conflict_metric} ${field_names} > ${outputdir}/${output_name_exe}.txt
endif

set color_palette = ${outputdir}/${field_name}_color_palette.txt

if(${num_classes} == 3) then

if(${negation} == 0) then
echo "255 0 0  0 255 0  0 0 255" > ${color_palette}
set background = -1;
else
echo "0 255 255  255 0 255  255 255 0" > ${color_palette}
set background = 1;
endif

else if(${num_classes} == 4) then

if(${negation} == 0) then
echo "0 255 255  255 0 255  255 255 0  0 0 0" > ${color_palette}
set background = 1;
else
echo "255 0 0  0 255 0  0 0 255  255 255 255" > ${color_palette}
set background = 1;
endif

else if(${num_classes} == 6) then

if(${negation} == 0) then
echo "255 0 0  0 255 0  0 0 255  0 255 255  255 0 255  255 255 0" > ${color_palette}
set background = -1;
else
echo "0 255 255  255 0 255  255 255 0  255 0 0  0 255 0  0 0 255" > ${color_palette}
set background = 1;
endif

else if(${num_classes} == 7) then

if(${negation} == 0) then
echo "255 0 0  0 255 0  0 0 255  0 255 255  255 0 255  255 255 0  255 255 255" > ${color_palette}
set background = -1;
else
echo "0 255 255  255 0 255  255 255 0  255 0 0  0 255 0  0 0 255  0 0 0" > ${color_palette}
set background = 1;
endif

else
echo unknown number of classes;
exit(1);
endif

set id = -1;
#while(${id} < ${num_classes})
while(${id} < 0)
if(${id} == -1) then
set output_name_final = ${output_name_exe};
else
set output_name_final = ${output_name_exe}_${id};
grep "^${id}" ${outputdir}/${output_name_exe}.txt > ${outputdir}/${output_name_final}.txt
endif

src/Main/Release/DrawSamples ${outputdir}/${output_name_final}.txt ${outputdir}/${output_name_final}.fig ${dimension} ${dot_radius} ${background} ${domain_size} ${color_palette}
src/Scripts/fig2eps -f ${outputdir}/${output_name_final}.fig
epstopdf ${outputdir}/${output_name_final}.eps --outfile ${outputdir}/${output_name_final}.pdf

@ id = ${id} + 1; 
end # while(${id}

end # foreach num_classes
end # foreach negation 
end # foreach field_name

endif

# Penrose coloring
if(0) then

set outputdir = data;

set dimension = 2;
set num_classes = 3;
set domain_size = "1 1";

set uniform_field_value = 40000;
set dot_radius_original = 0.001;

set num_iterations = -1;

set illustration = 1;
set voronoi = 1;

# assuming 4626 penrose samples in total

# min/worst case for my algorithm, assuming same packing area
set r_min_value = 0.0287351106;
set r_min_radius = 0.00829511192; # r_value/2/sqrt(3)

# max/best case for my algorithm, assuming same number of samples
set r_max_value = 0.0185988865;
set r_max_radius = 0.00536903606;

# smaller ones for illustration
if(${illustration}) then
set uniform_field_value = 10000;
set dot_radius_original = 0.002;
set r_min_value = 0.0574702212;
set r_min_radius = 0.0165902238; # r_value/2/sqrt(3)
set r_max_value = 0.037197773;
set r_max_radius = 0.0107380721;
endif

set k_value = 0.68;

set num_runs = 10;
set select_id = 0;
set sft_size = 512;
if(${illustration}) then
set num_runs = 1;
set sft_size = 16;
endif

set sft_scale = ${sft_size};

foreach method (grid penrose)

if(${method} == 'penrose') then
set num_packings = 1;
else
set num_packings = 2;
endif

set packing = 0;

while(${packing} < ${num_packings})

if(${packing} == 0) then
set packing_name = 'min';
set r_value = ${r_min_value};
set r_radius = ${r_min_radius};
else
set packing_name = 'max';
set r_value = ${r_max_value};
set r_radius = ${r_max_radius};
endif

if(${method} == 'penrose') then
set exe_name = 'PenroseColoring';
set output_base_name = penrose_color_uniform${uniform_field_value};
else if(${method} == 'grid') then
set exe_name = DartThrowing;
set output_base_name = grid_${packing_name}_uniform${uniform_field_value};
else if(${method} == 'tree') then
set exe_name = TreeThrowing;
set output_base_name = tree_${packing_name}_uniform${uniform_field_value};
else if(${method} == 'multitree') then
set exe_name = MultiTreeUniformSampling;
set output_base_name = multitree_${packing_name}_uniform${uniform_field_value};
else 
echo unknown method
exit(1);
endif

if(${method} == 'penrose') then
set dot_radius = ${dot_radius_original};
else
set dot_radius = ${r_radius};
endif

set r_values = "";
set priority = "";
set i = 0;
while(${i} < ${num_classes})
set r_values = "${r_values} ${r_value}";
set priority = "${priority} 0";
@ i = ${i} + 1;
end

set all_pfm_files = "";
set i = 0;
while(${i} < ${num_runs})

set output_name = ${output_base_name}_${i};

if(${method} == 'penrose') then
if(${i} == 0) then
set output_tile_file_name = ${outputdir}/${output_base_name}_tile_${i}.fig;
else
set output_tile_file_name = "";
endif
src/Tiling/Release/${exe_name} ${domain_size} ${uniform_field_value} ${num_iterations} ${output_tile_file_name} > ${outputdir}/${output_name}.txt;
else #if(${method} == 'grid') then
src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size > ${outputdir}/${output_name}.txt
endif

src/Main/Release/SampleSelector ${outputdir}/${output_name}.txt ${dimension} ${select_id} > ${outputdir}/${output_name}_selected.txt

../BlueNoise/src/Main/Release/SFT ${outputdir}/${output_name}_selected.txt ${outputdir}/${output_name}.pfm ${dimension} 1 1 ${sft_size} ${sft_scale}

set all_pfm_files = "${all_pfm_files} ${outputdir}/${output_name}.pfm";

if(${i} == 0) then
if(${method} == 'penrose') then
set output_tile_name = ${output_base_name}_tile_${i};
src/Scripts/fig2eps -f ${outputdir}/${output_tile_name}.fig
epstopdf ${outputdir}/${output_tile_name}.eps --outfile ${outputdir}/${output_tile_name}.pdf
endif

if(${voronoi}) then
set draw_name = ${output_base_name}_voronoi_${i};
else
set draw_name = ${output_name};
endif

set color_palette = ${outputdir}/${draw_name}_palette.txt;
echo "255 0 0   0 255 0   0 0 255   0 255 255   255 0 255  255 255 0" > ${color_palette}
if(${voronoi}) then
src/Main/Release/DrawSamples ${outputdir}/${output_name}.txt ${outputdir}/${draw_name}.fig ${dimension} -0.100000001 1 ${domain_size} ${color_palette}
else
src/Main/Release/DrawSamples ${outputdir}/${output_name}.txt ${outputdir}/${draw_name}.fig ${dimension} ${dot_radius} -2 ${domain_size} ${color_palette}
endif
src/Scripts/fig2eps -f ${outputdir}/${draw_name}.fig
epstopdf ${outputdir}/${draw_name}.eps --outfile ${outputdir}/${draw_name}.pdf
endif

@ i = ${i} + 1;
end # while(${i} < ${num_runs})

echo ${all_pfm_files} > ${outputdir}/all_pfm_files_list.txt;

src/Sain/Release/RadialPlot data/all_pfm_files_list.txt ${outputdir}/${output_base_name}_average${num_runs}.pfm ${outputdir}/${output_base_name}_average${num_runs}_mean.txt ${outputdir}/${output_base_name}_average${num_runs}_variance.txt
rm data/all_pfm_files_list.txt;

../BlueNoise/src/Main/Release/PFM2PPM ${outputdir}/${output_base_name}_average${num_runs}.pfm ${outputdir}/${output_base_name}_average${num_runs}.ppm 1 1 1

@ packing = ${packing} + 1;

end # while(${packing} < ${num_packings})

end # foreach method

endif

# penrose zone plate
if(0) then

set outputdir = data;

set dimension = 2;
set num_classes = 3;
set domain_size = "1 1";

set uniform_field_value = 40000;
set dot_radius = 0.001;

set num_iterations = -1;

# min/worst case for my algorithm, assuming same packing area
set r_min_value = 0.0287351106;
set r_min_radius = 0.00829511192; # r_value/2/sqrt(3)

# max/best case for my algorithm, assuming same number of samples
set r_max_value = 0.0185988865;
set r_max_radius = 0.00536903606;

set k_value = 0.68;

# for tiling
set num_tile_per_dim = 10;
set scaling = 0.1;
set num_tiles = "${num_tile_per_dim} ${num_tile_per_dim}";

# for zone plate
set function_offset = 0;
set function_scale_class = 8; #16;
set function_scale_all = 13.8422392; #27.6844784;
set output_resolution_class = 393; # individual class
set output_resolution_all = 680; # putting all samples together
set filter_gaussian_std = 1.5;

foreach method (grid penrose)

if(${method} == 'penrose') then
set num_packings = 1;
else
set num_packings = 2;
endif

set packing = 0;

while(${packing} < ${num_packings})

if(${packing} == 0) then
set packing_name = 'min';
set r_value = ${r_min_value};
set r_radius = ${r_min_radius};
else
set packing_name = 'max';
set r_value = ${r_max_value};
set r_radius = ${r_max_radius};
endif

if(${method} == 'penrose') then
set exe_name = PenroseColoring;
set output_base_name = penrose_color_uniform${uniform_field_value};
else if(${method} == 'grid') then
set exe_name = DartThrowing;
set output_base_name = grid_${packing_name}_uniform${uniform_field_value};
else if(${method} == 'tree') then
set exe_name = TreeThrowing;
set output_base_name = tree_${packing_name}_uniform${uniform_field_value};
else if(${method} == 'multitree') then
set exe_name = MultiTreeUniformSampling;
set output_base_name = multitree_${packing_name}_uniform${uniform_field_value};
else 
echo unknown method
exit(1);
endif

set r_values = "";
set priority = "";
set i = 0;
while(${i} < ${num_classes})
set r_values = "${r_values} ${r_value}";
set priority = "${priority} 0";
@ i = ${i} + 1;
end

set output_name = ${output_base_name};
#set output_name = penrose_color_uniform${uniform_field_value};
set output_tiling_name = ${output_base_name}_tiling${num_tile_per_dim};

if(${method} == 'penrose') then
src/Tiling/Release/${exe_name} ${domain_size} ${uniform_field_value} ${num_iterations} > ${outputdir}/${output_name}.txt;
else #if(${method} == 'grid') then
src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size > ${outputdir}/${output_name}.txt
endif

src/Main/Release/TileSamples ${outputdir}/${output_name}.txt ${dimension} ${scaling} ${num_tiles} ${domain_size} > ${outputdir}/${output_tiling_name}.txt;

foreach class (all 0 1 2)

if(${class} == 'all') then
set select_ids = '0 1 2'; 
set function_scale = ${function_scale_all};
set output_resolution = ${output_resolution_all};
else
set select_ids = ${class};
set function_scale = ${function_scale_class};
set output_resolution = ${output_resolution_class};
endif

src/Main/Release/SampleSelector ${outputdir}/${output_tiling_name}.txt ${dimension} ${select_ids} > ${outputdir}/${output_tiling_name}_${class}.txt

src/Sain/Release/ZonePlate ${dimension} ${outputdir}/${output_tiling_name}_${class}.txt ${outputdir}/${output_tiling_name}_${class}_zoneplate.ppm ${function_offset} ${function_scale} ${output_resolution} ${filter_gaussian_std}

../BlueNoise/src/Main/Release/PPM2PFM ${outputdir}/${output_tiling_name}_${class}_zoneplate.ppm ${outputdir}/${output_tiling_name}_${class}_zoneplate.pfm
end

# merge
src/Sain/Release/Merge3PFM ${outputdir}/${output_tiling_name}_0_zoneplate.pfm ${outputdir}/${output_tiling_name}_1_zoneplate.pfm ${outputdir}/${output_tiling_name}_2_zoneplate.pfm ${outputdir}/${output_tiling_name}_merge_zoneplate.pfm 

src/Sain/Release/PFM2PPM ${outputdir}/${output_tiling_name}_merge_zoneplate.pfm ${outputdir}/${output_tiling_name}_merge_zoneplate.ppm 1 1 2

if(0) then
src/Main/Release/DrawSamples ${outputdir}/${output_tiling_name}.txt ${outputdir}/${output_tiling_name}.fig ${dimension} ${dot_radius} 1 ${domain_size}
src/Scripts/fig2eps -f ${outputdir}/${output_tiling_name}.fig
epstopdf ${outputdir}/${output_tiling_name}.eps --outfile ${outputdir}/${output_tiling_name}.pdf
endif

@ packing = ${packing} + 1;

end # while(${packing} < ${num_packings})

end # foreach method

endif

# rainbow spectrum image
if(0) then

set outputdir = data;

set dimension = 2;

set exe_name = DartThrowing;

set num_classes = 3;
set priority = "0 0 0";

set r_values = "0.02 0.02 0.02";
set r_text = 0p02;

set output_base_name = grid_${dimension}d_${num_classes}c_${r_text};

set k_value = 0.68;
set domain_size = "1 1";

set num_runs = 10;
set sft_size = 512;
set sft_scale = 512;

set i = 0;
while(${i} < ${num_runs})

set output_name = ${output_base_name}_${i};

src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size > ${outputdir}/${output_name}.txt
sleep 1;

@ i = ${i} + 1;
end

foreach class (0 1 2)

set all_pfm_files = "";
set i = 0;
while(${i} < ${num_runs})

set input_name = ${output_base_name}_${i};
set output_name = ${output_base_name}_c${class}_${i};

src/Main/Release/SampleSelector ${outputdir}/${input_name}.txt ${dimension} ${class} > ${outputdir}/${output_name}_selected.txt

../BlueNoise/src/Main/Release/SFT ${outputdir}/${output_name}_selected.txt ${outputdir}/${output_name}.pfm ${dimension} 1 1 ${sft_size} ${sft_scale}

set all_pfm_files = "${all_pfm_files} ${outputdir}/${output_name}.pfm";

@ i = ${i} + 1;
end # while(${i} < ${num_runs})

echo ${all_pfm_files} > ${outputdir}/all_pfm_files_list.txt;

src/Sain/Release/RadialPlot data/all_pfm_files_list.txt ${outputdir}/${output_base_name}_c${class}_average${num_runs}.pfm ${outputdir}/${output_base_name}_c${class}_average${num_runs}_mean.txt ${outputdir}/${output_base_name}_c${class}_average${num_runs}_variance.txt
rm data/all_pfm_files_list.txt;

../BlueNoise/src/Main/Release/PFM2PPM ${outputdir}/${output_base_name}_c${class}_average${num_runs}.pfm ${outputdir}/${output_base_name}_c${class}_average${num_runs}.ppm 1 1 1

end # foreach class

# combine
src/Sain/Release/Merge3PFM ${outputdir}/${output_base_name}_c0_average${num_runs}.pfm ${outputdir}/${output_base_name}_c1_average${num_runs}.pfm ${outputdir}/${output_base_name}_c2_average${num_runs}.pfm ${outputdir}/${output_base_name}_merge_average${num_runs}.pfm
src/Sain/Release/PFM2PPM ${outputdir}/${output_base_name}_merge_average${num_runs}.pfm ${outputdir}/${output_base_name}_merge_average${num_runs}.ppm 1 1 1

endif

# comparing single class Poisson disk sampling (applied to individual classes or the entire collection) to our method
# visualization via object placements
if(0) then

set outputdir = data;

set dimension = 2;

set domain_size = "1 1";
set target_samples_per_class = 163;

if(0) then
set domain_size = "1 0.5";
set target_samples_per_class = 81;
endif

set num_classes = 2;
set priority = "0 0";

set dot_radius = 0.005;

set object_scale = 0.04
set window_size = 1024; #768;
set background_color = "data/free_grass_texture.ppm"; #1;
set batch_mode = 1;
set boundary_condition = 1; # none or toroidal
set fig_or_ppm = 1; # 0 for fig, 1 for ppm

set flower0 = red_flower;
set flower1 = yellow_flower;

set flower0 = flower_red_rose;
set flower0 = flower_dahlia_pink;
set flower1 = flower_buttercup_yellow;
set flower1 = flower_cactus_white;
set flower1 = flower_daisy_white;

if(${fig_or_ppm} == 0) then
set object_files = " ${flower0}.gif ${flower1}.gif";
set output_file_ext = "fig";
else
set object_files = " ${outputdir}/${flower0}.ppm ${outputdir}/${flower1}.ppm";
set output_file_ext = "ppm";
endif

set object_name = "flower";

set object_sizes = "${object_scale} ${object_scale}";

set k_value = 100;
@ k_value = -${target_samples_per_class} * ${num_classes};
#set k_value = 16.67;

foreach option (0 1 2)

if(${option} == 0) then # individual class single-class Poisson
set r_value = 0.0565685425;
set r_values = "${r_value} 0 ${r_value}";
set r_text = 0p04sqrt20p000p04sqrt2;

else if(${option} == 1) then # entire set single-class Poisson
set r_value = 0.04;
set r_values = "${r_value} ${r_value} ${r_value}";
set r_text = 0p040p040p04;

else if(${option} == 2) then # our method

set r_values = "0.0565685425 0.04 0.0565685425";
set r_text = 0p04sqrt20p040p04sqrt2;

else
echo unknown option;
exit(1);
endif

if(${option} == 0) then

set list_file_name = ${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}_list.txt

echo "" > ${list_file_name}

foreach id (0 1 all)
set output_root_name = poisson_${dimension}d_${num_classes}c_${r_text}_${id}

if(${id} == 'all') then

src/Main/Release/MergeSamples ${list_file_name} ${dimension} > ${outputdir}/${output_root_name}.txt

rm ${list_file_name};

else
sleep 1;

src/Main/Release/DartThrowing $dimension 1 0 $r_value -${target_samples_per_class} $domain_size > ${outputdir}/${output_root_name}.txt

echo ${outputdir}/${output_root_name}.txt >> ${list_file_name}
endif

end

else

echo src/Main/Release/DartThrowing $dimension $num_classes $priority $r_values $k_value $domain_size
src/Main/Release/DartThrowing $dimension $num_classes $priority $r_values $k_value $domain_size > ${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}.txt

foreach id (0 1 all)

set output_root_name = poisson_${dimension}d_${num_classes}c_${r_text}_${id}

if(${id} == 'all') then
cp ${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}.txt ${outputdir}/${output_root_name}.txt
else
#grep "^${id}" ${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}.txt > ${outputdir}/${output_root_name}.txt
endif

end

endif # ${option} == 0

foreach id (0 1 all)

set input_root_name = poisson_${dimension}d_${num_classes}c_${r_text}_${id};
set output_root_name = ${object_name}_uniform_${dimension}d_${num_classes}c_${r_text}_${id};

if(${id} != 'all') then
grep "^${id}" ${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}_all.txt > ${outputdir}/${input_root_name}.txt
endif

src/Main/Release/DrawSamples ${outputdir}/${input_root_name}.txt ${outputdir}/${output_root_name}.fig ${dimension} ${dot_radius} 1 ${domain_size}
src/Scripts/fig2eps -f ${outputdir}/${output_root_name}.fig
epstopdf ${outputdir}/${output_root_name}.eps --outfile ${outputdir}/${output_root_name}.pdf

src/GPU/Release/DrawObjects ${outputdir}/${input_root_name}.txt ${outputdir}/${output_root_name}_objects.${output_file_ext} ${dimension} ${domain_size} ${num_classes} ${object_files} ${object_sizes} ${window_size} ${background_color} ${batch_mode} ${boundary_condition}

if(${fig_or_ppm} == 0) then
src/Scripts/fig2eps -f ${outputdir}/${output_root_name}_objects.fig
epstopdf ${outputdir}/${output_root_name}_objects.eps --outfile ${outputdir}/${output_root_name}_objects.pdf
else

if(${background_color} == 0) then
set transparent_color = 'black';
else
set transparent_color = 'white';
endif

convert ${outputdir}/${output_root_name}_objects.ppm -transparent ${transparent_color} ${outputdir}/${output_root_name}_objects.gif
endif

end #foreach id

end #foreach option

endif

# R1G2B1 sensor layout
if(0) then

set dimension = 2;
set num_classes = 3;
set priority = "0 0 0";

set num_iterations = 3;

set domain_size = "1 1";

set grid_size_1d = 20;
set dot_radius = 0.02;
set grid_size = "${grid_size_1d} ${grid_size_1d}";

set input_base_name = r1g2b1_grid_${grid_size_1d}x${grid_size_1d};
set output_base_name = r1g2b1_dart_${grid_size_1d}x${grid_size_1d};

src/Tiling/Release/R1G2B1GridSampling ${domain_size} ${grid_size} > data/${input_base_name}.txt

set iteration = 0;
src/Main/Release/ResetClassLabel ${dimension} data/${input_base_name}.txt > data/${output_base_name}_${iteration}.txt

while(${iteration} < ${num_iterations})

set previous_iteration = ${iteration};
@ iteration = ${iteration} + 1;

if(${iteration} == 1) then
set r_values = "0.098 0.04 0.04 0.07 0.04 0.098"; # 1:1/sqrt(2):1
set k_value = 0.63;
else if(${iteration} == 2) then
set r_values = "0.07 0.028 0.028 0.05 0.028 0.07"; # 1:1/sqrt(2):1
set k_value = 0.80;
else if(${iteration} == 3) then
set r_values = "0.04 0.016 0.016 0.028 0.016 0.04"; # 1:1/sqrt(2):1
set k_value = 0.9999;
else
echo unhandled iteration
exit(1);
endif

src/Main/Release/DiscreteDartThrowing $dimension $num_classes $priority $r_values $k_value $domain_size data/${output_base_name}_${previous_iteration}.txt > data/${output_base_name}_${iteration}.txt

end # while(${iteration} < ${num_iterations})

cp data/${output_base_name}_${num_iterations}.txt data/${output_base_name}.txt 

foreach name (${input_base_name} ${output_base_name})

src/Main/Release/DrawSamples data/${name}.txt data/${name}.fig 2 ${dot_radius} 1 ${domain_size}
src/Scripts/fig2eps -f data/${name}.fig
epstopdf data/${name}.eps --outfile data/${name}.pdf

end # foreach name

endif

# R1G2B1 spectrum
if(0) then

set outputdir = data;

set dimension = 2;
set num_classes = 3;
set priority = "0 0 0";

set num_iterations = 3;

set domain_size = "1 1";

set grid_size_1d = 80;
set dot_radius = 0.005;
set grid_size = "${grid_size_1d} ${grid_size_1d}";

set input_base_name = r1g2b1_grid_${grid_size_1d}x${grid_size_1d};
set output_base_name = r1g2b1_dart_${grid_size_1d}x${grid_size_1d};

set num_runs = 10;
set sft_size = 512;
set sft_scale = 512;

set i = 0;
while(${i} < ${num_runs})

src/Tiling/Release/R1G2B1GridSampling ${domain_size} ${grid_size} > ${outputdir}/${input_base_name}_${i}.txt

set iteration = 0;
src/Main/Release/ResetClassLabel ${dimension} ${outputdir}/${input_base_name}_${i}.txt > ${outputdir}/${output_base_name}_${i}_${iteration}.txt

while(${iteration} < ${num_iterations})

set previous_iteration = ${iteration};
@ iteration = ${iteration} + 1;

if(${iteration} == 1) then
set r_values = "0.0245 0.01 0.01 0.0175 0.01 0.0245"; # 1:1/sqrt(2):1
set k_value = 0.63;
else if(${iteration} == 2) then
set r_values = "0.0174 0.007 0.007 0.0124 0.007 0.0174"; # 1:1/sqrt(2):1
set k_value = 0.80;
else if(${iteration} == 3) then
set r_values = "0.01 0.004 0.004 0.007 0.004 0.01"; # 1:1/sqrt(2):1
set k_value = 0.9999;
else
echo unhandled iteration
exit(1);
endif

src/Main/Release/DiscreteDartThrowing $dimension $num_classes $priority $r_values $k_value $domain_size ${outputdir}/${output_base_name}_${i}_${previous_iteration}.txt > ${outputdir}/${output_base_name}_${i}_${iteration}.txt

sleep 1;

end # while(${iteration} < ${num_iterations})

@ i = ${i} + 1;
end

# spectrum
foreach select_id (0 1)

set all_pfm_files = "";
set i = 0;
while(${i} < ${num_runs})

set output_name = ${output_base_name}_${i}_${num_iterations};

src/Main/Release/SampleSelector ${outputdir}/${output_name}.txt ${dimension} ${select_id} > ${outputdir}/${output_name}_selected.txt

../BlueNoise/src/Main/Release/SFT ${outputdir}/${output_name}_selected.txt ${outputdir}/${output_name}.pfm ${dimension} 1 1 ${sft_size} ${sft_scale}

set all_pfm_files = "${all_pfm_files} ${outputdir}/${output_name}.pfm";

@ i = ${i} + 1;
end

echo ${all_pfm_files} > ${outputdir}/all_pfm_files_list.txt;

src/Sain/Release/RadialPlot data/all_pfm_files_list.txt ${outputdir}/${output_base_name}_${select_id}_average${num_runs}.pfm ${outputdir}/${output_base_name}_${select_id}_average${num_runs}_mean.txt ${outputdir}/${output_base_name}_${select_id}_average${num_runs}_variance.txt
rm data/all_pfm_files_list.txt;

../BlueNoise/src/Main/Release/PFM2PPM ${outputdir}/${output_base_name}_${select_id}_average${num_runs}.pfm ${outputdir}/${output_base_name}_${select_id}_average${num_runs}.ppm 1 1 1

end #foreach select_id

endif

# R1G2B1 zoneplate
if(0) then

set outputdir = data;

set dimension = 2;
set num_classes = 3;
set priority = "0 0 0";

set num_iterations = 3;

set domain_size = "1 1";

set grid_size_1d = 80;
set dot_radius = 0.01;

# for tiling
set num_tile_per_dim = 4;
set scaling = 0.25;
set num_tiles = "${num_tile_per_dim} ${num_tile_per_dim}";

# for zone plate
set function_offset = 0;
set function_scale = 10;
set output_resolution = 320;
set filter_gaussian_std = 1.5;

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
set r_values = "0.0245 0.01 0.01 0.0175 0.01 0.0245"; # 1:1/sqrt(2):1
set k_value = 0.63;
else if(${iteration} == 2) then
set r_values = "0.0174 0.007 0.007 0.0124 0.007 0.0174"; # 1:1/sqrt(2):1
set k_value = 0.80;
else if(${iteration} == 3) then
set r_values = "0.01 0.004 0.004 0.007 0.004 0.01"; # 1:1/sqrt(2):1
set k_value = 0.9999;
else
echo unhandled iteration
exit(1);
endif

src/Main/Release/DiscreteDartThrowing $dimension $num_classes $priority $r_values $k_value $domain_size ${outputdir}/${output_base_name}_${previous_iteration}.txt > ${outputdir}/${output_base_name}_${iteration}.txt

end # while(${iteration} < ${num_iterations})

cp ${outputdir}/${output_base_name}_${num_iterations}.txt ${outputdir}/${output_base_name}.txt 

foreach base_name (${input_base_name} ${output_base_name})

set output_name = ${base_name};
set output_tiling_name = ${base_name}_tiling${num_tile_per_dim};

src/Main/Release/TileSamples ${outputdir}/${output_name}.txt ${dimension} ${scaling} ${num_tiles} ${domain_size} > ${outputdir}/${output_tiling_name}.txt;

foreach class (0 1 2)
set select_ids = ${class};

src/Main/Release/SampleSelector ${outputdir}/${output_tiling_name}.txt ${dimension} ${select_ids} > ${outputdir}/${output_tiling_name}_${class}.txt

src/Sain/Release/ZonePlate ${dimension} ${outputdir}/${output_tiling_name}_${class}.txt ${outputdir}/${output_tiling_name}_${class}_zoneplate.ppm ${function_offset} ${function_scale} ${output_resolution} ${filter_gaussian_std}

../BlueNoise/src/Main/Release/PPM2PFM ${outputdir}/${output_tiling_name}_${class}_zoneplate.ppm ${outputdir}/${output_tiling_name}_${class}_zoneplate.pfm
end

# merge
src/Sain/Release/Merge3PFM ${outputdir}/${output_tiling_name}_0_zoneplate.pfm ${outputdir}/${output_tiling_name}_1_zoneplate.pfm ${outputdir}/${output_tiling_name}_2_zoneplate.pfm ${outputdir}/${output_tiling_name}_merge_zoneplate.pfm 

echo ${output_tiling_name}_merge_zoneplate.ppm
src/Sain/Release/PFM2PPM ${outputdir}/${output_tiling_name}_merge_zoneplate.pfm ${outputdir}/${output_tiling_name}_merge_zoneplate.ppm 1 1 2

end # foreach base_name

endif

# timing measurement
if(0) then

set outputdir = data;

set dimension = 2;

foreach exe_option (0)

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

set k_value = 4;
set domain_size = "1 1";

set r_value = 0.02;
set r_text = 0p02;

set dot_radius = 0.002;

set num_runs = 10;

set which_run = 0;
while(${which_run} < ${num_runs})

set timing_file_name = ${outputdir}/"timing_${output_name}_${r_text}_${which_run}.txt";

echo "" > ${timing_file_name}

foreach num_classes (8 13 17 256) #(1 2 3 4 5 6 7 8)

set priority = "";
set r_values = "";

set i = 0;
while(${i} < ${num_classes})

set priority = "${priority} 0";
set r_values = "${r_values} ${r_value}";

@ i = ${i} + 1;
end

echo src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size
(src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size > ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${r_text}.txt) |& grep "samples" >>& ${timing_file_name}

echo ${timing_file_name}

if(0) then
src/Main/Release/DrawSamples ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${r_text}.txt ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${r_text}.fig 2 ${dot_radius} 1 ${domain_size}
src/Scripts/fig2eps -f ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${r_text}.fig
epstopdf ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${r_text}.eps --outfile ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${r_text}.pdf
endif

end # foreach num_classes

@ which_run = ${which_run} + 1;
end

end # foreach exe_option
endif

# linear subset for totality test
if(0) then

set outputdir = data;

set exe_name = DartThrowing;
set output_name = grid;

set dimension = 2;
set domain_size = "1 1";

set k_value = 0.65;

set num_runs = 10;
set sft_size = 512;
set sft_scale = 512;

set r_value_spec = (16 08 04 02 01);
set c1_value_spec = (16 0715541753 0349148624 0173544366 00866448578);

set num_classes = $#r_value_spec;
set one_class = 1;

set r_values = "";
set r_text = "";
set priority = "";

set i = 1;
while(${i} <= ${num_classes})
set r_values = "${r_values} 0.$r_value_spec[${i}]";
set r_text = "${r_text}_0p$r_value_spec[${i}]";
set priority = "${priority} 0";

@ i = ${i} + 1;
end

set output_base_name = ${output_name}_${dimension}d_${num_classes}c${r_text};

# my results
set i = 0;
while(${i} < ${num_runs})

set output_root_name = ${output_base_name}_${i};

src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size > ${outputdir}/${output_root_name}.txt

sleep 1;

@ i = ${i} + 1;
end

# my results pfm
set ids_so_far = "";
set select_id = "";
set r_text_so_far = "";

set class = 0;
while(${class} < ${num_classes})

set ids_so_far = "${ids_so_far} ${class}";
set select_id = "${select_id}${class}";

@ class = ${class} + 1;
set r_text_so_far = "${r_text_so_far}$r_value_spec[${class}]";

set all_pfm_files = "";
set i = 0;
while(${i} < ${num_runs})

set output_root_name = ${output_base_name}_${i};

src/Main/Release/SampleSelector ${outputdir}/${output_root_name}.txt ${dimension} ${ids_so_far} > ${outputdir}/${output_root_name}_selected.txt

../BlueNoise/src/Main/Release/SFT ${outputdir}/${output_root_name}_selected.txt ${outputdir}/${output_root_name}.pfm ${dimension} 1 1 ${sft_size} ${sft_scale}

set all_pfm_files = "${all_pfm_files} ${outputdir}/${output_root_name}.pfm";

@ i = ${i} + 1;
end

echo ${all_pfm_files} > ${outputdir}/all_pfm_files_list.txt;

src/Sain/Release/RadialPlot data/all_pfm_files_list.txt ${outputdir}/${output_base_name}_${select_id}_average${num_runs}.pfm ${outputdir}/${output_base_name}_${select_id}_average${num_runs}_mean.txt ${outputdir}/${output_base_name}_${select_id}_average${num_runs}_variance.txt
rm data/all_pfm_files_list.txt;

../BlueNoise/src/Main/Release/PFM2PPM ${outputdir}/${output_base_name}_${select_id}_average${num_runs}.pfm ${outputdir}/${output_base_name}_${select_id}_average${num_runs}.ppm 1 1 1

end

# ground truth
set class = 1;
while(${class} <= ${num_classes})

set r_values = "0.$c1_value_spec[${class}]";
set r_text = "0p$c1_value_spec[${class}]";
set priority = "0";
set ids_so_far = "0";
set select_id = "0";

set output_base_name = ${output_name}_${dimension}d_${one_class}c${r_text};

set all_pfm_files = "";
set i = 0;
while(${i} < ${num_runs})

set output_root_name = ${output_base_name}_${i};

src/Main/Release/${exe_name} $dimension $one_class $priority $r_values $k_value $domain_size > ${outputdir}/${output_root_name}.txt

sleep 1;

src/Main/Release/SampleSelector ${outputdir}/${output_root_name}.txt ${dimension} ${ids_so_far} > ${outputdir}/${output_root_name}_selected.txt

../BlueNoise/src/Main/Release/SFT ${outputdir}/${output_root_name}_selected.txt ${outputdir}/${output_root_name}.pfm ${dimension} 1 1 ${sft_size} ${sft_scale}

set all_pfm_files = "${all_pfm_files} ${outputdir}/${output_root_name}.pfm";

@ i = ${i} + 1;
end

echo ${all_pfm_files} > ${outputdir}/all_pfm_files_list.txt;

src/Sain/Release/RadialPlot data/all_pfm_files_list.txt ${outputdir}/${output_base_name}_${select_id}_average${num_runs}.pfm ${outputdir}/${output_base_name}_${select_id}_average${num_runs}_mean.txt ${outputdir}/${output_base_name}_${select_id}_average${num_runs}_variance.txt
rm data/all_pfm_files_list.txt;

../BlueNoise/src/Main/Release/PFM2PPM ${outputdir}/${output_base_name}_${select_id}_average${num_runs}.pfm ${outputdir}/${output_base_name}_${select_id}_average${num_runs}.ppm 1 1 1

@ class = ${class} + 1;
end

endif


# diff comparing ground truth and artificial
if(0) then

set outputdir = data;

set dimension = 2;
set domain_size = "1 1";

set r_value = 04;
set r_variance = 004;

set num_runs = 10;
set num_pairs = 52003;
@ total_num_pairs = ${num_pairs} * ${num_runs};

# for DFT
set gen_dft = 1; # set to 1 to run stuff, could be slow
set dft_freq = 128;
@ dft_size = ${dft_freq} * 2;
set dft_scale = ${dft_size};

# for histogram
set hist_freq = 1;
set num_bins = 800;

# ground truth
set k_value = 0.67;

set num_classes = 1;
set priority = "0";
set r_values = 0.${r_value};
set r_text = 0p${r_value};

set exe_name = DartThrowing;
set output_name = grid;

set ground_truth_name = ${output_name}_${dimension}d_${num_classes}c_${r_text};
set ground_truth_pair_name = ${output_name}_${dimension}d_${num_classes}c_${r_text}_${num_pairs};

set i = 0;
rm ${outputdir}/${ground_truth_pair_name}.txt;

while(${i} < ${num_runs})

@ i = ${i} + 1;

src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size > ${outputdir}/${ground_truth_name}.txt

src/Main/Release/SampleSelector ${outputdir}/${ground_truth_name}.txt ${dimension} 0 > ${outputdir}/${ground_truth_name}_selected.txt

src/Theory/Release/DiffSampling ${outputdir}/${ground_truth_name}_selected.txt ${dimension} ${num_pairs} >> ${outputdir}/${ground_truth_pair_name}.txt

sleep 1;

end

# diff generator

set option = "hybrid";

set diff_synth_pair_name_all = "";

foreach length_sign ('-' '+')
foreach variance_sign ('-' '+')

set diff_synth_pair_name = diff_${option}_${dimension}d_${length_sign}0p${r_value}_${variance_sign}0p${r_variance}_${num_pairs}

set diff_synth_pair_name_all = "${diff_synth_pair_name_all} ${diff_synth_pair_name}";

src/Theory/Release/DiffThrowing ${option} ${dimension} ${length_sign}0.${r_value} ${variance_sign}0.${r_variance} ${total_num_pairs} ${domain_size} > ${outputdir}/${diff_synth_pair_name}.txt

end # foreach variance_sign
end # foreach length_sign

# comparison
foreach diff_file_base_name (${ground_truth_pair_name} ${diff_synth_pair_name_all})

# DFT
if(${gen_dft}) then
src/Sain/Release/DFT ${outputdir}/${diff_file_base_name}.txt ${outputdir}/${diff_file_base_name}.pfm ${dimension} 1 1 ${dft_size} ${dft_scale} 

src/Sain/Release/PFM2PPM ${outputdir}/${diff_file_base_name}.pfm ${outputdir}/${diff_file_base_name}.ppm 1 1 0

echo ${outputdir}/${diff_file_base_name}.pfm > ${outputdir}/all_pfm_files_list.txt;
src/Sain/Release/RadialPlot ${outputdir}/all_pfm_files_list.txt ${outputdir}/${diff_file_base_name}_all.pfm ${outputdir}/${diff_file_base_name}_mean.txt ${outputdir}/${diff_file_base_name}_variance.txt 
rm ${outputdir}/all_pfm_files_list.txt;

src/Scripts/txt2dat ${outputdir}/${diff_file_base_name}_mean.txt ${outputdir}/${diff_file_base_name}_variance.txt ${total_num_pairs} ${outputdir}/${diff_file_base_name}_mean.dat ${outputdir}/${diff_file_base_name}_variance.dat 1
endif

# histogram
echo ${outputdir}/${diff_file_base_name}.txt > ${outputdir}/all_sample_file_list.txt;

foreach which_dimension_text ("0" "01")

if(${which_dimension_text} == "0") then
set which_dimension = "0";
else
set which_dimension = "0 1";
endif

src/Theory/Release/DiffHistogram ${outputdir}/all_sample_file_list.txt $dimension $which_dimension ${hist_freq} $num_bins > ${outputdir}/${diff_file_base_name}_${which_dimension_text}_histogram.txt
end

rm ${outputdir}/all_sample_file_list.txt;

end # foreach diff_file_base_name

# plot stuff
foreach diff_synth_pair_name (${diff_synth_pair_name_all})

set output_base_name = truth_${diff_synth_pair_name};

# radial plot
set output_name = ${output_base_name}_mean;

echo set terminal postscript eps >> foo.gp
echo set output \'${outputdir}/${output_name}.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp
echo set autoscale y >> foo.gp
echo plot "[1:${dft_freq}]" \"${outputdir}/${diff_synth_pair_name}_mean.dat\" with line linetype rgb \"red\", \"${outputdir}/${ground_truth_pair_name}_mean.dat\" with line linetype rgb \"\#009900\" >> foo.gp

wgnuplot foo.gp
rm foo.gp;

epstopdf ${outputdir}/${output_name}.eps --outfile ${outputdir}/${output_name}.pdf;

echo ${outputdir}/${output_name}.pdf;

# histogram plot
foreach which_dimension_text ("0" "01")

if(${which_dimension_text} == "0") then
set x_max = "1";
else
set x_max = "1.41245";
endif

set output_name = ${output_base_name}_${which_dimension_text}_histogram;

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
echo plot "[0:${x_max}]" \"${outputdir}/${diff_synth_pair_name}_${which_dimension_text}_histogram.txt\" title \'${option}\' with lines linetype rgb \"red\", \"${outputdir}/${ground_truth_pair_name}_${which_dimension_text}_histogram.txt\" title \'truth\' with lines linetype rgb \"\#009900\" >> foo.gp

wgnuplot foo.gp
rm foo.gp;

epstopdf ${outputdir}/${output_name}.eps --outfile ${outputdir}/${output_name}.pdf;

echo ${outputdir}/${output_name}.pdf;
end # foreach which_dimension_text

end # foreach diff_synth_pair_name 

endif

# green noise
if(0) then

set outputdir = data;

set dimension = 2;
set domain_size = "1 1";

set num_classes = 1;
set priority = 0;

set k_value = 0.70;

set large_r_value = 04;
set large_r_variance = 02;
set small_r_value = 01;
set dot_radius = 0.001;
set conflict_metric = 0; # mean

# sft
set sft_freq = 256;
@ sft_size = ${sft_freq} * 2;
set sft_scale = ${sft_size};

# for histogram
set hist_freq = 1;
set num_bins = 800;
set normalization = 1;

# number of runs
set num_runs = 10;

set cluster_sample_root_name = grid_${dimension}d_${num_classes}c_0p${large_r_value};
set final_sample_root_name = adaptive_${dimension}d_${num_classes}c_0p${large_r_value}_0p${large_r_variance}_0p${small_r_value}
set white_sample_root_name = white_${dimension}d_${num_classes}c_0p${large_r_value}_0p${large_r_variance}_0p${small_r_value}

set all_sample_files = "";
set all_pfm_files = "";
set run = 0;
while(${run} < ${num_runs})

# generate cluster centers

set exe_name = DartThrowing;
#set output_name = grid;
set cluster_sample_base_name = ${cluster_sample_root_name}_${run}

src/Main/Release/${exe_name} $dimension $num_classes $priority 0.${large_r_value} $k_value $domain_size > ${outputdir}/${cluster_sample_base_name}.txt

# generate importance map
set field_base_name = blobs_${dimension}d_${num_classes}c_0p${large_r_value}_0p${large_r_variance}

set cluster_samples = `src/Main/Release/SampleSelector ${outputdir}/${cluster_sample_base_name}.txt ${dimension} 0`;

@ num_samples = $#cluster_samples / ${dimension};

echo ${num_samples} > ${outputdir}/${field_base_name}.txt
echo ${dimension} >> ${outputdir}/${field_base_name}.txt

set i = 0;
while(${i} < $#cluster_samples)

@ i = ${i} + 1;
set sample_location = "$cluster_samples[${i}]";
@ i = ${i} + 1;
set sample_location = "${sample_location} $cluster_samples[${i}]";

echo "1 ${sample_location} 0.${large_r_variance} 0.${large_r_variance}" >> ${outputdir}/${field_base_name}.txt;
end

echo ${outputdir}/${field_base_name}.txt

set output_field_name = ${field_base_name};
src/Sain/Release/MakeBlobs ${outputdir}/${output_field_name}.txt ${outputdir}/${output_field_name}.pfm 512 1 ${domain_size};
if(${num_runs} == 1) then
src/Sain/Release/PFM2PPM ${outputdir}/${output_field_name}.pfm ${outputdir}/${output_field_name}.ppm 1 1 2;
endif

# generate samples via adaptive sampling
set exe_name = AdaptiveDartThrowing;
#set output_name = adaptive;

set final_sample_base_name = ${final_sample_root_name}_${run}

src/Main/Release/${exe_name} $dimension $num_classes $priority 0.${small_r_value} $k_value $domain_size ${conflict_metric} ${outputdir}/${output_field_name}.pfm > ${outputdir}/${final_sample_base_name}.txt

set all_sample_files = "${all_sample_files} ${outputdir}/${final_sample_base_name}.txt";

set final_samples = `src/Main/Release/SampleSelector ${outputdir}/${final_sample_base_name}.txt ${dimension} 0`;
@ num_samples = $#final_samples / ${dimension};

if(${num_runs} == 1) then
src/Main/Release/DrawSamples ${outputdir}/${final_sample_base_name}.txt ${outputdir}/${final_sample_base_name}.fig ${dimension} ${dot_radius} 1 ${domain_size}
src/Scripts/fig2eps -f ${outputdir}/${final_sample_base_name}.fig
epstopdf ${outputdir}/${final_sample_base_name}.eps --outfile ${outputdir}/${final_sample_base_name}.pdf
echo ${outputdir}/${final_sample_base_name}.pdf
endif

# SFT
src/Main/Release/SampleSelector ${outputdir}/${final_sample_base_name}.txt ${dimension} 0 > ${outputdir}/${final_sample_base_name}_selected.txt
src/Sain/Release/SFT ${outputdir}/${final_sample_base_name}_selected.txt ${outputdir}/${final_sample_base_name}.pfm ${dimension} 1 1 ${sft_size} ${sft_scale}

set all_pfm_files = "$all_pfm_files ${outputdir}/${final_sample_base_name}.pfm";

@ run = ${run} + 1;
end

echo ${all_pfm_files} > data/all_pfm_files_list.txt;
src/Sain/Release/RadialPlot data/all_pfm_files_list.txt ${outputdir}/${final_sample_root_name}_average${num_runs}.pfm ${outputdir}/${final_sample_root_name}_average${num_runs}_mean.txt ${outputdir}/${final_sample_root_name}_average${num_runs}_variance.txt
rm data/all_pfm_files_list.txt;

src/Sain/Release/PFM2PPM ${outputdir}/${final_sample_root_name}_average${num_runs}.pfm ${outputdir}/${final_sample_root_name}_average${num_runs}.ppm 1 1 0

src/Scripts/txt2dat ${outputdir}/${final_sample_root_name}_average${num_runs}_mean.txt ${outputdir}/${final_sample_root_name}_average${num_runs}_variance.txt ${num_samples} ${outputdir}/${final_sample_root_name}_average${num_runs}_mean.dat ${outputdir}/${final_sample_root_name}_average${num_runs}_variance.dat 1

echo set terminal postscript eps >> foo.gp
echo set output \'${outputdir}/${final_sample_root_name}_average${num_runs}_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp
echo set autoscale y >> foo.gp
echo plot "[1:${sft_freq}]" \"${outputdir}/${final_sample_root_name}_average${num_runs}_mean.dat\" with line linetype rgb \"red\" >> foo.gp

wgnuplot foo.gp
rm foo.gp

set output_name = ${final_sample_root_name}_average${num_runs}_mean;
epstopdf ${outputdir}/${output_name}.eps --outfile ${outputdir}/${output_name}.pdf;
echo ${outputdir}/${output_name}.pdf;

# white noise
set all_white_files = "";
set run = 0;
while(${run} < ${num_runs})

set white_sample_base_name = ${white_sample_root_name}_${run}

../BlueNoise/src/Main/Release/RandomDart ${dimension} ${num_samples} > ${outputdir}/${white_sample_base_name}.txt 

sleep 1;

set all_white_files = "${all_white_files} ${outputdir}/${white_sample_base_name}.txt";

@ run = ${run} + 1;
end

# histogram
foreach option (${final_sample_root_name} ${white_sample_root_name})

if(${option} == ${final_sample_root_name}) then
echo ${all_sample_files} > ${outputdir}/all_sample_file_list.txt;
else
echo ${all_white_files} > ${outputdir}/all_sample_file_list.txt;
endif

foreach which_dimension_text ("0" "01")

if(${which_dimension_text} == "0") then
set which_dimension = "0";
else
set which_dimension = "0 1";
endif

src/Theory/Release/Histogram ${outputdir}/all_sample_file_list.txt $dimension $which_dimension ${hist_freq} $num_bins > ${outputdir}/${option}_${which_dimension_text}_histogram.txt

end # foreach which_dimension_text

rm ${outputdir}/all_sample_file_list.txt;

end # foreach option

# histogram plot
foreach which_dimension_text ("0" "01")

if(${normalization}) then
src/Scripts/normalize_hist ${outputdir}/${final_sample_root_name}_${which_dimension_text}_histogram.txt ${outputdir}/${white_sample_root_name}_${which_dimension_text}_histogram.txt ${outputdir}/${final_sample_root_name}_${which_dimension_text}_histogram_normalized.txt
endif

if(${which_dimension_text} == "0") then
set x_max = "1";
else
set x_max = "1.41245";
endif

set output_name = ${final_sample_root_name}_${which_dimension_text}_histogram;
if(${normalization}) then
set output_name = "${output_name}_normalized";
endif

echo "" > foo.gp;

echo set terminal postscript eps >> foo.gp
echo set output \'${outputdir}/${output_name}.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"\|coordinate difference\|\" >> foo.gp
if(${normalization}) then
echo set ylabel \"normalized density\" >> foo.gp
else
echo set ylabel \"density\" >> foo.gp
endif
echo set key off >> foo.gp
echo set autoscale y >> foo.gp
set max_bin = "";
@ max_bin = $num_bins - 1;

if(${normalization}) then
echo plot "[0:${x_max}]" \"${outputdir}/${final_sample_root_name}_${which_dimension_text}_histogram_normalized.txt\" with lines linetype rgb \"red\" >> foo.gp
else
echo plot "[0:${x_max}]" \"${outputdir}/${final_sample_root_name}_${which_dimension_text}_histogram.txt\" with lines linetype rgb \"red\", \"${outputdir}/${white_sample_root_name}_${which_dimension_text}_histogram.txt\" with lines linetype rgb \"\#009900\" >> foo.gp
endif

wgnuplot foo.gp
rm foo.gp;

epstopdf ${outputdir}/${output_name}.eps --outfile ${outputdir}/${output_name}.pdf;

echo ${outputdir}/${output_name}.pdf;
end # foreach which_dimension_text

endif


######## post rebuttal stuff ########

# show that generating all classes together produces better visual results than sequentially
if(0) then

set dimension = 2;
set exe_name = DartThrowing;
set output_name = priority;

set num_classes = 128;

set r_value = 0.04;
set r_text = 0p04;

set k_value = 0.65;
set domain_size = "1 1";

set dot_radius = 0.002;

foreach option (0 1)

set priority = "";
set r_values = "";
set which_p = 0;
while(${which_p} < ${num_classes})
set r_values = "${r_value} ${r_values}";
if(${option} == 0) then
set priority = "${priority} 0";
else
set priority = "${priority} ${which_p}";
endif
@ which_p = ${which_p} + 1;
end

if(${option} == 0) then
set p_text = "together";
else
set p_text = "separate";
endif

set output_root_name = ${output_name}_${p_text}_${dimension}d_${num_classes}c_${r_text};

echo src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size
src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size > ${outputdir}/${output_root_name}.txt

# the last class for worst case scenario
set last_root_name = ${output_root_name}_last;
@ last_class_id = ${num_classes} - 1;
grep "^${last_class_id}" ${outputdir}/${output_root_name}.txt > ${outputdir}/${last_root_name}.txt

# plot all
foreach plot_root_name (${output_root_name} ${last_root_name})
src/Main/Release/DrawSamples ${outputdir}/${plot_root_name}.txt ${outputdir}/${plot_root_name}.fig 2 ${dot_radius} 1 ${domain_size}
src/Scripts/fig2eps -f ${outputdir}/${plot_root_name}.fig
epstopdf ${outputdir}/${plot_root_name}.eps --outfile ${outputdir}/${plot_root_name}.pdf
end

end # foreach option

endif

######## new stuff added for SIGGRAPH 2010 ########

# class uniformity measurement for all possible number of subsets under a uniform parameter setting for hard dart throwing
if(0) then

set dimension = 2;

set num_class = 64;

set r_value = 0.02;
set r_text = 0p02;

set target_samples = 1300;
set k_value = 0.671;
set domain_size = "1 1";

set priority = "";
set r_values = "";
set i = 0;

while(${i} < ${num_class})
set priority = "$priority 0";
set r_values = "$r_values $r_value";
@ i = ${i} + 1;
end

set output_base_name = poisson_${dimension}d_${num_class}c_${r_text};

src/Main/Release/DartThrowing ${dimension} ${num_class} ${priority} ${r_values} ${k_value} ${domain_size} > ${outputdir}/${output_base_name}.txt

echo "" > ${outputdir}/${output_base_name}_minspacing.txt
set ids = "";
set i = 0;
while(${i} < ${num_class})
set ids = "${ids} ${i}";

src/Main/Release/SampleSelector ${outputdir}/${output_base_name}.txt ${dimension} ${ids} > ${outputdir}/${output_base_name}_select${i}.txt;

src/Main/Release/MinSampleSpacing ${outputdir}/${output_base_name}_select${i}.txt ${dimension} ${domain_size} 1 >> ${outputdir}/${output_base_name}_minspacing.txt

@ i = ${i} + 1;
end

echo done see ${outputdir}/${output_base_name}_minspacing.txt
endif
