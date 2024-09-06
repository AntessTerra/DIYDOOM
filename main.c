/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phelebra <xhelp00@gmail.com>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/10 16:50:14 by jbartosi          #+#    #+#             */
/*   Updated: 2023/10/16 16:05:02 by phelebra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

static void	render_fov(t_box *box)
{
	uint32_t x, y;

	y = -1;
	while (++y < SCREENHEIGHT)
	{
		x = -1;
		while (++x < SCREENWIDTH)
			my_mlx_pyxel_put(&box->image, x, y, 0xFF3e403f);
	}
}

static void	draw_automap(t_box *box)
{
	uint32_t	i, y, x;
	t_vertex	start, end;

	y = -1;
	while (++y < SCREENHEIGHT / 4)
	{
		x = -1;
		while (++x < SCREENWIDTH / 4)
			my_mlx_pyxel_put(&box->minimap, x, y, 0x4D000000);
	}

	i = -1;
	while (++i < box->map->n_linedefs)
	{
		start = box->map->vertexes[box->map->linedef[i].start_vertex];
		end = box->map->vertexes[box->map->linedef[i].end_vertex];

		draw_line(&box->minimap,
			remap_x_to_screen(box, start.x),
			remap_y_to_screen(box, start.y),
			remap_x_to_screen(box, end.x),
			remap_y_to_screen(box, end.y),
			0xFFFFFF);
	}
}

static void	update_screen(t_box *box)
{
	free_solid_segs(box);
	add_solid_seg_after(box, new_solid_seg(SCREENWIDTH, INT_MAX, 0), NULL);
	add_solid_seg_after(box, new_solid_seg(INT_MIN, -1, 0), NULL);
	render_fov(box);
	draw_automap(box);
	render_bsp_nodes(box, box->WAD.maps[0].n_nodes - 1);
	my_mlx_put_image_to_window(box, &box->image, 0, 0, -1);

	my_mlx_put_image_to_window(box, &box->minimap, SCREENWIDTH * 0.75, 0, -1);

	//Draw player icon on location
	my_mlx_put_image_to_window(box, &box->textures[UI_PICKUPS],
		(remap_x_to_screen(box, box->map->player.x) - 8) + SCREENWIDTH * 0.75,
		remap_y_to_screen(box, box->map->player.y) - 8,
		27);
}

/** timer()
 * 	-------
 *
 *  Timer function that is called every frame
 *
 * 	param: t_box *box
 * 	return: 0
 */
static int	timer(t_box *box)
{
	update_screen(box);
	return (0);
}

/** main()
 * 	------
 *
 *  Main function that initializes the mlx window and hooks
 *  Calls the parser to parse the WAD file
 *  Initializes the values and textures
 *  Creates the images for the window and minimap
 *  Hooks the events to the mlx window
 *  Calls the timer function to render the game
 *  Loops the mlx
 *
 * 	return: 0
 */
int	main(void)
{
	t_box	box;

	parser(&box);

	box.mlx = mlx_init();
	box.win = mlx_new_window(box.mlx, SCREENWIDTH, SCREENHEIGHT, "doom-nukem");
	init_values(&box);
	init_textures(&box);
	new_image(box.mlx, &box.image, SCREENWIDTH, SCREENHEIGHT);
	new_image(box.mlx, &box.minimap, SCREENWIDTH / 4, SCREENHEIGHT / 4);
	mlx_hook(box.win, 17, 0, exit_hook, &box);
	mlx_hook(box.win, 2, 1L << 0, key_press, &box);
	mlx_hook(box.win, 3, 1L << 1, key_release, &box);
	mlx_hook(box.win, 4, 1L << 2, mouse_press, &box);
	mlx_hook(box.win, 5, 1L << 3, mouse_release, &box);
	mlx_hook(box.win, 6, 1L << 6, mouse_move, &box);

	// render_fov(&box);
	// draw_automap(&box);
	// render_bsp_nodes(&box, box.WAD.maps[0].n_nodes - 1);

	// update_screen(&box);
	// print_solid_segs(&box, 0);

	mlx_loop_hook(box.mlx, timer, &box);
	mlx_loop(box.mlx);
	return (0);
}
