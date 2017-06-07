
clear all;

input_file_name = 'siggraph_logo_gradient.jpg';
output_file_name = 'siggraph_logo_gradient.tif';

input = double(imread(input_file_name));

% color inversion
input = 255 - input;

output = rgb2gray(input);

imwrite(output/max(output(:)), output_file_name);
