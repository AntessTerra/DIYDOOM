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

static float	get_scale_factor(t_box *box, int v_x_screen, t_angle seg_to_normal, float dist_to_normal)
{
	static float MAX_SCALE_FACTOR = 64.0f;
	static float MIN_SCALE_FACTOR = 0.00390625f;

	t_angle screen_x_angle = box->map->screen_x_to_angle[v_x_screen];
	t_angle skew_angle = new_angle(box->map->screen_x_to_angle[v_x_screen].angle_val + box->map->player.angle.angle_val - seg_to_normal.angle_val);

	float scale_factor = (box->map->player.dist_to_screen * get_cos_value(skew_angle)) / (dist_to_normal * get_cos_value(screen_x_angle));

	if (scale_factor > MAX_SCALE_FACTOR)
		scale_factor = MAX_SCALE_FACTOR;
	else if (scale_factor < MIN_SCALE_FACTOR)
		scale_factor = MIN_SCALE_FACTOR;

	return (scale_factor);
}

void	calculate_wall_height(t_box *box, int v1_x_screen, int v2_x_screen, t_angle v1_angle, t_seg seg)
{
	t_angle seg_to_normal_angle = new_angle(seg.angle.angle_val + 90);

	t_angle normal_to_v1_angle = new_angle(seg_to_normal_angle.angle_val - v1_angle.angle_val);

	t_angle seg_to_player_angle = new_angle(90 - normal_to_v1_angle.angle_val);

	float dist_to_v1 = player_to_point(box, seg.p_start_vertex);
	float dist_to_normal = get_sin_value(seg_to_player_angle) * dist_to_v1;

	float v1_scale_factor = get_scale_factor(box, v1_x_screen, seg_to_normal_angle, dist_to_normal);
	float v2_scale_factor = get_scale_factor(box, v2_x_screen, seg_to_normal_angle, dist_to_normal);

	float steps = (v2_scale_factor - v1_scale_factor) / (v2_x_screen - v1_x_screen);

	float ceiling = seg.p_right_sector->ceiling_height - box->map->player.z;
	float floor = seg.p_right_sector->floor_height - box->map->player.z;

	float ceiling_step = -(ceiling * steps);
	float ceiling_end = (SCREENHEIGHT / 2) - (ceiling * v1_scale_factor);

	float floor_step = -(floor * steps);
	float floor_start = (SCREENHEIGHT / 2) - (floor * v1_scale_factor);

	int color = get_wall_color(box, seg.p_linedef->p_right_sidedef->middle_texture);

	int ix = v1_x_screen;
	while (ix <= v2_x_screen)
	{
		draw_line(&box->image, ix, ceiling_end, ix, floor_start, color);
		++ix;

		ceiling_end += ceiling_step;
		floor_start += floor_step;
	}
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
void	clip_wall(t_box *box, int v1_x_screen, int v2_x_screen, t_angle v1_angle, t_seg seg)
{
	t_solid_seg	*foundWall = NULL, *nextWall = NULL, currWall = {v1_x_screen, v2_x_screen, NULL};

	foundWall = box->map->solid_segs;
	while (foundWall && foundWall->x_end < currWall.x_start - 1)
		foundWall = foundWall->next;
	// printf("%i, %i\n", currWall.x_start, currWall.x_end);
	// print_solid_segs(box);
	if (currWall.x_start < foundWall->x_start)
	{
		// printf("TEST\n");
		if (currWall.x_end < foundWall->x_start - 1)
		{
			calculate_wall_height(box, currWall.x_start, currWall.x_end, v1_angle, seg);
			add_solid_seg_before(box, foundWall, new_solid_seg(currWall.x_start, currWall.x_end));
			return ;
		}
		calculate_wall_height(box, currWall.x_start, foundWall->x_start - 1, v1_angle, seg);
		foundWall->x_start = currWall.x_start;
	}
	// printf("%i <= %i\n", currWall.x_end, foundWall->x_end);
	if (currWall.x_end <= foundWall->x_end)
		return ;

	nextWall = foundWall;
	// printf("%i >= %i\n", currWall.x_end, nextWall->next->x_start - 1);
	while (currWall.x_end >= nextWall->next->x_start - 1)
	{
		calculate_wall_height(box, nextWall->x_end + 1, nextWall->next->x_start - 1, v1_angle, seg);
		nextWall = nextWall->next;
		if (currWall.x_end <= nextWall->x_end)
		{
			foundWall->x_end = nextWall->x_end;
			if (nextWall != foundWall)
			{
				foundWall = foundWall->next;
				nextWall = nextWall->next;
				// printf("Deleting from %i,%i	to %i,%i\n", foundWall->x_start, foundWall->x_end, nextWall->x_start, nextWall->x_end);
				delete_seg(box, foundWall, nextWall);
			}
			return ;
		}
	}
	// printf("%i, %i | %i, %i\n", nextWall->x_start, nextWall->x_end, foundWall->x_start, foundWall->x_end);
	calculate_wall_height(box, nextWall->x_end + 1, currWall.x_end, v1_angle, seg);
	foundWall->x_end = currWall.x_end;
	if (nextWall != foundWall)
	{
		foundWall = foundWall->next;
		nextWall = nextWall->next;
		// printf("Deleting from %i, %i	to %i, %i\n", foundWall->x_start, foundWall->x_end, nextWall->x_start, nextWall->x_end);
		delete_seg(box, foundWall, nextWall);
	}
}

void	add_wall_in_fov(t_box *box, t_angle v1_angle, t_angle v1_angle_from_player, t_angle v2_angle_from_player, t_seg seg)
{
	int v1_x = angle_to_screen(v1_angle_from_player);
	int v2_x = angle_to_screen(v2_angle_from_player);

	if (v1_x == v2_x)
		return;

	if (seg.p_left_sector == NULL)
		clip_wall(box, v1_x, v2_x, v1_angle, seg);
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
		v2_angle_from_player->angle_val = -half_FOV.angle_val;

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
			add_wall_in_fov(box, v1_angle, v1_angle_from_player, v2_angle_from_player, seg);
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


