#include <iostream>
#include <fstream>
#include "sphere.h"
#include "moving_sphere.h"
#include "hitable_list.h"
#include "float.h"
#include "camera.h"
#include "material.h"
#include <chrono>
#include <iomanip> // put_time
#include <sstream> // stringstream
#include "rayhelpers.h"
#include "bvh_node.h"
#include "texture.h"
#include "aarect.h"
#include "box.h"
#include "hitable.h"
#include "constant_medium.h"

#define STB_IMAGE_IMPLEMENTATION 
#include "stb_image.h"

hitable *random_scene(LookAtValues& LookValues)
{
	int n = 50000;
	
	hitable **list = new hitable*[n + 1];

	texture* green = new constant_texture(vec3(0.2, 0.3, 0.1));
	texture* white = new constant_texture(vec3(0.9, 0.9, 0.9));
	texture* red = new constant_texture(vec3(0.9, 0.0, 0.0));

	texture* checker = new checker_texture(green, white);

	int nx, ny, nn;
	unsigned char *tex_data = stbi_load("earthmap.jpg", &nx, &ny, &nn, 0);

	texture* imgText = new image_texture(tex_data, nx, ny);
	material *imgMat = new lambertian(imgText);
	
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(checker));
	
	int i = 1;
	
	for (int a = -10; a < 10; a++) 
	{
		for (int b = -10; b < 10; b++) 
		{
			float choose_mat = drand48();
			vec3 center(a + 0.9*drand48(), 0.2, b + 0.9*drand48());
			if ((center - vec3(4, 0.2, 0)).length() > 0.9) 
			{
				if (choose_mat < 0.6) // diffuse moving
				{
					texture* colorText = new constant_texture(vec3(drand48()*drand48(), drand48()*drand48(), drand48()*drand48()));
					material *colormat = new lambertian(colorText);

					list[i++] = new sphere(center, 0.1, colormat);
				}
				else if (choose_mat < 0.8) // diffuse Globes
				{  
					list[i++] = new sphere(center, 0.2, imgMat);
				}
				else if (choose_mat < 0.95) // metal
				{ 
					list[i++] = new sphere(center, 0.2,
						new metal(vec3(0.5*(1 + drand48()), 0.5*(1 + drand48()), 0.5*(1 + drand48())), 0.5*drand48()));
				}
				else // glass
				{  
					list[i++] = new sphere(center, 0.2, new dielectric(1.5));
				}
			}
		}
	}

	list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
	list[i++] = new sphere(vec3(-4, 1, 0), 1.0, imgMat);
	list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));
	list[i++] = new sphere(vec3(-8, 1, 0), 1.0, new lambertian(red));

	//return new hitable_list(list,i);
	return new bvh_node(list, i, 0.0, 1.0);
}

hitable *basic_scene(LookAtValues& LookValues)
{
	texture* blueish = new constant_texture(vec3(0.1, 0.2, 0.5));
	texture* yellow = new constant_texture(vec3(0.8, 0.8, 0.0));
	texture* green = new constant_texture(vec3(0.2, 0.8, 0.2));

	hitable *list[5];
	float R = cos(M_PI / 4);
	list[0] = new sphere(vec3(0, 0, -1), 0.5, new lambertian(blueish));
	list[1] = new sphere(vec3(0, -100.5, -1), 100, new lambertian(yellow));
	list[2] = new sphere(vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.6, 0.2), 0.0));
	list[3] = new sphere(vec3(-1, 0, -1), 0.5, new dielectric(1.5));
	list[4] = new sphere(vec3(-1, 0, -1), -0.45, new dielectric(1.5));
	return new hitable_list(list, 5);
}

hitable *two_perlin_spheres(LookAtValues& LookValues)
{
	texture* pertext = new noise_texture(5);
	texture* green = new constant_texture(vec3(0.2, 0.3, 0.1));
	texture* white = new constant_texture(vec3(0.9, 0.9, 0.9));
	texture* checker = new checker_texture(green, white);

	hitable **list = new hitable*[2];

	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(pertext));
	list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(checker));

	return new hitable_list(list, 2);
}

hitable *two_textured_spheres(LookAtValues& LookValues)
{
	int nx, ny, nn; 
	unsigned char *tex_data = stbi_load("earthmap.jpg", &nx, &ny, &nn, 0); 
	
	texture* imgText = new image_texture(tex_data, nx, ny);
	material *mat = new lambertian(imgText);

	texture* green = new constant_texture(vec3(0.2, 0.3, 0.1));
	texture* white = new constant_texture(vec3(0.9, 0.9, 0.9));
	texture* checker = new checker_texture(green, white);

	hitable **list = new hitable*[2];

	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(checker));
	list[1] = new sphere(vec3(0, 2, 0), 2, mat);

	return new hitable_list(list, 2);
}

hitable* simple_light(LookAtValues& LookValues)
{
	texture* pertext = new noise_texture(4);
	hitable** list = new hitable*[4];

	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(pertext));
	list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext));
	list[2] = new sphere(vec3(0, 7, 0), 2, new diffuse_light(new constant_texture(vec3(4, 4, 4))));
	list[3] = new xy_rect(3, 5, 1, 3, -2, new diffuse_light(new constant_texture(vec3(4, 4, 4))));

	return new hitable_list(list, 4);
}

hitable* Box_light(LookAtValues& LookValues)
{
	texture* pertext = new noise_texture(8);
	hitable** list = new hitable*[8];

	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(pertext));
	list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext));
	list[2] = new sphere(vec3(0, 7, 0), 2, new diffuse_light(new constant_texture(vec3(4, 4, 4))));
	list[3] = new xy_rect(3, 5, 1, 3, -2, new diffuse_light(new constant_texture(vec3(0, 4, 4))));
	list[4] = new xy_rect(3, 5, 1, 3, 2, new diffuse_light(new constant_texture(vec3(0, 4, 4))));
	list[5] = new xz_rect(3, 5, 1, 3, -2, new diffuse_light(new constant_texture(vec3(0, 4, 4))));
	list[6] = new xz_rect(3, 5, 1, 3, 2, new diffuse_light(new constant_texture(vec3(4, 4, 0))));
	list[7] = new yz_rect(3, 5, 1, 3, -2, new diffuse_light(new constant_texture(vec3(4, 4, 0))));
	list[8] = new yz_rect(3, 5, 1, 3, 2, new diffuse_light(new constant_texture(vec3(4, 4, 4))));

	return new hitable_list(list, 8);
}

hitable *cornell_box(LookAtValues& LookValues) {
	hitable **list = new hitable*[8];
	int i = 0;
	material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
	material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
	material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
	material *light = new diffuse_light(new constant_texture(vec3(15, 15, 15)));
	list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
	list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
	list[i++] = new xz_rect(213, 343, 227, 332, 554, light);
	list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
	list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
	list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
	list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18), vec3(130, 0, 65));
	list[i++] = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white), 15), vec3(265, 0, 295));

	LookValues.lookfrom = vec3(278, 278, -800);
	LookValues.lookat = vec3(278, 278, 0);

	return new hitable_list(list, i);
}

hitable *cornell_smoke(LookAtValues& LookValues) {
	hitable **list = new hitable*[8];
	int i = 0;
	material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
	material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
	material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
	material *light = new diffuse_light(new constant_texture(vec3(7, 7, 7)));
	list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
	list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
	list[i++] = new xz_rect(113, 443, 127, 432, 554, light);
	list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
	list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
	list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
	hitable *b1 = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18), vec3(130, 0, 65));
	hitable *b2 = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white), 15), vec3(265, 0, 295));
	list[i++] = new constant_medium(b1, 0.01, new constant_texture(vec3(1.0, 1.0, 1.0)));
	list[i++] = new constant_medium(b2, 0.01, new constant_texture(vec3(0.0, 0.0, 0.0)));

	LookValues.lookfrom = vec3(278, 278, -800);
	LookValues.lookat = vec3(278, 278, 0);

	return new hitable_list(list, i);
}

hitable *cornell_final(LookAtValues& LookValues) {
	hitable **list = new hitable*[30];
	hitable **boxlist = new hitable*[10000];
	texture *pertext = new noise_texture(0.1);
	int nx, ny, nn;
	unsigned char *tex_data = stbi_load("earthmap.jpg", &nx, &ny, &nn, 0);
	material *mat = new lambertian(new image_texture(tex_data, nx, ny));
	int i = 0;
	material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
	material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
	material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
	material *light = new diffuse_light(new constant_texture(vec3(7, 7, 7)));
	//list[i++] = new sphere(vec3(260, 50, 145), 50,mat);
	list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
	list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
	list[i++] = new xz_rect(123, 423, 147, 412, 554, light);
	list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
	list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
	list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));
	
	hitable *boundary = new sphere(vec3(160, 50, 345), 50, new dielectric(1.5));
	list[i++] = boundary;
	list[i++] = new constant_medium(boundary, 0.2, new constant_texture(vec3(0.2, 0.4, 0.9)));
	list[i++] = new sphere(vec3(460, 50, 105), 50, new dielectric(1.5));
	list[i++] = new sphere(vec3(120, 50, 205), 50, new lambertian(pertext));
	int ns = 10000;
	for (int j = 0; j < ns; j++) {
	boxlist[j] = new sphere(vec3(165*drand48(), 330*drand48(), 165*drand48()), 10, white);
	}
	list[i++] =   new translate(new rotate_y(new bvh_node(boxlist,ns, 0.0, 1.0), 15), vec3(265,0,295));
	
	hitable *boundary2 = new translate(new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), new dielectric(1.5)), -18), vec3(130, 0, 65));
	list[i++] = boundary2;
	list[i++] = new constant_medium(boundary2, 0.2, new constant_texture(vec3(0.9, 0.9, 0.9)));

	LookValues.lookfrom = vec3(278, 278, -800);
	LookValues.lookat = vec3(278, 278, 0);

	return new hitable_list(list, i);
}

hitable *final(LookAtValues& LookValues) {
	int nb = 20;
	hitable **list = new hitable*[30];
	hitable **boxlist = new hitable*[10000];
	hitable **boxlist2 = new hitable*[10000];
	material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
	material *ground = new lambertian(new constant_texture(vec3(0.48, 0.83, 0.53)));
	int b = 0;
	for (int i = 0; i < nb; i++) {
		for (int j = 0; j < nb; j++) {
			float w = 100;
			float x0 = -1000 + i * w;
			float z0 = -1000 + j * w;
			float y0 = 0;
			float x1 = x0 + w;
			float y1 = 100 * (drand48() + 0.01);
			float z1 = z0 + w;
			boxlist[b++] = new box(vec3(x0, y0, z0), vec3(x1, y1, z1), ground);
		}
	}
	int l = 0;
	list[l++] = new bvh_node(boxlist, b, 0, 1);
	material *light = new diffuse_light(new constant_texture(vec3(7, 7, 7)));
	list[l++] = new xz_rect(123, 423, 147, 412, 554, light);
	vec3 center(400, 400, 200);
	list[l++] = new moving_sphere(center, center + vec3(30, 0, 0), 0, 1, 50, new lambertian(new constant_texture(vec3(0.7, 0.3, 0.1))));
	list[l++] = new sphere(vec3(260, 150, 45), 50, new dielectric(1.5));
	list[l++] = new sphere(vec3(0, 150, 145), 50, new metal(vec3(0.8, 0.8, 0.9), 10.0));
	hitable *boundary = new sphere(vec3(360, 150, 145), 70, new dielectric(1.5));
	list[l++] = boundary;
	list[l++] = new constant_medium(boundary, 0.2, new constant_texture(vec3(0.2, 0.4, 0.9)));
	boundary = new sphere(vec3(0, 0, 0), 5000, new dielectric(1.5));
	list[l++] = new constant_medium(boundary, 0.0001, new constant_texture(vec3(1.0, 1.0, 1.0)));
	int nx, ny, nn;
	unsigned char *tex_data = stbi_load("earthmap.jpg", &nx, &ny, &nn, 0);
	material *emat = new lambertian(new image_texture(tex_data, nx, ny));
	list[l++] = new sphere(vec3(400, 200, 400), 100, emat);
	texture *pertext = new noise_texture(0.1);
	list[l++] = new sphere(vec3(220, 280, 300), 80, new lambertian(pertext));
	int ns = 1000;
	for (int j = 0; j < ns; j++) {
		boxlist2[j] = new sphere(vec3(165 * drand48(), 165 * drand48(), 165 * drand48()), 10, white);
	}
	list[l++] = new translate(new rotate_y(new bvh_node(boxlist2, ns, 0.0, 1.0), 15), vec3(-100, 270, 395));

	LookValues.lookfrom = vec3(478, 278, -600);
	LookValues.lookat = vec3(278, 278, 0);
	
	return new hitable_list(list, l);
}

std::string GetTimeAsString()
{
	std::stringstream ss;
	auto n = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(n);
	std::tm buf;
	localtime_s(&buf, &in_time_t);
	ss << std::put_time(&buf, "%Y-%m-%d(%H_%M)");

	return ss.str();
}

vec3 color(const ray& r, hitable *world, int depth)
{
	hit_record rec;
	if (world->hit(r, 0.001, MAXFLOAT, rec))
	{
		ray scattered;
		vec3 attenuation;
		vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

		if (depth < 5 && rec.mat_ptr->scatter(r, rec, attenuation, scattered))
		{
			return emitted + attenuation * color(scattered, world, depth + 1);
		}
		else
		{
			return emitted;
		}
	}
	else
	{
		return vec3(0, 0, 0);
		//vec3 unit_direction = unit_vector(r.direction());
		//float t = 0.5*(unit_direction.y() + 1.0);
		//return (1.0 - t)*vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
	}
}

int main() 
{
	std::ofstream myfile;
	std::string filename = GetTimeAsString() + ".ppm";

	std::cout << "Writing to: " << filename << std::endl;
	myfile.open(filename);

	//int nx = 4096;
	//int ny = 2160;

	//int nx = 1920;
	//int ny = 1080;

	//int nx = 1200;
	//int ny = 720;

	int nx = 512;
	int ny = 296;

	int ns = 2000;

	myfile << "P3\n" << nx << " " << ny << "\n255\n";
	
	auto StartTime = std::chrono::high_resolution_clock::now();

	LookAtValues LookValues;
	
	//hitable* world = two_perlin_spheres(LookValues);
	//hitable* world = two_textured_spheres(LookValues);
	//hitable* world = random_scene(LookValues);
	//hitable* world = basic_scene(LookValues);
	//hitable* world = simple_light(LookValues);
	//hitable* world = cornell_box(LookValues);
	//hitable* world = cornell_smoke(LookValues);
	//hitable* world = Box_light(LookValues);
	//hitable* world = cornell_final(LookValues);

	hitable* world = final(LookValues);

	auto SceneSetupComplete = std::chrono::high_resolution_clock::now();
	auto PercentStartTime = SceneSetupComplete;

	std::chrono::duration<double> SceneSetupTime = SceneSetupComplete - StartTime;
	std::cout << "SceneSetupTime: " << SceneSetupTime.count() << std::endl;

	//vec3 lookfrom(278,278,-800);
	//vec3 lookat(278,278,0);
	//float dist_to_focus = 10.0;
	//float aperture = 0.0;
	//float vfov = 40.0;

	
	camera cam(LookValues.lookfrom, LookValues.lookat, vec3(0, 1, 0), LookValues.vfov, float(nx) / float(ny),
		LookValues.aperture, LookValues.dist_to_focus, 0.0, 1.0);

	int pixelNum = 0;
	int pixelCount = nx * ny;
	int LastPercent = 0;

	for (int j = ny - 1; j >= 0; j--) 
	{
		for (int i = 0; i < nx; i++) 
		{
			vec3 col(0, 0, 0);
			for (int s = 0; s < ns; s++) 
			{
				float u = float(i + drand48()) / float(nx);
				float v = float(j + drand48()) / float(ny);
				ray r = cam.get_ray(u, v);
				vec3 p = r.point_at_parameter(2.0);
				col += color(r, world, 0);
			}

			col /= float(ns);
			col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
			int ir = int(255.99*col[0]);
			int ig = int(255.99*col[1]);
			int ib = int(255.99*col[2]);
			myfile << ir << " " << ig << " " << ib << "\n";
			
			pixelNum++;

			int newPercent = (pixelNum * 100) / pixelCount;
			if (newPercent > LastPercent)
			{
				auto PercentTime = std::chrono::high_resolution_clock::now();
				std::chrono::duration<double> PercentDuration = PercentTime - PercentStartTime;
				PercentStartTime = PercentTime;

				float EstimatedTimeRemaining = (100 - newPercent) * PercentDuration.count();

				std::cout << pixelNum << "/" << pixelCount <<  " (" << newPercent << "%)" << " PT: " << PercentDuration.count() << " RT: " << EstimatedTimeRemaining << std::endl;
				LastPercent = newPercent;
			}
		}
	}

	auto EndTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> TotalDuration = EndTime - StartTime;

	std::cout << "Total Time: " << TotalDuration.count() << std::endl;

	myfile.close();

	return 0;
}