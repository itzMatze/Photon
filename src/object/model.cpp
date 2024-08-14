#include "object/model.hpp"

#include <iostream>
#include <string>

#include "image/image_file_handler.hpp"
#include "object/mesh.hpp"
#include "scene/scene_builder.hpp"
#include "tinygltf/tiny_gltf.h"
#include "glm/gtc/type_ptr.hpp"
#include "util/timer.hpp"
#include "util/log.hpp"

namespace GLTFModel
{
struct ModelData
{
  std::vector<Mesh> meshes;
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
  std::vector<int32_t> texture_indices;
  std::vector<int32_t> material_indices;
};

int32_t load_material(SceneBuilder& scene_builder, int mat_id, const tinygltf::Model& model, ModelData& model_data)
{
  if (model_data.material_indices[mat_id] > -1) return model_data.material_indices[mat_id];
  const tinygltf::Material& mat = model.materials[mat_id];

  auto get_texture = [&](const std::string& name) -> int32_t {
    if (mat.values.find(name) == mat.values.end()) return -1;
    // check if texture is already loaded and if not load it
    int texture_idx = mat.values.at(name).TextureIndex();
    if (model_data.texture_indices[texture_idx] > -1) return model_data.texture_indices[texture_idx];
    const tinygltf::Texture& tex = model.textures[texture_idx];
    model_data.texture_indices[texture_idx] = scene_builder.get_geometry().add_texture(Texture(load_image(model.images[tex.source].image.data(), model.images[tex.source].width, model.images[tex.source].height, 4)));
    return model_data.texture_indices[texture_idx];
  };

  MaterialParameters material;
  material.albedo_texture_id = get_texture("baseColorTexture");
  //material.metallic_roughness_texture = get_texture("metallicRoughnessTexture", 1);
  //material.normal_texture = get_texture("normalTexture", 1);
  //material.emissive_texture = get_texture("emissiveTexture", 1);
  //material.occlusion_texture = get_texture("occlusionTexture", 1);
  if (mat.values.find("baseColorFactor") != mat.values.end())
  {
    material.albedo = glm::make_vec4(mat.values.at("baseColorFactor").ColorFactor().data());
  }
  if (mat.values.find("metallicFactor") != mat.values.end())
  {
    material.metallic = static_cast<float>(mat.values.at("metallicFactor").Factor());
  }
  if (mat.values.find("roughnessFactor") != mat.values.end())
  {
    material.roughness = static_cast<float>(mat.values.at("roughnessFactor").Factor());
  }
  if (mat.additionalValues.find("emissiveFactor") != mat.additionalValues.end())
  {
    material.emission = glm::vec4(glm::make_vec3(mat.additionalValues.at("emissiveFactor").ColorFactor().data()), 1.0);
  }
  if (mat.extensions.find("KHR_materials_emissive_strength") != mat.extensions.end())
  {
    material.emission_strength = mat.extensions.at("KHR_materials_emissive_strength").Get("emissiveStrength").GetNumberAsDouble();
  }
  if (mat.extensions.find("KHR_materials_transmission") != mat.extensions.end())
  {
    material.transmission = mat.extensions.at("KHR_materials_transmission").Get("transmissionFactor").GetNumberAsDouble();
  }
  model_data.material_indices[mat_id] = scene_builder.get_geometry().add_material(material);
  return model_data.material_indices[mat_id];
}

void process_mesh(SceneBuilder& scene_builder, const tinygltf::Mesh& mesh, const tinygltf::Model& model, const glm::mat4 matrix, ModelData& model_data)
{
  for (const tinygltf::Primitive& primitive : mesh.primitives)
  {
    uint32_t idx_count = model_data.indices.size();
    uint32_t vertex_count = model_data.vertices.size();
    // vertices
    {
      const float* pos_buffer = nullptr;
      const float* normal_buffer = nullptr;
      const float* tex_buffer = nullptr;
      const float* color_buffer = nullptr;

      int pos_stride;
      int normal_stride;
      int tex_stride;
      int color_stride;

      // load access information
      const tinygltf::Accessor& pos_accessor = model.accessors[primitive.attributes.find("POSITION")->second];
      const tinygltf::BufferView& pos_view = model.bufferViews[pos_accessor.bufferView];
      pos_buffer = reinterpret_cast<const float*>(&(model.buffers[pos_view.buffer].data[pos_accessor.byteOffset + pos_view.byteOffset]));
      pos_stride = pos_accessor.ByteStride(pos_view) ? (pos_accessor.ByteStride(pos_view) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC3);
      if (primitive.attributes.find("NORMAL") != primitive.attributes.end())
      {
        const tinygltf::Accessor& normal_accessor = model.accessors[primitive.attributes.find("NORMAL")->second];
        const tinygltf::BufferView& normal_view = model.bufferViews[normal_accessor.bufferView];
        normal_buffer = reinterpret_cast<const float*>(&(model.buffers[normal_view.buffer].data[normal_accessor.byteOffset + normal_view.byteOffset]));
        normal_stride = normal_accessor.ByteStride(normal_view) ? (normal_accessor.ByteStride(normal_view) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC3);
      }
      if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end())
      {
        const tinygltf::Accessor& tex_accessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
        const tinygltf::BufferView& tex_view = model.bufferViews[tex_accessor.bufferView];
        tex_buffer = reinterpret_cast<const float*>(&(model.buffers[tex_view.buffer].data[tex_accessor.byteOffset + tex_view.byteOffset]));
        tex_stride = tex_accessor.ByteStride(tex_view) ? (tex_accessor.ByteStride(tex_view) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC2);
      }
      if (primitive.attributes.find("COLOR_0") != primitive.attributes.end())
      {
        const tinygltf::Accessor& color_accessor = model.accessors[primitive.attributes.find("COLOR_0")->second];
        const tinygltf::BufferView& color_view = model.bufferViews[color_accessor.bufferView];
        color_buffer = reinterpret_cast<const float*>(&(model.buffers[color_view.buffer].data[color_accessor.byteOffset + color_view.byteOffset]));
        color_stride = color_accessor.ByteStride(color_view) ? (color_accessor.ByteStride(color_view) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC3);
      }

      // access data and load vertices
      for (size_t i = 0; i < pos_accessor.count; ++i)
      {
        Vertex vertex;
        glm::vec4 tmp_pos = matrix * glm::vec4(glm::make_vec3(&pos_buffer[i * pos_stride]), 1.0f);
        vertex.pos = glm::vec3(tmp_pos.x / tmp_pos.w, tmp_pos.y / tmp_pos.w, tmp_pos.z / tmp_pos.w);
        glm::vec4 tmp_normal = glm::inverse(glm::transpose(matrix)) * glm::vec4(glm::normalize(glm::make_vec3(&normal_buffer[i * normal_stride])), 0.0f);
        vertex.normal = glm::normalize(glm::vec3(tmp_normal));
        vertex.color = color_buffer ? Color(glm::make_vec4(&color_buffer[i * color_stride])) : Color(1.0, 1.0, 1.0);
        vertex.tex_coords = tex_buffer ? glm::make_vec2(&tex_buffer[i * tex_stride]) : glm::vec2(-1.0f);
        model_data.vertices.push_back(vertex);
      }
    }
    // indices
    const tinygltf::Accessor& accessor = model.accessors[primitive.indices > -1 ? primitive.indices : 0];
    const tinygltf::BufferView& buffer_view = model.bufferViews[accessor.bufferView];
    const void* raw_data = &(model.buffers[buffer_view.buffer].data[accessor.byteOffset + buffer_view.byteOffset]);

    auto add_indices([&](const auto* buf) -> void {
      for (size_t i = 0; i < accessor.count; ++i)
      {
        model_data.indices.push_back(buf[i] + vertex_count);
      }
    });
    switch (accessor.componentType)
    {
      case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
        add_indices(static_cast<const uint32_t*>(raw_data));
        break;
      case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
        add_indices(static_cast<const uint16_t*>(raw_data));
        break;
      case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
        add_indices(static_cast<const uint8_t*>(raw_data));
        break;
      default:
        break;
    }
    if (model_data.material_indices.size() > 0 && primitive.material > -1)
    {
      model_data.meshes.push_back(Mesh(load_material(scene_builder, primitive.material, model, model_data), idx_count, model_data.indices.size() - idx_count));
    }
    else
    {
      model_data.meshes.push_back(Mesh(-1, idx_count, model_data.indices.size() - idx_count));
    }
  }
}

void process_node(SceneBuilder& scene_builder, const tinygltf::Node& node, const tinygltf::Model& model, const glm::mat4 transformation, ModelData& model_data)
{
  glm::vec3 translation = (node.translation.size() == 3) ? glm::make_vec3(node.translation.data()) : glm::dvec3(0.0f);
  glm::quat q = (node.rotation.size() == 4) ? glm::make_quat(node.rotation.data()) : glm::qua<double>();
  glm::vec3 scale = (node.scale.size() == 3) ? glm::make_vec3(node.scale.data()) : glm::dvec3(1.0f);
  glm::mat4 matrix = (node.matrix.size() == 16) ? glm::make_mat4x4(node.matrix.data()) : glm::dmat4(1.0f);
  matrix = transformation * glm::translate(glm::mat4(1.0f), translation) * glm::mat4(q) * glm::scale(glm::mat4(1.0f), scale) * matrix;
  for (auto& child_idx : node.children)
  {
    process_node(scene_builder, model.nodes[child_idx], model, matrix, model_data);
  }
  if (node.mesh > -1) (process_mesh(scene_builder, model.meshes[node.mesh], model, matrix, model_data));
}

int32_t load(SceneBuilder& scene_builder, const std::string& model_path, bool load_materials)
{
  std::string path = std::string("assets/models/") + model_path;
  Timer t;
  phlog::debug("Loading glb \"{}\"", path);
  tinygltf::TinyGLTF loader;
  tinygltf::Model model;
  std::string err;
  std::string warn;
  if (!loader.LoadBinaryFromFile(&model, &err, &warn, path))
  {
    phlog::error("Failed to load model \"{}\": {}", path, err);
    return -1;
  }
  if (!warn.empty()) phlog::warn("Warning: {}", warn);

  ModelData model_data;
  if (load_materials)
  {
    model_data.texture_indices.resize(model.textures.size(), -1);
    model_data.material_indices.resize(model.materials.size(), -1);
  }

  const tinygltf::Scene& scene = model.scenes[model.defaultScene > -1 ? model.defaultScene : 0];
  // traverse scene nodes
  for (auto& node_idx : scene.nodes)
  {
    process_node(scene_builder, model.nodes[node_idx], model, glm::mat4(1.0f), model_data);
  }
  phlog::debug("Successfully loaded glb \"{}\" in {}ms", path, t.elapsed<std::milli>());
  return scene_builder.get_geometry().add_object(Object(model_data.vertices, model_data.indices, model_data.meshes));
}
} // namespace GLTFModel
