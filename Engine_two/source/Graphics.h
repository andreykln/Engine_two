#pragma once
#include <wrl.h>
#include <unordered_map>
#include <cassert>
#include <memory>
#include <string>
#include <DirectXColors.h>
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

struct Resource
{
	std::shared_ptr<void> internal_state;
	bool IsValid() const { return internal_state.get() != nullptr; }
};

