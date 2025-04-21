/** ResourceManager.h
 */

#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include <filesystem>
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

    //// Resource managers should NOT be copyable
    //ResourceManager(const ResourceManager& other) = delete;
    //ResourceManager(ResourceManager&& other) = delete;
    //ResourceManager& operator=(const ResourceManager& other) = delete;
    ResourceManager() {
    }
    ~ResourceManager() {
    }

    /** Load a resource.
     */
    std::shared_ptr<T> load(const std::string& fileName) {
       // auto& resourceManager = instance();
        std::shared_ptr<T> resource;

        //prepare path
        std::string pPath = mAssetPath;
        pPath += fileName;
        // Check if the resource is already loaded
        auto resIter = resources.find(pPath);
        if (resIter != resources.end()) {
            // The resource was loaded previously. Let's see if we can
            resource = resIter->second.lock();
        }

        if (!resource) {
            // Load the resource from disk
            resource = std::make_shared<T>(pPath);
            resources[pPath] = resource;
        }

        return resource;
    }

    /** Gets the global resource manager instance.
    // */
    //static auto& instance() {
    //    static ResourceManager resourceManager;
    //    return resourceManager;
    //}

    //usually we just have a texture file name, but they're all in a sub folder
    void SetAssetPath(const std::string& path) {
        mAssetPath = path;
    }
    const std::string& GetAssetPath() const { return mAssetPath; }
    std::string mAssetPath;
private:

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