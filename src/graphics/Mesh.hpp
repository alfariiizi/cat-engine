#pragma once

#include <unordered_map>
#include <iostream>

#include "AllocatedBuffer.hpp"
#include "ShaderStruct.hpp"
#include "DeletionQueue.hpp"

typedef Vertex::SimpleVertex vertex0;

// template<typename T>
class Mesh
{
public:
    Mesh() = default;

    /**
     * @brief Construct a new Mesh w/ CPU Only visibility
     * 
     * @param vertex actually this is a vertices, you get the idea right ?! :)
     * @param bufferInfo 
     * @param allocInfo 
     * @param pAllocator 
     */
    Mesh ( const std::vector<vertex0>& vertex, 
           const vk::BufferCreateInfo& bufferInfo, 
           const vma::AllocationCreateInfo& allocInfo, 
           const vma::Allocator* pAllocator = nullptr )
    {
        __vertices = vertex;
        assert( allocInfo.usage == vma::MemoryUsage::eCpuToGpu );

        if( pAllocator )
        {
            __buffer.createBuffer( *pAllocator, bufferInfo, allocInfo );
            allocateMesh( *pAllocator, __buffer );
        }
    }
    /**
     * @brief Construct a new Mesh object w/ GPU only visibility
     * 
     * @param device 
     * @param pool pool that used for one time submit cmdbuffer, so the pool must be have "transient" flag
     * @param queue 
     * @param vertex actually this is a vertices, you get the idea right ?! :)
     * @param bufferInfo 
     * @param allocInfo 
     * @param pAllocator 
     */
    Mesh ( const vk::Device& device,
           const vk::CommandPool& pool,
           vk::Queue queue,
           const std::vector<vertex0>& vertex, 
           vk::BufferCreateInfo& bufferInfo, 
           vma::AllocationCreateInfo& allocInfo, 
           const vma::Allocator* pAllocator = nullptr )
    {
        __vertices = vertex;
        
        // check that allocInfo must be eGpuOnly
        assert( allocInfo.usage == vma::MemoryUsage::eGpuOnly );

        if( pAllocator )
        {
            if( (bufferInfo.usage & vk::BufferUsageFlagBits::eTransferDst) == vk::BufferUsageFlags{0} ) {
                auto usage = bufferInfo.usage;
                bufferInfo.setUsage( usage | vk::BufferUsageFlagBits::eTransferDst );
            }

            // Buffer creation
            {
                __buffer.createBuffer( *pAllocator, bufferInfo, allocInfo );
            }

            // Stagging Buffer creation
            Buffer staggingBuffer;
            std::function<void ()> stagBuffDestroyer;
            {
                vk::BufferCreateInfo stagBuffInfo = bufferInfo;
                stagBuffInfo.setUsage( vk::BufferUsageFlagBits::eTransferSrc );
                vma::AllocationCreateInfo stagBuffAlloc = allocInfo;
                stagBuffAlloc.setUsage( vma::MemoryUsage::eCpuOnly );
                stagBuffDestroyer = staggingBuffer.createBuffer( *pAllocator, stagBuffInfo, stagBuffAlloc );
            }

            // Allocating Mesh with staggingBuffer
            allocateMesh( *pAllocator, staggingBuffer );

            // Copying from staggingBuffer to the Buffer
            {
                auto cmdBufferInfo = vk::CommandBufferAllocateInfo {
                    pool,
                    vk::CommandBufferLevel::ePrimary,
                    1
                };
                auto cmdBuffer = device.allocateCommandBuffers( cmdBufferInfo ).front();
                
                auto beginInfo = vk::CommandBufferBeginInfo{
                    vk::CommandBufferUsageFlagBits::eOneTimeSubmit
                };
                
                // Begin recording
                cmdBuffer.begin( beginInfo );
                
                vk::BufferCopy bufferCopy {};
                bufferCopy.setSrcOffset( vk::DeviceSize{0} );
                bufferCopy.setDstOffset( vk::DeviceSize{0} );
                bufferCopy.setSize( bufferInfo.size );
                cmdBuffer.copyBuffer( staggingBuffer.buffer(), __buffer.buffer(), bufferCopy );

                // End recording
                cmdBuffer.end();

                vk::SubmitInfo submitInfo {};
                submitInfo.setCommandBuffers( cmdBuffer );
                queue.submit( submitInfo );
                queue.waitIdle();

                device.freeCommandBuffers( pool, cmdBuffer );
            }

            stagBuffDestroyer();
        }
    }
    void allocateMesh( const vma::Allocator& allocator, Buffer& buffer )
    {
        void* ptr = allocator.mapMemory( buffer.allocation() );
        memcpy( ptr, __vertices.data(), __vertices.size() * sizeof(vertex0) );
        allocator.unmapMemory( buffer.allocation() );
    }
    Buffer& buffer() { return __buffer; };
    std::vector<vertex0>& vertices() { return __vertices; };
    // int& ID() { return __ID; };

private:
    Buffer __buffer;
    // std::vector<void*> __vertices;
    std::vector<vertex0> __vertices;
    // void* __structPtr;
    // int __ID;
};


using namespace Vertex;
class Meshes
{
public:
    Meshes() {};
    void loadAllMeshes()
    {
        /// Meshes 1
        {

            // _allocations.emplace_back();
        }

        /// Meshes 2
        {

            // _allocations.emplace_back();
        }

        /// Meshes 3
        {

            // _allocations.emplace_back();
        }

        /// Meshes 4
        {

            // _allocations.emplace_back();
        }

    }

    template<typename T>
    void regToAlloc( Mesh mesh )
    {
        _allocations.emplace_back( mesh.buffer().allocation() );
        _trackers.emplace_back( std::make_pair<int, int>( IDs++, sizeof(T) ) );
        __delQueue.pushFunction( [&](){
            __allocator.destroyBuffer( mesh.buffer().buffer(), mesh.buffer().allocation() );
        });
    }

public:
    std::vector<vma::Allocation> _allocations;
    std::vector<std::pair<int, int>> _trackers;  // first == ID, second == sizeof(type)
    int IDs = 0;

private:
    vma::Allocator __allocator;
    DeletionQueue __delQueue;
};


namespace obj
{
bool loadFromObj( const std::string& fileName, Mesh& mesh );
}