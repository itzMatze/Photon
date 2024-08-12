#include "scene/scene_factory.hpp"
#include "glm/gtc/quaternion.hpp"
#include "image/image_file_handler.hpp"
#include "object/material.hpp"
#include "object/object.hpp"
#include "scene/scene.hpp"
#include "scene/scene_builder.hpp"
#include "scene/scene_file_handler.hpp"

Scene create_single_triangle_scene()
{
  SceneBuilder scene_builder;
  scene_builder.get_geometry().add_material(MaterialParameters{.albedo = glm::vec3(0.99, 0.01, 0.55), .show_albedo = true});
  const std::vector<Vertex> vertices{
    glm::vec3(-1.0, -1.0, -5.0),
    glm::vec3(1.0, -1.0, -5.0),
    glm::vec3(0.0, 1.0, -5.0)
  };
  uint32_t object_id = scene_builder.get_geometry().add_object(Object(vertices, {0, 1, 2}, true));
  scene_builder.get_geometry().add_object_instance(object_id, 0);
  return scene_builder.build_scene();
}

Scene create_triple_triangle_scene()
{
  SceneBuilder scene_builder;
  scene_builder.get_geometry().add_material(MaterialParameters{.albedo = glm::vec3(0.99, 0.01, 0.55), .show_albedo = true});
  const std::vector<Vertex> vertices{
    glm::vec3(-1.0, -1.0, -5.0),
    glm::vec3(1.0, -1.0, -5.0),
    glm::vec3(0.0, 1.0, -5.0),
    glm::vec3(-2.0, -1.0, -4.0),
    glm::vec3(0.0, -1.0, -4.0),
    glm::vec3(-1.0, 1.0, -4.0),
    glm::vec3(0.0, -1.0, -4.0),
    glm::vec3(2.0, -1.0, -4.0),
    glm::vec3(1.0, 1.0, -4.0)
  };
  uint32_t object_id = scene_builder.get_geometry().add_object(Object(vertices, {0, 1, 2, 3, 4, 5, 6, 7, 8}, true));
  scene_builder.get_geometry().add_object_instance(object_id, 0);
  return scene_builder.build_scene();
}

Object add_star(float inner_radius, float tip_length, uint32_t tip_count) {
  std::vector<Vertex> vertices{glm::vec3(0.0, 0.0, 0.0)};
  std::vector<uint32_t> indices;
  // the angle that one tip takes up
  const float tip_angle = (2 * M_PI / float(tip_count));
  for (uint32_t i = 0; i < tip_count; i++)
  {
    const float angle0 = i * tip_angle;
    const float angle1 = angle0 - 0.5 * tip_angle;
    const float angle2 = angle0 + 0.5 * tip_angle;

    vertices.emplace_back(glm::vec3(sin(angle0) * tip_length, cos(angle0) * tip_length, 0.0));
    vertices.emplace_back(glm::vec3(sin(angle1) * inner_radius, cos(angle1) * inner_radius, 0.0));
    vertices.emplace_back(glm::vec3(sin(angle2) * inner_radius, cos(angle2) * inner_radius, 0.0));

    indices.emplace_back(0);
    indices.emplace_back(vertices.size() - 3);
    indices.emplace_back(vertices.size() - 2);
    indices.emplace_back(0);
    indices.emplace_back(vertices.size() - 1);
    indices.emplace_back(vertices.size() - 3);
  }
  return Object(vertices, indices, true);
}

Scene create_pyramid_star_scene()
{
  SceneBuilder scene_builder;
  scene_builder.get_geometry().add_material(MaterialParameters{.albedo = glm::vec3(0.99, 0.01, 0.55), .show_albedo = true});
  std::vector<uint32_t> object_instance_ids;
  // pyramid
  {
    const std::vector<Vertex> vertices{
      glm::vec3(-1.0, -1.0, 1.0),
      glm::vec3(1.0, -1.0, 1.0),
      glm::vec3(1.0, -1.0, -1.0),
      glm::vec3(-1.0, -1.0, -1.0),
      glm::vec3(0.0, 1.0, 0.0)
    };
    const std::vector<uint32_t> indices{
      0, 1, 4,
      1, 2, 4,
      2, 3, 4,
      3, 0, 4,
      0, 2, 1,
      0, 3, 2
    };
    SpatialConfiguration spatial_conf;
    spatial_conf.rotate(45.0, 0.0, 0.0);
    // left pyramid
    spatial_conf.set_position(glm::vec3(-2.0, 0.0, -6.0));
    uint32_t object_id = scene_builder.get_geometry().add_object(Object(vertices, indices, true));
    object_instance_ids.emplace_back(scene_builder.get_geometry().add_object_instance(object_id, 0, spatial_conf));
    // right pyramid
    spatial_conf.set_position(glm::vec3(2.0, 0.0, -6.0));
    object_instance_ids.emplace_back(scene_builder.get_geometry().add_object_instance(object_id, 0, spatial_conf));
  }

  {
    // star
    uint32_t object_id = scene_builder.get_geometry().add_object(add_star(0.1, 0.5, 5));
    SpatialConfiguration spatial_conf;
    spatial_conf.set_position(glm::vec3(0.0, 0.5, -4.0));
    scene_builder.get_geometry().add_object_instance(object_id, 0, spatial_conf);
  }

  {
    scene_builder.new_keyframe(30);
    for (uint32_t id : object_instance_ids)
    {
      scene_builder.get_geometry().get_object_instance(id).get_spatial_conf().rotate(90.0, 0.0, 0.0);
    }
    scene_builder.get_camera().get_spatial_conf().set_position(glm::vec3(-4.0, 0.0, 1.0));
    const glm::vec3 view_dir = glm::vec3(0.0, 0.0, -6.0) - scene_builder.get_camera().get_spatial_conf().get_position();
    scene_builder.get_camera().get_spatial_conf().set_orientation(glm::quatLookAt(glm::normalize(view_dir), glm::vec3(0.0, 1.0, 0.0)));
  }

  {
    scene_builder.new_keyframe(30);
    for (uint32_t id : object_instance_ids)
    {
      scene_builder.get_geometry().get_object_instance(id).get_spatial_conf().rotate(90.0, 0.0, 0.0);
    }
    scene_builder.get_camera().get_spatial_conf().set_position(glm::vec3(-4.0, 4.0, 1.0));
  }

  {
    scene_builder.new_keyframe(30);
    for (uint32_t id : object_instance_ids)
    {
      scene_builder.get_geometry().get_object_instance(id).get_spatial_conf().rotate(90.0, 0.0, 0.0);
    }
    const glm::vec3 view_dir = glm::vec3(0.0, 0.0, -6.0) - scene_builder.get_camera().get_spatial_conf().get_position();
    scene_builder.get_camera().get_spatial_conf().set_orientation(glm::quatLookAt(glm::normalize(view_dir), glm::vec3(0.0, 1.0, 0.0)));
  }

  {
    scene_builder.new_keyframe(30);
    for (uint32_t id : object_instance_ids)
    {
      scene_builder.get_geometry().get_object_instance(id).get_spatial_conf().rotate(90.0, 0.0, 0.0);
    }
    scene_builder.get_camera().get_spatial_conf().set_position(glm::vec3(0.0, 0.0, 0.0));
    scene_builder.get_camera().get_spatial_conf().set_orientation(glm::quatLookAt(glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, 1.0, 0.0)));
  }

  {
    scene_builder.new_keyframe(15);
    for (uint32_t id : object_instance_ids)
    {
      scene_builder.get_geometry().get_object_instance(id).get_spatial_conf().rotate(45.0, 0.0, 0.0);
    }
    scene_builder.get_camera().get_spatial_conf().rotate(30.0, 0.0, 0.0);
  }
  {
    scene_builder.new_keyframe(30);
    for (uint32_t id : object_instance_ids)
    {
      scene_builder.get_geometry().get_object_instance(id).get_spatial_conf().rotate(90.0, 0.0, 0.0);
    }
    scene_builder.get_camera().get_spatial_conf().rotate(-60.0, 0.0, 0.0);
  }
  {
    scene_builder.new_keyframe(15);
    for (uint32_t id : object_instance_ids)
    {
      scene_builder.get_geometry().get_object_instance(id).get_spatial_conf().rotate(45.0, 0.0, 0.0);
    }
    scene_builder.get_camera().get_spatial_conf().rotate(30.0, 0.0, 0.0);
  }

  return scene_builder.build_scene();
}
