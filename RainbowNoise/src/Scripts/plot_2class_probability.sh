#!/bin/tcsh -f

if(0) then

foreach rho (0p67 0p70)

echo "" > foo.gp;

echo set terminal postscript eps >> foo.gp
echo set output \'data/class_probability_2d_2c_0p02m0p01osqrt2_rho${rho}.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"p0\" >> foo.gp
echo set ylabel \"rho\" >> foo.gp
#echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
echo plot "[0:1] [0:1]" \"data/class_probability_2d_2c_0p02m0p01osqrt2_rho${rho}_0.txt\" title \'class 0\' with lines linetype rgb \"red\", \"data/class_probability_2d_2c_0p02m0p01osqrt2_rho${rho}_1.txt\"  title \'class 1\' with lines linetype rgb \"blue\" >> foo.gp

wgnuplot foo.gp
rm foo.gp;

end

endif

if(1) then

set rho_0 = 0.65;
set rho_0_text = 0p65;
set rho_1 = 0.67;
set rho_1_text = 0p67;
foreach r_option (1 2 3 4 5 6)

if(${r_option} == 1) then
set r_values = "0.01 0.01";
set r_value_0 = 0.01;
set r_value_1 = 0.01;
set r_text = 0p01m0p01osqrt2;
else if(${r_option} == 2) then
set r_values = "0.02 0.00755929";
set r_value_0 = 0.02;
set r_value_1 = 0.00756;
set r_text = 0p02m0p01osqrt2;
else if(${r_option} == 3) then
set r_values = "0.04 0.00718421";
set r_value_0 = 0.04;
set r_value_1 = 0.00718;
set r_text = 0p04m0p01osqrt2;
else if(${r_option} == 4) then
set r_values = "0.08 0.00709885";
set r_value_0 = 0.08;
set r_value_1 = 0.00710;
set r_text = 0p08m0p01osqrt2;
else if(${r_option} == 5) then
set r_values = "0.011 0.00923099";
set r_value_0 = 0.011;
set r_value_1 = 0.00923;
set r_text = 0p011m0p01osqrt2;
else if(${r_option} == 6) then
set r_values = "0.0141421356 0.00816497";
set r_value_0 = 0.01414;
set r_value_1 = 0.00816;
set r_text = 0p01sqrt2m0p01osqrt2;
else
echo unknown r_option;
exit(1);
endif

echo "" > foo.gp;

echo set terminal postscript eps >> foo.gp
echo set output \'data/class_probability_2d_2c_${r_text}.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"p0\" >> foo.gp
echo set ylabel \"rho\" >> foo.gp
#echo set key off >> foo.gp
#echo set autoscale y >> foo.gp
echo plot "[0:1] [0:1]" \"data/class_probability_2d_2c_${r_text}_${rho_0_text}_0.txt\" title \'class 0, rho ${rho_0}\' with lines linetype rgb \"red\", \"data/class_probability_2d_2c_${r_text}_${rho_0_text}_1.txt\" title \'class 1, rho ${rho_0}\' with lines linetype rgb \"blue\", \"data/class_probability_2d_2c_${r_text}_${rho_1_text}_0.txt\" title \'class 0, rho ${rho_1}\' with lines linetype rgb \"red\", \"data/class_probability_2d_2c_${r_text}_${rho_1_text}_1.txt\" title \'class 1, rho ${rho_1}\' with lines linetype rgb \"blue\" >> foo.gp

wgnuplot foo.gp
rm foo.gp;

end # foreach r_option

endif
