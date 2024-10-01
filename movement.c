#include "doom-nukem.h"

/**
 * Calculates the time between frames and updates player speed
 *
 * @param t_box* box
 */
static void	calc_time(t_box *box)
{
	double	frame_time;

	box->old_time = box->time;
	gettimeofday(&box->time, NULL);

	frame_time = (box->time.tv_sec - box->old_time.tv_sec) +
				(box->time.tv_usec - box->old_time.tv_usec) / 1000000.0;

	box->map->player.move_speed = frame_time * 3.0;
	box->map->player.move_speed *= box->map->player.speed / 5;

	box->map->player.rot_speed = frame_time * 1.5;

	if (box->map->player.sprint)
	{
		box->map->player.move_speed *= 2;
		box->map->player.rot_speed *= 2;
	}
}
/**
 * Updates player position based on key presses
 *
 * @param t_box* box
 */
void	calc_move(t_box *box)
{
	calc_time(box);
	if (box->map->player.rotate == 1) // left arrow key
	{
		box->map->player.angle.angle_val += (0.1875f * box->map->player.move_speed);
		normalize_360(&box->map->player.angle.angle_val);
	}
	if (box->map->player.rotate == -1) // right arrow key
	{
		box->map->player.angle.angle_val -= (0.1875f * box->map->player.move_speed);
		normalize_360(&box->map->player.angle.angle_val);
	}
	if (box->map->player.move_x == 1) // w key
	{
		box->map->player.x += get_cos_value(box->map->player.angle) * box->map->player.move_speed;
		box->map->player.y += get_sin_value(box->map->player.angle) * box->map->player.move_speed;
	}
	if (box->map->player.move_x == -1) // s key
	{
		box->map->player.x -= get_cos_value(box->map->player.angle) * box->map->player.move_speed;
		box->map->player.y -= get_sin_value(box->map->player.angle) * box->map->player.move_speed;
	}
	if (box->map->player.move_y == 1) // a key
	{
		box->map->player.x -= get_sin_value(box->map->player.angle) * box->map->player.move_speed;
		box->map->player.y += get_cos_value(box->map->player.angle) * box->map->player.move_speed;
	}
	if (box->map->player.move_y == -1) // d key
	{
		box->map->player.x += get_sin_value(box->map->player.angle) * box->map->player.move_speed;
		box->map->player.y -= get_cos_value(box->map->player.angle) * box->map->player.move_speed;
	}
	if (box->map->player.move_z == 1) // space key
		box->map->player.z += box->map->player.move_speed;
	if (box->map->player.move_z == -1) // ctrl key
		box->map->player.z -= box->map->player.move_speed;
}

