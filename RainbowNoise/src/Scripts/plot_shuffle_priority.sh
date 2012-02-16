#!/bin/tcsh -f

set normalize = 1;

# class priority, sequential generation, matching 0p040p020p01
if(1) then

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

set num_runs = 10;

foreach priority_option (1c1c1c gaussian uniform)

if(${priority_option} == "1c1c1c") then
set class_text = "1c1c1c"; # sequential
else
set class_text = "3c"; # concurrent
endif

foreach id (0 1 2)

if(${id} == 0) then
set num_samples = 323;
set normalized_num_samples = 15.3402441; #323^2/6801
else if(${id} == 1) then
set num_samples = 1295;
set normalized_num_samples = 246.585061;
else if(${id} == 2) then
set num_samples = 5183;
set normalized_num_samples = 3949.93222;
else
echo unknown id
exit(1);
endif

if(${class_text} == "1c1c1c") then
set base_name = shuffle_${dimension}d_${class_text}_${num_samples_text}_c${id}_average${num_runs};
else
set base_name = shuffle_${dimension}d_${class_text}_${num_samples_text}_sphererandom${refine_num_trials}_throw_fair_${priority_option}_c${id}_average${num_runs};
endif

set r_text = 0p040p020p01;
set reference_name = poisson_${dimension}d_1c_any_${r_text}_selected${id}_average${num_runs};

set reference_name = shuffle_${dimension}d_1c_${num_samples}_${refine_trough_method}_${init_shuffle_method}_${class_weighting_scheme}_call_average${num_runs};

./txt2dat data/${base_name}_mean.txt data/${base_name}_variance.txt ${normalized_num_samples} data/${base_name}_mean.dat data/${base_name}_variance.dat 1

./txt2dat data/${reference_name}_mean.txt data/${reference_name}_variance.txt ${normalized_num_samples} data/${reference_name}_mean.dat data/${reference_name}_variance.dat 1

if(${id} == 0) then
set y_range = "[0:0.09]";
else if(${id} == 1) then
set y_range = "[0:0.35]";
else if(${id} == 2) then
set y_range = "[0:1.4]";
else
echo unknown id;
exit(1);
endif

echo "" > foo.gp
echo set terminal postscript eps >> foo.gp
echo set output \'data/${base_name}_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
echo plot "[1:256] ${y_range}" \"data/${base_name}_mean.dat\" with line linetype rgb \"red\", \"data/${reference_name}_mean.dat\" with line linetype rgb \"\#009900\" >> foo.gp

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

wgnuplot foo.gp;
rm foo.gp;

end # foreach id
end # foreach priority_option

endif
