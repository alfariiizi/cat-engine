#pragma once

#include "Image.hpp"

class Depth : public Image
{
public:
    void create() override;
    void initDepth( vk::Extent3D extent );

public:
    const vk::Format&       getFormat();
    const vk::Extent3D&     getExtent();

public:
    bool _hasInitDepth_ = false;

private:
    vk::Format      _format_     = vk::Format::eD32Sfloat;
    vk::Extent3D    _extent_;
};