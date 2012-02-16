#!/bin/tcsh -f
# equivalent to do.sh for dart throwing

set output_dir = data;

# first test
if(0) then

set dimension = 2;
set domain_size = "1 1";
set num_class = 2;
set num_samples_one_class = 100;
@ num_trials = ${num_class} * ${num_samples_one_class};
set energy_store_method = bag; #grid5;
set peak_method = "default";
set trough_method = random${num_trials};
set shuffle_method = "maxpeak";
set kernel_type = "gaussian";
set relative_kernel_size = 0.5;
set moving_window_size = 100;
set max_num_iterations = 200;

set energy_class_id = 0;
set energy_array_size = 256; # visualization and debug

set num_samples_per_class = "";
set i = 0;
while(${i} < ${num_class})
set num_samples_per_class = "${num_samples_per_class} ${num_samples_one_class}";
@ i = ${i} + 1;
end

#set num_samples_per_class = "10 90"; # debug
 
set dot_radius = 0.005;

set base_name = shuffle_${dimension}d_${num_class}c_${num_samples_one_class};

./src/Main/Release/WhiteNoiseSamples ${dimension} ${num_class} ${num_samples_per_class} ${domain_size} > ${output_dir}/${base_name}_init_samples.txt

foreach class_weighting_scheme (fair single all)

set output_base_name = ${base_name}_${energy_store_method}_${trough_method}_${class_weighting_scheme};

if(${energy_array_size} > 0) then
./src/Main/Release/DartShufflingEnergy ${output_dir}/${base_name}_init_samples.txt ${dimension} ${domain_size} ${energy_store_method} ${peak_method} ${trough_method} ${shuffle_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} ${energy_class_id} ${energy_array_size} ${output_dir}/${output_base_name}_energy.pfm  ${output_dir}/${output_base_name}_history.txt > ${output_dir}/${output_base_name}_samples.txt
./src/Sain/Release/PFM2PPM ${output_dir}/${output_base_name}_energy.pfm ${output_dir}/${output_base_name}_energy.ppm 1 1 2
else
./src/Main/Release/DartShuffling ${output_dir}/${base_name}_init_samples.txt ${dimension} ${domain_size} ${energy_store_method} ${peak_method} ${trough_method} ${shuffle_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} > ${output_dir}/${output_base_name}_samples.txt
endif

./src/Main/Release/DrawSamples ${output_dir}/${output_base_name}_samples.txt ${output_dir}/${output_base_name}.fig 2 ${dot_radius} 1 ${domain_size}
./src/Scripts/fig2eps -f ${output_dir}/${output_base_name}.fig
epstopdf ${output_dir}/${output_base_name}.eps --outfile ${output_dir}/${output_base_name}.pdf

end # foreach class_weighting_scheme

endif

# 2 class with different number of samples per class
if(0) then

set dimension = 2;
set domain_size = "1 1";

set num_class = 2;
set num_samples_per_class = "50 200";
set num_samples_text = 50n200;

set num_trials = 200;
set energy_store_method = grid5;
set peak_method = "default";
set trough_method = random${num_trials};
set shuffle_method = "maxpeak";
set kernel_type = "gaussian";
set relative_kernel_size = 0.5;
set moving_window_size = 200;
set max_num_iterations = 200;

set energy_class_id = 0;
set energy_array_size = 256; # visualization and debug

set dot_radius = 0.005;

foreach class_weighting_scheme (fair)

set output_base_name = shuffle_${dimension}d_${num_class}c_${num_samples_text}_${energy_store_method}_${trough_method}_${class_weighting_scheme};

./src/Main/Release/WhiteNoiseSamples ${dimension} ${num_class} ${num_samples_per_class} ${domain_size} > ${output_dir}/${output_base_name}_init_samples.txt

if(${energy_array_size} > 0) then
./src/Main/Release/DartShufflingEnergy ${output_dir}/${output_base_name}_init_samples.txt ${dimension} ${domain_size} ${energy_store_method} ${peak_method} ${trough_method} ${shuffle_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} ${energy_class_id} ${energy_array_size} ${output_dir}/${output_base_name}_energy.pfm  ${output_dir}/${output_base_name}_history.txt > ${output_dir}/${output_base_name}_samples.txt
./src/Sain/Release/PFM2PPM ${output_dir}/${output_base_name}_energy.pfm ${output_dir}/${output_base_name}_energy.ppm 1 1 2
else
./src/Main/Release/DartShuffling ${output_dir}/${output_base_name}_init_samples.txt ${dimension} ${domain_size} ${energy_store_method} ${peak_method} ${trough_method} ${shuffle_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} > ${output_dir}/${output_base_name}_samples.txt
endif

./src/Main/Release/DrawSamples ${output_dir}/${output_base_name}_samples.txt ${output_dir}/${output_base_name}.fig 2 ${dot_radius} 1 ${domain_size}
./src/Scripts/fig2eps -f ${output_dir}/${output_base_name}.fig
epstopdf ${output_dir}/${output_base_name}.eps --outfile ${output_dir}/${output_base_name}.pdf

end # foreach class_weighting_scheme

endif

# 3 class for teaser
if(0) then

set dimension = 2;
set domain_size = "1 1";

set num_class = 3;
set num_samples_per_class = "88 353 1414"; # match up 0p080p040p02
set num_samples_text = 88n353n1414;

if(1) then # debug
set num_class = 3;
set num_samples_per_class = "100 200 400";
set num_samples_text = 100n200n400;
endif

if(0) then # debug
set num_class = 3;
set num_samples_per_class = "1 299 400";
set num_samples_text = 1n299n400;
endif

set num_trials = 200;
set energy_store_method = grid20;
set peak_method = "default";
set trough_method = random${num_trials};
set shuffle_method = "maxpeak";
set kernel_type = "gaussian";
set relative_kernel_size = 0.5;
set moving_window_size = 700;
set max_num_iterations = 700;

set energy_class_id = 0;
set energy_array_size = 16; # visualization and debug

set dot_radius = 0.005;

foreach class_weighting_scheme (fair)

set output_base_name = shuffle_${dimension}d_${num_class}c_${num_samples_text}_${energy_store_method}_${trough_method}_${shuffle_method}_${class_weighting_scheme};

./src/Main/Release/WhiteNoiseSamples ${dimension} ${num_class} ${num_samples_per_class} ${domain_size} > ${output_dir}/${output_base_name}_init_samples.txt

if(${energy_array_size} > 0) then
./src/Main/Release/DartShufflingEnergy ${output_dir}/${output_base_name}_init_samples.txt ${dimension} ${domain_size} ${energy_store_method} ${peak_method} ${trough_method} ${shuffle_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} ${energy_class_id} ${energy_array_size} ${output_dir}/${output_base_name}_energy.pfm  ${output_dir}/${output_base_name}_history.txt > ${output_dir}/${output_base_name}_samples.txt
./src/Sain/Release/PFM2PPM ${output_dir}/${output_base_name}_energy.pfm ${output_dir}/${output_base_name}_energy.ppm 1 1 2
else
./src/Main/Release/DartShuffling ${output_dir}/${output_base_name}_init_samples.txt ${dimension} ${domain_size} ${energy_store_method} ${peak_method} ${trough_method} ${shuffle_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} > ${output_dir}/${output_base_name}_samples.txt
endif

./src/Main/Release/DrawSamples ${output_dir}/${output_base_name}_samples.txt ${output_dir}/${output_base_name}.fig 2 ${dot_radius} 1 ${domain_size}
./src/Scripts/fig2eps -f ${output_dir}/${output_base_name}.fig
epstopdf ${output_dir}/${output_base_name}.eps --outfile ${output_dir}/${output_base_name}.pdf

end # foreach class_weighting_scheme

endif

# different initial conditions
if(0) then

set dimension = 2;
set domain_size = "1 1";
set num_class = 3;

# for dart throwing
set priority = "0 0 0";
set r_values = "0.08 0.04 0.02";
set r_text = 0p080p040p02;
set k_value = 0.67;

# for white initial
set num_samples_per_class = "80 323 1295"; # match up dart throwing above

if(0) then # single class debug
set num_class = 1;
set priority = "0";
set r_values = "0.02";
set r_text = 0p02;
set k_value = 0.67;
set num_samples_per_class = 1295;
endif

# for dart shuffling
set energy_store_method = grid40;
set num_trials = 200; # 1698; # should be proportional to the number of samples
set peak_method = "default";
set first_trough_method = random${num_trials};
set second_trough_method = sphererandom${num_trials};
set shuffle_method = "maxpeak";
set kernel_type = "gaussian";
set relative_kernel_size = 0.5;
set moving_window_size = 1698; #should proportional to number of samples
set max_num_iterations = 1698;
set num_samples_text = ${r_text};

set energy_class_id = 0;
set energy_array_size = 16; # visualization and debug

set dot_radius = 0.004;

set base_name = ${dimension}d_${num_class}c_${num_samples_text};

foreach init (poisson white grid)

set init_base_name = ${init}_${base_name};

if(${init} == poisson) then
./src/Main/Release/DartThrowing ${dimension} ${num_class} ${priority} ${r_values} ${k_value} ${domain_size} > ${output_dir}/${init_base_name}_samples.txt
else if(${init} == grid) then
./src/Main/Release/GridNoiseSamples ${dimension} ${num_class} ${num_samples_per_class} ${domain_size} > ${output_dir}/${init_base_name}_samples.txt
else
./src/Main/Release/WhiteNoiseSamples ${dimension} ${num_class} ${num_samples_per_class} ${domain_size} > ${output_dir}/${init_base_name}_samples.txt
endif

foreach class_weighting_scheme (fair)

set output_base_name = shuffle_${dimension}d_${num_class}c_${num_samples_text}_${init}_${energy_store_method}_${first_trough_method}_${class_weighting_scheme};

set refine_base_name = shuffle_${dimension}d_${num_class}c_${num_samples_text}_${init}_${energy_store_method}_${second_trough_method}_${class_weighting_scheme};

foreach output_name (${output_base_name} ${refine_base_name})

if(${output_name} == ${output_base_name}) then
set trough_method = ${first_trough_method};
set input_name = ${init_base_name};
else if(${output_name} == ${refine_base_name}) then
set trough_method = ${second_trough_method};
set input_name = ${output_base_name};
else
echo unknown output name;
exit(1);
endif

if(${energy_array_size} > 0) then
./src/Main/Release/DartShufflingEnergy ${output_dir}/${input_name}_samples.txt ${dimension} ${domain_size} ${energy_store_method} ${peak_method} ${trough_method} ${shuffle_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} ${energy_class_id} ${energy_array_size} ${output_dir}/${output_name}_energy.pfm ${output_dir}/${output_name}_history.txt > ${output_dir}/${output_name}_samples.txt
./src/Sain/Release/PFM2PPM ${output_dir}/${output_name}_energy.pfm ${output_dir}/${output_name}_energy.ppm 1 1 2

./src/Scripts/txt1dat ${output_dir}/${output_name}_history.txt ${output_dir}/${output_name}_history.dat

echo "" > ${output_dir}/foo.gp;
echo set terminal postscript eps >> ${output_dir}/foo.gp
echo set output \'${output_dir}/${output_name}_history.eps\' >> ${output_dir}/foo.gp
#echo set size 0.5, 0.675 >> ${output_dir}/foo.gp
echo set title \"\" >> ${output_dir}/foo.gp
echo set xlabel \"iteration\" >> ${output_dir}/foo.gp
echo set ylabel \"energy\" >> ${output_dir}/foo.gp
echo set key off >> ${output_dir}/foo.gp
echo set autoscale y >> ${output_dir}/foo.gp
echo plot \"${output_dir}/${output_name}_history.dat\" with lines linetype rgb \"blue\" >> ${output_dir}/foo.gp

wgnuplot ${output_dir}/foo.gp
rm ${output_dir}/foo.gp;

epstopdf ${output_dir}/${output_name}_history.eps --outfile ${output_dir}/foo.pdf;
#cp ${output_dir}/${output_name}_history.pdf foo.pdf
ps2pdf13 -dPDFSETTINGS=/prepress ${output_dir}/foo.pdf ${output_dir}/${output_name}_history.pdf;
rm ${output_dir}/foo.pdf;

else
./src/Main/Release/DartShuffling ${output_dir}/${input_name}_samples.txt ${dimension} ${domain_size} ${energy_store_method} ${peak_method} ${trough_method} ${shuffle_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} > ${output_dir}/${output_name}_samples.txt
endif
end #foreach output_name

foreach plot_base_name (${init_base_name} ${output_base_name} ${refine_base_name})
./src/Main/Release/DrawSamples ${output_dir}/${plot_base_name}_samples.txt ${output_dir}/${plot_base_name}.fig 2 ${dot_radius} 1 ${domain_size}
./src/Scripts/fig2eps -f ${output_dir}/${plot_base_name}.fig
epstopdf ${output_dir}/${plot_base_name}.eps --outfile ${output_dir}/${plot_base_name}.pdf
end

end # foreach class_weighting_scheme
end # foreach init

endif

# discrete sample set
if(0) then

set dimension = 2;
set domain_size = "1 1";
set num_class = 2;
set num_samples_one_class = 100;
set peak_method = "default";
set kernel_type = "gaussian";
set relative_kernel_size = 0.5;

set num_points = 10058; # match the PD setting right below
set r_value = 0.0075;
set k_value = 0.70;

set moving_window_size = 100;
set max_num_iterations = 200;

set num_samples_per_class = "";
set i = 0;
while(${i} < ${num_class})
set num_samples_per_class = "${num_samples_per_class} ${num_samples_one_class}";
@ i = ${i} + 1;
end

set energy_class_id = 0;
set energy_array_size = 16; # visualization and debug

set dot_radius = 0.005;

set base_name = ${dimension}d_${num_class}c_${num_samples_one_class};

foreach init (poisson white)

set init_base_name = shuffle_${init}_${base_name};

if(${init} == poisson) then
./src/Main/Release/DartThrowing ${dimension} 1 "0" ${r_value} ${k_value} ${domain_size} > ${output_dir}/${init_base_name}_init_points.txt
else
./src/Main/Release/WhiteNoiseSamples ${dimension} 1 ${num_points} ${domain_size} > ${output_dir}/${init_base_name}_init_points.txt
endif

./src/Main/Release/RandomPickPointsSites ${output_dir}/${init_base_name}_init_points.txt ${dimension} ${num_class} ${num_samples_per_class} > ${output_dir}/${init_base_name}_points.txt

foreach option (union tribe)

if(${option} == "tribe") then
set energy_store_method = grid5;
set trough_method = grid100;
else
set energy_store_method = grid100;
set trough_method = union;
endif

set shuffle_method = "maxpeak";

foreach class_weighting_scheme (fair single all)

set output_base_name = ${init_base_name}_${energy_store_method}_${trough_method}_${class_weighting_scheme};

if(${energy_array_size} > 0) then
./src/Main/Release/DartShufflingEnergy ${output_dir}/${init_base_name}_points.txt ${dimension} ${domain_size} ${energy_store_method} ${peak_method} ${trough_method} ${shuffle_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations}  ${energy_class_id} ${energy_array_size} ${output_dir}/${output_base_name}_energy.pfm ${output_dir}/${output_base_name}_history.txt > ${output_dir}/${output_base_name}_samples.txt
./src/Sain/Release/PFM2PPM ${output_dir}/${output_base_name}_energy.pfm ${output_dir}/${output_base_name}_energy.ppm 1 1 2

./src/Scripts/txt1dat ${output_dir}/${output_base_name}_history.txt ${output_dir}/${output_base_name}_history.dat

echo "" > ${output_dir}/foo.gp;
echo set terminal postscript eps >> ${output_dir}/foo.gp
echo set output \'${output_dir}/${output_base_name}_history.eps\' >> ${output_dir}/foo.gp
#echo set size 0.5, 0.675 >> ${output_dir}/foo.gp
echo set title \"\" >> ${output_dir}/foo.gp
echo set xlabel \"iteration\" >> ${output_dir}/foo.gp
echo set ylabel \"energy\" >> ${output_dir}/foo.gp
echo set key off >> ${output_dir}/foo.gp
echo set autoscale y >> ${output_dir}/foo.gp
echo plot \"${output_dir}/${output_base_name}_history.dat\" with lines linetype rgb \"blue\" >> ${output_dir}/foo.gp

wgnuplot ${output_dir}/foo.gp
rm ${output_dir}/foo.gp;

epstopdf ${output_dir}/${output_base_name}_history.eps --outfile ${output_dir}/foo.pdf;
#cp ${output_dir}/${output_base_name}_history.pdf foo.pdf
ps2pdf13 -dPDFSETTINGS=/prepress ${output_dir}/foo.pdf ${output_dir}/${output_base_name}_history.pdf;
rm ${output_dir}/foo.pdf;

else
./src/Main/Release/DartShuffling ${output_dir}/${init_base_name}_points.txt ${dimension} ${domain_size} ${energy_store_method} ${peak_method} ${trough_method} ${shuffle_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} > ${output_dir}/${output_base_name}_samples.txt
endif

./src/Main/Release/DrawSamples ${output_dir}/${output_base_name}_samples.txt ${output_dir}/${output_base_name}.fig 2 ${dot_radius} 1 ${domain_size}
./src/Scripts/fig2eps -f ${output_dir}/${output_base_name}.fig
epstopdf ${output_dir}/${output_base_name}.eps --outfile ${output_dir}/${output_base_name}.pdf

end # foreach class_weighting_scheme
end # foreach option
end # foreach init

endif

# exactly tight situation for discrete setting
if(0) then

set dimension = 2;
set domain_size = "1 1";
set num_class = 2;
set num_samples_one_class = 113;
set peak_method = "default";
set kernel_type = "gaussian";
set relative_kernel_size = 0.5;

set num_points = 226; # match the PD setting right below
set r_value = 0.050;
set k_value = 0.70;

set moving_window_size = 100;
set max_num_iterations = 2000;

set swap_method = "random";

set num_samples_per_class = "";
set i = 0;
while(${i} < ${num_class})
set num_samples_per_class = "${num_samples_per_class} ${num_samples_one_class}";
@ i = ${i} + 1;
end

set energy_class_id = 0;
set energy_array_size = 16; # visualization and debug

set dot_radius = 0.005;

set base_name = ${dimension}d_${num_class}c_${num_samples_one_class};

foreach init (poisson)

set init_base_name = shuffle_tight_${init}_${base_name};

if(${init} == poisson) then
./src/Main/Release/DartThrowing ${dimension} 1 "0" ${r_value} ${k_value} ${domain_size} > ${output_dir}/${init_base_name}_init_points.txt
else
./src/Main/Release/WhiteNoiseSamples ${dimension} 1 ${num_points} ${domain_size} > ${output_dir}/${init_base_name}_init_points.txt
endif

./src/Main/Release/RandomPickPointsSites ${output_dir}/${init_base_name}_init_points.txt ${dimension} ${num_class} ${num_samples_per_class} > ${output_dir}/${init_base_name}_points.txt

foreach option (union tribe)

if(${option} == "tribe") then
set energy_store_method = grid10;
set trough_method = union;
else
set energy_store_method = grid10;
set trough_method = grid10;
endif

#foreach shuffle_method (maxpeak gradualpeak)
foreach shuffle_method (peak1 peak0)
foreach class_weighting_scheme (fair)

set output_base_name = ${init_base_name}_${energy_store_method}_${trough_method}_${shuffle_method}_${class_weighting_scheme};

echo "shuffle";

if(${energy_array_size} > 0) then
./src/Main/Release/DartShufflingEnergy ${output_dir}/${init_base_name}_points.txt ${dimension} ${domain_size} ${energy_store_method} ${peak_method} ${trough_method} ${shuffle_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations}  ${energy_class_id} ${energy_array_size} ${output_dir}/${output_base_name}_energy.pfm ${output_dir}/${output_base_name}_history.txt > ${output_dir}/${output_base_name}_samples.txt
./src/Sain/Release/PFM2PPM ${output_dir}/${output_base_name}_energy.pfm ${output_dir}/${output_base_name}_energy.ppm 1 1 2

./src/Scripts/txt1dat ${output_dir}/${output_base_name}_history.txt ${output_dir}/${output_base_name}_history.dat

echo "" > ${output_dir}/foo.gp;
echo set terminal postscript eps >> ${output_dir}/foo.gp
echo set output \'${output_dir}/${output_base_name}_history.eps\' >> ${output_dir}/foo.gp
#echo set size 0.5, 0.675 >> ${output_dir}/foo.gp
echo set title \"\" >> ${output_dir}/foo.gp
echo set xlabel \"iteration\" >> ${output_dir}/foo.gp
echo set ylabel \"energy\" >> ${output_dir}/foo.gp
echo set key off >> ${output_dir}/foo.gp
echo set autoscale y >> ${output_dir}/foo.gp
echo plot \"${output_dir}/${output_base_name}_history.dat\" with lines linetype rgb \"blue\" >> ${output_dir}/foo.gp

wgnuplot ${output_dir}/foo.gp
rm ${output_dir}/foo.gp;

epstopdf ${output_dir}/${output_base_name}_history.eps --outfile ${output_dir}/foo.pdf;
#cp ${output_dir}/${output_base_name}_history.pdf foo.pdf
ps2pdf13 -dPDFSETTINGS=/prepress ${output_dir}/foo.pdf ${output_dir}/${output_base_name}_history.pdf;
rm ${output_dir}/foo.pdf;

else
./src/Main/Release/DartShuffling ${output_dir}/${init_base_name}_points.txt ${dimension} ${domain_size} ${energy_store_method} ${peak_method} ${trough_method} ${shuffle_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} > ${output_dir}/${output_base_name}_samples.txt
endif

if(${swap_method} != "") then
echo "swap";
./src/Main/Release/DartSwapping ${output_dir}/${output_base_name}_samples.txt ${dimension} ${domain_size} ${energy_store_method} ${swap_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} > ${output_dir}/${output_base_name}_swap_samples.txt

foreach draw_name (${output_base_name}_samples ${output_base_name}_swap_samples)
foreach id (0 1)
if(${id} == "all") then
cp ${output_dir}/${draw_name}.txt ${output_dir}/${draw_name}_${id}.txt;
else
grep "^${id}" ${output_dir}/${draw_name}.txt > ${output_dir}/${draw_name}_${id}.txt;
endif
echo ${draw_name}_${id};
./src/Main/Release/MinSampleSpacing ${output_dir}/${draw_name}_${id}.txt ${dimension} ${domain_size} 1;
rm ${output_dir}/${draw_name}_${id}.txt;
end
end

endif

foreach draw_name (${output_base_name}_samples ${output_base_name}_swap_samples)
./src/Main/Release/DrawSamples ${output_dir}/${draw_name}.txt ${output_dir}/${draw_name}.fig 2 ${dot_radius} 1 ${domain_size}
./src/Scripts/fig2eps -f ${output_dir}/${draw_name}.fig
epstopdf ${output_dir}/${draw_name}.eps --outfile ${output_dir}/${draw_name}.pdf
end

end # foreach class_weighting_scheme
end # foreach shuffle_method
end # foreach option
end # foreach init

endif

# first test for swap
if(0) then

set dimension = 2;
set domain_size = "1 1";
set num_class = 2;
set num_samples_one_class = 113;
set energy_store_method = grid10;
set swap_method = "random";
set kernel_type = "gaussian";
set relative_kernel_size = 0.5;

set num_points = 226; # match the PD setting right below
set r_value = 0.050;
set k_value = 0.70;

set moving_window_size = ${num_points};
set max_num_iterations = 2000;

set num_samples_per_class = "";
set i = 0;
while(${i} < ${num_class})
set num_samples_per_class = "${num_samples_per_class} ${num_samples_one_class}";
@ i = ${i} + 1;
end

#set num_samples_per_class = "10 90"; # debug
 
set dot_radius = 0.005;

set base_name = ${dimension}d_${num_class}c_${num_samples_one_class};

foreach init (poisson)# white)

set init_base_name = swap_tight_${init}_${base_name};

if(${init} == poisson) then
./src/Main/Release/DartThrowing ${dimension} 1 "0" ${r_value} ${k_value} ${domain_size} > ${output_dir}/${init_base_name}_init_points.txt
else
./src/Main/Release/WhiteNoiseSamples ${dimension} 1 ${num_points} ${domain_size} > ${output_dir}/${init_base_name}_init_points.txt
endif

./src/Main/Release/RandomPickPointsSites ${output_dir}/${init_base_name}_init_points.txt ${dimension} ${num_class} ${num_samples_per_class} > ${output_dir}/${init_base_name}_points.txt

foreach class_weighting_scheme (fair)# single all)

set output_base_name = ${init_base_name}_${energy_store_method}_${swap_method}_${class_weighting_scheme};

./src/Main/Release/DartSwapping ${output_dir}/${init_base_name}_points.txt ${dimension} ${domain_size} ${energy_store_method} ${swap_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} > ${output_dir}/${output_base_name}_samples.txt

./src/Main/Release/DrawSamples ${output_dir}/${output_base_name}_samples.txt ${output_dir}/${output_base_name}.fig 2 ${dot_radius} 1 ${domain_size}
./src/Scripts/fig2eps -f ${output_dir}/${output_base_name}.fig
epstopdf ${output_dir}/${output_base_name}.eps --outfile ${output_dir}/${output_base_name}.pdf

end # foreach class_weighting_scheme
end # foreach init

endif

# non-tight swap
if(0) then

set dimension = 2;
set domain_size = "1 1";
set num_class = 2;
set num_samples_per_class = 100;
set num_points_per_sample = 100;
set energy_store_method = grid10;
set swap_method = "random";
set kernel_type = "gaussian";
set relative_kernel_size = 0.5;

@ num_points_per_class = ${num_points_per_sample} * ${num_samples_per_class};
@ num_points = ${num_points_per_class} * ${num_class};

set moving_window_size = ${num_points};
@ moving_window_size = ${num_class} * ${num_samples_per_class};
@ max_num_iterations = 100 * ${moving_window_size};

set num_samples = "";
set i = 0;
while(${i} < ${num_class})
set num_samples = "${num_samples} ${num_samples_per_class}";
@ i = ${i} + 1;
end

#set num_samples = "10 90"; # debug
 
set dot_radius = 0.005;

set base_name = ${dimension}d_${num_class}c_${num_samples_per_class};

foreach init (white)

set init_base_name = swap_${init}_${base_name};

./src/Main/Release/WhiteNoiseSamples ${dimension} 1 ${num_points} ${domain_size} > ${output_dir}/${init_base_name}_init_points.txt

./src/Main/Release/RandomPickPointsSites ${output_dir}/${init_base_name}_init_points.txt ${dimension} ${num_class} ${num_samples} > ${output_dir}/${init_base_name}_points.txt

foreach class_weighting_scheme (fair)# single all)

set output_base_name = ${init_base_name}_${energy_store_method}_${swap_method}_${class_weighting_scheme};

./src/Main/Release/DartSwapping ${output_dir}/${init_base_name}_points.txt ${dimension} ${domain_size} ${energy_store_method} ${swap_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} > ${output_dir}/${output_base_name}_samples.txt

./src/Main/Release/DrawSamples ${output_dir}/${output_base_name}_samples.txt ${output_dir}/${output_base_name}.fig 2 ${dot_radius} 1 ${domain_size}
./src/Scripts/fig2eps -f ${output_dir}/${output_base_name}.fig
epstopdf ${output_dir}/${output_base_name}.eps --outfile ${output_dir}/${output_base_name}.pdf

end # foreach class_weighting_scheme
end # foreach init

endif

# 3 class case for tracker energy field
if(0) then

set dimension = 2;
set domain_size = "1 1";

set num_class = 3;
set num_samples_per_class = "50 50 200";
set num_samples_text = 50n50n200;

set num_trials = 300;
set energy_store_method = grid15;
set peak_method = "default";
set trough_method = trandom${num_trials};
set shuffle_method = "maxpeak";
set kernel_type = "gaussian";
set relative_kernel_size = 0.5;
set moving_window_size = 200;
set max_num_iterations = 200;

set energy_class_id = -1;
set energy_array_size = 256; # visualization and debug

set dot_radius = 0.005;

foreach class_weighting_scheme (fair)

set output_base_name = shuffle_${dimension}d_${num_class}c_${num_samples_text}_${energy_store_method}_${trough_method}_${class_weighting_scheme};

./src/Main/Release/WhiteNoiseSamples ${dimension} ${num_class} ${num_samples_per_class} ${domain_size} > ${output_dir}/${output_base_name}_init_samples.txt

./src/Main/Release/DartShuffling ${output_dir}/${output_base_name}_init_samples.txt ${dimension} ${domain_size} ${energy_store_method} ${peak_method} ${trough_method} ${shuffle_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} > ${output_dir}/${output_base_name}_samples.txt

./src/Main/Release/DrawSamples ${output_dir}/${output_base_name}_samples.txt ${output_dir}/${output_base_name}.fig 2 ${dot_radius} 1 ${domain_size}
./src/Scripts/fig2eps -f ${output_dir}/${output_base_name}.fig
epstopdf ${output_dir}/${output_base_name}.eps --outfile ${output_dir}/${output_base_name}.pdf

if(${energy_array_size} > 0) then

if(${energy_class_id} >= 0) then
set energy_class_ids = ${energy_class_id};
else
set i = 0;
set energy_class_ids = "";
while(${i} < ${num_class})
set energy_class_ids = "${energy_class_ids} ${i}";
@ i = ${i} + 1;
end
endif

foreach current_class_id (${energy_class_ids})
./src/Main/Release/DartShufflingEnergy ${output_dir}/${output_base_name}_samples.txt ${dimension} ${domain_size} ${energy_store_method} ${peak_method} ${trough_method} ${shuffle_method} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} 0 0 ${current_class_id} ${energy_array_size} ${output_dir}/${output_base_name}_energy_${current_class_id}.pfm ${output_dir}/${output_base_name}_history.txt > /dev/null;
end

if(${energy_class_id} >= 0) then
mv ${output_dir}/${output_base_name}_energy_${energy_class_id}.pfm ${output_dir}/${output_base_name}_energy.pfm;
else
if(${num_class} != 3) then
echo num_class must be 3 for merging 3 pfm;
exit(1);
endif

./src/Sain/Release/Merge3PFM ${output_dir}/${output_base_name}_energy_0.pfm ${output_dir}/${output_base_name}_energy_1.pfm ${output_dir}/${output_base_name}_energy_2.pfm  ${output_dir}/${output_base_name}_energy.pfm
endif

./src/Sain/Release/PFM2PPM ${output_dir}/${output_base_name}_energy.pfm ${output_dir}/${output_base_name}_energy.ppm 1 1 2
endif

end # foreach class_weighting_scheme

endif

# draw energy sample tracker
if(0) then

set dimension = 2;
set domain_size = "1 1";

set trial_class = -1;
set num_trials = 10000;

# for dart throwing
set num_class = 3;
set priority = "0 0 0";
set r_values = "0.16 0.08 0.04";
set k_value = 0.67;

set dot_radius = 0.005;

set base_name = "shuffle_draw_energy_sample_tracker";

set input_base_name = ${base_name}_input;
set output_base_name = ${base_name}_output;

# echo "0 0.1 0.1  0 0.2 0.1  1 0.75 0.25  1 0.25 0.75  2 0.75 0.25  2 0.75 0.57" > ${output_dir}/${input_base_name}.txt;
./src/Main/Release/DartThrowing ${dimension} ${num_class} ${priority} ${r_values} ${k_value} ${domain_size} > ${output_dir}/${input_base_name}.txt;

./src/Main/Release/DrawEnergySampleTracker ${output_dir}/${input_base_name}.txt ${dimension} ${domain_size} ${trial_class} ${num_trials} > ${output_dir}/${output_base_name}.txt;

foreach draw_name (${input_base_name} ${output_base_name})
./src/Main/Release/DrawSamples ${output_dir}/${draw_name}.txt ${output_dir}/${draw_name}.fig 2 ${dot_radius} 1 ${domain_size}
./src/Scripts/fig2eps -f ${output_dir}/${draw_name}.fig
epstopdf ${output_dir}/${draw_name}.eps --outfile ${output_dir}/${draw_name}.pdf
end

endif

# GridNoiseSamples
if(0) then
set dimension = 2;
set domain_size = "1 1";
set num_class = 1;
set num_samples_per_class = 100;

set output_base_name = grid_${dimension}d_${num_class}c;

set dot_radius = 0.004;

./src/Main/Release/GridNoiseSamples ${dimension} ${num_class} ${num_samples_per_class} ${domain_size} > ${output_dir}/${output_base_name}.txt

foreach draw_name (${output_base_name})
./src/Main/Release/DrawSamples ${output_dir}/${draw_name}.txt ${output_dir}/${draw_name}.fig 2 ${dot_radius} 1 ${domain_size}
./src/Scripts/fig2eps -f ${output_dir}/${draw_name}.fig
epstopdf ${output_dir}/${draw_name}.eps --outfile ${output_dir}/${draw_name}.pdf
end
endif

# thrower dart shuffler
if(0) then

set dimension = 2;
set domain_size = "1 1";
set num_class = 2;
set num_samples_one_class = 100;
set k_number = 8;
@ total_num_samples = ${num_class} * ${num_samples_one_class};
@ num_trials = ${k_number} * ${total_num_samples};
set energy_store_method = grid10;
set peak_method = "default";
set trough_method = random${num_trials};
set shuffle_method = "thrower";
set energy_threshold = "0.001";

set kernel_type = "gaussian";
set relative_kernel_size = 0.5;

if(0) then
set kernel_type = "monomial1";
set relative_kernel_size = 1.0;
endif

set moving_window_size = ${total_num_samples};
set max_num_iterations = ${total_num_samples};

set energy_class_id = 0;
set energy_array_size = 256; # visualization and debug

set num_samples_per_class = "";
set i = 0;
while(${i} < ${num_class})
set num_samples_per_class = "${num_samples_per_class} ${num_samples_one_class}";
@ i = ${i} + 1;
end

set dot_radius = 0.005;

set base_name = shuffle_${dimension}d_${num_class}c_${num_samples_one_class};

./src/Main/Release/WhiteNoiseSamples ${dimension} ${num_class} ${num_samples_per_class} ${domain_size} > ${output_dir}/${base_name}_init_samples.txt

foreach class_weighting_scheme (fair)

set output_base_name = ${base_name}_${energy_store_method}_${trough_method}_${shuffle_method}_${class_weighting_scheme};

if(${energy_array_size} > 0) then
./src/Main/Release/DartShufflingEnergy ${output_dir}/${base_name}_init_samples.txt ${dimension} ${domain_size} ${energy_store_method} ${peak_method} ${trough_method} ${shuffle_method}${energy_threshold} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} ${energy_class_id} ${energy_array_size} ${output_dir}/${output_base_name}_energy.pfm  ${output_dir}/${output_base_name}_history.txt > ${output_dir}/${output_base_name}_samples.txt
./src/Sain/Release/PFM2PPM ${output_dir}/${output_base_name}_energy.pfm ${output_dir}/${output_base_name}_energy.ppm 1 1 2
else
./src/Main/Release/DartShuffling ${output_dir}/${base_name}_init_samples.txt ${dimension} ${domain_size} ${energy_store_method} ${peak_method} ${trough_method} ${shuffle_method}${energy_threshold} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${moving_window_size} ${max_num_iterations} > ${output_dir}/${output_base_name}_samples.txt
endif

./src/Main/Release/DrawSamples ${output_dir}/${output_base_name}_samples.txt ${output_dir}/${output_base_name}.fig 2 ${dot_radius} 1 ${domain_size}
./src/Scripts/fig2eps -f ${output_dir}/${output_base_name}.fig
epstopdf ${output_dir}/${output_base_name}.eps --outfile ${output_dir}/${output_base_name}.pdf

foreach id (0 1 all)
if(${id} == 'all') then
cp ${output_dir}/${output_base_name}_samples.txt ${output_dir}/${output_base_name}_${id}.txt;
else
grep "^${id} " ${output_dir}/${output_base_name}_samples.txt > ${output_dir}/${output_base_name}_${id}.txt;
endif
echo ${id}
./src/Main/Release/MinSampleSpacing ${output_dir}/${output_base_name}_${id}.txt ${dimension} ${domain_size} 1;
end

end # foreach class_weighting_scheme

endif

# fixed samples
if(0) then

set dimension = 2;
set domain_size = "1 1";
set num_class = 2;
set num_samples_one_class = 100;
set k_number = 8;
@ total_num_samples = ${num_class} * ${num_samples_one_class};
@ num_trials = ${k_number} * ${total_num_samples};
set energy_store_method = grid10;
set peak_method = "default";
set trough_method = random${num_trials};
set shuffle_method = "thrower";
set energy_threshold = "0.001";

set class_weighting_scheme = "fair";

set kernel_type = "gaussian";
set relative_kernel_size = 0.5;

set dot_radius = 0.005;

set current_num_class = 1;
while(${current_num_class} <= ${num_class})

set num_samples = "";
set class_text = "";
set last_class_text = "";
set i = 0;
while(${i} < ${current_num_class})
set num_samples = "${num_samples} ${num_samples_one_class}";
set class_text = "${class_text}1c";
@ i = ${i} + 1;
if(${i} < ${current_num_class}) then
set last_class_text = "${last_class_text}1c";
endif
end

set base_name = shuffle_${dimension}d_${class_text}_${num_samples_one_class};
set lase_name = shuffle_${dimension}d_${last_class_text}_${num_samples_one_class};

./src/Main/Release/WhiteNoiseSamples ${dimension} 1 ${num_samples_one_class} ${domain_size} > ${output_dir}/${base_name}_init_samples.txt

@ current_num_class_minus_one = ${current_num_class} - 1;
set i = 0;
while(${i} < ${current_num_class_minus_one})
./src/Main/Release/ProcessLloydSites ${output_dir}/${base_name}_init_samples.txt ${output_dir}/${base_name}_init_samples.txt ${dimension} "increment" ${i}
@ i = ${i} + 1;
end

set output_base_name = ${base_name}_${energy_store_method}_${trough_method}_${shuffle_method}_${class_weighting_scheme};

if(${current_num_class} > 1) then
set last_base_name = ${lase_name}_${energy_store_method}_${trough_method}_${shuffle_method}_${class_weighting_scheme};

cat ${output_dir}/${last_base_name}_samples.txt ${output_dir}/${base_name}_init_samples.txt > ${output_dir}/${base_name}_init_cat_samples.txt;
mv ${output_dir}/${base_name}_init_cat_samples.txt ${output_dir}/${base_name}_init_samples.txt;
endif

./src/Main/Release/DartShuffling ${output_dir}/${base_name}_init_samples.txt ${dimension} ${domain_size} ${energy_store_method} ${peak_method} ${trough_method} ${shuffle_method}${energy_threshold} ${class_weighting_scheme} ${kernel_type} ${relative_kernel_size} ${num_samples_one_class} ${num_samples_one_class} > ${output_dir}/${output_base_name}_samples.txt

./src/Main/Release/ProcessLloydSites ${output_dir}/${output_base_name}_samples.txt ${output_dir}/${output_base_name}_samples.txt ${dimension} "loosen" 

./src/Main/Release/DrawSamples ${output_dir}/${output_base_name}_samples.txt ${output_dir}/${output_base_name}.fig 2 ${dot_radius} 1 ${domain_size}
./src/Scripts/fig2eps -f ${output_dir}/${output_base_name}.fig
epstopdf ${output_dir}/${output_base_name}.eps --outfile ${output_dir}/${output_base_name}.pdf

./src/Main/Release/ProcessLloydSites ${output_dir}/${output_base_name}_samples.txt ${output_dir}/${output_base_name}_samples.txt ${dimension} "fasten" 

@ current_num_class = ${current_num_class} + 1;
end # while

endif

# Voronoi drawing
if(0) then

set dimension = 2;
set domain_size = "1 1";
set num_class = 3;
set num_samples = "100 200 400";
set dot_radius = 0.02;

set output_base_name = shuffle_trivial;

./src/Main/Release/WhiteNoiseSamples ${dimension} ${num_class} ${num_samples} ${domain_size} > ${output_dir}/${output_base_name}.txt;

./src/Main/Release/DrawSamples ${output_dir}/${output_base_name}.txt ${output_dir}/${output_base_name}.fig 2 -${dot_radius} 1 ${domain_size}
./src/Scripts/fig2eps -f ${output_dir}/${output_base_name}.fig
epstopdf ${output_dir}/${output_base_name}.eps --outfile ${output_dir}/${output_base_name}.pdf
endif
