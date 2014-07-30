#ifndef INCLUDED_SHAPE_H
#define INCLUDED_SHAPE_H

#include <stdio.h>

#include "lib/gfx/rgb_colour.h"
#include "lib/matrix34.h"
#include "lib/vector3.h"


class TextFileReader;
class Shape;
class Matrix34;
class Shape;


// ****************
// Class RayPackage
// ****************

class RayPackage
{
public:
	Vector3 m_rayStart;
	Vector3 m_rayEnd;
	Vector3 m_rayDir;
    float   m_rayLen;

	RayPackage(Vector3 const &_start, Vector3 const &_dir, float _length = 1e10)
	:	m_rayStart(_start),
		m_rayDir(_dir)
	{
        m_rayLen = _length;
		m_rayEnd = m_rayStart + m_rayDir * _length;
	}
};


// *******************
// Class SpherePackage
// *******************

class SpherePackage
{
public:
    Vector3 m_pos;
    float   m_radius;

    SpherePackage(Vector3 const &_pos, float _radius)
    :   m_pos(_pos),
        m_radius(_radius)
    {
    }
};


// ******************
// Class VertexPosCol
// ******************

class VertexPosCol
{
public:
	unsigned short m_posId;
	unsigned short m_colId;
};


// *******************
// Class ShapeTriangle
// *******************

class ShapeTriangle
{
public:
	unsigned short v1, v2, v3;	// Vertex indices (into ShapeFragment::m_vertices)
};


// *******************
// Class ShapeFragment
// *******************

class Shape
{
protected:
	char			*m_displayListName;

	void ParsePositionBlock		(TextFileReader *in, unsigned int numPositions);
	void ParseColourBlock		(TextFileReader *in, unsigned int numColours);
	void ParseVertexBlock		(TextFileReader *in, unsigned int numVerts);
	void ParseTriangleBlock		(TextFileReader *in, unsigned int numTriangles);

	void RegisterPositions		(Vector3 *positions, unsigned int numPositions);

	void GenerateNormals		();

public:
	unsigned int	m_numPositions;
	Vector3			*m_positions;
	Vector3			*m_positionsInWS;	// Temp storage space used to cache World Space versions of all the vertex positions in the hit check routines
	unsigned int	m_numNormals;
	Vector3			*m_normals;
	unsigned int	m_numColours;
	RgbaColour		*m_colours;
	unsigned int	m_numVertices;		// Each element contains an index into m_positions and an index into m_colours
	VertexPosCol	*m_vertices;
	unsigned int	m_numTriangles;
	unsigned int	m_maxTriangles;
	ShapeTriangle	*m_triangles;

    Vector3			m_centre;
	float			m_radius;
	float			m_mostPositiveY;
	float			m_mostNegativeY;

    Shape						(char const *filename);
    ~Shape						();

	void BuildDisplayList		();

	void Render					(float predictionTime, Matrix34 mat);
	void RenderSlow				();

	bool RayHit					(RayPackage *package, Matrix34 const &transform, bool accurate = false);
    bool SphereHit              (SpherePackage *package, Matrix34 const &transform, bool accurate = false);
//     bool ShapeHit               (Shape *_shape, Matrix34 const &_theTransform,              // Transform of _shape
//                                                 Matrix34 const &_ourTransform,              // Transform of this
// 								 bool _accurate = false);
};


#endif
