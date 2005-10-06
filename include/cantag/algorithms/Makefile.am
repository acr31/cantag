cantagincludedir = $(includedir)/cantag/algorithms

cantaginclude_HEADERS = DistortionCorrection.hh FitEllipseLS.hh FitEllipseSimple.hh TransformEllipseFull.hh TransformEllipseLinear.hh SampleTagCircle.hh FitQuadTangleCorner.hh FitQuadTangleConvexHull.hh FitQuadTanglePolygon.hh TransformQuadTangleProjective.hh TransformQuadTangleReduced.hh ConvexHull.hh TransformEllipseRotate.hh TransformRotateToPayload.hh DrawTagCircle.hh DrawTagSquare.hh Encode.hh TransformSelectEllipse.hh RemoveNonConcentricEllipse.hh DrawEntity.hh TransformSelectEllipseErrorOfFit.hh FitQuadTangleRegression.hh AccumulateCorrespondances.hh

# disabled acr31 branch
#cantaginclude_HEADERS += ValidateDistortionCorrection.hh ValidateFitEllipse.hh ValidateFitQuadTangle.hh ValidateTransformQuadTangle.hh ValidateDecode.hh

if HAVE_GSL_MULTIMIN_H
if HAVELIB_GSLCBLAS
if HAVELIB_GSL
cantaginclude_HEADERS += TransformQuadTangleSpaceSearch.hh TransformQuadTangleCyberCode.hh
endif
endif
endif
