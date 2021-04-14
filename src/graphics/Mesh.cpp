#include "Mesh.hpp"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader/tiny_obj_loader.h"

bool obj::loadFromObj( const std::string& fileName, Mesh& mesh )
{
    // Attribute will contain vertex array of file
    tinyobj::attrib_t attrib;

    // Shapes will contain the info each separate object in the file (info interconnection within attrib)
    std::vector<tinyobj::shape_t> shapes;

    // Contain material for each shape, but we won't use it for now
    std::vector<tinyobj::material_t> materials;

    std::string warn;
    std::string err;

    const char* fn = fileName.c_str();

    // Load the OBJ file
    // tinyobj::LoadObj( &attrib, &shapes, &materials, &warn, &err, fileName.c_str() );
    tinyobj::LoadObj( &attrib, &shapes, &materials, &warn, &err, fn );

    if( !warn.empty() )
        std::cout << "WARN: " << warn << "\n";
    else if( !err.empty() ){
        std::cerr << "ERR: " << err << "\n";
        return false;
    }

    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++) {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {

            //hardcode loading to triangles
            int fv = 3;

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {                
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                //vertex position
                tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
                tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
                tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
                //vertex normal
                tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
                tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
                tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];
                
                //copy it into our vertex
                vertex0 new_vert;
                new_vert.position.x = vx;
                new_vert.position.y = vy;
                new_vert.position.z = vz;

                new_vert.normal.x = nx;
                new_vert.normal.y = ny;
                new_vert.normal.z = nz;

                //we are setting the vertex color as the vertex normal. This is just for display purposes
                new_vert.color = new_vert.normal;

                
                mesh.vertices().emplace_back(new_vert);
            }
            index_offset += fv;
        }
    }

    return true; 
}