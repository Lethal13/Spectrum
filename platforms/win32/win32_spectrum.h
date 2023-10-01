#ifndef PLATFORMS_WIN32_SPECTRUM_H
#define PLATFORMS_WIN32_SPECTRUM_H

#include "../../core/spectrum.h"

#include <stdio.h>
#include <Windows.h>
#include <Xinput.h>
#include "../../renderer/vulkan/spectrum_vulkan.c"

typedef struct
{
	HMODULE game_code_dll;
	game_update_and_render *update_and_render;
	FILETIME dll_last_write_time;
} win32_game_code;

internal uint64_t get_os_timer_freq(void);
internal uint64_t read_os_timer(void);
inline uint64_t read_cpu_timer(void);
inline uint64_t estimate_cpu_freq(void);

PLATFORM_FREE_FILE_MEMORY(win32_free_file_memory);
PLATFORM_READ_FILE(win32_read_file);
PLATFORM_WRITE_FILE(win32_write_file);
PLATFORM_PRINT(win32_print);

internal void toggle_fullscreen(HWND hwnd);
inline FILETIME win32_get_last_write_time(char *filename);
internal win32_game_code win32_load_game_code(char *const source_dll);
internal int32_t win32_load_xinput();
internal void win32_get_controller_input(game_input *old_input, game_input *new_input, DWORD controller_index);
internal void win32_handle_window_messages(HWND hwnd, game_input *old_input, game_input *new_input);
internal HWND win32_create_window(HINSTANCE hInstance, int nCmdShow);

#endif
