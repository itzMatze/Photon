#include "util/vec3.hpp"

glm::vec3 rgb_to_xyz(const glm::vec3& rgb);
glm::vec3 xyz_to_rgb(const glm::vec3& xyz);
// wavelength in nanometer
glm::vec3 wavelength_to_xyz(uint wavelength);
glm::vec3 wavelength_to_rgb(uint wavelength);
uint32_t get_random_wavelength(float random);
glm::vec3 get_inv_wavelength_probability();
