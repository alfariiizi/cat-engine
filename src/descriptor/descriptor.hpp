#pragma once

#include <vulkan/vulkan.hpp>

class DescriptorAllocator
{
public:
    struct PoolSizes
    {
        std::vector<std::pair<vk::DescriptorType, float>> sizes = 
        {
            { vk::DescriptorType::eSampler, 0.5f },
            { vk::DescriptorType::eCombinedImageSampler, 4.0f },
            { vk::DescriptorType::eSampledImage, 4.0f },
            { vk::DescriptorType::eStorageImage, 0.5f },
            { vk::DescriptorType::eUniformTexelBuffer, 0.5f },
            { vk::DescriptorType::eStorageTexelBuffer, 0.5f },
            { vk::DescriptorType::eUniformBuffer, 0.5f },
            { vk::DescriptorType::eStorageBuffer, 0.5f },
            { vk::DescriptorType::eUniformTexelBuffer, 0.5f },
            { vk::DescriptorType::eStorageTexelBuffer, 0.5f },
            { vk::DescriptorType::eInputAttachment, 0.5f }
        };
    };

public:
    void resetPool();
    bool allocate( vk::DescriptorSet* set, vk::DescriptorSetLayout setLayout );
    void init( vk::Device device );
    void cleanup();

private:
    vk::DescriptorPool grabPool();

public:
    vk::Device _device;

private:
    vk::DescriptorPool __currentPool;
};