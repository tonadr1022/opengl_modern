#include "model_loader.h"

#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "engine/renderer/renderer.h"
#include "engine/resource/data_types.h"
#include "engine/resource/material_manager.h"
#include "engine/resource/resource.h"

namespace engine {

glm::vec3 aiVec3ToGLM(const aiVector3f& vec) { return {vec.x, vec.y, vec.z}; }
glm::vec2 aiVec2ToGLM(const aiVector3D& vec) { return {vec.x, vec.y}; }

std::optional<ModelData> ModelLoader::LoadModel(const ModelLoadParams& params) {
  ZoneScopedNC("load model", tracy::Color::Red);
  int slash_idx = params.filepath.find_last_of("/\\");
  int dot_idx = params.filepath.find_last_of('.');
  std::string directory = params.filepath.substr(0, slash_idx + 1);

  spdlog::info("loading {}", params.filepath.substr(slash_idx + 1));

  uint32_t flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace;
  const aiScene* scene = importer_.ReadFile(params.filepath, flags);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    spdlog::error("Path: {}\nAssimp error: {}", params.filepath, importer_.GetErrorString());
    return std::nullopt;
  }

  // in sync with scene materials, material manager has different ids.
  // each index contains the material id
  std::vector<AssetHandle> material_handles;
  material_handles.resize(scene->mNumMaterials);
  for (int ai_scene_mat_idx = 0; ai_scene_mat_idx < scene->mNumMaterials; ai_scene_mat_idx++) {
    auto* material = scene->mMaterials[ai_scene_mat_idx];

    // returns full path if texture exists.
    // TODO(tony): other texture meta data?
    auto get_texture_path = [&](aiTextureType type) -> std::optional<std::string> {
      aiString filename;
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
    if (!m.roughness_path.has_value()) m.roughness_path = get_texture_path(aiTextureType_SHININESS);
    m.metalness_path = get_texture_path(aiTextureType_METALNESS);
    if (!m.metalness_path.has_value()) m.metalness_path = get_texture_path(aiTextureType_AMBIENT);
    m.ao_path = get_texture_path(aiTextureType_AMBIENT_OCCLUSION);
    m.normal_path = get_texture_path(aiTextureType_HEIGHT);
    if (!m.normal_path.has_value()) m.normal_path = get_texture_path(aiTextureType_NORMALS);
    m.flip_textures = params.flip_textures;
    // spdlog::info("\nroughness: {}\n metalness: {}\n ao: {}\n albedo: {}\n normal:{}\n",
    //              m.roughness_path.value_or("no roughnesspath"),
    //              m.metalness_path.value_or("no metalnesspath"), m.ao_path.value_or("no ao_path"),
    //              m.albedo_path.value_or("no albedopath"), m.normal_path.value_or("no normal
    //              path"));

    AssetHandle handle = MaterialManager::Get().AddMaterial(m);
    material_handles[ai_scene_mat_idx] = handle;

    // aiString alphaMaskPath;
    // material->GetTexture(aiTextureType_OPACITY, 0, &alphaMaskPath);
    // spdlog::info("{}\n{}\n{}\n{}\n{}\n", albedoPath.data, metallicPath.data, normalPath.data,
    //              roughnessPath.data, alphaMaskPath.data);

    // add material and get id. associate it with the scene in the vector so that
    // meshes can access the real material id.
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

  ModelData model;
  model.meshes.reserve(scene->mNumMeshes);
  std::vector<gfx::Vertex> vertices;
  std::vector<gfx::Index> indices;
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

      AssetHandle mat_handle = (mesh.mMaterialIndex >= 0)
                                   ? material_handles[mesh.mMaterialIndex]
                                   : MaterialManager::Get().GetDefaultMaterialHandle();
      AssetHandle mesh_id = gfx::Renderer::Get().AddBatchedMesh(vertices, indices);
      Mesh m{.mesh_handle = mesh_id, .material_handle = mat_handle};
      model.meshes.emplace_back(m);
    }
  }
  return std::move(model);
}

}  // namespace engine
