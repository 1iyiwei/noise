#!/bin/tcsh -f

# zone plate
if(0) then

#set r = 0.002; #159329
#set r = 0.001; #637866

#set r = 0.0016; #249851
#set r = 0.0015; #285011

set r = 0.00155; #266889

src/Main/Release/AdaptiveNDim 2 ${r} 30 2 1.0 uniform 1 1 > data/adaptive_2dim_zoneplate_rebuttal.txt

src/Main/Release/FlyingZonePlate 2 0 data/adaptive_2dim_zoneplate_rebuttal.txt data/zoneplate_rebuttal.ppm 16 512 1.5

endif

# adaptive sampling, victor
if(0) then

set r = 0.01;
set rtext = 0p01;

src/Main/Release/AdaptiveNDim 2 ${r} 30 2 1.0 data/importance_ostro.pfm 1 1 > data/importance_ostro.txt

cp data/importance_ostro.txt data/importance_ostro_samples_${rtext}.txt

src/Main/Release/DrawSamples data/importance_ostro_samples_${rtext}.txt data/importance_ostro_samples_${rtext}.ppm 2 512 1

convert data/importance_ostro_samples_${rtext}.ppm data/importance_ostro_samples_${rtext}.png

endif

# adaptive sampling, kopf
if(0) then

set r = 0.0016;
set rtext = 0p0016;

#set r = 0.002;
#set rtext = 0p002;

src/Main/Release/AdaptiveNDim 2 ${r} 300 2 1.0 data/importance_kopf.pfm 1 1 > data/importance_kopf.txt

cp data/importance_kopf.txt data/importance_kopf_samples_${rtext}.txt

src/Main/Release/DrawSamples data/importance_kopf_samples_${rtext}.txt data/importance_kopf_samples_${rtext}.ppm 2 845 1

convert data/importance_kopf_samples_${rtext}.ppm data/importance_kopf_samples_${rtext}.png

if(0) then
nice src/Main/Release/DrawSamples data/importance_kopf_samples_${rtext}.txt data/importance_kopf_samples_${rtext}_samples.fig 2 0.0005 0

nice doc/fig2eps -f data/importance_kopf_samples_${rtext}_samples.fig

nice epstopdf data/importance_kopf_samples_${rtext}_samples.eps --outfile data/importance_kopf_samples_${rtext}_samples.pdf
endif

endif

# Figure 2, comparing different synthesis orders

if(0) then

#foreach k (4 5 6 7 8 9 10 30)
#foreach order (1 2 3)
#foreach lvl (1 3 5)

foreach k (10)
foreach order (1 2)
foreach lvl (1 3 5)

set ordertext = order${order};

set r = 0.01;
set rtext = 0p01;

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

#echo poisson_2dim_${rtext}_${ordertext}_lvl${lvl}

foreach i (0 1 2 3 4 5 6 7 8 9)

if(${order} == 3) then
 src/Main/Release/AdaptiveNDIM 2 ${r} ${k} 2 1 uniform 1 1 > data/poisson_2dim_${rtext}_k${k}_${ordertext}_lvl${lvl}_${i}.txt
else
 src/Main/Release/PoissonNDim 2 ${r} ${k} ${order} ${lvl} 0 -1 > data/poisson_2dim_${rtext}_k${k}_${ordertext}_lvl${lvl}_${i}.txt
endif

src/Main/Release/SFT data/poisson_2dim_${rtext}_k${k}_${ordertext}_lvl${lvl}_${i}.txt data/poisson_2dim_${rtext}_k${k}_${ordertext}_lvl${lvl}_${i}_dft.pfm 2 1 1 192 768

#src/Main/Release/PFM2PPM data/poisson_2dim_${rtext}_k${k}_${ordertext}_lvl${lvl}_${i}_dft.pfm data/poisson_2dim_${rtext}_k${k}_${ordertext}_lvl${lvl}_${i}_dft.ppm 1 1 0

#convert data/poisson_2dim_${rtext}_k${k}_${ordertext}_lvl${lvl}_${i}_dft.ppm ../../../liyiwei-msra/Research/BlueNoise/doc/figs/raster/poisson_2dim_${rtext}_k${k}_${ordertext}_lvl${lvl}_dft.png

end

#wc -l data/poisson_2dim_${rtext}_k${k}_${ordertext}_lvl${lvl}_?.txt

if(1) then
src/Main/Release/RadialPlot data/poisson_2dim_${rtext}_k${k}_${ordertext}_lvl${lvl}_0_dft.pfm data/poisson_2dim_${rtext}_k${k}_${ordertext}_lvl${lvl}_1_dft.pfm data/poisson_2dim_${rtext}_k${k}_${ordertext}_lvl${lvl}_2_dft.pfm data/poisson_2dim_${rtext}_k${k}_${ordertext}_lvl${lvl}_3_dft.pfm data/poisson_2dim_${rtext}_k${k}_${ordertext}_lvl${lvl}_4_dft.pfm data/poisson_2dim_${rtext}_k${k}_${ordertext}_lvl${lvl}_5_dft.pfm data/poisson_2dim_${rtext}_k${k}_${ordertext}_lvl${lvl}_6_dft.pfm data/poisson_2dim_${rtext}_k${k}_${ordertext}_lvl${lvl}_7_dft.pfm data/poisson_2dim_${rtext}_k${k}_${ordertext}_lvl${lvl}_8_dft.pfm data/poisson_2dim_${rtext}_k${k}_${ordertext}_lvl${lvl}_9_dft.pfm data/poisson_2dim_${rtext}_k${k}_${ordertext}_lvl${lvl}_average_10_dft.pfm > data/poisson_2dim_${rtext}_k${k}_${ordertext}_lvl${lvl}_average_10_radial.txt

src/Main/Release/PFM2PPM data/poisson_2dim_${rtext}_k${k}_${ordertext}_lvl${lvl}_average_10_dft.pfm data/poisson_2dim_${rtext}_k${k}_${ordertext}_lvl${lvl}_average_10_dft.ppm 1 1 1
endif

end
end
end

endif

# reference white noise for Figure 2 stuff
if(0) then

foreach i (0 1 2 3 4 5 6 7 8 9)
 src/Main/Release/RandomDart 2 6441 > data/white_2dim_6441_${i}.txt;
 src/Main/Release/SFT data/white_2dim_6441_${i}.txt data/white_2dim_6441_${i}.pfm 2 1 1 512 512
end

src/Main/Release/RadialPlot data/white_2dim_6441_0_dft.pfm data/white_2dim_6441_1_dft.pfm data/white_2dim_6441_2_dft.pfm data/white_2dim_6441_3_dft.pfm data/white_2dim_6441_4_dft.pfm data/white_2dim_6441_5_dft.pfm data/white_2dim_6441_6_dft.pfm data/white_2dim_6441_7_dft.pfm data/white_2dim_6441_8_dft.pfm data/white_2dim_6441_9_dft.pfm data/white_2dim_6441_average_10_dft.pfm > data/white_2dim_6441_average_10_radial.txt

src/Main/Release/PFM2PPM data/white_2dim_6441_average_10_dft.pfm data/white_2dim_6441_average_10_dft.ppm 1 1 1

endif
