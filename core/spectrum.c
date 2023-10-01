#include "spectrum.h"

internal void update_camera(game_camera *camera)
{
	camera->model_matrix = mat4_identity();
	camera->view_matrix = mat4_identity();
	camera->projection_matrix = mat4_identity();
}

// update_and_render(game_memory *memory, game_input *inputs, game_camera *camera, vk_renderer *renderer)
GAME_UPDATE_AND_RENDER(update_and_render)
{
	game_state *state = (game_state*)memory->permanent_storage;
	// game_input *old_input = &inputs[0];
	// game_input *new_input = &inputs[1];

	if(memory->is_initialized == 0)
	{
		memory->is_initialized = 1;
	}

	state->accumulator += state->frame_time;
	while(state->accumulator >= state->dt)
	{
		// Update.
		// previous_state = current_state;
		// integrate(&current_state, t, dt);
		state->t += state->dt;
		state->accumulator -= state->dt;
		update_camera(camera);
	}

	return;
}
