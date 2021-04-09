/*==================================================================================
*Assignment 2 
*===================================================================================
*/
#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include "Sphere.h"
#include "SceneObject.h"
#include "Ray.h"
#include <GL/freeglut.h>
#include "Plane.h"
#include "TextureBMP.h"
using namespace std;

TextureBMP eyeTexture;
const float WIDTH = 20.0;  
const float HEIGHT = 20.0;
const float EDIST = 40.0;
const int NUMDIV = 800;
const int MAX_STEPS = 5;
const float XMIN = -WIDTH * 0.5;
const float XMAX =  WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX =  HEIGHT * 0.5;
const bool AA = true; 

vector<SceneObject*> sceneObjects;


//---The most important function in a ray tracer! ---------------------------------- 
//   Computes the colour value obtained by tracing a ray and finding its 
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step)
{
	glm::vec3 backgroundCol(0);						//Background colour = (0,0,0)
	glm::vec3 lightPos(7, 20, -10);					//Light's position
	glm::vec3 color(0);
	SceneObject* obj;

    ray.closestPt(sceneObjects);					//Compare the ray with all objects in the scene
    if(ray.index == -1) return backgroundCol;		//no intersection
	obj = sceneObjects[ray.index];					//object on which the closest point of intersection is found



	//floor pattern
	if (ray.index == 3) {
		int stripeWidth = 5;
		int ix = ((ray.hit.x) / stripeWidth) + 50 ;
		int iz = (ray.hit.z) / stripeWidth;
		int k = (iz + ix) % 2;
		if (k != 0) color = glm::vec3(0.05, 0.3, 0.05);
		else color = glm::vec3(0.2, 1, 0.2);
		obj->setColor(color);
		

		
	}

	
	//stripe pattern
	if (ray.index == 4) {
		int stripeWidth = 1;
		int ix = ((ray.hit.x) / stripeWidth) + 50 ;
		int iy = ((ray.hit.y) / stripeWidth) + 50;
		int k = (iy + ix) % 3;
		if (k != 0) color = glm::vec3(0.05, 0.3, 0.05);
		else color = glm::vec3(0.2, 1, 0.2);
		obj->setColor(color);
		

		
	}

	//the eye in the bg reflection
	if (ray.index == 5) {
		float texCoords = (ray.hit.x - -100)/(100 -(-100));
		float texCoordt = (ray.hit.y - -30)/(135 - -30);
		if(texCoords > 0 && texCoords < 1 &&
			texCoordt > 0 && texCoordt <1)
		{
			color=eyeTexture.getColorAt(texCoords, texCoordt);
			obj->setColor(color);
		}
		

		
	}

	color = obj->lighting(lightPos, -ray.dir, ray.hit);
	glm::vec3 lightVec = lightPos - ray.hit;
	Ray shadowRay(ray.hit, lightVec);


	float lightDist = glm::length(lightVec);
	shadowRay.closestPt(sceneObjects);



	if (shadowRay.index > -1 && shadowRay.dist < lightDist) {
		if (shadowRay.index != 2 && shadowRay.index != 1) {
			color = 0.2f * obj->getColor();
		}
		else {
			color = glm::vec3(0.45f) * obj->getColor();
		}
	}

	if (obj->isReflective() && step < MAX_STEPS) 
	{
		float rho = obj->getReflectionCoeff();
		glm::vec3 normalVec = obj->normal(ray.hit);
		glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVec);
		Ray reflectedRay(ray.hit, reflectedDir);
		glm::vec3 reflectedColor = trace(reflectedRay, step +1);
		color = color + (rho * reflectedColor);
	}

	//transparent object
	if (ray.index == 2) {
		Ray transparentRay(ray.hit, ray.dir);
		glm::vec3 transparentColour = trace(transparentRay, 1);
		color = color + (0.9f * transparentColour);
	}

	
	if (ray.index == 1) {
		Ray transparentRay(ray.hit, ray.dir);
		glm::vec3 transparentColour = trace(transparentRay, 1);
		color = color + (0.9f * transparentColour);
	}


	return color;
}
//---------------------------------------------------------------------------------------
// Draws a tetrahedron
//---------------------------------------------------------------------------------------
void tetrahedron(float x, float y, float z, float size)
{
	glm::vec3 a(x, y, z+(size * sqrt(3.0)));
	glm::vec3 b(x+(size * sqrt(3.0)), y, z - (size * sqrt(3.0)));
 	glm::vec3 c(x-(size * sqrt(3.0)), y, z - (size * sqrt(3.0)));
 	glm::vec3 d(x, y+10, z);
 
	Plane *triangle1 = new Plane(a, b, c);
	triangle1->setColor(glm::vec3(1, 1, 1));   
	triangle1->setShininess(4.0);

	Plane *triangle2 = new Plane(b, c, d);
	triangle2->setColor(glm::vec3(1, 0, 0));  
	triangle2->setShininess(4.0);

	Plane *triangle3 = new Plane(c, d, a);
	triangle3->setColor(glm::vec3(0, 1, 0));   
	triangle3->setShininess(4.0);

	Plane *triangle4 = new Plane(d, a, b);
	triangle4->setColor(glm::vec3(0, 0, 1));   
	triangle4->setShininess(4.0);

	sceneObjects.push_back(triangle1);
	sceneObjects.push_back(triangle2);
	sceneObjects.push_back(triangle3);
	sceneObjects.push_back(triangle4);


}

void cube(float x, float y, float z, float size) 
{
	glm::vec3 S1 = glm::vec3(x - ( size / 2), y - ( size / 2), z - ( size / 2));
	glm::vec3 S2 = glm::vec3(x + ( size / 2), y - ( size / 2), z - ( size / 2));
	glm::vec3 S3 = glm::vec3(x + ( size / 2), y + ( size / 2), z - ( size / 2));
	glm::vec3 S4 = glm::vec3(x - ( size / 2), y + ( size / 2), z - ( size / 2));
	glm::vec3 S5 = glm::vec3(x + ( size / 2), y - ( size / 2), z + ( size / 2));
	glm::vec3 S6 = glm::vec3(x + ( size / 2), y + ( size / 2), z + ( size / 2));
	glm::vec3 S7 = glm::vec3(x - ( size / 2), y + ( size / 2), z + ( size / 2));
	glm::vec3 S8 = glm::vec3(x - ( size / 2), y - ( size / 2), z + ( size / 2));

	Plane *facea = new Plane(S1,S2,S3,S4);
	facea->setColor(glm::vec3(0, 1, 0));   

	Plane *faceb = new Plane(S2,S5,S6,S3);
	faceb->setColor(glm::vec3(0, 0, 1));

	Plane *facec = new Plane(S5,S8,S7,S6);
	facec->setColor(glm::vec3(1, 0, 0));
	
	Plane *faced = new Plane(S4,S7,S8,S1);
	faced->setColor(glm::vec3(1, 0, 1));

	Plane *facee = new Plane(S4,S3,S6,S7);
	facee->setColor(glm::vec3(0, 1, 1));
	
	Plane *facef = new Plane(S8,S5,S2,S1);
	facef->setColor(glm::vec3(1, 1, 0));                                    
											          
	sceneObjects.push_back(facea);
	sceneObjects.push_back(faceb);
	sceneObjects.push_back(facec);
	sceneObjects.push_back(faced);
	sceneObjects.push_back(facee);
	sceneObjects.push_back(facef);
}

//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display()
{
	float xp, yp;  //grid point
	float cellX = (XMAX-XMIN)/NUMDIV;  //cell width
	float cellY = (YMAX-YMIN)/NUMDIV;  //cell height
	glm::vec3 eye(0., 0., 0.);

	glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glBegin(GL_QUADS);  //Each cell is a tiny quad.

	for(int i = 0; i < NUMDIV; i++)	//Scan every cell of the image plane
	{
		xp = XMIN + i*cellX;
		for(int j = 0; j < NUMDIV; j++)
		{
			yp = YMIN + j*cellY;

		    glm::vec3 dir(xp+0.5*cellX, yp+0.5*cellY, -EDIST);	//direction of the primary ray
            glm::vec3 col;

			// Check if AntiAliasing is on
		    if (AA) {
                glm::vec3 dir0(xp+0.25*cellX, yp+0.25*cellY, -EDIST);
                glm::vec3 dir1(xp+0.75*cellX, yp+0.25*cellY, -EDIST);
                glm::vec3 dir2(xp+0.25*cellX, yp+0.75*cellY, -EDIST);
                glm::vec3 dir3(xp+0.75*cellX, yp+0.75*cellY, -EDIST);

                Ray ray0 = Ray(eye, dir0);
                Ray ray1 = Ray(eye, dir1);
                Ray ray2 = Ray(eye, dir2);
                Ray ray3 = Ray(eye, dir3);

				ray0.dir = glm::normalize(ray0.dir);
				ray1.dir = glm::normalize(ray1.dir);
				ray2.dir = glm::normalize(ray2.dir);
				ray3.dir = glm::normalize(ray3.dir);


                glm::vec3 col0;
                glm::vec3 col1;
                glm::vec3 col2;
                glm::vec3 col3;

                col0 = trace(ray0, 1);
                col1 = trace(ray1, 1);
                col2 = trace(ray2, 1);
                col3 = trace(ray3, 1);

                col = (col0 + col1 + col2 + col3) * 0.25f;

		    } else {

				Ray ray = Ray(eye, dir);
				ray.dir = glm::normalize(dir);
				col = trace (ray, 1); //Trace the primary ray and get the colour value
			}
			glColor3f(col.r, col.g, col.b);
			glVertex2f(xp, yp);				//Draw each cell with its color value
			glVertex2f(xp+cellX, yp);
			glVertex2f(xp+cellX, yp+cellY);
			glVertex2f(xp, yp+cellY);
			}
        }
    

    glEnd();
    glFlush();
}



//---This function initializes the scene ------------------------------------------- 
//   Specifically, it creates scene objects (spheres, planes, cones, cylinders etc)
//     and add them to the list of scene objects.
//   It also initializes the OpenGL orthographc projection matrix for drawing the
//     the ray traced image.
//----------------------------------------------------------------------------------
void initialize()
{
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);

    glClearColor(0, 0, 0, 1);

eyeTexture = TextureBMP("eye.bmp"); 

//object 0
Sphere *sphere1 = new Sphere(glm::vec3(0.0, 10.0, -100.0), 12.0);
sphere1->setColor(glm::vec3(0, 0, 0));   
sphere1->setReflectivity(true, 0.8);
sceneObjects.push_back(sphere1);		 

//object 1
Sphere *sphere2 = new Sphere(glm::vec3(15.0, 5.0, -90.0), 4.0);
sphere2->setColor(glm::vec3(0.3, 0, 0));   
sceneObjects.push_back(sphere2);		 

//object 2
Sphere *sphere3 = new Sphere(glm::vec3(-15.0, 5.0, -90.0), 4.0);
sphere3->setColor(glm::vec3(0, 0, 0.3));   
sceneObjects.push_back(sphere3);		 



//object 3
Plane *plane = new Plane (glm::vec3(-60., -15, -40),
						  glm::vec3(60., -15, -40),
						  glm::vec3(60., -15, -200),
						  glm::vec3(-60., -15, -200));
plane->setSpecularity(false);
sceneObjects.push_back(plane);

//object 4
Plane *plane2 = new Plane (glm::vec3(-80., -25, -200),
						  glm::vec3(80., -25, -200),
						  glm::vec3(80., 100, -200),
						  glm::vec3(-80., 100, -200));
plane2->setSpecularity(false);
sceneObjects.push_back(plane2);

//object 5
Plane *plane3 = new Plane (glm::vec3(-40., -55, 10),
						  glm::vec3(40., -55, 10),
						  glm::vec3(40., 65, 10),
						  glm::vec3(-40., 65, 10));
plane3->setColor(glm::vec3(0.5, 1, 0));   //Set colour 
plane3->setSpecularity(false);
sceneObjects.push_back(plane3);

//object 6, 7, 8, 9
tetrahedron(10, -15, -80, 4);

//object 10, 11, 12, 13,
tetrahedron(-10, -15, -80, 4);

//object 14 15 16 17 18 19
cube(10, 15, -90, 3);

//object 20 - 25
cube(-10, 15, -90, 3);

//object 26 - 29
tetrahedron(0, -15, -130, -3);


}


int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Raytracing");

    glutDisplayFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}
