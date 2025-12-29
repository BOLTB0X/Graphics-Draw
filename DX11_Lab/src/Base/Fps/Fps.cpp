///////////////////////////////////////////////////////////////////////////////
// Filename: Fps.cpp
///////////////////////////////////////////////////////////////////////////////
#include "Fps.h"


Fps::Fps()
{
} // Fps


Fps::Fps(const Fps& other)
{
} // Fps


Fps::~Fps()
{
} // ~Fps


void Fps::Init()
{
	m_fps = 0;
	m_count = 0;
	m_startTime = timeGetTime();
	return;
} // init


void Fps::Frame()
{
	m_count++;

	if (timeGetTime() >= (m_startTime + 1000))
	{
		m_fps = m_count;
		m_count = 0;

		m_startTime = timeGetTime();
	}
} // Frame


int Fps::GetFps()
{
	return m_fps;
} // GetFps