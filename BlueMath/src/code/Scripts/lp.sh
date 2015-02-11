#!/bin/tcsh -f

if ($#argv < 1) then
 echo "please specify the output directory"
 exit(1)
endif

set output_dir = $argv[1]

mkdir -p ${output_dir}

set huge_dir = ${output_dir}/huge;
mkdir -p ${huge_dir}

set exe_dir = ./bin;

if(0) then

echo "uniform lp dart throwing";

set dimension = 2;
set domain_size = "1 1";

set p_value = 8;
set r_value = 0.02;
set r_text = 0p02;
set k_value = 0.70;

set sample_grid_resolution_per_unit_domain = 40;

set dot_radius = 0.005;

foreach domain_option ("uniform")

foreach boundary (0 1)

set base_name = poisson_${domain_option}_l${p_value}_${r_text}_b${boundary};

if(${domain_option} == "uniform") then

${exe_dir}/DartThrowing ${domain_option}l${p_value} ${dimension} ${r_value} ${k_value} ${domain_size} ${boundary} ${sample_grid_resolution_per_unit_domain} > ${output_dir}/${base_name}_samples.txt;

else

echo unknown domain option
exit(1);

endif

# draw samples
echo "0 0 0" > ${output_dir}/palette.txt
${exe_dir}/DrawSamples ${output_dir}/${base_name}_samples.txt ${output_dir}/${base_name}_samples.fig ${dimension} ${dot_radius} 1 ${domain_size} ${output_dir}/palette.txt
${exe_dir}/fig2eps -f ${output_dir}/${base_name}_samples.fig
epstopdf ${output_dir}/${base_name}_samples.eps --outfile ${output_dir}/${base_name}_samples.pdf
rm ${output_dir}/palette.txt

echo ${output_dir}/${base_name}_samples.pdf;

end # foreach boundary
end # foreach domain_option

endif

if(0) then

echo "lp diff transform"

set dimension = 2;
set domain_size = "1 1";

set r_value = 0.04;
set r_text = 0p04;
set k_value = 0.67;
set dot_radius = 0.005;

set sample_grid_resolution_per_unit_domain = 40;

set domain_size = "3 3";

set sub_domain_min = "1 1";
set sub_domain_max = "2 2";

set diff_max = 0.1;
set diff_domain_max = "${diff_max} ${diff_max}";

set local_diff_range = 10;
set mean_dist = ${r_value};

set kernel_option = "gaussian";
set kernel_size = 1;
set keep_dc = 0;
set output_resolution = 640; # per unit domain

set domain_option = "uniform";
set boundary = 1;

set num_runs = 10;

foreach p_value (inf 2 3 4 5 6 7 8)

foreach method ("poisson")

set base_name = ${method}_l${p_value}_diffxform;

# generate samples
set all_sample_files = "";
set i = 0;
while(${i} < ${num_runs})

if(${method} == "poisson") then

${exe_dir}/DartThrowing ${domain_option}l${p_value} ${dimension} ${r_value} ${k_value} ${domain_size} ${boundary} ${sample_grid_resolution_per_unit_domain} > ${huge_dir}/${base_name}_${i}.txt;

else
echo unknown sampling method;
exit(1);
endif

sleep 1;

# draw samples
if(${i} == 0) then
echo "0 0 0" > ${output_dir}/palette.txt
${exe_dir}/DrawSamples ${huge_dir}/${base_name}_${i}.txt ${output_dir}/${base_name}_${i}.fig ${dimension} ${dot_radius} 1 ${domain_size} ${output_dir}/palette.txt
${exe_dir}/fig2eps -f ${output_dir}/${base_name}_${i}.fig
epstopdf ${output_dir}/${base_name}_${i}.eps --outfile ${output_dir}/${base_name}_${i}.pdf
rm ${output_dir}/palette.txt

echo ${output_dir}/${base_name}_${i}.pdf;
endif

set all_sample_files = "${all_sample_files} ${huge_dir}/${base_name}_${i}.txt";

@ i = ${i} + 1;
end

# diff transform
foreach xform (l${p_value} l2)
foreach diff_norm ("me") #"unit")
foreach diff_weight ("otherme" "unit") #"othermeanmemean"

set output_base_name = ${base_name}_${xform}_${diff_norm}_${diff_weight};

echo ${all_sample_files} > ${output_dir}/all_sample_files_list.txt;

if(${xform} == "l2") then
${exe_dir}/UniformDiffXform ${output_dir}/all_sample_files_list.txt ${dimension} ${sub_domain_min} ${sub_domain_max} ${diff_domain_max} ${kernel_option} ${kernel_size} ${keep_dc} ${output_resolution} ${output_dir}/${output_base_name}_average${num_runs}.f2m;
else
${exe_dir}/UniformLpDiffXform ${p_value} ${local_diff_range} ${mean_dist} ${diff_norm} ${diff_weight} ${output_dir}/all_sample_files_list.txt ${dimension} ${sub_domain_min} ${sub_domain_max} ${diff_domain_max} ${kernel_option} ${kernel_size} ${keep_dc} ${output_resolution} ${output_dir}/${output_base_name}_average${num_runs}.f2m;
endif

rm ${output_dir}/all_sample_files_list.txt;

${exe_dir}/F2M2PFM ${output_dir}/${output_base_name}_average${num_runs}.f2m ${output_dir}/${output_base_name}_average${num_runs}.pfm;

${exe_dir}/PFM2PPM ${output_dir}/${output_base_name}_average${num_runs}.pfm ${output_dir}/${output_base_name}_average${num_runs}.ppm 1 1 0

convert ${output_dir}/${output_base_name}_average${num_runs}.ppm ${output_dir}/${output_base_name}_average${num_runs}.png

echo ${output_dir}/${output_base_name}_average${num_runs}.png

end # foreach diff_norm
end # foreach diff_weight
end # foreach xform (l${p_value} l2)

# radial mean (todo)

end # foreach method

end # foreach p_value

endif
