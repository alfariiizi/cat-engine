#pragma once

#include <unordered_map>

#include "AllocatedBuffer.hpp"
#include "ShaderStruct.hpp"
#include "DeletionQueue.hpp"

template<typename T>
class Mesh
{
public:
    // Mesh() = default;
    // Mesh ( const std::vector<Vertex::SimpleVertex>& vertex, 
    //        const vk::BufferCreateInfo& bufferInfo, 
    //        const vma::AllocationCreateInfo& allocInfo, 
    //        const vma::Allocator* pAllocator = nullptr )
    // {
    //     __vertices = vertex;

    //     if( pAllocator )
    //     {
    //         __buffer.createBuffer( *pAllocator, bufferInfo, allocInfo );
    //         allocateMesh( *pAllocator );
    //     }
    // }
    // void allocateMesh( const vma::Allocator& allocator )
    // {
    //     // void* ptr = allocator.mapMemory( _buffer.allocation() );
    //     // memcpy( ptr, _vertices.data(), _vertices.size() * sizeof(T) );
    //     // allocator.unmapMemory( _buffer.allocation() );
    // }
    Buffer& buffer() { return __buffer; };
    std::vector<Vertex::SimpleVertex>& vertices() { return __vertices; };
    // int& ID() { return __ID; };

private:
    Buffer __buffer;
    std::vector<T> __vertices;
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
    void regToAlloc( Mesh<T> mesh )
    {
        _allocations.emplace_back( mesh.buffer().allocation() );
        _trackers.emplace_back( std::make_pair<int, int>( IDs++, sizeof(T) ) );
        __delQueue.pushFunction( [&](){
            __allocator.destroyBuffer( mesh.buffer().buffer(), mesh.buffer().allocation() );
        });
    }

public:
    // Mesh<Vertex::SimpleVertex> __simpleVertex;
    // Mesh _simpleVertex;
    std::vector<vma::Allocation> _allocations;
    std::vector<std::pair<int, int>> _trackers;  // first == ID, second == sizeof(type)
    int IDs = 0;

private:
    vma::Allocator __allocator;
    DeletionQueue __delQueue;
};