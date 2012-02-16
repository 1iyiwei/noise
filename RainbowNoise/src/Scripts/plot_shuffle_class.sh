#!/bin/tcsh -f

set dimension = 2;
set domain_size = "1 1";
set num_samples_one_class = 1300;
@ num_trials = ${num_samples_one_class};
set trough_method = random${num_trials};
set shuffle_method = "maxpeak";
set class_weighting_scheme = "fair";

set num_runs = 10;

set class = 0;

echo "" > foo.gp

foreach kernel_size_option ("constant" "optimal")

foreach num_class (1 2 4 9 16)

if(${num_class} == 1) then
set grid_res = 36;
else if(${num_class} == 2) then
set grid_res = 50;
else if(${num_class} == 4) then
set grid_res = 75;
else if(${num_class} == 8) then
set grid_res = 100;
else if(${num_class} == 9) then
set grid_res = 100;
else if(${num_class} == 16) then
set grid_res = 150;
else if(${num_class} == 32) then
set grid_res = 200;
else if(${num_class} == 64) then
set grid_res = 300;
else
echo unsupported num_class;
exit(1);
endif

set energy_store_method = grid${grid_res};

set root_name = shuffle_${dimension}d_${num_class}c_${num_samples_one_class}_${energy_store_method}_${trough_method}_${class_weighting_scheme};
if(${kernel_size_option} == "optimal") then
set root_name = ${root_name}_ksizeopt;
endif

set base_name = ${root_name}_c${class}_average${num_runs};

set reference_name = shuffle_${dimension}d_1c_${num_samples_one_class}_grid36_${trough_method}_${class_weighting_scheme}_c${class}_average${num_runs};

./txt2dat data/${base_name}_mean.txt data/${base_name}_variance.txt 1300 data/${base_name}_mean.dat data/${base_name}_variance.dat 1

echo set terminal postscript eps >> foo.gp
echo set output \'data/${base_name}_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp

if(${num_class} == 1) then
echo plot "[1:256] [0:1.8]" \"data/${base_name}_mean.dat\" with line linetype rgb \"\#009900\" >> foo.gp
else
echo plot "[1:256] [0:1.8]" \"data/${base_name}_mean.dat\" with line linetype rgb \"red\", \"data/${reference_name}_mean.dat\" with line linetype rgb \"\#009900\" >> foo.gp
endif

echo "" >> foo.gp

echo set terminal postscript eps >> foo.gp
echo set output \'data/${base_name}_variance.eps\' >> foo.gp
echo set size 0.5, 0.3375 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"anisotrophy\" >> foo.gp
echo set key off >> foo.gp
echo plot "[1:256] [-20:0]" \"data/${base_name}_variance.dat\" with line linetype rgb \"blue\" >> foo.gp

echo "" >> foo.gp

echo ${base_name}

end # num_class
end # foreach kernel_size_option

wgnuplot foo.gp

rm foo.gp
