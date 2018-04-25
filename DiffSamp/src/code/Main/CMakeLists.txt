CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# library path
INCLUDE_DIRECTORIES(${BlueMath_SOURCE_DIR}/Library ${BlueMath_SOURCE_DIR}/Analysis ${BlueMath_SOURCE_DIR}/Synthesis ${BlueMath_SOURCE_DIR}/Delaunay)
LINK_DIRECTORIES(${BlueMath_BINARY_DIR}/Library ${BlueMath_BINARY_DIR}/Analysis ${BlueMath_BINARY_DIR}/Synthesis ${BlueMath_BINARY_DIR}/Delaunay)

SET(LIBS BlueMathAnalysis BlueMathSynthesis BlueMathDelaunay BlueMathLibrary)

IF(WIN32)
    SET(LIBS ${LIBS}
        odbc32.lib odbccp32.lib winmm.lib)
ELSE(WIN32)
    MESSAGE(Some MS Windows-specific function exist. Build on other platform would fail)
ENDIF(WIN32)

set(projects
AdoptSamples
AnisoIsoDiffRho
AnisoIsoDiffXform
AnisotropicDiffRho
AnisotropicDiffXform
AnisotropicWhiteNoise
ApplyAperture
Blueness
CascadeWhiteNoise
ClipSamples
ConvolveSamples
Coverage
CoverageGradient
DartLining
DartThrowing
DiffHistogram
DrawSamples
F2M2PFM
HexagonalGrid
Histogram
IDSamples
Importance2JacobianField
IsotropicDiffRho
IsotropicDiffXform
IsotropicWhiteNoise
Jacobian
JitterSamples
LloydRelaxation2D
MakeBlobs
MakeImportance
Math
Matrix
Nearest2D
NormF2M
NormPFT
PFM2F2M
PFM2PPM
PFM3PPM
PGM2F2M
ProjectSamples
RadialPlot
RandomPickPointsSites
RegularGrid
Rui2F2M
SamplePFT
SampleSelector
ScaleSamples
ShiftSamples
Stub
SyntheticPFT
TileF2M
TileSamples
UniformDiffRho
UniformDiffXform
UniformLpDiffXform
UniformWhiteNoise
Warp2JacobianField
WarpSamples)

foreach(project ${projects})

PROJECT(${project})
ADD_EXECUTABLE(${project} ${project}.cpp)
TARGET_LINK_LIBRARIES(${project} ${LIBS})

endforeach(project)
