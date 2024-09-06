/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jbartosi <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/06 13:56:26 by jbartosi          #+#    #+#             */
/*   Updated: 2024/09/06 13:56:30 by jbartosi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MAP_H
# define MAP_H

# include <stdint.h>

typedef struct s_angle
{
	float	angle_val;
}				t_angle;

typedef struct s_player
{
	int			x;
	int			y;
	int			move_speed;
	t_angle		angle;
}				t_player;

typedef struct s_vertex
{
	int16_t		x;
	int16_t		y;
}				t_vertex;

typedef struct s_linedef
{
	uint16_t	start_vertex;
	uint16_t	end_vertex;
	uint16_t	flags;
	uint16_t	line_type;
	uint16_t	sector_tag;
	uint16_t	right_sidedef;
	uint16_t	left_sidedef;
}				t_linedef;

enum e_linedef_flags
{
	BLOCKING		= 0,
	BLOCKMONSTERS	= 1,
	TWOSIDED		= 2,
	DONTPEGTOP		= 4,
	DONTPEGBOTTOM	= 8,
	SECRET			= 16,
	SOUNDBLOCK		= 32,
	DONTDRAW		= 64,
	DRAW			= 128
};

typedef struct s_thing
{
	int16_t		x;
	int16_t		y;
	uint16_t	angle;
	uint16_t	type;
	uint16_t	flags;
}				t_thing;

typedef struct s_node
{
	int16_t		partition_x;
	int16_t		partition_y;
	int16_t		change_partition_x;
	int16_t		change_partition_y;

	int16_t		right_box_top;
	int16_t		right_box_bottom;
	int16_t		right_box_left;
	int16_t		right_box_right;

	int16_t		left_box_top;
	int16_t		left_box_bottom;
	int16_t		left_box_left;
	int16_t		left_box_right;

	uint16_t	right_child_id;
	uint16_t	left_child_id;
}				t_node;

typedef struct s_ssector
{
	uint16_t	seg_count;
	uint16_t	first_seg_id;
}				t_ssector;

typedef struct s_seg
{
	uint16_t	start_vertex_id;
	uint16_t	end_vertex_id;
	uint16_t	angle;
	uint16_t	linedef_id;
	uint16_t	direction;
	uint16_t	offset;
}				t_seg;

typedef struct s_sidedef
{
	int16_t		x_offset;
	int16_t		y_offset;
	char		upper_texture[8];
	char		lower_texture[8];
	char		middle_texture[8];
	uint16_t	sector_id;
}				t_sidedef;

typedef struct s_sector
{
	int16_t		floor_height;
	int16_t		ceiling_height;
	char		floor_texture[8];
	char		ceiling_texture[8];
	uint16_t	light_level;
	uint16_t	type;
	uint16_t	tag;
}				t_sector;

typedef struct s_solid_seg
{
	int					x_start;
	int					x_end;
	int					color;
	struct s_solid_seg	*next;
}				t_solid_seg;

typedef struct s_map
{
	t_thing		*things;
	t_linedef	*linedef;
	t_sidedef	*sidedefs;
	t_vertex	*vertexes;
	t_seg		*segs;
	t_ssector	*ssectors;
	t_node		*nodes;
	t_sector	*sectors;
	uint32_t	n_things;
	uint32_t	n_linedefs;
	uint32_t	n_sidedefs;
	uint32_t	n_vertexes;
	uint32_t	n_segs;
	uint32_t	n_ssectors;
	uint32_t	n_nodes;
	uint32_t	n_sectors;
	t_player	player;
	char		name[5];
	int			min_x;
	int			min_y;
	int			max_x;
	int			max_y;
	int			automap_scale_factor;
	t_solid_seg	*solid_segs;
	t_angle		*screen_x_to_angle;
}				t_map;

#endif
