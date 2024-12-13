
#pragma once
#include <d3d11.h>
#include <string>
#include <vector>
#include "SimpleMath.h"
#include <unordered_map>
class TexCache {
public:
	struct Data {
		Data(){}
		Data(const std::string& Name, ID3D11ShaderResourceView* p, const DirectX::SimpleMath::Vector2& _dim):fileName(Name),pTex(p),dim(_dim){}
		ID3D11ShaderResourceView* pTex = nullptr;
		std::string fileName;
		DirectX::SimpleMath::Vector2 dim;
	};
	std::vector<Data> Cache;

	//tidy up at the end
	void Release();

	//if this texture is new load it in, otherwise find it and return a handle
	ID3D11ShaderResourceView* LoadTexture(ID3D11Device* pDevice, const std::string& fileName, const std::string& texName = "", bool appendPath = true);

	//usually we just have a texture file name, but they're all in a sub folder
	void SetAssetPath(const std::string& path) {
		mAssetPath = path;
	}
	const std::string& GetAssetPath() const { return mAssetPath; }
	//pull out a texture by nickname = fast
	Data& Get(const std::string& texName) {
		return mCache.at(texName);
	}

	//slowly find a texture by handle
	Data& Get(ID3D11ShaderResourceView* pTex);

private:
	DirectX::SimpleMath::Vector2 GetDimensions(ID3D11ShaderResourceView* pTex);
	typedef std::unordered_map<std::string, Data> MyMap;
	MyMap mCache;
	//data sub folder with all the textures
	std::string mAssetPath;
};
