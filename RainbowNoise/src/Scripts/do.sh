#!/bin/tcsh -f

set outputdir = data;

# dart throwing
if(0) then

set dimension = 2;

set num_classes = 1;
set priority = 0;
set r_values = 0.01;
set ids = 0;
set r_text = 0p01
set dot_radius = 0.002;

if(0) then
# bad input r values
set num_classes = 2;
set priority = "0 0";
set r_values = "0.01 0.01 0.01";
set ids = "0 1";
set ids = 0;
set r_text = 0p01
set dot_radius = 0.002;
endif

if(0) then
set num_classes = 2;
set priority = "0 0";
set r_values = "0.014 0.01 0.014";
set r_values = "0.014 0.014";
set ids = "0 1";
set ids = 0;
set r_text = 0p01
set dot_radius = 0.002;
endif

if(0) then
# match up final number of samples with above
set num_classes = 1;
set priority = "0";
set r_values = 0.016;
set ids = 0;
set r_text = 0p016
set dot_radius = 0.002;
endif

if(0) then
set num_classes = 6;
set priority = "0 0 0 0 0 0";
set r_values = "0.02 0.02 0.02 0.02 0.02 0.02";
set ids = "0 1 2 3 4 5";
set ids = 0;
set r_text = 0p02
set dot_radius = 0.002;
endif

if(1) then
set num_classes = 3;
set priority = "0 0 0";
set r_values = "0.04 0.02 0.01";
set ids = "0 1 2";
#set ids = 2;
set r_text = 0p02
set dot_radius = 0.002;
endif

set k_value = 4;

set domain_size = "1 1";

set num_relaxations = 0;

echo src/Main/Release/DartThrowing $dimension $num_classes $priority $r_values $k_value $domain_size

src/Main/Release/DartThrowing $dimension $num_classes $priority $r_values $k_value $domain_size > data/poisson_${dimension}d_${num_classes}c_${r_text}.txt

src/Main/Release/SampleSelector data/poisson_${dimension}d_${num_classes}c_${r_text}.txt ${dimension} ${ids} > data/poisson_${dimension}d_${num_classes}c_${r_text}_selected.txt

if(1) then
src/Main/Release/DrawSamples data/poisson_${dimension}d_${num_classes}c_${r_text}.txt data/poisson_${dimension}d_${num_classes}c_${r_text}.fig 2 ${dot_radius} 1 ${domain_size}
src/Scripts/fig2eps -f data/poisson_${dimension}d_${num_classes}c_${r_text}.fig
epstopdf data/poisson_${dimension}d_${num_classes}c_${r_text}.eps --outfile data/poisson_${dimension}d_${num_classes}c_${r_text}.pdf
endif

if(1) then
../BlueNoise/src/Main/Release/DrawSamples data/poisson_${dimension}d_${num_classes}c_${r_text}_selected.txt data/poisson_${dimension}d_${num_classes}c_${r_text}_selected.fig 2 ${dot_radius} 1 ${domain_size}
src/Scripts/fig2eps -f data/poisson_${dimension}d_${num_classes}c_${r_text}_selected.fig
epstopdf data/poisson_${dimension}d_${num_classes}c_${r_text}_selected.eps --outfile data/poisson_${dimension}d_${num_classes}c_${r_text}_selected.pdf
endif

if(0) then
../BlueNoise/src/Main/Release/SFT data/poisson_${dimension}d_${num_classes}c_${r_text}_selected.txt data/poisson_${dimension}d_${num_classes}c_${r_text}.ppm ${dimension} 1 1 256 256
endif

src/Main/Release/LloydRelaxation2D data/poisson_${dimension}d_${num_classes}c_${r_text}.txt ${dimension} ${domain_size} ${num_relaxations} > data/poisson_${dimension}d_${num_classes}c_${r_text}_${num_relaxations}i.txt

if(0) then
src/Main/Release/DrawSamples data/poisson_${dimension}d_${num_classes}c_${r_text}_${num_relaxations}i.txt data/poisson_${dimension}d_${num_classes}c_${r_text}_${num_relaxations}i.fig 2 ${dot_radius} 1 ${domain_size}
src/Scripts/fig2eps -f data/poisson_${dimension}d_${num_classes}c_${r_text}_${num_relaxations}i.fig
epstopdf data/poisson_${dimension}d_${num_classes}c_${r_text}_${num_relaxations}i.eps --outfile data/poisson_${dimension}d_${num_classes}c_${r_text}_${num_relaxations}i.pdf
endif

endif

# relaxation
if(0) then

set num_points = 1600;
set num_iterations = 10;
set boundary_condition = 1;

src/Main/Release/LloydRelaxation2DRandom $num_points $num_iterations ${boundary_condition} > data/relaxation_${num_points}p_${num_iterations}i_${boundary_condition}b.txt

if(1) then
../BlueNoise/src/Main/Release/SFT data/relaxation_${num_points}p_${num_iterations}i_${boundary_condition}b.txt data/relaxation_${num_points}p_${num_iterations}i_${boundary_condition}b.ppm 2 1 1 256 256;
endif

endif

# study white noise
if(0) then

set dimension = 2;
set target_samples = 2000;

set all_pfm_files = "";
set all_pfm_files_count = 0;

foreach i (0 1 2 3 4 5 6 7 8 9)
 ../BlueNoise/src/Main/Release/RandomDart ${dimension} ${target_samples} > data/white_${dimension}d_${target_samples}s_${i}.txt;

../BlueNoise/src/Main/Release/SFT data/white_${dimension}d_${target_samples}s_${i}.txt data/white_${dimension}d_${target_samples}s_${i}.pfm ${dimension} 1 1 256 256;

set all_pfm_files = "$all_pfm_files data/white_${dimension}d_${target_samples}s_${i}.pfm";
@ all_pfm_files_count = ${all_pfm_files_count} + 1;
end

echo ${all_pfm_files} data/all_pfm_files_list.txt
src/Sain/Release/RadialPlot data/all_pfm_files_list.txt data/white_${dimension}d_${target_samples}s_average${all_pfm_files_count}.pfm data/white_${dimension}d_${target_samples}s_average${all_pfm_files_count}_radial_mean.txt data/white_${dimension}d_${target_samples}s_average${all_pfm_files_count}_radial_variance.txt
rm data/all_pfm_files_list.txt;

../BlueNoise/src/Main/Release/PFM2PPM data/white_${dimension}d_${target_samples}s_average${all_pfm_files_count}.pfm data/white_${dimension}d_${target_samples}s_average${all_pfm_files_count}.ppm 1 1 1

endif

# 1D? dart throwing
if(0) then

set source_dimension = 1; # 1 or 2
set dimension = 1;

set num_classes = 1;
set priority = 0;
set p_text = 0;
set ids = 0;
set id = 0;

if(${source_dimension} == 1) then
set r_values = 0.02;
set r_text = 0p02
set target_samples = 38;
set domain_size = "1";
else if(${source_dimension} == 2) then
set r_values = 0.02;
set r_text = 0p02
set target_samples = 1675.8;
set domain_size = "1 1";
else
echo "unhandled source dimension";
exit(1);
endif

set k_value = 100;

set sft_size = 256;

set num_runs = 2560;

set all_pfm_files = "";
set all_pfm_files_count = 0;

set i = 0;
while(${i} < ${num_runs})

src/Main/Release/DartThrowing $source_dimension $num_classes $priority $r_values $k_value $domain_size > data/poisson_${source_dimension}d_${num_classes}c_${p_text}_${r_text}_${i}.txt

if(${source_dimension} > ${dimension}) then
src/Main/Release/ProjectSamples data/poisson_${source_dimension}d_${num_classes}c_${p_text}_${r_text}_${i}.txt ${source_dimension} 1 > data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}.txt
endif

#../BlueNoise/src/Main/Release/RandomDart ${dimension} ${target_samples} > data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}.txt

src/Main/Release/SampleSelector data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}.txt ${dimension} ${id} > data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}_selected${id}.txt

../BlueNoise/src/Main/Release/SFT data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}_selected${id}.txt data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}_selected${id}.pfm ${dimension} 1 ${sft_size} ${sft_size} 

if(0 && (${source_dimension} > ${dimension})) then
src/Main/Release/SampleSelector data/poisson_${source_dimension}d_${num_classes}c_${p_text}_${r_text}_${i}.txt ${source_dimension} ${id} > data/poisson_${source_dimension}d_${num_classes}c_${p_text}_${r_text}_${i}_selected${id}.txt

../BlueNoise/src/Main/Release/SFT data/poisson_${source_dimension}d_${num_classes}c_${p_text}_${r_text}_${i}_selected${id}.txt data/poisson_${source_dimension}d_${num_classes}c_${p_text}_${r_text}_${i}_selected${id}.pfm ${source_dimension} 1 1 ${sft_size} ${sft_size} 
endif

set all_pfm_files = "$all_pfm_files data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}_selected${id}.pfm";
@ all_pfm_files_count = ${all_pfm_files_count} + 1;

@ i = ${i} + 1;
end

echo ${all_pfm_files} > data/all_pfm_files_list.txt;
src/Sain/Release/RadialPlot data/all_pfm_files_list.txt data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_selected${id}_average${all_pfm_files_count}.pfm data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_selected${id}_average${all_pfm_files_count}_radial_mean.txt data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_selected${id}_average${all_pfm_files_count}_radial_variance.txt
rm data/all_pfm_files_list.txt;

src/Scripts/txt2dat data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_selected${id}_average${all_pfm_files_count}_radial_mean.txt data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_selected${id}_average${all_pfm_files_count}_radial_variance.txt ${target_samples} data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_selected${id}_average${all_pfm_files_count}_mean.dat data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_selected${id}_average${all_pfm_files_count}_variance.dat

echo "" > foo.gp

echo set terminal postscript eps >> foo.gp
echo set output \'data/poisson_${source_dimension}s_${dimension}d_${num_classes}c_${p_text}_${r_text}_selected${id}_average${all_pfm_files_count}_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp
echo plot "[1:128] [0:2.0]" \"data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_selected${id}_average${all_pfm_files_count}_mean.dat\" with line linetype rgb \"red\" >> foo.gp

wgnuplot foo.gp

rm foo.gp

#clean up
set i = 0;
while(${i} < ${num_runs})
rm data/poisson_${source_dimension}d_${num_classes}c_${p_text}_${r_text}_${i}.txt;

if(${source_dimension} > ${dimension}) then
rm data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}.txt;
endif

rm data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}_selected${id}.txt;
rm data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_${i}_selected${id}.pfm;

@ i = ${i} + 1;
end

endif

# test DiscreteRelaxSamples
if(0) then

set dimension = 2;
set num_classes = 2;

set r_value = 0.02;
set r_text = 0p02;
set dot_radius = 0.002;

set k_value = 100;
set domain_size = "1 1";

set id = 0;
set num_continuous_iterations = 15;
set num_discrete_iterations = 10;

set priority = "";
set r_values = "";
set foo = 0;

while($foo < $num_classes)
set priority = "$priority 0";
set r_values = "$r_values $r_value";
@ foo = ${foo} + 1;
end

set r_values = "0.1 0.02";
#set r_values = "0.02 0.00755929";

#src/Main/Release/DartThrowing $dimension $num_classes $priority $r_values $k_value $domain_size > data/poisson_${dimension}d_${num_classes}c_${r_text}.txt

src/Main/Release/RandomizeSamples ${dimension} data/poisson_${dimension}d_${num_classes}c_${r_text}.txt > data/poisson_${dimension}d_${num_classes}c_${r_text}_randomized.txt

if(0) then
echo debug
cp data/poisson_${dimension}d_${num_classes}c_${r_text}.txt data/poisson_${dimension}d_${num_classes}c_${r_text}_randomized.txt
endif

grep "^${id}" data/poisson_${dimension}d_${num_classes}c_${r_text}.txt > data/poisson_${dimension}d_${num_classes}c_${r_text}_selected${id}.txt
src/Main/Release/DrawSamples data/poisson_${dimension}d_${num_classes}c_${r_text}_selected${id}.txt data/poisson_${dimension}d_${num_classes}c_${r_text}_selected${id}.fig 2 ${dot_radius} 1 ${domain_size}
src/Scripts/fig2eps -f data/poisson_${dimension}d_${num_classes}c_${r_text}_selected${id}.fig
epstopdf data/poisson_${dimension}d_${num_classes}c_${r_text}_selected${id}.eps --outfile data/poisson_${dimension}d_${num_classes}c_${r_text}_selected${id}.pdf

src/Main/Release/DiscreteRelaxSamples data/poisson_${dimension}d_${num_classes}c_${r_text}_randomized.txt ${dimension} ${domain_size} ${num_continuous_iterations} ${num_discrete_iterations} ${id} 1 > data/poisson_${dimension}d_${num_classes}c_${r_text}_discreterelax${id}.txt


src/Main/Release/DrawSamples data/poisson_${dimension}d_${num_classes}c_${r_text}_discreterelax${id}.txt data/poisson_${dimension}d_${num_classes}c_${r_text}_discreterelax${id}.fig 2 ${dot_radius} 1 ${domain_size}
src/Scripts/fig2eps -f data/poisson_${dimension}d_${num_classes}c_${r_text}_discreterelax${id}.fig
epstopdf data/poisson_${dimension}d_${num_classes}c_${r_text}_discreterelax${id}.eps --outfile data/poisson_${dimension}d_${num_classes}c_${r_text}_discreterelax${id}_iterations${num_continuous_iterations}c${num_discrete_iterations}d.pdf

endif

# discrete dart throwing
if(0) then

set dimension = 2;

set num_classes_source = 1;
set r_values_source = 0.014;
set r_text_source = 0p014;
set priority_source = "0";
set p_text_source = 0;

if(1) then
set num_classes_source = 2;
set r_values_source = "0.02 0.014 0.02";
set r_text_source = 0p02;
set priority_source = "0 0";
set p_text_source = 00;
endif

set num_classes_discrete = 2;
set r_values_discrete = "0.02 0.014 0.02";
set r_text_discrete = 0p02;
set priority_discrete = "0 0";
set p_text_discrete = 00;

set dot_radius = 0.002;

set k_value_source = 10;
set k_value_discrete = 100;

set domain_size = "1 1";

# single class dart throwing
echo ${num_classes_source}-class source dart throwing
src/Main/Release/DartThrowing $dimension ${num_classes_source} ${priority_source} $r_values_source $k_value_source $domain_size > data/poisson_${dimension}d_${num_classes_source}c_${p_text_source}_${r_text_source}.txt

# discrete dart throwing
echo ${num_classes_discrete}-class discrete dart throwing
src/Main/Release/DiscreteDartThrowing $dimension $num_classes_discrete $priority_discrete $r_values_discrete $k_value_discrete $domain_size data/poisson_${dimension}d_${num_classes_source}c_${p_text_source}_${r_text_source}.txt > data/discrete_${dimension}d_${num_classes_discrete}c_${p_text_discrete}_${r_text_discrete}.txt

src/Main/Release/DrawSamples data/discrete_${dimension}d_${num_classes_discrete}c_${p_text_discrete}_${r_text_discrete}.txt data/discrete_${dimension}d_${num_classes_discrete}c_${p_text_discrete}_${r_text_discrete}.fig 2 ${dot_radius} 1 ${domain_size}
src/Scripts/fig2eps -f data/discrete_${dimension}d_${num_classes_discrete}c_${p_text_discrete}_${r_text_discrete}.fig
epstopdf data/discrete_${dimension}d_${num_classes_discrete}c_${p_text_discrete}_${r_text_discrete}.eps --outfile data/discrete_${dimension}d_${num_classes_discrete}c_${p_text_discrete}_${r_text_discrete}.pdf
endif

# R1G2B1
if(0) then

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

src/Tiling/Release/R1G2B1GridSampling ${domain_size} ${grid_size} > data/${input_base_name}.txt

set iteration = 0;
src/Main/Release/ResetClassLabel ${dimension} data/${input_base_name}.txt > data/${output_base_name}_${iteration}.txt

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

src/Main/Release/DiscreteDartThrowing $dimension $num_classes $priority $r_values $k_value $domain_size data/${output_base_name}_${previous_iteration}.txt > data/${output_base_name}_${iteration}.txt

foreach name (${output_base_name}_${iteration})

src/Main/Release/DrawSamples data/${name}.txt data/${name}.fig 2 ${dot_radius} 1 ${domain_size}
src/Scripts/fig2eps -f data/${name}.fig
epstopdf data/${name}.eps --outfile data/${name}.pdf

end # foreach name

end # while(${iteration} < ${num_iterations})

endif

# sampling history
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

set normalize = ${rho_number};
set max_num_time_samples = 1000000;

foreach k_option (0 1)

if(${k_option} == 0) then
set k_value = -${target_samples};
else if(${k_option} == 1) then
set k_value = ${rho_number};
else
echo unknown k_option;
exit(1);
endif

src/Main/Release/DartThrowingHistory $dimension $num_classes $priority $r_values $k_value $domain_size ${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}_${k_option}k_history.txt > ${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}_${k_option}k.txt

src/Main/Release/AnalyzeHistory cdf ${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}_${k_option}k_history.txt  ${dimension} ${num_classes} ${r_values} ${normalize} ${max_num_time_samples} ${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}_${k_option}k_history_id0.txt ${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}_${k_option}k_history_id1.txt


echo "" > foo.gp;

echo set terminal postscript eps >> foo.gp
echo set output \'${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}_${k_option}k_history.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"trials\" >> foo.gp
echo set ylabel \"cdf for % of target \# of samples\" >> foo.gp
#echo set key off >> foo.gp
echo set autoscale y >> foo.gp
echo plot "[0:1]" \"${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}_${k_option}k_history_id0.txt\" title \'class 0\' with lines linetype rgb \"red\", \"${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}_${k_option}k_history_id1.txt\" title \'class 1\' with lines linetype rgb \"blue\" >> foo.gp

wgnuplot foo.gp
rm foo.gp;

epstopdf ${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}_${k_option}k_history.eps --outfile foo.pdf;
#cp ${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}_${k_option}k_history.pdf foo.pdf
ps2pdf13 -dPDFSETTINGS=/prepress foo.pdf ${outputdir}/poisson_${dimension}d_${num_classes}c_${r_text}_${k_option}k_history.pdf;
rm foo.pdf;

end # foreach k_option

endif

# test TreeThrowing
if(0) then

set outputdir = data;

set dimension = 2;

set record_history = 1;

foreach exe_option (0 1)

if(${exe_option} == 0) then
set exe_name = DartThrowing;
set output_name = grid;
else
set exe_name = TreeThrowing;
set output_name = tree;
endif

if(${record_history}) then
set exe_name = ${exe_name}History;
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

set k_value = 20;

if(1) then
set rho_number = 0.65;
set k_value = ${rho_number};
endif

set domain_size = "1 1";

if(${record_history}) then
set history_file_name = ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${r_text}_history.txt;
else
set history_file_name = "";
endif

echo src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size ${history_file_name}
time src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size ${history_file_name} > ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${r_text}.txt

if(1) then
src/Main/Release/DrawSamples ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${r_text}.txt ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${r_text}.fig 2 ${dot_radius} 1 ${domain_size}
src/Scripts/fig2eps -f ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${r_text}.fig
epstopdf ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${r_text}.eps --outfile ${outputdir}/${output_name}_${dimension}d_${num_classes}c_${r_text}.pdf
endif

end # foreach option

end # foreach exe_option

endif

# fuzzy dart throwing
if(0) then

set monomial_degree = 1;

set dimension = 2;

set num_classes = 1;
set priority = 0;
set r_values = 0.01;
set ids = 0;
set r_text = 0p01
set dot_radius = 0.002;

set k_value = 8;

set domain_size = "1 1";

set num_relaxations = 0;

echo src/Main/Release/FuzzyDartThrowing $dimension $num_classes $priority $r_values $k_value $domain_size

src/Main/Release/FuzzyDartThrowing $dimension $num_classes $priority $r_values $k_value $domain_size $monomial_degree > data/fuzzy_${dimension}d_${num_classes}c_${r_text}.txt

#src/Main/Release/DartThrowing $dimension $num_classes $priority $r_values $k_value $domain_size > data/fuzzy_${dimension}d_${num_classes}c_${r_text}.txt

src/Main/Release/SampleSelector data/fuzzy_${dimension}d_${num_classes}c_${r_text}.txt ${dimension} ${ids} > data/fuzzy_${dimension}d_${num_classes}c_${r_text}_selected.txt

if(1) then
src/Main/Release/DrawSamples data/fuzzy_${dimension}d_${num_classes}c_${r_text}.txt data/fuzzy_${dimension}d_${num_classes}c_${r_text}.fig 2 ${dot_radius} 1 ${domain_size}
src/Scripts/fig2eps -f data/fuzzy_${dimension}d_${num_classes}c_${r_text}.fig
epstopdf data/fuzzy_${dimension}d_${num_classes}c_${r_text}.eps --outfile data/fuzzy_${dimension}d_${num_classes}c_${r_text}.pdf
endif

if(0) then
../BlueNoise/src/Main/Release/DrawSamples data/fuzzy_${dimension}d_${num_classes}c_${r_text}_selected.txt data/fuzzy_${dimension}d_${num_classes}c_${r_text}_selected.fig 2 ${dot_radius} 1 ${domain_size}
src/Scripts/fig2eps -f data/fuzzy_${dimension}d_${num_classes}c_${r_text}_selected.fig
epstopdf data/fuzzy_${dimension}d_${num_classes}c_${r_text}_selected.eps --outfile data/fuzzy_${dimension}d_${num_classes}c_${r_text}_selected.pdf
endif

if(0) then
../BlueNoise/src/Main/Release/SFT data/fuzzy_${dimension}d_${num_classes}c_${r_text}_selected.txt data/fuzzy_${dimension}d_${num_classes}c_${r_text}.ppm ${dimension} 1 1 256 256
endif

endif

# adaptive sampling trial
if(0) then

set dimension = 2;

set num_classes = 2;
set priority = "0 0";

set r_values = "0.02 0.02";
set r_text = 0p02;
set dot_radius = 0.002;

if(0) then
set r_values = "0.01 0.01";
set r_text = 0p01;
set dot_radius = 0.001;
endif

set k_value = 8;
set domain_size = "1 1";

set conflict_metric = 0; # mean metric

set field_name = "blob";
#set field_name = "stripe";

set output_name = adaptive_${dimension}d_${num_classes}c_${r_text};

# create distance fields
echo build fields
foreach class (0 1)
set output_field_name = ${field_name}_${class}

if(${field_name} == 'blob') then
echo "1 2 1 0.5 0.5 0.3 0.3" > data/${output_field_name}.txt
else if(${field_name} == 'stripe') then
if(${class} == 0) then
echo "1 2 1 0.5 0.5 10000000000 0.2" > data/${output_field_name}.txt
else
echo "1 2 1 0.5 0.5 0.2 10000000000" > data/${output_field_name}.txt
endif
else
echo unknown field name
exit(1);
endif

src/Sain/Release/MakeBlobs data/${output_field_name}.txt data/${output_field_name}.pfm 512 0;
src/Sain/Release/PFM2PPM data/${output_field_name}.pfm data/${output_field_name}.ppm 1 1 2;
end # foreach class

echo src/Main/Release/AdaptiveDartThrowing $dimension $num_classes $priority $r_values $k_value $domain_size ${conflict_metric} data/${field_name}_0.pfm data/${field_name}_1.pfm
src/Main/Release/AdaptiveDartThrowing $dimension $num_classes $priority $r_values $k_value $domain_size ${conflict_metric} data/${field_name}_0.pfm data/${field_name}_1.pfm > data/${output_name}.txt

if(1) then
src/Main/Release/DrawSamples data/${output_name}.txt data/${output_name}.fig ${dimension} ${dot_radius} 1 ${domain_size}
src/Scripts/fig2eps -f data/${output_name}.fig
epstopdf data/${output_name}.eps --outfile data/${output_name}.pdf
endif

endif

# multi-tree uniform sampling
if(0) then

set outputdir = data;

set dimension = 2;

foreach exe_option (2)

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

foreach option (-1 0 1)

if(${option} == -1) then
set num_classes = 1;
set priority = "0";
set r_values = "0.02";
set r_text = 0p02;
else if(${option} == 0) then
set num_classes = 2;
set priority = "0 0";
set r_values = "0.02 0.02";
set r_text = 0p020p02;
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

set k_value = 16.67;
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

# multi-tree adaptive sampling
if(0) then

set outputdir = data;

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

foreach exe_option (1) #(0 1)

if(${exe_option} == 0) then
set exe_name = AdaptiveDartThrowing;
set output_name_exe = grid;
else if(${exe_option} == 1) then
set exe_name = MultiTreeAdaptiveSampling;
set output_name_exe = multitree;
else
endif

set output_name_exe = ${output_name_exe}_${output_name}

if(${num_classes} == 2) then
echo src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size ${conflict_metric} ${outputdir}/${field_name}_0.pfm ${outputdir}/${field_name}_1.pfm
src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size ${conflict_metric} ${outputdir}/${field_name}_0.pfm ${outputdir}/${field_name}_1.pfm > ${outputdir}/${output_name_exe}.txt
else
echo src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size ${conflict_metric} ${outputdir}/${field_name}_0.pfm
src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size ${conflict_metric} ${outputdir}/${field_name}_0.pfm > ${outputdir}/${output_name_exe}.txt
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

end #foreach exe_option

endif

# lena
if(0) then

set outputdir = data;

set dimension = 2;

set num_classes = 3;

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

if(0) then
set r_value = 002;
set dot_radius = 0.00051;
endif

set k_value = 16;
set domain_size = "1 1";

set conflict_metric = 0; # mean

set field_name = "lena";

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

if(${num_classes} == 3) then
set conversion_source = ${outputdir}/${field_name}.ppm;
else if(${num_classes} == 4) then
set conversion_source = ${outputdir}/${field_name}.tif;
convert -colorspace RGB ${outputdir}/${field_name}.ppm -colorspace CMYK ${conversion_source}
else
echo unknown number of classes
exit(1);
endif

set class = 0;
while(${class} < ${num_classes})

if(${num_classes} == 3) then

if(${class} == 0) then
set channel = 'R';
else if(${class} == 1) then
set channel = 'G';
else if(${class} == 2) then
set channel = 'B';
else
echo unknown channel
exit(1);
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
echo unknown channel
exit(1);
endif

else
echo unknown number of classes;
exit(1);
endif

convert -separate -channel ${channel} -compress none ${conversion_source} ${outputdir}/${field_name}_${class}.ppm
../BlueNoise/src/Main/Release/PPM2PFM ${outputdir}/${field_name}_${class}.ppm ${outputdir}/${field_name}_${class}.pfm

@ class = ${class} + 1;
end # while

# echo debug; exit(1);

set exe_name = MultiTreeAdaptiveSampling;
set output_name_exe = multitree;

set output_name = adaptive_${field_name}_${dimension}d_${num_classes}c_${r_text};

set output_name_exe = ${output_name_exe}_${output_name}

echo src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size ${conflict_metric} ${field_names}
time src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size ${conflict_metric} ${field_names} > ${outputdir}/${output_name_exe}.txt

set color_palette = ${outputdir}/${field_name}_color_palette.txt

if(${num_classes} == 3) then
echo "255 0 0  0 255 0  0 0 255" > ${color_palette}
set background = -1;
else if(${num_classes} == 4) then
echo "0 255 255  255 0 255  255 255 0  0 0 0" > ${color_palette}
set background = 1;
else
echo unknown number of classes;
exit(1);
endif

src/Main/Release/DrawSamples ${outputdir}/${output_name_exe}.txt ${outputdir}/${output_name_exe}.fig ${dimension} ${dot_radius} ${background} ${domain_size} ${color_palette}
src/Scripts/fig2eps -f ${outputdir}/${output_name_exe}.fig
epstopdf ${outputdir}/${output_name_exe}.eps --outfile ${outputdir}/${output_name_exe}.pdf

endif

# random
if(0) then

set outputdir = data;

set dimension = 2;

set num_classes = 4;

set r_value = 02;
set dot_radius = 0.004; # about 1/5 #0.0${r_value};

set k_value = 16;
set domain_size = "1 1";

set conflict_metric = 0; # mean

set field_name = "random";

set priority = "";
set r_values = "";
set field_names = "";
set field_pixel_size = 256;

set class = 0;
while(${class} < ${num_classes})
set priority = "${priority} 0";
set r_values = "${r_values} 0.${r_value}";
set field_names = "${field_names} ${outputdir}/${field_name}_${class}.pfm";

@ class = ${class} + 1;
end

set r_text = 0p${r_value};

if(${field_name} == "random") then
src/Sain/Release/MakeRandom ${dimension} ${field_pixel_size} ${outputdir}/${field_name}.pfm;
else
echo unknown field name
exit(1);
endif

src/Sain/Release/PFM2PPM ${outputdir}/${field_name}.pfm ${outputdir}/${field_name}.ppm 1 1 2;

if(${num_classes} == 3) then
set conversion_source = ${outputdir}/${field_name}.ppm;
else if(${num_classes} == 4) then
set conversion_source = ${outputdir}/${field_name}.tif;
convert -colorspace RGB ${outputdir}/${field_name}.ppm -colorspace CMYK ${conversion_source}
else
echo unknown number of classes
exit(1);
endif

set class = 0;
while(${class} < ${num_classes})

if(${num_classes} == 3) then

if(${class} == 0) then
set channel = 'R';
else if(${class} == 1) then
set channel = 'G';
else if(${class} == 2) then
set channel = 'B';
else
echo unknown channel
exit(1);
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
echo unknown channel
exit(1);
endif

else
echo unknown number of classes;
exit(1);
endif

convert -separate -channel ${channel} -compress none ${conversion_source} ${outputdir}/${field_name}_${class}.ppm
../BlueNoise/src/Main/Release/PPM2PFM ${outputdir}/${field_name}_${class}.ppm ${outputdir}/${field_name}_${class}.pfm

@ class = ${class} + 1;
end # while

# echo debug; exit(1);

set exe_name = MultiTreeAdaptiveSampling;
set output_name_exe = multitree;

set output_name = adaptive_${field_name}_${dimension}d_${num_classes}c_${r_text};

set output_name_exe = ${output_name_exe}_${output_name}

echo src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size ${conflict_metric} ${field_names}
time src/Main/Release/${exe_name} $dimension $num_classes $priority $r_values $k_value $domain_size ${conflict_metric} ${field_names} > ${outputdir}/${output_name_exe}.txt

set color_palette = ${outputdir}/${field_name}_color_palette.txt

if(${num_classes} == 3) then
echo "255 0 0  0 255 0  0 0 255" > ${color_palette}
set background = -1;
else if(${num_classes} == 4) then
echo "0 255 255  255 0 255  255 255 0  0 0 0" > ${color_palette}
set background = 1;
else
echo unknown number of classes;
exit(1);
endif

src/Main/Release/DrawSamples ${outputdir}/${output_name_exe}.txt ${outputdir}/${output_name_exe}.fig ${dimension} ${dot_radius} ${background} ${domain_size} ${color_palette}
src/Scripts/fig2eps -f ${outputdir}/${output_name_exe}.fig
epstopdf ${outputdir}/${output_name_exe}.eps --outfile ${outputdir}/${output_name_exe}.pdf

endif

# Penrose sampling
if(0) then

set outputdir = data;

set dimension = 2;
set domain_size = "1 1";
set correction_amount = 0.0;
set uniform_field_value = 10000;
set output_name = penrose_uniform${uniform_field_value};

set dot_radius = 0.002;

src/Tiling/Release/PenroseSampling ${domain_size} ${correction_amount} ${uniform_field_value} > ${outputdir}/${output_name}.txt;

if(1) then
src/Main/Release/DrawSamples ${outputdir}/${output_name}.txt ${outputdir}/${output_name}.fig ${dimension} ${dot_radius} 1 ${domain_size}
src/Scripts/fig2eps -f ${outputdir}/${output_name}.fig
epstopdf ${outputdir}/${output_name}.eps --outfile ${outputdir}/${output_name}.pdf
endif

if(0) then

foreach group_id (0 1)

src/Main/Release/SampleSelector ${outputdir}/${output_name}.txt ${dimension} ${group_id} > ${outputdir}/${output_name}_select${group_id}.txt

src/Sain/Release/SFT data/${output_name}_select${group_id}.txt data/${output_name}_select${group_id}.pfm ${dimension} 1 1 256 256
src/Sain/Release/PFM2PPM ${outputdir}/${output_name}_select${group_id}.pfm ${outputdir}/${output_name}_select${group_id}.ppm 1 1 0

end #foreach group_id

endif

endif

# Penrose coloring
if(0) then

set outputdir = data;

set dimension = 2;
set domain_size = "1 1";

set uniform_field_value = 100;
set dot_radius = 0.01;

if(0) then
set uniform_field_value = 400;
set dot_radius = 0.005;
endif

if(0) then
set uniform_field_value = 10000;
set dot_radius = 0.002;
endif

if(0) then
set uniform_field_value = 40000;
set dot_radius = 0.001;
endif

set output_tile = 1;

set num_iterations = -1;

set output_name = penrose_color_uniform${uniform_field_value};
set output_tile_name = penrose_color_uniform${uniform_field_value}_tile;

if(${output_tile}) then
set output_tile_file_name = ${outputdir}/${output_tile_name}.fig;
else
set  output_tile_file_name = "";
endif

src/Tiling/Release/PenroseColoring ${domain_size} ${uniform_field_value} ${num_iterations} ${output_tile_file_name} > ${outputdir}/${output_name}.txt;

if(1) then
set color_palette = ${outputdir}/${output_name}_palette.txt;
echo "255 0 0   0 255 0   0 0 255   0 255 255   255 0 255  255 255 0" > ${color_palette}
src/Main/Release/DrawSamples ${outputdir}/${output_name}.txt ${outputdir}/${output_name}.fig ${dimension} ${dot_radius} 1 ${domain_size} ${color_palette}
src/Scripts/fig2eps -f ${outputdir}/${output_name}.fig
epstopdf ${outputdir}/${output_name}.eps --outfile ${outputdir}/${output_name}.pdf
endif

if(${output_tile}) then
src/Scripts/fig2eps -f ${outputdir}/${output_tile_name}.fig
epstopdf ${outputdir}/${output_tile_name}.eps --outfile ${outputdir}/${output_tile_name}.pdf
endif

endif

# object placement test
if(0) then

set outputdir = data;

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
set batch_mode = 0;
set boundary_condition = 0; // none or toroidal

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

if(${background_color} == 0) then
set transparent_color = 'black';
else
set transparent_color = 'white';
endif

convert ${outputdir}/${output_object_name}.ppm -transparent ${transparent_color} ${outputdir}/${output_object_name}.gif
endif

endif

# DFT test
if(0) then

set outputdir = data;

set dimension = 2;
set domain_size = "1 1";
set num_pairs = 160000; #640000; #1600;

# for DFT
set dft_freq = 256;
@ dft_size = ${dft_freq} * 2;
set dft_scale = ${dft_size};

# for histogram
set which_dimension = "0 1";
set hist_freq = 1;
set num_bins = 800;

foreach option ("hybrid") #("hat" "needle" "hybrid")

if(${option} == "hat") then
else if(${option} == "needle") then
set needle_dimension = ${dimension};
set needle_length = 0.02;
else if(${option} == "hybrid") then
set needle_length = 0.02;
set needle_variance = 0; #0.002;
else
echo unknown option
exit(1);
endif

set diff_file_base_name = diff_${option}_${dimension}d_${num_pairs}

if(${option} == "hat") then
src/Theory/Release/DiffThrowing ${option} ${dimension} ${num_pairs} ${domain_size} > ${outputdir}/${diff_file_base_name}.txt
else if(${option} == "needle") then
src/Theory/Release/DiffThrowing ${option} ${dimension} ${needle_dimension} ${needle_length} ${num_pairs} ${domain_size} > ${outputdir}/${diff_file_base_name}.txt
else if(${option} == "hybrid") then
src/Theory/Release/DiffThrowing ${option} ${dimension} ${needle_length} ${needle_variance} ${num_pairs} ${domain_size} > ${outputdir}/${diff_file_base_name}.txt
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
echo plot "[1:${dft_freq}]" \"${outputdir}/${diff_file_base_name}_mean.dat\" with line linetype rgb \"red\" >> foo.gp

if(0) then
echo set terminal postscript eps >> foo.gp
echo set output \'${outputdir}/${diff_file_base_name}_variance.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp
echo plot "[1:${dft_freq}] [-20:20]" \"${outputdir}/${diff_file_base_name}_variance.dat\" with line linetype rgb \"blue\" >> foo.gp
endif

wgnuplot foo.gp
rm foo.gp

set output_name = ${diff_file_base_name}_mean;
epstopdf ${outputdir}/${output_name}.eps --outfile ${outputdir}/${output_name}.pdf;

# histogram
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

end # foreach option

endif

# diff histogram
if(0) then

set outputdir = data;

set dimension = 2;
set domain_size = "1 1";
set num_pairs = 640000; #640000; #1600;

set option = "hybrid";
set needle_length = 02;
set needle_variance = 004;

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

# mine field test
if(0) then

set outputdir = data;

set dimension = 2;
set domain_size = "1 1";
set num_pairs = 160000;

# base sample generation
set num_classes = 1;
set r_value = 02;
set priority = 0;
set exe_name = DartThrowing;
set output_name = grid;
set k_value = 0.70;

# diff generation
# set option = "mine";
set diff_r_value = 014; #01;

# histogram
set normalize_hist = 1;
set which_dimension = "0 1";
set hist_freq = 1;
set num_bins = 800;

# for DFT
set gen_dft = 0;
set dft_freq = 256;
@ dft_size = ${dft_freq} * 2;
set dft_scale = ${dft_size};

# generate base samples
set sample_file_base_name = ${output_name}_${dimension}d_${num_classes}c_0p${r_value}

src/Main/Release/${exe_name} $dimension $num_classes $priority 0.$r_value $k_value $domain_size > ${outputdir}/${sample_file_base_name}.txt

# generate diffs
foreach option ("hat" "mine")
set diff_file_base_name = diff_${option}_${dimension}d_${num_classes}c_0p${r_value}_0p${diff_r_value}_${num_pairs}

if(${option} == "hat") then
set reference_file_base_name = ${diff_file_base_name};
src/Theory/Release/DiffThrowing ${option} ${dimension} ${num_pairs} ${domain_size} > ${outputdir}/${diff_file_base_name}.txt
else if(${option} == "mine") then
src/Theory/Release/DiffThrowing ${option} ${outputdir}/${sample_file_base_name}.txt ${dimension} $num_classes 0.$diff_r_value ${num_pairs} ${domain_size} > ${outputdir}/${diff_file_base_name}.txt
endif

# histogram
echo ${outputdir}/${diff_file_base_name}.txt > ${outputdir}/all_sample_file_list.txt;

src/Theory/Release/DiffHistogram ${outputdir}/all_sample_file_list.txt $dimension $which_dimension ${hist_freq} $num_bins > ${outputdir}/${diff_file_base_name}_histogram.txt

rm ${outputdir}/all_sample_file_list.txt;

end # foreach option

set reference_name = ${reference_file_base_name}_histogram;
set output_name = ${diff_file_base_name}_histogram;

if(${normalize_hist}) then
set output_normalized_name = ${diff_file_base_name}_histogram_normalized;
./src/Scripts/normalize_hist ${outputdir}/${output_name}.txt ${outputdir}/${reference_name}.txt ${outputdir}/${output_normalized_name}.txt 
endif

echo "" > foo.gp;

echo set terminal postscript eps >> foo.gp
echo set output \'${outputdir}/${output_name}.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"\|coordinate difference\|\" >> foo.gp
if(${normalize_hist}) then
echo set ylabel \"normalized density\" >> foo.gp
else
echo set ylabel \"density\" >> foo.gp
endif
#echo set key off >> foo.gp
echo set autoscale y >> foo.gp
set max_bin = "";
@ max_bin = $num_bins - 1;

if(${normalize_hist}) then
echo plot "[0:1.0]" \"${outputdir}/${output_normalized_name}.txt\" title \'mine\' with lines linetype rgb \"red\" >> foo.gp
else
echo plot "[0:1.41245]" \"${outputdir}/${output_name}.txt\" title \'mine\' with lines linetype rgb \"red\", \"${outputdir}/${reference_name}.txt\" title \'ref\' with lines linetype rgb \"\#009900\" >> foo.gp
endif

wgnuplot foo.gp
rm foo.gp;

epstopdf ${outputdir}/${output_name}.eps --outfile ${outputdir}/${output_name}.pdf;
echo ${outputdir}/${output_name}.pdf;

# DFT
if(${gen_dft}) then
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
echo plot "[1:${dft_freq}]" \"${outputdir}/${diff_file_base_name}_mean.dat\" with line linetype rgb \"red\" >> foo.gp

wgnuplot foo.gp
rm foo.gp

set output_name = ${diff_file_base_name}_mean;
epstopdf ${outputdir}/${output_name}.eps --outfile ${outputdir}/${output_name}.pdf;
echo ${outputdir}/${output_name}.pdf;
endif

endif

# color transform
if(0) then

set base_name = Lenna;
#set base_name = red;
set base_name = Monarch;
#set base_name = Pepper;
#set base_name = van_gogh_bandaged;

if(${base_name} == red) then
echo "P3 1 1 255 255 0 0" > ${outputdir}/${base_name}.ppm;
else if(${base_name} == cyan) then
echo "P3 1 1 255 0 255 255" > ${outputdir}/${base_name}.ppm;
else if(${base_name} == white) then
echo "P3 1 1 255 255 255 255" > ${outputdir}/${base_name}.ppm;
endif

set output_dimension = 3;
set output_dimension = 6;
set output_dimension = 7;

if(${output_dimension} == 3) then
set output_basis = "1 0 0  0 1 0  0 0 1";
else if(${output_dimension} == 6) then
set output_basis = "1 0 0  0 1 0  0 0 1  0 1 1  1 0 1  1 1 0";
else if(${output_dimension} == 7) then
set output_basis = "1 0 0  0 1 0  0 0 1  0 1 1  1 0 1  1 1 0  1 1 1";
else
echo unknown output dimension;
exit(1);
endif

set input_file_name = ${outputdir}/${base_name}.pfm;
set output_file_names = "";

set i = 0;
while(${i} < ${output_dimension})
set output_file_names = "${output_file_names} ${outputdir}/${base_name}_${i}.pfm";
@ i = ${i} + 1;
end

./src/Sain/Release/PPM2PFM ${outputdir}/${base_name}.ppm ${input_file_name};

./src/Main/Release/ColorTransform ${input_file_name} ${output_dimension} ${output_basis} ${output_file_names}

set i = 0;
while(${i} < ${output_dimension})
./src/Sain/Release/PFM2PPM ${outputdir}/${base_name}_${i}.pfm ${outputdir}/${base_name}_${i}.ppm 1 1 2
@ i = ${i} + 1;
end

endif

# clip
if(0) then

foreach input (Seurat_SundayAfternoonGrandJatte) # (Pepper Lenna Mandril Monarch van_gogh_bandaged monnier2)

if((${input} == 'van_gogh_bandaged') || (${input} == 'monnier2')) then
set min_value = 257;
set max_value = 65278;
else if((${input} == 'Seurat_SundayAfternoonGrandJatte')) then
set min_value = 257;
set max_value = 64250;
else
set min_value = 1;
set max_value = 254;
endif

./src/Main/Release/HackImage clipmin ${outputdir}/${input}.ppm ${min_value} ${min_value} ${min_value} ${outputdir}/${input}.ppm

./src/Main/Release/HackImage clipmax ${outputdir}/${input}.ppm ${max_value} ${max_value} ${max_value} ${outputdir}/${input}.ppm

end

endif
