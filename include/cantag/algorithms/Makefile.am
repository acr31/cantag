cantagincludedir = $(includedir)/cantag/algorithms

cantaginclude_HEADERS =  DistortionCorrectionNone.hh
cantaginclude_HEADERS +=  DistortionCorrectionSimple.hh
cantaginclude_HEADERS += FitEllipseLS.hh
cantaginclude_HEADERS += FitEllipseSimple.hh 
cantaginclude_HEADERS += TransformEllipseFull.hh 
cantaginclude_HEADERS += TransformEllipseLinear.hh 
cantaginclude_HEADERS += SampleTagCircle.hh 
cantaginclude_HEADERS += FitQuadTangleCorner.hh 
cantaginclude_HEADERS += FitQuadTangleConvexHull.hh
cantaginclude_HEADERS += FitQuadTanglePolygon.hh
cantaginclude_HEADERS += TransformQuadTangleProjective.hh 
cantaginclude_HEADERS += TransformQuadTangleReduced.hh
cantaginclude_HEADERS += ConvexHull.hh 
cantaginclude_HEADERS += TransformEllipseRotate.hh 
cantaginclude_HEADERS += TransformRotateToPayload.hh 
cantaginclude_HEADERS += DrawTagCircle.hh 
cantaginclude_HEADERS += DrawTagCircleSVG.hh 
cantaginclude_HEADERS += DrawTagSquare.hh 
cantaginclude_HEADERS += Encode.hh 
cantaginclude_HEADERS += TransformSelectEllipse.hh 
cantaginclude_HEADERS += RemoveNonConcentricEllipse.hh 
cantaginclude_HEADERS += DrawEntity.hh 
cantaginclude_HEADERS += PrintEntity.hh 
cantaginclude_HEADERS += TransformSelectEllipseErrorOfFit.hh 
cantaginclude_HEADERS += FitQuadTangleRegression.hh 
cantaginclude_HEADERS += AccumulateCorrespondences.hh
cantaginclude_HEADERS += SimulateMinDistance.hh
cantaginclude_HEADERS += SimulateContour.hh
cantaginclude_HEADERS += SimulateMaxSampleError.hh
cantaginclude_HEADERS += ContourFollowerTree.hh
cantaginclude_HEADERS += ContourFollowerClearImageBorder.hh

if HAVE_GSL_MULTIMIN_H
if HAVELIB_GSLCBLAS
if HAVELIB_GSL
cantaginclude_HEADERS +=  DistortionCorrectionIterative.hh
cantaginclude_HEADERS += TransformQuadTangleSpaceSearch.hh 
cantaginclude_HEADERS += TransformQuadTangleCyberCode.hh
endif
endif
endif
