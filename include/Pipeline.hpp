#pragma once

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

struct Vertex
{
    glm::vec3   _position;
    glm::vec3   _normal;
    glm::vec3   _color;
};

class VertexAttribute
{

};

class Pipeline
{
public:
    void init(  const vk::Device& device,
                const std::string& vertFile,
                const std::string& fragFile
                
                 );
    void create();
    void destroy();

    /* Getter */
public:

    /* Utils */
private:
    std::vector<char> readFile( const std::string& fileName );

private:
    vk::Pipeline            _pipeline_;
    vk::PipelineLayout      _layout_;
    const std::string*      _pVertFile_;
    const std::string*      _pFragFile_;

    /* Checker */
private:
    bool _hasBeenInit_          = false;
    bool _hasBeenCreated_       = false;

    /* Depend */
private:
    const vk::Device*       _pDevice_;
};