CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
 
PROJECT(BluemathScripts)

#include_directories(${CMAKE_SOURCE_DIR}/automation/custom)
INCLUDE(${PROJECT_SOURCE_DIR}/../../automation/custom/CopyScripts.cmake)

# List all source files
SET(SOURCE_FILES
    aniso.sh
    demo.sh
    do.sh
    fig2eps
    geom.sh
    green.sh
    lp.sh
    mosaic.sh
    paper.sh
    regress.sh
    retina.sh
    surface.sh
    test.sh
    transfer.sh
    )

ADD_CUSTOM_TARGET(${PROJECT_NAME} ALL)
COPY_FILES_INTO_DIRECTORY_IF_CHANGED(${PROJECT_SOURCE_DIR} "${SOURCE_FILES}" ${EXECUTABLE_OUTPUT_PATH} ${PROJECT_NAME})

#MESSAGE("${PROJECT_NAME} ${PROJECT_BINARY_DIR} ${CMAKE_BINARY_DIR} ${EXECUTABLE_OUTPUT_PATH}")

#COPY_DIRECTORY_IF_CHANGED(. ${Example_BINARY_DIR} ${PROJECT_NAME})

set(projects behead;txt1dat;txt2dat)

foreach(project ${projects})

PROJECT(${project})
ADD_EXECUTABLE(${project} ${project}.cpp)

endforeach(project)
