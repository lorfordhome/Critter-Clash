//
//#pragma once
//#include <string>
//#include <vector>
//#include <unordered_map>
//#include <cassert>
//#include "raylib-cpp.hpp"
//class TexCache {
//public:
//	struct Data {
//		Data(){}
//		Data(const std::string& Name, Vector2& _dim):fileName(Name),dim(_dim){}
//		std::string fileName;
//		Vector2 dim;
//	};
//	std::vector<Data> Cache;
//
//	//tidy up at the end
//	void Release();
//
//	//if this texture is new load it in, otherwise find it and return a handle
//	raylib::Texture2D& LoadTexture(const std::string& fileName, const std::string& texName = "", bool appendPath = true);
//
//	//usually we just have a texture file name, but they're all in a sub folder
//	void SetAssetPath(const std::string& path) {
//		mAssetPath = path;
//	}
//	const std::string& GetAssetPath() const { return mAssetPath; }
//	//pull out a texture by nickname = fast
//	Data& Get(const std::string& texName) {
//		return mCache.at(texName);
//	}
//
//	//slowly find a texture by texture ID
//	Data& Get(raylib::Texture2D pTex);
//
//private:
//	typedef std::unordered_map<std::string, Data> MyMap;
//	MyMap mCache;
//	//data sub folder with all the textures
//	std::string mAssetPath;
//};
class TexCache {
};
