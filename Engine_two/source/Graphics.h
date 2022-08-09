#pragma once

#include <string>

enum class DescriptorHeap
{
	SRV_UAV_CBV,
	Sampler,
	RTV,
	DSV,
	Num_types
};

enum class DescriptorHeapFlag
{
	None,
	ShaderVisible
};

struct DescriptorHeapMap
{
	std::string renderTargetView = "RenderTargetView";
	std::string depthStencilView = "DepthStencilView";
	
};
