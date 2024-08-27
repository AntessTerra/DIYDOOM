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

#include "cub3d.h"

int	free_stuff(t_box *box)
{
	int m;

	m = -1;
	while (++m < 100)
		if (box->textures[m].img)
			mlx_destroy_image(box->mlx, box->textures[m].img);

	mlx_destroy_image(box->mlx, box->image.img);
	mlx_destroy_window(box->mlx, box->win);
	mlx_destroy_display(box->mlx);
	free(box->mlx);

	free(box->textures);
	free(box->WAD.dirs);
	m = -1;
	while (++m < 1)
	{
		free(box->WAD.maps[m].linedef);
		free(box->WAD.maps[m].vertexes);
		free(box->WAD.maps[m].things);
		free(box->WAD.maps[m].nodes);
		free(box->WAD.maps[m].ssectors);
		free(box->WAD.maps[m].segs);
	}
	return (0);
}

int	mouse_move(int x, int y, t_box *box)
{
	(void) box;
	(void) x;
	(void) y;
	// printf("X %i Y %i\n", x, y);
	return (0);
}

int	mouse_press(int keycode, int x, int y, t_box *box)
{
	(void)x;
	(void)y;
	(void) keycode;
	(void)box;
	// printf("X %i Y %i | Code %i\n", x, y, keycode);
	return (0);
}

int	mouse_release(int keycode, int x, int y, t_box *box)
{
	(void)x;
	(void)y;
	(void) keycode;
	(void)box;
	return (0);
}

/*	Key_press

	Processes pressed down key
*/
int	key_press(int key, t_box *box)
{
	(void) box;
	(void) key;
	// printf("Key pressed: %i\n", key);
	return (0);
}

/*	Key_release

	Processes let up key
*/
int	key_release(int key, t_box *box)
{
	// (void) box;
	if (key == 65307)
		exit_hook(box);
	// else if (key == 65362)
	// {
	// 	if (box->WAD.maps[0].bsp_layer > 0)
	// 		box->WAD.maps[0].bsp_layer--;
	// }
	// else if (key == 65364)
	// {
	// 	if (box->WAD.maps[0].bsp_layer < box->WAD.maps[0].n_nodes - 1)
	// 		box->WAD.maps[0].bsp_layer++;
	// }
	// printf("Key released: %i\n", key);
	return (0);
}

/*	Exit_hook

	Closes the window when the x in the corner is pressed
*/
int	exit_hook(t_box *box)
{
	free_stuff(box);
	exit(0);
}
