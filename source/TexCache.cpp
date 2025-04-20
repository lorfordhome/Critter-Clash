#include "TexCache.h"

#include <filesystem>

raylib::Texture2D& TexCache::LoadTexture( const std::string& fileName, const std::string& texName, bool appendPath) {
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
	std::string ws(pPath->begin(), pPath->end());//create wide string with value of ppath

	//load texture
	raylib::Texture2D pTex(ws);
	raylib::Vector2 pSize = pTex.GetSize();
	//save texture
	assert(pTex);
	mCache.insert(MyMap::value_type(name, Data(fileName, pTex, pSize)));
	return pTex;
}

TexCache::Data& TexCache::Get(raylib::Texture2D pTex){
	MyMap::iterator it = mCache.begin();
	Data* p = nullptr;
	while (it != mCache.end() && !p)
	{
		if ((*it).second.pTex.id == pTex.id)//found texture!
			p = &(*it).second;
		++it;
	}
	assert(p);//cant find :(
	return *p;
}


void TexCache::Release() {
	for (auto& pair : mCache)
		UnloadTexture(pair.second.pTex);
	mCache.clear();
}