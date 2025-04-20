/** ResourceManager.h
 */

#pragma once

#include <memory>
#include <unordered_map>

#include "raylib-cpp.hpp"

 /** Template class for managing the loading and shared use of resources (e.g., textures).
  *
  * It can load any resource whose class has a constructor of the form
  * Constructor(const std::string &filename).
  *
  * See the predefined resource managers below (e.g., TextureManager).
  *
  * NOTES:
  * - This class is NOT thread safe. It could be made thread-safe using a mutex, and locking
  * it during any function that modifies the data.
  * - This class doesn't trim the resources table when resources have been freed
  * - The performance depends on the STL implementation
  */
template <class T>
class ResourceManager {
public:

    // Resource managers should NOT be copyable
    ResourceManager(const ResourceManager& other) = delete;
    ResourceManager(ResourceManager&& other) = delete;
    ResourceManager& operator=(const ResourceManager& other) = delete;

    ~ResourceManager() {
    }

    /** Load a resource.
     */
    static std::shared_ptr<T> load(const std::string& filename) {
        auto& resourceManager = instance();
        std::shared_ptr<T> resource;

        //prepare path
        const std::string* pPath = &fileName;
        std::string path;
        if (appendPath) {
            path = mAssetPath + fileName;
            pPath = &path;
        }
        std::string ws(pPath->begin(), pPath->end());//create wide string with value of ppath

        // Check if the resource is already loaded
        auto resIter = resourceManager.resources.find(ws);
        if (resIter != resourceManager.resources.end()) {
            // The resource was loaded previously. Let's see if we can
            resource = resIter->second.lock();
        }

        if (!resource) {
            // Load the resource from disk
            resource = std::make_shared<T>(ws);
            resourceManager.resources[ws] = resource;
        }

        return resource;
    }

    /** Gets the global resource manager instance.
     */
    static auto& instance() {
        static ResourceManager resourceManager;
        return resourceManager;
    }

    //usually we just have a texture file name, but they're all in a sub folder
    void SetAssetPath(const std::string& path) {
        mAssetPath = path;
    }
    const std::string& GetAssetPath() const { return mAssetPath; }
private:
    std::string mAssetPath;
    /** The constructor is private so that only the single instance can be used
     */
    ResourceManager() {
    }

    /** The map of resources that have been loaded.
     */
    std::unordered_map<std::string, std::weak_ptr<T>> resources;
};


// Predefined resource managers.

/** Manages loading of textures.
 */
typedef ResourceManager<raylib::Texture> TextureManager;

/** Manages loading of sounds.
 */
typedef ResourceManager<raylib::Sound> SoundManager;

/** Manages loading of music.
 */
typedef ResourceManager<raylib::Music> MusicManager;