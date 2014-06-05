CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(RainbowNoiseSpectrum)

INCLUDE_DIRECTORIES(${RainbowNoise_SOURCE_DIR}/Library)

ADD_LIBRARY(${PROJECT_NAME}

DeformableLFT.cpp
DeformableLFT.hpp
Deformer.cpp
Deformer.hpp
IsotropicDeformer.cpp
IsotropicDeformer.hpp
LFT.cpp
LFT.hpp
LinearLFT.cpp
LinearLFT.hpp
PFT.cpp
PFT.hpp
RandomLFT.cpp
RandomLFT.hpp
SFT.cpp
SFT.hpp
)
