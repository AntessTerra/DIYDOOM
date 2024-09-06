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

int	remap_x_to_screen(t_box *box, int x)
{
	return (((x + (box->map->min_x * -1)) / box->map->automap_scale_factor));
}

int	remap_y_to_screen(t_box *box, int y)
{
	return (((3500 - (y + (box->map->min_y * -1))) / box->map->automap_scale_factor));
}

// static void	draw_clipped_wall(t_box *box, t_seg seg)
// {
// 	float	ceiling = seg.
// 	float	floor
// }

/**
 * clip_wall()
 * -----------
 *
 * Clips and draws a wall segment
 *
 * param: t_box *box
 * param: t_solid_seg currWall
 * param: int m
 *
 * return: 0
 */
static void	clip_wall(t_box *box, t_solid_seg currWall, t_seg seg)
{
	(void)seg;
	t_solid_seg	*foundWall, *nextWall;

	foundWall = box->map->solid_segs;

	while (foundWall && foundWall->x_end < currWall.x_start - 1)
		foundWall = foundWall->next;
	// printf("%i, %i\n", currWall.x_start, currWall.x_end);
	if (currWall.x_start < foundWall->x_start)
	{
		// printf("TEST\n");
		if (currWall.x_end < foundWall->x_start - 1)
		{
			draw_rect(box, (t_rect){currWall.x_start, 0, currWall.x_end - currWall.x_start, 200, 0xFF00FF00}, false);
			t_solid_seg *p_curr_wall = new_solid_seg(currWall.x_start, currWall.x_end, currWall.color);
			add_solid_seg_after(box, p_curr_wall, p_curr_wall);
			return ;
		}
		draw_rect(box, (t_rect){currWall.x_start, 0, currWall.x_end - currWall.x_start, 300, 0xFF0000FF}, false);
		foundWall->x_start = currWall.x_start;
		return ;
	}
	// printf("%i <= %i\n", currWall.x_end, foundWall->x_end);
	if (currWall.x_end <= foundWall->x_end)
		return ;

	nextWall = foundWall;
	// printf("%i >= %i\n", currWall.x_end, nextWall->next->x_start - 1);
	while (currWall.x_end >= nextWall->next->x_start - 1)
	{
		draw_rect(box, (t_rect){currWall.x_start, 0, currWall.x_end - currWall.x_start, 400, 0xFFFF0000}, false);
		nextWall = nextWall->next;
		if (currWall.x_end <= nextWall->x_end)
		{
			foundWall->x_end = nextWall->x_end;
			if (nextWall != foundWall)
			{
				foundWall = foundWall->next;
				nextWall = nextWall->next;
				// printf("Deleting %i, %i\n", foundWall->x_start, foundWall->x_end);
				delete_seg(box, foundWall);
			}
			return ;
		}
	}
	// printf("%i, %i | %i, %i\n", nextWall->x_start, nextWall->x_end, foundWall->x_start, foundWall->x_end);
	draw_rect(box, (t_rect){currWall.x_start, 0, currWall.x_end - currWall.x_start, 500, 0xFFFFFF00}, false);
	foundWall->x_end = currWall.x_end;
	if (nextWall != foundWall)
	{
		foundWall = foundWall->next;
		nextWall = nextWall->next;
		delete_seg(box, foundWall);
	}
}

void	add_wall_in_fov(t_box *box, t_angle v1, t_angle v2, t_seg seg, int color)
{
	int v1_x = angle_to_screen(v1);
	int v2_x = angle_to_screen(v2);

	if (v1_x < 0)
		v1_x = 0;
	if (v2_x < 0)
		v2_x = 0;
	if (v1_x > SCREENWIDTH)
		v1_x = SCREENWIDTH;
	if (v2_x > SCREENWIDTH)
		v2_x = SCREENWIDTH;
	// printf("%i | %i\n", v1_x, v2_x);
	// draw_line(&box->image, v1_x, 0, v1_x, SCREENHEIGHT, color);
	// draw_line(&box->image, v2_x, 0, v2_x, SCREENHEIGHT, color);
	clip_wall(box, (t_solid_seg){v1_x, v2_x, color, NULL}, seg);
	// (void)color;
}

void	update_screen(t_box *box)
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

static void	render_subsector(t_box *box, int subsector_id)
{
	t_ssector	subsector = box->WAD.maps[0].ssectors[subsector_id];
	t_seg		seg;
	t_vertex	start, end;

	uint32_t	i = -1;
	// printf("Subsector %i\n", subsector_id);
	while (++i < subsector.seg_count)
	{
		seg = box->map->segs[subsector.first_seg_id + i];
		start = box->map->vertexes[seg.start_vertex_id];
		end = box->map->vertexes[seg.end_vertex_id];

		if (clip_vertexes_in_FOV(box, start, end))
		{
			int *color = (int*)(box->map->sidedefs[box->map->linedef[seg.linedef_id].right_sidedef].lower_texture);
			draw_line(&box->minimap,
				remap_x_to_screen(box, start.x),
				remap_y_to_screen(box, start.y),
				remap_x_to_screen(box, end.x),
				remap_y_to_screen(box, end.y),
				*color);
			if (box->map->linedef[seg.linedef_id].left_sidedef == 0xFFFF)
				add_wall_in_fov(box, angle_to_vortex(box, start), angle_to_vortex(box, end), seg, *color);
		}
	}
}

static bool	is_point_on_left_side(t_box *box, int x, int y, int node_id)
{
	int dx = x - box->map->nodes[node_id].partition_x;
	int dy = y - box->map->nodes[node_id].partition_y;

	return (((dx * box->map->nodes[node_id].change_partition_y) - (dy * box->map->nodes[node_id].change_partition_x)) <= 0);
}

void	render_bsp_nodes(t_box *box, int node_id)
{
	if (node_id & LEAF_NODE)
	{
		render_subsector(box, node_id & (~LEAF_NODE));
		return ;
	}
	// update_screen(box);

	if (is_point_on_left_side(box, box->map->player.x, box->map->player.y, node_id))
	{
		render_bsp_nodes(box, box->map->nodes[node_id].left_child_id);
		render_bsp_nodes(box, box->map->nodes[node_id].right_child_id);
	}
	else
	{
		render_bsp_nodes(box, box->map->nodes[node_id].right_child_id);
		render_bsp_nodes(box, box->map->nodes[node_id].left_child_id);
	}
}


