#ifndef INCLUDE_OPENGL_HELPER_H
#define INCLUDE_OPENGL_HELPER_H


#include <gl/gl.h>


struct GlHelperEnabler
{
	unsigned m_featureId;
	
	GlHelperEnabler(unsigned featureId)
	{
		if (!glIsEnabled(featureId))
		{
			glEnable(featureId);
			m_featureId = featureId;
		}
		else
		{
			m_featureId = 0;
		}
	}

	~GlHelperEnabler() 
	{ 
		if (m_featureId)
			glDisable(m_featureId); 
	}
};


struct GlHelperDisabler
{
	unsigned m_featureId;

	GlHelperDisabler(unsigned featureId)
	{
		if (glIsEnabled(featureId))
		{
			glDisable(featureId);
			m_featureId = featureId;
		}
		else
		{
			m_featureId = 0;
		}
	}

	~GlHelperDisabler() 
	{ 
		if (m_featureId)
			glEnable(m_featureId); 
	}
};


#define GlHelperEnable(a) GlHelperEnabler enabler##a(a);
#define GlHelperDisable(a) GlHelperDisabler disabler##a(a);


#endif
