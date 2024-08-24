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

#include "cub3d.h"

void	my_mlx_pyxel_put(t_image *image, int x, int y, int color)
{
	unsigned char	*pixel;

	pixel = image->addr + (y * image->line_len + x
			* (image->bits_pp / 8));
	*(unsigned int *)pixel = color;
}

void	my_mlx_put_image_to_window(t_box *box, t_image *image, int x, int y, int sprite_id)
{
	if (sprite_id >= 0)
		mlx_put_image_to_window(box->mlx, box->win, image->img, x, y, 0 + (image->one_x * (sprite_id % image->n_col)), 0 + (image->one_y * (sprite_id / image->n_col)), image->one_x, image->one_y);
	else
		mlx_put_image_to_window(box->mlx, box->win, image->img, x, y, 0, 0, image->width, image->height);
}

void	draw_line(t_image *image, int beginX, int beginY, int endX, int endY, int color)
{
	double deltaX = endX - beginX;
	double deltaY = endY - beginY;
	int pixels = sqrt((deltaX * deltaX) + (deltaY * deltaY));
	deltaX /= pixels;
	deltaY /= pixels;
	double pixelX = beginX;
	double pixelY = beginY;
	while (pixels)
	{
		// printf("%i | %i\n", (int)pixelX, (int)pixelY);
		my_mlx_pyxel_put(image, pixelX, pixelY, 0xFF << 24 | color);
		pixelX += deltaX;
		pixelY += deltaY;
		--pixels;
	}
}

int	timer(t_box *box)
{
	(void)box;

	return (0);
}

void	put_automap(t_box *box)
{
	uint32_t	i, y, x, start_x, start_y, end_x, end_y;
	int	x_shift = -box->WAD.maps[0].min_x;
	int	y_shift = -box->WAD.maps[0].min_y;
	int	scaled_max_y = 3500;
	t_vertex	start, end;

	y = -1;
	while (++y < SCREENHEIGHT)
	{
		x = -1;
		while (++x < SCREENWIDTH)
			my_mlx_pyxel_put(&box->image, x, y, 0xFF000000);
	}

	i = -1;
	while (++i < box->WAD.maps[0].n_linedefs)
	{
		start = box->WAD.maps[0].vertexes[box->WAD.maps[0].linedef[i].start_vertex];
		end = box->WAD.maps[0].vertexes[box->WAD.maps[0].linedef[i].end_vertex];
		start_x = (start.x + x_shift) / box->WAD.maps[0].automap_scale_factor;
		start_y = ((scaled_max_y - (start.y + y_shift)) / box->WAD.maps[0].automap_scale_factor);
		end_x = (end.x + x_shift) / box->WAD.maps[0].automap_scale_factor;
		end_y = ((scaled_max_y - (end.y + y_shift)) / box->WAD.maps[0].automap_scale_factor);
		draw_line(&box->image, start_x, start_y, end_x, end_y, 0xFFFFFF);
		// printf("%i | %i \n", start_x, start_y);
		// printf("%i | %i \n", end_x, end_y);
	}
	// draw_line(&box->image, 100, 100, 150, 80, 0xFFFFFF);
	my_mlx_put_image_to_window(box, &box->image, 0, 0, -1);
}

/*
	new_image

	Creates a new mlx_image_alpha and sets its variables
*/
t_image	*new_image(void *mlx, t_image *image, int width, int height)
{
	image->img = mlx_new_image_alpha(mlx, width, height);
	image->width = width;
	image->height = height;
	image->addr = (unsigned char *)mlx_get_data_addr(image->img,
			&image->bits_pp, &image->line_len, &image->endian);
	return (image);
}

int	main(void)
{
	t_box	box;

	parser(&box);
	box.WAD.maps[0].automap_scale_factor = 5;
	box.mlx = mlx_init();
	box.win = mlx_new_window(box.mlx, SCREENWIDTH, SCREENHEIGHT, "doom-nukem");
	new_image(box.mlx, &box.image, SCREENWIDTH, SCREENHEIGHT);
	put_automap(&box);
	mlx_hook(box.win, 17, 0, exit_hook, &box);
	mlx_hook(box.win, 2, 1L << 0, key_press, &box);
	mlx_hook(box.win, 3, 1L << 1, key_release, &box);
	mlx_hook(box.win, 4, 1L << 2, mouse_press, &box);
	mlx_hook(box.win, 5, 1L << 3, mouse_release, &box);
	mlx_hook(box.win, 6, 1L << 6, mouse_move, &box);
	// mlx_loop_hook(box.mlx, timer, &box);
	mlx_loop(box.mlx);
	return (0);
}
