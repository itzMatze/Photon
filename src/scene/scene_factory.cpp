#include "scene/scene_factory.hpp"
#include "image/image_file_handler.hpp"
#include "object/material.hpp"
#include "object/object.hpp"
#include "scene/scene.hpp"
#include "scene/scene_builder.hpp"
#include "scene/scene_file_handler.hpp"
#include "util/quat.hpp"
#include "util/vec.hpp"

Scene create_single_triangle_scene()
{
  SceneBuilder scene_builder;
  scene_builder.get_geometry().add_material(Material(MaterialType::Diffuse, MaterialParameters{.show_albedo = true}));
  const std::vector<Vertex> vertices{
    cm::Vec3(-1.0, -1.0, -5.0),
    cm::Vec3(1.0, -1.0, -5.0),
    cm::Vec3(0.0, 1.0, -5.0)
  };
  scene_builder.get_geometry().add_object(Object(vertices, {0, 1, 2}, SpatialConfiguration(), -1, true));
  return scene_builder.build_scene();
}

Scene create_triple_triangle_scene()
{
  SceneBuilder scene_builder;
  const std::vector<Vertex> vertices{
    cm::Vec3(-1.0, -1.0, -5.0),
    cm::Vec3(1.0, -1.0, -5.0),
    cm::Vec3(0.0, 1.0, -5.0),
    cm::Vec3(-2.0, -1.0, -4.0),
    cm::Vec3(0.0, -1.0, -4.0),
    cm::Vec3(-1.0, 1.0, -4.0),
    cm::Vec3(0.0, -1.0, -4.0),
    cm::Vec3(2.0, -1.0, -4.0),
    cm::Vec3(1.0, 1.0, -4.0)
  };
  scene_builder.get_geometry().add_object(Object(vertices, {0, 1, 2, 3, 4, 5, 6, 7, 8}, SpatialConfiguration(), -1, true));
  return scene_builder.build_scene();
}

Object add_star(const cm::Vec3& center, float inner_radius, float tip_length, uint32_t tip_count) {
  std::vector<Vertex> vertices{cm::Vec3(0.0, 0.0, 0.0)};
  std::vector<uint32_t> indices;
  // the angle that one tip takes up
  const float tip_angle = (2 * M_PI / float(tip_count));
  for (uint32_t i = 0; i < tip_count; i++)
  {
    const float angle0 = i * tip_angle;
    const float angle1 = angle0 - 0.5 * tip_angle;
    const float angle2 = angle0 + 0.5 * tip_angle;

    vertices.emplace_back(cm::Vec3(sin(angle0) * tip_length, cos(angle0) * tip_length, 0.0));
    vertices.emplace_back(cm::Vec3(sin(angle1) * inner_radius, cos(angle1) * inner_radius, 0.0));
    vertices.emplace_back(cm::Vec3(sin(angle2) * inner_radius, cos(angle2) * inner_radius, 0.0));

    indices.emplace_back(0);
    indices.emplace_back(vertices.size() - 3);
    indices.emplace_back(vertices.size() - 2);
    indices.emplace_back(0);
    indices.emplace_back(vertices.size() - 1);
    indices.emplace_back(vertices.size() - 3);
  }
  SpatialConfiguration spatial_conf;
  spatial_conf.set_position(center);
  return Object(vertices, indices, spatial_conf, -1, true);
}

Scene create_pyramid_star_scene()
{
  SceneBuilder scene_builder;
  std::vector<uint32_t> object_ids;
  // pyramid
  {
    const std::vector<Vertex> vertices{
      cm::Vec3(-1.0, -1.0, 1.0),
      cm::Vec3(1.0, -1.0, 1.0),
      cm::Vec3(1.0, -1.0, -1.0),
      cm::Vec3(-1.0, -1.0, -1.0),
      cm::Vec3(0.0, 1.0, 0.0)
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
    spatial_conf.set_position(cm::Vec3(-2.0, 0.0, -6.0));
    object_ids.emplace_back(scene_builder.get_geometry().add_object(Object(vertices, indices, spatial_conf, -1, true)));
    // right pyramid
    spatial_conf.set_position(cm::Vec3(2.0, 0.0, -6.0));
    object_ids.emplace_back(scene_builder.get_geometry().add_object(Object(vertices, indices, spatial_conf, -1, true)));
  }

  // star
  scene_builder.get_geometry().add_object(add_star(cm::Vec3(0.0, 0.5, -4.0), 0.1, 0.5, 5));

  {
    scene_builder.new_keyframe(30);
    for (uint32_t id : object_ids)
    {
      scene_builder.get_geometry().get_object(id).get_spatial_conf().rotate(90.0, 0.0, 0.0);
    }
    scene_builder.get_camera().get_spatial_conf().set_position(cm::Vec3(-4.0, 0.0, 1.0));
    const cm::Vec3 view_dir = cm::Vec3(0.0, 0.0, -6.0) - scene_builder.get_camera().get_spatial_conf().get_position();
    scene_builder.get_camera().get_spatial_conf().set_orientation(cm::quat_look_at(cm::normalize(view_dir)));
  }

  {
    scene_builder.new_keyframe(30);
    for (uint32_t id : object_ids)
    {
      scene_builder.get_geometry().get_object(id).get_spatial_conf().rotate(90.0, 0.0, 0.0);
    }
    scene_builder.get_camera().get_spatial_conf().set_position(cm::Vec3(-4.0, 4.0, 1.0));
  }

  {
    scene_builder.new_keyframe(30);
    for (uint32_t id : object_ids)
    {
      scene_builder.get_geometry().get_object(id).get_spatial_conf().rotate(90.0, 0.0, 0.0);
    }
    const cm::Vec3 view_dir = cm::Vec3(0.0, 0.0, -6.0) - scene_builder.get_camera().get_spatial_conf().get_position();
    scene_builder.get_camera().get_spatial_conf().set_orientation(cm::quat_look_at(cm::normalize(view_dir)));
  }

  {
    scene_builder.new_keyframe(30);
    for (uint32_t id : object_ids)
    {
      scene_builder.get_geometry().get_object(id).get_spatial_conf().rotate(90.0, 0.0, 0.0);
    }
    scene_builder.get_camera().get_spatial_conf().set_position(cm::Vec3(0.0, 0.0, 0.0));
    scene_builder.get_camera().get_spatial_conf().set_orientation(cm::quat_look_at(cm::Vec3(0.0, 0.0, -1.0)));
  }

  {
    scene_builder.new_keyframe(15);
    for (uint32_t id : object_ids)
    {
      scene_builder.get_geometry().get_object(id).get_spatial_conf().rotate(45.0, 0.0, 0.0);
    }
    scene_builder.get_camera().get_spatial_conf().rotate(30.0, 0.0, 0.0);
  }
  {
    scene_builder.new_keyframe(30);
    for (uint32_t id : object_ids)
    {
      scene_builder.get_geometry().get_object(id).get_spatial_conf().rotate(90.0, 0.0, 0.0);
    }
    scene_builder.get_camera().get_spatial_conf().rotate(-60.0, 0.0, 0.0);
  }
  {
    scene_builder.new_keyframe(15);
    for (uint32_t id : object_ids)
    {
      scene_builder.get_geometry().get_object(id).get_spatial_conf().rotate(45.0, 0.0, 0.0);
    }
    scene_builder.get_camera().get_spatial_conf().rotate(30.0, 0.0, 0.0);
  }

  return scene_builder.build_scene();
}
