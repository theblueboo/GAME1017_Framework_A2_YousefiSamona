#include "MathManager.h"

double MathManager::Distance(const double& x1, const double& x2, 
	const double& y1, const double& y2)
{
	return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

double MathManager::Distance(const SDL_FRect* r1, const SDL_FRect* r2)
{
	return sqrt((r2->x - r1->x) * (r2->x - r1->x) + (r2->y - r1->y) * (r2->y - r1->y));
}

double MathManager::Distance(const SDL_FPoint* p1, const SDL_FPoint* p2)
{
	return sqrt((p2->x - p1->x) * (p2->x - p1->x) + (p2->y - p1->y) * (p2->y - p1->y));
}

SDL_FPoint MathManager::Normalize(SDL_FPoint vec)
{
	SDL_FPoint norm = { 0.0f, 0.0f };
	float x = vec.x;
	float y = vec.y;
	float length = (x * x) + (y * y);
	if (length > 0)
	{
		length = 1.0f / sqrt(length);
		norm.x = x * length;
		norm.y = y * length;
	}
	return norm;
}

double MathManager::AngleBetweenPoints(const double dy, const double dx)
{
	return atan2(dy, dx); // In radians.
}

void MathManager::SetDeltas(const double angle, double& dx, double& dy, double fx, double fy)
{
	dx = SetDeltaX(angle, fx);
	dy = SetDeltaY(angle, fy);
}

double MathManager::SetDeltaX(const double angle, double fx)
{
	return cos(angle) * fx;
}

double MathManager::SetDeltaY(const double angle, double fy)
{
	return sin(angle) * fy;
}

double MathManager::Deg2Rad(const double angle)
{
	return angle * (M_PI / 180.0);
}

double MathManager::Rad2Deg(const double angle)
{
	return angle * (180.0 / M_PI);
}

SDL_Rect MathManager::ConvertFRect2Rect(const SDL_FRect& r)
{
	return { (int)r.x, (int)r.y, (int)r.w, (int)r.h };
}

SDL_Point MathManager::ConvertFPoint2Point(const SDL_FPoint& p)
{
	return { (int)p.x, (int)p.y };
}

double MathManager::Angle180(double angle)
{ // Constrains an angle between -180 and 180.
	angle = fmod(angle + 180.0, 360.0);
	if (angle < 0)
		angle += 360.0;
	return angle - 180.0;
}

double MathManager::Angle360(double angle)
{ // Constrains an angle between 0 and 360.
	angle = fmod(angle, 360.0);
	if (angle < 0)
		angle += 360.0;
	return angle;
}

double MathManager::LerpD(double a, double b, double factor)
{
	return ((1.0 - factor) * a) + (factor * b);
}

double MathManager::LerpRad(double a, double b, double factor)
{
	double result; 
	double diff = b - a;
	if (diff < -M_PI)
	{
		b += M_PI*2;
		result = LerpD(a, b, factor);
		if (result >= M_PI * 2)
			result -= M_PI * 2;
	}
	else if (diff > M_PI)
	{
		b -= M_PI*2;
		result = LerpD(a, b, factor);
		if (result < 0.0)
			result += M_PI * 2;
	}
	else
		result = LerpD(a, b, factor);
	return result;
}

SDL_Point MathManager::HalfwayPoint(const SDL_Point& from, const SDL_Point& to)
{
	return { (int)(((double)from.x + (double)to.x) / 2.0), (int)(((double)from.y + (double)to.y) / 2.0) };
}

SDL_FPoint MathManager::HalfwayPoint(const SDL_FPoint& from, const SDL_FPoint& to)
{
	return { (from.x + to.x) / 2.0f, (from.y + to.y) / 2.0f };
}

int MathManager::Clamp(const int a, const int min, const int max)
{
	return std::min(std::max(a, min), max);
}

float MathManager::Clamp(const float a, const float min, const float max)
{
	return std::min(std::max(a, min), max);
}
