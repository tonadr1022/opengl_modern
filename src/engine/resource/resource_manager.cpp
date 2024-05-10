#include "resource_manager.h"

#include "engine/core/base.h"
#include "engine/core/e_assert.h"
#include "engine/renderer/gl/texture_2d.h"
#include "engine/resource/model_loader.h"

namespace engine {

ResourceManager* ResourceManager::instance_{nullptr};
ResourceManager& ResourceManager::Get() { return *instance_; }
ResourceManager::ResourceManager() {
  EASSERT_MSG(instance_ == nullptr, "cannot create two resource managers");
  model_loader_ = std::make_unique<ModelLoader>();
  instance_ = this;
}

ResourceManager::~ResourceManager() = default;

AssetHandle ResourceManager::GetHandle(const std::string& filepath) {
  static auto hasher = std::hash<std::string>{};
  return hasher(filepath);
}

AssetHandle ResourceManager::LoadModel(const std::string& filepath) {
  std::optional<ModelData> model = model_loader_->LoadModel(filepath);
  if (!model.has_value()) {
    spdlog::error("Failed to load model {}", filepath);
    return NullAssetHandle;
  }
  AssetHandle handle{GetHandle(filepath)};
  models_.emplace(handle, std::move(model.value()));
  return handle;
}

AssetHandle ResourceManager::LoadTexture(gfx::Texture2DCreateParams& params) {
  gfx::Texture2D texture{params};
  if (!texture.IsValid()) {
    spdlog::error("Invalid texture{}", params.filepath);
  }
  AssetHandle handle{GetHandle(params.filepath)};
  textures_.emplace(handle, texture);
  return handle;
}

}  // namespace engine
