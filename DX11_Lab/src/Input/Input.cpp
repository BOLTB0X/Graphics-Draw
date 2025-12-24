////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "Input.h"
#include <cstring>

Input::Input()
{
	memset(m_keys, 0, sizeof(m_keys));
} // Input


Input::Input(const Input& other)
{
	memset(m_keys, 0, sizeof(m_keys));
} // Input


Input::~Input()
{
} // ~Input


void Input::Init()
{
	int i;

	// 키보드의 모든 키를 눌리지 않은 상태로 초기화
	for (i = 0; i < 256; i++)
	{
		m_keys[i] = false;
	}

	return;
} // Init


void Input::KeyDown(unsigned int input)
{
	// 키가 눌리면 해당 상태를 키 배열에 저장
	m_keys[input] = true;
	return;
} // KeyDown


void Input::KeyUp(unsigned int input)
{
	// 키가 떼어지면 해당 상태를 키 배열에서 지움
	m_keys[input] = false;
	return;
} // KeyUp


bool Input::IsKeyDown(unsigned int key)
{
	// 키의 상태를 반환 (눌림/안눌림)
	return m_keys[key];
} // InputDown