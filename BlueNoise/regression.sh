#!/bin/tcsh -f

#echo $#argv

if ($#argv < 1) then
 echo "please specify the output directory"
 exit(1)
endif

set outputdir = $argv[1]

mkdir -p $outputdir

# blue + poisson gpu
if(1) then

foreach gpu_code (blue poisson)

set dimension = 2;

if(${gpu_code} == 'poisson') then
  set maxdimension = 5;
else
  set maxdimension = 6;
endif

while(${dimension} <= ${maxdimension})

foreach noise_option (0 1)

if(${dimension} == 2) then

set r_value = 0.01;
set r_text = 0p01;
set k_number = 10;
set num_cpu_levels = 2;
set clique_size = -1;
set importance = '1 1';

else if(${dimension} == 3) then

set r_value = 0.055;
set r_text = 0p055;
set k_number = 5;
set num_cpu_levels = 3;
set clique_size = -1;
set importance = '1 1 1';

else if(${dimension} == 4) then

set r_value = 0.125;
set r_text = 0p125;
set k_number = 5;
set num_cpu_levels = 2;
set clique_size = 2;
set importance = '1 1 1 1';

else if(${dimension} == 5) then

set r_value = 0.28;
set r_text = 0p28;
set k_number = 5;
set num_cpu_levels = 2;
set clique_size = 2;
set importance = '1 1 1 1 1';

else if(${dimension} == 6) then

set r_value = 0.307;
set r_text = 0p307;
set k_number = 5;
set num_cpu_levels = 1;
set clique_size = 2;
set importance = '1 1 1 1 1 1';

else
echo "illegal dimension";
exit(1);
endif

if(${gpu_code} == 'blue') then
set clique_size = -1;
endif

if(${gpu_code} == 'blue') then
set boundary_condition = 1;
else
set boundary_condition = 0;
endif

set root_cell_size = 1;

if(${dimension} == 2) then
set neighbor_offset_storage = 0;
else
set neighbor_offset_storage = 1; # 0 for program constant, 1 for texture
endif

set noise_text = 'n'${noise_option};

set show_off = 1;

#if(${dimension} != 6 || ${gpu_code} != 'poisson')
echo do ${gpu_code}_gpu_${dimension}dim_${r_text}${noise_text}.txt
src/GPU/Debug/${gpu_code}GPU ${dimension} ${r_value} ${k_number} ${num_cpu_levels} ${root_cell_size} ${clique_size} ${boundary_condition} ${neighbor_offset_storage} ${noise_option} ${show_off} ${importance} > ${outputdir}/${gpu_code}_gpu_${dimension}dim_${r_text}${noise_text}.txt
#end

# gpu 2D
if(${dimension} == 2) then

src/Main/Release/DrawSamples $outputdir/${gpu_code}_gpu_${dimension}dim_${r_text}${noise_text}.txt $outputdir/${gpu_code}_gpu_${dimension}dim_${r_text}${noise_text}_samples.fig 2 0.002 0

doc/fig2eps -f $outputdir/${gpu_code}_gpu_${dimension}dim_${r_text}${noise_text}_samples.fig

epstopdf $outputdir/${gpu_code}_gpu_${dimension}dim_${r_text}${noise_text}_samples.eps --outfile $outputdir/${gpu_code}_gpu_${dimension}dim_${r_text}${noise_text}_samples.pdf

#src/Main/Release/SFT.exe $outputdir/${gpu_code}_gpu_${dimension}dim_${r_text}${noise_text}.txt $outputdir/${gpu_code}_gpu_${dimension}dim_${r_text}${noise_text}_dft.ppm 2 1 1 512 512;

src/GPU/Release/SFTGPU $outputdir/${gpu_code}_gpu_${dimension}dim_${r_text}${noise_text}.txt $outputdir/${gpu_code}_gpu_${dimension}dim_${r_text}${noise_text}_dft.pfm 2 1 1 512 512;

src/Main/Release/PFM2PPM $outputdir/${gpu_code}_gpu_${dimension}dim_${r_text}${noise_text}_dft.pfm $outputdir/${gpu_code}_gpu_${dimension}dim_${r_text}${noise_text}_dft.ppm 1 1 0

endif

# gpu 3D
if(${dimension} == 3) then

src/Main/Release/SFT.exe $outputdir/${gpu_code}_gpu_${dimension}dim_${r_text}${noise_text}.txt $outputdir/${gpu_code}_gpu_${dimension}dim_${r_text}${noise_text}_dft.ppm 3 1 1 -64 128 128

endif

# gpu 4d
if(${dimension} == 4) then

src/Main/Release/SFT.exe $outputdir/${gpu_code}_gpu_${dimension}dim_${r_text}${noise_text}.txt $outputdir/${gpu_code}_gpu_${dimension}dim_${r_text}${noise_text}_dft.ppm 4 1 1 -32 -32 64 64

endif

# gpu 5d
if(${dimension} == 5) then

src/Main/Release/SFT.exe $outputdir/${gpu_code}_gpu_${dimension}dim_${r_text}${noise_text}.txt $outputdir/${gpu_code}_gpu_${dimension}dim_${r_text}${noise_text}_dft.ppm 5 1 1 -16 -16 -16 32 32

endif

# gpu 6d
if(${dimension} == 6) then

src/Main/Release/SFT.exe $outputdir/${gpu_code}_gpu_${dimension}dim_${r_text}${noise_text}.txt $outputdir/${gpu_code}_gpu_${dimension}dim_${r_text}${noise_text}_dft.ppm 6 1 1 -16 -16 -16 -16 32 32

endif

end # noise_option

@ dimension = ${dimension} + 1

# echo dimension ${dimension}

end # dimension

end # gpu_code

endif

# lloyd relaxation
if(1) then

set num_points = 1000;
set num_iterations = 100;
set toroidal_jitter_amount = 1;

set dotsize = 0.001;

src/Main/Release/LloydRelaxationRandom ${num_points} ${num_iterations} ${toroidal_jitter_amount} > ${outputdir}/lloyd_p${num_points}_i${num_iterations}.txt

src/Main/Release/DrawSamples ${outputdir}/lloyd_p${num_points}_i${num_iterations}.txt ${outputdir}/lloyd_p${num_points}_i${num_iterations}_samples.fig 2 ${dotsize} 0
doc/fig2eps -f ${outputdir}/lloyd_p${num_points}_i${num_iterations}_samples.fig
epstopdf ${outputdir}/lloyd_p${num_points}_i${num_iterations}_samples.eps --outfile ${outputdir}/lloyd_p${num_points}_i${num_iterations}_samples.pdf

src/Main/Release/SFT ${outputdir}/lloyd_p${num_points}_i${num_iterations}.txt ${outputdir}/lloyd_p${num_points}_i${num_iterations}_sft.pfm 2 1 1 128 128;

src/Main/Release/PFM2PPM ${outputdir}/lloyd_p${num_points}_i${num_iterations}_sft.pfm ${outputdir}/lloyd_p${num_points}_i${num_iterations}_sft.ppm 1 1 0
endif

# p-pentomino regression
if(1) then

set grid_size = 20;
set template_size = 10;
set num_templates = 4;
set num_subdivisions = 2;
set polyomino = ppentomino;
set num_iteration = 100;

set dft_scale = 256;

foreach k (0 1 2 3 4)

src/Main/Release/PolyominoRelaxation ${polyomino} ${grid_size} ${template_size} ${num_templates} ${num_subdivisions} ${num_iteration} > ${outputdir}/${polyomino}_${grid_size}x${grid_size}_s${num_subdivisions}_${k}.txt

src/Main/Release/SFT ${outputdir}/${polyomino}_${grid_size}x${grid_size}_s${num_subdivisions}_${k}.txt ${outputdir}/${polyomino}_${grid_size}x${grid_size}_s${num_subdivisions}_${k}_sft.pfm 2 1 1 ${dft_scale} ${dft_scale}

end

src/Main/Release/RadialPlot ${outputdir}/${polyomino}_${grid_size}x${grid_size}_s${num_subdivisions}_0_sft.pfm ${outputdir}/${polyomino}_${grid_size}x${grid_size}_s${num_subdivisions}_1_sft.pfm ${outputdir}/${polyomino}_${grid_size}x${grid_size}_s${num_subdivisions}_2_sft.pfm ${outputdir}/${polyomino}_${grid_size}x${grid_size}_s${num_subdivisions}_3_sft.pfm ${outputdir}/${polyomino}_${grid_size}x${grid_size}_s${num_subdivisions}_4_sft.pfm ${outputdir}/${polyomino}_${grid_size}x${grid_size}_s${num_subdivisions}_average5_sft.pfm > ${outputdir}/${polyomino}_${grid_size}x${grid_size}_s${num_subdivisions}_dft_average5_radial.txt

src/Main/Release/PFM2PPM ${outputdir}/${polyomino}_${grid_size}x${grid_size}_s${num_subdivisions}_average5_sft.pfm ${outputdir}/${polyomino}_${grid_size}x${grid_size}_s${num_subdivisions}_average5_sft_mag.ppm 1 1 0

src/Main/Release/PFM2PPM ${outputdir}/${polyomino}_${grid_size}x${grid_size}_s${num_subdivisions}_average5_sft.pfm ${outputdir}/${polyomino}_${grid_size}x${grid_size}_s${num_subdivisions}_average5_sft_log.ppm 1 1 1

endif

# 3D
if(1) then
src/Main/Release/PoissonNDim.exe 3 0.05 30 2 0 0 -1> $outputdir/poisson_3dim_0p05_random.txt

src/Main/Release/SFT.exe $outputdir/poisson_3dim_0p05_random.txt $outputdir/poisson_3dim_0p05_random_dft_x.ppm 3 -64 1 1 128 128;

src/Main/Release/SFT.exe $outputdir/poisson_3dim_0p05_random.txt $outputdir/poisson_3dim_0p05_random_dft_y.ppm 3 1 -64 1 128 128;

src/Main/Release/SFT.exe $outputdir/poisson_3dim_0p05_random.txt $outputdir/poisson_3dim_0p05_random_dft_z.ppm 3 1 1 -64 128 128;
endif

if(1) then
#src/Main/Release/PoissonNDim.exe 2 0.02 30 0 0 0 -1 > $outputdir/poisson_2dim_0p02_bridson.txt

#src/Main/Release/PoissonNDim.exe 2 0.02 30 1 0 0 -1 > $outputdir/poisson_2dim_0p02_scanline.txt

src/Main/Release/PoissonNDim.exe 2 0.02 30 2 0 0 -1 > $outputdir/poisson_2dim_0p02_random.txt

src/Main/Release/PoissonNDim.exe 2 0.02 30 3 0 0 -1 > $outputdir/poisson_2dim_0p02_gpu.txt

#src/Main/Release/PoissonNDim.exe 2 0.02 90 4 1 1 -1 > $outputdir/poisson_2dim_0p02_dart.txt

#src/Main/Release/PoissonNDim.exe 2 0.02 30 5 0 0 -1 > $outputdir/poisson_2dim_0p02_gpu_bad.txt

#foreach i (bridson scanline random gpu dart gpu_bad)
foreach i (random gpu)
 src/Main/Release/SFT.exe $outputdir/poisson_2dim_0p02_${i}.txt $outputdir/poisson_2dim_0p02_${i}_dft.ppm 2 1 1 512 512;
end

foreach i (random)
 src/Main/Release/SFT.exe $outputdir/poisson_2dim_0p02_${i}.txt $outputdir/poisson_2dim_0p02_${i}_dft.pfm 2 1 1 512 512;
end

src/Main/Release/RadialPlot.exe $outputdir/poisson_2dim_0p02_random_dft.pfm $outputdir/poisson_2dim_0p02_random_radial.ppm > $outputdir/poisson_2dim_0p02_random_radial.txt
endif

if(1) then
foreach i (ramp uniform)

src/Main/Release/AdaptiveNDim 2 0.01 30 2 1.0 ${i} 1.0 1.0 > $outputdir/adaptive_${i}_2dim_0p01_sequential.txt

src/Main/Release/DrawSamples $outputdir/adaptive_${i}_2dim_0p01_sequential.txt $outputdir/adaptive_${i}_2dim_0p01_sequential_samples.fig 2 0.002 0

doc/fig2eps -f $outputdir/adaptive_${i}_2dim_0p01_sequential_samples.fig

epstopdf $outputdir/adaptive_${i}_2dim_0p01_sequential_samples.eps --outfile $outputdir/adaptive_${i}_2dim_0p01_sequential_samples.pdf
end

foreach i (uniform)
src/Main/Release/SFT.exe $outputdir/adaptive_${i}_2dim_0p01_sequential.txt $outputdir/adaptive_${i}_2dim_0p01_sequential_dft.ppm 2 1 1 512 512;
end

endif
