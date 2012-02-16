#!/bin/tcsh -f

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

# with respect to PD reference
set normalized = 1;
set rho_text = 0p671069;
set k_text = ${rho_text}k;

echo "" > foo.gp;

foreach r_option (0p011 0p01sqrt2 0p02 0p04 0p08) # 0p16)

set r_text = ${r_option}m0p01osqrt2;

set base_name = shuffle_${dimension}d_${num_class}c_${r_text}_${energy_store_method}_${refine_trough_method}_${class_weighting_scheme};

foreach id (0 1)

if(${r_option} == '0p01') then

if(${id} == 0) then
set sample_count = 5200;
set sample_count_normalized = 2600; # 5200^2/10400
else
set sample_count = 5200;
set sample_count_normalized = 2600;
endif

else if(${r_option} == '0p011') then

if(${id} == 0) then
set sample_count = 4297;
set sample_count_normalized = 1775.40471;
else
set sample_count = 6103;
set sample_count_normalized = 3581.40471;
endif

else if(${r_option} == '0p01sqrt2') then

if(${id} == 0) then
set sample_count = 2600;
set sample_count_normalized = 650;
else
set sample_count = 7800;
set sample_count_normalized = 5850;
endif

else if(${r_option} == '0p02') then

if(${id} == 0) then
set sample_count = 1300;
set sample_count_normalized = 162.5;
else
set sample_count = 9100;
set sample_count_normalized = 7962.5;
endif

else if(${r_option} == '0p04') then

if(${id} == 0) then
set sample_count = 325;
set sample_count_normalized = 10.15625;
else
set sample_count = 10075;
set sample_count_normalized = 9760.15625;
endif

else if(${r_option} == '0p08') then

if(${id} == 0) then
set sample_count = 81;
set sample_count_normalized = 0.630865385;
else
set sample_count = 10319;
set sample_count_normalized = 10238.6309;
endif

else if(${r_option} == '0p16') then

if(${id} == 0) then
set sample_count = 20;
set sample_count_normalized = 0.0384615385;
else
set sample_count = 10380;
set sample_count_normalized = 10360.0385;
endif

endif

set my_name = ${base_name}_c${id}_average${num_runs};

set reference_name = poisson_2d_1c_0_${r_text}_${k_text}_selected${id};

set reference_name = shuffle_${dimension}d_1c_${sample_count}_${refine_trough_method}_${init_shuffle_method}_${class_weighting_scheme}_call_average${num_runs};

./txt2dat data/${my_name}_mean.txt data/${my_name}_variance.txt ${sample_count_normalized} data/${my_name}_mean.dat data/${my_name}_variance.dat 1

./txt2dat data/${reference_name}_mean.txt data/${reference_name}_variance.txt ${sample_count_normalized} data/${reference_name}_mean.dat data/${reference_name}_variance.dat 1

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

end # foreach id
end # foreach r_option

wgnuplot foo.gp
rm foo.gp

endif
