#!/bin/tcsh -f

if(1) then

set outputdir = data;

set output_name = grid;

set dimension = 2;
set domain_size = "1 1";

set k_value = 0.65;

set num_runs = 10;

set r_value_spec = (16 08 04 02 01);
set c1_value_spec = (16 0715541753 0349148624 0173544366 00866448578);
set num_sample_spec = (19 76 304 1219 4878); # per class, not yet cumulated

set num_classes = $#r_value_spec;
set one_class = 1;

set r_text = "";
set priority = "";

set i = 1;
while(${i} <= ${num_classes})
set r_text = "${r_text}_0p$r_value_spec[${i}]";
set priority = "${priority} 0";

@ i = ${i} + 1;
end

set output_base_name = ${output_name}_${dimension}d_${num_classes}c${r_text};

# my results pfm
set ids_so_far = "";
set select_id = "";
set r_text_so_far = "";

echo "" > foo.gp

set class = 0;
set num_samples = 0;
while(${class} < ${num_classes})

set select_id = "${select_id}${class}";

@ class = ${class} + 1;

@ num_samples = ${num_samples} + $num_sample_spec[${class}];

set r_text = "0p$c1_value_spec[${class}]";
set truth_select_id = "0";

set truth_base_name = ${output_name}_${dimension}d_${one_class}c${r_text};

./txt2dat ${outputdir}/${output_base_name}_${select_id}_average${num_runs}_mean.txt ${outputdir}/${output_base_name}_${select_id}_average${num_runs}_variance.txt ${num_samples} ${outputdir}/${output_base_name}_${select_id}_average${num_runs}_mean.dat ${outputdir}/${output_base_name}_${select_id}_average${num_runs}_variance.dat 1

./txt2dat ${outputdir}/${truth_base_name}_${truth_select_id}_average${num_runs}_mean.txt ${outputdir}/${truth_base_name}_${truth_select_id}_average${num_runs}_variance.txt ${num_samples} ${outputdir}/${truth_base_name}_${truth_select_id}_average${num_runs}_mean.dat ${outputdir}/${truth_base_name}_${truth_select_id}_average${num_runs}_variance.dat 1

echo set terminal postscript eps >> foo.gp
echo set output \'${outputdir}/${output_base_name}_${select_id}_average${num_runs}_mean.eps\' >> foo.gp
echo set size 0.5, 0.675 >> foo.gp
echo set title \"\" >> foo.gp
echo set xlabel \"frequency\" >> foo.gp
echo set ylabel \"power\" >> foo.gp
echo set key off >> foo.gp
echo plot "[1:256] [0:1.6]" \"${outputdir}/${output_base_name}_${select_id}_average${num_runs}_mean.dat\" with line linetype rgb \"red\", \"${outputdir}/${truth_base_name}_${truth_select_id}_average${num_runs}_mean.dat\" with line linetype rgb \"\#009900\" >> foo.gp
end

wgnuplot foo.gp
rm foo.gp

endif


