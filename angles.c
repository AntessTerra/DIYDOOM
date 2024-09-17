#include "doom-nukem.h"

/**
 * 	Normalizes angle to 0-360
 *
 * 	@param float* angle
 */
void	normalize_360(float *angle)
{
	*angle = fmod(*angle, 360);
	while (*angle < 0)
		*angle += 360;
	while (*angle > 360)
		*angle -= 360;
}

/**
 * 	Returns cosine value of angle
 *
 * 	@param t_angle angle
 *
 * 	@return {float} cosine value of angle
 */
float	get_cos_value(t_angle angle)
{
	return (cosf(angle.angle_val * M_PI / 180.0f));
}

/**
 * 	Returns sine value of angle
 *
 * 	@param t_angle angle
 *
 * 	@return {float} sine value of angle
 */
float	get_sin_value(t_angle angle)
{
	return (sinf(angle.angle_val * M_PI / 180.0f));
}

/**
 * 	Returns tangent value of angle
 *
 * 	@param t_angle angle
 *
 * 	@return {float} tangent value of angle
 */
float	get_tan_value(t_angle angle)
{
	return (tanf(angle.angle_val * M_PI / 180.0f));
}

/**
 * 	Returns signed value of angle
 *
 * 	@param t_angle angle
 *
 * 	@return {float} signed value of angle
 */
float	get_signed_value(t_angle angle)
{
	if (angle.angle_val > 180)
		return (angle.angle_val - 360);
	return (angle.angle_val);
}

/**
 * 	Creates new angle and normalizes it
 *
 * 	@param float angle
 *
 * 	@return {t_angle} new angle
 */
t_angle	new_angle(float angle)
{
	t_angle	new;

	new.angle_val = angle;
	normalize_360(&new.angle_val);
	return (new);
}

/**
 * 	Converts vortex to angle
 *
 * 	@param t_box* box
 * 	@param t_vertex vortex
 *
 * 	@return {t_angle} angle from 0 to 360
 */
t_angle	angle_to_vortex(t_box *box, t_vertex vortex)
{
	float Vdx = vortex.x - box->map->player.x;
	float Vdy = vortex.y - box->map->player.y;

	t_angle angle = new_angle(atan2f(Vdy, Vdx) * 180.f / M_PI);

	return (angle);
}

/**
 * 	Converts angle to x screen coordinate
 *
 * 	@param t_angle angle
 *
 * 	@return {int} x screen coordinate
 */
int	angle_to_screen(t_angle angle)
{
	int	ix = 0;

	if (angle.angle_val > 90)
	{
		angle.angle_val -= 90;
		ix = ((SCREENWIDTH / 2) / get_tan_value((t_angle){FOV/2})) - roundf(get_tan_value(angle) * (SCREENWIDTH / 2));
	}
	else
	{
		angle.angle_val = 90 - angle.angle_val;
		ix = roundf(get_tan_value(angle) * (SCREENWIDTH / 2));
		ix += ((SCREENWIDTH / 2) / get_tan_value((t_angle){FOV/2}));
	}
	return (ix);
}
