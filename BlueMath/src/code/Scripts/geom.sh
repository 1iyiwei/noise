#!/bin/tcsh -f

set output_dir = data;
set huge_dir = data/huge;

# uniform dart throwing
if(0) then

set dimension = 2;
set domain_size = "1 1";

set r_value = 0.1;
set r_text = 0p1;
set k_value = 0.70;

set sample_grid_resolution_per_unit_domain = 40;

set dot_radius = 0.005;

foreach domain_option ("uniform")

foreach boundary (1)

set base_name = poisson_${domain_option}_${r_text}_b${boundary};

if(${domain_option} == "uniform") then

./src/Main/Release/DartThrowing ${domain_option} ${dimension} ${r_value} ${k_value} ${domain_size} ${boundary} ${sample_grid_resolution_per_unit_domain} > ${output_dir}/${base_name}_samples.txt;

else

echo unknown domain option
exit(1);

endif

# draw samples
echo "0 0 0" > ${output_dir}/palette.txt
./src/Main/Release/DrawSamples ${output_dir}/${base_name}_samples.txt ${output_dir}/${base_name}_samples.fig ${dimension} ${dot_radius} 1 ${domain_size} ${output_dir}/palette.txt
src/Scripts/fig2eps -f ${output_dir}/${base_name}_samples.fig
epstopdf ${output_dir}/${base_name}_samples.eps --outfile ${output_dir}/${base_name}_samples.pdf
rm ${output_dir}/palette.txt

echo ${output_dir}/${base_name}_samples.pdf;

end # foreach boundary
end # foreach domain_option

endif
