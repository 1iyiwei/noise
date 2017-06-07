#!/bin/tcsh -f

foreach k (4 5 6 7 8 9 10)
 ./txt2dat.exe data/poisson_2dim_0p01_k${k}_gpu_mean.txt data/poisson_2dim_0p01_k${k}_gpu_variance.txt data/white_2dim_6441_mean.txt data/white_2dim_6441_variance.txt data/poisson_2dim_0p01_k${k}_gpu_mean.dat data/poisson_2dim_0p01_k${k}_gpu_variance.dat
end

wgnuplot plot_k.gp
