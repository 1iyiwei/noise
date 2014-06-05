CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# library path
INCLUDE_DIRECTORIES(${RainbowNoise_SOURCE_DIR}/Delaunay ${RainbowNoise_SOURCE_DIR}/Library ${RainbowNoise_SOURCE_DIR}/Lloyd ${RainbowNoise_SOURCE_DIR}/Shuffling ${RainbowNoise_SOURCE_DIR}/Spectrum ${RainbowNoise_SOURCE_DIR}/Theory)

LINK_DIRECTORIES(${RainbowNoise_BINARY_DIR}/Delaunay ${RainbowNoise_BINARY_DIR}/Library ${RainbowNoise_BINARY_DIR}/Lloyd ${RainbowNoise_BINARY_DIR}/Shuffling ${RainbowNoise_BINARY_DIR}/Spectrum ${RainbowNoise_BINARY_DIR}/Theory)

SET(LIBS RainbowNoiseDelaunay RainbowNoiseLibrary RainbowNoiseLloyd RainbowNoiseShuffling RainbowNoiseSpectrum RainbowNoiseTheory)

IF(WIN32)
    SET(LIBS ${LIBS}
        odbc32.lib odbccp32.lib winmm.lib)
ELSE(WIN32)
    MESSAGE(Some MS Windows-specific function exist. Build on other platform would fail)
ENDIF(WIN32)

set(projects

FillPenroseHole
PenroseColoring
PenroseSampling
R1G2B1GridSampling
)

foreach(project ${projects})

PROJECT(${project})
ADD_EXECUTABLE(${project} ${project}.cpp)
TARGET_LINK_LIBRARIES(${project} ${LIBS})

endforeach(project)
