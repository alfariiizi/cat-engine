#pragma once

#include <unordered_map>

#include "AllocatedBuffer.hpp"
#include "ShaderStruct.hpp"
#include "DeletionQueue.hpp"

typedef Vertex::SimpleVertex vertex0;

// template<typename T>
class Mesh
{
public:
    Mesh() = default;
    Mesh ( const std::vector<vertex0>& vertex, 
           const vk::BufferCreateInfo& bufferInfo, 
           const vma::AllocationCreateInfo& allocInfo, 
           const vma::Allocator* pAllocator = nullptr )
    {
        __vertices = vertex;

        if( pAllocator )
        {
            __buffer.createBuffer( *pAllocator, bufferInfo, allocInfo );
            allocateMesh( *pAllocator );
        }
    }
    void allocateMesh( const vma::Allocator& allocator )
    {
        void* ptr = allocator.mapMemory( __buffer.allocation() );
        memcpy( ptr, __vertices.data(), __vertices.size() * sizeof(vertex0) );
        allocator.unmapMemory( __buffer.allocation() );
    }
    Buffer& buffer() { return __buffer; };
    std::vector<vertex0>& vertices() { return __vertices; };
    // int& ID() { return __ID; };

private:
    Buffer __buffer;
    // std::vector<void*> __vertices;
    std::vector<vertex0> __vertices;
    void* __structPtr;
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