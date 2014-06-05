CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(RainbowNoiseLloyd)

INCLUDE_DIRECTORIES(${RainbowNoise_SOURCE_DIR}/Library ${RainbowNoise_SOURCE_DIR}/Shuffling)

ADD_LIBRARY(${PROJECT_NAME}
BruteDiscreteBalzerVoronoi.cpp
BruteDiscreteBalzerVoronoi.hpp
BruteDiscreteLloydCentroid.cpp
BruteDiscreteLloydCentroid.hpp
BruteDiscreteLloydVoronoi.cpp
BruteDiscreteLloydVoronoi.hpp
ComboLloydIndex.cpp
ComboLloydIndex.hpp
DiscreteLloyd.cpp
DiscreteLloyd.hpp
DiscreteLloydCentroid.cpp
DiscreteLloydCentroid.hpp
DiscreteLloydVoronoi.cpp
DiscreteLloydVoronoi.hpp
Lloyd.cpp
Lloyd.hpp
LloydIndex.cpp
LloydIndex.hpp
LloydSample.cpp
LloydSample.hpp
LloydSite.cpp
LloydSite.hpp
LloydTools.cpp
LloydTools.hpp
SoloLloydIndex.cpp
SoloLloydIndex.hpp
VoronoiFairClassWeight.cpp
VoronoiFairClassWeight.hpp
)
