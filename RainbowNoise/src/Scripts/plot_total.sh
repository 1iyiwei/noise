#!/bin/tcsh -f

# uniform classes
if(1) then

set dimension = 2;

set ids = 0;
set r_value = 0.02;
set r_text = 0p02;

set target_samples = 1300;
set k_value = 100;
set domain_size = "1 1";

set all_pfm_files_count = 10;

echo "" > foo.gp

foreach num_classes (4 8 9 13 16 17 64 256) #(1 2 4 8 9 16 32 64 128 256)
#foreach num_classes_sqrt (8 16) #(2 4 8 16)

if(1) then
if(${num_classes} <= 2) then
set num_classes_sqrt = 1;
else if(${num_classes} <= 6) then
set num_classes_sqrt = 2;
else if(${num_classes} <= 12) then
set num_classes_sqrt = 3;
else if(${num_classes} <= 20) then
set num_classes_sqrt = 4;
else if(${num_classes} == 64) then
set num_classes_sqrt = 8;
else if(${num_classes} == 256) then
set num_classes_sqrt = 16;
else
echo unhandled num_classes;
exit(1);
endif
else
set num_classes = 0;
@ num_classes = ${num_classes_sqrt} * ${num_classes_sqrt};
endif

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

@ total_num_samples = ${num_classes} * ${target_samples};

./txt2dat data/poisson_${dimension}d_${num_classes}c_${r_text}_all_average${all_pfm_files_count}_mean.txt data/poisson_${dimension}d_${num_classes}c_${r_text}_all_average${all_pfm_files_count}_variance.txt ${total_num_samples} data/poisson_${dimension}d_${num_classes}c_${r_text}_all_average${all_pfm_files_count}_mean.dat data/poisson_${dimension}d_${num_classes}c_${r_text}_all_average${all_pfm_files_count}_variance.dat 1

./txt2dat data/poisson_${dimension}d_${num_classes}c_${r_text}_c1_average${all_pfm_files_count}_mean.txt data/poisson_${dimension}d_${num_classes}c_${r_text}_c1_average${all_pfm_files_count}_variance.txt ${total_num_samples} data/poisson_${dimension}d_${num_classes}c_${r_text}_c1_average${all_pfm_files_count}_mean.dat data/poisson_${dimension}d_${num_classes}c_${r_text}_c1_average${all_pfm_files_count}_variance.dat 1

set ground_truth_name = poisson_${dimension}d_${num_classes}c_${r_text}_c1_average${all_pfm_files_count};

echo set terminal postscript eps >> foo.gp
echo set output \'data/poisson_${dimension}d_${num_classes}c_${r_text}_all_average${all_pfm_files_count}_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp
echo set autoscale y >> foo.gp
echo plot "[1:256]" \"data/poisson_${dimension}d_${num_classes}c_${r_text}_all_average${all_pfm_files_count}_mean.dat\" with line linetype rgb \"red\", \"data/${ground_truth_name}_mean.dat\" with line linetype rgb \"\#009900\" >> foo.gp

echo "" >> foo.gp

echo set terminal postscript eps >> foo.gp
echo set output \'data/poisson_${dimension}d_${num_classes}c_${r_text}_all_average${all_pfm_files_count}_variance.eps\' >> foo.gp
echo set size 0.5, 0.3375 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"anisotropy\" >> foo.gp
echo set key off >> foo.gp
echo plot "[1:256] [-20:0]" \"data/poisson_${dimension}d_${num_classes}c_${r_text}_all_average${all_pfm_files_count}_variance.dat\" with line linetype rgb \"blue\" >> foo.gp

echo "" >> foo.gp

end

wgnuplot foo.gp
rm foo.gp

endif


# 2 non-uniform classes
if(1) then

set dimension = 2;

set target_samples = 10400;
set k_value = 100;

set domain_size = "1 1";

set num_runs = 10;
set all_pfm_files_count = 10;

echo "" > foo.gp

foreach option (0 2 3 4 5 6)

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

./txt2dat data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_all_average${all_pfm_files_count}_mean.txt data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_all_average${all_pfm_files_count}_variance.txt ${target_samples} data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_all_average${all_pfm_files_count}_mean.dat data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_all_average${all_pfm_files_count}_variance.dat 1

if(${option} == 0) then
set ground_truth_name = poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_all_average${all_pfm_files_count};
endif

echo set terminal postscript eps >> foo.gp
echo set output \'data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_all_average${all_pfm_files_count}_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp
echo set autoscale y >> foo.gp
echo plot "[1:256]" \"data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_all_average${all_pfm_files_count}_mean.dat\" with line linetype rgb \"red\", \"data/${ground_truth_name}_mean.dat\" with line linetype rgb \"\#009900\" >> foo.gp

echo "" >> foo.gp

echo set terminal postscript eps >> foo.gp
echo set output \'data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_all_average${all_pfm_files_count}_variance.eps\' >> foo.gp
echo set size 0.5, 0.3375 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"anisotropy\" >> foo.gp
echo set key off >> foo.gp
echo plot "[1:256] [-20:0]" \"data/poisson_${dimension}d_${num_classes}c_${p_text}_${r_text}_all_average${all_pfm_files_count}_variance.dat\" with line linetype rgb \"blue\" >> foo.gp

echo "" >> foo.gp

end #foreach option

wgnuplot foo.gp
rm foo.gp

endif
