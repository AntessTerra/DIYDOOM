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

# define MAX_TEXTURE_COLORS 100

// STRUCTURES TO STORE RAW WAD DATA

typedef struct s_WAD_linedef
{
	uint16_t	start_vertex_id;
	uint16_t	end_vertex_id;
	uint16_t	flags;
	uint16_t	line_type;
	uint16_t	sector_tag;
	uint16_t	right_sidedef;
	uint16_t	left_sidedef;
}				t_WAD_linedef;

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

typedef struct s_WAD_thing
{
	int16_t		x;
	int16_t		y;
	uint16_t	angle;
	uint16_t	type;
	uint16_t	flags;
}				t_WAD_thing;

typedef struct s_WAD_node
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
}				t_WAD_node;

typedef struct s_WAD_ssector
{
	uint16_t	seg_count;
	uint16_t	first_seg_id;
}				t_WAD_ssector;

typedef struct s_WAD_seg
{
	uint16_t	start_vertex_id;
	uint16_t	end_vertex_id;
	uint16_t	angle;
	uint16_t	linedef_id;
	uint16_t	direction;
	uint16_t	offset;
}				t_WAD_seg;

typedef struct s_WAD_sidedef
{
	int16_t		x_offset;
	int16_t		y_offset;
	char		upper_texture[8];
	char		lower_texture[8];
	char		middle_texture[8];
	uint16_t	sector_id;
}				t_WAD_sidedef;

typedef struct s_WAD_sector
{
	int16_t		floor_height;
	int16_t		ceiling_height;
	char		floor_texture[8];
	char		ceiling_texture[8];
	uint16_t	light_level;
	uint16_t	type;
	uint16_t	tag;
}				t_WAD_sector;

// OTHER HELPFULL STUCTURES

typedef struct s_angle
{
	float	angle_val;
}				t_angle;

typedef struct s_texture_color
{
	int		color;
	char	name[8];
}				t_texture_color;

typedef struct s_player
{
	int			x;
	int			y;
	int			z;
	int			move_speed;
	t_angle		angle;
}				t_player;

typedef struct s_vertex
{
	int16_t		x;
	int16_t		y;
}				t_vertex;
typedef struct s_sidedef
{
	int16_t			x_offset;
	int16_t			y_offset;
	char			upper_texture[8];
	char			lower_texture[8];
	char			middle_texture[8];
	t_WAD_sector	*p_sector;
}				t_sidedef;

typedef struct s_linedef
{
	t_vertex		*p_start_vertex;
	t_vertex		*p_end_vertex;
	uint16_t		flags;
	uint16_t		line_type;
	uint16_t		sector_tag;
	t_sidedef		*p_right_sidedef;
	t_sidedef		*p_left_sidedef;
}				t_linedef;

typedef struct s_seg
{
	t_vertex		*p_start_vertex;
	t_vertex		*p_end_vertex;
	t_angle			angle;
	t_linedef		*p_linedef;
	uint16_t		direction;
	uint16_t		offset;
	t_WAD_sector	*p_right_sector;
	t_WAD_sector	*p_left_sector;
}				t_seg;

typedef struct s_solid_seg
{
	int					x_start;
	int					x_end;
	struct s_solid_seg	*next;
}				t_solid_seg;

typedef struct s_map
{
	t_WAD_thing			*WAD_things;
	t_WAD_linedef		*WAD_linedefs;
	t_WAD_sidedef		*WAD_sidedefs;
	t_vertex			*vertexes;
	t_WAD_seg			*WAD_segs;
	t_WAD_ssector		*WAD_ssectors;
	t_WAD_node			*WAD_nodes;
	t_WAD_sector		*WAD_sectors;

	t_linedef		*linedefs;
	t_sidedef		*sidedefs;
	t_seg			*segs;

	uint32_t		n_things;
	uint32_t		n_linedefs;
	uint32_t		n_sidedefs;
	uint32_t		n_vertexes;
	uint32_t		n_segs;
	uint32_t		n_ssectors;
	uint32_t		n_nodes;
	uint32_t		n_sectors;

	t_player		player;
	char			name[5];
	int				min_x;
	int				min_y;
	int				max_x;
	int				max_y;
	int				automap_scale_factor;
	t_solid_seg		*solid_segs;
	t_angle			*screen_x_to_angle;
	t_texture_color	texture_to_color[MAX_TEXTURE_COLORS];
}				t_map;

#endif
