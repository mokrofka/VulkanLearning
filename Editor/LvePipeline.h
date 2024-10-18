#pragma once
#include "defines.h"
#include "LveDevice.h"

#include <string>
#include <vector>

struct PipelineConfigInfo {
	PipelineConfigInfo()                                     = default;
	PipelineConfigInfo(const PipelineConfigInfo&)            = delete;
	PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

	std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
	VkPipelineViewportStateCreateInfo      viewportInfo;
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
	VkPipelineRasterizationStateCreateInfo rasterizationInfo;
	VkPipelineMultisampleStateCreateInfo   multisampleInfo;
	VkPipelineColorBlendAttachmentState    colorBlendAttachment;
	VkPipelineColorBlendStateCreateInfo    colorBlendInfo;
	std::vector<VkDynamicState>            dynamicStateEnables;
	VkPipelineDynamicStateCreateInfo       dynamicStateInfo;
	VkPipelineDepthStencilStateCreateInfo  depthStencilInfo;
	VkPipelineLayout                       pipelineLayout = nullptr;
	VkRenderPass                           renderPass     = nullptr;
	u32                                    subpass        = 0;
};

class LvePipeline {
	LveDevice&     device;
	VkPipeline     graphicsPipeline;
	VkShaderModule vertShaderModule;
	VkShaderModule fragShaderModule;

	void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);
	void createGraphicsPipeline(const std::string&        vertFilepath, const std::string& fragFilepath,
	                            const PipelineConfigInfo& configInfo);

	static std::vector<char> readFile(const std::string& shaderName);

public:
	LvePipeline(LveDevice&         device, const std::string&              vertFilepath,
	            const std::string& fragFilepath, const PipelineConfigInfo& configInfo);
	~LvePipeline();

	void bind(VkCommandBuffer commandBuffer);

	static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);
	static void enableAlphaBlending(PipelineConfigInfo& configInfo);

	LvePipeline(const LvePipeline&)            = delete;
	LvePipeline& operator=(const LvePipeline&) = delete;
};
