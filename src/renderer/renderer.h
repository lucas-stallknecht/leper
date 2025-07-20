#pragma once 

#include <unordered_map>

#include "leper/leper_geometry_types.h"
#include "leper/leper_rendering_types.h"

namespace leper {

class Renderer {
public: 
    explicit Renderer();
    
    void register_mesh(const Mesh& mesh);
    void draw_mesh(const Mesh& mesh);

private:
    std::unordered_map<const Mesh*, MeshGlObjetcs> mesh_objects_;

};

}
