#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>

#include "leper/leper_common_types.h"
#include "leper/leper_rendering_types.h"
#include "shader/shader.h"
#include "shader/material_id_to_shader_files.h"

namespace leper {

    class Renderer {
      public:
        Renderer();
        ~Renderer();

        void start_shadow_frame();
        void start_main_frame();
        void finish_main_frame(uint16_t width, uint16_t height);

        bool has_mesh_objects(const Mesh& mesh);
        void upload_mesh(const Mesh& mesh);
        void draw_mesh(const Mesh& mesh);

        template <typename T>
        bool has_material_shader() {
            return shaders_.find(get_material_id<T>()) != shaders_.end();
        }
        template <typename T>
        void create_shader() {
            assert(!has_material_shader<T>() && "Creating the same shader object twice");

            const MaterialId material_id = get_material_id<T>();

            const std::pair<std::string, std::string> shader_names = MATERIAL_TO_SHADER_FILES.at(material_id);
            shaders_.insert({material_id, Shader(shader_names.first, shader_names.second)});
        }
        template <typename T>
        Shader* get_material_shader() {

            if (has_material_shader<T>()) {
                return &shaders_.at(get_material_id<T>());
            }
            return nullptr;
        }
        Shader* get_depth_shader();
        void reload_shaders();

        void draw_trail(uint16_t width, uint16_t height, const std::vector<glm::vec2>& trailPoints);

      private:
        void init_main_frame();
        void init_shadow_map();
        void init_trail();

        std::unordered_map<std::string, MeshGlObjetcs> mesh_objects_;
        std::unordered_map<MaterialId, Shader> shaders_;

        GLuint main_fbo_ = 0;
        GLuint main_texture_ = 0;
        GLuint main_depth_rbo_ = 0;

        std::unique_ptr<Shader> depth_shader_;
        GLuint shadow_map_fbo_ = 0;
        GLuint shadow_map_ = 0;

        std::unique_ptr<Shader> trail_shader_;
        GLuint trail_vao_ = 0;
        GLuint trail_vbo_ = 0;
    };

} // namespace leper
