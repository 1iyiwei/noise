#!/bin/tcsh -f

set poisson_generator = AdaptiveNDim;
set output_name = adaptive_uniform;

# timing experiment
# make sure we only measure the computation part, not pre-memory-allocation
if(0) then
time src/Main/Release/${poisson_generator} 2 0.01 30 0 1.0 uniform 1 1 > /dev/null
time src/Main/Release/${poisson_generator} 3 0.046 30 0 1.0 uniform 1 1 1 > /dev/null
time src/Main/Release/${poisson_generator} 4 0.1 30 0 1.0 uniform 1 1 1 1 > /dev/null
time src/Main/Release/${poisson_generator} 5 0.158 30 0 1.0 uniform 1 1 1 1 1 > /dev/null
time src/Main/Release/${poisson_generator} 6 0.215 30 0 1.0 uniform 1 1 1 1 1 > /dev/null
endif

if(1) then

foreach i (0 1 2 3 4 5 6 7 8 9)
src/Main/Release/${poisson_generator} 3 0.046 30 0 1.0 uniform 1 1 1 > data/${output_name}_3dim_0p046_${i}.txt
src/Main/Release/SFT data/${output_name}_3dim_0p046_${i}.txt data/${output_name}_3dim_0p046_dft_${i}.pfm 3 1 1 1 128 128;
end

src/Main/Release/RadialPlot data/${output_name}_3dim_0p046_dft_0.pfm data/${output_name}_3dim_0p046_dft_1.pfm data/${output_name}_3dim_0p046_dft_2.pfm data/${output_name}_3dim_0p046_dft_3.pfm data/${output_name}_3dim_0p046_dft_4.pfm data/${output_name}_3dim_0p046_dft_5.pfm data/${output_name}_3dim_0p046_dft_6.pfm data/${output_name}_3dim_0p046_dft_7.pfm data/${output_name}_3dim_0p046_dft_8.pfm data/${output_name}_3dim_0p046_dft_9.pfm data/${output_name}_3dim_0p046_dft_average10.pfm > data/${output_name}_3dim_0p046_dft_average10_radial.txt

src/Main/Release/PFM2PPM data/${output_name}_3dim_0p046_dft_average10.pfm data/${output_name}_3dim_0p046_dft_average10_1_1_64.ppm 1 1 -64 1
src/Main/Release/PFM2PPM data/${output_name}_3dim_0p046_dft_average10.pfm data/${output_name}_3dim_0p046_dft_average10_1_64_1.ppm 1 -64 1 1
src/Main/Release/PFM2PPM data/${output_name}_3dim_0p046_dft_average10.pfm data/${output_name}_3dim_0p046_dft_average10_64_1_1.ppm -64 1 1 1
src/Main/Release/PFM2PPM data/${output_name}_3dim_0p046_dft_average10.pfm data/${output_name}_3dim_0p046_dft_average10_1_1_0.ppm 1 1 0 1
src/Main/Release/PFM2PPM data/${output_name}_3dim_0p046_dft_average10.pfm data/${output_name}_3dim_0p046_dft_average10_1_1_75.ppm 1 1 -75 1
src/Main/Release/PFM2PPM data/${output_name}_3dim_0p046_dft_average10.pfm data/${output_name}_3dim_0p046_dft_average10_1_1_79.ppm 1 1 -79 1

endif

if(1) then
foreach i (0 1 2 3 4 5 6 7 8 9)
src/Main/Release/${poisson_generator} 4 0.1 30 0 1.0 uniform 1 1 1 1 > data/${output_name}_4dim_0p10_${i}.txt
src/Main/Release/SFT data/${output_name}_4dim_0p10_${i}.txt data/${output_name}_4dim_0p10_dft_${i}.pfm 4 1 1 1 1 64 64;
end

src/Main/Release/RadialPlot data/${output_name}_4dim_0p10_dft_0.pfm data/${output_name}_4dim_0p10_dft_1.pfm data/${output_name}_4dim_0p10_dft_2.pfm data/${output_name}_4dim_0p10_dft_3.pfm data/${output_name}_4dim_0p10_dft_4.pfm data/${output_name}_4dim_0p10_dft_5.pfm data/${output_name}_4dim_0p10_dft_6.pfm data/${output_name}_4dim_0p10_dft_7.pfm data/${output_name}_4dim_0p10_dft_8.pfm data/${output_name}_4dim_0p10_dft_9.pfm data/${output_name}_4dim_0p10_dft_average10.pfm > data/${output_name}_4dim_0p10_dft_average10_radial.txt

src/Main/Release/PFM2PPM data/${output_name}_4dim_0p10_dft_average10.pfm data/${output_name}_4dim_0p10_dft_average10_1_1_32_32.ppm 1 1 -32 -32 1
src/Main/Release/PFM2PPM data/${output_name}_4dim_0p10_dft_average10.pfm data/${output_name}_4dim_0p10_dft_average10_1_32_1_32.ppm 1 -32 1 -32 1
src/Main/Release/PFM2PPM data/${output_name}_4dim_0p10_dft_average10.pfm data/${output_name}_4dim_0p10_dft_average10_1_32_32_1.ppm 1 -32 -32 1 1
src/Main/Release/PFM2PPM data/${output_name}_4dim_0p10_dft_average10.pfm data/${output_name}_4dim_0p10_dft_average10_32_1_1_32.ppm -32 1 1 -32 1
src/Main/Release/PFM2PPM data/${output_name}_4dim_0p10_dft_average10.pfm data/${output_name}_4dim_0p10_dft_average10_32_1_32_1.ppm -32 1 -32 1 1
src/Main/Release/PFM2PPM data/${output_name}_4dim_0p10_dft_average10.pfm data/${output_name}_4dim_0p10_dft_average10_32_32_1_1.ppm -32 -32 1 1 1
src/Main/Release/PFM2PPM data/${output_name}_4dim_0p10_dft_average10.pfm data/${output_name}_4dim_0p10_dft_average10_1_1_32_0.ppm 1 1 -32 0 1
src/Main/Release/PFM2PPM data/${output_name}_4dim_0p10_dft_average10.pfm data/${output_name}_4dim_0p10_dft_average10_1_1_32_36.ppm 1 1 -32 -36 1
src/Main/Release/PFM2PPM data/${output_name}_4dim_0p10_dft_average10.pfm data/${output_name}_4dim_0p10_dft_average10_1_1_32_40.ppm 1 1 -32 -40 1
endif

if(1) then

foreach i (0 1 2 3 4 5 6 7 8 9)
src/Main/Release/${poisson_generator} 5 0.158 30 0 1.0 uniform 1 1 1 1 1 > data/${output_name}_5dim_0p158_${i}.txt
src/Main/Release/SFT data/${output_name}_5dim_0p158_${i}.txt data/${output_name}_5dim_0p158_dft_${i}.pfm 5 1 1 1 1 1 32 32;
end

src/Main/Release/RadialPlot data/${output_name}_5dim_0p158_dft_0.pfm data/${output_name}_5dim_0p158_dft_1.pfm data/${output_name}_5dim_0p158_dft_2.pfm data/${output_name}_5dim_0p158_dft_3.pfm data/${output_name}_5dim_0p158_dft_4.pfm data/${output_name}_5dim_0p158_dft_5.pfm data/${output_name}_5dim_0p158_dft_6.pfm data/${output_name}_5dim_0p158_dft_7.pfm data/${output_name}_5dim_0p158_dft_8.pfm data/${output_name}_5dim_0p158_dft_9.pfm data/${output_name}_5dim_0p158_dft_average10.pfm > data/${output_name}_5dim_0p158_dft_average10_radial.txt

src/Main/Release/PFM2PPM data/${output_name}_5dim_0p158_dft_average10.pfm data/${output_name}_5dim_0p158_dft_average10_1_1_16_16_16.ppm 1 1 -16 -16 -16 1
src/Main/Release/PFM2PPM data/${output_name}_5dim_0p158_dft_average10.pfm data/${output_name}_5dim_0p158_dft_average10_16_1_16_1_16.ppm -16 1 -16 1 -16 1
src/Main/Release/PFM2PPM data/${output_name}_5dim_0p158_dft_average10.pfm data/${output_name}_5dim_0p158_dft_average10_16_16_16_1_1.ppm -16 -16 -16 1 1 1
src/Main/Release/PFM2PPM data/${output_name}_5dim_0p158_dft_average10.pfm data/${output_name}_5dim_0p158_dft_average10_1_1_16_16_0.ppm 1 1 -16 -16 0 1
src/Main/Release/PFM2PPM data/${output_name}_5dim_0p158_dft_average10.pfm data/${output_name}_5dim_0p158_dft_average10_1_1_16_16_18.ppm 1 1 -16 -16 -18 1
src/Main/Release/PFM2PPM data/${output_name}_5dim_0p158_dft_average10.pfm data/${output_name}_5dim_0p158_dft_average10_1_1_16_16_20.ppm 1 1 -16 -16 -20 1

endif

if(1) then
foreach i (0 1 2 3 4 5 6 7 8 9)
src/Main/Release/${poisson_generator} 6  0.215 30 0 1.0 uniform 1 1 1 1 1 1 > data/${output_name}_6dim_0p215_${i}.txt
src/Main/Release/SFT data/${output_name}_6dim_0p215_${i}.txt data/${output_name}_6dim_0p215_dft_${i}.pfm 6 1 1 1 1 1 1 16 16;
end

src/Main/Release/RadialPlot data/${output_name}_6dim_0p215_dft_0.pfm data/${output_name}_6dim_0p215_dft_1.pfm data/${output_name}_6dim_0p215_dft_2.pfm data/${output_name}_6dim_0p215_dft_3.pfm data/${output_name}_6dim_0p215_dft_4.pfm data/${output_name}_6dim_0p215_dft_5.pfm data/${output_name}_6dim_0p215_dft_6.pfm data/${output_name}_6dim_0p215_dft_7.pfm data/${output_name}_6dim_0p215_dft_8.pfm data/${output_name}_6dim_0p215_dft_9.pfm data/${output_name}_6dim_0p215_dft_average10.pfm > data/${output_name}_6dim_0p215_dft_average10_radial.txt

src/Main/Release/PFM2PPM data/${output_name}_6dim_0p215_dft_average10.pfm data/${output_name}_6dim_0p215_dft_average10_1_1_8_8_8_8.ppm 1 1 -8 -8 -8 -8 1
src/Main/Release/PFM2PPM data/${output_name}_6dim_0p215_dft_average10.pfm data/${output_name}_6dim_0p215_dft_average10_8_1_8_8_1_8.ppm -8 1 -8 -8 1 -8 1
src/Main/Release/PFM2PPM data/${output_name}_6dim_0p215_dft_average10.pfm data/${output_name}_6dim_0p215_dft_average10_8_8_8_8_1_1.ppm -8 -8 -8 -8 1 1 1
src/Main/Release/PFM2PPM data/${output_name}_6dim_0p215_dft_average10.pfm data/${output_name}_6dim_0p215_dft_average10_1_1_8_8_8_0.ppm 1 1 -8 -8 -8 0 1
src/Main/Release/PFM2PPM data/${output_name}_6dim_0p215_dft_average10.pfm data/${output_name}_6dim_0p215_dft_average10_1_1_8_8_8_9.ppm 1 1 -8 -8 -8 -9 1
src/Main/Release/PFM2PPM data/${output_name}_6dim_0p215_dft_average10.pfm data/${output_name}_6dim_0p215_dft_average10_1_1_8_8_8_10.ppm 1 1 -8 -8 -8 -10 1
endif

if(0) then
#foreach i (0.138 0.148 0.158 0.168 0.178)
foreach i (0.160)
src/Main/Release/${poisson_generator} 5 ${i} 30 0 1.0 uniform 1 1 1 1 1 > data/${output_name}_5dim_${i}.txt
src/Main/Release/SFT data/${output_name}_5dim_${i}.txt data/${output_name}_5dim_${i}_dft.pfm 5 1 1 1 1 1 32 32;
src/Main/Release/PFM2PPM data/${output_name}_5dim_${i}_dft.pfm data/${output_name}_5dim_${i}_dft.ppm 1 1 -16 -16 -16;
end
endif

if(0) then

src/Main/Release/PFM2PPM data/${output_name}_5dim_0p158_dft_average10.pfm data/${output_name}_5dim_0p158_dft_average10_1_1_16_16_15.ppm 1 1 -16 -16 -15
src/Main/Release/PFM2PPM data/${output_name}_5dim_0p158_dft_average10.pfm data/${output_name}_5dim_0p158_dft_average10_16_1_16_1_15.ppm -16 1 -16 1 -15
src/Main/Release/PFM2PPM data/${output_name}_5dim_0p158_dft_average10.pfm data/${output_name}_5dim_0p158_dft_average10_16_16_15_1_1.ppm -16 -16 -15 1 1

endif
