# Code #

This folder contains (almost) all source code for my SIGGRAPH 2008 paper Parallel Poisson Disk Sampling.

This is currently research-quality code, so use them at your own risk. I have not yet (and have little incentive) to clean them up.

## Compilation ##

You can compile everything via BlueNoise.sln.

If you only want the simplest CPU code to have a test of how the algorithm works, just compile AdaptiveNDim.vcproj project. This is my main CPU code for adaptive sampling in arbitrary dimensions. The code is not parallel but simulates that.

## Execution ##

Here is an example on how to run it:
AdaptiveNDim 2 0.02 30 0 1.0 uniform 1 1 > data/gjp_poisson_2dim_0p02.txt

This will produce a 2D Poisson disk samples with r = 0.02 (minimum distance between samples), k = 30 (number of trials per grid cell), root level cell size = 1, a uniform importance field, and sampling domain size [1 1] (2 numbers for 2D).

Note that my code is designed for efficiently generating a large number of samples (over several hundreds thousands), so if you only need a few hundred samples this might be an overkill.

