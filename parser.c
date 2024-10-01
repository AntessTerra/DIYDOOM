/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: phelebra <xhelp00@gmail.com>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/08 18:52:55 by jbartosi          #+#    #+#             */
/*   Updated: 2023/10/16 16:04:56 by phelebra         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "doom-nukem.h"

void	print_WAD(t_box *box)
{
	uint32_t	i;

	printf("\nPRINTING DIRECTORIES\n\n");
	i = -1;
	while (++i < 50)
		printf("%i | %i | %s\n", box->WAD.dirs[i].lump_offset, box->WAD.dirs[i].lump_size, box->WAD.dirs[i].name);

	// uint32_t	m;
	// printf("\nPRINTING MAPS\n\n");
	// m = -1;
	// while (++m < 1)
	// {
	// 	printf("%s\n", box->WAD.maps[m].name);
	// 	// i = -1;
	// 	// while (++i < 5)
	// 	// {
	// 	// 	printf("Vertex %i | %i \n", box->WAD.maps[m].vertexes[i].x, box->WAD.maps[m].vertexes[i].y);
	// 	// 	printf("Linedef %i | %i \n", box->WAD.maps[m].linedef[i].start_vertex, box->WAD.maps[m].linedef[i].end_vertex);
	// 	// 	printf("Things %i | %i | %i | %i | %i\n", box->WAD.maps[m].things[i].x, box->WAD.maps[m].things[i].y, box->WAD.maps[m].things[i].angle, box->WAD.maps[m].things[i].type, box->WAD.maps[m].things[i].flags);
	// 	// 	printf("Nodes %i | %i | %i | %i\n", box->WAD.maps[m].nodes[i].change_partition_x, box->WAD.maps[m].nodes[i].change_partition_y, box->WAD.maps[m].nodes[i].partition_x, box->WAD.maps[m].nodes[i].partition_y);
	// 	// 	printf("Segs %i | %i | %i | %i\n", box->WAD.maps[m].segs[i].start_vertex_id, box->WAD.maps[m].segs[i].end_vertex_id, box->WAD.maps[m].segs[i].angle, box->WAD.maps[m].segs[i].linedef_id);
	// 	// 	printf("Ssector %i | %i\n", box->WAD.maps[m].ssectors[i].seg_count, box->WAD.maps[m].ssectors[i].first_seg_id);
	// 	// 	printf("Sidedefs %i | %s | %s\n", box->WAD.maps[m].sidedefs[i].sector_id, box->WAD.maps[m].sidedefs[i].upper_texture, box->WAD.maps[m].sidedefs[i].lower_texture);
	// 	// 	printf("Sector %i | %i | %i\n\n", box->WAD.maps[m].sectors[i].floor_height, box->WAD.maps[m].sectors[i].ceiling_height, box->WAD.maps[m].sectors[i].light_level);
	// 	// }
	// }
}

static int	find_min_max_vertexes(t_box *box, uint32_t m)
{
	uint32_t	v;

	box->WAD.maps[m].min_x = 0;
	box->WAD.maps[m].max_x = 0;
	box->WAD.maps[m].min_y = 0;
	box->WAD.maps[m].max_y = 0;
	v = -1;
	while (++v < box->WAD.maps[m].n_vertexes)
	{
		if (box->WAD.maps[m].vertexes[v].x < box->WAD.maps[m].min_x)
			box->WAD.maps[m].min_x = box->WAD.maps[m].vertexes[v].x;
		else if (box->WAD.maps[m].vertexes[v].x > box->WAD.maps[m].max_x)
			box->WAD.maps[m].max_x = box->WAD.maps[m].vertexes[v].x;
		if (box->WAD.maps[m].vertexes[v].y < box->WAD.maps[m].min_y)
			box->WAD.maps[m].min_y = box->WAD.maps[m].vertexes[v].y;
		else if (box->WAD.maps[m].vertexes[v].y > box->WAD.maps[m].max_y)
			box->WAD.maps[m].max_y = box->WAD.maps[m].vertexes[v].y;
	}
	return (0);
}

static int	parse_vertexes(t_box *box, uint32_t i, uint32_t m)
{
	box->WAD.maps[m].n_vertexes = box->WAD.dirs[i].lump_size / sizeof(struct s_vertex);
	box->WAD.maps[m].vertexes = malloc(box->WAD.dirs[i].lump_size);
	if (!box->WAD.maps[m].vertexes)
		exit_hook(box, "ERROR WHILE MALLOCING VERTEXES\n");
	lseek(box->WAD.fd, box->WAD.dirs[i].lump_offset, SEEK_SET);
	if (read(box->WAD.fd, (void *)&box->WAD.maps[m].vertexes[0], box->WAD.dirs[i].lump_size) < 0)
		exit_hook(box, "ERROR WHILE READING VERTEXES\n");
	find_min_max_vertexes(box, m);
	return (0);
}

static int	parse_things(t_box *box, uint32_t i, uint32_t m)
{
	box->WAD.maps[m].n_things = box->WAD.dirs[i].lump_size / sizeof(struct s_WAD_thing);
	box->WAD.maps[m].WAD_things = malloc(box->WAD.dirs[i].lump_size);
	if (!box->WAD.maps[m].WAD_things)
		exit_hook(box, "ERROR WHILE MALLOCING WAD THINGS\n");
	lseek(box->WAD.fd, box->WAD.dirs[i].lump_offset, SEEK_SET);
	if (read(box->WAD.fd, (void *)&box->WAD.maps[m].WAD_things[0], box->WAD.dirs[i].lump_size) < 0)
		exit_hook(box, "ERROR WHILE READING WAD THINGS\n");
	// Init player
	box->WAD.maps[m].player.x = box->WAD.maps[m].WAD_things[0].x;
	box->WAD.maps[m].player.y = box->WAD.maps[m].WAD_things[0].y;
	box->WAD.maps[m].player.z = 41;
	box->WAD.maps[m].player.angle.angle_val = box->WAD.maps[m].WAD_things[0].angle;
	return (0);
}

static int	parse_linedefs(t_box *box, uint32_t i, uint16_t m)
{
	box->WAD.maps[m].n_linedefs = box->WAD.dirs[i].lump_size / sizeof(struct s_WAD_linedef);
	box->WAD.maps[m].WAD_linedefs = malloc(box->WAD.dirs[i].lump_size);
	if (!box->WAD.maps[m].WAD_linedefs)
		exit_hook(box, "ERROR WHILE MALLOCING WAD LINEDEFS\n");
	lseek(box->WAD.fd, box->WAD.dirs[i].lump_offset, SEEK_SET);
	if (read(box->WAD.fd, (void *)&box->WAD.maps[m].WAD_linedefs[0], box->WAD.dirs[i].lump_size) < 0)
		exit_hook(box, "ERROR WHILE READING WAD LINEDEFS\n");
	return (0);
}

static int	parse_nodes(t_box *box, uint32_t i, uint32_t m)
{
	box->WAD.maps[m].n_nodes = box->WAD.dirs[i].lump_size / sizeof(struct s_WAD_node);
	box->WAD.maps[m].WAD_nodes = malloc(box->WAD.dirs[i].lump_size);
	if (!box->WAD.maps[m].WAD_nodes)
		exit_hook(box, "ERROR WHILE MALLOCING WAD NODES\n");
	lseek(box->WAD.fd, box->WAD.dirs[i].lump_offset, SEEK_SET);
	if (read(box->WAD.fd, (void *)&box->WAD.maps[m].WAD_nodes[0], box->WAD.dirs[i].lump_size) < 0)
		exit_hook(box, "ERROR WHILE READING WAD NODES\n");
	return (0);
}

static int	parse_segs(t_box *box, uint32_t i, uint32_t m)
{
	box->WAD.maps[m].n_segs = box->WAD.dirs[i].lump_size / sizeof(struct s_WAD_seg);
	box->WAD.maps[m].WAD_segs = malloc(box->WAD.dirs[i].lump_size);
	if (!box->WAD.maps[m].WAD_segs)
		exit_hook(box, "ERROR WHILE MALLOCING WAD SEGS\n");
	lseek(box->WAD.fd, box->WAD.dirs[i].lump_offset, SEEK_SET);
	if (read(box->WAD.fd, (void *)&box->WAD.maps[m].WAD_segs[0], box->WAD.dirs[i].lump_size) < 0)
		exit_hook(box, "ERROR WHILE READING WAD SEGS\n");
	return (0);
}

static int	parse_ssectors(t_box *box, uint32_t i, uint32_t m)
{
	box->WAD.maps[m].n_ssectors = box->WAD.dirs[i].lump_size / sizeof(struct s_WAD_ssector);
	box->WAD.maps[m].WAD_ssectors = malloc(box->WAD.dirs[i].lump_size);
	if (!box->WAD.maps[m].WAD_ssectors)
		exit_hook(box, "ERROR WHILE MALLOCING WAD SSECTORS\n");
	lseek(box->WAD.fd, box->WAD.dirs[i].lump_offset, SEEK_SET);
	if (read(box->WAD.fd, (void *)&box->WAD.maps[m].WAD_ssectors[0], box->WAD.dirs[i].lump_size) < 0)
		exit_hook(box, "ERROR WHILE READING WAD SSECTORS\n");
	return (0);
}

static int	parse_sidedefs(t_box *box, uint32_t i, uint32_t m)
{
	box->WAD.maps[m].n_sidedefs = box->WAD.dirs[i].lump_size / sizeof(struct s_WAD_sidedef);
	box->WAD.maps[m].WAD_sidedefs = malloc(box->WAD.dirs[i].lump_size);
	if (!box->WAD.maps[m].WAD_sidedefs)
		exit_hook(box, "ERROR WHILE MALLOCING WAD SIDEDEFS\n");
	lseek(box->WAD.fd, box->WAD.dirs[i].lump_offset, SEEK_SET);
	if (read(box->WAD.fd, (void *)&box->WAD.maps[m].WAD_sidedefs[0], box->WAD.dirs[i].lump_size) < 0)
		exit_hook(box, "ERROR WHILE READING WAD SIDEDEFS\n");
	return (0);
}

static int	parse_sectors(t_box *box, uint32_t i, uint32_t m)
{
	box->WAD.maps[m].n_sectors = box->WAD.dirs[i].lump_size / sizeof(struct s_WAD_sector);
	box->WAD.maps[m].WAD_sectors = malloc(box->WAD.dirs[i].lump_size);
	if (!box->WAD.maps[m].WAD_sectors)
		exit_hook(box, "ERROR WHILE MALLOCING WAD SECTORS\n");
	lseek(box->WAD.fd, box->WAD.dirs[i].lump_offset, SEEK_SET);
	if (read(box->WAD.fd, (void *)&box->WAD.maps[m].WAD_sectors[0], box->WAD.dirs[i].lump_size) < 0)
		exit_hook(box, "ERROR WHILE READING WAD SECTORS\n");
	return (0);
}

static int	build_sidedefs(t_box *box, uint32_t m)
{
	uint32_t		i;
	t_sidedef		*sidedef;
	t_WAD_sidedef	*WAD_sidedef;

	i = -1;
	while (++i < MAX_TEXTURE_COLORS)
	{
		box->WAD.maps[m].texture_to_color[i].color = 0;
		ft_memset(box->WAD.maps[m].texture_to_color[i].name, 0, 8);
	}
	i = -1;
	box->WAD.maps[m].sidedefs = malloc(box->WAD.maps[m].n_sidedefs * sizeof(struct s_sidedef));
	if (!box->WAD.maps[m].sidedefs)
		exit_hook(box, "ERROR WHILE MALLOCING SIDEDEFS\n");
	while (++i < box->WAD.maps[m].n_sidedefs)
	{
		sidedef = &box->WAD.maps[m].sidedefs[i];
		WAD_sidedef = &box->WAD.maps[m].WAD_sidedefs[i];

		sidedef->x_offset = WAD_sidedef->x_offset;
		sidedef->y_offset = WAD_sidedef->y_offset;
		ft_strlcpy(sidedef->upper_texture, WAD_sidedef->upper_texture, 8);
		ft_strlcpy(sidedef->lower_texture, WAD_sidedef->lower_texture, 8);
		ft_strlcpy(sidedef->middle_texture, WAD_sidedef->middle_texture, 8);
		sidedef->p_sector = &box->WAD.maps[m].WAD_sectors[WAD_sidedef->sector_id];
	}
	return (0);
}

static int	build_linedefs(t_box *box, uint32_t m)
{
	uint32_t		i;
	t_linedef		*linedef;
	t_WAD_linedef	*WAD_linedef;

	i = -1;
	box->WAD.maps[m].linedefs = malloc(box->WAD.maps[m].n_linedefs * sizeof(struct s_linedef));
	if (!box->WAD.maps[m].linedefs)
		exit_hook(box, "ERROR WHILE MALLOCING LINEDEFS\n");
	while (++i < box->WAD.maps[m].n_linedefs)
	{
		linedef = &box->WAD.maps[m].linedefs[i];
		WAD_linedef = &box->WAD.maps[m].WAD_linedefs[i];

		linedef->p_start_vertex = &box->WAD.maps[m].vertexes[WAD_linedef->start_vertex_id];
		linedef->p_end_vertex = &box->WAD.maps[m].vertexes[WAD_linedef->end_vertex_id];
		linedef->flags = WAD_linedef->flags;
		linedef->line_type = WAD_linedef->line_type;
		linedef->sector_tag = WAD_linedef->sector_tag;

		linedef->p_right_sidedef = NULL;
		linedef->p_left_sidedef = NULL;
		if (WAD_linedef->right_sidedef != 0xFFFF)
			linedef->p_right_sidedef = &box->WAD.maps[m].sidedefs[WAD_linedef->right_sidedef];
		if (WAD_linedef->left_sidedef != 0xFFFF)
			linedef->p_left_sidedef = &box->WAD.maps[m].sidedefs[WAD_linedef->left_sidedef];
	}
	return (0);
}

static int	build_segs(t_box *box, uint32_t m)
{
	uint32_t	i;
	t_seg		*seg;
	t_WAD_seg	*WAD_seg;

	i = -1;
	box->WAD.maps[m].segs = malloc(box->WAD.maps[m].n_segs * sizeof(struct s_seg));
	if (!box->WAD.maps[m].segs)
		exit_hook(box, "ERROR WHILE MALLOCING SEGS\n");
	while (++i < box->WAD.maps[m].n_segs)
	{
		seg = &box->WAD.maps[m].segs[i];
		WAD_seg = &box->WAD.maps[m].WAD_segs[i];

		seg->p_start_vertex = &box->WAD.maps[m].vertexes[WAD_seg->start_vertex_id];
		seg->p_end_vertex = &box->WAD.maps[m].vertexes[WAD_seg->end_vertex_id];
		seg->angle.angle_val = ((float)(WAD_seg->angle << 16) * 8.38190317e-8);
		seg->p_linedef = &box->WAD.maps[m].linedefs[WAD_seg->linedef_id];
		seg->direction = WAD_seg->direction;
		seg->offset = (float)(WAD_seg->offset << 16) / (float)(1 << 16);

		t_sidedef	*p_right_sidedef;
		t_sidedef	*p_left_sidedef;

		if (seg->direction)
		{
			p_right_sidedef = seg->p_linedef->p_left_sidedef;
			p_left_sidedef = seg->p_linedef->p_right_sidedef;
		}
		else
		{
			p_right_sidedef = seg->p_linedef->p_right_sidedef;
			p_left_sidedef = seg->p_linedef->p_left_sidedef;
		}

		seg->p_right_sector = NULL;
		seg->p_left_sector = NULL;
		if (p_right_sidedef)
			seg->p_right_sector = p_right_sidedef->p_sector;
		if (p_left_sidedef)
			seg->p_left_sector = p_left_sidedef->p_sector;
	}
	return (0);
}

static int	parse_maps(t_box *box)
{
	uint32_t	i, m;

	i = -1;
	m = -1;
	while (++i < box->WAD.header.dir_count)
	{
		// if (m == 1)
		// 	break ;
		if (!ft_strncmp(box->WAD.dirs[i].name, "E1M", 3))
		{
			ft_memcpy(box->WAD.maps[++m].name, box->WAD.dirs[i].name, 5);
			parse_things(box, ++i, m);
			parse_linedefs(box, ++i, m);
			parse_sidedefs(box, ++i, m);
			parse_vertexes(box, ++i, m);
			parse_segs(box, ++i, m);
			parse_ssectors(box, ++i, m);
			parse_nodes(box, ++i, m);
			parse_sectors(box, ++i, m);
			build_sidedefs(box, m);
			build_linedefs(box, m);
			build_segs(box, m);
		}
	}
	return (0);
}

/**
 * Parses the WAD file
 *
 * @param t_box* box
 */
int	parser(t_box *box)
{
	uint32_t	i;

	box->WAD.fd = 0;
	box->WAD.fd = open("DOOM.WAD", O_RDONLY);
	if (!box->WAD.fd)
		exit_hook(box, "ERROR WHILE OPENING WAD FILE\n");

	ft_memset(box->WAD.header.type, 0 , 5);
	if (read(box->WAD.fd, box->WAD.header.type, 4) < 0)
		exit_hook(box, "ERROR WHILE READING WAD HEADER\n");
	if (read(box->WAD.fd, (void *)&box->WAD.header.dir_count, 4) < 0)
		exit_hook(box, "ERROR WHILE READING WAD HEADER\n");
	if (read(box->WAD.fd, (void *)&box->WAD.header.dir_offset, 4) < 0)
		exit_hook(box, "ERROR WHILE READING WAD HEADER\n");

	// printf("%s | %i | %i\n", box->WAD.header.type, box->WAD.header.dir_count, box->WAD.header.dir_offset);

	lseek(box->WAD.fd, box->WAD.header.dir_offset, SEEK_SET);

	box->WAD.dirs = NULL;
	box->WAD.dirs = malloc(box->WAD.header.dir_count * sizeof(struct s_directory));
	if (!box->WAD.dirs)
		exit_hook(box, "ERROR WHILE ALLOCATING MEMORY FOR DIRECTORIES\n");

	i = -1;
	while (++i < box->WAD.header.dir_count)
	{
		ft_memset(box->WAD.dirs[i].name, 0 , 9);
		if (read(box->WAD.fd, (void *)&box->WAD.dirs[i], 16) < 0)
			exit_hook(box, "ERROR WHILE READING DIRECTORIES\n");
	}
	parse_maps(box);

	// print_WAD(box);

	close(box->WAD.fd);
	// box->WAD.maps[0].player.angle.angle_val = 270;
	return (0);
}
