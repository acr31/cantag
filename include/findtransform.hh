/**
 * $Header$
 */

#ifndef FIND_TRANSFORM_GUARD
#define FIND_TRANSFORM_GUARD

#include <Ellipse.hh>
#include <QuadTangle.hh>
#include <Camera.hh>
#include <vector>


/**
 * Transform the object co-ordinates x0,y0 onto the image points
 * (NPCF co-ordinates) and append them to points
 */
void ApplyTransform(const float transform[16], float x0, float y0, std::vector<float>& points);

/**
 * Transform the object co-ordinates x0,y0,z0 onto the image points
 * (NPCF co-ordinates).
 */
void ApplyTransform(const float transform[16], float x0, float y0, float z0, float* projX, float* projY);

/**
 * Transform the points x0,y0 onto the image points (NPCF
 * co-ordinates).  If x0 and y0 lie on the unit circle centred on the
 * origin then they will be projected to lie on the ellipse.
 */
void ApplyTransform(const float transform[16], float x0, float y0, float* projX, float* projY);


/**
 * Transform the points (given as an array alternating x and y values)
 * to NPCF co-ordinates.  numpoints in the number of points in the
 * array _not_ the number of elements.
 */
void ApplyTransform(const float transform[16], float* points, int numpoints);

/**
 * Transform the points (given as an array alternating x and y values)
 * to NPCF co-ordinates.  numpoints in the number of points in the
 * array _not_ the number of elements.
 */
void ApplyTransform(const float transform[16], std::vector<float>& points);

/**
 * Transform the object co-ordinates (given as an array containing
 * x,y,z valus in repeating sequence) into NPCF co-ordinates.  The
 * array points should have length numpoints*3.  The resulting NPCF
 * co-ordinates will overwrite the x and y values of each input tuple.
 */ 
void ApplyTransform3D(const float transform[16], float* points, int numpoints);


/**
 * Transform the object co-ordinates given as an array of x,y,z values
 * in repeating sequence into Camera co-ordinates 

void ApplyTransformCamera(const float transform[16], float* points, int numpoints);
*/
/**
 * Calculate the normal vector for this transform by computing the
 * camera co-ordinates of (0,0,0),(0,1,0),(1,0,0), converting these to
 * world-coordinates (using the provided camera object), calculating
 * the vectors between these three points and taking the cross
 * product.
 */
void GetNormalVector(const float transform[16], const Camera& cam, float normal[3]);

/**
 * Transform the point (0,0,0) to camera co-ordinates and then world
 * co-ordinates.  The position of the tag lies somewhere along the
 * line that runs from the origin through this point.  If the tag has
 * unit width it will lie on the point so just scale the point by the
 * tag size.
 */
void GetLocation(const float transform[16], float location[3], float tag_size);

#endif//FIND_TRANSFORM_GUARD
