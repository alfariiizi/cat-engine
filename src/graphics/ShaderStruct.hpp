#pragma once

// #include "vku/vku.hpp"
#include <glm/glm.hpp>


/*** @brief UTILS */
struct VertexInputDescription
{
    std::vector<vk::VertexInputBindingDescription> bindings;
    std::vector<vk::VertexInputAttributeDescription> attributs;

    vk::PipelineVertexInputStateCreateFlags flags = vk::PipelineVertexInputStateCreateFlags();
};

namespace bo
{
// it's used for UBO
struct CameraData
{
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 viewProj;
};

struct SceneData
{
    glm::vec4 fogColor; // w is for exponent
    glm::vec4 fogDistances; // x for min, y for max, zw unused
    glm::vec4 ambientColor;
    glm::vec4 sunlightDirection; // w is for sun power
    glm::vec4 sunlightColor;
};
}


// Using std430 rule: All the data must be float of vec4. Note that mat4 is 4 * vec4, so it's okay to use.
struct MeshPushConstants
{
    glm::vec4 data;
    glm::mat4 renderMatrix;
};

namespace Vertex
{
struct SimpleVertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;

    static VertexInputDescription getVertexInputDescription() 
    {
        VertexInputDescription desc;

        /// Binding
        uint32_t bindingNumber = 0; // set your binding number
        {
            vk::VertexInputBindingDescription binding {};
            binding.setBinding( bindingNumber );
            binding.setInputRate( vk::VertexInputRate::eVertex ); // set your input rate
            binding.setStride( sizeof( SimpleVertex ) ); // set to this Class Name
            desc.bindings.emplace_back( std::move( binding ) );
        }

        /// Attribute (Penginisialisasian attribute ini haruslah URUT seperti pada Shader nya)
        uint32_t locationNumber = 0;
        {
            { // Position
                vk::VertexInputAttributeDescription attribute {};
                attribute.setBinding( bindingNumber );
                attribute.setFormat( vk::Format::eR32G32B32Sfloat ); // set your format, ex: vk::Format::eR32G32B32A32SFloat
                attribute.setLocation( locationNumber++ );
                attribute.setOffset( offsetof( SimpleVertex, position ) );
                desc.attributs.emplace_back( std::move(attribute) );
            }
            { // Normal
                vk::VertexInputAttributeDescription attribute {};
                attribute.setBinding( bindingNumber );
                attribute.setFormat( vk::Format::eR32G32B32Sfloat ); // set your format, ex: vk::Format::eR32G32B32A32SFloat
                attribute.setLocation( locationNumber++ );
                attribute.setOffset( offsetof( SimpleVertex, normal ) );
                desc.attributs.emplace_back( std::move(attribute) );
            }
            { // Color
                vk::VertexInputAttributeDescription attribute {};
                attribute.setBinding( bindingNumber );
                attribute.setFormat( vk::Format::eR32G32B32Sfloat ); // set your format, ex: vk::Format::eR32G32B32A32SFloat
                attribute.setLocation( locationNumber++ );
                attribute.setOffset( offsetof( SimpleVertex, color ) );
                desc.attributs.emplace_back( std::move(attribute) );
            }
        }

        return desc;
    }
};

struct Texture
{
    glm::vec3 position;
    glm::vec2 uv;

    static VertexInputDescription getVertexInputDescription()
    {
        VertexInputDescription desc;

        /// Binding
        uint32_t bindingNumber = 0; // set your binding number
        {
            vk::VertexInputBindingDescription binding {};
            binding.setBinding( bindingNumber );
            binding.setInputRate( vk::VertexInputRate::eVertex ); // set your input rate
            binding.setStride( sizeof( Texture ) ); // set to this Class Name
            desc.bindings.emplace_back( std::move( binding ) );
        }

        /// Attribute (Penginisialisasian attribute ini haruslah URUT seperti pada Shader nya)
        uint32_t locationNumber = 0;
        {
            { // Position
                vk::VertexInputAttributeDescription attribute {};
                attribute.setBinding( bindingNumber );
                attribute.setFormat( vk::Format::eR32G32B32Sfloat ); // set your format, ex: vk::Format::eR32G32B32A32SFloat
                attribute.setLocation( locationNumber++ );
                attribute.setOffset( offsetof( Texture, position ) );
                desc.attributs.emplace_back( std::move(attribute) );
            }
            { // UV
                vk::VertexInputAttributeDescription attribute {};
                attribute.setBinding( bindingNumber );
                attribute.setFormat( vk::Format::eR32G32Sfloat ); // set your format, ex: vk::Format::eR32G32B32A32SFloat
                attribute.setLocation( locationNumber++ );
                attribute.setOffset( offsetof( Texture, uv ) );
                desc.attributs.emplace_back( std::move(attribute) );
            }
        }

        return desc;
    }
};


/**** TEMPLATE OF THIS VERTEX ****/
// struct Template
// {
//     glm::vec3 position;
//     glm::vec2 uv;

//     static VertexInputDescription getVertexInputDescription()
//     {
//         VertexInputDescription desc;

//         /// Binding
//         uint32_t bindingNumber = 0; // set your binding number
//         {
//             vk::VertexInputBindingDescription binding {};
//             binding.setBinding( bindingNumber );
//             binding.setInputRate( vk::VertexInputRate::eVertex ); // set your input rate
//             binding.setStride( sizeof( Template ) ); // set to this Class Name
//             desc.bindings.emplace_back( std::move( binding ) );
//         }

//         /// Attribute (Penginisialisasian attribute ini haruslah URUT seperti pada Shader nya)
//         uint32_t locationNumber = 0;
//         {
//             { // 
//                 vk::VertexInputAttributeDescription attribute {};
//                 attribute.setBinding( bindingNumber );
//                 attribute.setFormat(  ); // set your format, ex: vk::Format::eR32G32B32A32SFloat
//                 attribute.setLocation( locationNumber++ );
//                 attribute.setOffset( offsetof( Template, /*name of your field here*/ ) );
//                 desc.attributs.emplace_back( std::move(attribute) );
//             }
//             { // 
//                 vk::VertexInputAttributeDescription attribute {};
//                 attribute.setBinding( bindingNumber );
//                 attribute.setFormat(  ); // set your format, ex: vk::Format::eR32G32B32A32SFloat
//                 attribute.setLocation( locationNumber++ );
//                 attribute.setOffset( offsetof( Template, /*name of your field here*/ ) );
//                 desc.attributs.emplace_back( std::move(attribute) );
//             }
//         }

//         return desc;
//     }
// };

}
