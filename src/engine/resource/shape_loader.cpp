#include "engine/resource/shape_loader.h"

#include "engine/renderer/renderer.h"
#include "engine/resource/data_types.h"
#include "engine/resource/material_manager.h"

namespace engine {

Mesh engine::ShapeLoader::LoadSphere() {
  std::vector<glm::vec3> positions;
  std::vector<glm::vec2> uv;
  std::vector<glm::vec3> normals;
  std::vector<unsigned int> indices;

  const unsigned int x_segments = 64;
  const unsigned int y_segments = 64;
  const float pi = 3.14159265359f;
  for (unsigned int x = 0; x <= x_segments; ++x) {
    for (unsigned int y = 0; y <= y_segments; ++y) {
      float x_segment = static_cast<float>(x) / static_cast<float>(x_segments);
      float y_segment = static_cast<float>(y) / static_cast<float>(y_segments);
      float x_pos = std::cos(x_segment * 2.0f * pi) * std::sin(y_segment * pi);
      float y_pos = std::cos(y_segment * pi);
      float z_pos = std::sin(x_segment * 2.0f * pi) * std::sin(y_segment * pi);
      positions.emplace_back(x_pos, y_pos, z_pos);
      uv.emplace_back(x_segment, y_segment);
      normals.emplace_back(x_pos, y_pos, z_pos);
    }
  }

  bool odd_row = false;
  for (unsigned int y = 0; y < y_segments; ++y) {
    if (!odd_row)  // even rows: y == 0, y == 2; and so on
    {
      for (unsigned int x = 0; x <= x_segments; ++x) {
        indices.push_back(y * (x_segments + 1) + x);
        indices.push_back((y + 1) * (x_segments + 1) + x);
      }
    } else {
      for (int x = x_segments; x >= 0; --x) {
        indices.push_back((y + 1) * (x_segments + 1) + x);
        indices.push_back(y * (x_segments + 1) + x);
      }
    }
    odd_row = !odd_row;
  }

  std::vector<gfx::Vertex> vertices;
  vertices.reserve(positions.size());
  gfx::Vertex v;
  for (unsigned int i = 0; i < positions.size(); ++i) {
    v.position = positions[i];
    // if (normals.size() > 0) {
    v.normal = normals[i];
    // }
    // if (uv.size() > 0) {
    v.tex_coords = uv[i];
    // }
    vertices.emplace_back(v);
  }

  AssetHandle mesh_handle = gfx::Renderer::Get().AddBatchedMesh(vertices, indices);
  Mesh m{.mesh_handle = mesh_handle,
         .material_handle = MaterialManager::Get().GetDefaultMaterialHandle()};
  return m;
}
}  // namespace engine
