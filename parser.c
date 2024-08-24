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

#include "cub3d.h"

int	parse_linedefs(t_box *box, uint32_t i, uint16_t m)
{
	uint32_t	l;

	box->WAD.maps[m].n_linedefs = box->WAD.dirs[i].lump_size / sizeof(struct s_linedef);
	box->WAD.maps[m].linedef = malloc(box->WAD.dirs[i].lump_size);
	lseek(box->WAD.fd, box->WAD.dirs[i].lump_offset, SEEK_SET);
	l = -1;
	while (++l < box->WAD.maps[m].n_linedefs)
	{
		read(box->WAD.fd, (void *)&box->WAD.maps[m].linedef[l].start_vertex, 2);
		read(box->WAD.fd, (void *)&box->WAD.maps[m].linedef[l].end_vertex, 2);
		read(box->WAD.fd, (void *)&box->WAD.maps[m].linedef[l].flags, 2);
		read(box->WAD.fd, (void *)&box->WAD.maps[m].linedef[l].line_type, 2);
		read(box->WAD.fd, (void *)&box->WAD.maps[m].linedef[l].sector_tag, 2);
		read(box->WAD.fd, (void *)&box->WAD.maps[m].linedef[l].right_sidedef, 2);
		read(box->WAD.fd, (void *)&box->WAD.maps[m].linedef[l].left_sidedef, 2);
	}
	return (0);
}

int	parse_vertexes(t_box *box, uint32_t i, uint32_t m)
{
	uint32_t	v;

	box->WAD.maps[m].n_vertexes = box->WAD.dirs[i].lump_size / sizeof(struct s_vertex);
	box->WAD.maps[m].vertexes = malloc(box->WAD.dirs[i].lump_size);
	lseek(box->WAD.fd, box->WAD.dirs[i].lump_offset, SEEK_SET);
	box->WAD.maps[m].min_x = 0;
	box->WAD.maps[m].max_x = 0;
	box->WAD.maps[m].min_y = 0;
	box->WAD.maps[m].max_y = 0;
	v = -1;
	while (++v < box->WAD.maps[m].n_vertexes)
	{
		read(box->WAD.fd, (void *)&box->WAD.maps[m].vertexes[v].x, 2);
		read(box->WAD.fd, (void *)&box->WAD.maps[m].vertexes[v].y, 2);
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

int	parse_maps(t_box *box)
{
	uint32_t	i, m;

	i = -1;
	m = -1;
	while (++i < box->WAD.header.dir_count)
	{
		if (!ft_strncmp(box->WAD.dirs[i].name, "E1M", 3))
			ft_memcpy(box->WAD.maps[++m].name, box->WAD.dirs[i].name, 5);
		else if (!ft_strncmp(box->WAD.dirs[i].name, "LINEDEFS", 9))
			parse_linedefs(box, i, m);
		else if (!ft_strncmp(box->WAD.dirs[i].name, "VERTEXES", 9))
			parse_vertexes(box, i, m);
	}
	return (0);
}

int	parser(t_box *box)
{
	uint32_t	i;

	ft_memset(box->WAD.header.type, 0 , 5);

	box->WAD.fd = open("DOOM.WAD", O_RDONLY);
	if (!box->WAD.fd)
		return (printf("ERROR WHILE OPENING WAD FILE\n"), 1);

	read(box->WAD.fd, box->WAD.header.type, 4);
	read(box->WAD.fd, (void *)&box->WAD.header.dir_count, 4);
	read(box->WAD.fd, (void *)&box->WAD.header.dir_offset, 4);

	printf("%s | %i | %i\n", box->WAD.header.type, box->WAD.header.dir_count, box->WAD.header.dir_offset);

	lseek(box->WAD.fd, box->WAD.header.dir_offset, SEEK_SET);

	box->WAD.dirs = malloc(box->WAD.header.dir_count * sizeof(struct s_directory));

	i = -1;
	while (++i < box->WAD.header.dir_count)
	{
		ft_memset(box->WAD.dirs[i].name, 0 , 9);
		read(box->WAD.fd, (void *)&box->WAD.dirs[i].lump_offset, 4);
		read(box->WAD.fd, (void *)&box->WAD.dirs[i].lump_size, 4);
		read(box->WAD.fd, box->WAD.dirs[i].name, 8);
	}
	parse_maps(box);

	// uint32_t	m;
	// printf("\nPRINTING DIRECTORIES\n\n");
	// i = -1;
	// while (++i < box->WAD.header.dir_count)
	// 	printf("%i | %i | %s\n", box->WAD.dirs[i].lump_offset, box->WAD.dirs[i].lump_size, box->WAD.dirs[i].name);

	// printf("\nPRINTING MAPS\n\n");
	// m = -1;
	// while (++m < 9)
	// {
	// 	printf("%s\n", box->WAD.maps[m].name);
	// 	i = -1;
	// 	while (++i < box->WAD.maps[m].n_vertexes)
	// 		printf("%i | %i \n", box->WAD.maps[m].vertexes[i].x, box->WAD.maps[m].vertexes[i].y);
	// 	i = -1;
	// 	while (++i < box->WAD.maps[m].n_linedefs)
	// 		printf("%i | %i \n", box->WAD.maps[m].linedef[i].start_vertex, box->WAD.maps[m].linedef[i].end_vertex);
	// }

	close(box->WAD.fd);
	return (0);

}
