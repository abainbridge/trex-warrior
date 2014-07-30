#ifndef INCLUDED_MATH_UTILS_H
#define INCLUDED_MATH_UTILS_H

#include <math.h>
#include <stdlib.h>

class Vector3;
class Vector2;
class Matrix34;
class Plane;

inline float frand(float range = 1.0f) { return range * ((float)rand() / (float)RAND_MAX); }
inline float sfrand(float range = 1.0f) { return (0.5f - (float)rand() / (float)(RAND_MAX)) * range; }

#ifndef M_PI
#define M_PI 3.1415926535897932384626f
#endif

#define sign(a)				((a) < 0 ? -1 : 1) 
#define signf(a)			((a) < 0.0f ? -1.0f : 1.0f) 


#define clamp(a, low, high)	if (a>high) a = high; else if (a<low) a = low;

#define NearlyEquals(a, b)	(fabsf((a) - (b)) < 1e-6 ? 1 : 0)

#define ASSERT_FLOAT_IS_SANE(x) \
	DebugAssert((x + 1.0f) != x);
#define ASSERT_VECTOR3_IS_SANE(v) \
	ASSERT_FLOAT_IS_SANE((v).x) \
	ASSERT_FLOAT_IS_SANE((v).y) \
	ASSERT_FLOAT_IS_SANE((v).z)



// **********************
// General Geometry Utils
// **********************

void GetPlaneMatrix(Vector3 const &t1, Vector3 const &t2, Vector3 const &t3, Matrix34 *mat);
float ProjectPointOntoPlane(Vector3 const &point, Matrix34 const &planeMat, Vector3 *result);
void ConvertWorldSpaceIntoPlaneSpace(Vector3 const &point, Matrix34 const &plane, Vector2 *result);
void ConvertPlaneSpaceIntoWorldSpace(Vector2 const &point, Matrix34 const &plane, Vector3 *result);
float CalcTriArea(Vector2 const &t1, Vector2 const &t2, Vector2 const &t3);


// *********************
// 2D Intersection Tests
// *********************

bool IsPointInTriangle(Vector2 const &pos, Vector2 const &t1, Vector2 const &t2, Vector2 const &t3);
float PointSegDist2D(Vector2 const &p,	// Point
				     Vector2 const &l0, Vector2 const &l1, // Line seg
				     Vector2 *result=NULL);
bool SegRayIntersection2D(Vector2 const &_lineStart, Vector2 const &_lineEnd,
						  Vector2 const &_rayStart, Vector2 const &_rayDir,
                          Vector2 *_result = NULL);

// *********************
// 3D Intersection Tests
// *********************

float RayRayDist(Vector3 const &a, Vector3 const &aDir,
				 Vector3 const &b, Vector3 const &bDir,
				 Vector3 *posOnA=NULL, Vector3 *posOnB=NULL);

bool RayTriIntersection         (Vector3 const &orig, Vector3 const &dir,
						         Vector3 const &vert0, Vector3 const &vert1, Vector3 const &vert2,
						         float _rayLen=1e10, Vector3 *result=NULL);

int RayPlaneIntersection(Vector3 const &pOnLine, Vector3 const &lineDir,
						 Plane const &plane, Vector3 *intersectionPoint=NULL);

bool RaySphereIntersection      (Vector3 const &rayStart, Vector3 const &rayDir, 
	                              Vector3 const &spherePos, float sphereRadius,
			                      float _rayLen=1e10, Vector3 *pos=NULL, Vector3 *normal=NULL);

bool SphereSphereIntersection   (Vector3 const &_sphere1Pos, float _sphere1Radius,
                                  Vector3 const &_sphere2Pos, float _sphere2Radius);

bool SphereTriangleIntersection(Vector3 const &sphereCentre, float sphereRadius,
								Vector3 const &t1, Vector3 const &t2, Vector3 const &t3);

#endif

