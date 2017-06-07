
#!/bin/tcsh -f

set clique = 2;

foreach i (0 1 2 3 4 5 6 7 8 9)
foreach noise (0 1)
foreach k (4 5 6 7 8 9 10)

(BlueGPU 2 0.0007 ${k} 4 1 ${clique} 0 0 ${noise} 0 1 1 > gpu_2dim_constant_clique${clique}_k${k}_n${noise}_samples_${i}.txt) >& gpu_2dim_constant_clique${clique}_k${k}_n${noise}_perf_${i}.txt

(BlueGPU 2 0.0007 ${k} 4 1 ${clique} 0 1 ${noise} 0 1 1 > gpu_2dim_texture_clique${clique}_k${k}_n${noise}_samples_${i}.txt) >& gpu_2dim_texture_clique${clique}_k${k}_n${noise}_perf_${i}.txt

(BlueGPU 3 0.055 ${k} 3 1 ${clique} 0 0 ${noise} 0 1 2 64 > gpu_3dim_constant_clique${clique}_k${k}_n${noise}_samples_${i}.txt) >& gpu_3dim_constant_clique${clique}_k${k}_n${noise}_perf_${i}.txt

(BlueGPU 3 0.055 ${k} 3 1 ${clique} 0 1 ${noise} 0 1 2 64 > gpu_3dim_texture_clique${clique}_k${k}_n${noise}_samples_${i}.txt) >& gpu_3dim_texture_clique${clique}_k${k}_n${noise}_perf_${i}.txt

(BlueGPU 4 0.0625 ${k} 2 1 ${clique} 0 0 ${noise} 0 1 2 1 2 > gpu_4dim_constant_clique${clique}_k${k}_n${noise}_samples_${i}.txt) >& gpu_4dim_constant_clique${clique}_k${k}_n${noise}_perf_${i}.txt

(BlueGPU 4 0.0625 ${k} 2 1 ${clique} 0 1 ${noise} 0 1 2 1 2 > gpu_4dim_texture_clique${clique}_k${k}_n${noise}_samples_${i}.txt) >& gpu_4dim_texture_clique${clique}_k${k}_n${noise}_perf_${i}.txt

(BlueGPU 5 0.28 ${k} 2 1 ${clique} 0 1 ${noise} 0 1 2 2 8 4 > gpu_5dim_texture_clique${clique}_k${k}_n${noise}_samples_${i}.txt) >& gpu_5dim_texture_clique${clique}_k${k}_n${noise}_perf_${i}.txt

(BlueGPU 6 0.307 ${k} 2 1 ${clique} 0 1 ${noise} 0 1 2 2 1 2 2 > gpu_6dim_texture_clique${clique}_k${k}_n${noise}_samples_${i}.txt) >& gpu_6dim_texture_clique${clique}_k${k}_n${noise}_perf_${i}.txt

end
end
end
