
#include "mesh_manager.h"

#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>
#include <entt/core/hashed_string.hpp>
#include <entt/entt.hpp>

#include "engine/renderer/renderer.h"
#include "engine/resource/data_types.h"
#include "engine/resource/shapes/shapes.h"

namespace engine {

namespace {

glm::vec3 aiVec3ToGLM(const aiVector3f& vec) { return {vec.x, vec.y, vec.z}; }
glm::vec2 aiVec2ToGLM(const aiVector3D& vec) { return {vec.x, vec.y}; }

MeshID ProcessMesh(const aiScene& scene, const aiMesh& mesh, std::vector<gfx::Vertex>& vertices,
                   std::vector<gfx::Index>& indices) {
  EASSERT_MSG(mesh.HasPositions() && mesh.HasNormals() && mesh.HasTextureCoords(0),
              "Mesh needs positions, normals, and texture coords");
  vertices.reserve(mesh.mNumVertices);

  gfx::Vertex v;
  // process vertices
  for (uint32_t i = 0; i < mesh.mNumVertices; i++) {
    v.position = aiVec3ToGLM(mesh.mVertices[i]);
    v.normal = aiVec3ToGLM(mesh.mNormals[i]);
    v.tex_coords = aiVec2ToGLM(mesh.mTextureCoords[0][i]);
  }

  // process indices
  for (uint32_t i = 0; i < mesh.mNumFaces; i++) {
    for (uint32_t j = 0; j < mesh.mFaces[i].mNumIndices; j++) {
      indices.push_back(mesh.mFaces[i].mIndices[j]);
    }
  }

  static int curr_id = 0;
  auto mesh_id = ++curr_id;
  if (mesh.mMaterialIndex >= 0) {
    aiMaterial* ai_mat = scene.mMaterials[mesh.mMaterialIndex];
    spdlog::info("name: {}", ai_mat->GetName().data);
    spdlog::info("scene name: {}", scene.mName.data);
    exit(0);
  }
  return mesh_id;
}

}  // namespace

MeshID MeshManager::LoadCube() {
  std::vector<gfx::Vertex> vertices;
  for (const auto& vertex : gfx::shape::CubeVertices) {
    vertices.emplace_back(vertex);
  }
  std::vector<gfx::Index> indices;
  for (const auto& index : gfx::shape::CubeIndices) {
    indices.emplace_back(index);
  }
  auto id = entt::hashed_string{"cube"};
  renderer_->AddBatchedMesh(id, vertices, indices);
  return id;
};

void MeshManager::Init(gfx::Renderer* renderer) { renderer_ = renderer; }

std::optional<MeshID> MeshManager::LoadModel(const std::string& path) {
  EASSERT_MSG(renderer_, "not initialized");
  spdlog::info("path: {}   directory: {}", path, path.find_last_of('/'));

  Assimp::Importer importer;

  uint32_t flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace;
  const aiScene* scene = importer.ReadFile(path, flags);
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    spdlog::error("Assimp error: {}", importer.GetErrorString());
    return std::nullopt;
  }
  std::stack<aiNode*> mesh_stack;
  mesh_stack.push(scene->mRootNode);
  aiNode* curr_node;
  for (int i = 0; i < scene->mNumMaterials; i++) {
    auto* material = scene->mMaterials[i];
    aiString texture_filename;
    if (material->GetTextureCount(aiTextureType_DIFFUSE)) {
    }

    int num_diffuse = material->GetTextureCount(aiTextureType_DIFFUSE);
    spdlog::info("material {}, {}", material->GetName().data, num_diffuse);
  }

  while (!mesh_stack.empty()) {
    curr_node = mesh_stack.top();
    mesh_stack.pop();
    std::vector<gfx::Vertex> vertices;
    std::vector<gfx::Index> indices;
    for (uint32_t i = 0; i < curr_node->mNumMeshes; i++) {
      aiMesh* mesh = scene->mMeshes[curr_node->mMeshes[i]];
      MeshID id = ProcessMesh(*scene, *mesh, vertices, indices);
      renderer_->AddBatchedMesh(id, vertices, indices);
    }
    // push children to stack
    for (uint32_t i = 0; i < curr_node->mNumChildren; i++) {
      mesh_stack.push(curr_node->mChildren[i]);
    }
  }
  return std::nullopt;
}

MeshID MeshManager::LoadShape(ShapeType type) {
  switch (type) {
    case ShapeType::Cube:
      return LoadCube();
  }
}
}  // namespace engine
