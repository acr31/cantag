#import "CantagWrapper.h"
#include "Cantag.hh"

using namespace Cantag;

template<int RINGS, int SECTORS>
struct CircleInnerOpt : public Cantag::TagCircleInner<RINGS,SECTORS>, public ParityCoder<RINGS*SECTORS> {};
typedef CircleInnerOpt<5,31> CircleTagType;

@implementation CantagWrapper

+ (void)process:(int)width :(int)height :(int)widthStep :(unsigned char *)data :(unsigned char *)debugData {

    try {
        CircleTagType tag;
        
        Camera camera;
        camera.SetIntrinsic(width,width,width/2,height/2,0);
        
        Image<Pix::Sze::Byte4,Pix::Fmt::BGRA32> i(width, height, widthStep, data);
        MonochromeImage m(i.GetWidth(),i.GetHeight());
        Tree<ComposedEntity<TL4(ContourEntity,ShapeEntity<Ellipse>,TransformEntity,DecodeEntity<CircleTagType::PayloadSize>) > > tree;
        
        Apply(i,m,ThresholdGlobal<Pix::Sze::Byte4,Pix::Fmt::BGRA32>(128));
        Apply(m,ContourFollowerClearImageBorder());
        Apply(m,tree,ContourFollowerTree(tag));
        ApplyTree(tree,DistortionCorrectionSimple(camera));
        ApplyTree(tree,FitEllipseLS());
        ApplyTree(tree,TransformEllipseFull(tag.GetBullseyeOuterEdge()));
        ApplyTree(tree,TransformSelectEllipse(tag,camera));
        ApplyTree(tree,RemoveNonConcentricEllipse(tag));
        ApplyTree(tree,Bind(TransformEllipseRotate(tag,camera),m));
        ApplyTree(tree,Bind(SampleTagCircle(tag,camera),m));
        ApplyTree(tree,Decode<CircleTagType>());
        ApplyTree(tree,PrintEntityDecode<CircleTagType::PayloadSize>(std::cout));
        ApplyTree(tree,TransformRotateToPayload(tag));
        
        if (debugData != NULL) {
            Image<Pix::Sze::Byte1,Pix::Fmt::Grey8> o(width, height, width, debugData);
            Apply(i,o,ConvertToGreyscale<Pix::Sze::Byte4,Pix::Fmt::BGRA32>());
            o.ConvertScale(0.25,190);
            //ApplyTree(tree,DrawEntityContour(*o));
            ApplyTree(tree,DrawEntitySample(o,camera,tag));
            ApplyTree(tree,DrawEntityTransform(o,camera));
        }
    }
    catch (const char* exception) {
        std::cerr << "Caught exception: " << exception << std::endl;
    }
}

@end
