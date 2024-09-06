#include "doom-nukem.h"

void	my_mlx_pyxel_put(t_image *image, int x, int y, int color)
{
	unsigned char	*pixel;

	pixel = image->addr + (y * image->line_len + x
			* (image->bits_pp / 8));
	*(unsigned int *)pixel = color;
}

void	my_mlx_put_image_to_window(t_box *box, t_image *image, int x, int y, int sprite_id)
{
	if (sprite_id >= 0)
		mlx_put_image_to_window(box->mlx, box->win, image->img, x, y, 0 + (image->one_x * (sprite_id % image->n_col)), 0 + (image->one_y * (sprite_id / image->n_col)), image->one_x, image->one_y);
	else
		mlx_put_image_to_window(box->mlx, box->win, image->img, x, y, 0, 0, image->width, image->height);
}

void	draw_line(t_image *image, int beginX, int beginY, int endX, int endY, int color)
{
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
		my_mlx_pyxel_put(image, pixelX, pixelY, 0xFF << 24 | color);
		pixelX += deltaX;
		pixelY += deltaY;
		--pixels;
	}
}

void	draw_rect(t_box *box, t_rect rect, bool filled)
{
	uint32_t x, y;

	y = rect.y;
	while (y++ < rect.y + rect.height)
	{
		x = rect.x;
		while (x++ < rect.x + rect.width)
			if (!filled)
			{
				if (x == rect.x || x == rect.x + rect.width || y == rect.y || y == rect.y + rect.height)
					my_mlx_pyxel_put(&box->image, x, y, rect.color);
			}
			else
				my_mlx_pyxel_put(&box->image, x, y, rect.color);
	}
}

/*
	new_image

	Creates a new mlx_image_alpha and sets its variables
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

/*
	png_file_to_image

	Tryes to convert png file to image
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

/*
	split_spritesheet

	Used on spritesheet images to split it into sprites with ids
	Needs to know number of images in rows and collums beforehand
	- This step can be done automatically in the future
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
