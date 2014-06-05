CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
 
PROJECT(RainbowNoiseScripts)

#include_directories(${CMAKE_SOURCE_DIR}/automation/custom)
INCLUDE(${PROJECT_SOURCE_DIR}/../../automation/custom/CopyScripts.cmake)

# List all source files
SET(SOURCE_FILES
demo.sh
discrete.sh
do.sh
draw_samples.sh
fig2eps
gpusamp.sh
hku.sh
lloyd.sh
logress.sh
loper.sh
math.sh
other.sh
paper.sh
plot_2class_probability.sh
plot_blob_adaptive.sh
plot_class.sh
plot_green.sh
plot_histogram.sh
plot_hybrid_diff.sh
plot_linear_partial.sh
plot_minspacing.sh
plot_nonuniform_2class.sh
plot_optimality.sh
plot_ostro_adaptive.sh
plot_penrose.sh
plot_priority.sh
plot_shuffle_class.sh
plot_shuffle_nonuniform_2class.sh
plot_shuffle_priority.sh
plot_shuffle_total.sh
plot_total.sh
regress.sh
shgress.sh
shper.sh
shuffle.sh
spectrum.sh
spectrum_paper.sh
spectrum_regression.sh
stipple.sh
talk.sh
)

ADD_CUSTOM_TARGET(${PROJECT_NAME} ALL)
COPY_FILES_INTO_DIRECTORY_IF_CHANGED(${PROJECT_SOURCE_DIR} "${SOURCE_FILES}" ${EXECUTABLE_OUTPUT_PATH} ${PROJECT_NAME})

#MESSAGE("${PROJECT_NAME} ${PROJECT_BINARY_DIR} ${CMAKE_BINARY_DIR} ${EXECUTABLE_OUTPUT_PATH}")

#COPY_DIRECTORY_IF_CHANGED(. ${Example_BINARY_DIR} ${PROJECT_NAME})

set(projects normalize; normalize_hist; siblingr; txt1dat; txt2dat)

foreach(project ${projects})

PROJECT(${project})
ADD_EXECUTABLE(${project} ${project}.cpp)

endforeach(project)
