#ifndef INCLUDED_RENDERER_H
#define INCLUDED_RENDERER_H


class Renderer
{
public:
    int     m_fps;

private:
    float   m_nearPlane;
    float   m_farPlane;
	int		m_screenW;
	int		m_screenH;

	float	m_fadedness;			// 1.0 means black screen. 0.0 means not fade out at all.
	float	m_fadeRate;				// +ve means fading out, -ve means fading in
	float	m_fadeDelay;			// Amount of time left to wait before starting fade

private:
	void	RenderBackground();
	void	RenderDash();

	void	RenderFadeOut	();
	void	RenderFrame		();
    void	RenderPaused	();
	
public:
	Renderer				();

	void	Render			();
    void	FpsMeterAdvance	();

	void	SetOpenGLState	() const;

	void	UnsetObjectLighting() const;

	void	SetupFor3D() const;

	void	StartFadeOut();
	void	StartFadeIn(float delay);
};

#endif
