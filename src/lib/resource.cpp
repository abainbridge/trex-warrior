#include "lib/universal_include.h"
#include "resource.h"

#include <GL/gl.h>
#include <stdio.h>

#include "lib/gfx/bitmap.h"
#include "lib/gfx/shape.h"
#include "lib/sound/sound_script.h"
#include "lib/sound/sound_wave.h"
#include "lib/binary_file_reader.h"
#include "lib/debug_utils.h"
#include "lib/filesys_utils.h"

#include "app.h"
#include "level.h"
#include "renderer.h"


Resource g_resourceManager;


Resource::Resource()
:	m_nameSeed(1)
{
}


Resource::~Resource()
{
	FlushOpenGlState();
}


int Resource::CreateDisplayList(char const *name)
{
	// Make sure name isn't NULL and isn't too long
	DebugAssert(name && strlen(name) < 20);

	unsigned int id = glGenLists(1);
	m_displayLists.PushBack(name, id);

	return id;
}


int Resource::GetDisplayList(char const *name)
{
	// Make sure name isn't NULL and isn't too long
	DebugAssert(name && strlen(name) < 20);

	return m_displayLists.GetData(name, -1);
}


void Resource::DeleteDisplayList(char const *name)
{
	if (!name) return;

	// Make sure name isn't too long
	DebugAssert(strlen(name) < 20);

	int id = m_displayLists.GetData(name);
    if (id >= 0)
	{
		glDeleteLists(id, 1);
		m_displayLists.RemoveData(name);
	}
}


int Resource::GetTexture(char const *name, bool masked)
{
	// First lookup this name in the existing textures
	int theTexture = m_textures.GetData(name, -1);

	// If the texture wasn't there, then load it from a file on the disk
	if (theTexture == -1)
	{
		char fullPath[512];
		sprintf(fullPath, "data/%s/%s", "bitmaps", name);
		strlwr(fullPath);

		BitmapRgba bmp;
		ReleaseAssert(bmp.Initialize(fullPath), "Couldn't load bitmap '%s'", fullPath);

		if (masked) 
			bmp.ConvertPinkToTransparent();
		theTexture = bmp.ConvertToTexture(true);
		m_textures.PushBack(name, theTexture);
	}

	if (theTexture == -1)
	{
		char errorString[512];
		sprintf(errorString, "Failed to load texture %s", name);
		ReleaseAssert(false, errorString);
	}

	return theTexture;
}


Shape *Resource::GetShape(char const *name)
{
	Shape *theShape = m_shapes.GetData(name);
	if (!theShape)
	{
		char fullPath[512];

		sprintf(fullPath, "data/shapes/%s", name);
		strlwr(fullPath);
		if (DoesFileExist(fullPath)) 
			theShape = new Shape(fullPath);

		ReleaseAssert(theShape, "Couldn't create shape file %s", name);

		m_shapes.PushBack(name, theShape);
	}

	return theShape;
}


SoundScript *Resource::GetSoundScript(char const *name)
{
	SoundScript *theScript = m_soundScripts.GetData(name);
	if (!theScript)
	{
		char fullPath[512];

		sprintf(fullPath, "data/sounds/%s", name);
		strlwr(fullPath);
		if (DoesFileExist(fullPath)) 
		{
			theScript = new SoundScript;
			theScript->Load(fullPath);
		}

		m_soundScripts.PushBack(name, theScript);
	}

	return theScript;
}


SoundWave *Resource::GetSoundSample(char const *name)
{
    SoundWave *theSample = m_soundSamples.GetData(name);
    if (!theSample)
    {
        char fullPath[512];

        sprintf(fullPath, "data/sounds/%s", name);
        strlwr(fullPath);
        if (DoesFileExist(fullPath)) 
        {
            theSample = new SoundWave;
            theSample->Load(fullPath);
        }

        m_soundSamples.PushBack(name, theSample);
    }

    return theSample;
}


void Resource::FlushOpenGlState()
{
	// Forget all the display lists
	m_displayLists.Empty();

	// Forget all the texture handles
	m_textures.Empty();
}


void Resource::RegenerateOpenGlState()
{
	// Tell the text renderers to reload their font
// 	g_gameFont.BuildOpenGlState();
	
	// Tell all the shapes to generate a new display list
	for (int i = 0; i < m_shapes.Size(); ++i)
	{
		if (!m_shapes.ValidIndex(i)) continue;

		m_shapes[i]->BuildDisplayList();
	}
}


char *Resource::GenerateName()
{
	int digits = log10f(m_nameSeed) + 1;
	char *name = new char [digits + 1];
	itoa(m_nameSeed, name, 10);
	m_nameSeed++;

	return name;
}
