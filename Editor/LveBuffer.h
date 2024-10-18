#pragma once

#include "LveDevice.h"

class LveBuffer {
	static VkDeviceSize getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

	LveDevice&     lveDevice;
	void*          mapped = nullptr;
	VkBuffer       buffer = VK_NULL_HANDLE;
	VkDeviceMemory memory = VK_NULL_HANDLE;

	VkDeviceSize          bufferSize;
	VkDeviceSize          instanceSize;
	u32                   instanceCount;
	VkDeviceSize          alignmentSize;
	VkBufferUsageFlags    usageFlags;
	VkMemoryPropertyFlags memoryPropertyFlags;

public:
	LveBuffer(
		LveDevice&            device,
		VkDeviceSize          instanceSize,
		u32                   instanceCount,
		VkBufferUsageFlags    usageFlags,
		VkMemoryPropertyFlags memoryPropertyFlags,
		VkDeviceSize          minOffsetAlignment = 1);
	~LveBuffer();

	LveBuffer(const LveBuffer&)            = delete;
	LveBuffer& operator=(const LveBuffer&) = delete;

	VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
	void     unmap();

	void                   writeToBuffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
	VkResult               flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
	VkDescriptorBufferInfo descriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
	VkResult               invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

	void                   writeToIndex(void* data, i32 index = 0);
	VkResult               flushIndex(i32 index);
	VkDescriptorBufferInfo descriptorInfoForIndex(i32 index);
	VkResult               invalidateIndex(i32 index);

	VkBuffer              getBuffer()              const { return buffer;              }
	void*                 getMappedMemory()        const { return mapped;              }
	u32                   getInstanceCount()       const { return instanceCount;       }
	VkDeviceSize          getInstanceSize()        const { return instanceSize;        }
	VkDeviceSize          getAlignmentSize()       const { return instanceSize;        }
	VkBufferUsageFlags    getUsageFlags()          const { return usageFlags;          }
	VkMemoryPropertyFlags getMemoryPropertyFlags() const { return memoryPropertyFlags; }
	VkDeviceSize          getBufferSize()          const { return bufferSize;          }
};
