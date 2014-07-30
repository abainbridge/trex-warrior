#ifndef INCLUDED_APP_H
#define INCLUDED_APP_H

class Camera;
class Level;
class ParticleSystem;
class Renderer;


class App
{
public:
	enum 
	{
		StatePlayLocation,
		StateEditLocation,
		StateGameOver,
		StateQuit
	};

private:
	int m_state;

public:
	// Things that are the world
	Level               *m_level;

	// Everything else
	Camera              *m_camera;
	ParticleSystem		*m_particleSystem;
	Renderer            *m_renderer;

	// State flags
	bool                m_paused;
	int 				m_requestedState;

private:
	void MainLoopCommonStart();
	void Render();
	
	void PlayLocationLoop();
	void EditLocationLoop();
	void GameOverLoop();

public:
	App();
	void RunGame();
};


extern App *g_app;

#endif
