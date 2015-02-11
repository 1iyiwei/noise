CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(BlueMathDelaunay)

INCLUDE_DIRECTORIES(${BlueMath_SOURCE_DIR}/Library)

ADD_LIBRARY(${PROJECT_NAME}
 CosCoverage.cpp
 CosCoverage.hpp
 Coverage.cpp
 Coverage.hpp
 Delaunay.cpp
 Delaunay.hpp
 DelaunayMesh.cpp
 DelaunayMesh.hpp
 DelaunayProximity2D.cpp
 DelaunayProximity2D.hpp
 LloydRelaxation.cpp
 LloydRelaxation.hpp
 Math2D.hpp
 Math2D.cpp
 MultiLinearCoverage.cpp
 MultiLinearCoverage.hpp
 Proximity2D.cpp
 Proximity2D.hpp
 QuadEdge.cpp
 QuadEdge.hpp
 VoronoiCoverage.cpp
 VoronoiCoverage.hpp
)
