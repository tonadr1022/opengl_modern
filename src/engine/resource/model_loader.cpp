#include "model_loader.h"

#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace engine {

glm::vec3 aiVec3ToGLM(const aiVector3f& vec) { return {vec.x, vec.y, vec.z}; }
glm::vec2 aiVec2ToGLM(const aiVector3D& vec) { return {vec.x, vec.y}; }

std::optional<ModelData> ModelLoader::LoadModel(const std::string& filepath) {
  ZoneScopedNC("load model", tracy::Color::Red);
  spdlog::info("loading {}", path);
  int slash_idx = path.find_last_of("/\\");
  std::string directory = path.substr(0, slash_idx + 1);

  uint32_t flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace;
  const aiScene* scene = importer->ReadFile(path, flags);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    spdlog::error("Path: {}\nAssimp error: {}", path, importer->GetErrorString());
    return {};
  }

  // in sync with scene materials, material manager has different ids.
  // each index contains the material id
  std::vector<MaterialHandle> material_ids;
  material_ids.resize(scene->mNumMaterials);
  aiString filename;
  for (int ai_scene_mat_idx = 0; ai_scene_mat_idx < scene->mNumMaterials; ai_scene_mat_idx++) {
    auto* material = scene->mMaterials[ai_scene_mat_idx];

    // returns full path if texture exists.
    // TODO(tony): other texture meta data?
    auto get_texture_path = [&](aiTextureType type) -> std::optional<std::string> {
      if (material->GetTextureCount(type)) {
        material->GetTexture(type, 0, &filename);
        if (filename.length > 0) return directory + filename.data;
      }
      return std::nullopt;
    };

    MaterialCreateInfo m;
    m.albedo_path = get_texture_path(aiTextureType_DIFFUSE);
    if (!m.albedo_path.has_value()) m.albedo_path = get_texture_path(aiTextureType_BASE_COLOR);
    m.roughness_path = get_texture_path(aiTextureType_DIFFUSE_ROUGHNESS);
    m.metalness_path = get_texture_path(aiTextureType_METALNESS);
    m.ao_path = get_texture_path(aiTextureType_AMBIENT_OCCLUSION);
    m.normal_path = get_texture_path(aiTextureType_NORMALS);

    // add material and get id. associate it with the scene in the vector so that
    // meshes can access the real material id.
    MaterialHandle material_id = MaterialManager::Get().AddMaterial(m);
    material_ids[ai_scene_mat_idx] = material_id;
  }

  // TODO(tony): scene graph instead of straight vector

  // while (!mesh_stack.empty()) {
  //   curr_node = mesh_stack.top();
  //   mesh_stack.pop();
  //   std::vector<gfx::Vertex> vertices;
  //   std::vector<gfx::Index> indices;
  //   for (uint32_t i = 0; i < curr_node->mNumMeshes; i++) {
  //     aiMesh* mesh = scene->mMeshes[curr_node->mMeshes[i]];
  //     MeshID id = ProcessMesh(*scene, *mesh, vertices, indices);
  //     renderer_->AddBatchedMesh(id, vertices, indices);
  //   }
  //   // push children to stack
  //   for (uint32_t i = 0; i < curr_node->mNumChildren; i++) {
  //     mesh_stack.push(curr_node->mChildren[i]);
  //   }
  // }

  std::vector<component::MeshMaterial> mesh_materials;
  mesh_materials.reserve(scene->mNumMeshes);
  std::vector<gfx::Vertex> vertices;
  std::vector<gfx::Index> indices;
  component::MeshMaterial mesh_material;
  {
    ZoneScopedN("geometry load");
    for (int i = 0; i < scene->mNumMeshes; i++) {
      aiMesh& mesh = *scene->mMeshes[i];
      EASSERT_MSG(mesh.HasPositions(), "need positions");
      EASSERT_MSG(mesh.HasNormals(), "need normals");
      EASSERT_MSG(mesh.HasTextureCoords(0), "need tex coords");

      vertices.clear();
      vertices.reserve(mesh.mNumVertices);
      indices.clear();

      gfx::Vertex v;
      // process vertices
      for (uint32_t i = 0; i < mesh.mNumVertices; i++) {
        v.position = aiVec3ToGLM(mesh.mVertices[i]);
        v.normal = aiVec3ToGLM(mesh.mNormals[i]);
        v.tex_coords = aiVec2ToGLM(mesh.mTextureCoords[0][i]);
        vertices.emplace_back(v);
      }

      // process indices
      for (uint32_t i = 0; i < mesh.mNumFaces; i++) {
        for (uint32_t j = 0; j < mesh.mFaces[i].mNumIndices; j++) {
          indices.push_back(mesh.mFaces[i].mIndices[j]);
        }
      }

      MaterialHandle mat_id = (mesh.mMaterialIndex >= 0)
                                  ? material_ids[mesh.mMaterialIndex]
                                  : MaterialManager::Get().GetDefaultMaterialId();
      MeshID mesh_id = renderer_->AddBatchedMesh(vertices, indices);
      mesh_material.material_handle = mat_id;
      mesh_material.mesh_handle = mesh_id;
      mesh_materials.emplace_back(mesh_material);
    }
  }
  return mesh_materials;
}

}  // namespace engine
