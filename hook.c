/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hook.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phelebra <xhelp00@gmail.com>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/08 18:52:55 by jbartosi          #+#    #+#             */
/*   Updated: 2023/10/16 16:04:56 by phelebra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

/**
 * free_stuff()
 * ------------
 *
 * Frees all the allocated memory
 *
 * param: t_box *box
 *
 * return: 0
 */
int	free_stuff(t_box *box)
{
	int m;

	m = -1;
	while (++m < 100)
		if (box->textures[m].img)
			mlx_destroy_image(box->mlx, box->textures[m].img);

	mlx_destroy_image(box->mlx, box->image.img);
	mlx_destroy_image(box->mlx, box->minimap.img);
	mlx_destroy_window(box->mlx, box->win);
	mlx_destroy_display(box->mlx);
	free(box->mlx);

	free(box->textures);
	free(box->WAD.dirs);
	m = -1;
	while (++m < 9)
	{
		free(box->WAD.maps[m].WAD_linedefs);
		free(box->WAD.maps[m].linedefs);
		free(box->WAD.maps[m].vertexes);
		free(box->WAD.maps[m].WAD_things);
		free(box->WAD.maps[m].WAD_nodes);
		free(box->WAD.maps[m].WAD_ssectors);
		free(box->WAD.maps[m].WAD_segs);
		free(box->WAD.maps[m].segs);
		free(box->WAD.maps[m].WAD_sectors);
		free(box->WAD.maps[m].WAD_sidedefs);
		free(box->WAD.maps[m].sidedefs);
	}
	free_solid_segs(box);
	free(box->map->screen_x_to_angle);
	return (0);
}

/**
 * mouse_move()
 * ------------
 *
 * Processes mouse movement
 *
 * param: int x
 * param: int y
 * param: t_box *box
 *
 * return: 0
 */
int	mouse_move(int x, int y, t_box *box)
{
	(void) box;
	(void) x;
	(void) y;
	// printf("X %i Y %i\n", x, y);
	return (0);
}

/**
 * mouse_press()
 * -------------
 *
 * Processes pressed mouse button
 *
 * param: int keycode
 * param: int x
 * param: int y
 * param: t_box *box
 *
 * return: 0
 */
int	mouse_press(int keycode, int x, int y, t_box *box)
{
	(void)x;
	(void)y;
	(void) keycode;
	(void)box;
	// printf("X %i Y %i | Code %i\n", x, y, keycode);
	return (0);
}

/**
 * mouse_release()
 * ---------------
 *
 * Processes released mouse button
 *
 * param: int keycode
 * param: int x
 * param: int y
 * param: t_box *box
 *
 * return: 0
 *
 */
int	mouse_release(int keycode, int x, int y, t_box *box)
{
	(void)x;
	(void)y;
	(void) keycode;
	(void)box;
	return (0);
}

/**
 * key_press()
 * -----------
 *
 * Processes pressed key
 *
 * param: int key
 * param: t_box *box
 *
 * return: 0
 *
 */
int	key_press(int key, t_box *box)
{
	(void) box;
	(void) key;
	// printf("Key pressed: %i\n", key);
	return (0);
}

/**
 * key_release()
 * -------------
 *
 * Processes released key
 *
 * param: int key
 * param: t_box *box
 *
 * return: 0
 *
 */
int	key_release(int key, t_box *box)
{
	// (void) box;
	if (key == 65307) // esc key
		exit_hook(box);
	else if (key == 65361) // left arrow key
	{
		box->WAD.maps[0].player.angle.angle_val += (0.1875f * 10);
		normalize_360(&box->WAD.maps[0].player.angle.angle_val);
	}
	else if (key == 65363) // right arrow key
	{
		box->WAD.maps[0].player.angle.angle_val -= (0.1875f * 10);
		normalize_360(&box->WAD.maps[0].player.angle.angle_val);
	}
	else if (key == 'w')
	{
		box->map->player.x += get_cos_value(box->map->player.angle) * box->map->player.move_speed;
		box->map->player.y += get_sin_value(box->map->player.angle) * box->map->player.move_speed;
	}
	else if (key == 's')
	{
		box->map->player.x -= get_cos_value(box->map->player.angle) * box->map->player.move_speed;
		box->map->player.y -= get_sin_value(box->map->player.angle) * box->map->player.move_speed;
	}
	else if (key == 'a')
	{
		box->map->player.x -= get_sin_value(box->map->player.angle) * box->map->player.move_speed;
		box->map->player.y += get_cos_value(box->map->player.angle) * box->map->player.move_speed;
	}
	else if (key == 'd')
	{
		box->map->player.x += get_sin_value(box->map->player.angle) * box->map->player.move_speed;
		box->map->player.y -= get_cos_value(box->map->player.angle) * box->map->player.move_speed;
	}
	// else if (key == 65453 && box->WAD.m > 0) // - numpad key
	// 		box->WAD.m--;
	// else if (key == 65363 && box->WAD.m < N_MAPS) // + numpad key
	// 		box->WAD.m++;
	// printf("Key released: %i\n", key);
	return (0);
}

/**
 * exit_hook()
 * -----------
 *
 * Exits the program
 *
 * param: t_box *box
 * return: 0
 *
 */
int	exit_hook(t_box *box)
{
	free_stuff(box);
	exit(0);
}
