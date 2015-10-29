#ifndef INCLUDED_EDITOR_H
#define INCLUDED_EDITOR_H

#define EDITOR_ENABLED 1
#if EDITOR_ENABLED == 1

class Editor
{
public:
	bool m_active;
	int	m_previousCameraMode;		// That was set before the editor was enabled

public:
	Editor();

	void Activate();
	void Deactivate();

	void Advance();
	void Render();

	bool Save();
};


extern Editor g_editor;


#endif

#endif