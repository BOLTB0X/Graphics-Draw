////////////////////////////////////////////////////////////////////////////////
// Filename: Zone.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _ZONE_H_
#define _ZONE_H_

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "Base/Input/Input.h"
#include "Base/Position/Position.h"

#include "Graphics/Renderer/D3DRenderer.h"

#include "Graphics/Resources/Model/Model.h"
#include "Graphics/Resources/Light/Light.h"
#include "Graphics/Resources/Texture/TextureManager.h"
#include "Graphics/Shaders/ShaderManager.h"

#include "World/Actors/Terrain/Terrain.h"
#include "World/Actors/Terrain/TerrainCell.h"
#include "World/Actors/Stone/Stone.h"

#include "World/Camera/Camera.h"
#include "World/Camera/Frustum.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: Zone
////////////////////////////////////////////////////////////////////////////////
class Zone
{
public:
	Zone();
	Zone(const Zone&);
	~Zone();

	bool Init(D3DRenderer*, HWND, int, int, float, Model*);
	void Shutdown();
	bool Frame(D3DRenderer*, Input*, ShaderManager*, TextureManager*, float, int);
	bool Render(D3DRenderer*, ShaderManager*, TextureManager*);

private:
	void HandleMovementInput(Input*, float);

private:
	Camera* m_Camera;
	Position* m_Position;
	Terrain* m_Terrain;
	Light* m_Light;
	Frustum* m_Frustum;

	Stone* m_PlayerStone;
	Stone* m_WallStone;
	//bool m_displayUI;
	bool m_wireFrame, m_cellLines, m_heightLocked;
}; // Zone


#endif