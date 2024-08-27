	/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phelebra <xhelp00@gmail.com>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/10 16:51:55 by jbartosi          #+#    #+#             */
/*   Updated: 2023/10/16 16:04:47 by phelebra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef CUB3D_H
# define CUB3D_H

# include <sys/socket.h>
# include <sys/ioctl.h>
# include <net/if.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/epoll.h>
# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <math.h>
# include <sys/time.h>
# include <dirent.h>
# include <pthread.h>
# include "Libft/libft.h"
# include "minilibx/mlx.h"
# include "cute_png/cute_png.h"
# include "cute_sound/cute_sound.h"
# include <signal.h>
# include <sys/wait.h>
# include <stdbool.h>
# include <errno.h>

# define MOUSE_CONTROL 0
# define SCREENWIDTH 1280
# define SCREENHEIGHT 720
# define LEAF_NODE 0x8000
# define M_PI 3.14159265358979323846
# define FOV 90

typedef struct s_image
{
	void			*img;
	unsigned char	*addr;
	int				bits_pp;
	int				line_len;
	int				endian;
	int				height;
	int				width;
	int				n_col;
	int				n_row;
	int				one_x;
	int				one_y;
	char			*name;
}				t_image;

typedef struct s_directory
{
	uint32_t	lump_offset;
	uint32_t	lump_size;
	char		name[9];
}				t_directory;

typedef struct s_vertex
{
	int16_t		x;
	int16_t		y;
}				t_vertex;

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

typedef struct s_angle
{
	float	angle_val;
}				t_angle;

typedef struct s_player
{
	int			x;
	int			y;
	t_angle		angle;
}				t_player;

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
typedef struct s_map
{
	char		name[5];
	uint32_t	n_vertexes;
	t_vertex	*vertexes;
	uint32_t	n_linedefs;
	t_linedef	*linedef;
	uint32_t	n_things;
	t_thing		*things;
	uint32_t	n_nodes;
	t_node		*nodes;
	uint32_t	n_ssectors;
	t_ssector	*ssectors;
	uint32_t	n_segs;
	t_seg		*segs;
	t_player	player;
	int			min_x, max_x;
	int			min_y, max_y;
	int			automap_scale_factor;
	uint32_t	bsp_layer;
}				t_map;

typedef struct s_WAD
{
	int				fd;
	struct s_header
	{
		char			type[5];
		uint32_t		dir_count;
		uint32_t		dir_offset;
	}				header;
	t_directory		*dirs;
	t_map			maps[9];
}				t_WAD;

enum e_textures
{
	UI_PICKUPS,
};

typedef struct s_box
{
	void			*mlx;
	void			*win;
	t_image			image;
	t_image			minimap;
	t_image			*textures;
	t_WAD			WAD;
}				t_box;

//Hook.c
int			exit_hook(t_box *box);
int			key_press(int key, t_box *box);
int			key_release(int key, t_box *box);
int			mouse_press(int keycode, int x, int y, t_box *box);
int			mouse_release(int keycode, int x, int y, t_box *box);
int			mouse_move(int x, int y, t_box *box);

//Parser.c
int			parser(t_box *box);

//Values.c
void		init_textures(t_box *box);

//Image.c
void		my_mlx_put_image_to_window(t_box *box, t_image *img, int x, int y, int id);
void		my_mlx_pyxel_put(t_image *img, int x, int y, int color);
void		draw_line(t_image *image, int beginX, int beginY, int endX, int endY, int color);
void		draw_rect(t_box *box, int beginX, int beginY, int endX, int endY, int color);
t_image		*new_image(void *mlx, t_image *img, int width, int height);
t_image		*img_resize(void *mlx_ptr, t_image *src_img, float n_times_bigger);
void		png_file_to_image(void *mlx, t_image *image, char *file);
void		split_spritesheet(t_image *image, int n_col, int n_row, int one_x, int one_y);
void		update_screen(t_box *box);

//Casting.c
void		render_bsp_nodes(t_box *box, int node_id);
void		draw_automap(t_box *box);
void		render_fov(t_box *box);

//Angles.c
void		normalize_360(float *angle);
bool		clip_vertexes_in_FOV(t_box *box, t_vertex v1, t_vertex v2);
int			angle_to_screen(t_box *box, t_angle angle);
t_angle		angle_to_vortex(t_box *box, t_vertex vortex);


#endif
