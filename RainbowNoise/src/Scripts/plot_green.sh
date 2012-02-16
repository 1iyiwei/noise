#!/bin/tcsh -f

if(1) then

set outputdir = data;

set dimension = 2;
set domain_size = "1 1";

set num_classes = 1;

set large_r_value = 04;
set large_r_variance = 02;
set small_r_value = 01;

# sft
set sft_freq = 256;
@ sft_size = ${sft_freq} * 2;
set sft_scale = ${sft_size};

# for histogram
set hist_freq = 1;
set num_bins = 800;

# number of runs
set num_runs = 10;

set num_samples = 5658;

set cluster_sample_root_name = grid_${dimension}d_${num_classes}c_0p${large_r_value};
set final_sample_root_name = adaptive_${dimension}d_${num_classes}c_0p${large_r_value}_0p${large_r_variance}_0p${small_r_value}
set white_sample_root_name = white_${dimension}d_${num_classes}c_0p${large_r_value}_0p${large_r_variance}_0p${small_r_value}

# radial mean
./txt2dat ${outputdir}/${final_sample_root_name}_average${num_runs}_mean.txt ${outputdir}/${final_sample_root_name}_average${num_runs}_variance.txt ${num_samples} ${outputdir}/${final_sample_root_name}_average${num_runs}_mean.dat ${outputdir}/${final_sample_root_name}_average${num_runs}_variance.dat 1

echo set terminal postscript eps >> foo.gp
echo set output \'${outputdir}/${final_sample_root_name}_average${num_runs}_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp
echo set autoscale y >> foo.gp
echo plot "[1:${sft_freq}]" \"${outputdir}/${final_sample_root_name}_average${num_runs}_mean.dat\" with line linetype rgb \"red\" >> foo.gp

wgnuplot foo.gp
rm foo.gp

echo ${outputdir}/${final_sample_root_name}_average${num_runs}_mean.eps

# histogram plot
foreach normalization (0 1)
foreach which_dimension_text ("0" "01")

if(${normalization}) then
./normalize_hist ${outputdir}/${final_sample_root_name}_${which_dimension_text}_histogram.txt ${outputdir}/${white_sample_root_name}_${which_dimension_text}_histogram.txt ${outputdir}/${final_sample_root_name}_${which_dimension_text}_histogram_normalized.txt
endif

if(${which_dimension_text} == "0") then
set x_max = "1";
else
set x_max = "1.41245";
endif

set output_name = ${final_sample_root_name}_${which_dimension_text}_histogram;
if(${normalization}) then
set output_name = "${output_name}_normalized";
endif

echo "" > foo.gp;

echo set terminal postscript eps >> foo.gp
echo set output \'${outputdir}/${output_name}.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"\|coordinate difference\|\" >> foo.gp
if(${normalization}) then
echo set ylabel \"normalized density\" >> foo.gp
else
echo set ylabel \"density\" >> foo.gp
endif
echo set key off >> foo.gp
echo set autoscale y >> foo.gp
set max_bin = "";
@ max_bin = $num_bins - 1;

if(${normalization}) then
echo plot "[0:${x_max}]" \"${outputdir}/${final_sample_root_name}_${which_dimension_text}_histogram_normalized.txt\" with lines linetype rgb \"red\" >> foo.gp
else
echo plot "[0:${x_max}]" \"${outputdir}/${final_sample_root_name}_${which_dimension_text}_histogram.txt\" with lines linetype rgb \"red\", \"${outputdir}/${white_sample_root_name}_${which_dimension_text}_histogram.txt\" with lines linetype rgb \"\#009900\" >> foo.gp
endif

wgnuplot foo.gp
rm foo.gp;

echo ${outputdir}/${output_name}.eps

end # foreach which_dimension_text
end # foreach normalization

endif

