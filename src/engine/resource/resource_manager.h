#pragma once

#include <memory>

#include "engine/core/base.h"
#include "engine/resource/resource.h"

namespace engine {
namespace gfx {
struct Texture2D;
struct Texture2DCreateParams;
}  // namespace gfx

class ModelLoader;

struct ModelAsset {
  ModelData model;
  std::string filepath;
};

class ResourceManager {
 public:
  ResourceManager& Get();
  ~ResourceManager();

  AssetHandle LoadModel(const std::string& filepath);
  ModelData& GetModel(AssetHandle handle);
  AssetHandle LoadTexture(gfx::Texture2DCreateParams& params);
  engine::gfx::Texture2D& GetTexture(AssetHandle handle);

 private:
  friend class Engine;
  ResourceManager();
  static ResourceManager* instance_;
  std::unique_ptr<ModelLoader> model_loader_;

  AssetHandle GetHandle(const std::string& filepath);

  // std::map<std::string, AssetHandle> file_path_to_id_;
  std::map<AssetHandle, ModelAsset> models_;
  std::map<AssetHandle, engine::gfx::Texture2D> textures_;
  // std::map<AssetID,
};

}  // namespace engine
