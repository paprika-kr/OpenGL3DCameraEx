#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <FreeImage/FreeImage.h>

#include "Shaders/LoadShaders.h"
#include "My_Shading.h"
#include "MyObjects.h"

// include glm/*.hpp only if necessary
//#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, lookAt, perspective, etc.
#include <glm/gtc/matrix_inverse.hpp> // inverseTranspose, etc.
glm::mat4 ModelViewProjectionMatrix, ModelViewMatrix;
glm::mat3 ModelViewMatrixInvTrans;
glm::mat4 ViewProjectionMatrix, ViewMatrix, ProjectionMatrix;

#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f

// lights in scene
Light_Parameters light[NUMBER_OF_LIGHT_SUPPORTED];

//RGB
float r = 0.0f;
float g = 0.0f;
float b = 0.0f;

// for animation
unsigned int timestamp_scene = 0; // the global clock in the scene
float timestamp_background = 0.0f;
float timestamp_background_temp = 0.0f;
bool isDayOver = false;

bool isAxisOn = true;
bool isFloorOn = true;

int isAnimationRun, flag_polygon_fill;
int timestamp_tiger = 0;
bool isTigerStandUp = true;

int timestamp_wolf = 0;
bool lookat_wolf = false;

int timestamp_spider = 0;

int timestamp_dragon = 0;
bool isDragonDeparted = false;

bool tigerStop = false;
float tigerStopTime = 0.0f;
bool wolfStop = false;
bool spiderStop = false;
bool dragonStop = false;

/*********************************  START: camera *********************************/
typedef struct _Camera {
	float pos[3];
	float uaxis[3], vaxis[3], naxis[3];
	float fovy, aspect_ratio, near_c, far_c;
	int move;
} Camera;

Camera camera[5];
bool isCamNumZero = true;
float camZeroAddedPosX = 0.0f;
float camZeroAddedPosY = 0.0f;
float camZeroAddedPosZ = 0.0f;

int curCamNum = 0;
float fovy = 45.0f;

void set_camera(void) {
	//camera #5 -> can control & zoom + init cam
	camera[0].pos[0] = 1100.0f; camera[0].pos[1] = 500.0f;  camera[0].pos[2] = 0.0f;
	camera[0].uaxis[0] = 0.0f; camera[0].uaxis[1] = 0.0f; camera[0].uaxis[2] = -1.0f;
	camera[0].vaxis[0] = -0.5f; camera[0].vaxis[2] = 0.0f; camera[0].vaxis[1] = 1.0f;
	camera[0].naxis[1] = 0.0f; camera[0].naxis[2] = 0.0f; camera[0].naxis[0] = 1.0f;

	camera[0].move = 0;
	camera[0].fovy = 45.0f, camera[0].aspect_ratio = 1.0f; camera[0].near_c = 0.1f; camera[0].far_c = 2500.0f;

	//camera #1 -> can rotate
	camera[1].pos[0] = -50.0f; camera[1].pos[1] = 100.0f;  camera[1].pos[2] = 70.0f;
	camera[1].uaxis[0] = camera[1].uaxis[1] = 0.0f; camera[1].uaxis[2] = -1.0f;
	camera[1].vaxis[0] = camera[1].vaxis[2] = 0.0f; camera[1].vaxis[1] = 1.0f;
	camera[1].naxis[1] = camera[1].naxis[2] = 0.0f; camera[1].naxis[0] = 1.0f;

	camera[1].move = 0;
	camera[1].fovy = 90.0f, camera[1].aspect_ratio = 1.0f; camera[1].near_c = 0.1f; camera[1].far_c = 2500.0f;

	//camera #2
	camera[2].pos[0] = -1000.0f; camera[2].pos[1] = 700.0f;  camera[2].pos[2] = 0.0f;
	camera[2].uaxis[0] = 0.0f; camera[2].uaxis[1] = 0.0f; camera[2].uaxis[2] = 1.0f;
	camera[2].vaxis[0] = 1.0f; camera[2].vaxis[2] = 0.0f; camera[2].vaxis[1] = 1.0f;
	camera[2].naxis[1] = 0.0f; camera[2].naxis[2] = 0.0f; camera[2].naxis[0] = -1.0f;

	camera[2].fovy = 80.0f, camera[2].aspect_ratio = 1.0f; camera[2].near_c = 0.1f; camera[2].far_c = 2500.0f;

	//camera #3
	camera[3].pos[0] = 0.0f; camera[3].pos[1] = 1500.0f;  camera[3].pos[2] = 0.0f;
	camera[3].uaxis[0] = camera[3].uaxis[1] = 0.0f; camera[3].uaxis[2] = 1.0f;
	camera[3].vaxis[0] = 1.0f; camera[3].vaxis[1] = 0.0f; camera[3].vaxis[2] = 0.0f;
	camera[3].naxis[0] = 0.0f;  camera[3].naxis[1] = 1.0f; camera[3].naxis[2] = 0.0f;

	camera[3].move = 0;
	camera[3].fovy = 45.0f, camera[3].aspect_ratio = 1.0f; camera[3].near_c = 0.1f; camera[3].far_c = 2500.0f;

	//camera #4
	camera[4].pos[0] = 800.0f; camera[4].pos[1] = 1000.0f;  camera[4].pos[2] = -800.0f;
	camera[4].uaxis[0] = 2.0f; camera[4].uaxis[1] = 0.0f; camera[4].uaxis[2] = 2.0f;
	camera[4].vaxis[0] = -1.0f; camera[4].vaxis[1] = 2.0f; camera[4].vaxis[2] = 1.0f;
	camera[4].naxis[0] = 1.334f;  camera[4].naxis[1] = 1.334f; camera[4].naxis[2] = -1.334f;

	camera[4].move = 0;
	camera[4].fovy = 60.0f, camera[4].aspect_ratio = 1.0f; camera[4].near_c = 0.1f; camera[4].far_c = 5000.0f;
}

void set_ViewMatrix_from_camera_frame(int num) {
	ViewMatrix = glm::mat4(camera[num].uaxis[0], camera[num].vaxis[0], camera[num].naxis[0], 0.0f,
		camera[num].uaxis[1], camera[num].vaxis[1], camera[num].naxis[1], 0.0f,
		camera[num].uaxis[2], camera[num].vaxis[2], camera[num].naxis[2], 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	if(num == 0)
		ViewMatrix = glm::translate(ViewMatrix, glm::vec3(
			-camera[num].pos[0] + camZeroAddedPosX, 
			-camera[num].pos[1] + camZeroAddedPosY, 
			-camera[num].pos[2] + camZeroAddedPosZ));

	else
		ViewMatrix = glm::translate(ViewMatrix, glm::vec3(
			-camera[num].pos[0],
			-camera[num].pos[1],
			-camera[num].pos[2]));
}

void initialize_camera(int num) {
	set_camera();
	set_ViewMatrix_from_camera_frame(num);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
}

#define CAM_RSPEED 0.1f
#define CAM0_PITCH 0
#define CAM0_YAW 1
#define CAM0_ROLL 2
int cam0_rotate_state = 0;

void renew_cam0_orientation_rotation_around(int angle) {
	// let's get a help from glm
	glm::mat3 RotationMatrix;
	glm::vec3 direction;

	switch (cam0_rotate_state)
	{
	case CAM0_PITCH:
		RotationMatrix = glm::mat3(glm::rotate(glm::mat4(1.0), CAM_RSPEED * TO_RADIAN * angle,
			glm::vec3(camera[0].uaxis[0], camera[0].uaxis[1], camera[0].uaxis[2])));
		break;
	case CAM0_YAW:
		RotationMatrix = glm::mat3(glm::rotate(glm::mat4(1.0), CAM_RSPEED * TO_RADIAN * angle,
			glm::vec3(camera[0].vaxis[0], camera[0].vaxis[1], camera[0].vaxis[2])));
		break;
	case CAM0_ROLL:
		RotationMatrix = glm::mat3(glm::rotate(glm::mat4(1.0), CAM_RSPEED * TO_RADIAN * angle,
			glm::vec3(camera[0].naxis[0], camera[0].naxis[1], camera[0].naxis[2])));
		break;
	default:
		break;
	}

	direction = RotationMatrix * glm::vec3(camera[0].uaxis[0], camera[0].uaxis[1], camera[0].uaxis[2]);
	camera[0].uaxis[0] = direction.x; camera[0].uaxis[1] = direction.y; camera[0].uaxis[2] = direction.z;
	direction = RotationMatrix * glm::vec3(camera[0].naxis[0], camera[0].naxis[1], camera[0].naxis[2]);
	camera[0].naxis[0] = direction.x; camera[0].naxis[1] = direction.y; camera[0].naxis[2] = direction.z;
	direction = RotationMatrix * glm::vec3(camera[0].vaxis[0], camera[0].vaxis[1], camera[0].vaxis[2]);
	camera[0].vaxis[0] = direction.x; camera[0].vaxis[1] = direction.y; camera[0].vaxis[2] = direction.z;
}

void renew_cam1_orientation_rotation_around(int angle_pitch, int angle_yaw) {
	// let's get a help from glm
	glm::mat3 RotationMatrix, R_pitch, R_yaw;
	glm::vec3 direction;

	R_pitch = glm::mat3(glm::rotate(glm::mat4(1.0), -CAM_RSPEED * TO_RADIAN * angle_pitch,
		glm::vec3(camera[1].vaxis[0], camera[1].vaxis[1], camera[1].vaxis[2])));

	R_yaw = glm::mat3(glm::rotate(glm::mat4(1.0), -CAM_RSPEED * TO_RADIAN * angle_yaw,
		glm::vec3(camera[1].uaxis[0], camera[1].uaxis[1], camera[1].uaxis[2])));

	RotationMatrix = R_pitch * R_yaw;

	direction = RotationMatrix * glm::vec3(camera[1].uaxis[0], camera[1].uaxis[1], camera[1].uaxis[2]);
	camera[1].uaxis[0] = direction.x; camera[1].uaxis[1] = direction.y; camera[1].uaxis[2] = direction.z;
	direction = RotationMatrix * glm::vec3(camera[1].naxis[0], camera[1].naxis[1], camera[1].naxis[2]);
	camera[1].naxis[0] = direction.x; camera[1].naxis[1] = direction.y; camera[1].naxis[2] = direction.z;
	direction = RotationMatrix * glm::vec3(camera[1].vaxis[0], camera[1].vaxis[1], camera[1].vaxis[2]);
	camera[1].vaxis[0] = direction.x; camera[1].vaxis[1] = direction.y; camera[1].vaxis[2] = direction.z;
}

/*********************************  END: camera *********************************/

// callbacks
float PRP_distance_scale[6] = { 0.5f, 1.0f, 2.5f, 5.0f, 10.0f, 20.0f };

void display(void) {
	r = timestamp_background / 1200.0f + 0.5f; g = timestamp_background / 3200.0f + 0.4f; b = timestamp_background / 4800.0f + 0.3f;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(r , g , b, 1.0f);

	//Draw axes
	if (isAxisOn) {
		glUseProgram(h_ShaderProgram_simple);
		ModelViewMatrix = glm::scale(ViewMatrix, glm::vec3(100.0f, 100.0f, 100.0f));
		ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix_simple, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glLineWidth(4.0f);
		draw_axes();
		glLineWidth(1.0f);
	}
	glUseProgram(h_ShaderProgram_TXPS);
	//Draw floor
	if (isFloorOn) {
		set_material_floor();
		glUniform1i(loc_texture, TEXTURE_ID_FLOOR);
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-500.0f, 0.0f, 500.0f));
		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(1000.0f, 1000.0f, 1000.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
		ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
		ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

		glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
		glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
		draw_floor();
	}
	

	//1~4 : Dynamic Objects [tiger/wolf/spider/dragon]
	//5~9 : Static Objects [godzilla/bus/bike/tank/optimus]
	//****************Dynamic Objects********************
	//1. Draw tiger
 	set_material_tiger();
	glUniform1i(loc_texture, TEXTURE_ID_TIGER);
 /*	ModelViewMatrix = glm::rotate(ViewMatrix, -rotation_angle_tiger, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(200.0f, 0.0f, 0.0f));
	
	*/
	ModelViewMatrix = glm::rotate(ViewMatrix, 90*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	if (timestamp_tiger >= 0 && timestamp_tiger < 45) {
		ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(0.0f, 0.0f, 150.0f));
	}
	else if (timestamp_tiger >= 45 && timestamp_tiger < 135) {
		ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(0.0f, 80.0f * sinf(2 * (timestamp_tiger - 45) * TO_RADIAN), 150.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -30 * sinf((timestamp_tiger-45) * 4 * TO_RADIAN) * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	}
	else if (timestamp_tiger >= 135 && timestamp_tiger < 150) {
		ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(0.0f, 0.0f, 150.0f));
	}
	else if (timestamp_tiger >= 150 && timestamp_tiger < 240) {
		ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(0.0f, 80.0f * sinf(2 * (timestamp_tiger - 150) * TO_RADIAN), 150.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -30 * sinf((timestamp_tiger - 150) * 4 * TO_RADIAN) * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	}
	else if (timestamp_tiger >= 240 && timestamp_tiger < 420) {
		if (timestamp_tiger == 240.0f)
			isTigerStandUp = false;

		ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3((timestamp_tiger - 240.0f) / 3.0f, 50.0f, 150.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -8*(timestamp_tiger - 240) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(0.0f, -50.0f, 0.0f));
	}
	else if (timestamp_tiger == 420) {
		isTigerStandUp = true;
	}
	else if (timestamp_tiger > 420 && timestamp_tiger < 465) {
		ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(60.0f, 0.0f, 150.0f));
	}
	else if (timestamp_tiger >= 465 && timestamp_tiger < 555) {
		ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(60.0f, 80.0f * sinf(2 * (timestamp_tiger - 465) * TO_RADIAN), 150.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -30 * sinf((timestamp_tiger - 465) * 4 * TO_RADIAN) * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	}
	else if (timestamp_tiger >= 555 && timestamp_tiger < 570) {
		ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(60.0f, 0.0f, 150.0f));
	}
	else if (timestamp_tiger >= 570 && timestamp_tiger < 750) {
		if (timestamp_tiger == 570.0f)
			isTigerStandUp = false;

		ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(60.0f - (timestamp_tiger - 570.0f) / 3.0f, 50.0f, 150.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, 8 * (timestamp_tiger - 570) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewMatrix = glm::translate(ModelViewMatrix, glm::vec3(0.0f, -50.0f, 0.0f));
	}
	else if (timestamp_tiger == 750) {
			isTigerStandUp = true;
	}

	ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	draw_tiger();

	//2. Draw wolf
	//set_material_wolf();
	glUniform1i(loc_texture, TEXTURE_ID_WOLF);

	if (timestamp_wolf >= 0 && timestamp_wolf <= 180)
		lookat_wolf = false; //look at right side
	if (timestamp_wolf > 180 && timestamp_wolf <= 359)
		lookat_wolf = true;	 //look at left side

	if (!lookat_wolf && timestamp_wolf >= 120) {	//turn left
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(60.0f, 0.0f, -240.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, 3 * (timestamp_wolf + 180) * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else if (!lookat_wolf) {
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(60.0f, 0.0f, -2*timestamp_wolf));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, 180 * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else if (lookat_wolf && timestamp_wolf >= 300) {	//turn right
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(60.0f, 0.0f, 0.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, 3 * (timestamp_wolf + 180) * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else if (lookat_wolf) {
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(60.0f, 0.0f, 2 * timestamp_wolf - 600.0f));
	}
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(150.0f, 150.0f, 150.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	glUniform3f(loc_primitive_color, 0.482f, 0.408f, 0.933f);
	draw_wolf();

	//3. Draw spider
	//set_material_tiger();
	glUniform1i(loc_texture, TEXTURE_ID_SPIDER);
	if (timestamp_spider >= 0 && timestamp_spider < 180) {
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(0.0f, 500.0f - 2 * timestamp_spider, 250.0f));
	}
	else if (timestamp_spider >= 180 && timestamp_spider < 360) {
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(
			100.0f * sinf(TO_RADIAN * (timestamp_spider - 180)), 
			140.0f + 100.0f * sinf(TO_RADIAN * (timestamp_spider - 180)), 
			250.0f + 20 * sinf(TO_RADIAN * 4 * (timestamp_spider - 180))));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, (timestamp_spider - 180) * 4 * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else if (timestamp_spider >= 360 && timestamp_spider < 540) {
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(
			100.0f * sinf(TO_RADIAN * (timestamp_spider - 180)),
			140.0f - 100.0f * sinf(TO_RADIAN * (timestamp_spider - 180)),
			250.0f + 20 * sinf(TO_RADIAN * 4 * (timestamp_spider - 180))));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, timestamp_spider * 4 * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else if (timestamp_spider >= 540 && timestamp_spider < 720) {
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(
			0.0f, 
			140.0f + 2 * (timestamp_spider - 540.0f), 
			250.0f + 50 * sinf(TO_RADIAN * 2 * (timestamp_spider - 180))));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, (timestamp_spider - 180) * 2 * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	ModelViewMatrix = glm::rotate(ModelViewMatrix, 90.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(50.0f, -50.0f, 50.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	draw_spider();

	//4. Draw dragon
	//set_material_tiger();
	glUniform1i(loc_texture, TEXTURE_ID_DRAGON);
	glUniform1i(loc_texture, TEXTURE_ID_DRAGON);
	if (!isDragonDeparted) {
		if (timestamp_dragon >= 0 && timestamp_dragon < 360)
			ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-450.0f + 0.6 * (timestamp_dragon), 100.0f + 0.003 * timestamp_dragon * timestamp_dragon, 0.0f));

		else if (timestamp_dragon >= 360 && timestamp_dragon < 450)
			ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-234.0f, 486.643f, 0.0f));

		else if (timestamp_dragon >= 450 && timestamp_dragon < 540) {
			ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-234.0f + 2 * (timestamp_dragon - 450.0f), 486.643f, 0.0f));
			ModelViewMatrix = glm::rotate(ModelViewMatrix, -TO_RADIAN * (timestamp_dragon - 90), glm::vec3(0.0f, 1.0f, 0.0f));
		}
			
		else if (timestamp_dragon >= 540 && timestamp_dragon < 900) {
			ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(
				-234.0f + 180 * cosf(TO_RADIAN * (timestamp_dragon - 540.0f)),
				486.643f + 100.0f * ((timestamp_dragon-540.0f)/360.0f)*sinf(TO_RADIAN * timestamp_dragon),
				180.0f * sinf(TO_RADIAN * (timestamp_dragon - 540.0f))));
			ModelViewMatrix = glm::rotate(ModelViewMatrix, -TO_RADIAN * (timestamp_dragon - 90), glm::vec3(0.0f, 1.0f, 0.0f));
		}

		else if (timestamp_dragon >= 900 && timestamp_dragon < 1440) {
			ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(
				-234.0f + (180.0f + (timestamp_dragon - 900.0f)) * cosf(TO_RADIAN * (timestamp_dragon - 540.0f)),
				486.643f + 100.0f * sinf(TO_RADIAN * timestamp_dragon),
				(180.0f + (timestamp_dragon - 900.0f)) * sinf(TO_RADIAN * (timestamp_dragon - 540.0f))));
			ModelViewMatrix = glm::rotate(ModelViewMatrix, -TO_RADIAN * (timestamp_dragon - 90), glm::vec3(0.0f, 1.0f, 0.0f));
		}

		if (timestamp_dragon == 1439)
			isDragonDeparted = true;
	}
	else {
		ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(
			-234.0f + 540.0f * cosf(TO_RADIAN * (timestamp_dragon - 540.0f)),
			486.643f + 100.0f * sinf(TO_RADIAN * timestamp_dragon),
			540.0f * sinf(TO_RADIAN * (timestamp_dragon - 540.0f))));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -TO_RADIAN * (timestamp_dragon - 90), glm::vec3(0.0f, 1.0f, 0.0f));
	}
	
	ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(18.0f, 18.0f, 18.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	draw_dragon();

	//****************Static Objects********************
	//5. Draw godzilla
	//set_material_tiger();
	glUniform1i(loc_texture, TEXTURE_ID_GODZILLA);
	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(400.0f, 0.0f, 280.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 210.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(2.0f, 2.0f, 2.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	draw_godzilla();

	//6. Draw bus
	//set_material_tiger();
	glUniform1i(loc_texture, TEXTURE_ID_BUS);
	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-330.0f, 0.0f, 280.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 150 * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(15.0f, 15.0f, 15.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	draw_bus();

	//7. Draw bike
	//set_material_tiger();
	glUniform1i(loc_texture, TEXTURE_ID_BIKE);
	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(-150.0f, 0.0f, -350.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 60 * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(30.0f, 30.0f, 30.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	draw_bike();

	//8. Draw tank
	//set_material_tiger();
	glUniform1i(loc_texture, TEXTURE_ID_TANK);
	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(100.0f, 0.0f, 100.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(40.0f, 40.0f, 40.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, 45.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	draw_tank();

	//9. Draw optimus
	//set_material_tiger();
	glUniform1i(loc_texture, TEXTURE_ID_OPTIMUS);
	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(300.0f, 0.0f, -280.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, -70.0f * TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(0.5f, 0.5f, 0.5f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	draw_optimus();


	/*
	glUseProgram(h_ShaderProgram_simple);
	ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(20.0f, 20.0f, 20.0f));
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_simple, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_axes();
	
	glUseProgram(h_ShaderProgram_TXPS);
	set_material_tiger();
	glUniform1i(loc_texture, TEXTURE_ID_TIGER);
	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(0.0f, 0.0f, 50.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(100.0f, -100.0f, -100.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	draw_ben();

	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(100.0f, 30.0f, 10.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(80.0f, 80.0f, 80.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	draw_cow();

	set_material_tiger();
	glUniform1i(loc_texture, TEXTURE_ID_TIGER);
	ModelViewMatrix = glm::translate(ViewMatrix, glm::vec3(0.0f, 0.0f, 120.0f));
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(20.0f, 20.0f, 20.0f));
	ModelViewProjectionMatrix = ProjectionMatrix * ModelViewMatrix;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_TXPS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_TXPS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_TXPS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	draw_ironman();
	*/

	glUseProgram(0);

	glutSwapBuffers();
}

void timer_scene(int value) {
	timestamp_scene = (timestamp_scene + 1) % UINT_MAX;
	timestamp_background_temp = timestamp_scene % 2400;

	if (timestamp_background_temp == 1200.0f) isDayOver = true;
	else if (timestamp_background_temp == 2399.0f) isDayOver = false;

	if (isDayOver == false) 
		timestamp_background = timestamp_background_temp;

	else 
		timestamp_background = 2400 - timestamp_background_temp;

	if (!isTigerStandUp) 
		cur_frame_tiger = timestamp_scene/3 % N_TIGER_FRAMES;
	
	//cur_frame_ben = timestamp_scene % N_BEN_FRAMES;
	cur_frame_wolf= timestamp_scene % N_WOLF_FRAMES;
	cur_frame_spider = timestamp_scene % N_SPIDER_FRAMES;
	if (!tigerStop) 
		timestamp_tiger = (timestamp_tiger + 1) % 751;
		
	if(!wolfStop)
		timestamp_wolf = (timestamp_wolf + 1) % 360;

	if(!spiderStop)
	timestamp_spider = (timestamp_spider + 1) % 720;
	
	if(!dragonStop)
	timestamp_dragon = (timestamp_dragon + 1) % 1440;

	glutPostRedisplay();
	if (isAnimationRun)
		glutTimerFunc(10, timer_scene, 0);
}


void keyboard(unsigned char key, int x, int y) {
	static int flag_cull_face = 0;
	static int PRP_distance_level = 4;

	glm::vec4 position_EC;
	glm::vec3 direction_EC;

	if ((key >= '0') && (key <= '0' + NUMBER_OF_LIGHT_SUPPORTED - 1)) {
	
		int cam_num = key - '0';

		switch (cam_num){
		case 0:
			isCamNumZero = true;
			curCamNum = 0;
			fprintf(stdout, "* Camera #5\n\n"); break;

		case 1:
			isCamNumZero = false;
			curCamNum = 1;
			fprintf(stdout, "* Camera #1\n\n"); break;

		case 2:
			isCamNumZero = false;
			curCamNum = 2;
			fprintf(stdout, "* Camera #2\n\n"); break;

		case 3:
			isCamNumZero = false;
			curCamNum = 3;
			fprintf(stdout, "* Camera #3\n\n"); break;

		case 4:
			isCamNumZero = false;
			curCamNum = 4;
			fprintf(stdout, "* Camera #4\n\n"); break;

		default:
			break;}
		
		set_ViewMatrix_from_camera_frame(cam_num);
		ProjectionMatrix = glm::perspective(
			TO_RADIAN * camera[curCamNum].fovy, camera[curCamNum].aspect_ratio, camera[curCamNum].near_c, camera[curCamNum].far_c);
		ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
		
		glutPostRedisplay();
		return;
	}
	
	//Cam#5 control
	if (curCamNum == 0) {
		switch (key) {
		case 'w':
			camZeroAddedPosX += 3.0f;
			break;
		case 's':
			camZeroAddedPosX -= 3.0f;
			break;
		case 'q':
			camZeroAddedPosY -= 3.0f;
			break;
		case 'e':
			camZeroAddedPosY += 3.0f;
			break;
		case 'a':
			camZeroAddedPosZ -= 3.0f;
			break;
		case 'd':
			camZeroAddedPosZ += 3.0f;
			break;
		}
		set_ViewMatrix_from_camera_frame(0);
	}
	
	//functions
	switch (key) {
	case 'f':
		cam0_rotate_state++;
		if (cam0_rotate_state == 3) cam0_rotate_state = 0;
		switch (cam0_rotate_state) {
		case CAM0_PITCH:
			printf("Cam#5 rotate state : Pitch\n");
			break;
		case CAM0_YAW:
			printf("Cam#5 rotate state : Yaw\n");
			break;
		case CAM0_ROLL:
			printf("Cam#5 rotate state : Roll\n");
			break;
		default:
			break;
		}
		break;

	case 'g': // toggle the animation effect.
		isAnimationRun = 1 - isAnimationRun;
		if (isAnimationRun) {
			glutTimerFunc(100, timer_scene, 0);
			fprintf(stdout, "Animation mode ON.\n");
		}
		else
			fprintf(stdout, "Animation mode OFF.\n");
		break;

	case 'r': //Axis on/off
		if (isAxisOn) {
			printf("Axis OFF\n");
			isAxisOn = false;
		}
		else {
			printf("Axis ON\n");
			isAxisOn = true;
		}
		break;

	case 't': //Floor on/off
		if (isFloorOn) {
			printf("Floor OFF\n");
			isFloorOn = false;
		}
		else {
			printf("Floor ON\n");
			isFloorOn = true;
		}
		break;

	case 'z': //tiger stop
		if (tigerStop) {
			tigerStop = false;
			printf("Tiger animation ON\n");
		}
			
		else {
			tigerStop = true;
			printf("Tiger animation OFF\n");
		}
		break;

	case 'x': //wolf stop
		if (wolfStop) {
			wolfStop = false;
			printf("Wolf animation ON\n");
		}
		else {
			wolfStop = true;
			printf("Wolf animation OFF\n");
		}	
		break;

	case 'c': //spider stop
		if (spiderStop) {
			spiderStop = false;
			printf("Spider animation ON\n");
		}	
		else {
			spiderStop = true;
			printf("Spider animation OFF\n");
		}	
		break;

	case 'v': //dragon stop
		if (dragonStop) {
			printf("Dragon animation OFF\n");
			dragonStop = false;
		}	
		else {
			printf("Dragon animation ON\n");
			dragonStop = true;
		}
		break;

	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups
		break;
		/*
	case 't':
		flag_texture_mapping = 1 - flag_texture_mapping;
		glUseProgram(h_ShaderProgram_TXPS);
		glUniform1i(loc_flag_texture_mapping, flag_texture_mapping);
		glUseProgram(0);
		glutPostRedisplay();
		break;
	
	case 'i':
		PRP_distance_level = (PRP_distance_level + 1) % 6;
		fprintf(stdout, "^^^ Distance level = %d.\n", PRP_distance_level);

		ViewMatrix = glm::lookAt(PRP_distance_scale[PRP_distance_level] * glm::vec3(500.0f, 300.0f, 500.0f),
			glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		glUseProgram(h_ShaderProgram_TXPS);
		// Must update the light 1's geometry in EC.
		position_EC = ViewMatrix * glm::vec4(light[1].position[0], light[1].position[1],
			light[1].position[2], light[1].position[3]);
		glUniform4fv(loc_light[1].position, 1, &position_EC[0]);
		direction_EC = glm::mat3(ViewMatrix) * glm::vec3(light[1].spot_direction[0],
			light[1].spot_direction[1], light[1].spot_direction[2]);
		glUniform3fv(loc_light[1].spot_direction, 1, &direction_EC[0]);
		glUseProgram(0);
		glutPostRedisplay();
		break;

	

	case 'p':
		flag_polygon_fill = 1 - flag_polygon_fill;
		if (flag_polygon_fill)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glutPostRedisplay();
		break;
	

	case 'f':
		flag_fog = 1 - flag_fog;
		glUseProgram(h_ShaderProgram_TXPS);
		glUniform1i(loc_flag_fog, flag_fog);
		glUseProgram(0);
		glutPostRedisplay();
		break;
	
	case 'c':
		flag_cull_face = (flag_cull_face + 1) % 3;
		switch (flag_cull_face) {
		case 0:
			glDisable(GL_CULL_FACE);
			glutPostRedisplay();
			break;
		case 1: // cull back faces;
			glCullFace(GL_BACK);
			glEnable(GL_CULL_FACE);
			glutPostRedisplay();
			break;
		case 2: // cull front faces;
			glCullFace(GL_FRONT);
			glEnable(GL_CULL_FACE);
			glutPostRedisplay();
			break;
		}
		break;*/

	}
}
int prevx, prevy;

void motion(int x, int y) {
	if (!(camera[1].move || camera[0].move)) return;

	if (camera[0].move) {
		renew_cam0_orientation_rotation_around(prevx - x);
		prevx = x; prevy = y;
		set_ViewMatrix_from_camera_frame(0);
	}
		
	else if (camera[1].move) {
		renew_cam1_orientation_rotation_around(prevx - x, prevy - y);
		prevx = x; prevy = y;
		set_ViewMatrix_from_camera_frame(1);
	}
	
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)	{
	//Zoom in & out for Cam#5
	if (curCamNum == 0) {
		switch (button) {
		case 3:
			if (camera[curCamNum].fovy >= 1.0f) {
				camera[curCamNum].fovy -= 1.0f;
				set_ViewMatrix_from_camera_frame(curCamNum);
				ProjectionMatrix = glm::perspective(
					TO_RADIAN * camera[curCamNum].fovy, camera[curCamNum].aspect_ratio, camera[curCamNum].near_c, camera[curCamNum].far_c);
				ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
			}
			else
				printf("Zoom in has reached its maximum!!\n");
			break;

		case 4:
			if (camera[curCamNum].fovy <= 89.0f) {
				camera[curCamNum].fovy += 1.0f;
				set_ViewMatrix_from_camera_frame(curCamNum);
				ProjectionMatrix = glm::perspective(
					TO_RADIAN * camera[curCamNum].fovy, camera[curCamNum].aspect_ratio, camera[curCamNum].near_c, camera[curCamNum].far_c);
				ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
			}
			else
				printf("Zoom Out has reached its maximum!!\n");
			break;

		default:
			break;
		}
	}

	if ((button == GLUT_LEFT_BUTTON)) {
		if (state == GLUT_DOWN) {
			if (curCamNum == 0) 
				camera[0].move = 1;				

			else if(curCamNum == 1)
				camera[1].move = 1;

			prevx = x; prevy = y;
		}
		else if (state == GLUT_UP) {
			camera[0].move = 0;
			camera[1].move = 0;
		}
	}	
}

void reshape(int width, int height) {
	float aspect_ratio;

	glViewport(0, 0, width, height);
	
	camera[curCamNum].aspect_ratio = (float) width / height;
	ProjectionMatrix = glm::perspective(
		TO_RADIAN * camera[curCamNum].fovy, camera[curCamNum].aspect_ratio, camera[curCamNum].near_c, camera[curCamNum].far_c);

	glutPostRedisplay();
}

void cleanup(void) {
	glDeleteVertexArrays(1, &axes_VAO); 
	glDeleteBuffers(1, &axes_VBO);

	glDeleteVertexArrays(1, &rectangle_VAO);
	glDeleteBuffers(1, &rectangle_VBO);

	glDeleteVertexArrays(1, &tiger_VAO);
	glDeleteBuffers(1, &tiger_VBO);

	glDeleteVertexArrays(1, &wolf_VAO);
	glDeleteBuffers(1, &wolf_VBO);

	glDeleteVertexArrays(1, &spider_VAO);
	glDeleteBuffers(1, &spider_VBO);

	glDeleteVertexArrays(1, &dragon_VAO);
	glDeleteBuffers(1, &dragon_VBO);

	glDeleteVertexArrays(1, &bus_VAO);
	glDeleteBuffers(1, &bus_VBO);

	glDeleteVertexArrays(1, &bike_VAO);
	glDeleteBuffers(1, &bike_VBO);

	glDeleteVertexArrays(1, &tank_VAO);
	glDeleteBuffers(1, &tank_VBO);

	glDeleteVertexArrays(1, &optimus_VAO);
	glDeleteBuffers(1, &optimus_VBO);

	glDeleteVertexArrays(1, &godzilla_VAO);
	glDeleteBuffers(1, &godzilla_VBO);

	glDeleteTextures(N_TEXTURES_USED, texture_names);
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMotionFunc(motion);
	glutMouseFunc(mouse);
	glutReshapeFunc(reshape);
	glutTimerFunc(100, timer_scene, 0);
	glutCloseFunc(cleanup);
}

void prepare_shader_program(void) {
	int i;
	char string[256];
	ShaderInfo shader_info_simple[3] = {
		{ GL_VERTEX_SHADER, "Shaders/simple.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/simple.frag" },
		{ GL_NONE, NULL }
	};
	ShaderInfo shader_info_TXPS[3] = {
		{ GL_VERTEX_SHADER, "Shaders/Phong_Tx.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/Phong_Tx.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram_simple = LoadShaders(shader_info_simple);
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram_simple, "u_primitive_color");
	loc_ModelViewProjectionMatrix_simple = glGetUniformLocation(h_ShaderProgram_simple, "u_ModelViewProjectionMatrix");

	h_ShaderProgram_TXPS = LoadShaders(shader_info_TXPS);
	loc_ModelViewProjectionMatrix_TXPS = glGetUniformLocation(h_ShaderProgram_TXPS, "u_ModelViewProjectionMatrix");
	loc_ModelViewMatrix_TXPS = glGetUniformLocation(h_ShaderProgram_TXPS, "u_ModelViewMatrix");
	loc_ModelViewMatrixInvTrans_TXPS = glGetUniformLocation(h_ShaderProgram_TXPS, "u_ModelViewMatrixInvTrans");

	loc_global_ambient_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_global_ambient_color");
	for (i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		sprintf(string, "u_light[%d].light_on", i);
		loc_light[i].light_on = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].position", i);
		loc_light[i].position = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].ambient_color", i);
		loc_light[i].ambient_color = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].diffuse_color", i);
		loc_light[i].diffuse_color = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].specular_color", i);
		loc_light[i].specular_color = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].spot_direction", i);
		loc_light[i].spot_direction = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].spot_exponent", i);
		loc_light[i].spot_exponent = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].spot_cutoff_angle", i);
		loc_light[i].spot_cutoff_angle = glGetUniformLocation(h_ShaderProgram_TXPS, string);
		sprintf(string, "u_light[%d].light_attenuation_factors", i);
		loc_light[i].light_attenuation_factors = glGetUniformLocation(h_ShaderProgram_TXPS, string);
	}

	loc_material.ambient_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.ambient_color");
	loc_material.diffuse_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.diffuse_color");
	loc_material.specular_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.specular_color");
	loc_material.emissive_color = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.emissive_color");
	loc_material.specular_exponent = glGetUniformLocation(h_ShaderProgram_TXPS, "u_material.specular_exponent");

	loc_texture = glGetUniformLocation(h_ShaderProgram_TXPS, "u_base_texture");

	loc_flag_texture_mapping = glGetUniformLocation(h_ShaderProgram_TXPS, "u_flag_texture_mapping");
	loc_flag_fog = glGetUniformLocation(h_ShaderProgram_TXPS, "u_flag_fog");
}

void initialize_lights_and_material(void) { // follow OpenGL conventions for initialization
	int i;

	glUseProgram(h_ShaderProgram_TXPS);

	glUniform4f(loc_global_ambient_color, 0.115f, 0.115f, 0.115f, 1.0f);
	for (i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		glUniform1i(loc_light[i].light_on, 0); // turn off all lights initially
		glUniform4f(loc_light[i].position, 0.0f, 0.0f, 1.0f, 0.0f);
		glUniform4f(loc_light[i].ambient_color, 0.0f, 0.0f, 0.0f, 1.0f);
		if (i == 0) {
			glUniform4f(loc_light[i].diffuse_color, 1.0f, 1.0f, 1.0f, 1.0f);
			glUniform4f(loc_light[i].specular_color, 1.0f, 1.0f, 1.0f, 1.0f);
		}
		else {
			glUniform4f(loc_light[i].diffuse_color, 0.0f, 0.0f, 0.0f, 1.0f);
			glUniform4f(loc_light[i].specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
		}
		glUniform3f(loc_light[i].spot_direction, 0.0f, 0.0f, -1.0f);
		glUniform1f(loc_light[i].spot_exponent, 0.0f); // [0.0, 128.0]
		glUniform1f(loc_light[i].spot_cutoff_angle, 180.0f); // [0.0, 90.0] or 180.0 (180.0 for no spot light effect)
		glUniform4f(loc_light[i].light_attenuation_factors, 1.0f, 0.0f, 0.0f, 0.0f); // .w != 0.0f for no ligth attenuation
	}

	glUniform4f(loc_material.ambient_color, 0.2f, 0.2f, 0.2f, 1.0f);
	glUniform4f(loc_material.diffuse_color, 0.8f, 0.8f, 0.8f, 1.0f);
	glUniform4f(loc_material.specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform4f(loc_material.emissive_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform1f(loc_material.specular_exponent, 0.0f); // [0.0, 128.0]

	glUseProgram(0);
}

void initialize_flags(void) {
	isAnimationRun = 1;
	//flag_polygon_fill = 1;
	//flag_texture_mapping = 1;
//	flag_fog = 0;

	glUseProgram(h_ShaderProgram_TXPS);
	//glUniform1i(loc_flag_fog, flag_fog);
//	glUniform1i(loc_flag_texture_mapping, flag_texture_mapping);
	glUseProgram(0);
}

void initialize_OpenGL(void) {

	glEnable(GL_MULTISAMPLE);
  	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//ViewMatrix = glm::lookAt(PRP_distance_scale[0] * glm::vec3(500.0f, 300.0f, 500.0f),
	//	glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//ViewMatrix = glm::lookAt(4.0f/6.0f * glm::vec3(500.0f, 600.0f, 500.0f),
	//	glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ViewMatrix = glm::mat4(1.0f);
	ProjectionMatrix = glm::mat4(1.0f);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
	initialize_lights_and_material();
	initialize_flags();

	glGenTextures(N_TEXTURES_USED, texture_names);
}

void set_up_scene_lights(void) {
	// point_light_EC: use light 0
	light[0].light_on = 1;
	light[0].position[0] = 0.0f; light[0].position[1] = 100.0f; 	// point light position in EC
	light[0].position[2] = 0.0f; light[0].position[3] = 1.0f;

	light[0].ambient_color[0] = 0.13f; light[0].ambient_color[1] = 0.13f;
	light[0].ambient_color[2] = 0.13f; light[0].ambient_color[3] = 1.0f;

	light[0].diffuse_color[0] = 0.5f; light[0].diffuse_color[1] = 0.5f;
	light[0].diffuse_color[2] = 0.5f; light[0].diffuse_color[3] = 1.5f;

	light[0].specular_color[0] = 0.8f; light[0].specular_color[1] = 0.8f;
	light[0].specular_color[2] = 0.8f; light[0].specular_color[3] = 1.0f;

	// spot_light_WC: use light 1
	light[1].light_on = 1;
	light[1].position[0] = -200.0f; light[1].position[1] = 500.0f; // spot light position in WC
	light[1].position[2] = -200.0f; light[1].position[3] = 1.0f;

	light[1].ambient_color[0] = 0.152f; light[1].ambient_color[1] = 0.152f;
	light[1].ambient_color[2] = 0.152f; light[1].ambient_color[3] = 1.0f;

	light[1].diffuse_color[0] = 0.572f; light[1].diffuse_color[1] = 0.572f;
	light[1].diffuse_color[2] = 0.572f; light[1].diffuse_color[3] = 1.0f;

	light[1].specular_color[0] = 0.772f; light[1].specular_color[1] = 0.772f;
	light[1].specular_color[2] = 0.772f; light[1].specular_color[3] = 1.0f;

	light[1].spot_direction[0] = 0.0f; light[1].spot_direction[1] = -1.0f; // spot light direction in WC
	light[1].spot_direction[2] = 0.0f;
	light[1].spot_cutoff_angle = 20.0f;
	light[1].spot_exponent = 8.0f;

	glUseProgram(h_ShaderProgram_TXPS);
	glUniform1i(loc_light[0].light_on, light[0].light_on);
	glUniform4fv(loc_light[0].position, 1, light[0].position);
	glUniform4fv(loc_light[0].ambient_color, 1, light[0].ambient_color);
	glUniform4fv(loc_light[0].diffuse_color, 1, light[0].diffuse_color);
	glUniform4fv(loc_light[0].specular_color, 1, light[0].specular_color);

	glUniform1i(loc_light[1].light_on, light[1].light_on);
	// need to supply position in EC for shading
	glm::vec4 position_EC = ViewMatrix * glm::vec4(light[1].position[0], light[1].position[1],
												light[1].position[2], light[1].position[3]);
	glUniform4fv(loc_light[1].position, 1, &position_EC[0]); 
	glUniform4fv(loc_light[1].ambient_color, 1, light[1].ambient_color);
	glUniform4fv(loc_light[1].diffuse_color, 1, light[1].diffuse_color);
	glUniform4fv(loc_light[1].specular_color, 1, light[1].specular_color);
	// need to supply direction in EC for shading in this example shader
	// note that the viewing transform is a rigid body transform
	// thus transpose(inverse(mat3(ViewMatrix)) = mat3(ViewMatrix)
	glm::vec3 direction_EC = glm::mat3(ViewMatrix) * glm::vec3(light[1].spot_direction[0], light[1].spot_direction[1], 
																light[1].spot_direction[2]);
	glUniform3fv(loc_light[1].spot_direction, 1, &direction_EC[0]); 
	glUniform1f(loc_light[1].spot_cutoff_angle, light[1].spot_cutoff_angle);
	glUniform1f(loc_light[1].spot_exponent, light[1].spot_exponent);
	glUseProgram(0);
}

void prepare_scene(void) {
	prepare_axes();
	prepare_floor();
	prepare_tiger();
	prepare_wolf();
	prepare_spider();
	prepare_dragon();
	prepare_godzilla();
	prepare_bus();
	prepare_bike();
	prepare_tank();
	prepare_optimus();
	/*	prepare_ben();
	prepare_cow();
	prepare_ironman();
	*/
	set_up_scene_lights();
}

void initialize_renderer(void) {
	register_callbacks();
	prepare_shader_program();
	initialize_OpenGL();
	prepare_scene();
	initialize_camera(0);
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = GL_TRUE;

	error = glewInit();
	if (error != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(error));
		exit(-1);
	}
	fprintf(stdout, "*********************************************************\n");
	fprintf(stdout, " - GLEW version supported: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, " - OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, " - OpenGL version supported: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "*********************************************************\n\n");
}

void greetings(char *program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 1
void main(int argc, char *argv[]) {
	char program_name[64] = "Sogang CSE4170 3D Objects HW3 20151523 ±èµ¿Çö";
	char messages[N_MESSAGE_LINES][256] = { "    - Keys used: '1' ~ '5', 'w', 's', 'a', 'd', 'q', 'e', 'z', 'x', 'c', 'v', 'f', 'g', 'ESC'"  };

	glutInit(&argc, argv);
  	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(900, 900);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	fprintf(stdout, " - Cam choice : Cam#1 : numkey 1 / Cam#2 : numkey 2 / Cam#3 : numkey 3 / Cam#4 : numkey 4 / Cam#5 : numkey 0\n");
	fprintf(stdout, " - On Cam#1 : control by mouse motion during clickin left button\n");
	fprintf(stdout, " - On Cam#5 : moving 'w', 's', 'a', 'd', 'q', 'e'\n");
	fprintf(stdout, " - On Cam#5 : control by moving horizontally the mouse during clickin left button\n");
	fprintf(stdout, " - On Cam#5 : change parameter of mouse motion : 'f'\n");
	fprintf(stdout, " - On Cam#5 : zoom in/out : mouse wheel\n");
	fprintf(stdout, " - On all Cam : animation switch of tiger/wolf/spider/dragon/all : 'z'/'x'/'c'/'v'/'g'\n");
	glutMainLoop();
}