#!/bin/tcsh -f

foreach target (\
AdoptSamples \
AnisoIsoDiffRho \
AnisoIsoDiffXform \
AnisotropicDiffRho \
AnisotropicDiffXform \
AnisotropicWhiteNoise \
Blueness \
CascadeWhiteNoise \
ClipSamples \
DartLining \
DartThrowing \
DiffHistogram \
DrawSamples \
F2M2PFM \
HexagonalGrid \
Histogram \
Importance2JacobianField \
IsotropicDiffRho \
IsotropicDiffXform \
IsotropicWhiteNoise \
Jacobian \
MakeImportance \
Matrix \
NormF2M \
NormPFT \
PFM2F2M \
PFM2PPM \
PFM3PPM \
PGM2F2M \
RadialPlot \
RandomPickPointsSites \
RegularGrid \
Rui2F2M \
SamplePFT \
SampleSelector \
ScaleSamples \
ShiftSamples \
Stub \
SyntheticPFT \
TileF2M \
TileSamples \
UniformDiffRho \
UniformDiffXform \
UniformLpDiffXform \
UniformWhiteNoise \
Warp2JacobianField \
WarpSamples \
)

echo PROJECT\(${target}\)

echo ADD_EXECUTABLE\(${target} ${target}.cpp\)

echo TARGET_LINK_LIBRARIES\(${target} \$\{LIBS\}\)

echo

end
