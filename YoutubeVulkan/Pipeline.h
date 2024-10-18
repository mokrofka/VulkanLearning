#pragma once
#include "defines.h"
#include "Device.h"

#include <string>
#include <vector>

struct PipelineConfigInfo {
	VkViewport                             viewport;
	VkRect2D                               scissor;
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
	VkPipelineRasterizationStateCreateInfo rasterizationInfo;
	VkPipelineMultisampleStateCreateInfo   multisampleInfo;
	VkPipelineColorBlendAttachmentState    colorBlendAttachment;
	VkPipelineColorBlendStateCreateInfo    colorBlendInfo;
	VkPipelineDepthStencilStateCreateInfo  depthStencilInfo;
	VkPipelineLayout                       pipelineLayout = nullptr;
	VkRenderPass                           renderPass     = nullptr;

	u32 subpass = 0;
};

class Pipeline {
	Device&        device;
	VkPipeline     graphicsPipeline;
	VkShaderModule vertShaderModule;
	VkShaderModule fragShaderModule;

	void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
	void createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath,
	                            const PipelineConfigInfo& configInfo);

	static std::vector<char> readFile(const std::string& filepath);

public:
	Pipeline(Device& device, const std::string& vertFilepath,
	         const std::string& fragFilepath, const PipelineConfigInfo& configInfo);
	~Pipeline();

	void bind(VkCommandBuffer commandBuffer);

	static PipelineConfigInfo defaultPipelineConfigInfo(u32 width, u32 height);

	Pipeline(const Pipeline&)       = delete;
	void operator=(const Pipeline&) = delete;
};
