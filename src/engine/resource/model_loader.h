#pragma once

#include <assimp/Importer.hpp>
#include <optional>

#include "resource.h"

namespace engine {

class ModelLoader {
 public:
  std::optional<ModelData> LoadModel(const ModelLoadParams& params);

 private:
  Assimp::Importer importer_;
};
}  // namespace engine
