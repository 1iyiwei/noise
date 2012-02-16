#!/bin/tcsh -f

# fuzzy dart throwing
if(0) then

set distribution = fuzzy;

set dimension = 2;

set num_classes = 1;
set priority = 0;
set r_value = 0.01;
set ids = 0;
set r_text = 0p01
set dot_radius = 0.002;

set k_value = 8;

set domain_size = "1 1";

set num_runs = 10;

set sft_size = 512;

foreach monomial_degree (1 2 4 8 16 32 64 128 256)

set d_text = ${monomial_degree}m;

set all_pfm_files = "";
set all_pfm_files_count = 0;

set i = 0;
set id = 0;
while(${i} < ${num_runs})

set sample_file_name = data/${distribution}_${dimension}d_${num_classes}c_${r_text}_${d_text}_${i}.txt;

echo data/${distribution}_${dimension}d_${num_classes}c_${r_text}_${d_text}_${i}.txt

src/Main/Release/FuzzyDartThrowing $dimension $num_classes $priority $r_value $k_value $domain_size $monomial_degree > ${sample_file_name}

echo data/${distribution}_${dimension}d_${num_classes}c_${r_text}_${d_text}_${i}_selected.txt

src/Main/Release/SampleSelector data/${distribution}_${dimension}d_${num_classes}c_${r_text}_${d_text}_${i}.txt ${dimension} ${id} > data/${distribution}_${dimension}d_${num_classes}c_${r_text}_${d_text}_${i}_selected.txt

../BlueNoise/src/Main/Release/SFT data/${distribution}_${dimension}d_${num_classes}c_${r_text}_${d_text}_${i}_selected.txt data/${distribution}_${dimension}d_${num_classes}c_${r_text}_${d_text}_${i}_selected.pfm ${dimension} 1 1 ${sft_size} ${sft_size} 

set all_pfm_files = "$all_pfm_files data/${distribution}_${dimension}d_${num_classes}c_${r_text}_${d_text}_${i}_selected.pfm";
@ all_pfm_files_count = ${all_pfm_files_count} + 1;

@ i = ${i} + 1;
end

echo ${all_pfm_files} > data/all_pfm_files_list.txt;
src/Sain/Release/RadialPlot data/all_pfm_files_list.txt data/${distribution}_${dimension}d_${num_classes}c_${r_text}_${d_text}_selected_average${all_pfm_files_count}.pfm data/${distribution}_${dimension}d_${num_classes}c_${r_text}_${d_text}_selected_average${all_pfm_files_count}_mean.txt data/${distribution}_${dimension}d_${num_classes}c_${r_text}_${d_text}_selected_average${all_pfm_files_count}_variance.txt
rm data/all_pfm_files_list.txt;

../BlueNoise/src/Main/Release/PFM2PPM data/${distribution}_${dimension}d_${num_classes}c_${r_text}_${d_text}_selected_average${all_pfm_files_count}.pfm data/${distribution}_${dimension}d_${num_classes}c_${r_text}_${d_text}_selected_average${all_pfm_files_count}.ppm 1 1 1

# plot one sample set for spatial visualization
src/Main/Release/SampleSelector data/${distribution}_${dimension}d_${num_classes}c_${r_text}_${d_text}_0.txt ${dimension} ${ids} > data/${distribution}_${dimension}d_${num_classes}c_${r_text}_${d_text}_0_selected.txt;

../BlueNoise/src/Main/Release/DrawSamples data/${distribution}_${dimension}d_${num_classes}c_${r_text}_${d_text}_0_selected.txt data/${distribution}_${dimension}d_${num_classes}c_${r_text}_${d_text}_0.fig 2 ${dot_radius} 1 ${domain_size}

src/Scripts/fig2eps -f data/${distribution}_${dimension}d_${num_classes}c_${r_text}_${d_text}_0.fig
epstopdf data/${distribution}_${dimension}d_${num_classes}c_${r_text}_${d_text}_0.eps --outfile data/${distribution}_${dimension}d_${num_classes}c_${r_text}_${d_text}_0.pdf

end # foreach monomial_degree

endif
