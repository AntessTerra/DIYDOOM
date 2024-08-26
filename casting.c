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

void	draw_automap(t_box *box)
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

	//Draw player
	x = (box->WAD.maps[0].player.x + x_shift) / box->WAD.maps[0].automap_scale_factor;
	y = ((scaled_max_y - (box->WAD.maps[0].player.y + y_shift)) / box->WAD.maps[0].automap_scale_factor);
	my_mlx_put_image_to_window(box, &box->textures[UI_PICKUPS], x - 8, y - 8, 27);
}

bool	is_point_on_left_side(t_box *box, int x, int y, int node_id)
{
	int dx = x - box->WAD.maps[0].nodes[node_id].partition_x;
	int dy = y - box->WAD.maps[0].nodes[node_id].partition_y;

	return (((dx * box->WAD.maps[0].nodes[node_id].change_partition_y) - (dy * box->WAD.maps[0].nodes[node_id].change_partition_x)) <= 0);
}

void	highlight_nodes(t_box *box, int node_id)
{
	int	x_shift = -box->WAD.maps[0].min_x;
	int	y_shift = -box->WAD.maps[0].min_y;
	t_node	node = box->WAD.maps[0].nodes[node_id];
	int	scaled_max_y = 3500;
	t_rect	rect[2];
	rect[0].corners[0].x = (node.right_box_left + x_shift) / box->WAD.maps[0].automap_scale_factor;
	rect[0].corners[0].y = ((scaled_max_y - (node.right_box_top + y_shift)) / box->WAD.maps[0].automap_scale_factor);
	rect[0].corners[1].x = (node.right_box_right + x_shift) / box->WAD.maps[0].automap_scale_factor;
	rect[0].corners[1].y = ((scaled_max_y - (node.right_box_bottom + y_shift)) / box->WAD.maps[0].automap_scale_factor);

	rect[1].corners[0].x = (node.left_box_left + x_shift) / box->WAD.maps[0].automap_scale_factor;
	rect[1].corners[0].y = ((scaled_max_y - (node.left_box_top + y_shift)) / box->WAD.maps[0].automap_scale_factor);
	rect[1].corners[1].x = (node.left_box_right + x_shift) / box->WAD.maps[0].automap_scale_factor;
	rect[1].corners[1].y = ((scaled_max_y - (node.left_box_bottom + y_shift)) / box->WAD.maps[0].automap_scale_factor);
	
	draw_rect(box, rect[0], 0x00FF00);
	draw_rect(box, rect[1], 0xFF0000);

	draw_line(&box->image,
		(node.partition_x + x_shift) / box->WAD.maps[0].automap_scale_factor,
		((scaled_max_y - (node.partition_y + y_shift)) / box->WAD.maps[0].automap_scale_factor),
		(node.partition_x + node.change_partition_x + x_shift) / box->WAD.maps[0].automap_scale_factor,
		((scaled_max_y - (node.partition_y + node.change_partition_y + y_shift)) / box->WAD.maps[0].automap_scale_factor),
		0x0000FF);
}

void	render_bsp_nodes(t_box *box, int node_id)
{
	if (node_id & LEAF_NODE)
	{
		printf("LEAF NODE %i\n", node_id);
		return ;
	}

	highlight_nodes(box, node_id);
	usleep(1000000);
	draw_automap(box);
	bool is_point_on_left = is_point_on_left_side(box, box->WAD.maps[0].player.x, box->WAD.maps[0].player.y, node_id);

	if (is_point_on_left)
	{
		render_bsp_nodes(box, box->WAD.maps[0].nodes[node_id].left_child_id);
		render_bsp_nodes(box, box->WAD.maps[0].nodes[node_id].right_child_id);
	}
	else
	{
		render_bsp_nodes(box, box->WAD.maps[0].nodes[node_id].right_child_id);
		render_bsp_nodes(box, box->WAD.maps[0].nodes[node_id].left_child_id);
	}
}


