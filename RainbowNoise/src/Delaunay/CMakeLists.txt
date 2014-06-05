CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(RainbowNoiseDelaunay)

INCLUDE_DIRECTORIES(${RainbowNoise_SOURCE_DIR}/Library)

ADD_LIBRARY(${PROJECT_NAME}
Delaunay.hpp
Delaunay.cpp
DelaunayMesh.hpp
DelaunayMesh.cpp
LloydRelaxation.hpp
LloydRelaxation.cpp
Math2D.hpp
Math2D.cpp
QuadEdge.hpp
QuadEdge.cpp
)
