#pragma once

#include <assimp/Importer.hpp>
#include <optional>

namespace engine {

struct ModelData;

class ModelLoader {
 public:
  std::optional<ModelData> LoadModel(const std::string& filepath);

 private:
  Assimp::Importer importer_;
};
}  // namespace engine
