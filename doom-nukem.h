/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   doom-nukem.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phelebra <xhelp00@gmail.com>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/10 16:51:55 by jbartosi          #+#    #+#             */
/*   Updated: 2023/10/16 16:04:47 by phelebra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef DOOM_NUKEM_H
# define DOOM_NUKEM_H

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
# include "map.h"

# define MOUSE_CONTROL 0
# define SCREENWIDTH 1280
# define SCREENHEIGHT 720
# define LEAF_NODE 0x8000
# define M_PI 3.14159265358979323846
# define FOV 90
# define MAX_TEXTURES 100
# define N_MAPS 9

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

typedef struct s_rect
{
	uint32_t	x, y;
	uint32_t	width, height;
	int			color;
}				t_rect;
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
	t_map			*map;
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
void		init_values(t_box *box);
t_solid_seg	*new_solid_seg(int start, int end);
void		add_solid_seg_after(t_box *box, t_solid_seg *what, t_solid_seg *where);
void		delete_seg(t_box *box, t_solid_seg *seg);
void		free_solid_segs(t_box *box);
void		print_solid_segs(t_box *box);
int			get_wall_color(t_box *box, char *str);

//Image.c
void		my_mlx_put_image_to_window(t_box *box, t_image *img, int x, int y, int id);
void		my_mlx_pyxel_put(t_image *img, int x, int y, int color);
void		draw_line(t_image *image, int beginX, int beginY, int endX, int endY, int color);
void		draw_rect(t_box *box, t_rect rect, bool filled);
t_image		*new_image(void *mlx, t_image *img, int width, int height);
t_image		*img_resize(void *mlx_ptr, t_image *src_img, float n_times_bigger);
void		png_file_to_image(void *mlx, t_image *image, char *file);
void		split_spritesheet(t_image *image, int n_col, int n_row, int one_x, int one_y);

//Casting.c
int			remap_x_to_screen(t_box *box, int x);
int			remap_y_to_screen(t_box *box, int y);
void		render_bsp_nodes(t_box *box, int node_id);

//Angles.c
void		normalize_360(float *angle);
float		get_cos_value(t_angle angle);
float		get_sin_value(t_angle angle);
float		get_tan_value(t_angle angle);
float		get_signed_value(t_angle angle);
t_angle		new_angle(float angle);
int			angle_to_screen(t_angle angle);
t_angle		angle_to_vortex(t_box *box, t_vertex vortex);


#endif
