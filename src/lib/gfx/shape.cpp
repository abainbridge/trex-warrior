#include "lib/universal_include.h"
#include "shape.h"

#include <float.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "lib/gfx/debug_render.h"
#include "lib/debug_utils.h"
#include "lib/matrix34.h"
#include "lib/resource.h"
#include "lib/text_file_reader.h"


// This constructor is used to load a shape from a file.
Shape::Shape(char const *filename)
:	m_displayListName(NULL),
	m_numPositions(0),
	m_positions(NULL),
	m_positionsInWS(NULL),
	m_numNormals(0),
	m_normals(NULL),
	m_numColours(0),
	m_colours(NULL),
	m_numTriangles(0),
	m_maxTriangles(0),
	m_triangles(NULL),
	m_centre(0.0f, 0.0f, 0.0f),
	m_radius(-1.0f),
	m_mostPositiveY(0.0f),
	m_mostNegativeY(0.0f)
{
	m_maxTriangles = 1;
	m_triangles = new ShapeTriangle[m_maxTriangles];

	TextFileReader foo(filename);
	TextFileReader *_in = &foo;

	while(_in->ReadLine())
	{
		if (!_in->TokenAvailable()) continue;

		char *firstWord = _in->GetNextToken();
		char *secondWord = _in->GetNextToken();

		if (stricmp(firstWord, "Positions") == 0)
		{
			int numPositions = atoi(secondWord);
			ParsePositionBlock(_in, numPositions);
		}
		else if (stricmp(firstWord, "Colours") == 0)
		{
			int numColours = atoi(secondWord);
			ParseColourBlock(_in, numColours);
		}
		else if (stricmp(firstWord, "Triangles") == 0)
		{
			int numTriangles = atoi(secondWord);
			ParseTriangleBlock(_in, numTriangles);
			break;
		}
	}

	GenerateNormals();

	m_positionsInWS = new Vector3[m_numPositions];
}


Shape::~Shape()
{
    delete [] m_normals;		m_normals = NULL;
    delete [] m_colours;		m_colours = NULL;
    delete [] m_triangles;		m_triangles = NULL;
	g_resourceManager.DeleteDisplayList(m_displayListName);
	delete [] m_displayListName;
	m_displayListName = NULL;
}


void Shape::BuildDisplayList()
{
	delete [] m_displayListName;
	m_displayListName = g_resourceManager.GenerateName();
	int id = g_resourceManager.CreateDisplayList(m_displayListName);
	glNewList(id, GL_COMPILE);
		RenderSlow();
	glEndList();
}


void Shape::ParsePositionBlock(TextFileReader *_in, unsigned int numPositions)
{
	Vector3 *positions = new Vector3[numPositions];

	int expectedId = 0;
	while (expectedId < numPositions)
	{
		if (_in->ReadLine() == 0)
		{
			DebugAssert(0);
		}

		char *c = _in->GetNextToken();
		if (c && isdigit(c[0]))
		{
			int id = atoi(c);
			if (id != expectedId || id >= numPositions)
				return;
			
			Vector3 *vect = &positions[id];
			c = _in->GetNextToken();
			DebugAssert(c);
			vect->x = (float)atof(c);
			c = _in->GetNextToken();
			DebugAssert(c);
			vect->y = (float)atof(c);
			c = _in->GetNextToken();
			DebugAssert(c);
			vect->z = (float)atof(c);

			expectedId++;
		}
	}

	RegisterPositions(positions, numPositions);
}


void Shape::ParseColourBlock(TextFileReader *_in, unsigned int numColours)
{
	m_colours = new RgbaColour[numColours];
	m_numColours = numColours;

	int expectedId = 0;
	while(expectedId < numColours)
	{
		if (_in->ReadLine() == 0)
		{
			DebugAssert(0);
		}

		char *c = _in->GetNextToken();
		if (c && isdigit(c[0]))
		{
			int id = atoi(c);
			if (id != expectedId || id >= numColours)
			{
				DebugAssert(0);
			}
			
			RgbaColour *col = &m_colours[id];
			c = _in->GetNextToken();
			DebugAssert(c);
			col->r = atoi(c);
			c = _in->GetNextToken();
			DebugAssert(c);
			col->g = atoi(c);
			c = _in->GetNextToken();
			DebugAssert(c);
			col->b = atoi(c);
			col->a = 0;

			*col *= 0.85f;
			col->a = 255;

			expectedId++;
		}
	}
}


void Shape::ParseTriangleBlock(TextFileReader *_in, unsigned int numTriangles)
{
	DebugAssert(m_numTriangles == 0 && m_maxTriangles == 1 && m_triangles != NULL);
	delete [] m_triangles;

	m_maxTriangles = numTriangles;
	m_triangles = new ShapeTriangle[numTriangles];

	while (m_numTriangles < numTriangles)
	{
		_in->ReadLine();

        char *c = _in->GetNextToken();
		m_triangles[m_numTriangles].posId1 = atoi(c);
		c = _in->GetNextToken();
		m_triangles[m_numTriangles].posId2 = atoi(c);
		c = _in->GetNextToken();
		m_triangles[m_numTriangles].posId3 = atoi(c);

        c = _in->GetNextToken();
        m_triangles[m_numTriangles].m_colId = atoi(c);

		m_numTriangles++;
	}
}


// Currently this function generates one normal for each face in all the
// strips, rather than one normal per vertex. This is what we need for
// facet shading, rather than smooth (gouraud) shading.
void Shape::GenerateNormals()
{
	m_numNormals = m_numTriangles;
	m_normals = new Vector3[m_numNormals];
	int normId = 0;
	
	for (int j = 0; j < m_numTriangles; ++j)
	{
		ShapeTriangle *tri = &m_triangles[j];
		Vector3 &a = m_positions[tri->posId1];
		Vector3 &b = m_positions[tri->posId2];
		Vector3 &c = m_positions[tri->posId3];
		Vector3 ab = b - a;
		Vector3 bc = c - b;
		m_normals[normId] = ab.CrossProduct(bc);
		m_normals[normId].Normalize();
		normId++;
	}
}


void Shape::RegisterPositions(Vector3 *positions, unsigned int numPositions)
{
    delete [] m_positions;
    m_positions = positions;
    m_numPositions = numPositions;

	// Calculate bounding sphere
	{
		// Find the centre of the fragment
		{
			float minX = FLT_MAX;
			float maxX = -FLT_MAX;
			float minY = FLT_MAX;
			float maxY = -FLT_MAX;
			float minZ = FLT_MAX;
			float maxZ = -FLT_MAX;
			for (int i = 0; i < m_numPositions; ++i)
			{
				if (m_positions[i].x < minX)		minX = m_positions[i].x;
				if (m_positions[i].x > maxX)		maxX = m_positions[i].x;
				if (m_positions[i].y < minY)		minY = m_positions[i].y;
				if (m_positions[i].y > maxY)		maxY = m_positions[i].y;
				if (m_positions[i].z < minZ)		minZ = m_positions[i].z;
				if (m_positions[i].z > maxZ)		maxZ = m_positions[i].z;
			}
			m_centre.x = (maxX + minX) / 2;
			m_centre.y = (maxY + minY) / 2;
			m_centre.z = (maxZ + minZ) / 2;
		}

		// Find the point furthest from the centre
		float radiusSquared = 0.0f;
		for (int i = 0; i < m_numPositions; ++i)
		{
			Vector3 delta = m_centre - m_positions[i];
			float magSquared = delta.LenSquared();
			if (magSquared > radiusSquared)
				radiusSquared = magSquared;
		}
		m_radius = sqrtf(radiusSquared);
	}
}


void Shape::Render(float predictionTime, Matrix34 mat)
{
	glEnable(GL_COLOR_MATERIAL);
	glMatrixMode(GL_MODELVIEW);
	Matrix34 predictedTransform = mat;

	glPushMatrix();
	glMultMatrixf(predictedTransform.ConvertToOpenGLFormat());
	
	int id = -1;
	if (m_displayListName) 
		id = g_resourceManager.GetDisplayList(m_displayListName);
	if (id != -1)
		glCallList(id);
	else
		RenderSlow();

	glPopMatrix();
	glDisable(GL_COLOR_MATERIAL);
}


void Shape::RenderSlow()
{
	glBegin(GL_TRIANGLES);

	int norm = 0;
	for (int i = 0; i < m_numTriangles; i++)
	{
        ShapeTriangle *tri = &m_triangles[i];
		
		glNormal3fv(m_normals[norm].GetData());
		glColor4ubv(m_colours[tri->m_colId].GetData());

		glVertex3fv(m_positions[tri->posId1].GetData());
		glVertex3fv(m_positions[tri->posId2].GetData());
		glVertex3fv(m_positions[tri->posId3].GetData());

		norm++;
	}

	glEnd();
}


bool Shape::RayHit(RayPackage *package, Matrix34 const &transform, bool accurate)
{
	Matrix34 totalMatrix = transform;
	Vector3 centre = totalMatrix * m_centre;

	// First do bounding sphere check
	if (m_radius > 0.0f &&
		RaySphereIntersection(package->m_rayStart, package->m_rayDir,
							  centre, m_radius, package->m_rayLen))
	{
		// Exit early to save loads of work if we don't care about accuracy too much
		if (accurate == false)
			return true;

		// Compute World Space versions of all the vertices
		for (int i = 0; i < m_numPositions; ++i)
			m_positionsInWS[i] = m_positions[i] * totalMatrix;

		// Check each triangle in this fragment for intersection
		for (int j = 0; j < m_numTriangles; ++j)
		{
            ShapeTriangle *tri = m_triangles + j;
			if (RayTriIntersection(package->m_rayStart,
								   package->m_rayDir,
								   m_positionsInWS[tri->posId1],
								   m_positionsInWS[tri->posId2],
								   m_positionsInWS[tri->posId3]))
				return true;
		}
	}

	return false;
}


bool Shape::SphereHit(SpherePackage *package, Matrix34 const &transform, 
                      bool accurate, Vector3 *result)
{
	Matrix34 totalMatrix = transform;
	Vector3 centre = totalMatrix * m_centre;

	if (m_radius > 0.0f &&
		SphereSphereIntersection(package->m_pos, package->m_radius,
                                 centre, m_radius))
	{
		// Exit early to save loads of work if we don't care about accuracy too much
		if (accurate == false)
			return true;
		
		// Compute World Space versions of all the vertices
		for (int i = 0; i < m_numPositions; ++i)
			m_positionsInWS[i] = m_positions[i] * totalMatrix;

        float radius = package->m_radius;

		// Check each triangle in this fragment for intersection
		for (int j = 0; j < m_numTriangles; ++j)
		{
            ShapeTriangle *tri = m_triangles + j;
			if (SphereTriangleIntersection(package->m_pos,
										   radius,
                                           m_positionsInWS[tri->posId1],
                                           m_positionsInWS[tri->posId2],
										   m_positionsInWS[tri->posId3],
                                           result))
            {
                Vector3 toHit = package->m_pos - *result;
                radius = toHit.Len();
            }
		}

        if (radius < package->m_radius)
            return true;
	}

	return false;
}
