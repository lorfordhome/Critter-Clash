#include "TexCache.h"
#include <DDSTextureLoader.h>
#include <string>
#include <filesystem>
#include "D3DUtil.h"
using namespace DirectX;
using namespace DirectX::SimpleMath;


ID3D11ShaderResourceView* TexCache::LoadTexture(ID3D11Device* pDevice, const std::string& fileName, const std::string& texName, bool appendPath) {
	std::string name = texName;
	if (name.empty()) {
		std::filesystem::path p(fileName);
		name = p.stem().string();
	}

	//search cache
	MyMap::iterator it = mCache.find(name);
	if (it != mCache.end())
		return(*it).second.pTex;//the first value is the name - so we want the second value, which is the actual texture

	//prepare path
	const std::string* pPath = &fileName;
	std::string path;
	if (appendPath) {
		path = mAssetPath + fileName;
		pPath = &path;
	}
	std::wstring ws(pPath->begin(), pPath->end());//create wide string with value of ppath

	//load texture
	DDS_ALPHA_MODE alpha;
	ID3D11ShaderResourceView* pTex;
	if (CreateDDSTextureFromFile(pDevice, ws.c_str(), nullptr, &pTex, 0, &alpha) != S_OK) {
		WDBOUT(L"Cannot load ");
		assert(false);
	}

	//save texture
	assert(pTex);
	mCache.insert(MyMap::value_type(name, Data(fileName, pTex, GetDimensions(pTex))));
	return pTex;
}

TexCache::Data& TexCache::Get(ID3D11ShaderResourceView* pTex){
	MyMap::iterator it = mCache.begin();
	Data* p = nullptr;
	while (it != mCache.end() && !p)
	{
		if ((*it).second.pTex == pTex)//found texture!
			p = &(*it).second;
		++it;
	}
	assert(p);//cant find :(
	return *p;
}

Vector2 TexCache::GetDimensions(ID3D11ShaderResourceView* pTex) {
	assert(pTex); //make sure its actually there...
	ID3D11Resource* res = nullptr;
	pTex->GetResource(&res);
	ID3D11Texture2D* texture2d = nullptr;
	HRESULT hr = res->QueryInterface(&texture2d);
	Vector2 dim(0, 0);
	if (SUCCEEDED(hr))
	{
		D3D11_TEXTURE2D_DESC desc;
		texture2d->GetDesc(&desc);
		dim.x = static_cast<float>(desc.Width);
		dim.y = static_cast<float>(desc.Height);
	}
	ReleaseCOM(texture2d);
	ReleaseCOM(res);
	return dim;
}

void TexCache::Release() {
	for (auto& pair : mCache)
		ReleaseCOM(pair.second.pTex);
	mCache.clear();
}