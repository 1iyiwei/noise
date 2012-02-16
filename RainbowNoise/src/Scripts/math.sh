#!/bin/tcsh -f

set outputdir = data;
set hugedir = data/huge;

# 1D dart throwing
if(0) then

set dimension = 1;

set exe_name = DartThrowing;
set output_name = grid;

if(0) then
set exe_name = DartLining;
set output_name = line;
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

set base_name = ${output_name}_${dimension}d_${num_class}c_${r_text};

set all_pfm_files = "";
set i = 0;
while(${i} < ${num_runs})

# sample set generation
if(${exe_name} == DartThrowing) then
src/Main/Release/${exe_name} $dimension $num_class $priority $r_values $k_value $domain_size > ${hugedir}/${base_name}_${i}.txt
else if(${exe_name} == DartLining) then
src/Main/Release/${exe_name} $dimension $num_class $r_values $domain_size > ${hugedir}/${base_name}_${i}.txt
else
else if(${exe_name} == WhiteNoiseSamples) then
src/Main/Release/${exe_name} $dimension $num_class $num_samples $domain_size > ${hugedir}/${base_name}_${i}.txt
else
echo unknown exe name;
exit(1);
endif

sleep 1;

# compute sft
src/Main/Release/SampleSelector ${hugedir}/${base_name}_${i}.txt ${dimension} 0 > ${hugedir}/${base_name}_${i}_selected.txt
src/Sain/Release/SFT ${hugedir}/${base_name}_${i}_selected.txt ${hugedir}/${base_name}_${i}.pfm ${dimension} 1 ${sft_size} ${sft_scale}

set all_pfm_files = "${all_pfm_files} ${hugedir}/${base_name}_${i}.pfm";

# num samples
set final_samples = `src/Main/Release/SampleSelector ${hugedir}/${base_name}_${i}.txt ${dimension} 0`;
@ num_samples = $#final_samples / ${dimension};

@ i = ${i} + 1;
end

# radial computation
echo ${all_pfm_files} > ${outputdir}/all_pfm_files_list.txt;
src/Sain/Release/RadialPlot ${outputdir}/all_pfm_files_list.txt ${outputdir}/${base_name}_average${num_runs}.pfm ${outputdir}/${base_name}_average${num_runs}_mean.txt ${outputdir}/${base_name}_average${num_runs}_variance.txt
rm ${outputdir}/all_pfm_files_list.txt;

# radial plot
src/Scripts/txt2dat ${outputdir}/${base_name}_average${num_runs}_mean.txt ${outputdir}/${base_name}_average${num_runs}_variance.txt ${num_samples} ${outputdir}/${base_name}_average${num_runs}_mean.dat ${outputdir}/${base_name}_average${num_runs}_variance.dat 1

echo set terminal postscript eps >> foo.gp
echo set output \'${outputdir}/${base_name}_average${num_runs}_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
echo plot "[1:${sft_freq}] [0:1.8]" \"${outputdir}/${base_name}_average${num_runs}_mean.dat\" with line linetype rgb \"red\" >> foo.gp

wgnuplot foo.gp
rm foo.gp

set output_name = ${base_name}_average${num_runs}_mean;
epstopdf ${outputdir}/${output_name}.eps --outfile ${outputdir}/${output_name}.pdf;
echo ${outputdir}/${output_name}.pdf;
endif
