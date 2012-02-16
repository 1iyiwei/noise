#!/bin/tcsh -f

#echo $#argv

if ($#argv < 1) then
 echo "please specify the output directory"
 exit(1)
endif

set outputdir = $argv[1]

mkdir -p $outputdir

# adaptive isotropic sampling
if(1) then

set dimension = 2;
set r_value = 05;
set k_value = 8;
set synthesis_order = 0;
set cell_size = 1;
set domain_size = "2 1";

set dot_radius = 0.002;

set dft_freq = 64;

#set importance_field_name = "uniform";
foreach importance_field_name ("uniform" "ramp")

set output_name = ${importance_field_name}_${dimension}d_0p${r_value};

../BlueNoise/src/Main/Release/AdaptiveNDim ${dimension} 0.${r_value} ${k_value} ${synthesis_order} ${cell_size} ${importance_field_name} ${domain_size} > ${outputdir}/${output_name}.txt

if(1) then
../BlueNoise/src/Main/Release/DrawSamples ${outputdir}/${output_name}.txt ${outputdir}/${output_name}.fig 2 ${dot_radius} 1 #${domain_size}
src/Scripts/fig2eps -f ${outputdir}/${output_name}.fig
epstopdf ${outputdir}/${output_name}.eps --outfile ${outputdir}/${output_name}.pdf
endif

foreach pft_method ('sft' 'lft')

if(${pft_method} == 'sft') then
set pft_exe = SFT;
else if (${pft_method} == 'lft') then
set pft_exe = IsotropicLFT;
else
echo unknown pft method
exit(1);
endif

src/Sain/Release/${pft_exe} ${outputdir}/${output_name}.txt ${outputdir}/${output_name}_${pft_method}.pfm ${dimension} 1 1 ${dft_freq} ${dft_freq} 0.${r_value} ${importance_field_name} 0.${r_value} ${domain_size}

src/Sain/Release/PFM2PPM ${outputdir}/${output_name}_${pft_method}.pfm ${outputdir}/${output_name}_${pft_method}.ppm 1 1 0
end # foreach pft_method

end # foreach importance_field_name

endif
