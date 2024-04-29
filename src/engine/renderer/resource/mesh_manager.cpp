
#include "mesh_manager.h"

#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>

#include "engine/renderer/gl/data_types.h"
#include "engine/renderer/renderer.h"
#include "engine/renderer/resource/shapes/shapes.h"

namespace gfx::mesh_manager {

namespace {

glm::vec3 aiVec3ToGLM(const aiVector3f& vec) { return {vec.x, vec.y, vec.z}; }
glm::vec2 aiVec2ToGLM(const aiVector3D& vec) { return {vec.x, vec.y}; }

Assimp::Importer importer;
uint32_t flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace;

void ProcessMesh(const aiMesh& mesh) {
  EASSERT_MSG(mesh.HasPositions() && mesh.HasNormals() && mesh.HasTextureCoords(0),
              "Mesh needs positions, normals, and texture coords");
  std::vector<Vertex> vertices;
  vertices.reserve(mesh.mNumVertices);
  std::vector<Index> indices;

  Vertex v;
  // process vertices
  for (uint32_t i = 0; i < mesh.mNumVertices; i++) {
    v.position = aiVec3ToGLM(mesh.mVertices[i]);
    v.normal = aiVec3ToGLM(mesh.mNormals[i]);
    v.tex_coords = aiVec2ToGLM(mesh.mTextureCoords[0][i]);
  }

  // process indices
  for (Index i = 0; i < mesh.mNumFaces; i++) {
    for (Index j = 0; j < mesh.mFaces[i].mNumIndices; j++) {
      indices.push_back(mesh.mFaces[i].mIndices[j]);
    }
  }

  // if (mesh.mMaterialIndex >= 0) {
  //   aiMaterial* ai_mat = scene.mMaterials[mesh.mMaterialIndex];
  // }
}

std::optional<MeshID> ProcessNodes(const aiScene& scene) {
  std::stack<aiNode*> mesh_stack;
  mesh_stack.push(scene.mRootNode);

  aiNode* curr_node;
  std::vector<bool> processed_materials(scene.mNumMaterials, false);

  while (!mesh_stack.empty()) {
    curr_node = mesh_stack.top();
    mesh_stack.pop();

    for (uint32_t i = 0; i < curr_node->mNumMeshes; i++) {
      aiMesh* mesh = scene.mMeshes[curr_node->mMeshes[i]];
      ProcessMesh(*mesh);
    }
    // push children to stack
    for (uint32_t i = 0; i < curr_node->mNumChildren; i++) {
      mesh_stack.push(curr_node->mChildren[i]);
    }
  }
  return std::nullopt;
}

MeshID LoadCube() {
  std::vector<Vertex> vertices;
  for (const auto& vertex : gfx::shape::CubeVertices) {
    vertices.emplace_back(vertex);
  }
  std::vector<Index> indices;
  for (const auto& index : gfx::shape::CubeIndices) {
    indices.emplace_back(index);
  }
  auto id = HashedString("cube");
  gfx::renderer::AddBatchedMesh(id, vertices, indices);
  return id;
};

}  // namespace

std::optional<MeshID> LoadModel(const std::string& path) {
  const aiScene* scene = importer.ReadFile(path, flags);
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    spdlog::error("Assimp error: {}", importer.GetErrorString());
    return std::nullopt;
  }

  return ProcessNodes(*scene);
}

MeshID LoadShape(ShapeType type) {
  switch (type) {
    case ShapeType::Cube:
      return LoadCube();
  }
}

}  // namespace gfx::mesh_manager
