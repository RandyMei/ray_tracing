#include <iostream>

#include "sphere.hpp"
#include "histable_list.hpp"
#include "camera.hpp"
#include "util.hpp"

#include "materials/metal.hpp"
#include "materials/lambertian.hpp"

#define N 4

vec3 color(const ray &r, hitable *world, int depth) {
    hit_record rec;
    if (world->hit(r, 0.0001, MAXFLOAT, rec)) {
        ray scattered;
        vec3 attenuation;
        if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
            return attenuation * color(scattered, world, depth + 1);
        } else {
            return {0, 0, 0};
        }
    } else {
        vec3 unit_direction = unit_vector(r.direction());
        double t = 0.5 * (unit_direction.y() + 1.0);
        return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
    }
}

int main(int argc, char* argv[]) {
    int nx = 2000, ny = 1000, ns = 100;
    std::ofstream fout = get_out(argv);
    fout << "P3\n" << nx << " " << ny << "\n255\n";
    camera cam;
    hitable *list[N];
    list[0] = new sphere(vec3(0, 0, -1), 0.5, new lambertian(vec3(0.8, 0.3, 0.3)));
    list[1] = new sphere(vec3(0, -1000.5, -1), 1000, new lambertian(vec3(0.8, 0.8, 0.0)));
    list[2] = new sphere(vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.6, 0.2), 0.3));
    list[3] = new sphere(vec3(-1, 0, -1), 0.5, new metal(vec3(.8, 0.8, 0.8), 1.0));
    auto *world = new hitable_list(list, N);
    for (int j = ny - 1; j >= 0; j--) {
        for (int i = 0; i < nx; i++) {
            vec3 col(0, 0, 0);
            for (int s = 0; s < ns; ++s) {
                double u = double(i + drand48()) / double(nx);
                double v = double(j + drand48()) / double(ny);
                ray r = cam.get_ray(u, v);
                col += color(r, world, 0);
            }
            col /= double(ns);
            col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
            int ir = int(255.99 * col[0]);
            int ig = int(255.99 * col[1]);
            int ib = int(255.99 * col[2]);
            fout << ir << " " << ig << " " << ib << "\n";
        }
    }
}

