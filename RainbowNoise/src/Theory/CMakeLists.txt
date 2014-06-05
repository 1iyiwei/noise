CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(RainbowNoiseTheory)

INCLUDE_DIRECTORIES(${RainbowNoise_SOURCE_DIR}/Library ${RainbowNoise_SOURCE_DIR}/Spectrum)

ADD_LIBRARY(${PROJECT_NAME}
DFT.cpp
DFT.hpp
DiffGenerator.cpp
DiffGenerator.hpp
DiffHistogram.cpp
DiffSampling.cpp
DiffThrowing.cpp
EmptyMineField.cpp
EmptyMineField.hpp
HatDiffGenerator.cpp
HatDiffGenerator.hpp
Histogram.cpp
HybridDiffGenerator.cpp
HybridDiffGenerator.hpp
MineField.cpp
MineField.hpp
NeedleDiffGenerator.cpp
NeedleDiffGenerator.hpp
SampleMineField.cpp
SampleMineField.hpp
)
