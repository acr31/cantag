/**
 * $Header$
 *
 * $Log$
 * Revision 1.2  2004/02/21 10:05:57  acr31
 * got it working!
 *
 * Revision 1.1  2004/02/20 22:25:56  acr31
 * major reworking of matching algorithms and architecture
 *
 */

#ifndef CIRCLE_TRANSFORM_GUARD
#define CIRCLE_TRANSFORM_GUARD

#include <Ellipse.hh>

void GetTransform(const Ellipse& ellipse, float transform1[16], float transform2[16]);


/**
 * Transform the points x0,y0 onto the image points (NPCF
 * co-ordinates).  If x0 and y0 lie on the unit circle centred on the
 * origin then they will be projected to lie on the ellipse.
 */
void ApplyTransform(const float transform[16], float x0, float y0, float* projX, float* projY);

void ApplyTransform(const float transform[16], float* points, int numpoints);


#endif//CIRCLE_TRANSFORM_GUARD
