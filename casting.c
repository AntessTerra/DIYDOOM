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

static float	player_to_point(t_box * box, t_vertex *ver)
{
	// distance = square root ((X2 - X1)^2 + (y2 - y1)^2)
	return (sqrt(pow(box->map->player.x - ver->x, 2) + pow(box->map->player.y - ver->y, 2)));
}

static void	calc_ceeling_floor_heights(t_box *box, t_seg seg, int v_x_screen, float dist_to_v, float *ceiling_v_on_screen, float *floor_v_on_screen)
{
	float	ceiling = seg.p_right_sector->ceiling_height - box->map->player.z;
	float	floor = seg.p_right_sector->floor_height - box->map->player.z;

	t_angle	v_screen_angle = box->map->screen_x_to_angle[v_x_screen];

	float	dist_player_to_screen = (SCREENWIDTH / 2) / get_tan_value((t_angle){FOV / 2});
	float	dist_to_v_screen = dist_player_to_screen / get_cos_value(v_screen_angle);
	*ceiling_v_on_screen = (fabs(ceiling) * dist_to_v_screen) / dist_to_v;
	*floor_v_on_screen = (fabs(floor) * dist_to_v_screen) / dist_to_v;

	if (ceiling > 0)
		*ceiling_v_on_screen = (SCREENHEIGHT / 2) - *ceiling_v_on_screen;
	else
		*ceiling_v_on_screen += (SCREENHEIGHT / 2);

	if (floor > 0)
		*floor_v_on_screen = (SCREENHEIGHT / 2) - *floor_v_on_screen;
	else
		*floor_v_on_screen += (SCREENHEIGHT / 2);
}

static void	partial_seg(t_box *box, t_angle v1_angle, t_angle v2_angle, float *dist_to_v, bool is_left, t_seg seg)
{
	float	side_c = sqrt(pow(seg.p_start_vertex->x - seg.p_end_vertex->x, 2) + pow(seg.p_start_vertex->y - seg.p_end_vertex->y, 2));
	t_angle v1_to_v1_span = new_angle(v1_angle.angle_val - v2_angle.angle_val);
	float	sine_angle_b = *dist_to_v * get_sin_value(v1_to_v1_span) / side_c;
	t_angle	angle_b = new_angle(asinf(sine_angle_b) * 180.0 / M_PI);
	t_angle	angle_a = new_angle(180 - v1_to_v1_span.angle_val - angle_b.angle_val);

	t_angle	angle_v_to_FOV;
	if (is_left)
		angle_v_to_FOV = new_angle(v1_angle.angle_val - (box->map->player.angle.angle_val + 45));
	else
		angle_v_to_FOV = new_angle((box->map->player.angle.angle_val - 45) - v2_angle.angle_val);

	t_angle	new_angle_b = new_angle(180 - angle_v_to_FOV.angle_val - angle_a.angle_val);
	*dist_to_v = *dist_to_v * get_sin_value(angle_a) / get_sin_value(new_angle_b);

}

static void	draw_clipped_wall(t_box *box, int v1_x_screen, int v2_x_screen, t_angle v1_angle, t_angle v2_angle, t_seg seg)
{
	if (v1_x_screen > SCREENWIDTH || v1_x_screen <= 0 || v2_x_screen > SCREENWIDTH || v2_x_screen <= 0)
		return ;
	// (void)v1_angle, (void)v2_angle;
	float	dist_to_v1 = player_to_point(box, seg.p_start_vertex);
	float	dist_to_v2 = player_to_point(box, seg.p_end_vertex);

	if (v1_x_screen <= 0)
		partial_seg(box, v1_angle, v2_angle, &dist_to_v1, true, seg);
	if (v2_x_screen >= SCREENWIDTH - 1)
		partial_seg(box, v1_angle, v2_angle, &dist_to_v2, false, seg);

	float	ceiling_v1_on_screen;
	float	floor_v1_on_screen;
	float	ceiling_v2_on_screen;
	float	floor_v2_on_screen;

	calc_ceeling_floor_heights(box, seg, v1_x_screen, dist_to_v1, &ceiling_v1_on_screen, &floor_v1_on_screen);
	calc_ceeling_floor_heights(box, seg, v2_x_screen, dist_to_v2, &ceiling_v2_on_screen, &floor_v2_on_screen);

	int color = get_wall_color(box, seg.p_linedef->p_right_sidedef->middle_texture);
	if (v1_x_screen > SCREENWIDTH || v1_x_screen <= 0 || v2_x_screen > SCREENWIDTH || v2_x_screen <= 0
		|| ceiling_v1_on_screen > SCREENHEIGHT || ceiling_v1_on_screen <= 0 || ceiling_v2_on_screen > SCREENHEIGHT || ceiling_v2_on_screen <= 0
		|| floor_v1_on_screen > SCREENHEIGHT || floor_v1_on_screen <= 0 || floor_v2_on_screen > SCREENHEIGHT || floor_v2_on_screen <= 0)
		return ;

	draw_line(&box->image, v1_x_screen, ceiling_v1_on_screen, v1_x_screen, floor_v1_on_screen, color);
	draw_line(&box->image, v2_x_screen, ceiling_v2_on_screen, v2_x_screen, floor_v2_on_screen, color);
	draw_line(&box->image, v1_x_screen, ceiling_v1_on_screen, v2_x_screen, ceiling_v2_on_screen, color);
	draw_line(&box->image, v1_x_screen, floor_v1_on_screen, v2_x_screen, floor_v2_on_screen, color);
}

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
static void	clip_wall(t_box *box, t_solid_seg currWall, t_angle v1_angle, t_angle v2_angle, t_seg seg)
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
			draw_clipped_wall(box, currWall.x_start, currWall.x_end, v1_angle, v2_angle, seg);
			t_solid_seg *p_curr_wall = new_solid_seg(currWall.x_start, currWall.x_end);
			add_solid_seg_after(box, p_curr_wall, p_curr_wall);
			return ;
		}
		draw_clipped_wall(box, currWall.x_start, currWall.x_end, v1_angle, v2_angle, seg);
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
		draw_clipped_wall(box, currWall.x_start, currWall.x_end, v1_angle, v2_angle, seg);
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
	draw_clipped_wall(box, currWall.x_start, currWall.x_end, v1_angle, v2_angle, seg);
	foundWall->x_end = currWall.x_end;
	if (nextWall != foundWall)
	{
		foundWall = foundWall->next;
		nextWall = nextWall->next;
		delete_seg(box, foundWall);
	}
}

void	add_wall_in_fov(t_box *box, t_angle v1_angle, t_angle v2_angle, t_angle v1_angle_from_player, t_angle v2_angle_from_player, t_seg seg)
{
	int v1_x = angle_to_screen(v1_angle_from_player);
	int v2_x = angle_to_screen(v2_angle_from_player);

	if (v1_x == v2_x)
		return;

	// printf("%i | %i\n", v1_x, v2_x);
	// draw_line(&box->image, v1_x, 0, v1_x, SCREENHEIGHT, color);
	// draw_line(&box->image, v2_x, 0, v2_x, SCREENHEIGHT, color);
	clip_wall(box, (t_solid_seg){v1_x, v2_x, NULL}, v1_angle, v2_angle, seg);
	// (void)color;
}

static bool	clip_vertexes_in_FOV(t_box *box, t_vertex v1, t_vertex v2, t_angle *v1_angle, t_angle *v2_angle, t_angle *v1_angle_from_player, t_angle *v2_angle_from_player)
{
	*v1_angle = angle_to_vortex(box, v1);
	*v2_angle = angle_to_vortex(box, v2);

	t_angle	v1_to_v2 = new_angle(v1_angle->angle_val - v2_angle->angle_val);

	if (v1_to_v2.angle_val >= 180)
		return (false);

	*v1_angle_from_player = new_angle(v1_angle->angle_val - box->map->player.angle.angle_val);
	*v2_angle_from_player = new_angle(v2_angle->angle_val - box->map->player.angle.angle_val);

	t_angle	half_FOV = new_angle(FOV / 2);

	t_angle	v1_moved = new_angle(v1_angle_from_player->angle_val + half_FOV.angle_val);

	if (v1_moved.angle_val > FOV)
	{
		t_angle	v1_moved_angle = new_angle(v1_moved.angle_val - FOV);

		if (v1_moved_angle.angle_val >= v1_to_v2.angle_val)
			return (false);
		v1_angle_from_player->angle_val = half_FOV.angle_val;
	}

	t_angle	v2_moved = new_angle(half_FOV.angle_val - v2_angle_from_player->angle_val);

	if (v2_moved.angle_val > FOV)
		v2_angle_from_player->angle_val -= half_FOV.angle_val;

	v1_angle_from_player->angle_val += 90;
	v2_angle_from_player->angle_val += 90;

	return (true);
}

static void	render_subsector(t_box *box, int subsector_id)
{
	t_WAD_ssector	subsector = box->WAD.maps[0].WAD_ssectors[subsector_id];
	t_seg		seg;

	uint32_t	i = -1;
	// printf("Subsector %i\n", subsector_id);
	while (++i < subsector.seg_count)
	{
		seg = box->map->segs[subsector.first_seg_id + i];
		t_angle	v1_angle, v2_angle, v1_angle_from_player, v2_angle_from_player;

		if (clip_vertexes_in_FOV(box, *seg.p_start_vertex, *seg.p_end_vertex, &v1_angle, &v2_angle, &v1_angle_from_player, &v2_angle_from_player))
		{
			draw_line(&box->minimap,
				remap_x_to_screen(box, (*seg.p_start_vertex).x),
				remap_y_to_screen(box, (*seg.p_start_vertex).y),
				remap_x_to_screen(box, (*seg.p_end_vertex).x),
				remap_y_to_screen(box, (*seg.p_end_vertex).y),
				get_wall_color(box, seg.p_linedef->p_right_sidedef->middle_texture));
			if (!seg.p_left_sector)
				add_wall_in_fov(box, v1_angle, v2_angle, v1_angle_from_player, v2_angle_from_player, seg);
		}
	}
}

static bool	is_point_on_left_side(t_box *box, int x, int y, int node_id)
{
	int dx = x - box->map->WAD_nodes[node_id].partition_x;
	int dy = y - box->map->WAD_nodes[node_id].partition_y;

	return (((dx * box->map->WAD_nodes[node_id].change_partition_y) - (dy * box->map->WAD_nodes[node_id].change_partition_x)) <= 0);
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
		render_bsp_nodes(box, box->map->WAD_nodes[node_id].left_child_id);
		render_bsp_nodes(box, box->map->WAD_nodes[node_id].right_child_id);
	}
	else
	{
		render_bsp_nodes(box, box->map->WAD_nodes[node_id].right_child_id);
		render_bsp_nodes(box, box->map->WAD_nodes[node_id].left_child_id);
	}
}


