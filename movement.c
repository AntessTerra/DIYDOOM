#include "doom-nukem.h"

void	calc_move(t_box *box)
{
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
