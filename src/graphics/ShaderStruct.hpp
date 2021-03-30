#pragma once

#include "vku/vku.hpp"
#include <glm/glm.hpp>

/*** @brief UTILS */
struct VertexInputDescription
{
    std::vector<vk::VertexInputBindingDescription> bindings;
    std::vector<vk::VertexInputAttributeDescription> attributs;

    vk::PipelineVertexInputStateCreateFlags flags = vk::PipelineVertexInputStateCreateFlags();
};

namespace Vertex
{
struct SimpleVertex
{
    glm::vec3 position;
    // glm::vec3 normal;
    glm::vec3 color;
    // glm::vec2 uv;

    static VertexInputDescription getVertexInputDescription() 
    {
        VertexInputDescription description;

        // we just have 1 binding
        vk::VertexInputBindingDescription mainBinding{};
        mainBinding.setBinding( 0 );
        mainBinding.setInputRate( vk::VertexInputRate::eVertex );
        mainBinding.setStride( sizeof( SimpleVertex ) );
        description.bindings.push_back( mainBinding );


        // position will be stored at location = 0
        vk::VertexInputAttributeDescription positionAttribute {};
        positionAttribute.setBinding( 0 );
        positionAttribute.setLocation( 0 );
        positionAttribute.setFormat( vk::Format::eR32G32B32Sfloat );
        positionAttribute.setOffset( offsetof( SimpleVertex, position ) );
        description.attributs.push_back( positionAttribute );

        // // normal will be stored at location = 1
        // vk::VertexInputAttributeDescription normalAttribute {};
        // normalAttribute.setBinding( 0 );
        // normalAttribute.setLocation( 1 );
        // normalAttribute.setFormat( vk::Format::eR32G32B32Sfloat );
        // normalAttribute.setOffset( offsetof( Vertex, normal ) );
        // description.attributs.push_back( normalAttribute );

        // color will be stored at location = 2
        vk::VertexInputAttributeDescription colorAttribute {};
        colorAttribute.setBinding( 0 );
        // colorAttribute.setLocation( 2 );
        colorAttribute.setLocation( 1 );
        colorAttribute.setFormat( vk::Format::eR32G32B32Sfloat );
        colorAttribute.setOffset( offsetof( SimpleVertex, color ) );
        description.attributs.push_back( colorAttribute );

        // uv will be stored at location = 3
        // vk::VertexInputAttributeDescription uvAttribute {};
        // uvAttribute.setBinding( 0 );
        // uvAttribute.setLocation( 3 );
        // uvAttribute.setFormat( vk::Format::eR32G32Sfloat );
        // uvAttribute.setOffset( offsetof( Vertex, uv ) );
        // description.attributs.push_back( uvAttribute );

        return description;
    }
};

struct Texture
{
    glm::vec3 position;
    glm::vec2 uv;
};
}
