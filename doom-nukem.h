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

/**
 * Structure to store data about the image
 *
 * @typedef {t_image} MLX image
 * @property {void*} img - image pointer
 * @property {unsigned char*} addr - address of the image
 * @property {int} bits_pp - bits per pixel
 * @property {int} line_len - length of the line
 * @property {int} endian - endian
 * @property {int} height - height of the image
 * @property {int} width - width of the image
 * @property {int} n_col - number of columns, used for spritesheets
 * @property {int} n_row - number of rows, used for spritesheets
 * @property {int} one_x - width of one sprite, used for spritesheets
 * @property {int} one_y - height of one sprite, used for spritesheets
 * @property {char*} name - name of the image
 */
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

/**
 * Structure to store data about a directory
 *
 * @typedef {t_directory} Directory
 * @property {uint32_t} lump_offset - offset of the lump
 * @property {uint32_t} lump_size - size of the lump
 * @property {char[9]} name - name of the directory
 */
typedef struct s_directory
{
	uint32_t	lump_offset;
	uint32_t	lump_size;
	char		name[9];
}				t_directory;

/**
 * Structure to store WAD file data
 *
 * @typedef {t_WAD} WAD file data
 * @property {int} fd - file descriptor
 * @property {t_header} header - WAD header
 * @property {t_directory*} dirs - array of directories
 * @property {t_map[9]} maps - array of maps
 *
 * @typedef {t_header} WAD header
 * @property {char[5]} type - WAD type
 * @property {uint32_t} dir_count - number of directories
 * @property {uint32_t} dir_offset - offset of the directories
 */
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

/**
 * Main structure to store all the data
 *
 * @typedef {t_box} Main structure
 * @property {void*} mlx - mlx pointer
 * @property {void*} win - window pointer
 * @property {t_image} image - main image
 * @property {t_image} minimap - minimap image
 * @property {t_image*} textures - array of textures
 * @property {t_WAD} WAD - WAD file data
 * @property {t_map*} map - pointer to the current map
 */
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
void		add_solid_seg_before(t_box *box, t_solid_seg *where, t_solid_seg *what);
void		delete_seg(t_box *box, t_solid_seg *first, t_solid_seg *last);
void		free_solid_segs(t_box *box);
void		print_solid_segs(t_box *box);
int			get_wall_color(t_box *box, char *str);

//Image.c
void		my_mlx_put_image_to_window(t_box *box, t_image *img, int x, int y, int id);
void		my_mlx_pyxel_put(t_image *img, int x, int y, int color);
void		draw_line(t_image *image, int beginX, int beginY, int endX, int endY, int color);
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

//Movement.c
void		calc_move(t_box *box);


#endif
