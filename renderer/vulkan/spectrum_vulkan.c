#include "spectrum_vulkan.h"
#include "../../math/vec3.h"
#include "../../filesystem/obj.h"

internal void import_obj(vertex **vertex_buffer, uint16_t **index_buffer, uint32_t *total_vertices,
		uint16_t *total_indices)
{
	char const *filename = "assets/monkey_smooth.obj";
	ObjResult obj_result = {0};
	ObjInfo obj_info = {0};
	read_obj_file(filename, &obj_result, &obj_info);

	*total_vertices = obj_result.vertex_buffer_size;
	*total_indices = (uint16_t)obj_result.index_buffer_size;

	*vertex_buffer = (vertex*)malloc(obj_result.vertex_buffer_size * sizeof(vertex));
	*index_buffer = (uint16_t*)malloc(obj_result.index_buffer_size * sizeof(uint16_t));

	for(uint32_t i = 0; i < obj_result.vertex_buffer_size; ++i)
	{
		vertex v = {
			.position = {
				.x = obj_result.vertex_buffer[i].point.x,
				.y = -obj_result.vertex_buffer[i].point.y, 
				.z = obj_result.vertex_buffer[i].point.z
			},
			.base_color = { 
				.x = obj_result.vertex_buffer[i].normal.x,
				.y = obj_result.vertex_buffer[i].normal.y,
				.z = obj_result.vertex_buffer[i].normal.z
			},
			.normal = {
				.x = obj_result.vertex_buffer[i].normal.x,
				.y = obj_result.vertex_buffer[i].normal.y,
				.z = obj_result.vertex_buffer[i].normal.z
			}
		};
		(*vertex_buffer)[i] = v;
		// vk_renderer->vertices[i].texture_coordinates.x = vertex_buffer[i].texture_coordinate.u;
		// vk_renderer->vertices[i].texture_coordinates.y = 1.0f - vertex_buffer[i].texture_coordinate.v;
	}

	for(uint32_t i = 0; i < obj_result.index_buffer_size; ++i)
	{
		(*index_buffer)[i] = ((uint16_t*)obj_result.index_buffer)[i];
	}

	free(obj_result.data);
}

// TODO: Move it elsewhere.
internal void create_rectangle(vertex **vertex_buffer, uint16_t **index_buffer, uint32_t *total_vertices,
		uint16_t *total_indices)
{
	uint32_t indices_per_rectangle = 6;
	uint32_t vertices_per_rectangle = 4;
	uint32_t number_of_rectangles = 1;

	*total_vertices = vertices_per_rectangle * number_of_rectangles;
	*total_indices = (uint16_t)(indices_per_rectangle * number_of_rectangles);

	*vertex_buffer = (vertex*)malloc(vertices_per_rectangle * number_of_rectangles * sizeof(vertex));
	*index_buffer = (uint16_t*)malloc(indices_per_rectangle * number_of_rectangles * sizeof(uint16_t));

    vec3_t color = {.x = 0.5f, .y =  0.5f, .z = 0.5f};

	vertex v1 = {
		.position = {.x = -1.0f, .y = -1.0f, .z = 0.0f},
		.base_color = color,
		.normal = { .x = 0.0f, .y = 0.0f, .z = 0.0f}
	};
	
	vertex v2 = {
		.position = {.x = 1.0f, .y = -1.0f, .z = 0.0f},
		.base_color = color,
		.normal = { .x = 0.0f, .y = 0.0f, .z = 0.0f}
	};

	vertex v3 = {
		.position = {.x = 1.0f, .y = 1.0f, .z = 0.0f},
		.base_color = color,
		.normal = { .x = 0.0f, .y = 0.0f, .z = 0.0f}
	};

	vertex v4 = {
		.position = {.x = -1.0f, .y = 1.0f, .z = 0.0f},
		.base_color = color,
		.normal = { .x = 0.0f, .y = 0.0f, .z = 0.0f}
	};

	// Counter clockwise
	// v1 top left
	// v2 top right
	// v3 bottom right
	// v4 bottom left
	
    (*vertex_buffer)[0] = v1;
    (*vertex_buffer)[1] = v2;
    (*vertex_buffer)[2] = v3;
    (*vertex_buffer)[3] = v4;

    (*index_buffer)[0] = 0;
    (*index_buffer)[1] = 3;
    (*index_buffer)[2] = 1;
    (*index_buffer)[3] = 1;
    (*index_buffer)[4] = 3;
    (*index_buffer)[5] = 2;
}

const char *device_extensions_list[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

const char *instance_layers_list[] = {
    "VK_LAYER_KHRONOS_validation"
};

VK_EXPORTED_FUNCTION( vkGetInstanceProcAddr )
#undef VK_EXPORTED_FUNCTION

#if _WIN32
#include <vulkan/vulkan_win32.h>

const char *instance_extensions_list[] = {
    VK_KHR_SURFACE_EXTENSION_NAME,
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME
};

VK_INSTANCE_LEVEL_FUNCTION( vkCreateWin32SurfaceKHR ) // VK_KHR_WIN32_SURFACE_EXTENSION_NAME

internal VkResult load_vulkan(void)
{
    HMODULE library = LoadLibraryA("vulkan-1.dll");
    if (!library)
        return VK_ERROR_INITIALIZATION_FAILED;

    vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)(void(*)(void))GetProcAddress(library, "vkGetInstanceProcAddr");
    if(!vkGetInstanceProcAddr)
    {
        FreeLibrary(library);
        return VK_ERROR_INITIALIZATION_FAILED;
    }
    return VK_SUCCESS;
}

#endif

VK_GLOBAL_LEVEL_FUNCTION( vkEnumerateInstanceVersion )
VK_GLOBAL_LEVEL_FUNCTION( vkEnumerateInstanceExtensionProperties )
VK_GLOBAL_LEVEL_FUNCTION( vkEnumerateInstanceLayerProperties )
VK_GLOBAL_LEVEL_FUNCTION( vkCreateInstance )
#undef VK_GLOBAL_LEVEL_FUNCTION

VK_INSTANCE_LEVEL_FUNCTION( vkGetDeviceProcAddr )
VK_INSTANCE_LEVEL_FUNCTION( vkDestroyInstance )
VK_INSTANCE_LEVEL_FUNCTION( vkEnumeratePhysicalDevices )
VK_INSTANCE_LEVEL_FUNCTION( vkGetPhysicalDeviceProperties )
VK_INSTANCE_LEVEL_FUNCTION( vkGetPhysicalDeviceQueueFamilyProperties )

VK_INSTANCE_LEVEL_FUNCTION( vkCreateDebugUtilsMessengerEXT ) // VK_EXT_DEBUG_UTILS_EXTENSION_NAME
VK_INSTANCE_LEVEL_FUNCTION( vkDestroyDebugUtilsMessengerEXT ) // VK_EXT_DEBUG_UTILS_EXTENSION_NAME

VK_INSTANCE_LEVEL_FUNCTION( vkDestroySurfaceKHR ) // VK_KHR_SURFACE_EXTENSION_NAME
VK_INSTANCE_LEVEL_FUNCTION( vkGetPhysicalDeviceSurfaceSupportKHR ) // VK_KHR_SURFACE_EXTENSION_NAME
VK_INSTANCE_LEVEL_FUNCTION( vkGetPhysicalDeviceSurfaceCapabilitiesKHR ) // VK_KHR_SURFACE_EXTENSION_NAME
VK_INSTANCE_LEVEL_FUNCTION( vkGetPhysicalDeviceSurfaceFormatsKHR ) // VK_KHR_SURFACE_EXTENSION_NAME
VK_INSTANCE_LEVEL_FUNCTION( vkGetPhysicalDeviceSurfacePresentModesKHR ) // VK_KHR_SURFACE_EXTENSION_NAME
VK_INSTANCE_LEVEL_FUNCTION( vkCreateDevice )
VK_INSTANCE_LEVEL_FUNCTION( vkGetPhysicalDeviceMemoryProperties )
VK_INSTANCE_LEVEL_FUNCTION( vkGetPhysicalDeviceFeatures )
VK_INSTANCE_LEVEL_FUNCTION( vkGetPhysicalDeviceFormatProperties )
#undef VK_INSTANCE_LEVEL_FUNCTION

VK_DEVICE_LEVEL_FUNCTION( vkGetDeviceQueue )
VK_DEVICE_LEVEL_FUNCTION( vkDestroyDevice )
VK_DEVICE_LEVEL_FUNCTION( vkDeviceWaitIdle )
VK_DEVICE_LEVEL_FUNCTION( vkCreateSwapchainKHR ) // VK_KHR_swapchain.
VK_DEVICE_LEVEL_FUNCTION( vkGetSwapchainImagesKHR ) // VK_KHR_swapchain.
VK_DEVICE_LEVEL_FUNCTION( vkDestroySwapchainKHR ) // VK_KHR_swapchain.
VK_DEVICE_LEVEL_FUNCTION( vkQueuePresentKHR ) // VK_KHR_swapchain
VK_DEVICE_LEVEL_FUNCTION( vkAcquireNextImageKHR ) // VK_KHR_swapchain
VK_DEVICE_LEVEL_FUNCTION( vkCreateImageView )
VK_DEVICE_LEVEL_FUNCTION( vkDestroyImageView )
VK_DEVICE_LEVEL_FUNCTION( vkCreateRenderPass )
VK_DEVICE_LEVEL_FUNCTION( vkDestroyRenderPass )
VK_DEVICE_LEVEL_FUNCTION( vkCreateFramebuffer )
VK_DEVICE_LEVEL_FUNCTION( vkDestroyFramebuffer )
VK_DEVICE_LEVEL_FUNCTION( vkCreateGraphicsPipelines )
VK_DEVICE_LEVEL_FUNCTION( vkDestroyPipeline )
VK_DEVICE_LEVEL_FUNCTION( vkCreateShaderModule )
VK_DEVICE_LEVEL_FUNCTION( vkDestroyShaderModule )
VK_DEVICE_LEVEL_FUNCTION( vkCreatePipelineLayout )
VK_DEVICE_LEVEL_FUNCTION( vkDestroyPipelineLayout )
VK_DEVICE_LEVEL_FUNCTION( vkCreateCommandPool )
VK_DEVICE_LEVEL_FUNCTION( vkDestroyCommandPool )
VK_DEVICE_LEVEL_FUNCTION( vkAllocateCommandBuffers )
VK_DEVICE_LEVEL_FUNCTION( vkResetCommandBuffer )
VK_DEVICE_LEVEL_FUNCTION( vkBeginCommandBuffer )
VK_DEVICE_LEVEL_FUNCTION( vkCmdBeginRenderPass )
VK_DEVICE_LEVEL_FUNCTION( vkCmdBindPipeline )
VK_DEVICE_LEVEL_FUNCTION( vkCmdDraw )
VK_DEVICE_LEVEL_FUNCTION( vkCmdEndRenderPass )
VK_DEVICE_LEVEL_FUNCTION( vkEndCommandBuffer )
VK_DEVICE_LEVEL_FUNCTION( vkCreateSemaphore )
VK_DEVICE_LEVEL_FUNCTION( vkDestroySemaphore )
VK_DEVICE_LEVEL_FUNCTION( vkCreateFence )
VK_DEVICE_LEVEL_FUNCTION( vkDestroyFence )
VK_DEVICE_LEVEL_FUNCTION( vkWaitForFences )
VK_DEVICE_LEVEL_FUNCTION( vkResetFences )
VK_DEVICE_LEVEL_FUNCTION( vkQueueSubmit )
VK_DEVICE_LEVEL_FUNCTION( vkCmdSetViewport )
VK_DEVICE_LEVEL_FUNCTION( vkCmdSetScissor )
VK_DEVICE_LEVEL_FUNCTION( vkCreateBuffer )
VK_DEVICE_LEVEL_FUNCTION( vkGetBufferMemoryRequirements )
VK_DEVICE_LEVEL_FUNCTION( vkAllocateMemory )
VK_DEVICE_LEVEL_FUNCTION( vkBindBufferMemory )
VK_DEVICE_LEVEL_FUNCTION( vkMapMemory )
VK_DEVICE_LEVEL_FUNCTION( vkUnmapMemory )
VK_DEVICE_LEVEL_FUNCTION( vkDestroyBuffer )
VK_DEVICE_LEVEL_FUNCTION( vkFreeMemory )
VK_DEVICE_LEVEL_FUNCTION( vkCmdBindVertexBuffers )
VK_DEVICE_LEVEL_FUNCTION( vkCmdCopyBuffer )
VK_DEVICE_LEVEL_FUNCTION( vkQueueWaitIdle )
VK_DEVICE_LEVEL_FUNCTION( vkFreeCommandBuffers )
VK_DEVICE_LEVEL_FUNCTION( vkCmdBindIndexBuffer )
VK_DEVICE_LEVEL_FUNCTION( vkCmdDrawIndexed )
VK_DEVICE_LEVEL_FUNCTION( vkCreateDescriptorSetLayout )
VK_DEVICE_LEVEL_FUNCTION( vkDestroyDescriptorSetLayout )
VK_DEVICE_LEVEL_FUNCTION( vkCreateDescriptorPool )
VK_DEVICE_LEVEL_FUNCTION( vkAllocateDescriptorSets )
VK_DEVICE_LEVEL_FUNCTION( vkDestroyDescriptorPool )
VK_DEVICE_LEVEL_FUNCTION( vkUpdateDescriptorSets )
VK_DEVICE_LEVEL_FUNCTION( vkCmdBindDescriptorSets )
VK_DEVICE_LEVEL_FUNCTION( vkCreateImage )
VK_DEVICE_LEVEL_FUNCTION( vkGetImageMemoryRequirements )
VK_DEVICE_LEVEL_FUNCTION( vkBindImageMemory )
VK_DEVICE_LEVEL_FUNCTION( vkCmdCopyBufferToImage )
VK_DEVICE_LEVEL_FUNCTION( vkCmdPipelineBarrier )
VK_DEVICE_LEVEL_FUNCTION( vkDestroyImage )
VK_DEVICE_LEVEL_FUNCTION( vkCreateSampler )
VK_DEVICE_LEVEL_FUNCTION( vkDestroySampler )
VK_DEVICE_LEVEL_FUNCTION( vkCreateQueryPool )
VK_DEVICE_LEVEL_FUNCTION( vkDestroyQueryPool )
VK_DEVICE_LEVEL_FUNCTION( vkCmdResetQueryPool )
VK_DEVICE_LEVEL_FUNCTION( vkResetQueryPool )
VK_DEVICE_LEVEL_FUNCTION( vkCmdBeginQuery )
VK_DEVICE_LEVEL_FUNCTION( vkCmdEndQuery )
VK_DEVICE_LEVEL_FUNCTION( vkCmdWriteTimestamp )
VK_DEVICE_LEVEL_FUNCTION( vkCmdWriteTimestamp2 )
VK_DEVICE_LEVEL_FUNCTION( vkGetQueryPoolResults )
VK_DEVICE_LEVEL_FUNCTION( vkCmdBlitImage )
VK_DEVICE_LEVEL_FUNCTION( vkCmdPushConstants )
VK_DEVICE_LEVEL_FUNCTION( vkCmdCopyImage )
VK_DEVICE_LEVEL_FUNCTION( vkCmdNextSubpass )
#undef VK_DEVICE_LEVEL_FUNCTION

internal void load_vk_global_functions(void)
{
    vkCreateInstance = (PFN_vkCreateInstance)vkGetInstanceProcAddr(0, "vkCreateInstance");
    vkEnumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties)vkGetInstanceProcAddr(0, "vkEnumerateInstanceExtensionProperties");
    vkEnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties)vkGetInstanceProcAddr(0, "vkEnumerateInstanceLayerProperties");
    vkEnumerateInstanceVersion = (PFN_vkEnumerateInstanceVersion)vkGetInstanceProcAddr(0, "vkEnumerateInstanceVersion");
}

internal void load_vk_instance_functions(VkInstance instance)
{
    vkGetDeviceProcAddr = VK_INSTANCE_LEVEL_FUNCTION_LOAD( instance, vkGetDeviceProcAddr )
    vkDestroyInstance = VK_INSTANCE_LEVEL_FUNCTION_LOAD( instance, vkDestroyInstance )
    vkEnumeratePhysicalDevices = VK_INSTANCE_LEVEL_FUNCTION_LOAD( instance, vkEnumeratePhysicalDevices )
    vkGetPhysicalDeviceProperties = VK_INSTANCE_LEVEL_FUNCTION_LOAD( instance, vkGetPhysicalDeviceProperties )
    vkGetPhysicalDeviceQueueFamilyProperties = VK_INSTANCE_LEVEL_FUNCTION_LOAD( instance, vkGetPhysicalDeviceQueueFamilyProperties )
    vkCreateDebugUtilsMessengerEXT = VK_INSTANCE_LEVEL_FUNCTION_LOAD( instance, vkCreateDebugUtilsMessengerEXT )
    vkDestroyDebugUtilsMessengerEXT = VK_INSTANCE_LEVEL_FUNCTION_LOAD( instance, vkDestroyDebugUtilsMessengerEXT )
#if _WIN32
    vkCreateWin32SurfaceKHR = VK_INSTANCE_LEVEL_FUNCTION_LOAD( instance, vkCreateWin32SurfaceKHR )
#endif
	vkDestroySurfaceKHR = VK_INSTANCE_LEVEL_FUNCTION_LOAD( instance, vkDestroySurfaceKHR )
    vkGetPhysicalDeviceSurfaceSupportKHR = VK_INSTANCE_LEVEL_FUNCTION_LOAD( instance, vkGetPhysicalDeviceSurfaceSupportKHR )
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR = VK_INSTANCE_LEVEL_FUNCTION_LOAD( instance, vkGetPhysicalDeviceSurfaceCapabilitiesKHR )
    vkGetPhysicalDeviceSurfaceFormatsKHR = VK_INSTANCE_LEVEL_FUNCTION_LOAD( instance, vkGetPhysicalDeviceSurfaceFormatsKHR )
    vkGetPhysicalDeviceSurfacePresentModesKHR = VK_INSTANCE_LEVEL_FUNCTION_LOAD( instance, vkGetPhysicalDeviceSurfacePresentModesKHR )
    vkCreateDevice = VK_INSTANCE_LEVEL_FUNCTION_LOAD( instance, vkCreateDevice )
    vkGetPhysicalDeviceMemoryProperties = VK_INSTANCE_LEVEL_FUNCTION_LOAD( instance, vkGetPhysicalDeviceMemoryProperties )
    vkGetPhysicalDeviceFeatures = VK_INSTANCE_LEVEL_FUNCTION_LOAD( instance, vkGetPhysicalDeviceFeatures )
    vkGetPhysicalDeviceFormatProperties = VK_INSTANCE_LEVEL_FUNCTION_LOAD( instance, vkGetPhysicalDeviceFormatProperties )
}

internal void load_vk_device_functions(VkDevice device)
{
    vkGetDeviceQueue = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkGetDeviceQueue )
    vkDestroyDevice = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkDestroyDevice  )
    vkDeviceWaitIdle = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkDeviceWaitIdle )
    vkCreateSwapchainKHR = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkCreateSwapchainKHR )
    vkGetSwapchainImagesKHR = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkGetSwapchainImagesKHR )
    vkDestroySwapchainKHR = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkDestroySwapchainKHR )
    vkCreateImageView = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkCreateImageView )
    vkDestroyImageView = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkDestroyImageView )
    vkCreateRenderPass = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkCreateRenderPass )
    vkCreateFramebuffer = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkCreateFramebuffer )
    vkDestroyRenderPass = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkDestroyRenderPass )
    vkDestroyFramebuffer = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkDestroyFramebuffer )
    vkCreateGraphicsPipelines = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkCreateGraphicsPipelines )
    vkDestroyPipeline = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkDestroyPipeline )
    vkCreateShaderModule = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkCreateShaderModule )
    vkDestroyShaderModule = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkDestroyShaderModule )
    vkCreatePipelineLayout = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkCreatePipelineLayout )
    vkDestroyPipelineLayout = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkDestroyPipelineLayout )
    vkCreateCommandPool = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkCreateCommandPool )
    vkDestroyCommandPool = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkDestroyCommandPool )
    vkAllocateCommandBuffers = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkAllocateCommandBuffers )
    vkResetCommandBuffer = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkResetCommandBuffer )
    vkBeginCommandBuffer = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkBeginCommandBuffer )
    vkCmdBeginRenderPass = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkCmdBeginRenderPass )
    vkCmdBindPipeline = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkCmdBindPipeline )
    vkCmdDraw = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkCmdDraw )
    vkCmdEndRenderPass = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkCmdEndRenderPass )
    vkEndCommandBuffer = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkEndCommandBuffer )
    vkCreateSemaphore = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkCreateSemaphore )
    vkDestroySemaphore = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkDestroySemaphore )
    vkCreateFence = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkCreateFence )
    vkDestroyFence = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkDestroyFence )
    vkWaitForFences = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkWaitForFences )
    vkAcquireNextImageKHR = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkAcquireNextImageKHR )
    vkResetFences = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkResetFences )
    vkQueueSubmit = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkQueueSubmit )
    vkQueuePresentKHR = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkQueuePresentKHR )
    vkCmdSetViewport = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkCmdSetViewport )
    vkCmdSetScissor = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkCmdSetScissor )
    vkCreateBuffer = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkCreateBuffer )
    vkGetBufferMemoryRequirements = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkGetBufferMemoryRequirements )
    vkAllocateMemory = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkAllocateMemory )
    vkBindBufferMemory = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkBindBufferMemory )
    vkMapMemory = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkMapMemory )
    vkUnmapMemory = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkUnmapMemory )
    vkDestroyBuffer = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkDestroyBuffer )
    vkFreeMemory = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkFreeMemory )
    vkCmdBindVertexBuffers = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkCmdBindVertexBuffers )
    vkCmdCopyBuffer = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkCmdCopyBuffer )
    vkQueueWaitIdle = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkQueueWaitIdle )
    vkFreeCommandBuffers = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkFreeCommandBuffers )
    vkCmdBindIndexBuffer = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkCmdBindIndexBuffer )
    vkCmdDrawIndexed = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkCmdDrawIndexed )
    vkCreateDescriptorSetLayout = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkCreateDescriptorSetLayout )
    vkDestroyDescriptorSetLayout = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkDestroyDescriptorSetLayout )
    vkCreateDescriptorPool = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkCreateDescriptorPool )
    vkAllocateDescriptorSets = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkAllocateDescriptorSets )
    vkDestroyDescriptorPool = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkDestroyDescriptorPool )
    vkUpdateDescriptorSets = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkUpdateDescriptorSets )
    vkCmdBindDescriptorSets = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkCmdBindDescriptorSets )
    vkCreateImage = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkCreateImage )
    vkGetImageMemoryRequirements = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkGetImageMemoryRequirements )
    vkBindImageMemory = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkBindImageMemory )
    vkCmdCopyBufferToImage = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkCmdCopyBufferToImage )
    vkCmdPipelineBarrier = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkCmdPipelineBarrier )
    vkDestroyImage = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkDestroyImage )
    vkCreateSampler = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkCreateSampler )
    vkDestroySampler = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkDestroySampler )
    vkCreateQueryPool = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkCreateQueryPool )
    vkDestroyQueryPool = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkDestroyQueryPool )
    vkCmdResetQueryPool = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkCmdResetQueryPool )
    vkResetQueryPool = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkResetQueryPool )
    vkCmdBeginQuery = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkCmdBeginQuery )
    vkCmdEndQuery = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkCmdEndQuery )
    vkCmdWriteTimestamp = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkCmdWriteTimestamp )
    vkCmdWriteTimestamp2 = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkCmdWriteTimestamp2 )
    vkGetQueryPoolResults = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkGetQueryPoolResults )
    vkCmdBlitImage = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkCmdBlitImage )
    vkCmdPushConstants = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkCmdPushConstants )
    vkCmdCopyImage = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkCmdCopyImage )
	vkCmdNextSubpass = VK_DEVICE_LEVEL_FUNCTION_LOAD( device, vkCmdNextSubpass )
}

internal VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_type,
    const VkDebugUtilsMessengerCallbackDataEXT *p_callback_data,
    void *p_user_data)
{
    // printf("Validation Layer: %s\n", p_callback_data->pMessage);

    // VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT : Diagnostic Message.
    // VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT : Information message
    // like the creation of a resource.
    // VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT : Message about
    // behaviour not necessarily an error, but very likely a bug in our
    // application.
    // VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT : Message about behaviour
    // that is invalid and may cause crashes.
#if 0
    if(message_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
    {
        printf("Validation Layer: %s\n", p_callback_data->pMessage);
        fflush(stdout);
    }
#endif
    if(message_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        printf("Validation Layer: %s\n", p_callback_data->pMessage);
        fflush(stdout);
    }
    // message_type
    // VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT : Some event has happened
    // that is unrelated to the specification or performance.
    // VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT : Something has happened
    // that violates the specification or indicates a possible mistake.
    // VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT : Potential non-optimal
    // use of Vulkan.

    return VK_FALSE;
}

internal uint32_t vk_init(vk_renderer *renderer, platform_read_file *read_file,
	   platform_free_file_memory *free_file_memory,	void *extra)
{
	VkResult result = load_vulkan();
	if(result != VK_SUCCESS) ASSERT(0);
	load_vk_global_functions();
	vk_create_instance(&renderer->instance);
#if SPECTRUM_DEBUG
	vk_init_debug_messenger(renderer->instance, &renderer->debug_messenger);
#endif
	vk_create_surface(renderer->instance, &renderer->surface, extra);
    vk_pick_physical_device(renderer->instance, &renderer->physical_device, renderer->surface);
	vk_create_device(renderer);
    vk_create_swapchain(renderer->surface, renderer->physical_device,
        renderer->device, &renderer->swapchain, &renderer->swapchain_images[0], &renderer->swapchain_image_format,
        &renderer->swapchain_image_extent, VK_NULL_HANDLE);
    vk_create_swapchain_image_views(renderer->device, &renderer->swapchain_images[0],
            &renderer->swapchain_image_views[0], renderer->swapchain_image_format);

      // TODO: Find if the host gpu, supports this depth_image_format, or
    // if we need to fallback to another depth format.
    renderer->depth_image_format = VK_FORMAT_D24_UNORM_S8_UINT; 
    renderer->total_frames = (uint32_t)ArraySize(renderer->swapchain_images);
    renderer->current_frame = 0;
    renderer->recreate_wsi = VK_SUCCESS;

    VkPhysicalDeviceProperties physical_device_properties;
    vkGetPhysicalDeviceProperties(renderer->physical_device, &physical_device_properties);
    renderer->timestamp_period = physical_device_properties.limits.timestampPeriod;
    renderer->msaa_samples = VK_SAMPLE_COUNT_1_BIT;

    vk_create_query_pool(renderer->device, &renderer->query_pool);

    vk_create_render_pass(renderer->device, &renderer->render_pass,
        renderer->swapchain_image_format, renderer->depth_image_format, renderer->msaa_samples);
	vk_init_frame_data(renderer);

    vk_create_global_resources(renderer->physical_device, renderer->device,
        &renderer->uniform_buffer, &renderer->uniform_buffer_memory,
        &renderer->global_descriptor_set_layout, &renderer->global_descriptor_pool,
        &renderer->global_descriptor_sets[0], ODS_FRAME_DATA_SIZE, sizeof(game_camera));

	vk_create_final_resources(renderer->device, renderer->data[0].color_image_view,
		renderer->data[0].depth_image_view, renderer->data[1].color_image_view,
	   	renderer->data[1].depth_image_view, &renderer->final_descriptor_set_layout,
		&renderer->final_descriptor_pool, &renderer->final_descriptor_sets[0]);


	// TODO + NOTE: We propably need to move the material and assets
	// to core_layer.
    const char *vert_shader_file = "shaders\\uniform_vert.spv";
    const char *frag_shader_file = "shaders\\frag.spv";

    vk_create_material(read_file, free_file_memory,
            vert_shader_file, frag_shader_file, renderer->device, renderer->render_pass,
        renderer->global_descriptor_set_layout, renderer->msaa_samples, 0, &renderer->first_material);

	const char *second_vert_shader_file = "shaders\\default_vert.spv";
	const char *second_frag_shader_file = "shaders\\forward_frag.spv";

    vk_create_material(read_file, free_file_memory,
            second_vert_shader_file, second_frag_shader_file, renderer->device, renderer->render_pass,
        renderer->final_descriptor_set_layout, renderer->msaa_samples, 1, &renderer->final_material);

	import_obj(&renderer->vertices, &renderer->indices, &renderer->total_vertices,
		&renderer->total_indices);

	create_rectangle(&renderer->rect_vertices, &renderer->rect_indices, &renderer->total_rect_vertices,
			&renderer->total_rect_indices);

	VkDeviceSize vertex_buffer_size = renderer->total_vertices * sizeof(vertex);
	VkDeviceSize index_buffer_size = renderer->total_indices * sizeof(uint16_t);

	void *vertices_data = renderer->vertices;
	void *indices_data = renderer->indices;

	renderer->vertex_buffer_size = (uint32_t)vertex_buffer_size;
	renderer->index_buffer_size = (uint16_t)renderer->total_indices;

    vk_allocate_mesh_gpu_local(renderer->physical_device, renderer->device,
        renderer->graphics_queue, renderer->data[0].command_pool, &renderer->vk_buffer,
        &renderer->vk_buffer_memory, vertices_data, indices_data, index_buffer_size, vertex_buffer_size);

	VkDeviceSize rect_vertex_buffer_size = renderer->total_rect_vertices * sizeof(vertex);
	VkDeviceSize rect_index_buffer_size = renderer->total_rect_indices * sizeof(uint16_t);

	void *rect_indices_data = renderer->rect_indices;
	void *rect_vertices_data = renderer->rect_vertices;

	renderer->rect_vertex_buffer_size = (uint32_t)rect_vertex_buffer_size;
	renderer->rect_index_buffer_size = (uint16_t)renderer->total_rect_indices;

    vk_allocate_mesh_gpu_local(renderer->physical_device, renderer->device,
        renderer->graphics_queue, renderer->data[0].command_pool, &renderer->rect_buffer,
        &renderer->rect_buffer_memory, rect_vertices_data, rect_indices_data,
		rect_index_buffer_size, rect_vertex_buffer_size);

	return 0;
}

internal void vk_deinit(vk_renderer *renderer)
{
	vkDeviceWaitIdle(renderer->device);

	free(renderer->vertices);
	free(renderer->indices);
	free(renderer->rect_vertices);
	free(renderer->rect_indices);

    vk_destroy_buffer(renderer->device, renderer->rect_buffer, renderer->rect_buffer_memory);
    vk_destroy_buffer(renderer->device, renderer->vk_buffer, renderer->vk_buffer_memory);
	vk_destroy_material(renderer->device, &renderer->first_material);
    vk_destroy_material(renderer->device, &renderer->final_material);
	vk_final_global_resources(renderer->device, renderer->final_descriptor_set_layout,
        renderer->final_descriptor_pool);
    vk_destroy_global_resources(renderer->device, renderer->uniform_buffer,
        renderer->uniform_buffer_memory, renderer->global_descriptor_set_layout,
        renderer->global_descriptor_pool);
	vk_deinit_frame_data(renderer);
	vkDestroyRenderPass(renderer->device, renderer->render_pass, 0);
#if 0
	for(uint32_t i = 0; i < ODS_FRAME_DATA_SIZE; ++i)
	{
		vkDestroyImageView(renderer->device, renderer->data[i].color_image_view, 0);
		vkDestroyImage(renderer->device, renderer->data[i].color_image, 0);
		vkFreeMemory(renderer->device, renderer->data[i].color_image_memory, 0);
	}
#endif
    vkDestroyQueryPool(renderer->device, renderer->query_pool, 0);
    for(uint32_t i = 0; i < ODS_FRAME_DATA_SIZE; ++i)
    {
        vkDestroyImageView(renderer->device, renderer->swapchain_image_views[i], 0);
    }
	vkDestroySwapchainKHR(renderer->device, renderer->swapchain, 0);
	vkDestroyDevice(renderer->device, 0);
	vkDestroySurfaceKHR(renderer->instance, renderer->surface, 0);
#if SPECTRUM_DEBUG
    vkDestroyDebugUtilsMessengerEXT(renderer->instance, renderer->debug_messenger, 0);
#endif
	vkDestroyInstance(renderer->instance, 0);
}

internal void vk_create_instance(VkInstance *instance)
{
    VkApplicationInfo application_info = {0};
    application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    application_info.pNext = 0;
    application_info.pApplicationName = "Spectrum";
    application_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    application_info.pEngineName = "Spectrum";
    application_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    application_info.apiVersion = VK_API_VERSION_1_3;

    VkDebugUtilsMessengerCreateInfoEXT debug_utils_messenger_create_info = {0};
    debug_utils_messenger_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_utils_messenger_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debug_utils_messenger_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debug_utils_messenger_create_info.pfnUserCallback = debug_callback;
    debug_utils_messenger_create_info.pUserData = 0; // Optional
    
    VkInstanceCreateInfo instance_create_info = {0};
    instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_create_info.pNext = &debug_utils_messenger_create_info;
    instance_create_info.flags = 0;
    instance_create_info.pApplicationInfo = &application_info;
    instance_create_info.enabledLayerCount = (uint64_t)ArraySize(instance_layers_list);
    instance_create_info.ppEnabledLayerNames = &instance_layers_list[0];
    instance_create_info.enabledExtensionCount = (uint64_t)ArraySize(instance_extensions_list);
    instance_create_info.ppEnabledExtensionNames = &instance_extensions_list[0];

    VK_ASSERT(vkCreateInstance(&instance_create_info, 0, instance));

    load_vk_instance_functions(*instance);
}


internal void vk_init_debug_messenger(VkInstance instance, VkDebugUtilsMessengerEXT *debug_messenger)
{
    VkDebugUtilsMessengerCreateInfoEXT debug_utils_messenger_create_info = {0};
    debug_utils_messenger_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_utils_messenger_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debug_utils_messenger_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debug_utils_messenger_create_info.pfnUserCallback = debug_callback;
    debug_utils_messenger_create_info.pUserData = 0; // Optional

    vkCreateDebugUtilsMessengerEXT(instance, &debug_utils_messenger_create_info,
        0, debug_messenger);
}

internal void vk_create_surface(VkInstance instance, VkSurfaceKHR *surface, void *extra)
{
#if _WIN32
    VkWin32SurfaceCreateInfoKHR surface_create_info = {0};
    surface_create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surface_create_info.pNext = 0;
    surface_create_info.flags = 0;
    surface_create_info.hinstance = GetModuleHandle(0);
    surface_create_info.hwnd = (HWND)extra;

    VK_ASSERT(vkCreateWin32SurfaceKHR(instance, &surface_create_info, 0, surface));
#endif
}

internal uint32_t vk_pick_physical_device(VkInstance instance, VkPhysicalDevice *physical_device,
		VkSurfaceKHR surface)
{
	uint32_t total_physical_devices = 0;
    VK_ASSERT(vkEnumeratePhysicalDevices(instance, &total_physical_devices, 0));
    ASSERT(total_physical_devices > 0);
	
	// TODO: Replace 10 with a constant.
	VkPhysicalDevice gpus[10];
	ASSERT(total_physical_devices < 10);

    VK_ASSERT(vkEnumeratePhysicalDevices(instance, &total_physical_devices, &gpus[0]));

	for(uint32_t i = 0; i < total_physical_devices; ++i)
	{
		VkPhysicalDeviceProperties physical_device_properties;
        vkGetPhysicalDeviceProperties(gpus[i], &physical_device_properties);

        VkPhysicalDeviceFeatures physical_device_features;
        vkGetPhysicalDeviceFeatures(gpus[i], &physical_device_features);

        // TODO: Check if extensions needed by the application are available by the gpu.
        // vkEnumerateDeviceExtensionProperties

        // VkPhysicalDeviceType
        // 1. VK_PHYSICAL_DEVICE_TYPE_OTHER = 0 // does not match any other available type.
        // 2. VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU = 1 // embedded.
        // 3. VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU = 2 // seperate processor.
        // 4. VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU = 3 // virtual node in a virtualizzation environment.
        // 5. VK_PHYSICAL_DEVICE_TYPE_CPU = 4 // typically running on the same processor as the host.
    
        // For now, return the first discrete gpu we will find.
        // TODO: Do it, in a more sophisticated way.
        if(physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
           physical_device_features.samplerAnisotropy &&
           (physical_device_properties.limits.timestampComputeAndGraphics == VK_TRUE))
        {
            if(physical_device_surface_support(gpus[i], surface) == VK_TRUE)
            {
                *physical_device = gpus[i];
                return 0;
            }
        }
    }

    *physical_device = gpus[0];
    return 1;
}

internal VkBool32 physical_device_surface_support(VkPhysicalDevice physical_device, VkSurfaceKHR surface)
{
    uint32_t queue_families_count = 0;

    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_families_count, 0);

	ASSERT(queue_families_count > 0);
	// TODO: Replace 10 with a constant.
	VkQueueFamilyProperties queue_family_properties[10];
	ASSERT(queue_families_count < 10);

    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_families_count,
        &queue_family_properties[0]);

    uint32_t current_index = 0;
    VkBool32 is_supported = VK_FALSE;

	for(uint32_t i = 0; i < queue_families_count; ++i)
    {
        if(queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, current_index, surface, &is_supported);

            if(is_supported == VK_TRUE)
            {
                return is_supported;
            }
        }

        ++current_index;
    }
    return is_supported;
}

internal void vk_create_device(vk_renderer *renderer)
{
    VkBool32 result = get_graphics_queue_family(renderer->physical_device, renderer->surface,
            &renderer->graphics_queue_index);

	if(result == VK_FALSE) ASSERT(0);

	float queue_priority = 1.0f;

    VkDeviceQueueCreateInfo device_graphics_queue_create_info = {0};
    device_graphics_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    device_graphics_queue_create_info.pNext = 0;
    device_graphics_queue_create_info.flags = 0;
    device_graphics_queue_create_info.queueFamilyIndex = renderer->graphics_queue_index;
    device_graphics_queue_create_info.queueCount = 1;
    device_graphics_queue_create_info.pQueuePriorities = &queue_priority;

    VkDeviceQueueCreateInfo device_queue_create_infos[] = {
        device_graphics_queue_create_info
    };

    VkPhysicalDeviceFeatures physical_device_features = {0};
    physical_device_features.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo device_create_info = {0};
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.pNext = 0;
    device_create_info.flags = 0;
    device_create_info.queueCreateInfoCount = ArraySize(device_queue_create_infos);
    device_create_info.pQueueCreateInfos = &device_queue_create_infos[0];
    // Device layers are deprecated.
    device_create_info.enabledLayerCount = 0; // Deprecated.
    device_create_info.ppEnabledLayerNames = 0; // Deprecated.
    device_create_info.enabledExtensionCount = ArraySize(device_extensions_list);
    device_create_info.ppEnabledExtensionNames = &device_extensions_list[0];
    device_create_info.pEnabledFeatures = &physical_device_features;

    VK_ASSERT(vkCreateDevice(renderer->physical_device, &device_create_info, VK_NULL_HANDLE, &renderer->device));

    load_vk_device_functions(renderer->device);

    vkGetDeviceQueue(renderer->device, renderer->graphics_queue_index, 0, &renderer->graphics_queue);
}

// Get a graphics queue index that supports presentation to a chosen surface.
internal VkBool32 get_graphics_queue_family(VkPhysicalDevice physical_device, VkSurfaceKHR surface,
    uint32_t *graphics_queue_index)
{
    uint32_t queue_families_count;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_families_count, 0);

	ASSERT(queue_families_count > 0);
	// TODO: Replace 10 with a constant.
	VkQueueFamilyProperties queue_family_properties[10];
	ASSERT(queue_families_count < 10);

    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_families_count, &queue_family_properties[0]);

    uint32_t current_index = 0;

    VkBool32 is_supported = VK_FALSE;

	for(uint32_t i = 0; i < queue_families_count; ++i)
    {
        if(queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {

            vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, current_index, surface, &is_supported);
            if(is_supported == VK_TRUE)
            {
                *graphics_queue_index = current_index;
                return VK_TRUE;
            }
        }

        ++current_index;
    }

    return VK_FALSE;
}

internal VkSurfaceFormatKHR vk_choose_swapchain_format(VkSurfaceFormatKHR *available_surface_formats,
		uint32_t available_surface_formats_counter)
{
	for(uint32_t i = 0; i < available_surface_formats_counter; ++i)
    {
        if(available_surface_formats[i].format == VK_FORMAT_B8G8R8A8_SRGB &&
           available_surface_formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return available_surface_formats[i];
        }
    }

    return available_surface_formats[0];
}

// VK_PRESENT_MODE_FIFO_KHR: VSync bound, Best for mobile (battery life), Triple buffering, always supported
// VK_PRESENT_MODE_MAILBOX_KHR: Keep submitting, low latency, Not optimal for mobile (bad battery life), not always supported
// VK_PRESENT_MODE_IMMEDIATE_KHR: Low latency, high power consumption, tearing
internal VkPresentModeKHR vk_choose_swapchain_present_mode(VkPresentModeKHR *available_present_modes,
		uint32_t available_present_modes_counter)
{
	for(uint32_t i = 0; i < available_present_modes_counter; ++i)
    {
        if(available_present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return available_present_modes[i];
        }
    }

    return available_present_modes[0];
}

internal VkExtent2D vk_choose_swapchain_extent(VkSurfaceCapabilitiesKHR *surface_capabilities)
{
    if(surface_capabilities->currentExtent.width != UINT32_MAX)
    {
        return surface_capabilities->currentExtent;
    }
    else
    {
        // TODO: Handle the case where the window is minimized.
        int32_t width = 0, height = 0;

        VkExtent2D current_extent = {(uint32_t)width, (uint32_t)height};

        current_extent.width = Max(surface_capabilities->minImageExtent.width,
                               Min(surface_capabilities->maxImageExtent.width, current_extent.width));
        current_extent.height = Max(surface_capabilities->minImageExtent.height,
                                Min(surface_capabilities->maxImageExtent.height, current_extent.height));

        return current_extent;
    }
}

internal surface_details vk_get_surface_details(VkPhysicalDevice physical_device, VkSurfaceKHR surface)
{
    surface_details details = {0};

    VK_ASSERT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface,
        &details.surface_capabilities));

    VK_ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &details.available_surface_formats_counter, 0));

    ASSERT(details.available_surface_formats_counter != 0);
	ASSERT(details.available_surface_formats_counter < 50);

    VK_ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &details.available_surface_formats_counter,
        &details.surface_formats[0]));

    VK_ASSERT(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface,
        &details.available_present_modes_counter, 0));

    ASSERT(details.available_present_modes_counter != 0);
	ASSERT(details.available_present_modes_counter < 10);

    VK_ASSERT(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface,
        &details.available_present_modes_counter, &details.present_modes[0]));

    return details;
}

internal void vk_create_swapchain(VkSurfaceKHR surface, VkPhysicalDevice physical_device,
		VkDevice device, VkSwapchainKHR *swapchain, VkImage *swapchain_images,
		VkFormat *swapchain_image_format, VkExtent2D *swapchain_image_extent,
		VkSwapchainKHR old_swapchain)
{
	surface_details details = vk_get_surface_details(physical_device, surface);

    VkSurfaceFormatKHR surface_format = vk_choose_swapchain_format(&details.surface_formats[0], details.available_surface_formats_counter);
    VkPresentModeKHR present_mode = vk_choose_swapchain_present_mode(&details.present_modes[0], details.available_present_modes_counter);
    VkExtent2D extent = vk_choose_swapchain_extent(&details.surface_capabilities);

#if 0
    uint32_t image_count = surface_details.surface_capabilities.minImageCount + 1;
    
    if(surface_details.surface_capabilities.minImageCount > 0 &&
       image_count > surface_details.surface_capabilities.maxImageCount)
    {
        image_count = surface_details.surface_capabilities.maxImageCount;
    }
#else

    // TODO: Maybe do it, in a more sophisticated way?
    uint32_t image_count = ODS_FRAME_DATA_SIZE; //  Hard coded double buffering.
#endif

    VkSwapchainCreateInfoKHR swapchain_create_info = {0};
    swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchain_create_info.pNext = 0;
    swapchain_create_info.flags = 0;
    swapchain_create_info.surface = surface;
    swapchain_create_info.minImageCount = image_count;
    swapchain_create_info.imageFormat = surface_format.format;
    swapchain_create_info.imageColorSpace = surface_format.colorSpace;
    swapchain_create_info.imageExtent = extent;
    swapchain_create_info.imageArrayLayers = 1;
    swapchain_create_info.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchain_create_info.queueFamilyIndexCount = 0;
    swapchain_create_info.pQueueFamilyIndices = 0;
    swapchain_create_info.preTransform = details.surface_capabilities.currentTransform;
    swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchain_create_info.presentMode = present_mode;
    swapchain_create_info.clipped = VK_FALSE;
    swapchain_create_info.oldSwapchain = old_swapchain;

    VK_ASSERT(vkCreateSwapchainKHR(device, &swapchain_create_info, 0, swapchain));

    vkGetSwapchainImagesKHR(device, *swapchain, &image_count, 0);
    vkGetSwapchainImagesKHR(device, *swapchain, &image_count, &swapchain_images[0]);

    *swapchain_image_format = surface_format.format;
    *swapchain_image_extent = extent;
}

internal void vk_create_swapchain_image_views(VkDevice device, VkImage *swapchain_images,
        VkImageView *swapchain_image_views, VkFormat swapchain_image_format)
{
    for(size_t i = 0; i < ODS_FRAME_DATA_SIZE; ++i)
    {
        VkImageViewCreateInfo image_view_create_info = {0};
        image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_create_info.pNext = 0;
        image_view_create_info.flags = 0;
        image_view_create_info.image = swapchain_images[i];
        image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_create_info.format = swapchain_image_format;
        image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_view_create_info.subresourceRange.baseMipLevel = 0;
        image_view_create_info.subresourceRange.levelCount = 1;
        image_view_create_info.subresourceRange.baseArrayLayer = 0;
        image_view_create_info.subresourceRange.layerCount = 1;

        VK_ASSERT(vkCreateImageView(device, &image_view_create_info, 0, &swapchain_image_views[i]));
    }
}

internal void vk_create_query_pool(VkDevice device, VkQueryPool *query_pool)
{
    VkQueryPoolCreateInfo query_pool_create_info = {0};
    query_pool_create_info.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
    query_pool_create_info.pNext = 0;
    query_pool_create_info.flags = 0;
    query_pool_create_info.queryType = VK_QUERY_TYPE_TIMESTAMP;
    query_pool_create_info.queryCount = 2 * ODS_FRAME_DATA_SIZE;
    query_pool_create_info.pipelineStatistics = 0;

    VK_ASSERT(vkCreateQueryPool(device, &query_pool_create_info, 0, query_pool));
}

internal void vk_create_render_pass(VkDevice device, VkRenderPass *render_pass,
		VkFormat color_attachment_format, VkFormat depth_attachment_format,
		VkSampleCountFlagBits msaa_samples)
{
        VkAttachmentDescription color_attachment_description = {0};
        color_attachment_description.flags = 0;
        color_attachment_description.format = color_attachment_format;
        color_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
        color_attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        color_attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        color_attachment_description.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkAttachmentReference color_attachment_reference = {0};
        color_attachment_reference.attachment = 0;
        color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference color_attachment_reference2 = {0};
		color_attachment_reference2.attachment = 0;
		color_attachment_reference2.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkAttachmentDescription final_color_attachment_description = {0};
        final_color_attachment_description.flags = 0;
        final_color_attachment_description.format = color_attachment_format;
        final_color_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
        final_color_attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        final_color_attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        final_color_attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        final_color_attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        final_color_attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        final_color_attachment_description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference final_color_attachment_reference = {0};
        final_color_attachment_reference.attachment = 2;
        final_color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription depth_attachment_description = {0};
        depth_attachment_description.flags = 0;
        depth_attachment_description.format = depth_attachment_format;
        depth_attachment_description.samples = VK_SAMPLE_COUNT_1_BIT;
        depth_attachment_description.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth_attachment_description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        depth_attachment_description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depth_attachment_description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth_attachment_description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depth_attachment_description.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depth_attachment_reference = {0};
        depth_attachment_reference.attachment = 1;
        depth_attachment_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


		VkAttachmentDescription attachment_descriptions [] = {
			color_attachment_description,
			depth_attachment_description,
			final_color_attachment_description
		};

		VkSubpassDescription first_subpass_description = {0};
        first_subpass_description.flags = 0;
        first_subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        // Each element of the pInputAttachments array corresponds to an input attachment
        // index in a fragment shader
        // subpass_description.inputAttachmentCount
        // subpass_description.pInputAttachments
        // Each element of the pColorAttachments array corresponds to an output location
        // in the shader.
        first_subpass_description.colorAttachmentCount = 1;
        first_subpass_description.pColorAttachments = &color_attachment_reference;
        first_subpass_description.pResolveAttachments = 0;
        first_subpass_description.pDepthStencilAttachment = &depth_attachment_reference;
        first_subpass_description.preserveAttachmentCount = 1;
        first_subpass_description.pPreserveAttachments = &final_color_attachment_reference.attachment;

		VkSubpassDescription second_subpass_description = {0};
		second_subpass_description.flags = 0;
		second_subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		second_subpass_description.inputAttachmentCount = 1;
		second_subpass_description.pInputAttachments = &color_attachment_reference2;
        second_subpass_description.colorAttachmentCount = 1;
        second_subpass_description.pColorAttachments = &final_color_attachment_reference;
        second_subpass_description.pResolveAttachments = 0;
        second_subpass_description.pDepthStencilAttachment = &depth_attachment_reference;
        second_subpass_description.preserveAttachmentCount = 0;
        second_subpass_description.pPreserveAttachments = 0;

		VkSubpassDescription subpass_descriptions [] = {
			first_subpass_description,
			second_subpass_description
		};

        // NOTE + TODO: Check subpass dependency again.
        VkSubpassDependency subpass_dependency = {0};
        subpass_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        subpass_dependency.dstSubpass = 0;
        subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        subpass_dependency.srcAccessMask = 0;
        subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        // TODO: Check dependency flags.
        subpass_dependency.dependencyFlags = 0;

        VkSubpassDependency second_subpass_dependency = {0};
        second_subpass_dependency.srcSubpass = 0;
        second_subpass_dependency.dstSubpass = 1;
        second_subpass_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        second_subpass_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        second_subpass_dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        second_subpass_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        // TODO: Check dependency flags.
        second_subpass_dependency.dependencyFlags = 0;

		VkSubpassDependency subpass_dependencies [] = {
			subpass_dependency,
			second_subpass_dependency
		};
        
        VkRenderPassCreateInfo render_pass_create_info = {0};
        render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_create_info.pNext = 0;
        render_pass_create_info.flags = 0;
        render_pass_create_info.attachmentCount = ArraySize(attachment_descriptions);
        render_pass_create_info.pAttachments = &attachment_descriptions[0];
        render_pass_create_info.subpassCount = ArraySize(subpass_descriptions);
        render_pass_create_info.pSubpasses = &subpass_descriptions[0];
        render_pass_create_info.dependencyCount = ArraySize(subpass_dependencies);
        render_pass_create_info.pDependencies = &subpass_dependencies[0];

        VK_ASSERT(vkCreateRenderPass(device, &render_pass_create_info, 0, render_pass));
}

internal void vk_init_frame_data(vk_renderer *renderer)
{
	ASSERT(renderer->render_pass);

	for(uint32_t i = 0; i < ODS_FRAME_DATA_SIZE; ++i)
	{
		VkExtent3D color_image_extent = {0};
		color_image_extent.width = renderer->swapchain_image_extent.width;
		color_image_extent.height = renderer->swapchain_image_extent.height;
		color_image_extent.depth = 1;

		vk_create_image(renderer->physical_device, renderer->device,
				&renderer->data[i].color_image, &renderer->data[i].color_image_memory,
				renderer->swapchain_image_format, color_image_extent, VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, &renderer->graphics_queue_index,
				VK_IMAGE_LAYOUT_UNDEFINED, VK_SAMPLE_COUNT_1_BIT);

        VkImageViewCreateInfo image_view_create_info = {0};
        image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_create_info.pNext = 0;
        image_view_create_info.flags = 0;
        image_view_create_info.image = renderer->data[i].color_image;
        image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_create_info.format = renderer->swapchain_image_format;
        image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_view_create_info.subresourceRange.baseMipLevel = 0;
        image_view_create_info.subresourceRange.levelCount = 1;
        image_view_create_info.subresourceRange.baseArrayLayer = 0;
        image_view_create_info.subresourceRange.layerCount = 1;

        VK_ASSERT(vkCreateImageView(renderer->device, &image_view_create_info, 0, &renderer->data[i].color_image_view));
        vk_create_command_pool(renderer->device, &renderer->data[i].command_pool,
            renderer->graphics_queue_index);
        vk_create_command_buffers(renderer->device, renderer->data[i].command_pool,
            &renderer->data[i].command_buffer, 1);
        vk_create_sync_objects(renderer->device, &renderer->data[i].image_available,
            &renderer->data[i].render_finished, &renderer->data[i].render_fence);
        vk_create_depth_resource(renderer->physical_device, renderer->device,
            &renderer->data[i].depth_image, &renderer->data[i].depth_image_memory,
            &renderer->data[i].depth_image_view, renderer->depth_image_format,
            renderer->msaa_samples,
            renderer->swapchain_image_extent, &renderer->graphics_queue_index);
        vk_create_framebuffer(renderer->device, renderer->render_pass,
            renderer->swapchain_image_views[i], renderer->data[i].depth_image_view,
            renderer->data[i].color_image_view, &renderer->data[i].framebuffer,
			renderer->swapchain_image_extent);
	}
}

internal void vk_deinit_frame_data(vk_renderer *renderer)
{
    for(uint32_t i = 0; i < ODS_FRAME_DATA_SIZE; ++i)
    {
		vkDestroyImage(renderer->device, renderer->data[i].color_image, 0);
		vkFreeMemory(renderer->device, renderer->data[i].color_image_memory, 0);
		vkDestroyImageView(renderer->device, renderer->data[i].color_image_view, 0);
        vkDestroyCommandPool(renderer->device, renderer->data[i].command_pool, 0);
        vkDestroySemaphore(renderer->device, renderer->data[i].image_available, 0);
        vkDestroySemaphore(renderer->device, renderer->data[i].render_finished, 0);
        vkDestroyFence(renderer->device, renderer->data[i].render_fence, 0);
        vkDestroyFramebuffer(renderer->device, renderer->data[i].framebuffer, 0);
        vk_destroy_depth_resources(renderer->device, renderer->data[i].depth_image,
            renderer->data[i].depth_image_view, renderer->data[i].depth_image_memory);
    }
}

internal void vk_create_command_pool(VkDevice device, VkCommandPool *command_pool,
    uint32_t queue_family_index)
{
    VkCommandPoolCreateInfo command_pool_create_info = {0};
    command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    command_pool_create_info.pNext = 0;
    command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    command_pool_create_info.queueFamilyIndex = queue_family_index;

    VK_ASSERT(vkCreateCommandPool(device, &command_pool_create_info, 0, command_pool));
}

internal void vk_create_command_buffers(VkDevice device, VkCommandPool command_pool,
    VkCommandBuffer *command_buffer, uint32_t command_buffers_size)
{
    VkCommandBufferAllocateInfo command_buffer_allocate_info = {0};
    command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    command_buffer_allocate_info.pNext = 0;
    command_buffer_allocate_info.commandPool = command_pool;
    command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    command_buffer_allocate_info.commandBufferCount = 1;

    VK_ASSERT(vkAllocateCommandBuffers(device, &command_buffer_allocate_info, command_buffer));
}

internal void vk_create_sync_objects(VkDevice device, VkSemaphore *image_available, VkSemaphore *render_finished,
    VkFence *render_fence)
{
    VkSemaphoreCreateInfo semaphore_create_info = {0};
    semaphore_create_info.pNext = 0;
    semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_create_info = {0};
    fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_create_info.pNext = 0;
    fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VK_ASSERT(vkCreateSemaphore(device, &semaphore_create_info, 0, image_available));
    VK_ASSERT(vkCreateSemaphore(device, &semaphore_create_info, 0, render_finished));
    VK_ASSERT(vkCreateFence(device, &fence_create_info, 0, render_fence));
}

static void vk_create_framebuffer(VkDevice device, VkRenderPass render_pass,
        VkImageView image_view, VkImageView depth_image_view, VkImageView color_image_view,
        VkFramebuffer *framebuffer, VkExtent2D extent)
{
	VkImageView attachments[] = {
		color_image_view,
		depth_image_view,
		image_view
	};

	VkFramebufferCreateInfo framebuffer_create_info = {0};
	framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebuffer_create_info.pNext = 0;
	framebuffer_create_info.flags = 0;
	framebuffer_create_info.renderPass = render_pass;
	framebuffer_create_info.attachmentCount = ArraySize(attachments);
	framebuffer_create_info.pAttachments = &attachments[0];
	framebuffer_create_info.width = extent.width;
	framebuffer_create_info.height = extent.height;
	framebuffer_create_info.layers = 1;

	VK_ASSERT(vkCreateFramebuffer(device, &framebuffer_create_info, 0, framebuffer));
}

internal void vk_create_depth_resource(VkPhysicalDevice physical_device, VkDevice device,
        VkImage *depth_image, VkDeviceMemory *depth_image_memory, VkImageView *depth_image_view,
        VkFormat depth_image_format, VkSampleCountFlagBits samples, VkExtent2D extent,
        const uint32_t *const queue_index)
{
    VkExtent3D depth_image_extent = {extent.width, extent.height, 1};

    vk_create_image(physical_device, device,
            depth_image, depth_image_memory,
            depth_image_format, depth_image_extent, VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, queue_index,
            VK_IMAGE_LAYOUT_UNDEFINED, samples);

    VkImageViewCreateInfo depth_image_view_create_info = {0};
    depth_image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    depth_image_view_create_info.pNext = 0;
    depth_image_view_create_info.flags = 0;
    depth_image_view_create_info.image = *depth_image;
    depth_image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    depth_image_view_create_info.format = depth_image_format;
    depth_image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    depth_image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    depth_image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    depth_image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    depth_image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    depth_image_view_create_info.subresourceRange.baseMipLevel = 0;
    depth_image_view_create_info.subresourceRange.levelCount = 1;
    depth_image_view_create_info.subresourceRange.baseArrayLayer = 0;
    depth_image_view_create_info.subresourceRange.layerCount = 1;

    VK_ASSERT(vkCreateImageView(device, &depth_image_view_create_info, 0, depth_image_view));
}

internal void vk_destroy_depth_resources(VkDevice device, VkImage depth_image,
        VkImageView depth_image_view, VkDeviceMemory depth_image_memory)
{
    vkDestroyImage(device, depth_image, 0);
    vkDestroyImageView(device, depth_image_view, 0);
    vkFreeMemory(device, depth_image_memory, 0);
}

internal void vk_create_global_resources(VkPhysicalDevice physical_device, VkDevice device,
        VkBuffer *uniform_buffer, VkDeviceMemory *uniform_buffer_memory,
        VkDescriptorSetLayout *descriptor_set_layout, VkDescriptorPool *descriptor_pool,
        VkDescriptorSet *descriptor_sets, uint32_t descriptor_max_sets, size_t original_size)
{
	VkPhysicalDeviceProperties physical_device_properties;
    vkGetPhysicalDeviceProperties(physical_device, &physical_device_properties);

    VkDeviceSize total_size = 0;
    size_t padding = 0;
    // Calculate required alignment based on minimum device offset alignemnt.
    VkDeviceSize uniform_buffer_alignment = physical_device_properties.limits.minUniformBufferOffsetAlignment;
    if(uniform_buffer_alignment > 0)
    {
        total_size = (original_size + uniform_buffer_alignment - 1) & ~(uniform_buffer_alignment - 1);
        total_size *= ODS_FRAME_DATA_SIZE;
    }

    vk_create_buffer(physical_device, device, total_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            uniform_buffer, uniform_buffer_memory);

    VkDescriptorSetLayoutBinding uniform_buffer_layout_binding = {0};
    uniform_buffer_layout_binding.binding = 0;
    uniform_buffer_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uniform_buffer_layout_binding.descriptorCount = 1;
    uniform_buffer_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uniform_buffer_layout_binding.pImmutableSamplers = 0;

    VkDescriptorSetLayoutBinding descriptor_set_layout_bindings [] = {
        uniform_buffer_layout_binding
    };

    VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = {0};
    descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptor_set_layout_create_info.pNext = 0;
    descriptor_set_layout_create_info.flags = 0;
    descriptor_set_layout_create_info.bindingCount = ArraySize(descriptor_set_layout_bindings);
    descriptor_set_layout_create_info.pBindings = &descriptor_set_layout_bindings[0];

    VK_ASSERT(vkCreateDescriptorSetLayout(device, &descriptor_set_layout_create_info, 0,
                descriptor_set_layout));

    VkDescriptorPoolSize uniform_buffer_pool_size = {0};
    uniform_buffer_pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uniform_buffer_pool_size.descriptorCount = descriptor_max_sets;

    VkDescriptorPoolSize descriptor_pool_sizes [] = {
        uniform_buffer_pool_size
    };

    VkDescriptorPoolCreateInfo descriptor_pool_create_info = {0};
    descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptor_pool_create_info.pNext = 0;
    descriptor_pool_create_info.flags = 0;
    descriptor_pool_create_info.maxSets = descriptor_max_sets;
    descriptor_pool_create_info.poolSizeCount = ArraySize(descriptor_pool_sizes);
    descriptor_pool_create_info.pPoolSizes = &descriptor_pool_sizes[0];

    VK_ASSERT(vkCreateDescriptorPool(device, &descriptor_pool_create_info, 0, descriptor_pool));

    VkDescriptorSetLayout descriptor_set_layouts[ODS_FRAME_DATA_SIZE];

    for(size_t i = 0; i < ODS_FRAME_DATA_SIZE; ++i)
    {
        descriptor_set_layouts[i] = *descriptor_set_layout;
    }

    VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {0};
    descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptor_set_allocate_info.pNext = 0;
    descriptor_set_allocate_info.descriptorPool = *descriptor_pool;
    descriptor_set_allocate_info.descriptorSetCount = ODS_FRAME_DATA_SIZE;
    descriptor_set_allocate_info.pSetLayouts = &descriptor_set_layouts[0];

    VK_ASSERT(vkAllocateDescriptorSets(device, &descriptor_set_allocate_info, descriptor_sets));

    for(size_t i = 0; i < descriptor_max_sets; ++i)
    {
        VkDescriptorBufferInfo descriptor_buffer_info = {0};
        descriptor_buffer_info.buffer = *uniform_buffer;
        descriptor_buffer_info.offset = padding * i;
        descriptor_buffer_info.range = sizeof(game_camera);

        VkWriteDescriptorSet write_uniform_buffer_descriptor_set = {0};
        write_uniform_buffer_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write_uniform_buffer_descriptor_set.pNext = 0;
        write_uniform_buffer_descriptor_set.dstSet = descriptor_sets[i];
        write_uniform_buffer_descriptor_set.dstBinding = 0;
        write_uniform_buffer_descriptor_set.dstArrayElement = 0;
        write_uniform_buffer_descriptor_set.descriptorCount = 1;
        write_uniform_buffer_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write_uniform_buffer_descriptor_set.pImageInfo = 0;
        write_uniform_buffer_descriptor_set.pBufferInfo = &descriptor_buffer_info;
        write_uniform_buffer_descriptor_set.pTexelBufferView = 0;

        VkWriteDescriptorSet write_descriptor_sets [] = {
            write_uniform_buffer_descriptor_set
        };

        vkUpdateDescriptorSets(device, ArraySize(write_descriptor_sets), &write_descriptor_sets[0], 0, 0);
    }
}

internal void vk_destroy_global_resources(VkDevice device, VkBuffer uniform_buffer,
        VkDeviceMemory uniform_buffer_memory, VkDescriptorSetLayout global_descriptor_set_layout,
        VkDescriptorPool global_descriptor_pool)
{
    vkDestroyBuffer(device, uniform_buffer, 0);
    vkFreeMemory(device, uniform_buffer_memory, 0);
    vkDestroyDescriptorSetLayout(device, global_descriptor_set_layout, 0);
    vkDestroyDescriptorPool(device, global_descriptor_pool, 0);
}

// TODO: Check if final_resources are proper.
internal void vk_create_final_resources(VkDevice device, VkImageView color_image_view,
		VkImageView depth_image_view, VkImageView color_image_view1,
		VkImageView depth_image_view1, VkDescriptorSetLayout *descriptor_set_layout,
		VkDescriptorPool *descriptor_pool, VkDescriptorSet *descriptor_sets)
{
	VkDescriptorSetLayoutBinding color_image_layout_binding = {0};
	color_image_layout_binding.binding = 0;
	color_image_layout_binding.descriptorCount = 1;
	color_image_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
	color_image_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding depth_image_layout_binding = {0};
	depth_image_layout_binding.binding = 1;
	depth_image_layout_binding.descriptorCount = 1;
	depth_image_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
	depth_image_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutBinding descriptor_set_layout_bindings [] = {
        color_image_layout_binding,
		depth_image_layout_binding
    };

    VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = {0};
    descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptor_set_layout_create_info.pNext = 0;
    descriptor_set_layout_create_info.flags = 0;
    descriptor_set_layout_create_info.bindingCount = ArraySize(descriptor_set_layout_bindings);
    descriptor_set_layout_create_info.pBindings = &descriptor_set_layout_bindings[0];

    VK_ASSERT(vkCreateDescriptorSetLayout(device, &descriptor_set_layout_create_info, 0,
                descriptor_set_layout));

	uint32_t descriptor_max_sets = 2;

    VkDescriptorPoolSize color_image_pool_size = {0};
    color_image_pool_size.type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    color_image_pool_size.descriptorCount = descriptor_max_sets;

    VkDescriptorPoolSize depth_image_pool_size = {0};
    depth_image_pool_size.type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    depth_image_pool_size.descriptorCount = descriptor_max_sets;

    VkDescriptorPoolSize descriptor_pool_sizes [] = {
        color_image_pool_size,
		depth_image_pool_size
    };

    VkDescriptorPoolCreateInfo descriptor_pool_create_info = {0};
    descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptor_pool_create_info.pNext = 0;
    descriptor_pool_create_info.flags = 0;
    descriptor_pool_create_info.maxSets = descriptor_max_sets;
    descriptor_pool_create_info.poolSizeCount = ArraySize(descriptor_pool_sizes);
    descriptor_pool_create_info.pPoolSizes = &descriptor_pool_sizes[0];

    VK_ASSERT(vkCreateDescriptorPool(device, &descriptor_pool_create_info, 0, descriptor_pool));

    VkDescriptorSetLayout descriptor_set_layouts[ODS_FRAME_DATA_SIZE];

    for(size_t i = 0; i < ODS_FRAME_DATA_SIZE; ++i)
    {
        descriptor_set_layouts[i] = *descriptor_set_layout;
    }

    VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {0};
    descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptor_set_allocate_info.pNext = 0;
    descriptor_set_allocate_info.descriptorPool = *descriptor_pool;
    descriptor_set_allocate_info.descriptorSetCount = ODS_FRAME_DATA_SIZE;
    descriptor_set_allocate_info.pSetLayouts = &descriptor_set_layouts[0];

    VK_ASSERT(vkAllocateDescriptorSets(device, &descriptor_set_allocate_info, descriptor_sets));

	for(size_t i = 0; i < ODS_FRAME_DATA_SIZE; ++i)
	{
		VkDescriptorImageInfo descriptors[2] = {0};
		if(i == 0)
		{
			descriptors[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			descriptors[0].imageView = color_image_view;
			descriptors[0].sampler = VK_NULL_HANDLE;

			descriptors[1].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			descriptors[1].imageView = depth_image_view;
			descriptors[1].sampler = VK_NULL_HANDLE;
		}
		else
		{
			descriptors[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			descriptors[0].imageView = color_image_view1;
			descriptors[0].sampler = VK_NULL_HANDLE;

			descriptors[1].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			descriptors[1].imageView = depth_image_view1;
			descriptors[1].sampler = VK_NULL_HANDLE;
		}

		VkWriteDescriptorSet write_descriptor_sets[2] = {0};

		write_descriptor_sets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_descriptor_sets[0].dstSet = descriptor_sets[i];
		write_descriptor_sets[0].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		write_descriptor_sets[0].descriptorCount = 1;
		write_descriptor_sets[0].dstBinding = 0;
		write_descriptor_sets[0].pImageInfo = &descriptors[0];

		write_descriptor_sets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_descriptor_sets[1].dstSet = descriptor_sets[i];
		write_descriptor_sets[1].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		write_descriptor_sets[1].descriptorCount = 1;
		write_descriptor_sets[1].dstBinding = 1;
		write_descriptor_sets[1].pImageInfo = &descriptors[1];

		vkUpdateDescriptorSets(device, 2, &write_descriptor_sets[0], 0, 0);
	}
}

internal void vk_final_global_resources(VkDevice device, VkDescriptorSetLayout final_descriptor_set_layout,
        VkDescriptorPool final_descriptor_pool)
{
    vkDestroyDescriptorSetLayout(device, final_descriptor_set_layout, 0);
    vkDestroyDescriptorPool(device, final_descriptor_pool, 0);
}

internal void vk_create_image(VkPhysicalDevice physical_device, VkDevice device,
		VkImage *image, VkDeviceMemory *image_memory, VkFormat format,
		VkExtent3D extent, VkImageTiling tiling, VkImageUsageFlags usage,
		const uint32_t *const queue_index, VkImageLayout layout,
		VkSampleCountFlagBits samples)
{
    VkImageCreateInfo image_create_info = {0};
    image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_create_info.pNext = 0;
    image_create_info.flags = 0;
    image_create_info.imageType = VK_IMAGE_TYPE_2D;
    image_create_info.format = format;
    image_create_info.extent = extent;
    image_create_info.mipLevels = 1;
    image_create_info.arrayLayers = 1;
    image_create_info.samples = samples;
    image_create_info.tiling = tiling;
    image_create_info.usage = usage;
    image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    // image_create_info.queueFamilyIndexCount = 
    // image_create_info.pQueueFamilyIndices =
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VK_ASSERT(vkCreateImage(device, &image_create_info, 0, image));

    VkMemoryRequirements memory_requirements;
    vkGetImageMemoryRequirements(device, *image, &memory_requirements);

    VkMemoryAllocateInfo memory_allocate_info = {0};
    memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memory_allocate_info.pNext = 0;
    memory_allocate_info.allocationSize = memory_requirements.size;

    VkPhysicalDeviceMemoryProperties physical_device_memory_properties;
    vkGetPhysicalDeviceMemoryProperties(physical_device, &physical_device_memory_properties);

    // TODO: Add assert that we found the requested memory type.
    memory_allocate_info.memoryTypeIndex = vk_find_memory_type(physical_device,
            memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VK_ASSERT(vkAllocateMemory(device, &memory_allocate_info, 0, image_memory));

    vkBindImageMemory(device, *image, *image_memory, 0);

    // TODO: if the requested layout is different than VK_IMAGE_LAYOUT_UNDEFINED,
    // do a transition.
}

internal void vk_allocate_mesh_gpu_local(VkPhysicalDevice physical_device, VkDevice device,
        VkQueue queue, VkCommandPool command_pool, VkBuffer *buffer, VkDeviceMemory *buffer_memory,
        void *vertices_data, void *indices_data, VkDeviceSize index_buffer_size,
        VkDeviceSize vertex_buffer_size)
{
    VkDeviceSize total_buffer_size = index_buffer_size + vertex_buffer_size;

    VkBuffer staging_buffer;
    VkDeviceMemory staging_buffer_memory;

    vk_create_buffer(physical_device, device, total_buffer_size,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            &staging_buffer, &staging_buffer_memory);

    void *data;
    vkMapMemory(device, staging_buffer_memory, 0, total_buffer_size, 0, &data);
    memcpy(data, vertices_data, vertex_buffer_size);
    memcpy((uint8_t*)data + vertex_buffer_size, indices_data, index_buffer_size);
    vkUnmapMemory(device, staging_buffer_memory);

    vk_create_buffer(physical_device, device, total_buffer_size,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            buffer, buffer_memory);

    vk_copy_buffer(device, queue, command_pool,
            staging_buffer, *buffer, total_buffer_size);

    vk_destroy_buffer(device, staging_buffer, staging_buffer_memory);
}

internal void vk_create_buffer(VkPhysicalDevice physical_device, VkDevice device,
    VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
    VkBuffer *buffer, VkDeviceMemory *buffer_memory)
{
    VkBufferCreateInfo buffer_create_info = {0};
    buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_create_info.pNext = 0;
    buffer_create_info.flags = 0;
    buffer_create_info.size = size;
    buffer_create_info.usage = usage;
    buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    buffer_create_info.queueFamilyIndexCount = 0; // Ignored if sharing mode is not VK_SHARING_MODE_CONCURRENT
    buffer_create_info.pQueueFamilyIndices = 0; // Ignored if sharing mode is not VK_SHARING_MODE_CONCURRENT

    VK_ASSERT(vkCreateBuffer(device, &buffer_create_info, 0, buffer));

    VkMemoryRequirements memory_requirements;
    vkGetBufferMemoryRequirements(device, *buffer, &memory_requirements);

    VkMemoryAllocateInfo memory_allocate_info = {0};
    memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = vk_find_memory_type(physical_device,
        memory_requirements.memoryTypeBits, properties);

    VK_ASSERT(vkAllocateMemory(device, &memory_allocate_info, 0, buffer_memory));

    vkBindBufferMemory(device, *buffer, *buffer_memory, 0);
}


internal void vk_copy_buffer(VkDevice device, VkQueue queue, VkCommandPool command_pool,
    VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize buffer_size)
{
    VkCommandBufferAllocateInfo command_buffer_allocate_info = {0};
    command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    command_buffer_allocate_info.commandPool = command_pool;
    command_buffer_allocate_info.commandBufferCount = 1;

    VkCommandBuffer command_buffer;
    vkAllocateCommandBuffers(device, &command_buffer_allocate_info, &command_buffer);

    VkCommandBufferBeginInfo command_buffer_begin_info = {0};
    command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info);

    VkBufferCopy buffer_copy_region = {0};
    buffer_copy_region.srcOffset = 0;
    buffer_copy_region.dstOffset = 0;
    buffer_copy_region.size = buffer_size;

    vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &buffer_copy_region);

    vkEndCommandBuffer(command_buffer);

    VkSubmitInfo submit_info = {0};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;

    // TODO: Add fence synchronization.
    vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);

    vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);
}

internal void vk_destroy_buffer(VkDevice device, VkBuffer buffer, VkDeviceMemory buffer_memory)
{
    vkDestroyBuffer(device, buffer, 0);
    vkFreeMemory(device, buffer_memory, 0);
}

internal uint32_t vk_find_memory_type(VkPhysicalDevice physical_device, uint32_t type_filter,
    VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties physical_device_memory_properties;
    vkGetPhysicalDeviceMemoryProperties(physical_device, &physical_device_memory_properties);

    for(uint32_t i = 0; i < physical_device_memory_properties.memoryTypeCount; ++i)
    {
        if(type_filter & (1 << i) &&
            (physical_device_memory_properties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    return ~0u;
}

internal void vk_create_shader_module(platform_read_file *read_file, platform_free_file_memory *free_file_memory,
        VkDevice device, VkShaderModule *shader_module, const char *shader)
{
	uint32_t shader_size;
	void *shader_data = 0;
    int32_t result = read_file(shader, &shader_size, &shader_data);
    ASSERT(result == 0);

    VkShaderModuleCreateInfo shader_module_create_info = {0};
    shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shader_module_create_info.pNext = 0;
    shader_module_create_info.flags = 0;
    shader_module_create_info.codeSize = shader_size;
    shader_module_create_info.pCode = (uint32_t*)shader_data;

    VK_ASSERT(vkCreateShaderModule(device, &shader_module_create_info, 0, shader_module));

    free_file_memory(shader_data);
}

internal void vk_create_pipeline_layout(VkDevice device, VkPipelineLayout *pipeline_layout,
		VkDescriptorSetLayout descriptor_set_layout)
{
    VkPipelineLayoutCreateInfo pipeline_layout_create_info = {0};
    pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_create_info.pNext = 0;
    pipeline_layout_create_info.flags = 0;
    // TODO: Check VkPhysicalDeviceLimits::maxBoundDescriptorSets
    pipeline_layout_create_info.setLayoutCount = 1;
    pipeline_layout_create_info.pSetLayouts = &descriptor_set_layout;
    pipeline_layout_create_info.pushConstantRangeCount = 0;
    pipeline_layout_create_info.pPushConstantRanges = 0;

    VK_ASSERT(vkCreatePipelineLayout(device, &pipeline_layout_create_info, 0, pipeline_layout));
}

internal void vk_create_graphics_pipeline(VkDevice device, VkRenderPass render_pass,
        VkPipeline *graphics_pipeline, VkPipelineCache pipeline_cache,
        VkShaderModule vertex_shader_module, VkShaderModule fragment_shader_module,
        VkPipelineLayout pipeline_layout, VkSampleCountFlagBits msaa_samples,
		uint32_t subpass_index)
{
    
    VkPipelineShaderStageCreateInfo vertex_shader_stage_create_info = {0};
    vertex_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertex_shader_stage_create_info.pNext = 0;
    vertex_shader_stage_create_info.flags = 0;
    vertex_shader_stage_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertex_shader_stage_create_info.module = vertex_shader_module;
    vertex_shader_stage_create_info.pName = "main";
    vertex_shader_stage_create_info.pSpecializationInfo = 0;

    VkPipelineShaderStageCreateInfo frag_shader_stage_create_info = {0};
    frag_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    frag_shader_stage_create_info.pNext = 0;
    frag_shader_stage_create_info.flags = 0;
    frag_shader_stage_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_shader_stage_create_info.module = fragment_shader_module;
    frag_shader_stage_create_info.pName = "main";
    frag_shader_stage_create_info.pSpecializationInfo = 0;

    VkPipelineShaderStageCreateInfo pipeline_shader_stages[] = {
        vertex_shader_stage_create_info,
        frag_shader_stage_create_info
    };

    VkVertexInputBindingDescription vertex_input_binding_description = get_vertex_input_binding_description();

	VkVertexInputAttributeDescription vertex_input_attribute_descriptions[3];
	get_vertex_input_attribute_descriptions(&vertex_input_attribute_descriptions[0]);

   VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {0};
   vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
   vertex_input_state_create_info.pNext = 0;
   vertex_input_state_create_info.vertexBindingDescriptionCount = 1;
   vertex_input_state_create_info.pVertexBindingDescriptions = &vertex_input_binding_description;
   vertex_input_state_create_info.vertexAttributeDescriptionCount = (uint32_t)ArraySize(vertex_input_attribute_descriptions);
   vertex_input_state_create_info.pVertexAttributeDescriptions = &vertex_input_attribute_descriptions[0];
   
   VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info = {0};
   input_assembly_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
   input_assembly_state_create_info.pNext = 0;
   input_assembly_state_create_info.flags = 0;
   // VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN
   // VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
   // VK_PRIMITIVE_TOPOLOGY_LINE_LIST
   input_assembly_state_create_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
   input_assembly_state_create_info.primitiveRestartEnable = VK_FALSE;

   VkPipelineTessellationStateCreateInfo tesselation_state_create_info = {0};
   tesselation_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
   tesselation_state_create_info.pNext = 0;
   tesselation_state_create_info.flags = 0;
   tesselation_state_create_info.patchControlPoints = 0;
   
    // VkViewport viewport = {};
    // VkRect2D scissor = {};

    // VkPipelineViewportStateCreateInfo will be ignored based on spec, because
    // we have enable dynamic_state_viewport and dynamic_state_scissor.
    // The numberr of viewports/scissor rectangles used by pipeline are still
    // specified by the viewportCount and scissorCount.
    VkPipelineViewportStateCreateInfo viewport_state_create_info = {0};
    viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state_create_info.pNext = 0;
    viewport_state_create_info.flags = 0;
    viewport_state_create_info.viewportCount = 1;
    viewport_state_create_info.pViewports = 0;
    viewport_state_create_info.scissorCount = 1;
    viewport_state_create_info.pScissors = 0;

    VkPipelineRasterizationStateCreateInfo rasterization_state_create_info = {0};
    rasterization_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization_state_create_info.pNext = 0;
    rasterization_state_create_info.flags = 0;
    rasterization_state_create_info.depthClampEnable = VK_FALSE;
    rasterization_state_create_info.rasterizerDiscardEnable = VK_FALSE;
    rasterization_state_create_info.polygonMode = VK_POLYGON_MODE_FILL;
    // VK_CULL_MODE_BACK_BIT
    // VK_CULL_MODE_FRONT_BIT
    rasterization_state_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
    // VK_FRONT_FACE_COUNTER_CLOCKWISE
    // VK_FRONT_FACE_CLOCKWISE
    rasterization_state_create_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterization_state_create_info.depthBiasEnable = VK_FALSE;
    rasterization_state_create_info.depthBiasConstantFactor = 0.0f;
    rasterization_state_create_info.depthBiasClamp = 0.0f;
    rasterization_state_create_info.depthBiasSlopeFactor = 0.0f;
    rasterization_state_create_info.lineWidth = 1.0f;

    VkPipelineMultisampleStateCreateInfo multisample_state_create_info = {0};
    multisample_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample_state_create_info.pNext = 0;
    multisample_state_create_info.flags = 0;
    multisample_state_create_info.rasterizationSamples = msaa_samples;
    multisample_state_create_info.sampleShadingEnable = VK_FALSE;
    multisample_state_create_info.minSampleShading = 1.0f; // Optional.
    multisample_state_create_info.pSampleMask = 0; // Optional.
    multisample_state_create_info.alphaToCoverageEnable = VK_FALSE; // Optional.
    multisample_state_create_info.alphaToOneEnable = VK_FALSE; // Optional.
   
    VkPipelineDepthStencilStateCreateInfo depth_stencil_state_create_info = {0};
    depth_stencil_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil_state_create_info.depthTestEnable = VK_TRUE;
    depth_stencil_state_create_info.depthWriteEnable = VK_TRUE;
    depth_stencil_state_create_info.depthCompareOp = VK_COMPARE_OP_LESS;
    depth_stencil_state_create_info.depthBoundsTestEnable = VK_FALSE;
    depth_stencil_state_create_info.minDepthBounds = 0.0f;
    depth_stencil_state_create_info.maxDepthBounds = 1.0f;
    depth_stencil_state_create_info.stencilTestEnable = VK_FALSE;
    // depth_stencil_state_create_info.front = {};
    // depth_stencil_state_create_info.back = {};

    // Fragment color is being passed unmodified.
    VkPipelineColorBlendAttachmentState color_blend_attachment_state = {0};
    color_blend_attachment_state.blendEnable = VK_FALSE;
    color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    color_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    color_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
    color_blend_attachment_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
/*
    // Alpha Blending.
    // finalColor.rgb = newAlpha * newColor + (1 - newAlpha) * oldColor;
    // finalColor.a = newAlpha.a;
    color_blend_attachment_state.blendEnable = VK_TRUE;
    color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    color_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
    color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    color_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;
*/

    VkPipelineColorBlendStateCreateInfo color_blend_state_create_info = {0};
    color_blend_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend_state_create_info.pNext = 0;
    color_blend_state_create_info.flags = 0;
    color_blend_state_create_info.logicOpEnable = VK_FALSE;
    // color_blend_state_create_info.logicOp
    color_blend_state_create_info.attachmentCount = 1;
    color_blend_state_create_info.pAttachments = &color_blend_attachment_state;
    color_blend_state_create_info.blendConstants[0] = 0.0f; // Optional
    color_blend_state_create_info.blendConstants[1] = 0.0f; // Optional
    color_blend_state_create_info.blendConstants[2] = 0.0f; // Optional
    color_blend_state_create_info.blendConstants[3] = 0.0f; // Optional


    VkDynamicState dynamic_states [] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {0};
    dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state_create_info.pNext = 0;
    dynamic_state_create_info.flags = 0;
    dynamic_state_create_info.dynamicStateCount = ArraySize(dynamic_states);
    dynamic_state_create_info.pDynamicStates = &dynamic_states[0];

    VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = {0};
    graphics_pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphics_pipeline_create_info.pNext = 0;
    graphics_pipeline_create_info.flags = 0;
    graphics_pipeline_create_info.stageCount = ArraySize(pipeline_shader_stages);
    graphics_pipeline_create_info.pStages = &pipeline_shader_stages[0];
    graphics_pipeline_create_info.pVertexInputState = &vertex_input_state_create_info;
    graphics_pipeline_create_info.pInputAssemblyState = &input_assembly_state_create_info;
    graphics_pipeline_create_info.pTessellationState = &tesselation_state_create_info;
    graphics_pipeline_create_info.pViewportState = &viewport_state_create_info;
    graphics_pipeline_create_info.pRasterizationState = &rasterization_state_create_info;
    graphics_pipeline_create_info.pMultisampleState = &multisample_state_create_info;
    graphics_pipeline_create_info.pDepthStencilState = &depth_stencil_state_create_info;
    graphics_pipeline_create_info.pColorBlendState = &color_blend_state_create_info;
    graphics_pipeline_create_info.pDynamicState = &dynamic_state_create_info;
    graphics_pipeline_create_info.layout = pipeline_layout;
    graphics_pipeline_create_info.renderPass = render_pass;
    graphics_pipeline_create_info.subpass = subpass_index;
    graphics_pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
    graphics_pipeline_create_info.basePipelineIndex = 0;

    VK_ASSERT(vkCreateGraphicsPipelines(device, pipeline_cache, 1, &graphics_pipeline_create_info, 0,
        graphics_pipeline));

    vkDestroyShaderModule(device, fragment_shader_module, 0);
    vkDestroyShaderModule(device, vertex_shader_module, 0);
}

internal void vk_create_material(platform_read_file *read_file, platform_free_file_memory *free_file_memory,
		const char *vertex_shader_file, const char *fragment_shader_file, VkDevice device,
		VkRenderPass render_pass, VkDescriptorSetLayout descriptor_set_layout,
		VkSampleCountFlagBits msaa_samples, uint32_t subpass_index, vk_material *material)
{
	VkShaderModule vertex_shader_module;
	VkShaderModule fragment_shader_module;

	vk_create_shader_module(read_file, free_file_memory, device, &vertex_shader_module, vertex_shader_file);
	vk_create_shader_module(read_file, free_file_memory, device, &fragment_shader_module, fragment_shader_file);

    vk_create_pipeline_layout(device, &material->pipeline_layout, descriptor_set_layout);
    vk_create_graphics_pipeline(device, render_pass, &material->graphics_pipeline,
        VK_NULL_HANDLE, vertex_shader_module, fragment_shader_module, material->pipeline_layout,
        msaa_samples, subpass_index);
}

internal void vk_destroy_material(VkDevice device, vk_material *material)
{
    vkDestroyPipeline(device, material->graphics_pipeline, 0);
    vkDestroyPipelineLayout(device, material->pipeline_layout, 0);
}

internal void vk_acquire_image(vk_renderer *renderer, uint64_t *query_results)
{
    uint32_t frame_counter = renderer->current_frame;
    ASSERT(frame_counter < ODS_FRAME_DATA_SIZE);

    vkWaitForFences(renderer->device, 1, &renderer->data[frame_counter].render_fence, VK_TRUE,
            UINT64_MAX);

    vkGetQueryPoolResults(renderer->device, renderer->query_pool, 0 + 2 * frame_counter, 2,
            2 * sizeof(uint64_t), &query_results[2 * frame_counter], sizeof(uint64_t), VK_QUERY_RESULT_64_BIT);

    renderer->recreate_wsi = vkAcquireNextImageKHR(renderer->device, renderer->swapchain, UINT64_MAX,
            renderer->data[frame_counter].image_available, VK_NULL_HANDLE, &renderer->image_index);

    if(renderer->recreate_wsi == VK_ERROR_OUT_OF_DATE_KHR)
    {
        renderer->recreate_wsi = VK_SUCCESS;
        // recreate_wsi(renderer);
    }
    else if(renderer->recreate_wsi != VK_SUCCESS && renderer->recreate_wsi != VK_SUBOPTIMAL_KHR)
    {
        ASSERT(0);
    }
}

internal void vk_present_image(vk_renderer *renderer)
{
    uint32_t frame_counter = renderer->current_frame;
    VkSubmitInfo submit_info = {0};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    
    VkSemaphore wait_semaphores[] = {
        renderer->data[frame_counter].image_available
    };

    VkPipelineStageFlags wait_stages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };

    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &wait_semaphores[0];
    submit_info.pWaitDstStageMask = &wait_stages[0];
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &renderer->data[frame_counter].command_buffer;

    VkSemaphore signal_semaphores[] ={
        renderer->data[frame_counter].render_finished
    };

    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &signal_semaphores[0];

    vkResetFences(renderer->device, 1, &renderer->data[frame_counter].render_fence);
    VK_ASSERT(vkQueueSubmit(renderer->graphics_queue, 1, &submit_info,
                renderer->data[frame_counter].render_fence));

    VkPresentInfoKHR present_info = {0};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = ArraySize(signal_semaphores);
    present_info.pWaitSemaphores = &signal_semaphores[0];

    VkSwapchainKHR swapchains[] = {
        renderer->swapchain
    };

    present_info.swapchainCount = 1;
    present_info.pSwapchains = &swapchains[0];
    present_info.pImageIndices = &renderer->image_index;
    present_info.pResults = 0;

    renderer->recreate_wsi = vkQueuePresentKHR(renderer->graphics_queue, &present_info);

    renderer->current_frame = (renderer->current_frame + 1) % renderer->total_frames;

    if(renderer->recreate_wsi == VK_ERROR_OUT_OF_DATE_KHR || renderer->recreate_wsi == VK_SUBOPTIMAL_KHR)
    {
        renderer->recreate_wsi = VK_SUCCESS;
        // recreate_wsi(renderer);
    }
    else if(renderer->recreate_wsi != VK_SUCCESS)
    {
        ASSERT(0);
    }
}

internal void vk_command_buffer_recording(VkCommandBuffer command_buffer, VkRenderPass render_pass,
        VkFramebuffer framebuffer, vk_material *material, VkExtent2D extent,
        VkBuffer buffer, uint32_t vertex_buffer_size, uint16_t index_buffer_size,
        VkDescriptorSet global_descriptor_set,
		vk_material *read_material, VkDescriptorSet framebuffer_descriptor_set,
		VkBuffer rect_buffer, uint32_t rect_vertex_buffer_size, uint16_t rect_index_buffer_size,
		VkQueryPool query_pool, uint32_t frame_counter)
{
    vkResetCommandBuffer(command_buffer, 0);

    VkCommandBufferBeginInfo command_buffer_begin_info = {0};
    command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    command_buffer_begin_info.pNext = 0;
    command_buffer_begin_info.flags = 0;
    command_buffer_begin_info.pInheritanceInfo = 0;

    VK_ASSERT(vkBeginCommandBuffer(command_buffer, &command_buffer_begin_info));

    vkCmdResetQueryPool(command_buffer, query_pool, 0 + 2 * frame_counter, 2);

    vkCmdWriteTimestamp(command_buffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, query_pool, 0 + 2 * frame_counter);

    VkViewport viewport = {0};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)extent.width;
    viewport.height = (float)extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {
		.offset = {.x = 0, .y = 0},
		extent = extent
	};

    vkCmdSetViewport(command_buffer, 0, 1, &viewport);
    vkCmdSetScissor(command_buffer, 0, 1, &scissor);
    
    VkRenderPassBeginInfo render_pass_begin_info = {0};
    render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_begin_info.renderPass = render_pass;
    render_pass_begin_info.framebuffer = framebuffer;
    render_pass_begin_info.renderArea.offset.x = 0;
    render_pass_begin_info.renderArea.offset.y = 0;
    render_pass_begin_info.renderArea.extent = extent;

    VkClearValue clear_color_value = {{{0.2f, 0.4f, 1.0f, 1.0f}}};
    VkClearValue depth_color_value = {{{1.0f, 0.0f, 0.0f, 0.0f}}};

    VkClearValue clear_values[] = {
        clear_color_value,
        depth_color_value,
		clear_color_value
    };

    render_pass_begin_info.clearValueCount = ArraySize(clear_values);
    render_pass_begin_info.pClearValues = &clear_values[0];

    vkCmdBeginRenderPass(command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

    VkBuffer vertex_buffers [] = {
        buffer
    };
    VkDeviceSize offsets [] = {
        0
    };

    vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material->graphics_pipeline);
    vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffers, offsets);
    vkCmdBindIndexBuffer(command_buffer, buffer, vertex_buffer_size, VK_INDEX_TYPE_UINT16);

    vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material->pipeline_layout,
        0, 1, &global_descriptor_set, 0, 0); 
    vkCmdDrawIndexed(command_buffer, index_buffer_size, 1, 0, 0, 0);
	vkCmdNextSubpass(command_buffer, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, read_material->graphics_pipeline);
	vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, read_material->pipeline_layout,
			0, 1, &framebuffer_descriptor_set, 0, 0);

	VkBuffer rect_vertex_buffers [] = {
		rect_buffer
	};

	vkCmdBindVertexBuffers(command_buffer, 0, 1, rect_vertex_buffers, offsets);
	vkCmdBindIndexBuffer(command_buffer, rect_buffer, rect_vertex_buffer_size, VK_INDEX_TYPE_UINT16);
	

	vkCmdDrawIndexed(command_buffer, rect_index_buffer_size, 1, 0, 0, 0);

    vkCmdEndRenderPass(command_buffer);

    vkCmdWriteTimestamp(command_buffer, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, query_pool, 1 + 2 * frame_counter);
    VK_ASSERT(vkEndCommandBuffer(command_buffer));
}

internal void vk_update_global_resources(VkDevice device, VkDeviceMemory uniform_buffer_memory,
        void *camera, uint32_t offset)
{
	game_camera *cam = (game_camera*)camera;
    void *data;
    vkMapMemory(device, uniform_buffer_memory, offset, sizeof(game_camera), 0, &data);
    memcpy(data, cam, sizeof(game_camera));
    vkUnmapMemory(device, uniform_buffer_memory);
}
