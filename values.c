#include "doom-nukem.h"

/**
 * Inserts a solid segment into a sorted list
 *
 * @param t_solid_seg* node
 * @param t_solid_seg* sorted
 */
static t_solid_seg	*sorted_insert(t_solid_seg *node, t_solid_seg *sorted)
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
 * Sorts the solid segments in a map
 *
 * @param t_box* box
 */
static void	insertion_sort(t_box *box)
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
 * Adds a solid segment before a given segment
 *
 * @param t_box* box
 * @param t_solid_seg* where
 * @param t_solid_seg* what
 */
void	add_solid_seg_before(t_box *box, t_solid_seg *where, t_solid_seg *what)
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
		if (curr->next == where)
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
 * Creates a new solid segment
 *
 * @param int start
 * @param int end
 *
 * @return t_solid_seg* - new solid segment
 */
t_solid_seg	*new_solid_seg(int start, int end)
{
	t_solid_seg	*seg;

	seg = malloc(sizeof(t_solid_seg));
	seg->x_start = start;
	seg->x_end = end;
	seg->next = NULL;
	return (seg);
}

/**
 * Frees all the solid segments stored in a current map
 *
 * @param t_box* box
 */
void	free_solid_segs(t_box *box)
{
	t_solid_seg	*tmp;

	while (box->map->solid_segs)
	{
		tmp = box->map->solid_segs;
		box->map->solid_segs = (box->map->solid_segs)->next;
		free(tmp);
	}
}

/**
 * Deletes a segment from first to !!before!! last
 *
 * @param t_box* box
 * @param t_solid_seg* first
 * @param t_solid_seg* last
 */
void	delete_seg(t_box *box, t_solid_seg *first, t_solid_seg *last)
{
	t_solid_seg	*tmp, *to_rem;

	tmp = box->map->solid_segs;
	while (tmp)
	{
		// printf("TMP: %i,%i\n", tmp->x_start, tmp->x_end);
		if (tmp->next == first)
		{
			to_rem = tmp->next;
			// printf("REMOVING: %i,%i\n", to_rem->x_start, to_rem->x_end);
			tmp->next = to_rem->next;
			free(to_rem);
			while (tmp->next != last)
			{
				to_rem = tmp->next;
				tmp->next = to_rem->next;
				free(to_rem);
			}
		}
		tmp = tmp->next;
	}
}

/**
 * Prints all the solid segments stored in a current map
 *
 * @param t_box* box
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

/**
 * Gets wall color from buffer or generates a new one ans stores it in buffer
 *
 * @param t_box* box
 * @param char* str - name of the texture used as a key
 *
 * @return {int} - color of the wall
 */
int	get_wall_color(t_box *box, char *str)
{
	uint32_t	i, added;

	i = -1;
	added = 0;
	while (++i < MAX_TEXTURE_COLORS && box->map->texture_to_color[i].name[0] && !added)
		if (!ft_strncmp(box->map->texture_to_color[i].name, str, 8))
			added = 1;
	if (i < MAX_TEXTURE_COLORS && !added)
	{
		box->map->texture_to_color[i].color = 0xFF << 24 | rand() % 255 << 16 | rand() % 255 << 8 | rand() % 255;
		ft_strlcpy(box->map->texture_to_color[i].name, str, 8);
		return (box->map->texture_to_color[i].color);
	}
	if (added)
		return (box->map->texture_to_color[i].color);
	return (0);
}

/**
 * Initializes all the values used in the game
 *
 * @param t_box* box
 */
void	init_values(t_box *box)
{
	box->map = &box->WAD.maps[0];
	box->map->player.move_x = 0;
	box->map->player.move_y = 0;
	box->map->player.move_z = 0;
	box->map->player.rotate = 0;
	box->map->solid_segs = NULL;
	add_solid_seg_before(box, NULL, new_solid_seg(SCREENWIDTH, INT_MAX));
	add_solid_seg_before(box, NULL, new_solid_seg(INT_MIN, -1));

	box->map->automap_scale_factor = 20;
	box->map->player.move_speed = 0.5;

	box->map->screen_x_to_angle = malloc((SCREENWIDTH + 1) * sizeof(t_angle));
	box->map->player.dist_to_screen = (SCREENWIDTH / 2) / get_tan_value((t_angle){FOV / 2});
	int i = -1;
	while (++i <= SCREENWIDTH)
		box->map->screen_x_to_angle[i].angle_val = atan(((SCREENWIDTH / 2) - i) / (float)box->map->player.dist_to_screen) * 180 / M_PI;

	box->map->floor_clip_height = malloc((SCREENWIDTH + 1) * sizeof(int));
	i = -1;
	while (++i <= SCREENWIDTH)
		box->map->floor_clip_height[i] = SCREENHEIGHT;

	box->map->ceiling_clip_height = malloc((SCREENWIDTH + 1) * sizeof(int));
	i = -1;
	while (++i <= SCREENWIDTH)
		box->map->ceiling_clip_height[i] = -1;

}

/**
 * Initializes all the textures used in the game
 *
 * @param t_box* box
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

	// print_solid_segs(box);
	// clip_wall(box, 69, 80, (t_angle){0}, (t_angle){0}, (t_seg){0});
	// clip_wall(box, 46, 69, (t_angle){0}, (t_angle){0}, (t_seg){0});
	// clip_wall(box, 195, 210, (t_angle){0}, (t_angle){0}, (t_seg){0});
	// clip_wall(box, 107, 195, (t_angle){0}, (t_angle){0}, (t_seg){0});
	// clip_wall(box, 223, 291, (t_angle){0}, (t_angle){0}, (t_seg){0});
	// clip_wall(box, 76, 107, (t_angle){0}, (t_angle){0}, (t_seg){0});
	// clip_wall(box, 0, 42, (t_angle){0}, (t_angle){0}, (t_seg){0});
	// delete_seg(box, box->map->solid_segs->next, NULL);
	// print_solid_segs(box);
}

