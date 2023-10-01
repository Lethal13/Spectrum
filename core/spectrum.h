#ifndef CORE_SPECTRUM_H
#define CORE_SPECTRUM_H

#if SPECTRUM_DEBUG
#define ASSERT(expression) if(!(expression)) {*(volatile int*)0 = 0;}
#else
#define ASSERT(expression)
#endif

#define internal static
#define local_persist static
#define global_variable static

#define ArraySize(array) (sizeof(array)/sizeof((array)[0]))
#define Min(A, B) ((A < B) ? (A) : (B))
#define Max(A, B) ((A > B) ? (A) : (B))

#define BUTTON_PRESSED 0x1
#define BUTTON_RELEASED 0x2

#define Kilobytes(value) ((value)*1024LL)
#define Megabytes(value) (Kilobytes(value)*1024LL)
#define Gigabytes(value) (Megabytes(value)*1024LL)
#define Terabytes(value) (Gigabytes(value)*1024LL)

#define MAX_PLAYERS 1

#define M_PI 3.141592f

#include <stdint.h>
#include <stdalign.h>

#define PLATFORM_FREE_FILE_MEMORY(name) void name(void **data)
typedef PLATFORM_FREE_FILE_MEMORY(platform_free_file_memory);

#define PLATFORM_READ_FILE(name) int32_t name(const char *file_name, uint32_t *size, void **data)
typedef PLATFORM_READ_FILE(platform_read_file);

#define PLATFORM_WRITE_FILE(name) int32_t name(const char *file_name, uint32_t size, void **data)
typedef PLATFORM_WRITE_FILE(platform_write_file);

#define PLATFORM_PRINT(name) void name(char *text)
typedef PLATFORM_PRINT(platform_print);

#include "../math/vec3.h"
#include "../math/vec4.h"
#include "../math/mat4.h"
#include "../renderer/vulkan/spectrum_vulkan.h"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;

typedef uint8_t b8;

typedef struct
{
	uint32_t state;
} game_button_state;

typedef struct
{
	game_button_state up;
	game_button_state down;
	game_button_state right;
	game_button_state left;
} game_keyboard_input;

typedef struct
{
	uint32_t x;
	uint32_t y;

	game_button_state left_button;
	game_button_state right_button;
	game_button_state middle_button;
	game_button_state extra_button1;
	game_button_state extra_button2;
} game_mouse_input;

typedef struct
{
    int32_t is_connected;
    int32_t is_analog;

    float stick_average_l;
    float stick_average_lx;
    float stick_average_ly;
    float stick_average_r;
    float stick_average_rx;
    float stick_average_ry;

    game_button_state dpad_up;
    game_button_state dpad_down;
    game_button_state dpad_left;
    game_button_state dpad_right;

    game_button_state action_up;
    game_button_state action_down;
    game_button_state action_left;
    game_button_state action_right;

    game_button_state left_shoulder;
    game_button_state right_shoulder;

    // NOTE: Maybe we should use float.
    game_button_state back_left_trigger;
    game_button_state back_right_trigger;

    game_button_state right_thumb;
    game_button_state left_thumb;

    game_button_state back; // or select
    game_button_state start;
} game_controller_input;

typedef struct
{
	game_mouse_input mouse;
	game_keyboard_input keyboard;
	game_controller_input controllers[MAX_PLAYERS];
} game_input;

typedef struct
{
	uint8_t is_initialized;
	uint64_t permanent_storage_size;
	void *permanent_storage;
	uint64_t transient_storage_size;
	void *transient_storage;

	platform_read_file *read_file;
	platform_write_file *write_file;
	platform_free_file_memory *free_file_memory;
	platform_print *print;
} game_memory;

typedef struct
{
	double frame_time;
	double t;
	double dt;
	double accumulator;
} game_state;

typedef struct
{
	mat4_t model_matrix;
	mat4_t view_matrix;
	mat4_t projection_matrix;
} game_camera;

#define GAME_UPDATE_AND_RENDER(name) void name(game_memory *memory, game_input *inputs, game_camera *camera, vk_renderer *renderer)
typedef GAME_UPDATE_AND_RENDER(game_update_and_render);

#endif
