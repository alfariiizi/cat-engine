#include "Material.hpp"
#include <stdio.h>

#include "ShaderStruct.hpp"


Material::~Material() 
{
    destroy();
}

void Material::init( vk::PhysicalDevice physicalDevice, vk::Device device, vk::RenderPass renderpass, uint32_t width, uint32_t height )
{
    _physicalDevice_ = physicalDevice;
    _device_ = device;
    _renderpass_ = renderpass;
    _width_ = width;
    _height_ = height;

    _vertexShaderPath_ = getenv("PWD");
    _vertexShaderPath_.append( "/shaders/spv/");
    _fragmentShaderPath_ = _vertexShaderPath_;
    _vertexShaderPath_.append( "vertex" );
    _fragmentShaderPath_.append( "fragment" );


    trianglePipeline();

    _isInitialized_ = true;
    _stillExist_ = true;
}

void Material::destroy() 
{
    assert( _isInitialized_ );

    if( _stillExist_ )
    {
        for( auto& s : _s_ )
        {
            _device_.destroyPipeline( s._pipeline );
            _device_.destroyPipelineLayout( s._layout );
            // _device_.destroyDescriptorSetLayout( s._setLayout );
        }
    }

    _stillExist_ = false;
}

void Material::trianglePipeline() 
{
    /// State (FIX)
    State state;
    vku::PipelineMaker plm { _width_, _height_ };

    /// Descriptor set layout (CHANGEABLE)
    {
    }

    /// Layout (CHANGEABLE)
    {
        auto layoutMaker = vku::PipelineLayoutMaker{};
        state._layout = layoutMaker.createUnique( _device_ ).release();
    }


    /// Shader (CHANGEABLE)
    std::string filename = "simpletriangle";

    filename.append(".spv");
    std::string vertFilename = _vertexShaderPath_ + "/" + filename;
    std::string fragFilename = _fragmentShaderPath_ + "/" + filename;

    vku::ShaderModule vertexShader { _device_, vertFilename };
    vku::ShaderModule fragmentShader { _device_, fragFilename };
    if( !vertexShader.ok() || !fragmentShader.ok() )
        throw std::runtime_error( "FAILED: creating shaders" );
    plm.shader( vk::ShaderStageFlagBits::eVertex, vertexShader );
    plm.shader( vk::ShaderStageFlagBits::eFragment, fragmentShader );


    /// Pipeline State (CHANGEABLE)
    auto vertexDesc = Vertex::getVertexInputDescription();
    {
        // Depth
        plm.depthTestEnable( VK_TRUE ).depthWriteEnable( VK_TRUE );

        plm.vertexBinding( vertexDesc.bindings.front() );
        // for( auto& bind : vertexDesc.bindings )
        //     plm.vertexBinding( bind );

        for( auto& attr : vertexDesc.attributs )
            plm.vertexAttribute( attr );
    }

    /// Create the pipeline (FIX)
    state._pipeline = plm.createUnique( _device_, nullptr, state._layout, _renderpass_ ).release();

    /// Naming the pipeline (CHANGEABLE)
    state._name = "triangle";

    /// Copying the pipeline (FIX)
    _s_.emplace_back( std::move(state) );
}

std::vector<Material::State>::iterator Material::getMaterial(const std::string& name) 
{
    assert( _isInitialized_ );
    auto it = std::find_if( _s_.begin(), _s_.end(), 
                [&]( const State& state ){
                    return strcmp( state._name.c_str(), name.c_str() ) == 0;
                }
    );

    assert( it != _s_.end() );

    return it;
}
