#!/bin/tcsh -f

# test new sft code
if(0) then

set dimension = 2;

set num_classes = 1;
set priority = 0;
set r_values = 0.01;
set ids = 0;
set r_text = 0p01
set dot_radius = 0.002;

set k_value = 8;

set domain_size = "1 1";

set pft_size = 256;
set pft_scale = 256;

src/Main/Release/DartThrowing $dimension $num_classes $priority $r_values $k_value $domain_size > data/poisson_${dimension}d_${num_classes}c_${r_text}.txt

src/Main/Release/SampleSelector data/poisson_${dimension}d_${num_classes}c_${r_text}.txt ${dimension} ${ids} > data/poisson_${dimension}d_${num_classes}c_${r_text}_selected.txt

foreach pft_method ('sft' 'llft')

if(${pft_method} == 'sft') then
set pft_exe = SFT;
else if (${pft_method} == 'llft') then
set pft_exe = LinearLFT;
else
echo unknown pft method
exit(1);
endif

src/Sain/Release/${pft_exe} data/poisson_${dimension}d_${num_classes}c_${r_text}_selected.txt data/${pft_method}_${dimension}d_${num_classes}c_${r_text}.pfm ${dimension} 1 1 ${pft_size} ${pft_scale}

src/Sain/Release/PFM2PPM data/${pft_method}_${dimension}d_${num_classes}c_${r_text}.pfm data/${pft_method}_${dimension}d_${num_classes}c_${r_text}.ppm 1 1 0
end

endif

# random lft
if(0) then

set dimension = 2;

set num_classes = 1;
set priority = 0;
set r_values = 0.01;
set ids = 0;
set r_text = 0p01
set dot_radius = 0.002;

set k_value = 8;

set domain_size = "1 1";

src/Main/Release/DartThrowing $dimension $num_classes $priority $r_values $k_value $domain_size > data/poisson_${dimension}d_${num_classes}c_${r_text}.txt

src/Main/Release/SampleSelector data/poisson_${dimension}d_${num_classes}c_${r_text}.txt ${dimension} ${ids} > data/poisson_${dimension}d_${num_classes}c_${r_text}_selected.txt

foreach pft_method ('sft' 'llft' 'rlft')

if(${pft_method} == 'sft') then
set pft_exe = SFT;
else if (${pft_method} == 'llft') then
set pft_exe = LinearLFT;
else if (${pft_method} == 'rlft') then
set pft_exe = RandomLFT;
else
echo unknown pft method
exit(1);
endif

if((${pft_method} == 'sft') || (${pft_method} == 'llft')) then

src/Sain/Release/${pft_exe} data/poisson_${dimension}d_${num_classes}c_${r_text}_selected.txt data/${pft_method}_${dimension}d_${num_classes}c_${r_text}.pfm ${dimension} 1 1 256 256

src/Sain/Release/PFM2PPM data/${pft_method}_${dimension}d_${num_classes}c_${r_text}.pfm data/${pft_method}_${dimension}d_${num_classes}c_${r_text}.ppm 1 1 0

else if (${pft_method} == 'rlft') then

foreach percentage (00016 001)

set p_text = 0p${percentage}

src/Sain/Release/${pft_exe} data/poisson_${dimension}d_${num_classes}c_${r_text}_selected.txt data/${pft_method}_${dimension}d_${num_classes}c_${r_text}_${p_text}.pfm ${dimension} 1 1 256 256 0.${percentage}

src/Sain/Release/PFM2PPM data/${pft_method}_${dimension}d_${num_classes}c_${r_text}_${p_text}.pfm data/${pft_method}_${dimension}d_${num_classes}c_${r_text}_${p_text}.ppm 1 1 0

end

else
echo unknown pft method
exit(1);
endif

end

endif

# adaptive isotropic sampling
if(0) then

set dimension = 2;
set r_value = 05;
set k_value = 8;
set synthesis_order = 0;
set cell_size = 1;
set domain_size = "2 1";

set dot_radius = 0.002;

set dft_freq = 64;

#set importance_field_name = "uniform";
foreach importance_field_name ("ramp") #("uniform" "ramp" "blob" "blob_shifted" "stripe")

if(${importance_field_name} == "blob") then
echo "1 2 1 0.5 0.5 0.2 0.2" > data/${importance_field_name}.txt
./src/Sain/Release/MakeBlobs data/${importance_field_name}.txt data/${importance_field_name}.pfm 512 1;
set importance_file_name = data/${importance_field_name}.pfm;
set domain_size = "1 1";
else if(${importance_field_name} == "blob_shifted") then
echo "1 2 1 0.4 0.75 0.2 0.2" > data/${importance_field_name}.txt
./src/Sain/Release/MakeBlobs data/${importance_field_name}.txt data/${importance_field_name}.pfm 512 1;
set importance_file_name = data/${importance_field_name}.pfm;
set domain_size = "1 1";
else if(${importance_field_name} == "stripe") then
echo "1 2 1 0.4 0.75 1000000000 0.2" > data/${importance_field_name}.txt
./src/Sain/Release/MakeBlobs data/${importance_field_name}.txt data/${importance_field_name}.pfm 512 0;
set importance_file_name = data/${importance_field_name}.pfm;
set domain_size = "1 1";
else if(${importance_field_name} == "uniform_data") then
echo "1 2 1 0.5 0.5 1000000000 1000000000" > data/${importance_field_name}.txt
./src/Sain/Release/MakeBlobs data/${importance_field_name}.txt data/${importance_field_name}.pfm 512 0;
set importance_file_name = data/${importance_field_name}.pfm;
set domain_size = "1 1";
else
set importance_file_name = ${importance_field_name}
endif

set output_name = ${importance_field_name}_${dimension}d_0p${r_value};

../BlueNoise/src/Main/Release/AdaptiveNDim ${dimension} 0.${r_value} ${k_value} ${synthesis_order} ${cell_size} ${importance_file_name} ${domain_size} > data/${output_name}.txt

if(1) then
../BlueNoise/src/Main/Release/DrawSamples data/${output_name}.txt data/${output_name}.fig 2 ${dot_radius} 1 #${domain_size}
src/Scripts/fig2eps -f data/${output_name}.fig
epstopdf data/${output_name}.eps --outfile data/${output_name}.pdf
endif

foreach pft_method ('sft' 'lft' 'blft')

if(${pft_method} == 'sft') then
set pft_exe = SFT;
set distance_bounds = "";
else if (${pft_method} == 'lft') then
set pft_exe = IsotropicLFT;
set distance_bounds = "";
else if (${pft_method} == 'blft') then
set pft_exe = BandIsotropicLFT;
set distance_bounds = "0 0.3";
else
echo unknown pft method
exit(1);
endif

src/Sain/Release/${pft_exe} data/${output_name}.txt data/${output_name}_${pft_method}.pfm ${dimension} 1 1 ${dft_freq} ${dft_freq} 0.${r_value} ${importance_file_name} 0.${r_value} ${domain_size} ${distance_bounds}

src/Sain/Release/PFM2PPM data/${output_name}_${pft_method}.pfm data/${output_name}_${pft_method}.ppm 1 1 0
end # foreach pft_method

end # foreach importance_field_name

endif

# blob
if(0) then

set output_name = 'blob';
set output_name = 'stripe';
set output_name = 'stripe_side';
#set output_name = 'uniform';

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

# shift invariance, only works for uniform field!
if(0) then

set dimension = 2;
set r_value = 05;
set k_value = 8;
set synthesis_order = 0;
set cell_size = 1;
set domain_size = "1.4 1.7";
set shift_amount = "0.6 0.39";

set dot_radius = 0.002;

set dft_freq = 64;

set importance_field_name = "uniform";

set importance_file_name = ${importance_field_name};

set output_name = ${importance_field_name}_${dimension}d_0p${r_value};

../BlueNoise/src/Main/Release/AdaptiveNDim ${dimension} 0.${r_value} ${k_value} ${synthesis_order} ${cell_size} ${importance_file_name} ${domain_size} > data/${output_name}.txt

set output_root_name = ${output_name};

foreach shift_method (0 1) # check shift invariance

set output_name = ${output_root_name}_shift${shift_method};

if(${shift_method} == 0) then
cp data/${output_root_name}.txt data/${output_name}.txt;
else
src/Sain/Release/ShiftSamples data/${output_root_name}.txt data/${output_name}.txt ${dimension} ${domain_size} ${shift_amount}
endif

if(1) then
../BlueNoise/src/Main/Release/DrawSamples data/${output_name}.txt data/${output_name}.fig 2 ${dot_radius} 1 #${domain_size}
src/Scripts/fig2eps -f data/${output_name}.fig
epstopdf data/${output_name}.eps --outfile data/${output_name}.pdf
endif

foreach pft_method ('sft' 'lft' 'blft')

if(${pft_method} == 'sft') then
set pft_exe = SFT;
set distance_bounds = "";
else if (${pft_method} == 'lft') then
set pft_exe = IsotropicLFT;
set distance_bounds = "";
else if (${pft_method} == 'blft') then
set pft_exe = BandIsotropicLFT;
set distance_bounds = "0 0.3";
else
echo unknown pft method
exit(1);
endif

src/Sain/Release/${pft_exe} data/${output_name}.txt data/${output_name}_${pft_method}.pfm ${dimension} 1 1 ${dft_freq} ${dft_freq} 0.${r_value} ${importance_file_name} 0.${r_value} ${domain_size} ${distance_bounds}

src/Sain/Release/PFM2PPM data/${output_name}_${pft_method}.pfm data/${output_name}_${pft_method}.ppm 1 1 0

end # foreach pft_method

end # foreach shift_method

endif
