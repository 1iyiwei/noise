#!/bin/tcsh -f

foreach i (kopf_b2048_4x4 adaptive_2dim_0p0044 ppentomino_100x100_10x10_s2 ppentomino_10x10_10x10_s5 ghexomino_subdivision_432x432_s2 lagae_33856)
 ./txt2dat.exe data/${i}_mean.txt data/${i}_variance.txt data/white_2dim_0p0044_mean.txt data/white_2dim_0p0044_variance.txt data/${i}_mean.dat data/${i}_variance.dat
end

wgnuplot plot_kopf.gp
