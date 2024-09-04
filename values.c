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

#include "cub3d.h"

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

void	insertion_sort(t_box *box)
{
	t_solid_seg	*curr, *next, *sorted;
	sorted = NULL;
	curr = box->WAD.maps[0].solid_segs;

	while (curr != NULL)
	{
		next = curr->next;

		sorted = sorted_insert(curr, sorted);

		curr = next;
	}
	box->WAD.maps[0].solid_segs = sorted;
}

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

void	add_solid_seg_after(t_box *box, t_solid_seg *what, t_solid_seg *where, int m)
{
	t_solid_seg	*curr, *tmp;

	if (box->WAD.maps[m].solid_segs == NULL)
	{
		box->WAD.maps[m].solid_segs = what;
		return ;
	}
	curr = box->WAD.maps[m].solid_segs;
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

void	free_solid_segs(t_box *box, int m)
{
	t_solid_seg	*tmp;

	while (box->WAD.maps[m].solid_segs)
	{
		tmp = box->WAD.maps[m].solid_segs;
		box->WAD.maps[m].solid_segs = (box->WAD.maps[m].solid_segs)->next;
		free(tmp);
	}
}

void	delete_seg(t_box *box, t_solid_seg *seg, int m)
{
	t_solid_seg	*tmp;

	tmp = box->WAD.maps[m].solid_segs;
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

void	print_solid_segs(t_box *box, int m)
{
	t_solid_seg	*tmp;
	int			i = -1;

	tmp = box->WAD.maps[m].solid_segs;
	while (tmp)
	{
		printf("[%i] %i, %i\n", ++i, tmp->x_start, tmp->x_end);
		tmp = tmp->next;
	}
	printf("\n");
}

void	clip_wall(t_box *box, t_solid_seg currWall, int m)
{
	t_solid_seg	*foundWall, *nextWall;

	foundWall = box->WAD.maps[m].solid_segs;

	while (foundWall->next && foundWall->x_end < currWall.x_start - 1)
		foundWall = foundWall->next;
	// printf("%i, %i\n", currWall.x_start, currWall.x_end);
	if (currWall.x_start < foundWall->x_start)
	{
		// printf("TEST\n");
		if (currWall.x_end < foundWall->x_start - 1)
		{
			draw_rect(box, (t_rect){currWall.x_start, 0, currWall.x_end - currWall.x_start, 200, 0xFF00FF00}, false);
			t_solid_seg *p_curr_wall = new_solid_seg(currWall.x_start, currWall.x_end, currWall.color);
			add_solid_seg_after(box, p_curr_wall, p_curr_wall, 0);
			return ;
		}
		draw_rect(box, (t_rect){currWall.x_start, 0, currWall.x_end - currWall.x_start, 300, 0xFF0000FF}, false);
		foundWall->x_start = currWall.x_start;
		return ;
	}
	// printf("%i <= %i\n", currWall.x_end, foundWall->x_end);
	if (currWall.x_end <= foundWall->x_end)
		return ;

	nextWall = foundWall;
	// printf("%i >= %i\n", currWall.x_end, nextWall->next->x_start - 1);
	while (currWall.x_end >= nextWall->next->x_start - 1)
	{
		draw_rect(box, (t_rect){currWall.x_start, 0, currWall.x_end - currWall.x_start, 400, 0xFFFF0000}, false);
		nextWall = nextWall->next;
		if (currWall.x_end <= nextWall->x_end)
		{
			foundWall->x_end = nextWall->x_end;
			if (nextWall != foundWall)
			{
				foundWall = foundWall->next;
				nextWall = nextWall->next;
				// printf("Deleting %i, %i\n", foundWall->x_start, foundWall->x_end);
				delete_seg(box, foundWall, m);
			}
			return ;
		}
	}
	// printf("%i, %i | %i, %i\n", nextWall->x_start, nextWall->x_end, foundWall->x_start, foundWall->x_end);
	draw_rect(box, (t_rect){currWall.x_start, 0, currWall.x_end - currWall.x_start, 500, 0xFFFFFF00}, false);
	foundWall->x_end = currWall.x_end;
	if (nextWall != foundWall)
	{
		foundWall = foundWall->next;
		nextWall = nextWall->next;
		delete_seg(box, foundWall, m);
	}
}

void	init_textures(t_box *box)
{
	int		i;

	box->textures = malloc(100 * sizeof(t_image));
	i = -1;
	while (++i < 100)
		box->textures[i].img = NULL;
	png_file_to_image(box->mlx, &box->textures[UI_PICKUPS], "textures/ui_pickups.png");
	split_spritesheet(&box->textures[UI_PICKUPS], 8, 5, 16, 16);

	box->WAD.maps[0].solid_segs = NULL;
	add_solid_seg_after(box, new_solid_seg(SCREENWIDTH, INT_MAX, 0), NULL, 0);
	add_solid_seg_after(box, new_solid_seg(INT_MIN, -1, 0), NULL, 0);
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

