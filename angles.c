#include "doom-nukem.h"

void	normalize_360(float *angle)
{
	*angle = fmod(*angle, 360);
	while (*angle < 0)
		*angle += 360;
}

t_angle	new_angle(float angle)
{
	t_angle	new;

	new.angle_val = angle;
	normalize_360(&new.angle_val);
	return (new);
}

t_angle	angle_to_vortex(t_box *box, t_vertex vortex)
{
	float Vdx = vortex.x - box->WAD.maps[0].player.x;
	float Vdy = vortex.y - box->WAD.maps[0].player.y;

	t_angle angle = new_angle(atan2f(Vdy, Vdx) * 180 / M_PI);

	return (angle);
}

bool	clip_vertexes_in_FOV(t_box *box, t_vertex v1, t_vertex v2)
{
	t_angle	v1_angle = angle_to_vortex(box, v1);
	t_angle	v2_angle = angle_to_vortex(box, v2);

	t_angle	v1_to_v2 = new_angle(v1_angle.angle_val - v2_angle.angle_val);

	if (v1_to_v2.angle_val >= 180)
		return (false);

	v1_angle.angle_val -= box->WAD.maps[0].player.angle.angle_val;
	v2_angle.angle_val -= box->WAD.maps[0].player.angle.angle_val;

	t_angle	half_FOV = new_angle(FOV / 2);

	t_angle	v1_moved = new_angle(v1_angle.angle_val + half_FOV.angle_val);

	if (v1_moved.angle_val > FOV)
	{
		t_angle	v1_moved_angle = new_angle(v1_moved.angle_val - FOV);

		if (v1_moved_angle.angle_val >= v1_to_v2.angle_val)
			return (false);
		v1_angle.angle_val = half_FOV.angle_val;
	}

	t_angle	v2_moved = new_angle(half_FOV.angle_val - v2_angle.angle_val);

	if (v2_moved.angle_val > FOV)
		v2_angle.angle_val -= half_FOV.angle_val;

	v1_angle.angle_val += 90;
	v2_angle.angle_val += 90;

	return (true);
}

int	angle_to_screen(t_angle angle)
{
	int	ix = 0;

	// printf("DEBUG: %f° == ", angle.angle_val);
	if (angle.angle_val > 90)
	{
		angle.angle_val -= 90;
		ix = (SCREENWIDTH / 2) - round(tanf(angle.angle_val * M_PI / 180.0f) * (SCREENWIDTH / 2));
	}
	else
	{
		angle.angle_val = 90 - angle.angle_val;
		ix = round(tanf(angle.angle_val * M_PI / 180.0f) * (SCREENWIDTH / 2));
		ix += (SCREENWIDTH / 2);
	}
	// printf("%i\n", ix);
	return (ix);
}
