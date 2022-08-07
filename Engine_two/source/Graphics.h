#pragma once

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