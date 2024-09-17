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

/**
 * Remaps the x coordinate to the screen for the automap
 *
 * @param t_box* box
 * @param int x
 *
 * @return {int} - remapped x
 */
int	remap_x_to_screen(t_box *box, int x)
{
	return (((x + (box->map->min_x * -1)) / box->map->automap_scale_factor));
}

/**
 * Remaps the y coordinate to the screen for the automap
 *
 * @param t_box* box
 * @param int y
 *
 * @return {int} - remapped y
 */
int	remap_y_to_screen(t_box *box, int y)
{
	return (((3500 - (y + (box->map->min_y * -1))) / box->map->automap_scale_factor));
}

/**
 * Calculates the distance between the player and a point
 *
 * @param t_box* box
 * @param t_vertex* ver
 *
 * @return {float} - distance
 */
static float	player_to_point(t_box * box, t_vertex *ver)
{
	// distance = square root ((X2 - X1)^2 + (y2 - y1)^2)
	return (sqrt(pow(box->map->player.x - ver->x, 2) + pow(box->map->player.y - ver->y, 2)));
}

/**
 * Calculates the scale factor of the wall
 *
 * @param t_box* box
 * @param int v_x_screen
 * @param t_angle seg_to_normal
 * @param float dist_to_normal
 *
 * @return {float} - scale factor
 */
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

/**
 * Draws upper section of the wall, usually the lowered ceiling of the next room
 *
 * @param t_box* box
 * @param t_segment_render_data* render_data
 * @param int ix
 * @param int curr_ceiling
 * @param int color
 */
static void draw_upper_section(t_box *box, t_segment_render_data *render_data, int ix, int curr_ceiling, int color)
{
	if (render_data->b_draw_upper_section)
	{
		int upper_height = render_data->upper_height;
		render_data->upper_height += render_data->upper_height_step;

		if (upper_height >= box->map->floor_clip_height[ix])
			upper_height = box->map->floor_clip_height[ix] - 1;

		if (upper_height >= curr_ceiling)
		{
			draw_line(&box->image, ix, curr_ceiling, ix, upper_height, color);
			box->map->ceiling_clip_height[ix] = upper_height;
		}
		else
			box->map->ceiling_clip_height[ix] = curr_ceiling - 1;
	}
	else if (render_data->update_ceiling)
		box->map->ceiling_clip_height[ix] = curr_ceiling - 1;
}

/**
 * Draws middle section of the wall
 *
 * @param t_box* box
 * @param int ix
 * @param int curr_ceiling
 * @param int curr_floor
 * @param int color
 */
static void draw_middle_section(t_box *box, int ix, int curr_ceiling, int curr_floor, int color)
{
	draw_line(&box->image, ix, curr_ceiling, ix, curr_floor, color);
	box->map->ceiling_clip_height[ix] = SCREENHEIGHT;
	box->map->floor_clip_height[ix] = -1;
}

/**
 * Draws lower section of the wall, usually the raised floor of the next room
 *
 * @param t_box* box
 * @param t_segment_render_data* render_data
 * @param int ix
 * @param int curr_floor
 * @param int color
 */
static void draw_lower_section(t_box *box, t_segment_render_data *render_data, int ix, int curr_floor, int color)
{
	if (render_data->b_draw_lower_section)
	{
		int lower_height = render_data->lower_height;
		render_data->lower_height += render_data->lower_height_step;

		if (lower_height <= box->map->ceiling_clip_height[ix])
			lower_height = box->map->ceiling_clip_height[ix] + 1;

		if (lower_height <= curr_floor)
		{
			draw_line(&box->image, ix, lower_height, ix, curr_floor, color);
			box->map->floor_clip_height[ix] = lower_height;
		}
		else
			box->map->floor_clip_height[ix] = curr_floor + 1;
	}
	else if (render_data->update_floor)
		box->map->floor_clip_height[ix] = curr_floor + 1;
}

/**
 * Validates the range of the ceiling and floor
 *
 * @param t_box* box
 * @param t_segment_render_data* render_data
 * @param int* ix
 * @param int* curr_ceiling
 * @param int* curr_floor
 */
static bool	validate_range(t_box *box, t_segment_render_data *render_data, int *ix, int *curr_ceiling, int *curr_floor)
{
	if (*curr_ceiling < box->map->ceiling_clip_height[*ix] + 1)
		*curr_ceiling = box->map->ceiling_clip_height[*ix] + 1;

	if (*curr_floor >= box->map->floor_clip_height[*ix])
		*curr_floor = box->map->floor_clip_height[*ix] - 1;

	if (*curr_ceiling > *curr_floor)
	{
		render_data->ceiling_end += render_data->ceiling_step;
		render_data->floor_start += render_data->floor_step;
		*ix = *ix + 1;
		return (false);
	}
	return (true);
}

/**
 * Renders a segment on the screen
 *
 * @param t_box* box
 * @param t_segment_render_data* render_data
 */
static void	render_segment(t_box *box, t_segment_render_data *render_data)
{
	int color, curr_ceiling, curr_floor;

	if (render_data->p_seg->p_left_sector)
		color = get_wall_color(box, render_data->p_seg->p_linedef->p_right_sidedef->upper_texture);
	else
		color = get_wall_color(box, render_data->p_seg->p_linedef->p_right_sidedef->middle_texture);

	int ix = render_data->v1_x_screen;

	while (ix <= render_data->v2_x_screen)
	{
		curr_ceiling = render_data->ceiling_end;
		curr_floor = render_data->floor_start;

		if (!validate_range(box, render_data, &ix, &curr_ceiling, &curr_floor))
			continue;
		if (render_data->p_seg->p_left_sector)
		{
			draw_upper_section(box, render_data, ix, curr_ceiling, color);
			draw_lower_section(box, render_data, ix, curr_floor, color);
		}
		else
			draw_middle_section(box, ix, curr_ceiling, curr_floor, color);

		render_data->ceiling_end += render_data->ceiling_step;
		render_data->floor_start += render_data->floor_step;
		ix++;
	}
}

/**
 * Updates the ceiling and floor flags
 *
 * @param t_box* box
 * @param t_segment_render_data* render_data
 */
static void	ceiling_floor_update(t_box *box, t_segment_render_data *render_data)
{
	if (!render_data->p_seg->p_left_sector)
	{
		render_data->update_floor = true;
		render_data->update_ceiling = true;
		return ;
	}

	if (render_data->left_sector_ceiling != render_data->right_sector_ceiling)
		render_data->update_ceiling = true;
	else
		render_data->update_ceiling = false;

	if (render_data->left_sector_floor != render_data->right_sector_floor)
		render_data->update_floor = true;
	else
		render_data->update_floor = false;

	// closed door
	if (render_data->p_seg->p_left_sector->ceiling_height <= render_data->p_seg->p_right_sector->floor_height || render_data->p_seg->p_left_sector->floor_height >= render_data->p_seg->p_right_sector->ceiling_height)
		render_data->update_ceiling = render_data->update_floor = true;

	// below view plane
	if (render_data->p_seg->p_right_sector->ceiling_height <= box->map->player.z)
		render_data->update_ceiling = false;

	// above view plane
	if (render_data->p_seg->p_right_sector->floor_height >= box->map->player.z)
		render_data->update_floor = false;
}

/**
 * Calculates the height of the wall and renders it
 *
 * @param t_box* box
 * @param int v1_x_screen
 * @param int v2_x_screen
 * @param t_angle v1_angle
 * @param t_seg seg
 */
static void	calculate_wall_height(t_box *box, int v1_x_screen, int v2_x_screen, t_angle v1_angle, t_seg seg)
{
	t_angle seg_to_normal_angle = new_angle(seg.angle.angle_val + 90);
	t_angle normal_to_v1_angle = new_angle(seg_to_normal_angle.angle_val - v1_angle.angle_val);
	t_angle seg_to_player_angle = new_angle(90 - normal_to_v1_angle.angle_val);

	t_segment_render_data	render_data = {0};

	render_data.v1_x_screen = v1_x_screen;
	render_data.v2_x_screen = v2_x_screen;
	render_data.v1_angle = v1_angle;
	// render_data.v2_angle = v2_angle;

	render_data.dist_to_v1 = player_to_point(box, seg.p_start_vertex);
	render_data.dist_to_normal = get_sin_value(seg_to_player_angle) * render_data.dist_to_v1;

	render_data.v1_scale_factor = get_scale_factor(box, v1_x_screen, seg_to_normal_angle, render_data.dist_to_normal);
	render_data.v2_scale_factor = get_scale_factor(box, v2_x_screen, seg_to_normal_angle, render_data.dist_to_normal);

	render_data.steps = (render_data.v2_scale_factor - render_data.v1_scale_factor) / (v2_x_screen - v1_x_screen);

	render_data.right_sector_ceiling = seg.p_right_sector->ceiling_height - box->map->player.z;
	render_data.right_sector_floor = seg.p_right_sector->floor_height - box->map->player.z;

	render_data.ceiling_step = -(render_data.right_sector_ceiling * render_data.steps);
	render_data.ceiling_end = roundf((SCREENHEIGHT / 2) - (render_data.right_sector_ceiling * render_data.v1_scale_factor));

	render_data.floor_step = -(render_data.right_sector_floor * render_data.steps);
	render_data.floor_start = roundf((SCREENHEIGHT / 2) - (render_data.right_sector_floor * render_data.v1_scale_factor));

	render_data.p_seg = &seg;

	if (seg.p_left_sector)
	{
		render_data.left_sector_ceiling = seg.p_left_sector->ceiling_height - box->map->player.z;
		render_data.left_sector_floor = seg.p_left_sector->floor_height - box->map->player.z;

		ceiling_floor_update(box, &render_data);

		if (render_data.left_sector_ceiling < render_data.right_sector_ceiling)
		{
			render_data.b_draw_upper_section = true;
			render_data.upper_height_step = -(render_data.left_sector_ceiling * render_data.steps);
			render_data.upper_height = roundf((SCREENHEIGHT / 2) - (render_data.left_sector_ceiling * render_data.v1_scale_factor));
		}

		if (render_data.left_sector_floor > render_data.right_sector_floor)
		{
			render_data.b_draw_lower_section = true;
			render_data.lower_height_step = -(render_data.left_sector_floor * render_data.steps);
			render_data.lower_height = roundf((SCREENHEIGHT / 2) - (render_data.left_sector_floor * render_data.v1_scale_factor));
		}
	}

	render_segment(box, &render_data);
}

/**
 * Draws passable walls from near to far, makign sure that the walls are not drawn over each other
 *
 * @param t_box* box
 * @param int v1_x_screen
 * @param int v2_x_screen
 * @param t_angle v1_angle
 * @param t_seg seg
 */
static void	clip_pass_wall(t_box *box, int v1_x_screen, int v2_x_screen, t_angle v1_angle, t_seg seg)
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
			return ;
		}
		calculate_wall_height(box, currWall.x_start, foundWall->x_start - 1, v1_angle, seg);
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
			return ;
	}
	// printf("%i, %i | %i, %i\n", nextWall->x_start, nextWall->x_end, foundWall->x_start, foundWall->x_end);
	calculate_wall_height(box, nextWall->x_end + 1, currWall.x_end, v1_angle, seg);
}

/**
 * Draws solid walls from near to far, makign sure that the walls are not drawn over each other
 *
 * @param t_box* box
 * @param int v1_x_screen
 * @param int v2_x_screen
 * @param t_angle v1_angle
 * @param t_seg seg
 */
static void	clip_solid_wall(t_box *box, int v1_x_screen, int v2_x_screen, t_angle v1_angle, t_seg seg)
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

/**
 * Adds a wall in player's FOV
 *
 * @param t_box* box
 * @param t_angle v1_angle
 * @param t_angle v1_angle_from_player
 * @param t_angle v2_angle_from_player
 * @param t_seg seg
 */
static void	add_wall_in_fov(t_box *box, t_angle v1_angle, t_angle v1_angle_from_player, t_angle v2_angle_from_player, t_seg seg)
{
	int v1_x = angle_to_screen(v1_angle_from_player);
	int v2_x = angle_to_screen(v2_angle_from_player);

	if (v1_x == v2_x)
		return;

	if (seg.p_left_sector == NULL)
	{
		clip_solid_wall(box, v1_x, v2_x, v1_angle, seg);
		return ;
	}

	if (seg.p_left_sector->ceiling_height <= seg.p_right_sector->floor_height
		|| seg.p_left_sector->floor_height >= seg.p_right_sector->ceiling_height)
	{
		clip_solid_wall(box, v1_x, v2_x, v1_angle, seg);
		return ;
	}

	if (seg.p_right_sector->ceiling_height != seg.p_left_sector->ceiling_height
		|| seg.p_right_sector->floor_height != seg.p_left_sector->floor_height)
	{
		clip_pass_wall(box, v1_x, v2_x, v1_angle, seg);
		return ;
	}
}

/**
 * Clips vertexes in player's FOV and fills angle struct pointers
 *
 * @param t_box* box
 * @param t_vertex v1
 * @param t_vertex v2
 * @param t_angle* v1_angle
 * @param t_angle* v2_angle
 * @param t_angle* v1_angle_from_player
 * @param t_angle* v2_angle_from_player
 *
 * @return bool
 */
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

/**
 * Renders a subsector by rendering all the segs in it
 *
 * @param t_box* box
 * @param int subsector_id
 */
static void	render_subsector(t_box *box, int subsector_id)
{
	t_WAD_ssector	subsector;
	t_seg			seg;
	uint32_t		i;

	subsector = box->map->WAD_ssectors[subsector_id];
	i = -1;
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

/**
 * 	Checks if the point is on the left side of the node
 *
 * @param t_box* box
 * @param int x
 * @param int y
 * @param int node_id
 *
 * @return bool
 */
static bool	is_point_on_left_side(t_box *box, int x, int y, int node_id)
{
	int dx = x - box->map->WAD_nodes[node_id].partition_x;
	int dy = y - box->map->WAD_nodes[node_id].partition_y;

	return (((dx * box->map->WAD_nodes[node_id].change_partition_y) - (dy * box->map->WAD_nodes[node_id].change_partition_x)) <= 0);
}

/**
 * Binary search tree traversal with rendering of each leas subsector
 *
 * @param t_box* box
 * @param int node_id - Id of the root node, usually the last node in the array
 */
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


