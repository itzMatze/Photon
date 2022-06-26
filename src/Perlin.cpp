#include "Perlin.h"

#include <algorithm>
#include <random>
#include <vector>

    Perlin::Perlin() : ranvec(std::vector<glm::vec3>(256)), perm_x(std::vector<int>(256)), perm_y(std::vector<int>(256)), perm_z(std::vector<int>(256))
    {
        std::uniform_real_distribution dis(0.0f, 1.0f);
        std::mt19937 gen(0);
        std::generate(ranvec.begin(), ranvec.end(), [&](){ 
            return glm::vec3(dis(gen), dis(gen), dis(gen)) * 2.0f - glm::vec3(1.0f, 1.0f, 1.0f); 
            });
        auto perlin_gen_perm = [](std::vector<int>& a){
            int n = 0;
            std::generate(a.begin(), a.end(), [&](){ return n++; });
            std::uniform_real_distribution dis(0.0f, 1.0f);
            std::mt19937 gen(0);
            for (int i = n - 1; i > 0; --i)
            {
                int target = static_cast<int>(dis(gen) * (i + 1));
                std::swap(a[i], a[target]);
            }
        };
        perlin_gen_perm(perm_x);
        perlin_gen_perm(perm_y);
        perlin_gen_perm(perm_z);
    }

    float Perlin::noise(const glm::vec3& p) const
    {
        int i = static_cast<int>(4 * p.x) & 255;
        int j = static_cast<int>(4 * p.y) & 255;
        int k = static_cast<int>(4 * p.z) & 255;
        return ranvec[perm_x[i] /*^ perm_y[j]*/ ^ perm_z[k]].x;
    }

    inline float interp(glm::vec3 c[2][2][2], float u, float v, float w)
    {
        float uu = u * u * (3 - 2 * u);
        float vv = v * v * (3 - 2 * v);
        float ww = w * w * (3 - 2 * w);
        float accum = 0.0f;
        for (int i = 0; i < 2; ++i)
        {
            for (int j = 0; j < 2; ++j)
            {
                for (int k = 0; k < 2; ++k)
                {
                    glm::vec3 weight(u - i, v - j, w - k);
                    accum += (i*uu + (1-i)*(1-uu)) *
                        (j*vv + (1-j)*(1-vv)) *
                        (k*ww + (1-k)*(1-ww)) * 
                        glm::dot(c[i][j][k], weight);
                }
            }
        }
        return accum;
    }

    float Perlin::smooth_noise(const glm::vec3& p) const
    {
        float u = p.x - std::floor(p.x);
        float v = p.y - std::floor(p.y);
        float w = p.z - std::floor(p.z);
        int i = std::floor(p.x);
        int j = std::floor(p.y);
        int k = std::floor(p.z);
        glm::vec3 c[2][2][2];
        for (int di = 0; di < 2; ++di)
        {
            for (int dj = 0; dj < 2; ++dj)
            {
                for (int dk = 0; dk < 2; ++dk)
                {
                     c[di][dj][dk] = ranvec[
                            perm_x[(i+di) & 255] ^
                            perm_y[(j+dj) & 255] ^
                            perm_z[(k+dk) & 255]
                        ];
                }
            }
        }
        return interp(c, u, v, w);
    }

    float Perlin::turb(const glm::vec3& p, int depth) const
    {
        float accum = 0.0f;
        glm::vec3 temp_p = p;
        float weight = 1.0f;
        for (int i = 0; i < depth; ++i)
        {
            accum += weight * smooth_noise(temp_p);
            weight *= 0.5f;
            temp_p *= 2.0f;
        }
        return glm::abs(accum);
    }
