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
# define TEXTUREWIDTH 64
# define TEXTUREHEIGHT 64
# define UDIV 1
# define VDIV 1
# define VMOVE 0.0
# define MINIMAP_OFFSET 10
# define PI (atan(1) * 4)

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
	int16_t	x;
	int16_t	y;
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

typedef struct s_map
{
	char		name[5];
	uint32_t	n_vertexes;
	t_vertex	*vertexes;
	uint32_t	n_linedefs;
	t_linedef	*linedef;
	int			min_x, max_x;
	int			min_y, max_y;
	int			automap_scale_factor;
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

typedef struct s_box
{
	void			*mlx;
	void			*win;
	t_image			image;
	t_WAD			WAD;
}				t_box;

typedef struct s_node
{
	void			*data;
	struct s_node	*left;
	struct s_node	*right;
}				t_node;

//Hook.c
int			exit_hook(t_box *box);
int			key_press(int key, t_box *box);
int			key_release(int key, t_box *box);
int			mouse_press(int keycode, int x, int y, t_box *box);
int			mouse_release(int keycode, int x, int y, t_box *box);
int			mouse_move(int x, int y, t_box *box);

//Btree.c
struct s_node	*createNode(int data);

//Parser.c
int			parser(t_box *box);

#endif
