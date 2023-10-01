#ifndef RENDERER_VULKAN_SPECTRUM_VULKAN_H
#define RENDERER_VULKAN_SPECTRUM_VULKAN_H

#ifndef VK_NO_PROTOTYPES
#define VK_NO_PROTOTYPES
#endif

#include <vulkan/vulkan.h>

#if SPECTRUM_DEBUG
#define VK_ASSERT(result) \
        do { \
                VkResult result_ = result; \
                ASSERT(result_ == VK_SUCCESS); \
           } while(0)
#else
#define VK_ASSERT(result)
#endif

#define VK_EXPORTED_FUNCTION( name ) static PFN_##name name;
#define VK_GLOBAL_LEVEL_FUNCTION( name ) static PFN_##name name;
#define VK_INSTANCE_LEVEL_FUNCTION( name ) static PFN_##name name;
#define VK_DEVICE_LEVEL_FUNCTION( name ) static PFN_##name name;

#define VK_INSTANCE_LEVEL_FUNCTION_LOAD( loader_instance, name )    \
    name = (PFN_##name)vkGetInstanceProcAddr((loader_instance), #name);

#define VK_DEVICE_LEVEL_FUNCTION_LOAD( loader_device, name  ) \
    name = (PFN_##name)vkGetDeviceProcAddr((loader_device), #name);

#define ODS_FRAME_DATA_SIZE 2

struct game_camera; // Forward declaration.

typedef struct
{
	VkSurfaceCapabilitiesKHR surface_capabilities;
	// TODO: Replace 50/10 with contants both here
	// and in functions that uses surface_details.
	VkSurfaceFormatKHR surface_formats[50];
	VkPresentModeKHR present_modes[10];
	uint32_t available_surface_formats_counter;
	uint32_t available_present_modes_counter;
} surface_details;

typedef struct
{
	_Alignas(16) vec3_t position;
	_Alignas(16) vec3_t base_color;
	_Alignas(16) vec3_t normal;

} vertex;

internal VkVertexInputBindingDescription get_vertex_input_binding_description()
{
	VkVertexInputBindingDescription vertex_input_binding_description = {0};
	vertex_input_binding_description.binding = 0;
	vertex_input_binding_description.stride = sizeof(vertex);
	// VK_VERTEX_INPUT_RATE_VERTEX: Move the next data entry after each vertex.
	// VK_VERTEX_INPUT_RATE_INSTANCE: Move to the next data entry after each instance.
	vertex_input_binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return vertex_input_binding_description;
}

internal void get_vertex_input_attribute_descriptions(VkVertexInputAttributeDescription attribute_descriptions[3])
{
	attribute_descriptions[0].binding = 0;
	attribute_descriptions[0].location = 0;
	attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attribute_descriptions[0].offset = offsetof(vertex, position);

	attribute_descriptions[1].binding = 0;
	attribute_descriptions[1].location = 1;
	attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attribute_descriptions[1].offset = offsetof(vertex, base_color);

	attribute_descriptions[2].binding = 0;
	attribute_descriptions[2].location = 2;
	attribute_descriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
	attribute_descriptions[2].offset = offsetof(vertex, normal);
}

typedef struct
{
	VkSemaphore image_available;
	VkSemaphore render_finished;
	VkFence render_fence;

	VkCommandPool command_pool;
	VkCommandBuffer command_buffer;
	VkFramebuffer framebuffer;
	// VkFramebuffer final_framebuffer;

	VkImage depth_image;
	VkImageView depth_image_view;
	VkDeviceMemory depth_image_memory;

	VkImage color_image;
	VkDeviceMemory color_image_memory;
	VkImageView color_image_view;
} frame_data;

typedef struct
{
	VkPipeline graphics_pipeline;
	VkPipelineLayout pipeline_layout;
} vk_material;

typedef struct
{
	VkInstance instance;
	VkDebugUtilsMessengerEXT debug_messenger;
	VkSurfaceKHR surface;
	VkPhysicalDevice physical_device;

	VkDevice device;
	VkQueue graphics_queue;
	uint32_t graphics_queue_index;

    // Swapchain is an abstraction for an array of presentable images that are associated
    // with a surface.
    VkSwapchainKHR swapchain;
    VkImage swapchain_images[ODS_FRAME_DATA_SIZE];
    VkImageView swapchain_image_views[ODS_FRAME_DATA_SIZE];
    VkFormat swapchain_image_format;
    VkExtent2D swapchain_image_extent;

    uint32_t total_frames;
    uint32_t current_frame;
    VkFormat depth_image_format; 
    VkBool32 recreate_wsi;
	uint32_t image_index;

    // Stores the number of nanoseconds required for a timestamp query to be incremented by 1.
    float timestamp_period;
	VkSampleCountFlagBits msaa_samples;

    VkQueryPool query_pool;

	frame_data data[ODS_FRAME_DATA_SIZE];

	VkRenderPass render_pass;

    VkBuffer uniform_buffer;
    VkDeviceMemory uniform_buffer_memory;
    VkDescriptorSetLayout global_descriptor_set_layout;
    VkDescriptorPool global_descriptor_pool;
    // We could move global_descriptor_set inside FrameData structure.
    VkDescriptorSet global_descriptor_sets[ODS_FRAME_DATA_SIZE];

	VkDescriptorSetLayout final_descriptor_set_layout;
	VkDescriptorPool final_descriptor_pool;
	VkDescriptorSet final_descriptor_sets[ODS_FRAME_DATA_SIZE];

	vk_material first_material;
	vk_material final_material;

	vertex *vertices;
	uint16_t *indices;
	uint32_t total_vertices;
	uint16_t total_indices;
	VkBuffer vk_buffer;
	VkDeviceMemory vk_buffer_memory;
	uint32_t vertex_buffer_size;
	uint16_t index_buffer_size;

	vertex *rect_vertices;
	uint16_t *rect_indices;
	uint32_t total_rect_vertices;
	uint16_t total_rect_indices;
	VkBuffer rect_buffer;
	VkDeviceMemory rect_buffer_memory;
	uint32_t rect_vertex_buffer_size;
	uint16_t rect_index_buffer_size;

} vk_renderer;

internal VkResult load_vulkan(void);
internal void load_vk_global_functions(void);
internal void load_vk_instance_functions(VkInstance instance);
internal void load_vk_device_functions(VkDevice device);
internal VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_type,
    const VkDebugUtilsMessengerCallbackDataEXT *p_callback_data,
    void *p_user_data);
internal uint32_t vk_init(vk_renderer *renderer, platform_read_file *read_file,
	   platform_free_file_memory *free_file_memory,	void *extra);
internal void vk_deinit(vk_renderer *renderer);
internal void vk_create_instance(VkInstance *instance);
internal void vk_init_debug_messenger(VkInstance instance, VkDebugUtilsMessengerEXT *debug_messenger);
internal void vk_create_surface(VkInstance instance, VkSurfaceKHR *surface, void *extra);
internal uint32_t vk_pick_physical_device(VkInstance instance, VkPhysicalDevice *physical_device,
		VkSurfaceKHR surface);
internal VkBool32 physical_device_surface_support(VkPhysicalDevice physical_device, VkSurfaceKHR surface);
internal void vk_create_device(vk_renderer *renderer);
internal VkBool32 get_graphics_queue_family(VkPhysicalDevice physical_device, VkSurfaceKHR surface,
    uint32_t *graphics_queue_index);
internal VkSurfaceFormatKHR vk_choose_swapchain_format(VkSurfaceFormatKHR *available_surface_formats,
		uint32_t available_surface_formats_counter);
internal VkPresentModeKHR vk_choose_swapchain_present_mode(VkPresentModeKHR *available_present_modes,
		uint32_t available_present_modes_counter);
internal VkExtent2D vk_choose_swapchain_extent(VkSurfaceCapabilitiesKHR *surface_capabilities);
internal surface_details vk_get_surface_details(VkPhysicalDevice physical_device, VkSurfaceKHR surface);
internal void vk_create_swapchain(VkSurfaceKHR surface, VkPhysicalDevice physical_device,
		VkDevice device, VkSwapchainKHR *swapchain, VkImage *swapchain_images,
		VkFormat *swapchain_image_format, VkExtent2D *swapchain_image_extent,
		VkSwapchainKHR old_swapchain);
internal void vk_create_swapchain_image_views(VkDevice device, VkImage *swapchain_images,
        VkImageView *swapchain_image_views, VkFormat swapchain_image_format);
internal void vk_create_query_pool(VkDevice device, VkQueryPool *query_pool);
internal void vk_create_render_pass(VkDevice device, VkRenderPass *render_pass,
		VkFormat color_attachment_format, VkFormat depth_attachment_format,
		VkSampleCountFlagBits msaa_samples);
internal void vk_init_frame_data(vk_renderer *renderer);
internal void vk_deinit_frame_data(vk_renderer *renderer);
internal void vk_create_command_pool(VkDevice device, VkCommandPool *command_pool, uint32_t queue_family_index);
internal void vk_create_command_buffers(VkDevice device, VkCommandPool command_pool,
    VkCommandBuffer *command_buffer, uint32_t command_buffers_size);
internal void vk_create_sync_objects(VkDevice device, VkSemaphore *image_available, VkSemaphore *render_finished,
    VkFence *render_fence);
static void vk_create_framebuffer(VkDevice device, VkRenderPass render_pass,
        VkImageView image_view, VkImageView depth_image_view, VkImageView color_image_view,
        VkFramebuffer *framebuffer, VkExtent2D extent);
internal void vk_create_depth_resource(VkPhysicalDevice physical_device, VkDevice device,
        VkImage *depth_image, VkDeviceMemory *depth_image_memory, VkImageView *depth_image_view,
        VkFormat depth_image_format, VkSampleCountFlagBits samples, VkExtent2D extent,
        const uint32_t *const queue_index);
internal void vk_destroy_depth_resources(VkDevice device, VkImage depth_image,
        VkImageView depth_image_view, VkDeviceMemory depth_image_memory);
internal void vk_create_global_resources(VkPhysicalDevice physical_device, VkDevice device,
        VkBuffer *uniform_buffer, VkDeviceMemory *uniform_buffer_memory,
        VkDescriptorSetLayout *descriptor_set_layout, VkDescriptorPool *descriptor_pool,
        VkDescriptorSet *descriptor_sets, uint32_t descriptor_max_sets, size_t original_size);
internal void vk_destroy_global_resources(VkDevice device, VkBuffer uniform_buffer,
        VkDeviceMemory uniform_buffer_memory, VkDescriptorSetLayout global_descriptor_set_layout,
        VkDescriptorPool global_descriptor_pool);
internal void vk_create_final_resources(VkDevice device, VkImageView color_image_view,
		VkImageView depth_image_view, VkImageView color_image_view1,
		VkImageView depth_image_view1, VkDescriptorSetLayout *descriptor_set_layout,
		VkDescriptorPool *descriptor_pool, VkDescriptorSet *descriptor_sets);
internal void vk_final_global_resources(VkDevice device, VkDescriptorSetLayout final_descriptor_set_layout,
        VkDescriptorPool final_descriptor_pool);
internal void vk_create_buffer(VkPhysicalDevice physical_device, VkDevice device,
    VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
    VkBuffer *buffer, VkDeviceMemory *buffer_memory);
internal void vk_create_image(VkPhysicalDevice physical_device, VkDevice device,
		VkImage *image, VkDeviceMemory *image_memory, VkFormat format,
		VkExtent3D extent, VkImageTiling tiling, VkImageUsageFlags usage,
		const uint32_t *const queue_index, VkImageLayout layout,
		VkSampleCountFlagBits samples);
internal void vk_allocate_mesh_gpu_local(VkPhysicalDevice physical_device, VkDevice device,
        VkQueue queue, VkCommandPool command_pool, VkBuffer *buffer, VkDeviceMemory *buffer_memory,
        void *vertices_data, void *indices_data, VkDeviceSize index_buffer_size,
        VkDeviceSize vertex_buffer_size);
internal void vk_create_buffer(VkPhysicalDevice physical_device, VkDevice device,
    VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
    VkBuffer *buffer, VkDeviceMemory *buffer_memory);
internal void vk_copy_buffer(VkDevice device, VkQueue queue, VkCommandPool command_pool,
    VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize buffer_size);
internal void vk_destroy_buffer(VkDevice device, VkBuffer buffer, VkDeviceMemory buffer_memory);
internal uint32_t vk_find_memory_type(VkPhysicalDevice physical_device, uint32_t type_filter,
    VkMemoryPropertyFlags properties);

internal void vk_create_shader_module(platform_read_file *read_file, platform_free_file_memory *free_file_memory,
        VkDevice device, VkShaderModule *shader_module, const char *shader);
internal void vk_create_pipeline_layout(VkDevice device, VkPipelineLayout *pipeline_layout,
		VkDescriptorSetLayout descriptor_set_layout);
internal void vk_create_graphics_pipeline(VkDevice device, VkRenderPass render_pass,
        VkPipeline *graphics_pipeline, VkPipelineCache pipeline_cache,
        VkShaderModule vertex_shader_module, VkShaderModule fragment_shader_module,
        VkPipelineLayout pipeline_layout, VkSampleCountFlagBits msaa_samples,
		uint32_t subpass_index);
internal void vk_create_material(platform_read_file *read_file, platform_free_file_memory *free_file_memory,
		const char *vertex_shader_file, const char *fragment_shader_file, VkDevice device,
		VkRenderPass render_pass, VkDescriptorSetLayout descriptor_set_layout,
		VkSampleCountFlagBits msaa_samples, uint32_t subpass_index, vk_material *material);
internal void vk_destroy_material(VkDevice device, vk_material *material);
internal void vk_acquire_image(vk_renderer *renderer, uint64_t *query_results);
internal void vk_present_image(vk_renderer *renderer);
internal void vk_command_buffer_recording(VkCommandBuffer command_buffer, VkRenderPass render_pass,
        VkFramebuffer framebuffer, vk_material *material, VkExtent2D extent,
        VkBuffer buffer, uint32_t vertex_buffer_size, uint16_t index_buffer_size,
        VkDescriptorSet global_descriptor_set,
		vk_material *read_material, VkDescriptorSet framebuffer_descriptor_set,
		VkBuffer rect_buffer, uint32_t rect_vertex_buffer_size, uint16_t rect_index_buffer_size,
		VkQueryPool query_pool, uint32_t frame_counter);
internal void vk_update_global_resources(VkDevice device, VkDeviceMemory uniform_buffer_memory,
        void *camera, uint32_t offset);
#endif
