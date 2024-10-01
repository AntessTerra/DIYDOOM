#include "doom-nukem.h"

/**
 * Free all the allocated memory
 *
 * @param t_box* box
 */
int	free_stuff(t_box *box)
{
	int m;

	if (box->textures)
	{
		m = -1;
		while (++m < 100)
			if (box->textures[m].img)
				mlx_destroy_image(box->mlx, box->textures[m].img);
		free(box->textures);
	}

	if (box->image.img)
		mlx_destroy_image(box->mlx, box->image.img);
	if (box->minimap.img)
		mlx_destroy_image(box->mlx, box->minimap.img);
	if (box->win)
		mlx_destroy_window(box->mlx, box->win);
	if (box->mlx)
	{
		mlx_destroy_display(box->mlx);
		free(box->mlx);
	}

	if (box->WAD.dirs)
		free(box->WAD.dirs);
	m = -1;
	while (++m < 9)
	{
		if (box->WAD.maps[m].WAD_linedefs)
			free(box->WAD.maps[m].WAD_linedefs);
		if (box->WAD.maps[m].linedefs)
			free(box->WAD.maps[m].linedefs);
		if (box->WAD.maps[m].vertexes)
			free(box->WAD.maps[m].vertexes);
		if (box->WAD.maps[m].WAD_things)
			free(box->WAD.maps[m].WAD_things);
		if (box->WAD.maps[m].WAD_nodes)
			free(box->WAD.maps[m].WAD_nodes);
		if (box->WAD.maps[m].WAD_ssectors)
			free(box->WAD.maps[m].WAD_ssectors);
		if (box->WAD.maps[m].WAD_segs)
			free(box->WAD.maps[m].WAD_segs);
		if (box->WAD.maps[m].segs)
			free(box->WAD.maps[m].segs);
		if (box->WAD.maps[m].WAD_sectors)
			free(box->WAD.maps[m].WAD_sectors);
		if (box->WAD.maps[m].WAD_sidedefs)
			free(box->WAD.maps[m].WAD_sidedefs);
		if (box->WAD.maps[m].sidedefs)
			free(box->WAD.maps[m].sidedefs);
	}

	if (!box->map)
		return (0);

	free_solid_segs(box);
	if (box->map->screen_x_to_angle)
		free(box->map->screen_x_to_angle);
	if (box->map->texture_to_color)
		free(box->map->ceiling_clip_height);
	if (box->map->ceiling_clip_height)
		free(box->map->floor_clip_height);
	return (0);
}

/**
 * Processes mouse movement
 *
 * @param int x
 * @param int y
 * @param t_box* box
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
 * Processes pressed mouse button
 *
 * @param int keycode
 * @param int x
 * @param int y
 * @param t_box* box
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
 * Processes released mouse button
 *
 * @param int keycode
 * @param int x
 * @param int y
 * @param t_box* box
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
 * Processes pressed key
 *
 * @param int key
 * @param t_box* box
 */
int	key_press(int key, t_box *box)
{
	if (key == 65361 || key == 'q') // left arrow key
		box->map->player.rotate = 1;
	else if (key == 65363 || key =='e') // right arrow key
		box->map->player.rotate = -1;
	else if (key == 'w')
		box->map->player.move_x = 1;
	else if (key == 's')
		box->map->player.move_x = -1;
	else if (key == 'a')
		box->map->player.move_y = 1;
	else if (key == 'd')
		box->map->player.move_y = -1;
	else if (key == ' ')
		box->map->player.move_z = 1;
	else if (key == 65507) // ctrl key
		box->map->player.move_z = -1;
	else if (key == 65505) // shift key
		box->map->player.sprint = true;
	// printf("Key pressed: %i\n", key);
	return (0);
}

/**
 * Processes released key
 *
 * @param int key
 * @param t_box* box
 */
int	key_release(int key, t_box *box)
{
	if (key == 65307) // esc key
		exit_hook(box, NULL);
	else if (key == 65361 || key == 'q') // left arrow key
		box->map->player.rotate = 0;
	else if (key == 65363 || key == 'e') // right arrow key
		box->map->player.rotate = 0;
	else if (key == 'w')
		box->map->player.move_x = 0;
	else if (key == 's')
		box->map->player.move_x = 0;
	else if (key == 'a')
		box->map->player.move_y = 0;
	else if (key == 'd')
		box->map->player.move_y = 0;
	else if (key == ' ')
		box->map->player.move_z = 0;
	else if (key == 65507) // ctrl key
		box->map->player.move_z = 0;
	else if (key == 65505) // shift key
		box->map->player.sprint = false;
	// else if (key == 65453 && box->WAD.m > 0) // - numpad key
	// 		box->WAD.m--;
	// else if (key == 65363 && box->WAD.m < N_MAPS) // + numpad key
	// 		box->WAD.m++;
	// printf("Key released: %i\n", key);
	return (0);
}

/**
 * Exits the game
 *
 * @param t_box* box
 *
 * @return Called when pressing the red cross on the window or pressing esc
 */
int	exit_hook(t_box *box, char *msg)
{
	free_stuff(box);
	if (msg)
	{
		write(1, msg, ft_strlen(msg));
		exit(1);
	}
	exit(0);
}
