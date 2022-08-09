#pragma once
#include <d3d11.h>

#include "Device.h"



class Device_DirectX11 : public Device
{
public:
    void CreateDebugAndFactory() override;
    void CreateCommandObjects() override;
    void CreateSwapChain() override;
    void CreateDescriptorHeap(DescriptorHeap type, UINT numDesc, DescriptorHeapFlag flag, std::string name) override;

};


class dx11
{
};

