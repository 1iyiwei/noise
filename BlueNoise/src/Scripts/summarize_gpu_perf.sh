#!/bin/tcsh -f

set clique = 2;

foreach dim (2 3 4 5 6)
foreach k (4 5 6 7 8 9 10)
foreach noise (0)

set output_file_name = gpu_${dim}dim_clique${clique}_k${k}_n${noise}_perf_summary.txt

rm -f ${output_file_name}

foreach i (0 1 2 3 4 5 6 7 8 9)
foreach storage (constant texture)

grep total gpu_${dim}dim_${storage}_clique${clique}_k${k}_n${noise}_perf_${i}.txt >> ${output_file_name}

end
end

summarize_gpu_perf.exe ${output_file_name}

end
end
end
