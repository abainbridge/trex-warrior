#include "lib/universal_include.h"
#include "lib/math_utils.h"

#include <math.h>

#include "lib/matrix34.h"
#include "lib/plane.h"
#include "lib/vector2.h"
#include "lib/vector3.h"


// ****************************************************************************
// General Geometry Utils
// ****************************************************************************

// Generates a Matrix34 that defines a plane, as specified by three points in the plane
void GetPlaneMatrix(Vector3 const &t1, Vector3 const &t2, Vector3 const &t3, Matrix34 *mat)
{
	mat->f = (t1 - t2).Normalize();
	mat->r = t2 - t3;
	mat->u = (mat->f.CrossProduct(mat->r)).Normalize();
	mat->r = mat->f.CrossProduct(mat->u);
	mat->pos = t1;
}


// Returns the distance from the point to the plane and writes the point in the plane into
// result
float ProjectPointOntoPlane(Vector3 const &point, Matrix34 const &planeMat, Vector3 *result)
{
	Vector3 posToCentre = point - planeMat.pos;
	float distFromCentreToPlane = posToCentre * planeMat.u;

	*result = point - planeMat.u * distFromCentreToPlane;
	return distFromCentreToPlane;
}


// Imagine that the plane is a 2D co-ordinate system embedded in a 3D co-ordinate system.
// This function takes a point in 3D space (that must line on the plane) and converts it
// into a 2D point in the "plane's co-ordinate system". If the specified point does not
// lie on the plane in the first place, then the results are undefined.
void ConvertWorldSpaceIntoPlaneSpace(Vector3 const &point, Matrix34 const &plane, Vector2 *result)
{
	Vector3 pointToPlaneOrigin = point - plane.pos;
	result->x = pointToPlaneOrigin * plane.r;
	result->y = pointToPlaneOrigin * plane.f;
}


// The opposite of the function above
void ConvertPlaneSpaceIntoWorldSpace(Vector2 const &point, Matrix34 const &plane, Vector3 *result)
{
	*result = plane.pos + plane.f * point.y + plane.r * point.x;
}


// Returns the area of an arbitrary triangle in 2D space
float CalcTriArea(Vector2 const &t1, Vector2 const &t2, Vector2 const &t3)
{
	return fabsf((t2.x*t1.y-t1.x*t2.y) + (t3.x*t2.y-t2.x*t3.y) + (t1.x*t3.y-t3.x*t1.y)) * 0.5f;
}


// ****************************************************************************
// 2D Intersection Tests
// ****************************************************************************

bool IsPointInTriangle(Vector2 const &pos, Vector2 const &t1, Vector2 const &t2, Vector2 const &t3)
{
	float triArea = CalcTriArea(t1, t2, t3) * 1.0001f;
	float subTrisArea = CalcTriArea(t1, t2, pos);
	if (subTrisArea > triArea) return false;
	subTrisArea += CalcTriArea(t2, t3, pos);
	if (subTrisArea > triArea) return false;
	subTrisArea += CalcTriArea(t3, t1, pos);
	if (subTrisArea > triArea) return false;
	return true;
}


// Finds the point on the line segment that is nearest to the specified point.
// Often this will be one of the end points of the line segment
float PointSegDist2D(Vector2 const &p,	// Point
				     Vector2 const &l0, Vector2 const &l1, // Line seg
				     Vector2 *result)
{
	// Get direction of line
	Vector2 v = l1 - l0;

	// Get vector from start of line to point
	Vector2 w = p - l0;

	// Compute w dot v;
	float c1 = w.x * v.x + w.y * v.y;

	// If c1 <= 0.0f then the end point l0 is the nearest to p
	if (c1 <= 0.0f)
	{
		if (result) *result = l0;
		Vector2 delta = l0 - p;
		return delta.Len();
	}

	// Compute length squared of v, equivalent to v dot v (a dot b = |a| |b| cos theta)
	float c2 = v.LenSquared();
	
	// If c2 <= c1 then the end point l1 is the nearest to p
	if (c2 <= c1)
	{
		if (result) *result = l1;
		Vector2 delta = l1 - p;
		return delta.Len();
	}

	// Otherwise the nearest point is somewhere along the segment
	float b = c1 / c2;
	if (result) *result = l0 + b * v;
	
	Vector2 delta = (l0 + b * v) - p;
	return delta.Len();
}


// Adapted from comp.graphics.algorithms FAQ item 1.03
bool SegRayIntersection2D(Vector2 const &_lineStart, Vector2 const &_lineEnd,
						  Vector2 const &_rayStart, Vector2 const &_rayDir,
                          Vector2 *_result)
{
	float r = (_lineStart.y - _rayStart.y) * _rayDir.x - (_lineStart.x - _rayStart.x) * _rayDir.y;
	r /= (_lineEnd.x - _lineStart.x) * _rayDir.y - (_lineEnd.y - _lineStart.y) * _rayDir.x;

	float s = (_lineStart.y - _rayStart.y) * (_lineEnd.x - _lineStart.x) -
		      (_lineStart.x - _rayStart.x) * (_lineEnd.y - _lineStart.y);
	s /= (_lineEnd.x - _lineStart.x) * _rayDir.y -
		 (_lineEnd.y - _lineStart.y) * _rayDir.x;

	if (r >= 0.0f && r <= 1.0f && s >= 0.0f)
	{
        if (_result)
        {
            *_result = _rayStart + _rayDir * s;
        }

		return true;
	}

	return false;
}


// ****************************************************************************
// 3D Intersection Tests
// ****************************************************************************

// Returns the distance between to infinite 3D lines, assuming that they are skew.
// Stores the points of closest approach in posOnA and posOnB
float RayRayDist(Vector3 const &a, Vector3 const &aDir,
				 Vector3 const &b, Vector3 const &bDir,
				 Vector3 *posOnA, Vector3 *posOnB)
{
    // Connecting line is perpendicular to both
  	Vector3 cDir = aDir.CrossProduct(bDir);
	Vector3 temp1, temp2;

	if (posOnA == NULL) posOnA = &temp1;
	if (posOnB == NULL) posOnB = &temp2;

    // Check for near-parallel lines
//   if (0)   
//	 {
//       return 0.0f;	// degenerate case: lines are parallel
//   }

    // Form plane containing line A, parallel to cdir
    Plane planeA(a, a+aDir, a+cDir);

    // Form plane containing line B, parallel to cdir
    Plane planeB(b, b+bDir, b+cDir);

	// Now we are going to find the points in the two 
	{
		// Where line A intersects planeB is the point of closest approach to line B
		int result1 = RayPlaneIntersection(a, aDir, planeB, posOnA);

		// Where line B intersects planeA is the point of closest approach to line A
		int result2 = RayPlaneIntersection(b, bDir, planeA, posOnB);
	}

    float dist = (*posOnA - *posOnB).Len();
	
	return dist;
}


bool RayTriIntersection(Vector3 const &orig, Vector3 const &dir,
						Vector3 const &vert0, Vector3 const &vert1, Vector3 const &vert2,
						float rayLen, Vector3 *_result)
{
	// Find vectors for two edges sharing vert0
	Vector3 edge1 = vert1 - vert0;
	Vector3 edge2 = vert2 - vert0;

	// Begin calculating determinant - also used to calculate U parameter
	Vector3 pvec = dir.CrossProduct(edge2);

	// If determinant is near zero, ray lies in plane of triangle
	float det = edge1 * pvec;

	if (det > -0.0000001f && det < 0.0000001f)
		return false;
	
	float invDet = 1.0f / det;

	// calculate distance from vert0 to ray origin
	Vector3 tvec = orig - vert0;

	Vector3 result;

	// calculate Y parameter and test bounds
	result.y = (tvec * pvec) * invDet;
	if (result.y < 0.0f || result.y > 1.0f)
		return false;

	// prepare to test Z parameter
	Vector3 qvec = tvec.CrossProduct(edge1);

	// calculate Z parameter and test bounds
	result.z = (dir * qvec) * invDet;
	if (result.z < 0.0f || result.y + result.z > 1.0f)
		return false;

	// calculate X, ray intersects triangle
	result.x = (edge2 * qvec) * invDet;

    if (result.x < 0 || result.x > rayLen)
		return false;

	if (_result)
		*_result = orig + dir * result.x;

	return true;
}


bool RaySphereIntersection(Vector3 const &rayStart, Vector3 const &rayDir, 
                            Vector3 const &spherePos, float sphereRadius,
		                    float _rayLen, Vector3 *pos, Vector3 *normal)
{    
    Vector3 l = spherePos - rayStart;
    float radiusSqrd = sphereRadius * sphereRadius;
    
    if (l.LenSquared() < radiusSqrd)
        return true;    // Ray starts inside sphere

	// Find tca the distance along ray of point nearest to sphere centre.
	// We'll call this point P
	float tca = l * rayDir;
    if (tca < 0.0f)
        return false;
    
	// Use Pythagoras now to find dist from P to sphere centre. Actually
	// cheaper to calc dist sqrd and compare to radius sqrd
    float lMagSqrd = l.LenSquared();
    float d2 = lMagSqrd - (tca * tca);
    if (d2 > radiusSqrd) return false;
       
    float thc = sqrtf(radiusSqrd - d2);
    float t = tca - thc;

    if (t < 0 || t > _rayLen) return false;

    if (pos)
		*pos = rayStart + rayDir * t;

	if (normal)
	{
		*normal = *pos - spherePos;
		normal->Normalize();
	}

    return true;
}


// Returns 0 if line is parallel to plane but doesn't intersect,
//		   1 if line is in plane (ie intersects everywhere),
//		   2 otherwise (this is the general case)
int RayPlaneIntersection(Vector3 const &pOnLine, Vector3 const &lineDir,
						 Plane const &plane, Vector3 *intersectionPoint)
{
	/*If the plane is defined as:
         a*x + b*y + c*z + d = 0
	and the line is defined as:
        x = x1 + (x2 - x1)*t = x1 + i*t
        y = y1 + (y2 - y1)*t = y1 + j*t
        z = z1 + (z2 - z1)*t = z1 + k*t
	Then just substitute these into the plane equation. You end up
    with:
		t = - (a*x1 + b*y1 + c*z1 + d)/(a*i + b*j + c*k)
	When the denominator is zero, the line is contained in the plane
    if the numerator is also zero (the point at t=0 satisfies the
    plane equation), otherwise the line is parallel to the plane.*/

	float a, b, c, d;
	plane.GetCartesianDefinition(&a, &b, &c, &d);
	
	float x1 = pOnLine.x;
	float y1 = pOnLine.y;
	float z1 = pOnLine.z;

	float i = lineDir.x;
	float j = lineDir.y;
	float k = lineDir.z;

	float numerator = - (a*x1 + b*y1 + c*z1 + d);
	float denominator = a*i + b*j + c*k;

	if (fabs(denominator) < 1e-7) 
	{
		if (fabs(numerator) < 1e-7) 
			return 1;
		return 0;
	}

	float t = numerator / denominator;
	intersectionPoint->x = x1 + i * t;
	intersectionPoint->y = y1 + j * t;
	intersectionPoint->z = z1 + k * t;

	return 2;
}


bool SphereSphereIntersection(Vector3 const &_sphere1Pos, float _sphere1Radius,
                               Vector3 const &_sphere2Pos, float _sphere2Radius)
{
    float distanceSqrd = (_sphere1Pos - _sphere2Pos).LenSquared();
    float radiiSummed = _sphere1Radius + _sphere2Radius;
	return(distanceSqrd <= radiiSummed * radiiSummed);
}


bool SphereTriangleIntersection(Vector3 const &sphereCentre, float sphereRadius,
								Vector3 const &t1, Vector3 const &t2, Vector3 const &t3,
                                Vector3 *_result)
{
	Matrix34 planeMat;
	GetPlaneMatrix(t1, t2, t3, &planeMat);
	
	Vector3 result;
	float dist = ProjectPointOntoPlane(sphereCentre, planeMat, &result);
	if (dist > sphereRadius) return false;

	Vector2 point2D;
	ConvertWorldSpaceIntoPlaneSpace(result, planeMat, &point2D);

	Vector2 t12D, t22D, t32D;
	ConvertWorldSpaceIntoPlaneSpace(t1, planeMat, &t12D);
	ConvertWorldSpaceIntoPlaneSpace(t2, planeMat, &t22D);
	ConvertWorldSpaceIntoPlaneSpace(t3, planeMat, &t32D);
	
	bool isPointInTri = IsPointInTriangle(point2D, t12D, t22D, t32D);
	
	if (!isPointInTri)
	{
		// Test against edge 1
		Vector2 pointInTriangle;
		float nearest = PointSegDist2D(point2D, t12D, t22D, &pointInTriangle);
		
		// Test against edge 2
		Vector2 temp;
		float thisDist = PointSegDist2D(point2D, t22D, t32D, &temp);
		if (thisDist < nearest)
		{
			nearest = thisDist;
			pointInTriangle = temp;
		}
		
		// Test against edge 3
		thisDist = PointSegDist2D(point2D, t32D, t12D, &temp);
		if (thisDist < nearest)
		{
			nearest = thisDist;
			pointInTriangle = temp;
		}

		ConvertPlaneSpaceIntoWorldSpace(pointInTriangle, planeMat, &result);
	}

	Vector3 centreToNearestPointInPlane = sphereCentre - result;
	if (centreToNearestPointInPlane.LenSquared() > sphereRadius * sphereRadius)
		return false;

    if (_result)
        *_result = result;

	return true;
}
