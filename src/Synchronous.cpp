#include "Synchronous.hpp"

void Synchronous::init( const vk::Device& device, Synchronous::Type type, vk::FenceCreateFlagBits fenceFlag ) 
{
    _pDevice_ = &device;
    _type_ = type;
    _fenceFlag_ = fenceFlag;    // it only use if the type has fence

    _hasBeenInit_ = true;
}

void Synchronous::create() 
{
    assert( _hasBeenInit_ );

    switch( _type_ )
    {
    case Synchronous::Type::ONLY_FENCE :
        _fence_ = _pDevice_->createFence( vk::FenceCreateInfo{ _fenceFlag_ } );
        break;
    case Synchronous::Type::ONLY_SEMAPHORE :
        _semaphore_ = _pDevice_->createSemaphore( {} );
        break;
    case Synchronous::Type::FENCE_AND_SEMAPHORE :
        _fence_ = _pDevice_->createFence( vk::FenceCreateInfo{ _fenceFlag_ } );
        _semaphore_ = _pDevice_->createSemaphore( {} );
        break;
    default:
        break;
    }

    _hasBeenCreated_ = true;
}

void Synchronous::destroy() 
{
    assert( _hasBeenCreated_ );
    _pDevice_->destroyFence( _fence_ );
    _pDevice_->destroySemaphore( _semaphore_ );
}

vk::Fence& Synchronous::getFence() 
{
    assert( _hasBeenCreated_ );
    return _fence_;
}

vk::Semaphore& Synchronous::getSemaphore() 
{
    assert( _hasBeenCreated_ );
    return _semaphore_;
}
