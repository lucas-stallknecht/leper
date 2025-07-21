#pragma once

#include <cstdint>
#include <typeindex>
#include <unordered_map>

#include "leper/leper_common_types.h"
#include "leper/leper_material_to_shader_files.h"
#include "leper/leper_rendering_types.h"
#include "shader/shader.h"

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
            return shaders_.find(typeid(T)) != shaders_.end();
        }

        template <typename T>
        void create_shader() {
            assert(!has_material_shader<T>() && "Creating the same shader object twice");
            const std::type_index material_index = typeid(T);
            const std::pair<std::string, std::string> shader_names = MATERIAL_TO_SHADER_FILES.at(material_index);
            shaders_.insert({material_index, Shader(shader_names.first, shader_names.second)});
        }

        template <typename T>
        Shader* get_material_shader() {

            if (has_material_shader<T>()) {
                return &shaders_.at(typeid(T));
            }
            return nullptr;
        }

      private:
        std::unordered_map<std::string, MeshGlObjetcs> mesh_objects_;
        std::unordered_map<std::type_index, Shader> shaders_;

        GLuint main_fbo_ = 0;
        GLuint color_tex_ = 0;
        GLuint depth_rbo_ = 0;

    };

} // namespace leper
