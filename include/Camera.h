
#pragma once

#include "main.h"

class Camera {
public:
	Camera();

	void Update();
	void Reset();

	// Access functions
	void SetAspect(float a)					{Aspect=a;}
	void SetDistance(float d)				{Distance=d;}
	void SetAzimuth(float a)				{Azimuth=a;}
	void SetIncline(float i)				{Incline=i;}

	float GetDistance()						{return Distance;}
	float GetAzimuth()						{return Azimuth;}
	float GetIncline()						{return Incline;}

	const glm::mat4 &GetViewProjectMtx()	{return ViewProjectMtx;}

private:
	// Perspective controls
	float FOV;			// Field of View Angle (degrees)
	float Aspect;		// Aspect Ratio
	float NearClip;		// Near clipping plane distance
	float FarClip;		// Far clipping plane distance

	// Polar controls
	float Distance;		// Distance of the camera eye position to the origin (meters)
	float Azimuth;		// Rotation of the camera eye position around the Y axis (degrees)
	float Incline;		// Angle of the camera eye position over the XZ plane (degrees)

	// Computed data
	glm::mat4 ViewProjectMtx;
};

////////////////////////////////////////////////////////////////////////////////
