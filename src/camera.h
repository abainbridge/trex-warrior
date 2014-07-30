#ifndef INCLUDED_CAMERA_H
#define INCLUDED_CAMERA_H

#include "lib/vector3.h"

class Camera
{
private:
	float m_targetFov;
	int	m_mode;

private:
	void AdvanceDebugMode();
	void AdvancePlayerShipMode();

public:
    enum Mode
    {
		ModeDebug,
		ModePlayerShip,
		ModeNumModes
    };

	Vector3 m_pos;
	Vector3 m_front;
	Vector3 m_up;
    float   m_fov;
    Vector3 m_vel;
	float	m_shakeAmount;

public:
	Camera();
	~Camera();

	Vector3 GetRight() { return m_up.CrossProduct(m_front); }
	static char const *GetModeName(int _mode);
	int GetMode() { return m_mode; }
	void SetMode(int mode);

	void SetupModelviewMatrix();

	bool PosInViewFrustum(Vector3 const &_pos);
    bool SphereInViewFrustum(Vector3 const &_centre, float _radius );
                                                                              
	void Advance();

    void CreateShake(float _intensity);

	// SetTarget() only sets the target data. To make these changes take effect, call either
	// CutToTarget() or RequestMode(Camera::ModeMoveToTarget), depending on whether you 
	// want an instant cut or a smooth transition
	void SetTarget(Vector3 const &_pos, Vector3 const &_front, Vector3 const &_up=g_upVector);
    void SetFov(float _fov);

    void GetClickRay(Vector3 *_rayStart, Vector3 *_rayDir);
    void Get2dScreenPos(Vector3 const &_vector, float *_screenX, float *_screenY);

// 	void PlayAnimation	(CameraAnimation *_anim);
// 	void StopAnimation	();
// 	bool IsAnimPlaying	();
};


#endif
