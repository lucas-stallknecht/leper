#include "obj_loading.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <optional>
#include <spdlog/spdlog.h>
#include <vector>

namespace leper {

    std::optional<FaceVertexTriplet> parse_face_to_triplet(const std::string& token) {
        std::istringstream ss(token);
        char slash;

        FaceVertexTriplet out;

        ss >> out.v >> slash >> out.vt >> slash >> out.vn;

        if (ss.fail()) {
            return {};
        }
        return out;
    }

    // S/o https://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/
    // for the tutorial
    std::optional<std::vector<Vertex>> load_obj_geometry(const std::string& file_name) {

        std::filesystem::path shader_file_path = std::filesystem::path(ASSETS_DIR) / file_name;
        std::ifstream file(shader_file_path);

        if (!file.is_open()) {
            spdlog::error("Failed to open asset file {}", shader_file_path.string());
            return {};
        }

        std::vector<uint32_t> position_indices, uv_indices, normal_indices;
        std::vector<glm::vec3> temp_positions;
        std::vector<glm::vec2> temp_uvs;
        std::vector<glm::vec3> temp_normals;

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty())
                continue;

            if (line.rfind("v ", 0) == 0) {
                std::istringstream ss(line);
                char prefix;

                glm::vec3 pos;
                ss >> prefix >> pos.x >> pos.y >> pos.z;

                if (ss.fail()) {
                    spdlog::error("({}) Invalid vertex line: {}", file_name, line);
                    return {};
                }
                temp_positions.push_back(pos);
            } else if (line.rfind("vn ", 0) == 0) {
                std::istringstream ss(line);
                std::string prefix;

                glm::vec3 normal;
                ss >> prefix >> normal.x >> normal.y >> normal.z;

                if (ss.fail()) {
                    spdlog::error("({}) Invalid normal line: {}", file_name, line);
                    return {};
                }
                temp_normals.push_back(normal);
            } else if (line.rfind("f ", 0) == 0) {
                std::istringstream ss(line);
                char prefix;

                std::array<std::string, 3> tokens;

                ss >> prefix >> tokens[0] >> tokens[1] >> tokens[2];

                if (ss.fail()) {
                    spdlog::error("({}) Invalid face line: {}", file_name, line);
                    return {};
                }

                for (size_t i = 0; i < 3; i++) {
                    auto vertex_triplet = parse_face_to_triplet(tokens[i]);

                    if (!vertex_triplet.has_value()) {
                        spdlog::error("({}) Invalid face line: {}", file_name, line);
                        return {};
                    }

                    position_indices.push_back(vertex_triplet.value().v);
                    normal_indices.push_back(vertex_triplet.value().vn);
                }
            }
        }
        file.close();
        assert(position_indices.size() == normal_indices.size());

        std::vector<Vertex> vertices;
        vertices.reserve(position_indices.size());

        for (size_t i = 0; i < position_indices.size(); i++) {
            assert(position_indices[i] > 0 && position_indices[i] <= temp_positions.size());
            assert(normal_indices[i] > 0 && normal_indices[i] <= temp_normals.size());

            Vertex vertex;
            // We substract by 1 because obj are 1 index based and C++ is 0
            vertex.position = temp_positions[position_indices[i] - 1];
            vertex.normal = temp_normals[normal_indices[i] - 1];
            vertex.uv_1 = 0.0f;
            vertex.uv_2 = 0.0f;
            vertices.push_back(vertex);
        }

        return vertices;
    }

} // namespace leper
