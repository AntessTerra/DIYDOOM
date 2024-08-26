/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   values.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phelebra <xhelp00@gmail.com>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/08 18:52:55 by jbartosi          #+#    #+#             */
/*   Updated: 2023/10/16 16:04:56 by phelebra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	init_textures(t_box *box)
{
	int		i;

	box->textures = malloc(100 * sizeof(t_image));
	i = -1;
	while (++i < 100)
		box->textures[i].img = NULL;
	png_file_to_image(box->mlx, &box->textures[UI_PICKUPS], "textures/ui_pickups.png");
	split_spritesheet(&box->textures[UI_PICKUPS], 8, 5, 16, 16);

	// i = -1;
	// while (++i < 100)
	// if (box->textures[i].img == NULL)
	// {
	// 	png_file_to_image(box->mlx, &box->textures[i], "textures/missing.png");
	// 	img_resize(box->mlx, &box->textures[i], 0.5);
	// }
	// box->textures[i].addr = (unsigned char *)mlx_get_data_addr(box->textures[i].img,
		// &box->textures[i].bits_pp, &box->textures[i].line_len, &box->textures[i].endian);
}

