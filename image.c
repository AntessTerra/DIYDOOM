#include "doom-nukem.h"

/**
 * 	Puts pixel to image
 *
 * 	@param t_image* image
 * 	@param int x
 * 	@param int y
 * 	@param int color
 */
void	my_mlx_pyxel_put(t_image *image, int x, int y, int color)
{
	unsigned char	*pixel;

	pixel = image->addr + (y * image->line_len + x
			* (image->bits_pp / 8));
	*(unsigned int *)pixel = color;
}

/**
 * 	Puts image to window
 *
 * 	@param t_box* box
 * 	@param t_image* image
 * 	@param int x
 * 	@param int y
 * 	@param int sprite_id
 *
 * 	@return If sprite_id is negative, whole image will be put
 */
void	my_mlx_put_image_to_window(t_box *box, t_image *image, int x, int y, int sprite_id)
{
	if (sprite_id >= 0)
		mlx_put_image_to_window(box->mlx, box->win, image->img, x, y, 0 + (image->one_x * (sprite_id % image->n_col)), 0 + (image->one_y * (sprite_id / image->n_col)), image->one_x, image->one_y);
	else
		mlx_put_image_to_window(box->mlx, box->win, image->img, x, y, 0, 0, image->width, image->height);
}

/**
 * 	Draws line on image from begin point to end point
 *
 * 	@param t_image* image
 * 	@param int beginX
 * 	@param int beginY
 * 	@param int endX
 * 	@param int endY
 * 	@param int color
 */
void	draw_line(t_image *image, int beginX, int beginY, int endX, int endY, int color)
{
	if (beginY < 0)
		beginY = 0;
	if (beginY > SCREENHEIGHT)
		beginY = SCREENHEIGHT;
	if (endY < 0)
		endY = 0;
	if (endY > SCREENHEIGHT)
		endY = SCREENHEIGHT;
	double deltaX = endX - beginX;
	double deltaY = endY - beginY;
	int pixels = sqrt((deltaX * deltaX) + (deltaY * deltaY));
	deltaX /= pixels;
	deltaY /= pixels;
	double pixelX = beginX;
	double pixelY = beginY;
	while (pixels)
	{
		// printf("%i | %i\n", (int)pixelX, (int)pixelY);
		my_mlx_pyxel_put(image, pixelX, pixelY, color);
		pixelX += deltaX;
		pixelY += deltaY;
		--pixels;
	}
}

/**
 * 	Creates new image struct
 *
 * 	@param void* mlx
 * 	@param t_image* image
 * 	@param int width
 * 	@param int height
 *
 * 	@return Filled image struct
 */
t_image	*new_image(void *mlx, t_image *image, int width, int height)
{
	image->img = mlx_new_image_alpha(mlx, width, height);
	image->width = width;
	image->height = height;
	image->addr = (unsigned char *)mlx_get_data_addr(image->img,
			&image->bits_pp, &image->line_len, &image->endian);
	return (image);
}

/**
 * 	Resizes image by n_times_bigger
 *
 * 	@param void* mlx_ptr
 * 	@param t_image* src_img
 * 	@param float n_times_bigger
 *
 * 	@return Resized image
 */
t_image	*img_resize(void *mlx_ptr, t_image *src_img, float n_times_bigger)
{
	t_image		dst_img;
	unsigned char *pixel;
	unsigned char *color;
	int			src_x;
	int			src_y;
	int			dst_x;
	int			dst_y;

	if (new_image(mlx_ptr, &dst_img, src_img->width * n_times_bigger, src_img->height * n_times_bigger) == NULL)
		return (NULL);
	dst_y = -1;
	while (dst_y++ < dst_img.height)
	{
		src_y = src_img->height * dst_y / dst_img.height;
		dst_x = -1;
		while (dst_x++ < dst_img.width)
		{
			src_x = src_img->width * dst_x / dst_img.width;
			pixel = &dst_img.addr[dst_y * dst_img.line_len + dst_x * (dst_img.bits_pp / 8)];
			color = &src_img->addr[src_y * src_img->line_len + src_x * (src_img->bits_pp / 8)];
			*(unsigned int *)pixel = *(unsigned int *)color;
		}
	}
	mlx_destroy_image(mlx_ptr, src_img->img);
	*src_img = dst_img;
	return (src_img);
}

/**
 * 	Tryes to convert png file to image struct

 * 	@param void* mlx
 * 	@param t_image* image
 * 	@param char* file

 * 	@return Filled image struct or error message
 */
void	png_file_to_image(void *mlx, t_image *image, char *file)
{
	cp_image_t	png_img;
	cp_pixel_t 	*mlx_img_data;
	int			i;

	png_img = cp_load_png(file);
	if (png_img.pix == NULL)
		return printf("ERROR LOADING IMAGE %s\n", file), (void) 0;
	cp_premultiply(&png_img);
	new_image(mlx, image, png_img.w, png_img.h);
	if (image->img == NULL)
		return (free(png_img.pix), (void) 0);
	mlx_img_data = (cp_pixel_t *)mlx_get_data_addr(image->img, &image->bits_pp, &image->line_len, &image->endian);
	// image->addr = (unsigned char *)mlx_get_data_addr(image->img, &image->bits_pp, &image->line_len, &image->endian);
	i = -1;
	while (i++ < png_img.w * png_img.h)
	{
		mlx_img_data[i].r = png_img.pix[i].b;
		mlx_img_data[i].g = png_img.pix[i].g;
		mlx_img_data[i].b = png_img.pix[i].r;
		mlx_img_data[i].a = png_img.pix[i].a;
	}
	image->width = png_img.w;
	image->height = png_img.h;
	free(png_img.pix);
}



/**
 * Modifies image values for use in accesing parts of it by ids
 *
 * @param t_image* image
 * @param int n_col
 * @param int n_row
 * @param int one_x
 * @param int one_y
 *
 * @return If one_X or one_Y is 0, it will be calculated
 */
void	split_spritesheet(t_image *image, int n_col, int n_row, int one_x, int one_y)
{
	image->n_col = n_col;
	image->n_row = n_row;
	if (one_x == 0)
		image->one_x = image->width / image->n_col;
	else
		image->one_x = one_x;
	if (one_y == 0)
		image->one_y = image->height / image->n_row;
	else
		image->one_y = one_y;
}
