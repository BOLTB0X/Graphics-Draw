#include "Pch.h"
#include "Texture.h"
#include "TextureLoader.h"
// Common
#include "DebugHelper.h"


Texture::Texture() {};

Texture::~Texture() {};


bool Texture::Init(ID3D11Device* device, ID3D11DeviceContext* context, const std::string& path)
{
	return TextureLoader::CreateTextureFromFile(device, context, path, m_srv.GetAddressOf());
} // Init(로드용)


void Texture::PSSetShaderResources(ID3D11DeviceContext* context, UINT slot)
{
	if (m_srv == nullptr)
	{
		DebugHelper::SuccessCheck(false, "Texture::Bind 실패: m_srv 가 nullptr 임");
		return;
	}

	context->PSSetShaderResources(slot, 1, m_srv.GetAddressOf());
} // PSSetShaderResources