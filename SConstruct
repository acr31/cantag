import sys
from glob import glob
import os
from os.path import join

env=Environment(ENV = os.environ)
env.Append(CPPPATH=[join('/opt','local','include')])
env.Append(CPPPATH=[join('/opt','local','include', 'ImageMagick')])
env.Append(CPPPATH=['include'])
env.Append(LIBS=['gsl', 'gslcblas'])
env.Append(LIBS=['Magick++'])
env.Append(LIBS=['gmp'])
env.Append(CPPDEFINES=['MACOSX'])

env.Append(LIBPATH=[join('/opt', 'local', 'lib')])
env.Append(CCFLAGS=['-O4'])

src_sources = ['Ellipse.cc', 'QuadTangle.cc', 'Camera.cc', 'gaussianelimination.cc', 'Image.cc', 'FileImageSource.cc', 'polysolve.cc', 'Transform.cc', 'MonochromeImage.cc', 'GF4Poly.cc', 'AutoThresholdGlobal.cc' , 'BigInt.cc', 'EstimateTransform.cc', 'FileImageSource.cc']

entity_sources = ['ContourEntity.cc']

alg_sources = ['FitEllipseSimple.cc', 'FitEllipseLS.cc', 'DistortionCorrectionNone.cc','DistortionCorrectionSimple.cc','TransformEllipseLinear.cc','TransformEllipseFull.cc','FitQuadTangleCorner.cc','FitQuadTangleConvexHull.cc','FitQuadTanglePolygon.cc','TransformQuadTangleProjective.cc','TransformQuadTangleReduced.cc','ContourFollowerTree.cc','ContourFollowerClearImageBorder.cc','ThresholdGlobal.cc','ThresholdAdaptive.cc','ConvexHull.cc','RemoveNonConcentricEllipse.cc','DrawEntity.cc','PrintEntity.cc','FitQuadTangleRegression.cc','SimulateMinDistance.cc','SignalStrengthContour.cc','HoughTransform.cc', 'DistortionCorrectionIterative.cc', 'TransformQuadTangleSpaceSearch.cc', 'TransformQuadTangleCyberCode.cc']

sources = \
    [join('src', src) for src in src_sources] + \
    [join('src', 'entities', src) for src in entity_sources] + \
    [join('src', 'algorithms', src) for src in alg_sources]
    

cantag_library = env.SharedLibrary(join('lib', 'Cantag'), sources)

