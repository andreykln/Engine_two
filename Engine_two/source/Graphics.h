#pragma once
#include <wrl.h>
#include <unordered_map>
#include <cassert>
#include <memory>
#include <string>
#include "spdlog/spdlog.h"

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
