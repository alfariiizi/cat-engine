#include "Renderpass.hpp"


Renderpass::~Renderpass() 
{
    _delQueue_.flush();
}

// Renderpass::State Renderpass::getAllMembers() 
// {
//     assert( _hasBeenInitialized_ && _hasBeenCreated_ );

//     return { getRenderpass(), getFramebuffer() };
// }

void Renderpass::init( vk::PhysicalDevice physicalDevice, const vk::Device& device, const vma::Allocator& allocator, Swapchain& swapchain, bool useDepth ) 
{
    _physicalDevice_ = physicalDevice;
    _device_ = device;
    _pSwapchain_ = &swapchain;


    if( useDepth )
    {
        // _depth_.init( device, allocator );
        // _depth_.initDepth( vk::Extent3D{ swapchain.getExtent(), 1 } );

        vk::Extent2D extent = _pSwapchain_->getExtent();
        auto depth = vku::DepthStencilImage{ _device_, 
            _physicalDevice_.getMemoryProperties(),
            extent.width, extent.height,
            vk::Format::eD32Sfloat };
        
        // _depth_ = depth.
    }
    
    _useDepth_ = useDepth;

    _hasBeenInitialized_ = true;
}

void Renderpass::create() 
{
    assert( _hasBeenInitialized_ );

    // _depth_.create();

    /**
     * @brief the renderpass will use this COLOR attachment.
     */
    vk::AttachmentDescription colorAttachment {};
    //the attachment will have the format needed by the swapchain
    colorAttachment.setFormat           ( _pSwapchain_->getFormat() );
    //1 sample, we won't be doing MSAA
    colorAttachment.setSamples          ( vk::SampleCountFlagBits::e1 );
    // we Clear when this attachment is loaded
    colorAttachment.setLoadOp           ( vk::AttachmentLoadOp::eClear );
    // we keep the attachment stored when the renderpass ends
    colorAttachment.setStoreOp          ( vk::AttachmentStoreOp::eStore );
    //we don't care about stencil
    colorAttachment.setStencilLoadOp    ( vk::AttachmentLoadOp::eDontCare );
    colorAttachment.setStencilStoreOp   ( vk::AttachmentStoreOp::eDontCare );
    //we don't know or care about the starting layout of the attachment
    colorAttachment.setInitialLayout    ( vk::ImageLayout::eUndefined );
    //after the renderpass ends, the image has to be on a layout ready for display
    colorAttachment.setFinalLayout      ( vk::ImageLayout::ePresentSrcKHR );

    /**
     * @brief COLOR Attachment Reference, this is needed for creating Subpass
     */
    vk::AttachmentReference colorAttachmentRef {};
    //attachment number will index into the pAttachments array in the parent renderpass itself
    colorAttachmentRef.setAttachment( 0 );
    // this layout 'll be use during subpass that use this attachment ref
    colorAttachmentRef.setLayout( vk::ImageLayout::eColorAttachmentOptimal );

    /**
     * @brief the renderpass will use this DEPTH attachment.
     */
    vk::AttachmentDescription depthAttachment {};
    depthAttachment.setFormat( _depth_.format() );
    depthAttachment.setSamples( vk::SampleCountFlagBits::e1 );  // we won't use fancy sample for right now
    depthAttachment.setLoadOp( vk::AttachmentLoadOp::eClear );
    depthAttachment.setStoreOp( vk::AttachmentStoreOp::eStore );
    depthAttachment.setStencilLoadOp( vk::AttachmentLoadOp::eClear );
    depthAttachment.setStencilStoreOp( vk::AttachmentStoreOp::eDontCare );
    depthAttachment.setInitialLayout( vk::ImageLayout::eUndefined );
    depthAttachment.setFinalLayout( vk::ImageLayout::eDepthStencilAttachmentOptimal );

    /**
     * @brief DEPTH Attachment Reference, this is needed for creating Subpass
     */
    vk::AttachmentReference depthAttachmentRef {};
    depthAttachmentRef.setAttachment( 1 );
    depthAttachmentRef.setLayout( vk::ImageLayout::eDepthStencilAttachmentOptimal );


    /**
     * @brief Subpass and array of attachment
     */
    std::vector<vk::AttachmentDescription> attachDescs;
    vk::SubpassDescription subpass {};
    // this bind point could be graphics, compute, or maybe ray tracing
    subpass.setPipelineBindPoint( vk::PipelineBindPoint::eGraphics );
    attachDescs.push_back( colorAttachment );
    subpass.setColorAttachments( colorAttachmentRef );

    if( _useDepth_ )
    {
        subpass.setPDepthStencilAttachment( &depthAttachmentRef );
        attachDescs.push_back( depthAttachment );
    }

    vk::RenderPassCreateInfo renderpassInfo {};
    renderpassInfo.setAttachments( attachDescs );
    renderpassInfo.setSubpasses( subpass );

    _renderpass_ = _device_.createRenderPass( renderpassInfo );
    _delQueue_.pushFunction( [&]()
    {
        _device_.destroyRenderPass( _renderpass_ );
    });

    _depth_.

    /**
     * Sekalian create framebuffer nya
    */
    createFramebuffer();

    _hasBeenCreated_ = true;
}

void Renderpass::destroy() 
{
    assert( _hasBeenCreated_ );

    // for( const auto& fb : _uFramebuffers_ )
    //     _device_.destroyFramebuffer( fb );
    
    // _device_.destroyRenderPass( _renderpass_ );
    // _depth_.destroy();

    _hasBeenCreated_ = false;
}

void Renderpass::createFramebuffer() 
{
    _uFramebuffers_.reserve( _pSwapchain_->getImages().size() );

    for( const auto& view : _pSwapchain_->getImageViews() )
    {
        // this must be the same order as attachments on create renderpass
        // (in this case, normal image view first, then the depth image view)
        std::vector<vk::ImageView> attachments = { view };
        if( _useDepth_ )
            attachments.emplace_back( _depth_.imageView() );

        vk::FramebufferCreateInfo fbInfo {};
        fbInfo.setAttachments( attachments );
        fbInfo.setWidth( _pSwapchain_->getExtent().width );
        fbInfo.setHeight( _pSwapchain_->getExtent().height );
        fbInfo.setRenderPass( _renderpass_ );
        fbInfo.setLayers( 1 );

        _uFramebuffers_.emplace_back( _device_.createFramebufferUnique( fbInfo ) );
    }
}

const vk::RenderPass& Renderpass::getRenderpass() 
{
    assert( _hasBeenInitialized_ && _hasBeenCreated_ );
    return _renderpass_;
}

// const Depth& Renderpass::getDepth() 
// {
//     assert( _hasBeenInitialized_ && _hasBeenCreated_ );
//     return _depth_;
// }

std::vector<vk::Framebuffer> Renderpass::getFramebuffer() 
{
    assert( _hasBeenInitialized_ && _hasBeenCreated_ );

    std::vector<vk::Framebuffer> fbs;
    fbs.reserve( _uFramebuffers_.size() );

    for( auto& fb : _uFramebuffers_ )
        fbs.emplace_back( fb.get() );

    return fbs;
}
