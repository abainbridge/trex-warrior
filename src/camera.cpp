#include "lib/universal_include.h"

#include <math.h>
#include <string.h>
#include <float.h>

#include "lib/gfx/debug_render.h"
#include "lib/debug_utils.h"
#include "lib/hi_res_time.h"
#include "lib/input.h"
#include "lib/window_manager.h"
#include "lib/math_utils.h"
#include "lib/matrix33.h"
#include "lib/profiler.h"

#include "app.h"
#include "camera.h"
#include "level.h"
#include "renderer.h"
#include "ship.h"


#define MIN_GROUND_CLEARANCE	10.0f	// Minimum height relative to land
#define MIN_HEIGHT				10.0f	// Height above sea level (which is y=0)
#define MAX_HEIGHT				5000.0f // Height above sea level (which is y=0)


// ***************
// Private Methods
// ***************

void Camera::AdvanceDebugMode()
{
    m_targetFov = 60.0f;
        
    float speed = 60.0f;
	if (g_inputManager->m_mmb)
		speed *= 10.0f;
	if (g_keys[KEY_SHIFT])
		speed /= 10.0f;

	Vector3 right = m_front.CrossProduct(m_up);

    Vector3 move;
    if (g_keys[KEY_A]) move -= right;
	if (g_keys[KEY_D]) move += right;
	if (g_keys[KEY_Q]) move -= m_up;
	if (g_keys[KEY_E]) move += m_up;
	if (g_keys[KEY_W]) move += m_front;
	if (g_keys[KEY_S]) move -= m_front;
    m_pos += g_advanceTime * speed * move;

	int mx = g_inputManager->m_mouseVelX;
	int my = g_inputManager->m_mouseVelY;

    if (g_inputManager->m_mmb)
    {
        Matrix33 mat(1);
		mat.RotateAroundY((float)mx * -0.005f);
		m_up = m_up * mat;
		m_front = m_front * mat;

        Vector3 right = GetRight();
		mat.SetToIdentity();
		mat.FastRotateAround(right, (float)my * -0.005f);
		m_up = m_up * mat;
		m_front = m_front * mat;
    }
}


void Camera::AdvancePlayerShipMode()
{
	Ship *ship = (Ship*)g_app->m_level->m_playerShip;
	m_front = ship->m_front;
	m_up.Set(0, 1, 0);
	m_pos = ship->m_pos;
	m_pos.y += 6.0f;
	m_fov = 55.0f;
}


void Camera::Get2dScreenPos(Vector3 const &_vector, float *_screenX, float *_screenY)
{
    double outX, outY, outZ;

    int viewport[4];
    double viewMatrix[16];
    double projMatrix[16];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, viewMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
    gluProject(_vector.x, _vector.y, _vector.z, 
        viewMatrix,
        projMatrix,
        viewport,
        &outX,
        &outY,
        &outZ);

    *_screenX = outX;
	*_screenY = outY;
}


void Camera::SetFov(float _fov)
{
    m_targetFov = _fov;
}


// **************
// Public Methods
// **************

Camera::Camera()
:   m_fov(60.0f),
    m_targetFov(60.0f),
    m_vel(0,0,0),
    m_mode(ModePlayerShip),
    m_shakeAmount(0.0f)
{
    m_pos = Vector3(0.0f,
					10.0f,
					50.0f);
  
    m_front.Set(0, -0.1f, -1);
    m_front.Normalize();
    
    m_up = g_upVector;
    Vector3 right = m_up.CrossProduct(m_front);
    right.Normalize();

    m_up = m_front.CrossProduct(right);
    m_up.Normalize();
}


void Camera::CreateShake(float _intensity)
{
    m_shakeAmount = max(m_shakeAmount, _intensity);
}


void Camera::SetMode(int mode)
{
	m_mode = mode;

	if (m_mode == ModeDebug)
	{
		m_pos.Set(-550, 400, 0);
		
        m_front.Set(1, -1, 0);
        m_front.Normalize();
		
        Vector3 right = m_front.CrossProduct(g_upVector);
        m_up = right.CrossProduct(m_front);
        m_up.Normalize();
	}
}


void Camera::SetupModelviewMatrix()
{
	float dot = m_front * m_up;
	DebugAssert(NearlyEquals(m_front.LenSquared(), 1.0f));
	DebugAssert(NearlyEquals(m_up.LenSquared(), 1.0f));
	DebugAssert(NearlyEquals(dot, 0.0f));

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	float magOfPos = m_pos.Len();
	Vector3 front = m_front * magOfPos;
	Vector3 up = m_up * magOfPos;
	Vector3 forwards = m_pos + front;
	gluLookAt(m_pos.x, m_pos.y, m_pos.z,
              forwards.x, forwards.y, forwards.z,
              up.x, up.y, up.z);
}

 
bool Camera::PosInViewFrustum(Vector3 const &_pos)
{
	Vector3 dirToPos = (_pos - m_pos).Normalize();
	float angle = dirToPos * m_front;
    float tolerance = 0.2f;
	if (angle < m_fov - tolerance)
		return false;
	return true;
}


void Camera::Advance()
{
    START_PROFILE(g_app->m_profiler, "Advance Camera");
    
	if (g_keyDeltas[KEY_TAB])
	{
		int newMode = m_mode + 1;
		newMode %= ModeNumModes;
		SetMode(newMode);
	}

	switch (m_mode)
	{
	case ModePlayerShip:
		AdvancePlayerShipMode();
		break;
	default:
		AdvanceDebugMode();
	}
    
	if (m_shakeAmount > 0.0f)
	{
		m_front.RotateAroundY( sfrand(m_shakeAmount * 0.2f) );
		Vector3 up = g_upVector;
		up.RotateAround( m_front * sfrand(m_shakeAmount) * 0.3f );
		Vector3 right = m_front.CrossProduct(up);
		right.Normalize();
		m_front.Normalize();
		m_up = right.CrossProduct(m_front);
		m_shakeAmount -= g_advanceTime;
	}

	// Re-normalize
	m_front.Normalize();
	Vector3 right = m_up.CrossProduct(m_front);
	right.Normalize();
	m_up = m_front.CrossProduct(right);
	m_up.Normalize();

	ASSERT_VECTOR3_IS_SANE(m_pos);
	float dot = m_front * m_up;
	DebugAssert(NearlyEquals(dot, 0.0f));

    END_PROFILE(g_app->m_profiler, "Advance Camera");
}


void Camera::GetClickRay(Vector3 *rayStart, Vector3 *rayDir)
{
	int x = g_inputManager->m_mouseX;
	int y = g_inputManager->m_mouseY;

    GLint viewport[4];
    GLdouble mvMatrix[16], projMatrix[16];
    GLdouble objx, objy, objz;

    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, mvMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
    int realY = viewport[3] - y - 1;
 
    gluUnProject(x, realY, 0.0f,
        mvMatrix,
        projMatrix,
        viewport,
        &objx,
        &objy,
        &objz);
    rayStart->Set(objx, objy, objz);

    gluUnProject(x, realY, 1.0f,
        mvMatrix,
        projMatrix,
        viewport,
        &objx,
        &objy,
        &objz);
    rayDir->Set(objx, objy, objz);
    *rayDir -= *rayStart;
    rayDir->Normalize();
}
