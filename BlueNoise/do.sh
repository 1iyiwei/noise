#!/bin/tcsh -f

if(0) then
#foreach i (0 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1.0)
foreach i (0.91 0.92 0.93 0.94 0.95 0.96 0.97 0.98 0.99)
#foreach i (0.5)
src/Main/Release/RandomJitter.exe 57 ${i} > data/random_jitter_${i}.txt;
src/Main/Release/DrawSamples.exe data/random_jitter_${i}.txt data/random_jitter_samples_${i}.ppm 512
src/Main/Release/SFT.exe data/random_jitter_${i}.txt data/random_jitter_${i}.ppm 512
end
endif

if(0) then
foreach i (0 1 2 3 4 5)
foreach j (0 1 2 3 4 5 6 7 8 9)
foreach k (0 1 2 3 4 5 6 7 8 9)

echo poisson_3dim_0.05_signed_distance_${i}${j}${k}

src/Main/Release/PoissonSignedDistance.exe 3 0.020 data/poisson_3dim_0.05.txt  data/poisson_3dim_0.05_signed_distance_${i}${j}${k}.ppm 512 2 ${i}${j}${k}

end
end
end
endif

if(0) then

foreach i (0 1 2 3 4 5 6 7)
#foreach i (1 3 5)
 src/Main/Release/PoissonNDim.exe 2 0.01 30 1 ${i} 0 > data/poisson_2dim_0p01_scanline_lvl${i}.txt

 src/Main/Release/SFT.exe data/poisson_2dim_0p01_scanline_lvl${i}.txt data/poisson_2dim_0p01_scanline_lvl${i}_dft.ppm 256

 src/Main/Release/PoissonNDim.exe 2 0.01 30 2 ${i} 0 > data/poisson_2dim_0p01_random_lvl${i}.txt

 src/Main/Release/SFT.exe data/poisson_2dim_0p01_random_lvl${i}.txt data/poisson_2dim_0p01_random_lvl${i}_dft.ppm 256

end

endif

if(0) then

foreach i (1 3 5)

src/Main/Release/SFT.exe data/poisson_2dim_0p01_scanline_lvl${i}.txt d:/home/lywei/trees/liyiwei-msra/Research/BlueNoise/doc/figs/raster/poisson_2dim_0p01_scanline_lvl${i}_dft.ppm 256

src/Main/Release/SFT.exe data/poisson_2dim_0p01_random_lvl${i}.txt d:/home/lywei/trees/liyiwei-msra/Research/BlueNoise/doc/figs/raster/poisson_2dim_0p01_random_lvl${i}_dft.ppm 256

end

endif

if(0) then

foreach i (512)
src/Main/Release/SFT.exe data/poisson_2dim_0p01_random_lvl0.txt data/foo_dft_${i}.ppm ${i}
end

endif

if(0) then

 src/Main/Release/PoissonNDim.exe 2 0.01 30 2 0 0 > data/poisson_2dim_0p01_random.txt

# max: 291.862, 320.493
 src/Main/Release/SFT.exe data/poisson_2dim_0p01_random.txt data/poisson_2dim_0p01_random_dft.ppm 256

 src/Main/Release/PoissonNDim.exe 2 0.01 30 0 0 0 > data/poisson_2dim_0p01_bridson.txt

# max: 324.262, 294.66
 src/Main/Release/SFT.exe data/poisson_2dim_0p01_bridson.txt data/poisson_2dim_0p01_bridson_dft.ppm 256

endif

if(0) then

foreach i (0 1 2 3 4 5 6 7 8 9)
 src/Main/Release/PoissonNDim.exe 2 0.02 30 2 0 0 > data/poisson_2dim_0p02_random_${i}.txt;
 src/Main/Release/SFT.exe data/poisson_2dim_0p02_random_${i}.txt data/poisson_2dim_0p02_random_dft_${i}.pfm 512 512;
end

 src/Main/Release/RadialPlot.exe data/poisson_2dim_0p02_random_dft_0.pfm data/poisson_2dim_0p02_random_dft_1.pfm data/poisson_2dim_0p02_random_dft_2.pfm data/poisson_2dim_0p02_random_dft_3.pfm data/poisson_2dim_0p02_random_dft_4.pfm data/poisson_2dim_0p02_random_dft_5.pfm data/poisson_2dim_0p02_random_dft_6.pfm data/poisson_2dim_0p02_random_dft_7.pfm data/poisson_2dim_0p02_random_dft_8.pfm data/poisson_2dim_0p02_random_dft_9.pfm data/poisson_2dim_0p02_random_dft_average10.ppm > data/poisson_2dim_0p02_random_dft_average10_radial.txt
endif

if(0) then

foreach i (0 1 2 3 4 5 6 7 8 9)
 src/Main/Release/PoissonNDim.exe 2 0.02 30 3 0 0 > data/poisson_2dim_0p02_gpu_${i}.txt;
 src/Main/Release/SFT.exe data/poisson_2dim_0p02_gpu_${i}.txt data/poisson_2dim_0p02_gpu_dft_${i}.pfm 2 1 1 512 512;
end

 src/Main/Release/RadialPlot.exe data/poisson_2dim_0p02_gpu_dft_0.pfm data/poisson_2dim_0p02_gpu_dft_1.pfm data/poisson_2dim_0p02_gpu_dft_2.pfm data/poisson_2dim_0p02_gpu_dft_3.pfm data/poisson_2dim_0p02_gpu_dft_4.pfm data/poisson_2dim_0p02_gpu_dft_5.pfm data/poisson_2dim_0p02_gpu_dft_6.pfm data/poisson_2dim_0p02_gpu_dft_7.pfm data/poisson_2dim_0p02_gpu_dft_8.pfm data/poisson_2dim_0p02_gpu_dft_9.pfm data/poisson_2dim_0p02_gpu_dft_average10.ppm > data/poisson_2dim_0p02_gpu_dft_average10_radial.txt
endif

if(0) then
foreach i (0 1 2 3 4 5 6 7 8 9)
 src/Main/Release/PoissonNDim.exe 2 0.02 30 0 0 0 > data/poisson_2dim_0p02_bridson_${i}.txt;
 src/Main/Release/SFT.exe data/poisson_2dim_0p02_bridson_${i}.txt data/poisson_2dim_0p02_bridson_dft_${i}.pfm 512 512;
end

 src/Main/Release/RadialPlot.exe data/poisson_2dim_0p02_bridson_dft_0.pfm data/poisson_2dim_0p02_bridson_dft_1.pfm data/poisson_2dim_0p02_bridson_dft_2.pfm data/poisson_2dim_0p02_bridson_dft_3.pfm data/poisson_2dim_0p02_bridson_dft_4.pfm data/poisson_2dim_0p02_bridson_dft_5.pfm data/poisson_2dim_0p02_bridson_dft_6.pfm data/poisson_2dim_0p02_bridson_dft_7.pfm data/poisson_2dim_0p02_bridson_dft_8.pfm data/poisson_2dim_0p02_bridson_dft_9.pfm data/poisson_2dim_0p02_bridson_dft_average10.ppm > data/poisson_2dim_0p02_bridson_dft_average10_radial.txt
endif

if(0) then

foreach i (0 1 2 3 4 5 6 7 8 9)
 src/Main/Release/RandomDart.exe 1600 > data/poisson_2dim_0p02_white_${i}.txt;
 src/Main/Release/SFT.exe data/poisson_2dim_0p02_white_${i}.txt data/poisson_2dim_0p02_white_dft_${i}.pfm 512 512;
end

 src/Main/Release/RadialPlot.exe data/poisson_2dim_0p02_white_dft_0.pfm data/poisson_2dim_0p02_white_dft_1.pfm data/poisson_2dim_0p02_white_dft_2.pfm data/poisson_2dim_0p02_white_dft_3.pfm data/poisson_2dim_0p02_white_dft_4.pfm data/poisson_2dim_0p02_white_dft_5.pfm data/poisson_2dim_0p02_white_dft_6.pfm data/poisson_2dim_0p02_white_dft_7.pfm data/poisson_2dim_0p02_white_dft_8.pfm data/poisson_2dim_0p02_white_dft_9.pfm data/poisson_2dim_0p02_white_dft_average10.ppm > data/poisson_2dim_0p02_white_dft_average10_radial.txt

endif

if(0) then

foreach i (0 1 2 3 4 5 6 7 8 9)
 src/Main/Release/PoissonNDim.exe 2 0.02 90 4 1 1 > data/poisson_2dim_0p02_dart_${i}.txt;
 src/Main/Release/SFT.exe data/poisson_2dim_0p02_dart_${i}.txt data/poisson_2dim_0p02_dart_dft_${i}.pfm 512 512;
end

 src/Main/Release/RadialPlot.exe data/poisson_2dim_0p02_dart_dft_0.pfm data/poisson_2dim_0p02_dart_dft_1.pfm data/poisson_2dim_0p02_dart_dft_2.pfm data/poisson_2dim_0p02_dart_dft_3.pfm data/poisson_2dim_0p02_dart_dft_4.pfm data/poisson_2dim_0p02_dart_dft_5.pfm data/poisson_2dim_0p02_dart_dft_6.pfm data/poisson_2dim_0p02_dart_dft_7.pfm data/poisson_2dim_0p02_dart_dft_8.pfm data/poisson_2dim_0p02_dart_dft_9.pfm data/poisson_2dim_0p02_dart_dft_average10.ppm > data/poisson_2dim_0p02_dart_dft_average10_radial.txt
endif

if(0) then

foreach i (dart bridson random white)
 src/Main/Release/DrawSamples.exe data/poisson_2dim_0p02_${i}_0.txt data/poisson_2dim_0p02_${i}_samples.ppm 256 1
end

endif

if(0) then

foreach i (dart bridson random white)
 src/Main/Release/RadialPlot.exe data/poisson_2dim_0p02_${i}_dft_0.pfm data/poisson_2dim_0p02_${i}_dft_1.pfm data/poisson_2dim_0p02_${i}_dft_2.pfm data/poisson_2dim_0p02_${i}_dft_3.pfm data/poisson_2dim_0p02_${i}_dft_4.pfm data/poisson_2dim_0p02_${i}_dft_5.pfm data/poisson_2dim_0p02_${i}_dft_6.pfm data/poisson_2dim_0p02_${i}_dft_7.pfm data/poisson_2dim_0p02_${i}_dft_8.pfm data/poisson_2dim_0p02_${i}_dft_9.pfm data/poisson_2dim_0p02_${i}_dft_average10.ppm > data/poisson_2dim_0p02_${i}_dft_average10_radial.txt
end

endif


if(0) then

foreach i (0 1 2 3 4 5 6 7 8 9)
src/Main/Release/PoissonNDim.exe 3 0.05 30 2 0 0 > data/poisson_3dim_0p05_random_${i}.txt
src/Main/Release/SFT.exe data/poisson_3dim_0p05_random_${i}.txt data/poisson_3dim_0p05_random_dft_${i}.pfm 3 1 1 1 128 128;
end

src/Main/Release/RadialPlot.exe data/poisson_3dim_0p05_random_dft_0.pfm data/poisson_3dim_0p05_random_dft_1.pfm data/poisson_3dim_0p05_random_dft_2.pfm data/poisson_3dim_0p05_random_dft_3.pfm data/poisson_3dim_0p05_random_dft_4.pfm data/poisson_3dim_0p05_random_dft_5.pfm data/poisson_3dim_0p05_random_dft_6.pfm data/poisson_3dim_0p05_random_dft_7.pfm data/poisson_3dim_0p05_random_dft_8.pfm data/poisson_3dim_0p05_random_dft_9.pfm data/poisson_3dim_0p05_random_dft_average10.pfm > data/poisson_3dim_0p05_random_dft_average10_radial.txt
endif

if(0) then
#src/Main/Release/RadialPlot.exe data/poisson_3dim_0p05_random_dft_0.pfm data/poisson_3dim_0p05_random_dft_1.pfm data/poisson_3dim_0p05_random_dft_2.pfm data/poisson_3dim_0p05_random_dft_3.pfm data/poisson_3dim_0p05_random_dft_4.pfm data/poisson_3dim_0p05_random_dft_5.pfm data/poisson_3dim_0p05_random_dft_6.pfm data/poisson_3dim_0p05_random_dft_7.pfm data/poisson_3dim_0p05_random_dft_8.pfm data/poisson_3dim_0p05_random_dft_9.pfm data/poisson_3dim_0p05_random_dft_average10.pfm > data/poisson_3dim_0p05_random_dft_average10_radial.txt

src/Main/Release/PFM2PPM.exe data/poisson_3dim_0p05_random_dft_average10.pfm data/poisson_3dim_0p05_random_dft_average10_1_1_64.ppm 1 1 -64
src/Main/Release/PFM2PPM.exe data/poisson_3dim_0p05_random_dft_average10.pfm data/poisson_3dim_0p05_random_dft_average10_1_64_1.ppm 1 -64 1
src/Main/Release/PFM2PPM.exe data/poisson_3dim_0p05_random_dft_average10.pfm data/poisson_3dim_0p05_random_dft_average10_64_1_1.ppm -64 1 1
src/Main/Release/PFM2PPM.exe data/poisson_3dim_0p05_random_dft_average10.pfm data/poisson_3dim_0p05_random_dft_average10_1_1_0.ppm 1 1 0
src/Main/Release/PFM2PPM.exe data/poisson_3dim_0p05_random_dft_average10.pfm data/poisson_3dim_0p05_random_dft_average10_1_1_71.ppm 1 1 -71
endif

if(0) then

src/Main/Release/PFM2PPM.exe data/poisson_3dim_0p05_random_dft_4.pfm data/poisson_3dim_0p05_random_dft_1_1_64.ppm 1 1 -64
src/Main/Release/PFM2PPM.exe data/poisson_3dim_0p05_random_dft_4.pfm data/poisson_3dim_0p05_random_dft_1_64_1.ppm 1 -64 1
src/Main/Release/PFM2PPM.exe data/poisson_3dim_0p05_random_dft_4.pfm data/poisson_3dim_0p05_random_dft_64_1_1.ppm -64 1 1
src/Main/Release/PFM2PPM.exe data/poisson_3dim_0p05_random_dft_4.pfm data/poisson_3dim_0p05_random_dft_1_1_0.ppm 1 1 0
src/Main/Release/PFM2PPM.exe data/poisson_3dim_0p05_random_dft_4.pfm data/poisson_3dim_0p05_random_dft_1_1_69.ppm 1 1 -69
src/Main/Release/PFM2PPM.exe data/poisson_3dim_0p05_random_dft_4.pfm data/poisson_3dim_0p05_random_dft_1_1_71.ppm 1 1 -71
src/Main/Release/PFM2PPM.exe data/poisson_3dim_0p05_random_dft_4.pfm data/poisson_3dim_0p05_random_dft_1_1_73.ppm 1 1 -73
src/Main/Release/PFM2PPM.exe data/poisson_3dim_0p05_random_dft_4.pfm data/poisson_3dim_0p05_random_dft_1_1_75.ppm 1 1 -75
src/Main/Release/PFM2PPM.exe data/poisson_3dim_0p05_random_dft_4.pfm data/poisson_3dim_0p05_random_dft_1_1_77.ppm 1 1 -77
src/Main/Release/PFM2PPM.exe data/poisson_3dim_0p05_random_dft_4.pfm data/poisson_3dim_0p05_random_dft_1_1_79.ppm 1 1 -79
endif

if(0) then
foreach i (0 1 2 3 4 5 6 7 8 9)
src/Main/Release/PoissonNDim.exe 4 0.10 30 2 0 0 > data/poisson_4dim_0p10_random_${i}.txt
src/Main/Release/SFT.exe data/poisson_4dim_0p10_random_${i}.txt data/poisson_4dim_0p10_random_dft_${i}.pfm 4 1 1 1 1 64 64;
end

src/Main/Release/RadialPlot.exe data/poisson_4dim_0p10_random_dft_0.pfm data/poisson_4dim_0p10_random_dft_1.pfm data/poisson_4dim_0p10_random_dft_2.pfm data/poisson_4dim_0p10_random_dft_3.pfm data/poisson_4dim_0p10_random_dft_4.pfm data/poisson_4dim_0p10_random_dft_5.pfm data/poisson_4dim_0p10_random_dft_6.pfm data/poisson_4dim_0p10_random_dft_7.pfm data/poisson_4dim_0p10_random_dft_8.pfm data/poisson_4dim_0p10_random_dft_9.pfm data/poisson_4dim_0p10_random_dft_average10.pfm > data/poisson_4dim_0p10_random_dft_average10_radial.txt

src/Main/Release/PFM2PPM.exe data/poisson_4dim_0p10_random_dft_average10.pfm data/poisson_4dim_0p10_random_dft_average10_1_1_32_32.ppm 1 1 -32 -32
src/Main/Release/PFM2PPM.exe data/poisson_4dim_0p10_random_dft_average10.pfm data/poisson_4dim_0p10_random_dft_average10_1_32_1_32.ppm 1 -32 1 -32
src/Main/Release/PFM2PPM.exe data/poisson_4dim_0p10_random_dft_average10.pfm data/poisson_4dim_0p10_random_dft_average10_32_32_1_1.ppm -32 -32 1 1
src/Main/Release/PFM2PPM.exe data/poisson_4dim_0p10_random_dft_average10.pfm data/poisson_4dim_0p10_random_dft_average10_1_1_32_0.ppm 1 1 -32 0
src/Main/Release/PFM2PPM.exe data/poisson_4dim_0p10_random_dft_average10.pfm data/poisson_4dim_0p10_random_dft_average10_1_1_32_36.ppm 1 1 -32 -36
src/Main/Release/PFM2PPM.exe data/poisson_4dim_0p10_random_dft_average10.pfm data/poisson_4dim_0p10_random_dft_average10_1_1_32_40.ppm 1 1 -32 -40
endif

if(0) then
foreach i (0 1 2 3 4 5 6 7 8 9)
src/Main/Release/PoissonNDim.exe 5 0.20 30 2 0 0 > data/poisson_5dim_0p20_random_${i}.txt
src/Main/Release/SFT.exe data/poisson_5dim_0p20_random_${i}.txt data/poisson_5dim_0p20_random_dft_${i}.pfm 5 1 1 1 1 1 32 32;
end

src/Main/Release/RadialPlot.exe data/poisson_5dim_0p20_random_dft_0.pfm data/poisson_5dim_0p20_random_dft_1.pfm data/poisson_5dim_0p20_random_dft_2.pfm data/poisson_5dim_0p20_random_dft_3.pfm data/poisson_5dim_0p20_random_dft_4.pfm data/poisson_5dim_0p20_random_dft_5.pfm data/poisson_5dim_0p20_random_dft_6.pfm data/poisson_5dim_0p20_random_dft_7.pfm data/poisson_5dim_0p20_random_dft_8.pfm data/poisson_5dim_0p20_random_dft_9.pfm data/poisson_5dim_0p20_random_dft_average10.pfm > data/poisson_5dim_0p20_random_dft_average10_radial.txt

src/Main/Release/PFM2PPM.exe data/poisson_5dim_0p20_random_dft_average10.pfm data/poisson_5dim_0p20_random_dft_average10_1_1_16_16_16.ppm 1 1 -16 -16 -16
src/Main/Release/PFM2PPM.exe data/poisson_5dim_0p20_random_dft_average10.pfm data/poisson_5dim_0p20_random_dft_average10_16_1_16_1_16.ppm -16 1 -16 1 -16
src/Main/Release/PFM2PPM.exe data/poisson_5dim_0p20_random_dft_average10.pfm data/poisson_5dim_0p20_random_dft_average10_16_16_16_1_1.ppm -16 -16 -16 1 1
src/Main/Release/PFM2PPM.exe data/poisson_5dim_0p20_random_dft_average10.pfm data/poisson_5dim_0p20_random_dft_average10_1_1_16_16_0.ppm 1 1 -16 -16 0
src/Main/Release/PFM2PPM.exe data/poisson_5dim_0p20_random_dft_average10.pfm data/poisson_5dim_0p20_random_dft_average10_1_1_16_16_18.ppm 1 1 -16 -16 -18
src/Main/Release/PFM2PPM.exe data/poisson_5dim_0p20_random_dft_average10.pfm data/poisson_5dim_0p20_random_dft_average10_1_1_16_16_20.ppm 1 1 -16 -16 -20

endif

if(0) then
foreach i (0 1 2 3 4 5 6 7 8 9)
src/Main/Release/PoissonNDim.exe 6 0.30 30 2 0 0 > data/poisson_6dim_0p30_random_${i}.txt
src/Main/Release/SFT.exe data/poisson_6dim_0p30_random_${i}.txt data/poisson_6dim_0p30_random_dft_${i}.pfm 6 1 1 1 1 1 1 16 16;
end

src/Main/Release/RadialPlot.exe data/poisson_6dim_0p30_random_dft_0.pfm data/poisson_6dim_0p30_random_dft_1.pfm data/poisson_6dim_0p30_random_dft_2.pfm data/poisson_6dim_0p30_random_dft_3.pfm data/poisson_6dim_0p30_random_dft_4.pfm data/poisson_6dim_0p30_random_dft_5.pfm data/poisson_6dim_0p30_random_dft_6.pfm data/poisson_6dim_0p30_random_dft_7.pfm data/poisson_6dim_0p30_random_dft_8.pfm data/poisson_6dim_0p30_random_dft_9.pfm data/poisson_6dim_0p30_random_dft_average10.pfm > data/poisson_6dim_0p30_random_dft_average10_radial.txt

src/Main/Release/PFM2PPM.exe data/poisson_6dim_0p30_random_dft_average10.pfm data/poisson_6dim_0p30_random_dft_average10_1_1_8_8_8_8.ppm 1 1 -8 -8 -8 -8
src/Main/Release/PFM2PPM.exe data/poisson_6dim_0p30_random_dft_average10.pfm data/poisson_6dim_0p30_random_dft_average10_8_1_8_8_1_8.ppm -8 1 -8 -8 1 -8
src/Main/Release/PFM2PPM.exe data/poisson_6dim_0p30_random_dft_average10.pfm data/poisson_6dim_0p30_random_dft_average10_8_8_8_8_1_1.ppm -8 -8 -8 -8 1 1
src/Main/Release/PFM2PPM.exe data/poisson_6dim_0p30_random_dft_average10.pfm data/poisson_6dim_0p30_random_dft_average10_1_1_8_8_8_0.ppm 1 1 -8 -8 -8 0
src/Main/Release/PFM2PPM.exe data/poisson_6dim_0p30_random_dft_average10.pfm data/poisson_6dim_0p30_random_dft_average10_1_1_8_8_8_9.ppm 1 1 -8 -8 -8 -9
src/Main/Release/PFM2PPM.exe data/poisson_6dim_0p30_random_dft_average10.pfm data/poisson_6dim_0p30_random_dft_average10_1_1_8_8_8_10.ppm 1 1 -8 -8 -8 -10
endif

if(0) then
nice src/Main/Release/UniformNDim 2 0.01 30 0 1.0 > data/uniform_2dim_0p01_sequential.txt
nice src/Main/Release/DrawSamples data/uniform_2dim_0p01_sequential.txt data/uniform_2dim_0p01_sequential_samples.ppm 2 512 0
nice src/Main/Release/SFT data/uniform_2dim_0p01_sequential.txt data/uniform_2dim_0p01_sequential_dft.ppm 2 1 1 512 512
endif

if(0) then
nice src/Main/Release/UniformNDim 2 0.02 30 0 0.02 > data/uniform_2dim_0p02_sequential_1res.txt
nice src/Main/Release/UniformNDim 2 0.02 30 0 1.0 > data/uniform_2dim_0p02_sequential_nres.txt
nice src/Main/Release/DrawSamples data/uniform_2dim_0p02_sequential_1res.txt data/uniform_2dim_0p02_sequential_1res_samples.ppm 2 512 0
nice src/Main/Release/DrawSamples data/uniform_2dim_0p02_sequential_nres.txt data/uniform_2dim_0p02_sequential_nres_samples.ppm 2 512 0
endif

if(0) then
foreach i (ramp)
nice src/Main/Release/AdaptiveNDim 2 0.01 30 0 0.1 ${i} 1.0 0.2 > data/adaptive_${i}_2dim_0p01_sequential_1res.txt
nice src/Main/Release/DrawSamples data/adaptive_${i}_2dim_0p01_sequential_1res.txt data/adaptive_${i}_2dim_0p01_sequential_1res_samples.fig 2 0.0005 0
nice doc/fig2eps -f data/adaptive_${i}_2dim_0p01_sequential_1res_samples.fig
nice epstopdf data/adaptive_${i}_2dim_0p01_sequential_1res_samples.eps --outfile data/adaptive_${i}_2dim_0p01_sequential_1res_samples.pdf
end
endif

if(0) then
foreach i (uniform)

rm data/adaptive_${i}_2dim_0p01_sequential.txt

#src/Main/Release/AdaptiveNDim 2 0.01 30 0 0.1 ${i} 1.0 1.0 > data/adaptive_${i}_2dim_0p01_sequential.txt

time src/Main/Release/AdaptiveNDim 2 0.01 30 2 1.0 ${i} 1.0 1.0 > data/adaptive_${i}_2dim_0p01_random_template.txt

src/Main/Release/DrawSamples data/adaptive_${i}_2dim_0p01_sequential.txt data/adaptive_${i}_2dim_0p01_sequential_samples.fig 2 0.002 0

#src/Main/Release/DrawSamples data/adaptive_${i}_2dim_0p01_sequential.txt data/adaptive_${i}_2dim_0p01_sequential_samples.ppm 2 512 0

doc/fig2eps -f data/adaptive_${i}_2dim_0p01_sequential_samples.fig

epstopdf data/adaptive_${i}_2dim_0p01_sequential_samples.eps --outfile data/adaptive_${i}_2dim_0p01_sequential_samples.pdf

src/Main/Release/SFT.exe data/adaptive_${i}_2dim_0p01_sequential.txt data/adaptive_${i}_2dim_0p01_sequential.ppm 2 1 1 512 512
end
endif

if(0) then
foreach i (uniform)

rm data/adaptive_${i}_2dim_0p015_sequential_1res.txt

#src/Main/Release/AdaptiveNDim 2 0.015 30 0 0.1 ${i} 1.0 1.0 > data/adaptive_${i}_2dim_0p015_sequential_1res.txt

time src/Main/Release/AdaptiveNDim 2 0.015 30 0 1.0 ${i} 1.0 1.0 > data/adaptive_${i}_2dim_0p015_sequential_1res.txt

src/Main/Release/DrawSamples data/adaptive_${i}_2dim_0p015_sequential_1res.txt data/adaptive_${i}_2dim_0p015_sequential_1res_samples.fig 2 0.002 0

#src/Main/Release/DrawSamples data/adaptive_${i}_2dim_0p015_sequential_1res.txt data/adaptive_${i}_2dim_0p015_sequential_1res_samples.ppm 2 512 0

doc/fig2eps -f data/adaptive_${i}_2dim_0p015_sequential_1res_samples.fig

epstopdf data/adaptive_${i}_2dim_0p015_sequential_1res_samples.eps --outfile data/adaptive_${i}_2dim_0p015_sequential_1res_samples.pdf

end
endif

# timing experiment
# make sure we only measure the computation part, not pre-memory-allocation
if(0) then
time src/Main/Release/AdaptiveNDim 2 0.01 30 0 1.0 uniform 1 1 > /dev/null
time src/Main/Release/AdaptiveNDim 3 0.046 30 0 1.0 uniform 1 1 1 > /dev/null
time src/Main/Release/AdaptiveNDim 4 0.1 30 0 1.0 uniform 1 1 1 1 > /dev/null
time src/Main/Release/AdaptiveNDim 5 0.158 30 0 1.0 uniform 1 1 1 1 1 > /dev/null
time src/Main/Release/AdaptiveNDim 6 0.215 30 0 1.0 uniform 1 1 1 1 1 > /dev/null
endif

# gpu demo
if(0) then

# for demo
src/GPU/Debug/PoissonGPU 2 0.01 4 4 1 2 1 1 0 1 1 1 > data/gpu_2dim_0p01.txt

src/Main/Release/DrawSamples data/gpu_2dim_0p01.txt data/gpu_2dim_0p01_samples.fig 2 0.002 0

doc/fig2eps -f data/gpu_2dim_0p01_samples.fig

epstopdf data/gpu_2dim_0p01_samples.eps --outfile data/gpu_2dim_0p01_samples.pdf
# src/Main/Release/SFT.exe data/gpu_2dim_0p01.txt data/gpu_2dim_0p01_dft.ppm 2 1 1 512 512;
endif

# comparison with gpu 3d toy
if(0) then
src/Main/Release/AdaptiveNDim 3 0.055 5 2 1 uniform 1 1 1 > data/cpu_3dim_0p055.txt;
src/Main/Release/SFT.exe data/cpu_3dim_0p055.txt data/cpu_3dim_0p055_dft.ppm 3 1 1 -64 128 128

endif

# visualize weirdness of initialization
if(0) then
src/GPU/Debug/PoissonGPU 3 0.1 4 2 1 -1 1 1 1 1 1 1 > /dev/null
endif

# gpu 3D toy
if(0) then
src/GPU/Debug/PoissonGPU 3 0.055 5 3 1 -1 0 1 1 1 1 1 > data/gpu_3dim_0p055.txt;

src/Main/Release/SFT.exe data/gpu_3dim_0p055.txt data/gpu_3dim_0p055_dft.ppm 3 1 1 -64 128 128

#src/Main/Release/SFT.exe data/gpu_3dim_0p055.txt data/gpu_3dim_0p055_dft.pfm 3 1 1 1 128 128;

#src/Main/Release/PFM2PPM data/gpu_3dim_0p055_dft.pfm data/gpu_3dim_0p055_dft.ppm  1 1 -64;
endif

# gpu 3D performance
if(0) then
src/GPU/Release/PoissonGPU 3 0.055 4 3 1 -1 0 1 1 1 2 64 > data/gpu_3dim_0p055.txt;
src/Main/Release/SFT.exe data/gpu_3dim_0p055.txt data/gpu_3dim_0p055_dft.ppm 3 1 1 -63 128 128

endif

# gpu 4D toy
if(0) then
#src/GPU/Debug/PoissonGPU 4 0.1 5 2 1 -1 0 1 1 1 1 1 1 > data/gpu_4dim_0p10.txt;
src/GPU/Debug/PoissonGPU 4 0.125 5 2 1 2 0 1 1 1 1 1 1 > data/gpu_4dim_0p125.txt;

src/Main/Release/SFT.exe data/gpu_4dim_0p125.txt data/gpu_4dim_0p125_dft.ppm 4 1 1 -32 -32 64 64

endif

# gpu 4D
# need to put neighbor offsets into a texture as it exceeds program constant
if(0) then
#src/GPU/Debug/PoissonGPU 4 0.0625 4 2 1 2 0 1 1 1 1 1 1 > data/gpu_4dim_0p0625.txt

src/GPU/Debug/PoissonGPU 4 0.20 5 2 1 -1 0 1 1 1 1 1 1 > data/gpu_4dim_0p20.txt
endif

# gpu 5d toy
if(0) then
src/GPU/Debug/PoissonGPU 5 0.28 5 1 1 2 0 1 1 1 1 1 1 1 > data/gpu_5dim_0p28.txt
src/Main/Release/SFT.exe data/gpu_5dim_0p28.txt data/gpu_5dim_0p28_dft.ppm 5 1 1 -16 -16 -16 32 32
endif

# gpu 5d perf
if(0) then
src/GPU/Release/PoissonGPU 5 0.28 4 1 1 2 0 1 0 1 2 2 8 4  > /dev/null
endif

# gpu 6d toy
if(0) then
src/GPU/Debug/PoissonGPU 6 0.307 5 1 1 2 0 1 1 1 1 1 1 1 1 > data/gpu_6dim_0p307.txt
#src/GPU/Release/PoissonGPU 6 0.307 5 2 1 4 0 1 1 1 1 1 1 1 1 > data/gpu_6dim_0p307.txt
src/Main/Release/SFT.exe data/gpu_6dim_0p307.txt data/gpu_6dim_0p307_dft.ppm 6 1 1 -16 -16 -16 -16 32 32

#src/Main/Release/SFT.exe data/gpu_6dim_0p307.txt data/gpu_6dim_0p307_110000_dft.ppm 6 1 1 -16 -16 -16 -16 32 32
#src/Main/Release/SFT.exe data/gpu_6dim_0p307.txt data/gpu_6dim_0p307_000011_dft.ppm 6 -16 -16 -16 -16 1 1 32 32
endif

# gpu 6d extreme toy for debugging
if(0) then
src/GPU/Debug/PoissonGPU 6 0.62 5 1 1 2 0 1 1 1 1 1 1 1 1 > data/gpu_6dim_0p62.txt
endif

# gpu 6d perf
if(0) then
src/GPU/Release/PoissonGPU 6 0.307 5 1 1 2 0 1 1 1 2 2 1 2 2 > /dev/null
endif

# 5D debug for grid artifact
if(0) then

src/Main/Release/AdaptiveNDim 5 0.158 30 0 1.0 uniform 1 1 1 1 1 > data/adaptive_uniform_5dim_0p158.txt

src/Main/Release/SFT data/adaptive_uniform_5dim_0p158.txt data/adaptive_uniform_5dim_0p158_dft.ppm 5 1 1 -16 -16 -16 32 32;

endif

# gpu demo for dumping out color images
if(0) then

src/GPU/Debug/PoissonGPU 2 0.02 30 2 1 2 1 1 1 1 > data/gpu_2dim_0p02.txt

mv poisson_color_s*ppm data/

src/Main/Release/DrawSamples data/gpu_2dim_0p02.txt data/gpu_2dim_0p02_samples.fig 2 0.002 1

doc/fig2eps -f data/gpu_2dim_0p02_samples.fig

epstopdf data/gpu_2dim_0p02_samples.eps --outfile data/gpu_2dim_0p02_samples.pdf
endif

if(0) then

# for demo
src/GPU/Release/PoissonGPU 2 0.0007 4 4 1 2 0 0 1 1 > data/gpu_2dim_0p0007.txt

src/Main/Release/DrawSamples data/gpu_2dim_0p0007.txt data/gpu_2dim_0p0007_samples.fig 2 0.00014 0

doc/fig2eps -f data/gpu_2dim_0p0007_samples.fig

epstopdf data/gpu_2dim_0p0007_samples.eps --outfile data/gpu_2dim_0p0007_samples.pdf

# src/Main/Release/SFT.exe data/gpu_2dim_0p0007.txt data/gpu_2dim_0p0007_sft.ppm 2 
endif

# k statistics
if(0) then

foreach i (1 2 3 4 5 6 7 8 9 10)
foreach k (1 2 3 4 5 6 7 8 9 10)

(src/Main/Release/AdaptiveNDim 2 0.01 ${k} 0 1.0 uniform 1 1 > data/adaptive_uniform_2dim_k${k}_${i}.txt) |& grep "total # of samples" >>& data/adaptive_uniform_2dim_k${k}_statistics.txt

(src/Main/Release/AdaptiveNDim 3 0.046 ${k} 0 1.0 uniform 1 1 1 > data/adaptive_uniform_3dim_k${k}_${i}.txt) |& grep "total # of samples" >>& data/adaptive_uniform_3dim_k${k}_statistics.txt

(src/Main/Release/AdaptiveNDim 4 0.1 ${k} 0 1.0 uniform 1 1 1 1 > data/adaptive_uniform_4dim_k${k}_${i}.txt) |& grep "total # of samples" >>& data/adaptive_uniform_4dim_k${k}_statistics.txt

(src/Main/Release/AdaptiveNDim 5 0.158 ${k} 0 1.0 uniform 1 1 1 1 1 > data/adaptive_uniform_5dim_k${k}_${i}.txt) |& grep "total # of samples" >>& data/adaptive_uniform_5dim_k${k}_statistics.txt

(src/Main/Release/AdaptiveNDim 6 0.215 ${k} 0 1.0 uniform 1 1 1 1 1 1 > data/adaptive_uniform_6dim_k${k}_${i}.txt) |& grep "total # of samples" >>& data/adaptive_uniform_6dim_k${k}_statistics.txt

end
end

endif

# conflict checks using parameters similar to dimensionality comparison figure
if(0) then

foreach k (30)

(src/Main/Release/AdaptiveNDim 2 0.01 ${k} 0 1.0 uniform 1 1 > /dev/null) |& grep "# conflict checks" >& data/adaptive_uniform_2dim_typical_k${k}_conflicts.txt

(src/Main/Release/AdaptiveNDim 3 0.046 ${k} 0 1.0 uniform 1 1 1 >/dev/null) |& grep "# conflict checks" >& data/adaptive_uniform_3dim_typical_k${k}_conflicts.txt

(src/Main/Release/AdaptiveNDim 4 0.1 ${k} 0 1.0 uniform 1 1 1 1 >/dev/null) |& grep "# conflict checks" >& data/adaptive_uniform_4dim_typical_k${k}_conflicts.txt

(src/Main/Release/AdaptiveNDim 5 0.158 ${k} 0 1.0 uniform 1 1 1 1 1 >/dev/null) |& grep "# conflict checks" >& data/adaptive_uniform_5dim_typical_k${k}_conflicts.txt

(src/Main/Release/AdaptiveNDim 6 0.215 ${k} 0 1.0 uniform 1 1 1 1 1 1 >/dev/null) |& grep "# conflict checks" >& data/adaptive_uniform_6dim_typical_k${k}_conflicts.txt

end

endif

# conflict checks in best case scenario
if(0) then

foreach k (30)

# 128 grid
(src/Main/Release/AdaptiveNDim 2 0.0111 ${k} 0 1.0 uniform 1 1 > /dev/null) |& grep "# conflict checks" >& data/adaptive_uniform_2dim_best_k${k}_conflicts.txt

# 32 grid
(src/Main/Release/AdaptiveNDim 3 0.0542 ${k} 0 1.0 uniform 1 1 1 >/dev/null) |& grep "# conflict checks" >& data/adaptive_uniform_3dim_best_k${k}_conflicts.txt

# 16 grid
(src/Main/Release/AdaptiveNDim 4 0.1251 ${k} 0 1.0 uniform 1 1 1 1 >/dev/null) |& grep "# conflict checks" >& data/adaptive_uniform_4dim_best_k${k}_conflicts.txt

# 8 grid
(src/Main/Release/AdaptiveNDim 5 0.2796 ${k} 0 1.0 uniform 1 1 1 1 1 >/dev/null) |& grep "# conflict checks" >& data/adaptive_uniform_5dim_best_k${k}_conflicts.txt

# 8 grid
(src/Main/Release/AdaptiveNDim 6 0.3062 ${k} 0 1.0 uniform 1 1 1 1 1 1 >/dev/null) |& grep "# conflict checks" >& data/adaptive_uniform_6dim_best_k${k}_conflicts.txt
end

endif

# conflict checks in worst case scenario
if(0) then

foreach k (30)

# 128 grid
(src/Main/Release/AdaptiveNDim 2 0.0220 ${k} 0 1.0 uniform 1 1 > /dev/null) |& grep "# conflict checks" >& data/adaptive_uniform_2dim_worst_k${k}_conflicts.txt

# 32 grid
(src/Main/Release/AdaptiveNDim 3 0.1082 ${k} 0 1.0 uniform 1 1 1 >/dev/null) |& grep "# conflict checks" >& data/adaptive_uniform_3dim_worst_k${k}_conflicts.txt

# 16 grid
(src/Main/Release/AdaptiveNDim 4 0.2499 ${k} 0 1.0 uniform 1 1 1 1 >/dev/null) |& grep "# conflict checks" >& data/adaptive_uniform_4dim_worst_k${k}_conflicts.txt

# 8 grid
(src/Main/Release/AdaptiveNDim 5 0.5590 ${k} 0 1.0 uniform 1 1 1 1 1 >/dev/null) |& grep "# conflict checks" >& data/adaptive_uniform_5dim_worst_k${k}_conflicts.txt

# 8 grid
(src/Main/Release/AdaptiveNDim 6 0.6123 ${k} 0 1.0 uniform 1 1 1 1 1 1 >/dev/null) |& grep "# conflict checks" >& data/adaptive_uniform_6dim_worst_k${k}_conflicts.txt
end

endif

# 3D toy for teaser
if(0) then
src/Main/Release/AdaptiveNDim 3 0.07 30 0 1.0 uniform 1 1 1 > data/adaptive_3dim_0p07.txt
endif

# zone plate 2D
if(0) then

src/Main/Release/AdaptiveNDim 2 0.002 30 0 1.0 uniform 1 1 > data/adaptive_2dim_0p002.txt

src/Main/Release/FlyingZonePlate 2 0 data/adaptive_2dim_0p002.txt data/zoneplate_2dim_0p002.ppm 16 512 1.5

endif

# zone plate 3D
# 225399 = 60*3756.65 = 60*61.2915^2
if(0) then

set velocity = 1

set scale = 16
#set scale = 1

set outputres = 256
set std = 1.5

#src/Main/Release/AdaptiveNDim 3 0.014 30 0 1.0 uniform 1 1 1 > data/adaptive_3dim_0p014.txt

src/Main/Release/FlyingZonePlate 3 $velocity data/adaptive_3dim_0p014.txt data/zoneplate_3dim_0p014.ppm $scale $outputres $std

endif

# slices of 2D Poisson samples
if(0) then

set velocity = 1

set scale = 16
#set scale = 1

set outputres = 256
set std = 1.5

set r = 0.013
set rtext = 0p013
set numslices = 60

if(0) then
set r = 0.005
set rtext = 0p005
set jitter = 0
set numslices = 9
endif

foreach jitter (0 1)
src/Scripts/poisson_2d_stack $r 30 $jitter $numslices data/adaptive_2dim_${rtext}j${jitter}x${numslices}.txt

#src/Main/Release/FlyingZonePlate 3 $velocity data/adaptive_2dim_${rtext}j${jitter}x${numslices}.txt data/zoneplate_2dim_${rtext}j${jitter}x${numslices}.ppm $scale $outputres $std
end

endif

# compute SFT for zone plate stuff
if(0) then

foreach jitter (0 1)
src/Main/Release/SFT data/adaptive_2dim_0p005j${jitter}x9.txt data/adaptive_2dim_0p005j${jitter}x9_sft_1_1_256.ppm 3 1 1 -256 512 512
src/Main/Release/SFT data/adaptive_2dim_0p005j${jitter}x9.txt data/adaptive_2dim_0p005j${jitter}x9_sft_1_256_1.ppm 3 1 -256 1 512 512
src/Main/Release/SFT data/adaptive_2dim_0p005j${jitter}x9.txt data/adaptive_2dim_0p005j${jitter}x9_sft_256_1_1.ppm 3 -256 1 1 512 512
end

foreach jitter (0 1)
src/Main/Release/SFT data/adaptive_2dim_0p013j${jitter}x60.txt data/adaptive_2dim_0p013j${jitter}x60_sft_1_1_128.ppm 3 1 1 -128 256 256
src/Main/Release/SFT data/adaptive_2dim_0p013j${jitter}x60.txt data/adaptive_2dim_0p013j${jitter}x60_sft_1_128_1.ppm 3 1 -128 1 256 256
src/Main/Release/SFT data/adaptive_2dim_0p013j${jitter}x60.txt data/adaptive_2dim_0p013j${jitter}x60_sft_128_1_1.ppm 3 -128 1 1 256 256
end

if(0) then
src/Main/Release/SFT data/adaptive_3dim_0p014.txt data/adaptive_3dim_0p014_sft_1_1_256.ppm 3 1 1 -256 512 512
src/Main/Release/SFT data/adaptive_3dim_0p014.txt data/adaptive_3dim_0p014_sft_1_256_1.ppm 3 1 -256 1 512 512
src/Main/Release/SFT data/adaptive_3dim_0p014.txt data/adaptive_3dim_0p014_sft_256_1_1.ppm 3 -256 1 1 512 512
endif

endif

if(0) then
src/Main/Release/SFT data/adaptive_2dim_0p005j1x9.txt data/adaptive_2dim_0p005j1x9_sft.pfm 3 1 1 1 128 128

src/Main/Release/PFM2PPM data/adaptive_2dim_0p005j1x9_sft.pfm data/adaptive_2dim_0p005j1x9_sft_1_1_64.ppm 1 1 -64
src/Main/Release/PFM2PPM data/adaptive_2dim_0p005j1x9_sft.pfm data/adaptive_2dim_0p005j1x9_sft_1_64_1.ppm 1 -64 1
src/Main/Release/PFM2PPM data/adaptive_2dim_0p005j1x9_sft.pfm data/adaptive_2dim_0p005j1x9_sft_64_1_1.ppm -64 1 1
endif

# useless; use above
if(0) then

#0.013 best

set r = 0.013
set rtext = 0p013

rm -f data/adaptive_2d_${rtext}x60.txt;

foreach i (0 1 2 3 4 5 6 7 8 9)
foreach j (0 1 2 3 4 5)

src/Main/Release/AdaptiveNDim 2 $r 30 0 1.0 uniform 1 1 >> data/adaptive_2d_${rtext}x60.txt

end
end

wc -l data/adaptive_2d_${rtext}x60.txt

endif

# comparison of flying zone plates
# useless
if(0) then

set velocity = 1

set scale = 16

set outputres = 256

set std = 1.5

set numslices = 60

src/Main/Release/FlyingZonePlate 3 $velocity data/adaptive_3dim_0p014.txt data/zoneplate_3dim_0p014.ppm $scale $outputres $std

src/Main/Release/FlyingZonePlate 3 $velocity data/adaptive_2d_0p013x${numslices}.txt data/zoneplate_2dim_0p013x${numslices}.ppm $scale $outputres $std

endif

if(0) then

foreach i (poisson_2dim_0p01_random_lvl1_dft poisson_2dim_0p01_random_lvl3_dft poisson_2dim_0p01_random_lvl5_dft poisson_2dim_0p01_scanline_lvl1_dft poisson_2dim_0p01_scanline_lvl3_dft poisson_2dim_0p01_scanline_lvl5_dft)

#convert -compress none data/${i}.png data/${i}.ppm;

#src/Main/Release/DrawPixels data/${i}.ppm doc/figs/handdrawn/${i}.fig 186 63 192 63 192

#svn add doc/figs/handdrawn/${i}.fig

end

set i = poisson_2dim_0p01_scanline_lvl1_dft
src/Main/Release/DrawPixels data/${i}.ppm doc/figs/handdrawn/${i}.fig 77 63 192 63 192

set i = poisson_2dim_0p01_scanline_lvl3_dft
src/Main/Release/DrawPixels data/${i}.ppm doc/figs/handdrawn/${i}.fig 111 63 192 63 192

set i = poisson_2dim_0p01_scanline_lvl5_dft
src/Main/Release/DrawPixels data/${i}.ppm doc/figs/handdrawn/${i}.fig 133 63 192 63 192

set i = poisson_2dim_0p01_random_lvl1_dft
src/Main/Release/DrawPixels data/${i}.ppm doc/figs/handdrawn/${i}.fig 198 63 192 63 192

set i = poisson_2dim_0p01_random_lvl3_dft
src/Main/Release/DrawPixels data/${i}.ppm doc/figs/handdrawn/${i}.fig 255 63 192 63 192

set i = poisson_2dim_0p01_random_lvl5_dft
src/Main/Release/DrawPixels data/${i}.ppm doc/figs/handdrawn/${i}.fig 255 63 192 63 192

endif

# adaptive gpu demo
if(0) then
src/GPU/Debug/AdaptiveGPU 2 0.01 4 2 1 2 1 1 1 ramp 1 1 > data/adaptive_gpu_2dim_0p01.txt

src/Main/Release/DrawSamples data/adaptive_gpu_2dim_0p01.txt data/adaptive_gpu_2dim_0p01_samples.fig 2 0.002 0

doc/fig2eps -f data/adaptive_gpu_2dim_0p01_samples.fig

epstopdf data/adaptive_gpu_2dim_0p01_samples.eps --outfile data/adaptive_gpu_2dim_0p01_samples.pdf
endif

# 2D comparison of regular grid, random jitter, and poisson
if(0) then

src/Main/Release/AdaptiveNDim 2 0.02 30 0 1.0 uniform 1 1 > data/gjp_poisson_2dim_0p02.txt
src/Main/Release/SFT data/gjp_poisson_2dim_0p02.txt data/gjp_poisson_2dim_0p02_sft.ppm 2 1 1 256 256
src/Main/Release/DrawSamples data/gjp_poisson_2dim_0p02.txt data/gjp_poisson_2dim_0p02_samples.fig 2 0.004 0
doc/fig2eps -f data/gjp_poisson_2dim_0p02_samples.fig
epstopdf data/gjp_poisson_2dim_0p02_samples.eps --outfile data/gjp_poisson_2dim_0p02_samples.pdf

src/Main/Release/RandomJitter 40 0 > data/gjp_grid_2dim_0p025.txt
src/Main/Release/SFT data/gjp_grid_2dim_0p025.txt data/gjp_grid_2dim_0p025_sft.ppm 2 1 1 256 256
src/Main/Release/DrawSamples data/gjp_grid_2dim_0p025.txt data/gjp_grid_2dim_0p025_samples.fig 2 0.004 0
doc/fig2eps -f data/gjp_grid_2dim_0p025_samples.fig
epstopdf data/gjp_grid_2dim_0p025_samples.eps --outfile data/gjp_grid_2dim_0p025_samples.pdf

src/Main/Release/RandomJitter 40 1 > data/gjp_jitter_2dim_0p025.txt
src/Main/Release/SFT data/gjp_jitter_2dim_0p025.txt data/gjp_jitter_2dim_0p025_sft.ppm 2 1 1 256 256
src/Main/Release/DrawSamples data/gjp_jitter_2dim_0p025.txt data/gjp_jitter_2dim_0p025_samples.fig 2 0.004 0
doc/fig2eps -f data/gjp_jitter_2dim_0p025_samples.fig
epstopdf data/gjp_jitter_2dim_0p025_samples.eps --outfile data/gjp_jitter_2dim_0p025_samples.pdf

endif

# toy case for ppt slides
if(0) then

set r = 0.177
set rtext = 0p177
set dotsize = 0.01

src/Main/Release/AdaptiveNDim 2 ${r} 30 0 1.0 uniform 1 1 > data/adaptive_2dim_${rtext}.txt

src/Main/Release/DrawSamples data/adaptive_2dim_${rtext}.txt data/adaptive_2dim_${rtext}_samples.fig 2 ${dotsize} -1 1 1
doc/fig2eps -f data/adaptive_2dim_${rtext}_samples.fig
epstopdf data/adaptive_2dim_${rtext}_samples.eps --outfile data/adaptive_2dim_${rtext}_samples.pdf
endif

# toy jittered grid for ppt slides
if(0) then

src/Main/Release/RandomJitter 8 0.8 > data/jitter_2dim_0p125.txt

set dotsize = 0.01

src/Main/Release/DrawSamples data/jitter_2dim_0p125.txt data/jitter_2dim_0p125_samples.fig 2 ${dotsize} -1 1 1
doc/fig2eps -f data/jitter_2dim_0p125_samples.fig
epstopdf data/jitter_2dim_0p125_samples.eps --outfile data/jitter_2dim_0p125_samples.pdf

endif

# toy ramp case for ppt slides
if(0) then

set r = 0.177
set rtext = 0p177
set dotsize = 0.01

src/Main/Release/AdaptiveNDim 2 ${r} 30 0 1.0 ramp 1 1 > data/ramp_2dim_${rtext}.txt

src/Main/Release/DrawSamples data/ramp_2dim_${rtext}.txt data/ramp_2dim_${rtext}_samples.fig 2 ${dotsize} -1 1 1
doc/fig2eps -f data/ramp_2dim_${rtext}_samples.fig
epstopdf data/ramp_2dim_${rtext}_samples.eps --outfile data/ramp_2dim_${rtext}_samples.pdf
endif

# kopf tile
if(0) then

set basepoint_num = 2048;
set tiling_size = 4;
set dft_scale = 512;

if(0) then
set basepoint_num = 64;
set tiling_size = 8;
set dft_scale = 256;

set basepoint_num = 512;
set tiling_size = 4;
set dft_scale = 256;

set basepoint_num = 512;
set tiling_size = 8;
set dft_scale = 512;
endif

set dotsize = 0.001;

src/Kopf/wang_tile_poisson data/tileset_${basepoint_num}.dat ${tiling_size} data/kopf_b${basepoint_num}_${tiling_size}x${tiling_size}.txt

src/Main/Release/DrawSamples data/kopf_b${basepoint_num}_${tiling_size}x${tiling_size}.txt data/kopf_b${basepoint_num}_${tiling_size}x${tiling_size}_samples.fig 2 ${dotsize} 0
doc/fig2eps -f data/kopf_b${basepoint_num}_${tiling_size}x${tiling_size}_samples.fig
epstopdf data/kopf_b${basepoint_num}_${tiling_size}x${tiling_size}_samples.eps --outfile data/kopf_b${basepoint_num}_${tiling_size}x${tiling_size}_samples.pdf

src/Main/Release/SFT data/kopf_b${basepoint_num}_${tiling_size}x${tiling_size}.txt data/kopf_b${basepoint_num}_${tiling_size}x${tiling_size}_sft.ppm 2 1 1 256 ${dft_scale}
endif

# kopf averaged spectrum
if(0) then

set basepoint_num = 2048;
set tiling_size = 4;
set dft_scale = 512;

foreach k (0 1 2 3 4 5 6 7 8 9)

src/Kopf/wang_tile_poisson data/tileset_${basepoint_num}.dat ${tiling_size} data/kopf_b${basepoint_num}_${tiling_size}x${tiling_size}_${k}.txt

src/Main/Release/SFT data/kopf_b${basepoint_num}_${tiling_size}x${tiling_size}_${k}.txt data/kopf_b${basepoint_num}_${tiling_size}x${tiling_size}_${k}_sft.pfm 2 1 1 512 ${dft_scale}

end

src/Main/Release/RadialPlot data/kopf_b${basepoint_num}_${tiling_size}x${tiling_size}_0_sft.pfm data/kopf_b${basepoint_num}_${tiling_size}x${tiling_size}_1_sft.pfm data/kopf_b${basepoint_num}_${tiling_size}x${tiling_size}_2_sft.pfm data/kopf_b${basepoint_num}_${tiling_size}x${tiling_size}_3_sft.pfm data/kopf_b${basepoint_num}_${tiling_size}x${tiling_size}_4_sft.pfm data/kopf_b${basepoint_num}_${tiling_size}x${tiling_size}_5_sft.pfm data/kopf_b${basepoint_num}_${tiling_size}x${tiling_size}_6_sft.pfm data/kopf_b${basepoint_num}_${tiling_size}x${tiling_size}_7_sft.pfm data/kopf_b${basepoint_num}_${tiling_size}x${tiling_size}_8_sft.pfm data/kopf_b${basepoint_num}_${tiling_size}x${tiling_size}_9_sft.pfm data/kopf_b${basepoint_num}_${tiling_size}x${tiling_size}_average_10.pfm > data/kopf_b${basepoint_num}_${tiling_size}x${tiling_size}_dft_average10_radial.txt

endif

# generate enough samples to compare with kopf
if(0) then

set r = 0.0044;
set rtext = 0p0044;
set dft_scale = 512;

foreach k (0 1 2 3 4 5 6 7 8 9)

src/Main/Release/AdaptiveNDim 2 $r 30 0 1.0 uniform 1 1 > data/adaptive_2dim_${rtext}_${k}.txt

src/Main/Release/SFT data/adaptive_2dim_${rtext}_${k}.txt data/adaptive_2dim_${rtext}_${k}_sft.pfm 2 1 1 512 ${dft_scale}

end

src/Main/Release/RadialPlot data/adaptive_2dim_${rtext}_0_sft.pfm data/adaptive_2dim_${rtext}_1_sft.pfm data/adaptive_2dim_${rtext}_2_sft.pfm data/adaptive_2dim_${rtext}_3_sft.pfm data/adaptive_2dim_${rtext}_4_sft.pfm data/adaptive_2dim_${rtext}_5_sft.pfm data/adaptive_2dim_${rtext}_6_sft.pfm data/adaptive_2dim_${rtext}_7_sft.pfm data/adaptive_2dim_${rtext}_8_sft.pfm data/adaptive_2dim_${rtext}_9_sft.pfm data/adaptive_2dim_${rtext}_average_10.pfm > data/adaptive_2dim_${rtext}_dft_average10_radial.txt

endif

# p-pentomino toy

if(0) then

set grid_size = 400;
set template_size = 10;
set num_templates = 1600;
set polyomino = ppentomino;
set num_iteration = 100;

set dft_scale = 512;

foreach k (0)

src/Main/Release/PolyominoRelaxation ${polyomino} ${grid_size} ${template_size} ${num_templates} ${num_iteration} > data/${polyomino}_${grid_size}x${grid_size}_${k}.txt

src/Main/Release/SFT data/${polyomino}_${grid_size}x${grid_size}_${k}.txt data/${polyomino}_${grid_size}x${grid_size}_${k}_sft.ppm 2 1 1 512 ${dft_scale}

end

endif

# p-pentomino test
if(0) then

set grid_size = 10;
set template_size = $grid_size;
set num_templates = 1;
set num_subdivisions = 5;
set polyomino = ppentomino;
set num_iteration = 100;

set dft_scale = 512;

#foreach k (0 1 2 3 4 5 6 7 8 9)
foreach k (0)
src/Main/Release/PolyominoRelaxation ${polyomino} ${grid_size} ${template_size} ${num_templates} ${num_subdivisions} ${num_iteration} > data/${polyomino}_${grid_size}x${grid_size}_${num_subdivisions}_${k}.txt

src/Main/Release/SFT data/${polyomino}_${grid_size}x${grid_size}_${num_subdivisions}_${k}.txt data/${polyomino}_${grid_size}x${grid_size}_${num_subdivisions}_${k}_sft.ppm 2 1 1 512 ${dft_scale}

#src/Main/Release/PFM2PPM data/${polyomino}_${grid_size}x${grid_size}_${num_subdivisions}_${k}_sft.pfm data/${polyomino}_${grid_size}x${grid_size}_${num_subdivisions}_${k}_sft.ppm 1 1
end

endif

# p-pentomino and g-hexomino
if(0) then

set num_iteration = 100;

set dft_scale = 512;

foreach option (2)

if($option == 0) then

set polyomino = ppentomino;
set grid_size = 100;
set template_size = 10;
set num_templates = 100;
set num_subdivisions = 2;

else if($option == 1) then

set polyomino = ppentomino;
set grid_size = 10;
set template_size = 10;
set num_templates = 1;
set num_subdivisions = 5;

else if($option == 2) then

set polyomino = ghexomino_subdivision;
set grid_size = 432;
set template_size = ${grid_size};
set num_templates = 1;
set num_subdivisions = 2;

else if($option == 3) then
# too slow
set polyomino = ghexomino;
set grid_size = 48;
set template_size = 48;
set num_templates = 1;
set num_subdivisions = 1;

else

endif

foreach k (0 1 2 3 4 5 6 7 8 9)
src/Main/Release/PolyominoRelaxation ${polyomino} ${grid_size} ${template_size} ${num_templates} ${num_subdivisions} ${num_iteration} > data/${polyomino}_${grid_size}x${grid_size}_${num_subdivisions}_${k}.txt

src/Main/Release/SFT data/${polyomino}_${grid_size}x${grid_size}_${num_subdivisions}_${k}.txt data/${polyomino}_${grid_size}x${grid_size}_${num_subdivisions}_${k}_sft.pfm 2 1 1 ${dft_scale} ${dft_scale}

end

src/Main/Release/RadialPlot data/${polyomino}_${grid_size}x${grid_size}_${num_subdivisions}_0_sft.pfm data/${polyomino}_${grid_size}x${grid_size}_${num_subdivisions}_1_sft.pfm data/${polyomino}_${grid_size}x${grid_size}_${num_subdivisions}_2_sft.pfm data/${polyomino}_${grid_size}x${grid_size}_${num_subdivisions}_3_sft.pfm data/${polyomino}_${grid_size}x${grid_size}_${num_subdivisions}_4_sft.pfm data/${polyomino}_${grid_size}x${grid_size}_${num_subdivisions}_5_sft.pfm data/${polyomino}_${grid_size}x${grid_size}_${num_subdivisions}_6_sft.pfm data/${polyomino}_${grid_size}x${grid_size}_${num_subdivisions}_7_sft.pfm data/${polyomino}_${grid_size}x${grid_size}_${num_subdivisions}_8_sft.pfm data/${polyomino}_${grid_size}x${grid_size}_${num_subdivisions}_9_sft.pfm data/${polyomino}_${grid_size}x${grid_size}_${num_subdivisions}_average_10_sft.pfm > data/${polyomino}_${grid_size}x${grid_size}_${num_subdivisions}_dft_average10_radial.txt

src/Main/Release/PFM2PPM data/${polyomino}_${grid_size}x${grid_size}_${num_subdivisions}_average_10_sft.pfm data/${polyomino}_${grid_size}x${grid_size}_${num_subdivisions}_average_10_sft_mag.ppm 1 1 0

src/Main/Release/PFM2PPM data/${polyomino}_${grid_size}x${grid_size}_${num_subdivisions}_average_10_sft.pfm data/${polyomino}_${grid_size}x${grid_size}_${num_subdivisions}_average_10_sft_log.ppm 1 1 1

end
#options

endif

# polyomino relaxation
if(0) then

set gridsize = 128;
set templatesize = 128;
set numtemplates = 1;
set num_subdivisions = 0;
set polyomino = monomino;
set numiteration = 5;
set dotsize = 0.001;

if(0) then
set gridsize = 10;
set templatesize = 10;
set numtemplates = 1;
set num_subdivisions = 3;
set polyomino = ppentomino;
set numiteration = 0;
set dotsize = 0.001;
endif

if(0) then
set gridsize = 100;
set templatesize = 10;
set numtemplates = 100;
set num_subdivisions = 0;
set polyomino = ppentomino;
set numiteration = 0;
set dotsize = 0.001;
endif

foreach numiteration (0 1 2 3 4 5 100)

src/Main/Release/PolyominoRelaxation ${polyomino} ${gridsize} ${templatesize} ${numtemplates} ${num_subdivisions} ${numiteration} > data/${polyomino}_${gridsize}x${gridsize}_s${num_subdivisions}_k${numiteration}.txt

src/Main/Release/DrawSamples data/${polyomino}_${gridsize}x${gridsize}_s${num_subdivisions}_k${numiteration}.txt data/${polyomino}_${gridsize}x${gridsize}_s${num_subdivisions}_k${numiteration}_samples.fig 2 ${dotsize} 0
doc/fig2eps -f data/${polyomino}_${gridsize}x${gridsize}_s${num_subdivisions}_k${numiteration}_samples.fig
epstopdf data/${polyomino}_${gridsize}x${gridsize}_s${num_subdivisions}_k${numiteration}_samples.eps --outfile data/${polyomino}_${gridsize}x${gridsize}_s${num_subdivisions}_k${numiteration}_samples.pdf

src/Main/Release/SFT data/${polyomino}_${gridsize}x${gridsize}_s${num_subdivisions}_k${numiteration}.txt data/${polyomino}_${gridsize}x${gridsize}_s${num_subdivisions}_k${numiteration}_sft.pfm 2 1 1 256 256

src/Main/Release/PFM2PPM data/${polyomino}_${gridsize}x${gridsize}_s${num_subdivisions}_k${numiteration}_sft.pfm data/${polyomino}_${gridsize}x${gridsize}_s${num_subdivisions}_k${numiteration}_sft.ppm 1 1 1

end

endif

# tiling visualization
if(0) then

set height = 10;
set width = 10;
set template_height = $height;
set template_width = $width;
set polyomino = monomino;
set num_templates = 0;
set num_subdivisions = 0;

if(0) then
set height = 10;
set width = 10;
set template_height = $height;
set template_width = $width;
set polyomino = ppentomino;
set num_templates = 1;
set num_subdivisions = 2;
endif

if(0) then
set height = 100;
set width = 100;
set template_height = 10;
set template_width = 10;
set polyomino = ppentomino;
set num_templates = 100;
set num_subdivisions = 2;
endif

if(0) then
set height = 80;
set width = 80;
set template_height = $height;
set template_width = $width;
set polyomino = ppentomino;
set num_templates = 1;
set num_subdivisions = 0;
endif

if(0) then
set height = 400;
set width = 400;
set template_height = $height;
set template_width = $width;
set polyomino = ppentomino;
set num_templates = 1;
set num_subdivisions = 0;
endif

if(0) then
set height = 100;
set width = 100;
set template_height = 10;
set template_width = 10;
set polyomino = ppentomino;
set num_templates = 100;
set num_subdivisions = 0;
endif

if(0) then
set height = 9;
set width = 12;
set template_height = $height;
set template_width = $width;
set polyomino = ghexomino;
set num_templates = 1;
set num_subdivisions = 0;
endif

if(0) then
# already very slow
set height = 24;
set width = 27;
set template_height = $height;
set template_width = $width;
set polyomino = ghexomino;
set num_templates = 1;
set num_subdivisions = 0;
endif

if(0) then
set height = 6;
set width = 10;
set template_height = 6;
set template_width = 10;
set polyomino = pentomino;
set num_templates = 1;
set num_subdivisions = 0;
endif

if(0) then
set height = 5;
set width = 8;
set template_height = 5;
set template_width = 8;
set polyomino = tetromino;
set num_templates = 1;
set num_subdivisions = 0;
endif

if(0) then
set height = 9;
set width = 9;
set template_height = 9;
set template_width = 9;
set polyomino = tromino;
set num_templates = 1;
set num_subdivisions = 0;
endif

if(0) then
set height = 40;
set width = ${height};
set template_height = ${height};
set template_width = ${width};
set polyomino = domino;
set num_templates = 1;
set num_subdivisions = 0;
endif

if(0) then
set height = 27;
set width = ${height};
set template_height = ${height};
set template_width = ${width};
set polyomino = ghexomino_subdivision;
set num_templates = 1;
set num_subdivisions = 1;
endif

echo ${polyomino} grid:${height}x${width} template:${template_height}x${template_width} num_templates:${num_templates} num_subdivisions:${num_subdivisions}

src/Main/Release/PolyominoTiling ${polyomino} ${height} ${width} ${template_height} ${template_width} ${num_templates} ${num_subdivisions} data/${polyomino}_${height}x${width}_s${num_subdivisions}.txt

src/Main/Release/PolyominoTiling data/${polyomino}_${height}x${width}_s${num_subdivisions}.txt ${height} ${width} ${template_height} ${template_width} ${num_templates} ${num_subdivisions} data/${polyomino}_${height}x${width}_s${num_subdivisions}.ppm
#> data/${polyomino}_${height}x${width}_matrix.txt

endif

# test polyomino IO routine
if(0) then

set height = 30;
set width = $height;
set template_height = $height;
set template_width = $width;
set polyomino = ppentomino;
set num_templates = 1;

src/Main/Release/PolyominoTiling ${polyomino} ${height} ${width} ${template_height} ${template_width} ${num_templates} data/${polyomino}_${height}x${width}.txt

src/Main/Release/PolyominoTiling data/${polyomino}_${height}x${width}.txt ${height} ${width} ${template_height} ${template_width} ${num_templates} data/${polyomino}_${height}x${width}.ppm

set gridsize = $height;
set templatesize = $height;
set numtemplates = 1;
set numiteration = 30;
set dotsize = 0.001;

src/Main/Release/PolyominoRelaxation data/${polyomino}_${height}x${width}.txt ${gridsize} ${templatesize} ${numtemplates} ${numiteration} > data/${polyomino}_${gridsize}x${gridsize}_k${numiteration}.txt

src/Main/Release/DrawSamples data/${polyomino}_${gridsize}x${gridsize}_k${numiteration}.txt data/${polyomino}_${gridsize}x${gridsize}_k${numiteration}_samples.fig 2 ${dotsize} 0
doc/fig2eps -f data/${polyomino}_${gridsize}x${gridsize}_k${numiteration}_samples.fig
epstopdf data/${polyomino}_${gridsize}x${gridsize}_k${numiteration}_samples.eps --outfile data/${polyomino}_${gridsize}x${gridsize}_k${numiteration}_samples.pdf
endif

# debug log tonemap
if(0) then

src/Main/Release/SFT data/poisson_2dim_0p02_gpu_0.txt data/poisson_2dim_0p02_gpu_0_sft.pfm 2 1 1 256 256;

src/Main/Release/PFM2PPM data/poisson_2dim_0p02_gpu_0_sft.pfm data/poisson_2dim_0p02_gpu_0_sft_log.ppm 1 1 1

src/Main/Release/PFM2PPM data/poisson_2dim_0p02_gpu_0_sft.pfm data/poisson_2dim_0p02_gpu_0_sft_mag.ppm 1 1 0

endif

# lloyd relaxation
if(0) then

set num_points = 1000;
set num_iterations = 100;
set toroidal_jitter_amount = 1;

set dotsize = 0.001;

src/Main/Release/LloydRelaxation ${num_points} ${num_iterations} ${toroidal_jitter_amount} > data/lloyd_p${num_points}_i${num_iterations}.txt

src/Main/Release/DrawSamples data/lloyd_p${num_points}_i${num_iterations}.txt data/lloyd_p${num_points}_i${num_iterations}_samples.fig 2 ${dotsize} 0
doc/fig2eps -f data/lloyd_p${num_points}_i${num_iterations}_samples.fig
epstopdf data/lloyd_p${num_points}_i${num_iterations}_samples.eps --outfile data/lloyd_p${num_points}_i${num_iterations}_samples.pdf

src/Main/Release/SFT data/lloyd_p${num_points}_i${num_iterations}.txt data/lloyd_p${num_points}_i${num_iterations}_sft.pfm 2 1 1 128 128;

src/Main/Release/PFM2PPM data/lloyd_p${num_points}_i${num_iterations}_sft.pfm data/lloyd_p${num_points}_i${num_iterations}_sft.ppm 1 1 0
endif

# toroidal hack
if(0) then

src/Main/Release/ToroidalHack data/ppentomino_100x100_2_average_10_sft.pfm data/ppentomino_100x100_2_average_10_sft_hacked.pfm

src/Main/Release/RadialPlot data/ppentomino_100x100_2_average_10_sft_hacked.pfm data/ppentomino_100x100_2_average_10_sft_hacked_out.pfm > data/ppentomino_100x100_2_average_10_radial.txt

src/Main/Release/PFM2PPM data/ppentomino_100x100_2_average_10_sft_hacked_out.pfm data/ppentomino_100x100_2_average_10_sft_hacked_out.ppm 1 1 1

endif

# toroidal hack
if(0) then

src/Main/Release/ToroidalHack data/ppentomino_10x10_5_average_10_sft.pfm data/ppentomino_10x10_5_average_10_sft_hacked.pfm

src/Main/Release/RadialPlot data/ppentomino_10x10_5_average_10_sft_hacked.pfm data/ppentomino_10x10_5_average_10_sft_hacked_out.pfm > data/ppentomino_10x10_5_average_10_radial.txt

src/Main/Release/PFM2PPM data/ppentomino_10x10_5_average_10_sft_hacked_out.pfm data/ppentomino_10x10_5_average_10_sft_hacked_out.ppm 1 1 1

endif

# toroidal hack
if(0) then

src/Main/Release/ToroidalHack data/ghexomino_subdivision_432x432_2_average_10_sft.pfm data/ghexomino_subdivision_432x432_2_average_10_sft_hacked.pfm

src/Main/Release/RadialPlot data/ghexomino_subdivision_432x432_2_average_10_sft_hacked.pfm data/ghexomino_subdivision_432x432_2_average_10_sft_hacked_out.pfm > data/ghexomino_subdivision_432x432_2_average_10_radial.txt

src/Main/Release/PFM2PPM data/ghexomino_subdivision_432x432_2_average_10_sft_hacked_out.pfm data/ghexomino_subdivision_432x432_2_average_10_sft_hacked_out.ppm 1 1 1

endif

# randomness test
if(0) then

src/Main/Release/SynthesisOrder 2 2048 0 data/hash.32
src/external/diehard/diehard data/hash.32 > data/hash.out

src/Main/Release/SynthesisOrder 2 2048 1 data/random.32
src/external/diehard/diehard data/random.32 > data/random.out

src/Main/Release/SynthesisOrder 2 2048 2 data/scanline.32
src/external/diehard/diehard data/scanline.32 > data/scanline.out

src/Main/Release/SynthesisOrder 2 2048 3 data/grid_phase_random.32
src/external/diehard/diehard data/grid_phase_random.32 > data/grid_phase_random.out

src/Main/Release/SynthesisOrder 2 2048 4 data/grid_phase_scanline.32
src/external/diehard/diehard data/grid_phase_scanline.32 > data/grid_phase_scanline.out

endif

# gpu demo and "chaotic simultaneous sampling"
if(0) then

# for demo
set dimension = 2;
set r_value = 0.01;
set r_text = 0p01;
set k_value = 4;
set num_cpu_levels = 4;
set root_cell_size = 1;
set clique_size = 2;
set boundary_condition = 1;
set neighbor_offset_storage = 1; # 0 for program constant, 1 for texture
set show_off = 1;
set importance_field_size = 1;

set dot_size = 0.001;

src/GPU/Debug/PoissonGPU ${dimension} ${r_value} ${k_value} ${num_cpu_levels} ${root_cell_size} ${clique_size} ${boundary_condition} ${neighbor_offset_storage} ${show_off} ${importance_field_size} ${importance_field_size} > data/gpu_${dimension}dim_${r_text}.txt

if(0) then
src/Main/Release/DrawSamples data/gpu_${dimension}dim_${r_text}.txt data/gpu_${dimension}dim_${r_text}_samples.fig ${dimension} ${dot_size} 0

src/Main/Release/DrawSamples data/gpu_${dimension}dim_${r_text}.txt data/gpu_${dimension}dim_${r_text}_samples.ppm ${dimension} 512 0

doc/fig2eps -f data/gpu_${dimension}dim_${r_text}_samples.fig

epstopdf data/gpu_${dimension}dim_${r_text}_samples.eps --outfile data/gpu_${dimension}dim_${r_text}_samples.pdf

# src/Main/Release/SFT.exe data/gpu_${dimension}dim_${r_text}.txt data/gpu_${dimension}dim_${r_text}_dft.ppm ${dimension} 1 1 512 512;
endif

endif

if(0) then

set dimension = 2;
set r_value = 0.0007;
set r_value = 0.01;
#set r_text = 0p0007;

set k_value = 4;
set num_cpu_levels = 4;
set root_cell_size = 1;
set clique_size = 2;
set boundary_condition = 0;
set neighbor_offset_storage = 0; # 0 for program constant, 1 for texture
set show_off = 0;
set importance_field_size = 1;

src/GPU/Release/PoissonGPU ${dimension} ${r_value} ${k_value} ${num_cpu_levels} ${root_cell_size} ${clique_size} ${boundary_condition} ${neighbor_offset_storage} ${show_off} ${importance_field_size} ${importance_field_size} > /dev/null

endif

# dump 3d samples for video
if(0) then

set dimension = 3;

if(0) then
set r_value = 0.07;
set r_text = 0p07;
set k_value = 30;
set synthesis_order = 0;
set cell_size = 1;
set importance_image_name = uniform;
set importance_field_size = 1 1 1;

src/Main/Release/AdaptiveNDim ${dimension} ${r_value} ${k_value} ${synthesis_order} ${cell_size} ${importance_image_name} ${importance_field_size} > data/adaptive_3dim_
endif

set radius = 0.04;
set color_scheme = 0;
set spin_delta = 2;

src/GPU/Release/Draw3DSamples data/adaptive_3dim_0p07.txt adaptive_3dim_0p07 ${dimension} ${radius} ${color_scheme} 0 1 ${spin_delta}

endif

# adaptive sampling for video

if(0) then

set dimension = 2;
set r_value = 0.015;
set r_text = 0p015;
set k_value = 30;
set synthesis_order = 0;
set cell_size = 1;

set importance_image_dir = '';
set importance_image_name = ramp;
set importance_image_name_ext = '';

if(0) then
set importance_image_dir = 'data/';
set importance_image_name = siggraph_logo_gradient;
set importance_image_name_ext = '.pfm';
endif

set importance_field_size = 1;

src/Main/Release/AdaptiveNDim ${dimension} ${r_value} ${k_value} ${synthesis_order} ${cell_size} ${importance_image_dir}${importance_image_name}${importance_image_name_ext} ${importance_field_size} ${importance_field_size} > data/adaptive_${dimension}dim_${importance_image_name}_${r_text}.txt

src/Main/Release/DrawSamples data/adaptive_${dimension}dim_${importance_image_name}_${r_text}.txt data/adaptive_${dimension}dim_${importance_image_name}_${r_text}.ppm ${dimension} 640 0

endif

# polyomino relaxation from monomino to pentomino
# all with 1280 samples
if(0) then

set num_templates = 1;
set num_subdivisions = 0;
set num_iteration = 100;

set dft_scale = 256;

foreach psize (1 2 3 4 5)

if(${psize} == 1) then
set grid_size = 36;
set polyomino = monomino;
endif

if(${psize} == 2) then
set grid_size = 50;
set polyomino = domino;
endif

if(${psize} == 3) then
set grid_size = 63;
set polyomino = tromino;
endif

if(${psize} == 4) then
set grid_size = 72;
set polyomino = tetromino;
endif

if(${psize} == 5) then
set grid_size = 80;
set polyomino = pentomino;
endif

set template_size = $grid_size;

foreach k (0 1 2 3 4 5 6 7 8 9)
#foreach k (0)

src/Main/Release/PolyominoTiling ${polyomino} ${grid_size} ${grid_size} ${template_size} ${template_size} ${num_templates} ${num_subdivisions} data/${polyomino}_${grid_size}x${grid_size}_tiling_${k}.txt

src/Main/Release/PolyominoTiling data/${polyomino}_${grid_size}x${grid_size}_tiling_${k}.txt ${grid_size} ${grid_size} ${template_size} ${template_size} ${num_templates} ${num_subdivisions} data/${polyomino}_${grid_size}x${grid_size}_tiling_${k}.ppm

src/Main/Release/PolyominoRelaxation data/${polyomino}_${grid_size}x${grid_size}_tiling_${k}.txt ${grid_size} ${template_size} ${num_templates} ${num_subdivisions} ${num_iteration} > data/${polyomino}_${grid_size}x${grid_size}_${k}.txt

src/Main/Release/SFT data/${polyomino}_${grid_size}x${grid_size}_${k}.txt data/${polyomino}_${grid_size}x${grid_size}_${k}_sft.pfm 2 1 1 ${dft_scale} ${dft_scale}

end

src/Main/Release/RadialPlot data/${polyomino}_${grid_size}x${grid_size}_0_sft.pfm data/${polyomino}_${grid_size}x${grid_size}_1_sft.pfm data/${polyomino}_${grid_size}x${grid_size}_2_sft.pfm data/${polyomino}_${grid_size}x${grid_size}_3_sft.pfm data/${polyomino}_${grid_size}x${grid_size}_4_sft.pfm data/${polyomino}_${grid_size}x${grid_size}_5_sft.pfm data/${polyomino}_${grid_size}x${grid_size}_6_sft.pfm data/${polyomino}_${grid_size}x${grid_size}_7_sft.pfm data/${polyomino}_${grid_size}x${grid_size}_8_sft.pfm data/${polyomino}_${grid_size}x${grid_size}_9_sft.pfm data/${polyomino}_${grid_size}x${grid_size}_average_10_sft.pfm > data/${polyomino}_${grid_size}x${grid_size}_average_10_radial.txt

src/Main/Release/PFM2PPM data/${polyomino}_${grid_size}x${grid_size}_average_10_sft.pfm data/${polyomino}_${grid_size}x${grid_size}_average_10_sft_mag.ppm 1 1 0
src/Main/Release/PFM2PPM data/${polyomino}_${grid_size}x${grid_size}_average_10_sft.pfm data/${polyomino}_${grid_size}x${grid_size}_average_10_sft_log.ppm 1 1 1
end

endif

if(0) then

src/Main/Release/DrawSamples data/ghexomino_subdivision_432x432_2_0.txt data/ghexomino_subdivision_432x432_2_0_samples.fig 2 0.001 0 1 1

doc/fig2eps -f data/ghexomino_subdivision_432x432_2_0_samples.fig

epstopdf data/ghexomino_subdivision_432x432_2_0_samples.eps --outfile data/ghexomino_subdivision_432x432_2_0_samples.pdf

endif

# SFTGPU toy
if(0) then

# for demo
src/GPU/Debug/PoissonGPU 2 0.01 4 4 1 2 1 1 1 1 1 > data/gpu_2dim_0p01.txt

src/GPU/Release/SFTGPU data/gpu_2dim_0p01.txt data/gpu_2dim_0p01_dft.pfm 2 1 1 512 512;

src/Main/Release/PFM2PPM data/gpu_2dim_0p01_dft.pfm data/gpu_2dim_0p01_dft.ppm 1 1 0

endif

# sample selection
if(0) then

src/Main/Release/PoissonNDim 2 0.01 30 4 1 1 > data/dart_2dim_0p01.txt
src/Main/Release/SelectSamples data/dart_2dim_0p01.txt data/dart_2dim_0p01_selected.txt 2 6000

src/GPU/Release/SFTGPU data/dart_2dim_0p01.txt data/dart_2dim_0p01_dft.pfm 2 1 1 512 512;
src/Main/Release/PFM2PPM data/dart_2dim_0p01_dft.pfm data/dart_2dim_0p01_dft.ppm 1 1 0

src/GPU/Release/SFTGPU data/dart_2dim_0p01_selected.txt data/dart_2dim_0p01_selected_dft.pfm 2 1 1 512 512;
src/Main/Release/PFM2PPM data/dart_2dim_0p01_selected_dft.pfm data/dart_2dim_0p01_selected_dft.ppm 1 1 0

endif

# phase group images for paper
if(0) then

foreach k (0 1 2)
src/Main/Release/PhaseGroupVisualization 18 3 ${k} data/phase_group_vis_${k}.ppm
end

endif

# BlueGPU demo
if(0) then

set exe_name = BlueGPU;
#set exe_name = PoissonGPU;

# 2D
set dimension = 2;
set r_value = 0.01;
set r_text = 0p01;
set num_cpu_levels = 2;
set importance = '1 1';

# 3D
if(0) then
set dimension = 3;
set r_value = 0.055;
set r_text = 0p055;
set num_cpu_levels = 2;
set importance = '1 1 1';
endif

# 4d
if(0) then
set dimension = 4;
set r_value = 0.125;
set r_text = 0p125;
set num_cpu_levels = 2;
set importance = '1 1 1 1';
endif

# 5d
if(0) then
set dimension = 5;
set r_value = 0.28;
set r_text = 0p28;
set num_cpu_levels = 2;
set importance = '1 1 1 1 1';
endif

# 6d
if(0) then
set dimension = 6;
set r_value = 0.307;
set r_text = 0p307;
set num_cpu_levels = 1;
set importance = '1 1 1 1 1 1';
endif

set k_number = 4;
set root_cell_size = 1;
set clique_size = -1;
set boundary_condition = 1; #(0 for none, 1 for toroidal)
set neighbor_offset_storage = 1; #(0 for program constant, 1 for texture)
set show_off = 1;

if(${exe_name} == 'BlueGPU') then
set noise_option = 1;
else
set noise_option = '';
endif

src/GPU/Debug/${exe_name} ${dimension} ${r_value} ${k_number} ${num_cpu_levels} ${root_cell_size} ${clique_size} ${boundary_condition} ${neighbor_offset_storage} ${noise_option} ${show_off} ${importance} > data/gpu_${dimension}dim_${r_text}.txt

src/GPU/Release/SFTGPU data/gpu_${dimension}dim_${r_text}.txt data/gpu_${dimension}dim_${r_text}_dft.pfm 2 1 1 512 512; src/Main/Release/PFM2PPM data/gpu_${dimension}dim_${r_text}_dft.pfm data/gpu_${dimension}dim_${r_text}_dft.ppm 1 1 0

endif

if(0) then

set outputdir = data;

foreach gpu_code (blue)

foreach dimension (2 3 4 5 6)

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
set neighbor_offset_storage = 1; #(0 for program constant, 1 for texture)
endif

if(${gpu_code} == 'blue') then
set noise_option = 1;
else
set noise_option = '';
endif

set show_off = 1;

#if(${dimension} != 6 || ${gpu_code} != 'poisson')
src/GPU/Debug/${gpu_code}GPU ${dimension} ${r_value} ${k_number} ${num_cpu_levels} ${root_cell_size} ${clique_size} ${boundary_condition} ${neighbor_offset_storage} ${noise_option} ${show_off} ${importance} > ${outputdir}/${gpu_code}_gpu_${dimension}dim_${r_text}.txt
#end

#set dimension = 0; # debug

# gpu 2D
if(${dimension} == 2) then

src/Main/Release/DrawSamples $outputdir/${gpu_code}_gpu_2dim_0p01.txt $outputdir/${gpu_code}_gpu_2dim_0p01_samples.fig 2 0.002 0

doc/fig2eps -f $outputdir/${gpu_code}_gpu_2dim_0p01_samples.fig

epstopdf $outputdir/${gpu_code}_gpu_2dim_0p01_samples.eps --outfile $outputdir/${gpu_code}_gpu_2dim_0p01_samples.pdf

#src/Main/Release/SFT.exe $outputdir/${gpu_code}_gpu_2dim_0p01.txt $outputdir/${gpu_code}_gpu_2dim_0p01_dft.ppm 2 1 1 512 512;

src/GPU/Release/SFTGPU $outputdir/${gpu_code}_gpu_2dim_0p01.txt $outputdir/${gpu_code}_gpu_2dim_0p01_dft.pfm 2 1 1 512 512;

src/Main/Release/PFM2PPM $outputdir/${gpu_code}_gpu_2dim_0p01_dft.pfm $outputdir/${gpu_code}_gpu_2dim_0p01_dft.ppm 1 1 0

endif

# gpu 3D
if(${dimension} == 3) then

src/Main/Release/SFT.exe $outputdir/${gpu_code}_gpu_3dim_0p055.txt $outputdir/${gpu_code}_gpu_3dim_0p055_dft.ppm 3 1 1 -64 128 128

endif

# gpu 4d
if(${dimension} == 4) then

src/Main/Release/SFT.exe $outputdir/${gpu_code}_gpu_4dim_0p125.txt $outputdir/${gpu_code}_gpu_4dim_0p125_dft.ppm 4 1 1 -32 -32 64 64

endif

# gpu 5d
if(${dimension} == 5) then

src/Main/Release/SFT.exe $outputdir/${gpu_code}_gpu_5dim_0p28.txt $outputdir/${gpu_code}_gpu_5dim_0p28_dft.ppm 5 1 1 -16 -16 -16 32 32

endif

# gpu 6d
if(${dimension} == 6) then

src/Main/Release/SFT.exe $outputdir/${gpu_code}_gpu_6dim_0p307.txt $outputdir/${gpu_code}_gpu_6dim_0p307_dft.ppm 6 1 1 -16 -16 -16 -16 32 32

endif

end # dimension

end # gpu_code

endif

# brute force dart throwing
if(0) then

set dimension = 2;
set r_value = 0.01;
set r_text = 0p01;
set k_number = 0;
set num_samples = 6856;

src/Main/Release/DartThrowing ${dimension} ${r_value} ${k_number} ${num_samples} > data/dart_${dimension}dim_r${r_text}_k${k_number}_s${num_samples}.txt

src/GPU/Release/SFTGPU data/dart_${dimension}dim_r${r_text}_k${k_number}_s${num_samples}.txt data/dart_${dimension}dim_r${r_text}_k${k_number}_s${num_samples}_dft.pfm 2 1 1 512 512;

src/Main/Release/PFM2PPM data/dart_${dimension}dim_r${r_text}_k${k_number}_s${num_samples}_dft.pfm data/dart_${dimension}dim_r${r_text}_k${k_number}_s${num_samples}_dft.ppm 1 1 0

endif

# Figure 2, comparing different synthesis orders

if(0) then

foreach k (4)
foreach order (1 2)
foreach lvl (1 3 5)

set ordertext = order${order};

if(${order} == 1) then
 set ordertext = "scanline";
else if(${order} == 2) then
 set ordertext = "random";
else if(${order} == 3) then
 set ordertext = "gpu";
else
 echo "unknown synthesis order";
 exit(1);
endif

#echo poisson_2dim_0p01_${ordertext}_lvl${lvl}

foreach i (0)

if(${order} == 3) then
 src/Main/Release/AdaptiveNDIM 2 0.01 ${k} 2 1 uniform 1 1 > data/poisson_2dim_0p01_k${k}_${ordertext}_lvl${lvl}_${i}.txt
else
 src/Main/Release/PoissonNDim 2 0.01 ${k} ${order} ${lvl} 0 -1 > data/poisson_2dim_0p01_k${k}_${ordertext}_lvl${lvl}_${i}.txt
endif

 src/Main/Release/SFT data/poisson_2dim_0p01_k${k}_${ordertext}_lvl${lvl}_${i}.txt data/poisson_2dim_0p01_k${k}_${ordertext}_lvl${lvl}_${i}_dft.pfm 2 1 1 512 512

src/Main/Release/PFM2PPM data/poisson_2dim_0p01_k${k}_${ordertext}_lvl${lvl}_${i}_dft.pfm data/poisson_2dim_0p01_k${k}_${ordertext}_lvl${lvl}_${i}_dft.ppm 1 1 0

if(0) then
src/Main/Release/DrawSamples data/poisson_2dim_0p01_k${k}_${ordertext}_lvl${lvl}_${i}.txt data/poisson_2dim_0p01_k${k}_${ordertext}_lvl${lvl}_${i}_samples.fig 2 0.002 0

doc/fig2eps -f data/poisson_2dim_0p01_k${k}_${ordertext}_lvl${lvl}_${i}_samples.fig

epstopdf data/poisson_2dim_0p01_k${k}_${ordertext}_lvl${lvl}_${i}_samples.eps --outfile data/poisson_2dim_0p01_k${k}_${ordertext}_lvl${lvl}_${i}_samples.pdf
endif

end

end
end
end

endif

# old data for Figure 2
if(0) then

foreach order (scanline random)
foreach lvl (1 3 5)

#src/Main/Release/SFT ../../../liyiwei-msra/Research/BlueNoise/doc/figs/raster/poisson_2dim_0p01_${order}_lvl${lvl}.txt data/poisson_2dim_0p01_${order}_lvl${lvl}_dft.pfm 2 1 1 512 1024

src/Main/Release/PFM2PPM data/poisson_2dim_0p01_${order}_lvl${lvl}_dft.pfm data/poisson_2dim_0p01_${order}_lvl${lvl}_dft.ppm 1 1 0 1.28285

convert data/poisson_2dim_0p01_${order}_lvl${lvl}_dft.ppm ../../../liyiwei-msra/Research/BlueNoise/doc/figs/raster/poisson_2dim_0p01_${order}_lvl${lvl}_dft.png

end
end

endif

# victor
if(0) then

foreach rtext (0p01 0p02 0p04 0p08)
 src/Main/Release/DrawSamples ../../../liyiwei-msra/Research/BlueNoise/review/importance_ostro_samples_${rtext}.txt data/importance_ostro_samples_${rtext}.fig 2 0.002 0 1 1

 doc/fig2eps -f data/importance_ostro_samples_${rtext}.fig

 epstopdf data/importance_ostro_samples_${rtext}.eps --outfile ../../../liyiwei-msra/Research/BlueNoise/doc/figs/raster/importance_ostro_samples_${rtext}.pdf
 # data/importance_ostro_samples_${rtext}.pdf
end

endif

# lagae averaged spectrum
if(0) then

set dft_scale = 512;

foreach k (0 1 2 3 4 5 6 7 8 9)

src/Main/Release/SFT data/distribution_${k}.txt data/lagae_33856_${k}_sft.pfm 2 1 1 512 ${dft_scale}

end

src/Main/Release/RadialPlot data/lagae_33856_0_sft.pfm data/lagae_33856_1_sft.pfm data/lagae_33856_2_sft.pfm data/lagae_33856_3_sft.pfm data/lagae_33856_4_sft.pfm data/lagae_33856_5_sft.pfm data/lagae_33856_6_sft.pfm data/lagae_33856_7_sft.pfm data/lagae_33856_8_sft.pfm data/lagae_33856_9_sft.pfm data/lagae_33856_average_10.pfm > data/lagae_33856_dft_average10_radial.txt

endif

# smaller sample sets for comparing the spatial layouts for ghex and mine
if(0) then

set r = 0.0396;
set rtext = 0p0396;

set dotsize = 0.01;

foreach k (0)

src/Main/Release/AdaptiveNDim 2 $r 10 0 1.0 uniform 1 1 > data/adaptive_2dim_${rtext}_${k}.txt

src/Main/Release/DrawSamples data/adaptive_2dim_${rtext}_${k}.txt data/adaptive_2dim_${rtext}_${k}_samples.fig 2 ${dotsize} 1 1 1

doc/fig2eps -f data/adaptive_2dim_${rtext}_${k}_samples.fig

epstopdf data/adaptive_2dim_${rtext}_${k}_samples.eps --outfile data/adaptive_2dim_${rtext}_${k}_samples.pdf

end

set polyomino = ghexomino_subdivision;
set grid_size = 48;
set template_size = ${grid_size};
set num_templates = 1;
set num_subdivisions = 1;

set num_iteration = 100;

foreach k (0)

src/Main/Release/PolyominoRelaxation ${polyomino} ${grid_size} ${template_size} ${num_templates} ${num_subdivisions} ${num_iteration} > data/${polyomino}_${grid_size}x${grid_size}_${num_subdivisions}_${k}.txt

src/Main/Release/DrawSamples data/${polyomino}_${grid_size}x${grid_size}_${num_subdivisions}_${k}.txt data/${polyomino}_${grid_size}x${grid_size}_${num_subdivisions}_${k}_samples.fig 2 ${dotsize} 1 1 1

doc/fig2eps -f data/${polyomino}_${grid_size}x${grid_size}_${num_subdivisions}_${k}_samples.fig

epstopdf data/${polyomino}_${grid_size}x${grid_size}_${num_subdivisions}_${k}_samples.eps --outfile data/${polyomino}_${grid_size}x${grid_size}_${num_subdivisions}_${k}_samples.pdf

end

endif
