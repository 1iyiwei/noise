#!/bin/tcsh -f

# uniform classes
if(1) then

set dimension = 2;
set domain_size = "1 1";
set num_samples_one_class = 1300;

# set peak_method = random;
set refine_peak_method = random;

@ num_trials = ${num_samples_one_class};
set trough_method = random${num_trials};
set refine_trough_method = sphererandom${num_trials};
# set refine_trough_method = "";

set shuffle_method = "maxpeak";
set k_number = 2;
set shuffle_method = "throw";
set threshold_energy = "0.001";
# set shuffle_method = "gradualpeak0";
set refine_shuffle_method = "gradualpeak0";

set class_weighting_scheme = "fair";

set kernel_type = "gaussian";
set relative_kernel_size = 0.5;

set num_runs = 10;
set sft_size = 512;
set sft_scale = ${sft_size};

set sft_half_size = 256;

set class_option = "all";

echo "" > foo.gp;

foreach kernel_size_option ("constant" "optimal")

foreach num_class (2 4 9 16)

if(${num_class} == 1) then
set grid_res = 36;
set num_classes_sqrt = 1;
else if(${num_class} == 2) then
set grid_res = 50;
set num_classes_sqrt = 1;
else if(${num_class} == 4) then
set grid_res = 75;
set num_classes_sqrt = 2;
else if(${num_class} == 8) then
set grid_res = 100;
set num_classes_sqrt = 2;
else if(${num_class} == 9) then
set grid_res = 100;
set num_classes_sqrt = 3;
else if(${num_class} == 16) then
set grid_res = 150;
set num_classes_sqrt = 4;
else if(${num_class} == 32) then
set grid_res = 200;
set num_classes_sqrt = 5;
else if(${num_class} == 64) then
set grid_res = 300;
set num_classes_sqrt = 8;
else
echo unsupported num_class;
exit(1);
endif

@ total_num_samples = ${num_class} * ${num_samples_one_class};

@ throw_num_trials = ${k_number} * ${total_num_samples};
@ throw_num_trials = ${k_number} * ${num_samples_one_class};
set throw_trough_method = random${throw_num_trials};

set energy_store_method = grid${grid_res};

@ moving_window_size = ${num_samples_one_class};
@ max_num_iterations = 2 * ${num_class} * ${num_samples_one_class};

set num_samples_per_class = "";
set i = 0;
while(${i} < ${num_class})
set num_samples_per_class = "${num_samples_per_class} ${num_samples_one_class}";
@ i = ${i} + 1;
end

set all_class_ids = "";
set i = 0;
while(${i} < ${num_class})
set all_class_ids = "${all_class_ids} ${i}";
@ i = ${i} + 1;
end

@ sft_scale_all = ${sft_size} * ${num_classes_sqrt};
@ sft_half_scale_all = ${sft_half_size} * ${num_classes_sqrt};

set root_name = shuffle_${dimension}d_${num_class}c_${num_samples_one_class}_${energy_store_method}_${trough_method}_${class_weighting_scheme};
if(${kernel_size_option} == "optimal") then
set root_name = ${root_name}_ksizeopt;
endif

set base_name = ${root_name}_c${class_option}_average${num_runs};

set reference_name = poisson_${dimension}d_${num_class}c_0p02_c1_average${num_runs};

set reference_name = shuffle_${dimension}d_1c_${total_num_samples}_${refine_trough_method}_${shuffle_method}_${class_weighting_scheme}_call_average${num_runs};

foreach input_name (${base_name} ${reference_name})
./txt2dat data/${input_name}_mean.txt data/${input_name}_variance.txt ${total_num_samples} data/${input_name}_mean.dat data/${input_name}_variance.dat ${num_classes_sqrt}
end

echo set terminal postscript eps >> foo.gp
echo set output \'data/${base_name}_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp

echo plot "[1:${sft_half_scale_all}] [0:1.6]" \"data/${base_name}_mean.dat\" with line linetype rgb \"red\", \"data/${reference_name}_mean.dat\" with line linetype rgb \"\#009900\" >> foo.gp

echo "" >> foo.gp

echo set terminal postscript eps >> foo.gp
echo set output \'data/${base_name}_variance.eps\' >> foo.gp
echo set size 0.5, 0.3375 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"anisotrophy\" >> foo.gp
echo set key off >> foo.gp
echo plot "[1:${sft_half_scale_all}] [-20:0]" \"data/${base_name}_variance.dat\" with line linetype rgb \"blue\" >> foo.gp

echo "" >> foo.gp

end # foreach num_class
end # foreach kernel_size_option

wgnuplot foo.gp
rm foo.gp

endif

# non-uniform 2 class
if(1) then

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

echo "" > foo.gp;

foreach r_option (0p011 0p01sqrt2 0p02 0p04 0p08) # 0p16)

set r_text = ${r_option}m0p01osqrt2;

set base_name = shuffle_${dimension}d_${num_class}c_${r_text}_${energy_store_method}_${refine_trough_method}_${class_weighting_scheme};

set id = "all";

set my_name = ${base_name}_c${id}_average${num_runs};

set p_text = 0;
set r_text = 0p00707;

set reference_name = poisson_${dimension}d_1c_${p_text}_${r_text}_${id}_average${num_runs};

set reference_name = shuffle_${dimension}d_1c_${total_num_samples}_${refine_trough_method}_${init_shuffle_method}_${class_weighting_scheme}_call_average${num_runs};

./txt2dat data/${my_name}_mean.txt data/${my_name}_variance.txt ${total_num_samples} data/${my_name}_mean.dat data/${my_name}_variance.dat 1

./txt2dat data/${reference_name}_mean.txt data/${reference_name}_variance.txt ${total_num_samples} data/${reference_name}_mean.dat data/${reference_name}_variance.dat 1

echo set terminal postscript eps >> foo.gp
echo set output \'data/${my_name}_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp
echo set autoscale y >> foo.gp
echo plot "[1:256]" \"data/${my_name}_mean.dat\" with line linetype rgb \"red\", \"data/${reference_name}_mean.dat\" with line linetype rgb \"\#009900\" >> foo.gp

echo "" >> foo.gp

echo set terminal postscript eps >> foo.gp
echo set output \'data/${my_name}_variance.eps\' >> foo.gp
echo set size 0.5, 0.3375 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"anisotropy\" >> foo.gp
echo set key off >> foo.gp
echo plot "[1:256] [-20:0]" \"data/${my_name}_variance.dat\" with line linetype rgb \"blue\" >> foo.gp

echo "" >> foo.gp

end # foreach r_option

wgnuplot foo.gp
rm foo.gp

endif
