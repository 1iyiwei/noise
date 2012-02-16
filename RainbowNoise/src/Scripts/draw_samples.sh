#!/bin/tcsh -f

if ($#argv < 6) then
echo "Usage: dimension input_file_name output_file_name dot_radius domain_size";
exit(1);
endif

set dimension = $argv[1];
set input_file_name = $argv[2];
set output_file_name = $argv[3];
set dot_radius = $argv[4];
set domain_size = "$argv[5] $argv[6]";

set output_dir = data;
set output_base_name = foo;

if(${dimension} == 2) then
./src/Main/Release/DrawSamples ${input_file_name} ${output_dir}/${output_base_name}.fig ${dimension} ${dot_radius} 1 ${domain_size}

./src/Scripts/fig2eps -f ${output_dir}/${output_base_name}.fig

epstopdf ${output_dir}/${output_base_name}.eps --outfile ${output_file_name};

rm ${output_dir}/${output_base_name}.fig ${output_dir}/${output_base_name}.eps;

else
echo "dimension must be 2";
exit(1);
endif
