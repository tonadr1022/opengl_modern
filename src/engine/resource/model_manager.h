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
struct ModelLoadParams;

struct ModelAsset {
  ModelData model;
  std::string filepath;
};

class ModelManager {
 public:
  static ModelManager& Get();
  ~ModelManager();

  [[nodiscard]] AssetHandle LoadModel(const std::string& filepath);
  [[nodiscard]] ModelData& GetModel(AssetHandle handle);

 private:
  friend class Engine;
  ModelManager();
  static ModelManager* instance_;
  std::unique_ptr<ModelLoader> model_loader_;
  AssetHandle GetHandle(const std::string& filepath);
  std::map<AssetHandle, ModelAsset> models_;
};

}  // namespace engine
