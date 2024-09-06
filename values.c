/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   values.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phelebra <xhelp00@gmail.com>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/08 18:52:55 by jbartosi          #+#    #+#             */
/*   Updated: 2023/10/16 16:04:56 by phelebra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

/**
 * sorted_insert()
 * ---------------
 *
 * Inserts a solid segment in a sorted linked list
 *
 * param: t_solid_seg *node
 * param: t_solid_seg *sorted
 *
 * return: t_solid_seg *
 */
t_solid_seg	*sorted_insert(t_solid_seg *node, t_solid_seg *sorted)
{
	if (sorted == NULL || sorted->x_start >= node->x_start)
	{
		node->next = sorted;
		sorted = node;
	}
	else
	{
		t_solid_seg	*curr = sorted;

		while (curr->next != NULL && curr->next->x_start < node->x_start)
			curr = curr->next;
		node->next = curr->next;
		curr->next = node;
	}
	return (sorted);
}

/**
 * insertion_sort()
 * ----------------
 *
 * Sorts the solid segments in a current map
 *
 * param: t_box *box
 *
 * return: 0
 */
void	insertion_sort(t_box *box)
{
	t_solid_seg	*curr, *next, *sorted;
	sorted = NULL;
	curr = box->map->solid_segs;

	while (curr != NULL)
	{
		next = curr->next;

		sorted = sorted_insert(curr, sorted);

		curr = next;
	}
	box->map->solid_segs = sorted;
}

/**
 * new_solid_seg()
 * ---------------
 *
 * Creates a new solid segment
 *
 * param: int start
 * param: int end
 * param: int color
 *
 * return: t_solid_seg *
 */
t_solid_seg	*new_solid_seg(int start, int end, int color)
{
	t_solid_seg	*seg;

	seg = malloc(sizeof(t_solid_seg));
	seg->x_start = start;
	seg->x_end = end;
	seg->color = color;
	seg->next = NULL;
	return (seg);
}

/**
 * add_solid_seg_after()
 * ---------------------
 *
 * Adds a solid segment after a given segment
 *
 * param: t_box *box
 * param: t_solid_seg *what
 * param: t_solid_seg *where
 *
 * return: 0
 */
void	add_solid_seg_after(t_box *box, t_solid_seg *what, t_solid_seg *where)
{
	t_solid_seg	*curr, *tmp;

	if (box->map->solid_segs == NULL)
	{
		box->map->solid_segs = what;
		return ;
	}
	curr = box->map->solid_segs;
	while (curr->next)
	{
		if (curr == where)
		{
			tmp = curr->next;
			curr->next = what;
			what->next = tmp;
			return;
		}
		curr = curr->next;
	}
	curr->next = what;
	insertion_sort(box);
}

/**
 * free_solid_segs()
 * -----------------
 *
 * Frees all the solid segments stored in a current map
 *
 * param: t_box *box
 *
 * return: 0
 */
void	free_solid_segs(t_box *box)
{
	t_solid_seg	*tmp;
	// int			m;

	// m = -1;
	// while (++m < N_MAPS)
	// {
		while (box->map->solid_segs)
		{
			tmp = box->map->solid_segs;
			box->map->solid_segs = (box->map->solid_segs)->next;
			free(tmp);
		}
	// }
}

/**
 * delete_seg()
 * ------------------
 *
 * Prints all segments stored in a current map
 *
 * param: t_box *box
 * param: t_solid_seg *seg

 *
 * return: 0
 */
void	delete_seg(t_box *box, t_solid_seg *seg)
{
	t_solid_seg	*tmp;

	tmp = box->map->solid_segs;
	while (tmp)
	{
		if (tmp->next == seg)
		{
			tmp->next = seg->next;
			free(seg);
			return ;
		}
		tmp = tmp->next;
	}
}

/**
 * print_solid_segs()
 * ------------------
 *
 * Prints all segments stored in a current map
 *
 * param: t_box *box
 *
 * return: 0
 */
void	print_solid_segs(t_box *box)
{
	t_solid_seg	*tmp;
	int			i = -1;

	tmp = box->map->solid_segs;
	while (tmp)
	{
		printf("[%i] %i, %i\n", ++i, tmp->x_start, tmp->x_end);
		tmp = tmp->next;
	}
	printf("\n");
}

float	distance_to_point(t_box * box, t_vertex *ver)
{
	// distance = square root ((X2 - X1)^2 + (y2 - y1)^2)
	return (sqrt(pow(box->map->player.x - ver->x, 2) + pow(box->map->player.y - ver->y, 2)));
}

/**
 * init_values()
 * -------------
 *
 * Inits all the values used
 *
 * param: t_box *box
 *
 * return: 0
 */
void	init_values(t_box *box)
{
	box->map = &box->WAD.maps[0];
	box->map->solid_segs = NULL;
	add_solid_seg_after(box, new_solid_seg(SCREENWIDTH, INT_MAX, 0), NULL);
	add_solid_seg_after(box, new_solid_seg(INT_MIN, -1, 0), NULL);

	box->map->automap_scale_factor = 20;
	box->map->player.move_speed = 10;

	box->map->screen_x_to_angle = malloc((SCREENWIDTH + 1) * sizeof(t_angle));
	t_angle	screen_angle = new_angle(FOV / 2.0f);
	float	f_step = (float)FOV / (float)(SCREENWIDTH + 1);
	int i = -1;
	while (++i <= SCREENWIDTH)
	{
		box->map->screen_x_to_angle[i] = screen_angle;
		screen_angle.angle_val -= f_step;
	}
}

/**
 * init_textures()
 * ---------------
 *
 * Inits all the textures used
 *
 * param: t_box *box
 *
 * return: 0
 */
void	init_textures(t_box *box)
{
	int		i;

	box->textures = malloc(MAX_TEXTURES * sizeof(t_image));
	i = -1;
	while (++i < MAX_TEXTURES)
		box->textures[i].img = NULL;
	png_file_to_image(box->mlx, &box->textures[UI_PICKUPS], "textures/ui_pickups.png");
	split_spritesheet(&box->textures[UI_PICKUPS], 8, 5, 16, 16);

	// clip_wall(box, new_solid_seg(69, 80, 0), 0);
	// clip_wall(box, new_solid_seg(46, 69, 0), 0);
	// clip_wall(box, new_solid_seg(195, 210, 0), 0);
	// clip_wall(box, new_solid_seg(107, 195, 0), 0);
	// clip_wall(box, new_solid_seg(223, 291, 0), 0);
	// clip_wall(box, new_solid_seg(76, 107, 0), 0);
	// clip_wall(box, new_solid_seg(0, 42, 0), 0);
	// print_solid_segs(box, 0);
	// delete_seg(box, box->WAD.maps[0].solid_segs->next, 0);
	// print_solid_segs(box, 0);
}

