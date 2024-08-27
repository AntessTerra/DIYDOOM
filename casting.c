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

int	remap_x_to_screen(t_box *box, int x)
{
	return (((x + (box->WAD.maps[0].min_x * -1)) / box->WAD.maps[0].automap_scale_factor));
}

int	remap_y_to_screen(t_box *box, int y)
{
	return (((3500 - (y + (box->WAD.maps[0].min_y * -1))) / box->WAD.maps[0].automap_scale_factor));
}

void	add_wall_in_fov(t_box *box, t_angle v1, t_angle v2, int color)
{
	int v1_x = angle_to_screen(box, v1);
	int v2_x = angle_to_screen(box, v2);

	if (v1_x < 0 || v2_x < 0)
		return ;
	draw_line(&box->image, v1_x, 0, v1_x, SCREENHEIGHT, color);
	draw_line(&box->image, v2_x, 0, v2_x, SCREENHEIGHT, color);
}

void	render_fov(t_box *box)
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

void	update_screen(t_box *box)
{

	my_mlx_put_image_to_window(box, &box->image, 0, 0, -1);

	my_mlx_put_image_to_window(box, &box->minimap, SCREENWIDTH * 0.75, 0, -1);
	//Draw player icon on location
	my_mlx_put_image_to_window(box, &box->textures[UI_PICKUPS],
		(remap_x_to_screen(box, box->WAD.maps[0].player.x) - 8) + SCREENWIDTH * 0.75,
		remap_y_to_screen(box, box->WAD.maps[0].player.y) - 8,
		27);
}

void	render_subsector(t_box *box, int subsector_id)
{
	t_ssector	subsector = box->WAD.maps[0].ssectors[subsector_id];
	t_seg		seg;
	t_vertex	start, end;

	uint32_t	i = -1;
	// printf("Subsector %i\n", subsector_id);
	while (++i < subsector.seg_count)
	{
		seg = box->WAD.maps[0].segs[subsector.first_seg_id + i];
		start = box->WAD.maps[0].vertexes[seg.start_vertex_id];
		end = box->WAD.maps[0].vertexes[seg.end_vertex_id];

		if (clip_vertexes_in_FOV(box, start, end))
		{
			int color = rand() % 255 << 16 | rand() % 255 << 8 | rand() % 255;
			draw_line(&box->minimap,
				remap_x_to_screen(box, start.x),
				remap_y_to_screen(box, start.y),
				remap_x_to_screen(box, end.x),
				remap_y_to_screen(box, end.y),
				color);
			add_wall_in_fov(box, angle_to_vortex(box, start), angle_to_vortex(box, end), color);
		}
	}
	// usleep(100000);
	return ;
}

void	draw_automap(t_box *box)
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
	while (++i < box->WAD.maps[0].n_linedefs)
	{
		start = box->WAD.maps[0].vertexes[box->WAD.maps[0].linedef[i].start_vertex];
		end = box->WAD.maps[0].vertexes[box->WAD.maps[0].linedef[i].end_vertex];

		draw_line(&box->minimap,
			remap_x_to_screen(box, start.x),
			remap_y_to_screen(box, start.y),
			remap_x_to_screen(box, end.x),
			remap_y_to_screen(box, end.y),
			0xFFFFFF);
	}
	// draw_line(&box->image, 100, 100, 150, 80, 0xFFFFFF);
	// update_screen(box);
}

bool	is_point_on_left_side(t_box *box, int x, int y, int node_id)
{
	int dx = x - box->WAD.maps[0].nodes[node_id].partition_x;
	int dy = y - box->WAD.maps[0].nodes[node_id].partition_y;

	return (((dx * box->WAD.maps[0].nodes[node_id].change_partition_y) - (dy * box->WAD.maps[0].nodes[node_id].change_partition_x)) <= 0);
}

void	highlight_nodes(t_box *box, int node_id)
{
	t_node	node = box->WAD.maps[0].nodes[node_id];

	draw_rect(box,
		remap_x_to_screen(box, node.right_box_left),
		remap_y_to_screen(box, node.right_box_top),
		remap_x_to_screen(box, node.right_box_right),
		remap_y_to_screen(box, node.right_box_bottom),
		0x00FF00);
	draw_rect(box,
		remap_x_to_screen(box, node.left_box_left),
		remap_y_to_screen(box, node.left_box_top),
		remap_x_to_screen(box, node.left_box_right),
		remap_y_to_screen(box, node.left_box_bottom),
		0xFF0000);

	draw_line(&box->minimap,
		remap_x_to_screen(box, node.partition_x),
		remap_y_to_screen(box, node.partition_y),
		remap_x_to_screen(box, node.partition_x + node.change_partition_x),
		remap_y_to_screen(box, node.partition_y + node.change_partition_y),
		0x0000FF);
}

void	render_bsp_nodes(t_box *box, int node_id)
{
	if (node_id & LEAF_NODE)
	{
		render_subsector(box, node_id & (~LEAF_NODE));
		return ;
	}
	// update_screen(box);
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


