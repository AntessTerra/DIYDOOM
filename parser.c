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
	lseek(box->WAD.fd, box->WAD.dirs[i].lump_offset, SEEK_SET);
	read(box->WAD.fd, (void *)&box->WAD.maps[m].vertexes[0], box->WAD.dirs[i].lump_size);
	find_min_max_vertexes(box, m);
	return (0);
}

static int	parse_things(t_box *box, uint32_t i, uint32_t m)
{
	box->WAD.maps[m].n_things = box->WAD.dirs[i].lump_size / sizeof(struct s_thing);
	box->WAD.maps[m].things = malloc(box->WAD.dirs[i].lump_size);
	lseek(box->WAD.fd, box->WAD.dirs[i].lump_offset, SEEK_SET);
	read(box->WAD.fd, (void *)&box->WAD.maps[m].things[0], box->WAD.dirs[i].lump_size);
	box->WAD.maps[m].player.x = box->WAD.maps[m].things[0].x;
	box->WAD.maps[m].player.y = box->WAD.maps[m].things[0].y;
	box->WAD.maps[m].player.angle.angle_val = box->WAD.maps[m].things[0].angle;
	return (0);
}

static int	parse_linedefs(t_box *box, uint32_t i, uint16_t m)
{
	box->WAD.maps[m].n_linedefs = box->WAD.dirs[i].lump_size / sizeof(struct s_linedef);
	box->WAD.maps[m].linedef = malloc(box->WAD.dirs[i].lump_size);
	lseek(box->WAD.fd, box->WAD.dirs[i].lump_offset, SEEK_SET);
	read(box->WAD.fd, (void *)&box->WAD.maps[m].linedef[0], box->WAD.dirs[i].lump_size);
	return (0);
}

static int	parse_nodes(t_box *box, uint32_t i, uint32_t m)
{
	box->WAD.maps[m].n_nodes = box->WAD.dirs[i].lump_size / sizeof(struct s_node);
	box->WAD.maps[m].nodes = malloc(box->WAD.dirs[i].lump_size);
	lseek(box->WAD.fd, box->WAD.dirs[i].lump_offset, SEEK_SET);
	read(box->WAD.fd, (void *)&box->WAD.maps[m].nodes[0], box->WAD.dirs[i].lump_size);
	return (0);
}

static int	parse_segs(t_box *box, uint32_t i, uint32_t m)
{
	box->WAD.maps[m].n_segs = box->WAD.dirs[i].lump_size / sizeof(struct s_seg);
	box->WAD.maps[m].segs = malloc(box->WAD.dirs[i].lump_size);
	lseek(box->WAD.fd, box->WAD.dirs[i].lump_offset, SEEK_SET);
	read(box->WAD.fd, (void *)&box->WAD.maps[m].segs[0], box->WAD.dirs[i].lump_size);
	return (0);
}

static int	parse_ssectors(t_box *box, uint32_t i, uint32_t m)
{
	box->WAD.maps[m].n_ssectors = box->WAD.dirs[i].lump_size / sizeof(struct s_ssector);
	box->WAD.maps[m].ssectors = malloc(box->WAD.dirs[i].lump_size);
	lseek(box->WAD.fd, box->WAD.dirs[i].lump_offset, SEEK_SET);
	read(box->WAD.fd, (void *)&box->WAD.maps[m].ssectors[0], box->WAD.dirs[i].lump_size);
	return (0);
}

static int	parse_sidedefs(t_box *box, uint32_t i, uint32_t m)
{
	box->WAD.maps[m].n_sidedefs = box->WAD.dirs[i].lump_size / sizeof(struct s_sidedef);
	box->WAD.maps[m].sidedefs = malloc(box->WAD.dirs[i].lump_size);
	lseek(box->WAD.fd, box->WAD.dirs[i].lump_offset, SEEK_SET);
	read(box->WAD.fd, (void *)&box->WAD.maps[m].sidedefs[0], box->WAD.dirs[i].lump_size);
	return (0);
}

static int	parse_sectors(t_box *box, uint32_t i, uint32_t m)
{
	box->WAD.maps[m].n_sectors = box->WAD.dirs[i].lump_size / sizeof(struct s_sector);
	box->WAD.maps[m].sectors = malloc(box->WAD.dirs[i].lump_size);
	lseek(box->WAD.fd, box->WAD.dirs[i].lump_offset, SEEK_SET);
	read(box->WAD.fd, (void *)&box->WAD.maps[m].sectors[0], box->WAD.dirs[i].lump_size);
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
		}
	}
	return (0);
}

/*
	parser

	Creates a new mlx_image_alpha and sets its variabasdles
*/
int	parser(t_box *box)
{
	uint32_t	i;

	box->WAD.fd = open("DOOM.WAD", O_RDONLY);
	if (!box->WAD.fd)
		return (printf("ERROR WHILE OPENING WAD FILE\n"), 1);

	ft_memset(box->WAD.header.type, 0 , 5);
	read(box->WAD.fd, box->WAD.header.type, 4);
	read(box->WAD.fd, (void *)&box->WAD.header.dir_count, 4);
	read(box->WAD.fd, (void *)&box->WAD.header.dir_offset, 4);

	// printf("%s | %i | %i\n", box->WAD.header.type, box->WAD.header.dir_count, box->WAD.header.dir_offset);

	lseek(box->WAD.fd, box->WAD.header.dir_offset, SEEK_SET);

	box->WAD.dirs = malloc(box->WAD.header.dir_count * sizeof(struct s_directory));

	i = -1;
	while (++i < box->WAD.header.dir_count)
	{
		ft_memset(box->WAD.dirs[i].name, 0 , 9);
		read(box->WAD.fd, (void *)&box->WAD.dirs[i], 16);
	}
	parse_maps(box);

	// print_WAD(box);

	close(box->WAD.fd);
	// box->WAD.maps[0].player.angle.angle_val = 270;
	return (0);
}
