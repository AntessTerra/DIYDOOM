#include "doom-nukem.h"

void	normalize_360(float *angle)
{
	*angle = fmod(*angle, 360);
	while (*angle < 0)
		*angle += 360;
}

float	get_cos_value(t_angle angle)
{
	return (cosf(angle.angle_val * M_PI / 180.0f));
}

float	get_sin_value(t_angle angle)
{
	return (sinf(angle.angle_val * M_PI / 180.0f));
}

float	get_tan_value(t_angle angle)
{
	return (tanf(angle.angle_val * M_PI / 180.0f));
}

float	get_signed_value(t_angle angle)
{
	if (angle.angle_val > 180)
		return (angle.angle_val - 360);
	return (angle.angle_val);
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

int	angle_to_screen(t_angle angle)
{
	int	ix = 0;

	// printf("DEBUG: %f° == ", angle.angle_val);
	if (angle.angle_val > 90)
	{
		angle.angle_val -= 90;
		ix = (SCREENWIDTH / 2) - round(get_tan_value(angle) * (SCREENWIDTH / 2));
	}
	else
	{
		angle.angle_val = 90 - angle.angle_val;
		ix = round(get_tan_value(angle) * (SCREENWIDTH / 2));
		ix += (SCREENWIDTH / 2);
	}
	// printf("%i\n", ix);
	return (ix);
}
