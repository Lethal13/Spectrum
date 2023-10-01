#include "win32_spectrum.h"

global_variable uint32_t window_is_active;
global_variable WINDOWPLACEMENT window_position = {sizeof(window_position)};

internal uint64_t get_os_timer_freq(void)
{
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	return freq.QuadPart;
}

internal uint64_t read_os_timer(void)
{
	LARGE_INTEGER value;
	QueryPerformanceCounter(&value);
	return value.QuadPart;
}

inline uint64_t read_cpu_timer(void)
{
	return __rdtsc();
}

inline uint64_t estimate_cpu_freq(void)
{
	uint64_t milliseconds_to_wait = 100;
	uint64_t os_freq = get_os_timer_freq();

	uint64_t cpu_start = read_cpu_timer();
	uint64_t os_start = read_os_timer();
	uint64_t os_end = 0;
	uint64_t os_elapsed = 0;
	uint64_t os_wait_time = os_freq * milliseconds_to_wait / 1000;
	while(os_elapsed < os_wait_time)
	{
		os_end = read_os_timer();
		os_elapsed = os_end - os_start;
	}

	uint64_t cpu_end = read_cpu_timer();
	uint64_t cpu_elapsed = cpu_end - cpu_start;
	
	uint64_t cpu_freq = 0;
	if(os_elapsed)
	{
		cpu_freq = os_freq * cpu_elapsed / os_elapsed;
	}

	return cpu_freq;
}

PLATFORM_FREE_FILE_MEMORY(win32_free_file_memory)
{
    if(*data)
    {
        VirtualFree(*data, 0, MEM_RELEASE);
		*data = 0;
    }
}

PLATFORM_READ_FILE(win32_read_file)
{
    HANDLE file_handle = CreateFile(file_name, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if(file_handle != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER file_size;
        if(GetFileSizeEx(file_handle, &file_size))
        {
            // TODO: Defines for maximum values.
            uint32_t file_size_32 = file_size.LowPart;
            *data = VirtualAlloc(0, file_size_32, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);

            if(*data)
            {
                DWORD bytes_read;
                if(ReadFile(file_handle, *data, file_size_32, &bytes_read, 0) &&
                  (file_size_32 == bytes_read))
                {
                    //NOTE: File read was successfull.
                    *size = file_size_32;
                    CloseHandle(file_handle);
                    return 0;
                }
                else
                {
                    win32_free_file_memory(data);
                    *data = 0;
                    return 1;
                    //TODO: Logging.
                    //DWORD error =  GetLastError();
                }
            }
            else
            {
                // TODO: Logging
            }

        }
        else
        {
            // TODO: Logging.
        }
    }
    else
    {
        // TODO: Logging.
    }

    CloseHandle(file_handle);
    return 1;
}

PLATFORM_WRITE_FILE(win32_write_file)
{
    HANDLE file_handle = CreateFile(file_name, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);

    if(file_handle != INVALID_HANDLE_VALUE)
    {
        DWORD bytes_written;

        //NOTE: File read successfully.
        if(WriteFile(file_handle, data, size, &bytes_written, 0))
        {
            uint32_t result = (bytes_written == size);
            CloseHandle(file_handle);
            if(result != 0)
            {
                return 0;
            }
            else
            {
                // TODO: Logging.
                return 1;
            }
        }
        else
        {
            // TODO: Logging.
            return 1;
        }

    }
    else
    {
        //TODO: Logging.
    }

    return 1;
}

PLATFORM_PRINT(win32_print)
{
    OutputDebugStringA(text);
}


// NOTE: For fullscreen toggling, see:
// http://blogs.msdn.com/b/oldnewthing/archive/2010/04/12/9994016.aspx
internal void toggle_fullscreen(HWND hwnd)
{
    DWORD style = GetWindowLong(hwnd, GWL_STYLE);
    if(style & WS_OVERLAPPEDWINDOW)
    {
        MONITORINFO monitor_info = {sizeof(monitor_info)};
        if(GetWindowPlacement(hwnd, &window_position) &&
           GetMonitorInfo(MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY), &monitor_info))
        {
            SetWindowLong(hwnd, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);
            SetWindowPos(hwnd, HWND_TOP,
                         monitor_info.rcMonitor.left, monitor_info.rcMonitor.top,
                         monitor_info.rcMonitor.right - monitor_info.rcMonitor.left,
                         monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top,
                         SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        }
    }
    else
    {
        SetWindowLong(hwnd, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(hwnd, &window_position);
        SetWindowPos(hwnd, 0, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                     SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
}

inline FILETIME win32_get_last_write_time(char *filename)
{
	FILETIME last_write_time = {0};

	WIN32_FILE_ATTRIBUTE_DATA data;
	if(GetFileAttributesEx(filename, GetFileExInfoStandard, &data))
	{
		last_write_time = data.ftLastWriteTime;
	}

	return last_write_time;
}

internal win32_game_code win32_load_game_code(char *const source_dll)
{
	win32_game_code result = {0};

	result.dll_last_write_time = win32_get_last_write_time(source_dll);

	result.game_code_dll = LoadLibraryA(source_dll);
	result.update_and_render = (game_update_and_render*)GetProcAddress(result.game_code_dll, "update_and_render");

	// TODO: Check if the code was loaded succesfully.
	// If GetProcAddress fails, return NULL(0).
	return result;
}

// https://guide.handmadehero.org/code/day006/  
// NOTE: XInputGetState
#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE *pState)
typedef X_INPUT_GET_STATE(x_input_get_state);
internal X_INPUT_GET_STATE(XInputGetStateStub)
{
    return(ERROR_DEVICE_NOT_CONNECTED);
}
global_variable x_input_get_state *XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_

// NOTE: XInputSetState
#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state);
internal X_INPUT_SET_STATE(XInputSetStateStub)
{
    return(ERROR_DEVICE_NOT_CONNECTED);
}
global_variable x_input_set_state *XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_

internal int32_t win32_load_xinput()
{
    HMODULE x_input_library = LoadLibraryExA("Xinput1_4.dll", 0, 0);

    if(!x_input_library)
    {
        x_input_library = LoadLibraryExA("Xinput9_1_0.dll", 0, 0);
    }

    if(!x_input_library)
    {
        x_input_library = LoadLibraryExA("Xinput1_3.dll", 0, 0);
    }

    if(!x_input_library)
    {
        // xinput dll, was not found.
		return 1;
    }

    XInputGetState = (x_input_get_state*)GetProcAddress(x_input_library, "XInputGetState");
    XInputSetState = (x_input_set_state*)GetProcAddress(x_input_library, "XInputSetState");
    return 0;
}

internal void win32_get_controller_input(game_input *old_input, game_input *new_input, DWORD controller_index)
{
    XINPUT_STATE controller_input_state = {0};

    // controller_index can be a value from 0 to 3. (Up to 4 controllers at the same time).
    // If the function fails the return value is an error code defined in Winerror.h
    // The functiuon does not use SetLastError to set the calling thread's last error code.
    DWORD result = XInputGetState(controller_index, &controller_input_state);

    if(result == ERROR_SUCCESS)
    {
        // Controller is connected.

        // TODO: Check if dwPacketNumber has changed between two sequential calls
        // to XInputGetState

        // game_controller_input *old_controller_input = &old_input->controllers[controller_index];
        game_controller_input *new_controller_input = &new_input->controllers[controller_index];

        // new_controller_input->packet_number = controller_input_state.dwPacketNumber;
        new_controller_input->is_connected = 1;

        uint32_t dpad_up = controller_input_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP;
        uint32_t dpad_down = controller_input_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
        uint32_t dpad_left = controller_input_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
        uint32_t dpad_right = controller_input_state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;
        uint32_t start = controller_input_state.Gamepad.wButtons & XINPUT_GAMEPAD_START;
        uint32_t back = controller_input_state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK;
        uint32_t left_thumb = controller_input_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB;
        uint32_t right_thumb = controller_input_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB;
        uint32_t left_shoulder = controller_input_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER;
        uint32_t right_shoulder = controller_input_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER;
        uint32_t button_a = controller_input_state.Gamepad.wButtons & XINPUT_GAMEPAD_A;
        uint32_t button_b = controller_input_state.Gamepad.wButtons & XINPUT_GAMEPAD_B;
        uint32_t button_x = controller_input_state.Gamepad.wButtons & XINPUT_GAMEPAD_X;
        uint32_t button_y = controller_input_state.Gamepad.wButtons & XINPUT_GAMEPAD_Y;
        uint32_t back_left_trigger = controller_input_state.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD ? controller_input_state.Gamepad.bLeftTrigger : 0;
        uint32_t back_right_trigger = controller_input_state.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD ? controller_input_state.Gamepad.bRightTrigger : 0;

        float stick_lx = controller_input_state.Gamepad.sThumbLX;
        float stick_ly = controller_input_state.Gamepad.sThumbLY;
        float stick_rx = controller_input_state.Gamepad.sThumbRX;
        float stick_ry = controller_input_state.Gamepad.sThumbRY;

        // determine how far the left stick is pushed.
        float magnitude_l = (float)sqrt(stick_lx * stick_lx + stick_ly * stick_ly);

        // determine the direction the left stick is pushed.
        float normalized_lx = stick_lx / magnitude_l;
        float normalized_ly = stick_ly / magnitude_l;

        float normalized_magnitude_l = 0.0f;

        // check if the left stick is outside the circular dead zone.
        if(magnitude_l > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
        {
            // clip the magnitude at its expected maximum value.
            if(magnitude_l > 32767) magnitude_l = 32767;

            // adjust magnitude relative to the end of the dead zone.
            magnitude_l -= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;

            // optionally normalize the magnitude with respect to its
            // expected range giving a magnitude value of 0.0 to 1.0 .
            normalized_magnitude_l = magnitude_l / (32767 - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
        }
        else // if the left stick is in the deadzone, zero out the magnitude.
        {
            magnitude_l = 0.0f;
            normalized_magnitude_l = 0.0f;
            normalized_lx = 0.0f;
            normalized_ly = 0.0f;
        }

        // determine how far the right stick has been pushed.
        float magnitude_r = (float)sqrt(stick_rx * stick_rx + stick_ry * stick_ry);

        // determine the direction the controller is pushed.
        float normalized_rx = stick_rx / magnitude_r;
        float normalized_ry = stick_ry / magnitude_r;

        float normalized_magnitude_r = 0.0f;

        // check if the right stick is outside the circular dead zone.
        if(magnitude_r > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
        {
            // clip the magnitude to its expected maximum value.
            if(magnitude_r > 32767) magnitude_r = 32767;

            // adjust magnitude relative to the end of the dead zone.
            magnitude_r -= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;

            // Optionally normalize the magnitude with respect to its
            // expected range giving a magnitude value of 0.0 to 1.0 .
            normalized_magnitude_r = magnitude_r / (32767 - XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
        } // if the right stick is in the deadzone, zero out the magnitude.
        else
        {
            magnitude_r = 0.0f;
            normalized_magnitude_r = 0.0f;
            normalized_rx = 0.0f;
            normalized_ry = 0.0f;
        }

        // new_controller_input->packet_number = packet_number;
        new_controller_input->stick_average_l = normalized_magnitude_l;
        new_controller_input->stick_average_r = normalized_magnitude_r;
        new_controller_input->stick_average_lx = normalized_lx;
        new_controller_input->stick_average_ly = normalized_ly;
        new_controller_input->stick_average_rx = normalized_rx;
        new_controller_input->stick_average_ry = normalized_ry;
        new_controller_input->dpad_up.state = dpad_up;
        new_controller_input->dpad_down.state = dpad_down;
        new_controller_input->dpad_left.state = dpad_left;
        new_controller_input->dpad_right.state = dpad_right;
        new_controller_input->action_up.state = button_y;
        new_controller_input->action_right.state = button_b;
        new_controller_input->action_left.state = button_x;
        new_controller_input->action_down.state = button_a;
        new_controller_input->left_shoulder.state = left_shoulder;
        new_controller_input->right_shoulder.state = right_shoulder;
        new_controller_input->back_left_trigger.state = back_left_trigger;
        new_controller_input->back_right_trigger.state = back_right_trigger;
        new_controller_input->left_thumb.state = left_thumb;
        new_controller_input->right_thumb.state = right_thumb;
        new_controller_input->back.state = back;
        new_controller_input->start.state = start;

        // TODO(John): Support vibration, example code below.
        // right motor is the high-frequency motor.
        // left motor is the low-frequency motor.
        /*
        XINPUT_VIBRATION controller_vibration = {};
        controller_vibration.wLeftMotorSpeed = 32000; // use any value between 0-65535 here
        controller_vibration.wRightMotorSpeed = 16000; // use any value between 0-65535 here
        XInputSetState(controller_index, &controller_vibration );
        */
        // TODO(John): Support audio devices connected to a controller.


    }
    else // ERROR_DEVICE_NOT_CONNECTED
    {
        // Controller is not connected.
    }
}

internal void win32_handle_window_messages(HWND hwnd, game_input *old_input, game_input *new_input)
{
    MSG message = {0};
    while (PeekMessage(&message, hwnd, 0, 0, PM_REMOVE))
    {
        switch(message.message)
        {
            case WM_KEYDOWN: // If keyboard key was pressed.
            {
                uint32_t vk_code = (uint32_t)message.wParam;
                if(vk_code == 0x57) new_input->keyboard.up.state |= BUTTON_PRESSED; // W key
                if(vk_code == 0x41) new_input->keyboard.left.state |= BUTTON_PRESSED; // A key
                if(vk_code == 0x44) new_input->keyboard.right.state |= BUTTON_PRESSED; // D key
                if(vk_code == 0x53) new_input->keyboard.down.state |= BUTTON_PRESSED; // S key
                if(vk_code == 0x25) new_input->keyboard.left.state |= BUTTON_PRESSED; // LEFT ARROW key
                if(vk_code == 0x26) new_input->keyboard.up.state |= BUTTON_PRESSED; // UP ARROW key
                if(vk_code == 0x27) new_input->keyboard.right.state |= BUTTON_PRESSED; // RIGHT ARROW key
                if(vk_code == 0x28) new_input->keyboard.down.state |= BUTTON_PRESSED; // DOWN ARROW key
            } break;
            case WM_KEYUP: // If keyboard key was released.
            {
                uint32_t vk_code = (uint32_t)message.wParam;
                if(vk_code == 0x57) new_input->keyboard.up.state |= BUTTON_RELEASED; // W key
                if(vk_code == 0x41) new_input->keyboard.left.state |= BUTTON_RELEASED; // A key
                if(vk_code == 0x44) new_input->keyboard.right.state |= BUTTON_RELEASED; // D key
                if(vk_code == 0x53) new_input->keyboard.down.state |= BUTTON_RELEASED; // S key
                if(vk_code == 0x25) new_input->keyboard.left.state |= BUTTON_RELEASED; // LEFT ARROW key
                if(vk_code == 0x26) new_input->keyboard.up.state |= BUTTON_RELEASED; // UP ARROW key
                if(vk_code == 0x27) new_input->keyboard.right.state |= BUTTON_RELEASED; // RIGHT ARROW key
                if(vk_code == 0x28) new_input->keyboard.down.state |= BUTTON_RELEASED; // DOWN ARROW key
                if(vk_code == 0x20) toggle_fullscreen(hwnd); // SPACE KEY
                if(vk_code == 0x1B) window_is_active = 0; // ESCAPE KEY
            } break;
            default:
                TranslateMessage(&message);
                DispatchMessage(&message);
        }
    }
}

LRESULT CALLBACK win32_main_window_callback(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
    LRESULT result = 0;
    switch (message)
    {
        case WM_DESTROY:
        {
            window_is_active = 0;
        } break;
        default:
            result = DefWindowProc(window, message, wparam, lparam); 
    }
    return result; 
}

internal HWND win32_create_window(HINSTANCE hInstance, int nCmdShow)
{
	WNDCLASS window_class = {0};
	window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	window_class.lpfnWndProc = win32_main_window_callback;
	window_class.hInstance = hInstance;
	// window_class.hIcon
	window_class.lpszClassName = "SpectrumWindowClass";

	RegisterClassA(&window_class);

	// Create the window.
    HWND hwnd = CreateWindowExA(
        0,                              // Optional window styles.
        window_class.lpszClassName,                     // Window class
        "Spectrum",                     // Window text
        WS_OVERLAPPEDWINDOW, // Window style
        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        0,       // Parent window    
        0,       // Menu
        hInstance,  // Instance handle
        0        // Additional application data
	);

    window_is_active = 1;
    ShowWindow(hwnd, nCmdShow);

    return hwnd;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	printf("Hello Spectrum.\n");

	int32_t result = win32_load_xinput();
	win32_game_code game_code = win32_load_game_code("spectrum.dll");
	if(result != 0) return 1;
	game_memory memory = {0};
	memory.permanent_storage_size = Megabytes(256);
	memory.transient_storage_size = Megabytes(256);
	memory.permanent_storage = VirtualAlloc(0, memory.permanent_storage_size + memory.transient_storage_size,
			MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	memory.transient_storage = ((uint8_t*)memory.permanent_storage + memory.permanent_storage_size);
	memory.read_file = win32_read_file;
	memory.write_file = win32_write_file;
	memory.free_file_memory = win32_free_file_memory;
	memory.print = win32_print;

	game_state *state = (game_state*)memory.permanent_storage;
	state->t = 0.0;
	state->dt = 0.01;
	state->accumulator = 0.0;

    game_input inputs[2] = {0};
    game_input *old_input = &inputs[0];
    game_input *new_input = &inputs[1];

	game_camera camera = {0};

    HWND hwnd = win32_create_window(hInstance, nCmdShow);
    if(hwnd == 0) return 1;

	vk_renderer renderer = {0};
	vk_init(&renderer, win32_read_file, win32_free_file_memory, hwnd);

    DWORD max_controllers_count = XUSER_MAX_COUNT;
    if(max_controllers_count > MAX_PLAYERS) max_controllers_count = MAX_PLAYERS;

    uint64_t query_results[ODS_FRAME_DATA_SIZE * 2] = {0};

	uint64_t current_time = read_cpu_timer();
	uint64_t cpu_freq_estimation = estimate_cpu_freq();

	while(window_is_active)
	{
		uint64_t new_time = read_cpu_timer();
		state->frame_time = (double)(new_time - current_time) / cpu_freq_estimation;
		if(state->frame_time > 0.25) state->frame_time = 0.25;
		current_time = new_time;

		uint32_t frame_counter = renderer.current_frame;
		vk_acquire_image(&renderer, &query_results[0]);

        win32_handle_window_messages(hwnd, old_input, new_input);

		// Controller input.
		for(DWORD controller_index = 0; controller_index < max_controllers_count; ++controller_index)
		{
			win32_get_controller_input(old_input, new_input, controller_index);
		} 

		game_code.update_and_render(&memory, inputs, &camera, &renderer);

		vk_update_global_resources(renderer.device, renderer.uniform_buffer_memory,
			&camera, frame_counter * sizeof(game_camera));

		vk_command_buffer_recording(renderer.data[frame_counter].command_buffer, renderer.render_pass,
			renderer.data[frame_counter].framebuffer, &renderer.first_material,
			renderer.swapchain_image_extent,
			renderer.vk_buffer, renderer.vertex_buffer_size, renderer.index_buffer_size,
			renderer.global_descriptor_sets[frame_counter],
			&renderer.final_material, renderer.final_descriptor_sets[frame_counter],
			renderer.rect_buffer, renderer.rect_vertex_buffer_size, renderer.rect_index_buffer_size,
			renderer.query_pool,
			renderer.current_frame);
		vk_present_image(&renderer);
		*new_input = *old_input;
	}

	vk_deinit(&renderer);

	return 0;
}
