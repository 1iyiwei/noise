#!/bin/tcsh -f

# adaptive isotropic sampling
if(0) then

set dimension = 2;
set r_value = 04;
set k_value = 16;
set synthesis_order = 0;
set cell_size = 1;
set domain_size = "1 1";

set dot_radius = 0.002;

set dft_freq = 64;

set importance_field_name = uniform;
set importance_file_name = uniform;
# set importance_file_name = data/uniform.pfm;

if(0) then
set importance_field_name = ramp;
set importance_file_name = ramp;
endif

if(0) then
set importance_field_name = ostro;
set importance_file_name = data/importance_ostro.pfm;
endif

if(0) then
set importance_field_name = blob;
set importance_file_name = data/blob.pfm;
endif

if(1) then
set importance_field_name = stripe;
set importance_file_name = data/stripe.pfm;
endif

if(0) then
set importance_field_name = stripe_side;
set importance_file_name = data/stripe_side.pfm;
endif

if(1) then

set output_name = ${importance_field_name}
echo generate data/${output_name}.pfm

if(${output_name} == 'blob') then
echo "1 2 1 0.5 0.5 0.2 0.2" > data/${output_name}.txt
else if(${output_name} == 'stripe') then
echo "1 2 1 0.5 0.5 10000000000 0.4" > data/${output_name}.txt
else if(${output_name} == 'stripe_side') then
echo "1 2 1 0.5 1.0 10000000000 0.4" > data/${output_name}.txt
else if(${output_name} == 'uniform') then
echo "1 2 1 0.5 0.5 10000000000 10000000000" > data/${output_name}.txt
endif

./src/Sain/Release/MakeBlobs data/${output_name}.txt data/${output_name}.pfm 512 0;
./src/Sain/Release/PFM2PPM data/${output_name}.pfm data/${output_name}.ppm 1 1 2;
endif

set num_runs = 10;

set output_root_name = ${importance_field_name}_${dimension}d_0p${r_value};

set i = 0;
while(${i} < ${num_runs})

set output_name = ${output_root_name}_${i}

../BlueNoise/src/Main/Release/AdaptiveNDim ${dimension} 0.${r_value} ${k_value} ${synthesis_order} ${cell_size} ${importance_file_name} ${domain_size} > data/${output_name}.txt

sleep 1; # the random seed is based on seconds

if(${i} == 0) then
../BlueNoise/src/Main/Release/DrawSamples data/${output_name}.txt data/${output_name}.fig 2 ${dot_radius} 1 ${domain_size}
src/Scripts/fig2eps -f data/${output_name}.fig
epstopdf data/${output_name}.eps --outfile data/${output_name}.pdf
endif

@ i = ${i} + 1;
end # while

foreach pft_method ('sft' 'lft')

if(${pft_method} == 'sft') then
set pft_exe = SFT;
else if (${pft_method} == 'lft') then
set pft_exe = IsotropicLFT;
else
echo unknown pft method
exit(1);
endif

set all_pfm_files = "";
set i = 0;
while(${i} < ${num_runs})

set output_name = ${output_root_name}_${i}

src/Sain/Release/${pft_exe} data/${output_name}.txt data/${output_name}_${pft_method}.pfm ${dimension} 1 1 ${dft_freq} ${dft_freq} 0.${r_value} ${importance_file_name} 0.${r_value} ${domain_size}

if(0) then
src/Sain/Release/PFM2PPM data/${output_name}_${pft_method}.pfm data/${output_name}_${pft_method}.ppm 1 1 0
endif

set all_pfm_files = "$all_pfm_files data/${output_name}_${pft_method}.pfm";

@ i = ${i} + 1;
end # while

echo ${all_pfm_files} > data/all_pfm_files_list.txt;
src/Sain/Release/RadialPlot data/all_pfm_files_list.txt data/${output_root_name}_average${num_runs}_${pft_method}.pfm data/${output_root_name}_average${num_runs}_${pft_method}_mean.txt data/${output_root_name}_average${num_runs}_${pft_method}_variance.txt
rm data/all_pfm_files_list.txt;

src/Sain/Release/PFM2PPM data/${output_root_name}_average${num_runs}_${pft_method}.pfm data/${output_root_name}_average${num_runs}_${pft_method}.ppm 1 1 1

end # foreach pft_method

endif
