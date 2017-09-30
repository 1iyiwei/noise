CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(BlueMathSynthesis)

INCLUDE_DIRECTORIES(${BlueMath_SOURCE_DIR}/Library)

ADD_LIBRARY(${PROJECT_NAME}
AnisotropicConflictChecker.cpp
AnisotropicConflictChecker.hpp
AnisotropicWhiteNoise.cpp
AnisotropicWhiteNoise.hpp
ConflictChecker.cpp
ConflictChecker.hpp
IsotropicConflictChecker.cpp
IsotropicConflictChecker.hpp
IsotropicWhiteNoise.cpp
IsotropicWhiteNoise.hpp
UniformConflictChecker.cpp
UniformConflictChecker.hpp
UniformLpConflictChecker.cpp
UniformLpConflictChecker.hpp
UniformWhiteNoise.cpp
UniformWhiteNoise.hpp
WhiteNoise.cpp
WhiteNoise.hpp
)
