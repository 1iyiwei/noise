#!/bin/tcsh -f

if(1) then

set dimension = 2;

set num_class = 64;

set r_value = 0.02;
set r_text = 0p02;

set k_value = 0.671;

set output_base_name = poisson_${dimension}d_${num_class}c_${r_text};

./txt1dat data/${output_base_name}_minspacing.txt data/${output_base_name}_minspacing.dat

echo "" > foo.gp

echo set terminal postscript eps >> foo.gp
echo set output \'data/${output_base_name}_minspacing.eps\' >> foo.gp
echo set size 1.0, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"\# of class considered\" >> foo.gp
echo set ylabel \"min spacing\" >> foo.gp
echo set key off >> foo.gp
echo plot "[1:${num_class}] [0:0.7]" \"data/${output_base_name}_minspacing.dat\" with line linetype rgb \"purple\" >> foo.gp

echo "" >> foo.gp

wgnuplot foo.gp
rm foo.gp

endif
