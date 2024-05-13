#include "model_manager.h"

#include "engine/core/base.h"
#include "engine/core/e_assert.h"
#include "engine/resource/model_loader.h"

namespace engine {

ModelManager* ModelManager::instance_{nullptr};
ModelManager& ModelManager::Get() { return *instance_; }
ModelManager::ModelManager() {
  EASSERT_MSG(instance_ == nullptr, "cannot create two resource managers");
  model_loader_ = std::make_unique<ModelLoader>();
  instance_ = this;
}

ModelManager::~ModelManager() = default;

AssetHandle ModelManager::GetHandle(const std::string& filepath) {
  static auto hasher = std::hash<std::string>{};
  return hasher(filepath);
}

AssetHandle ModelManager::LoadModel(const ModelLoadParams& params) {
  std::optional<ModelData> model = model_loader_->LoadModel(params);
  if (!model.has_value()) {
    spdlog::error("Failed to load model {}", params.filepath);
    return NullAssetHandle;
  }
  AssetHandle handle{GetHandle(params.filepath)};
  models_.emplace(handle, std::move(model.value()));
  return handle;
}

ModelData& ModelManager::GetModel(AssetHandle handle) {
  EASSERT_MSG(handle != NullAssetHandle, "null asset");
  auto it = models_.find(handle);
  EASSERT_MSG(it != models_.end(), "model not found");
  return it->second.model;
}

}  // namespace engine
