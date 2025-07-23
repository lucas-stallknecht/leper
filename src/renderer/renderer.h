#pragma once

#include <cstdint>
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

        void start_frame();
        void finish_frame(uint16_t width, uint16_t height);

        void upload_mesh(const Mesh& mesh);
        bool has_mesh_objects(const Mesh& mesh);
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

      private:
        std::unordered_map<std::string, MeshGlObjetcs> mesh_objects_;
        std::unordered_map<MaterialId, Shader> shaders_;

        GLuint main_fbo_ = 0;
        GLuint color_tex_ = 0;
        GLuint depth_rbo_ = 0;
    };

} // namespace leper
