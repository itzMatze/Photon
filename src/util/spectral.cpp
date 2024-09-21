#include "util/spectral.hpp"
#include "glm/mat3x3.hpp"

constexpr glm::vec3 cie_colour_match[81] = {
  glm::vec3(0.0014f,0.0000f,0.0065f), glm::vec3(0.0022f,0.0001f,0.0105f), glm::vec3(0.0042f,0.0001f,0.0201f),
  glm::vec3(0.0076f,0.0002f,0.0362f), glm::vec3(0.0143f,0.0004f,0.0679f), glm::vec3(0.0232f,0.0006f,0.1102f),
  glm::vec3(0.0435f,0.0012f,0.2074f), glm::vec3(0.0776f,0.0022f,0.3713f), glm::vec3(0.1344f,0.0040f,0.6456f),
  glm::vec3(0.2148f,0.0073f,1.0391f), glm::vec3(0.2839f,0.0116f,1.3856f), glm::vec3(0.3285f,0.0168f,1.6230f),
  glm::vec3(0.3483f,0.0230f,1.7471f), glm::vec3(0.3481f,0.0298f,1.7826f), glm::vec3(0.3362f,0.0380f,1.7721f),
  glm::vec3(0.3187f,0.0480f,1.7441f), glm::vec3(0.2908f,0.0600f,1.6692f), glm::vec3(0.2511f,0.0739f,1.5281f),
  glm::vec3(0.1954f,0.0910f,1.2876f), glm::vec3(0.1421f,0.1126f,1.0419f), glm::vec3(0.0956f,0.1390f,0.8130f),
  glm::vec3(0.0580f,0.1693f,0.6162f), glm::vec3(0.0320f,0.2080f,0.4652f), glm::vec3(0.0147f,0.2586f,0.3533f),
  glm::vec3(0.0049f,0.3230f,0.2720f), glm::vec3(0.0024f,0.4073f,0.2123f), glm::vec3(0.0093f,0.5030f,0.1582f),
  glm::vec3(0.0291f,0.6082f,0.1117f), glm::vec3(0.0633f,0.7100f,0.0782f), glm::vec3(0.1096f,0.7932f,0.0573f),
  glm::vec3(0.1655f,0.8620f,0.0422f), glm::vec3(0.2257f,0.9149f,0.0298f), glm::vec3(0.2904f,0.9540f,0.0203f),
  glm::vec3(0.3597f,0.9803f,0.0134f), glm::vec3(0.4334f,0.9950f,0.0087f), glm::vec3(0.5121f,1.0000f,0.0057f),
  glm::vec3(0.5945f,0.9950f,0.0039f), glm::vec3(0.6784f,0.9786f,0.0027f), glm::vec3(0.7621f,0.9520f,0.0021f),
  glm::vec3(0.8425f,0.9154f,0.0018f), glm::vec3(0.9163f,0.8700f,0.0017f), glm::vec3(0.9786f,0.8163f,0.0014f),
  glm::vec3(1.0263f,0.7570f,0.0011f), glm::vec3(1.0567f,0.6949f,0.0010f), glm::vec3(1.0622f,0.6310f,0.0008f),
  glm::vec3(1.0456f,0.5668f,0.0006f), glm::vec3(1.0026f,0.5030f,0.0003f), glm::vec3(0.9384f,0.4412f,0.0002f),
  glm::vec3(0.8544f,0.3810f,0.0002f), glm::vec3(0.7514f,0.3210f,0.0001f), glm::vec3(0.6424f,0.2650f,0.0000f),
  glm::vec3(0.5419f,0.2170f,0.0000f), glm::vec3(0.4479f,0.1750f,0.0000f), glm::vec3(0.3608f,0.1382f,0.0000f),
  glm::vec3(0.2835f,0.1070f,0.0000f), glm::vec3(0.2187f,0.0816f,0.0000f), glm::vec3(0.1649f,0.0610f,0.0000f),
  glm::vec3(0.1212f,0.0446f,0.0000f), glm::vec3(0.0874f,0.0320f,0.0000f), glm::vec3(0.0636f,0.0232f,0.0000f),
  glm::vec3(0.0468f,0.0170f,0.0000f), glm::vec3(0.0329f,0.0119f,0.0000f), glm::vec3(0.0227f,0.0082f,0.0000f),
  glm::vec3(0.0158f,0.0057f,0.0000f), glm::vec3(0.0114f,0.0041f,0.0000f), glm::vec3(0.0081f,0.0029f,0.0000f),
  glm::vec3(0.0058f,0.0021f,0.0000f), glm::vec3(0.0041f,0.0015f,0.0000f), glm::vec3(0.0029f,0.0010f,0.0000f),
  glm::vec3(0.0020f,0.0007f,0.0000f), glm::vec3(0.0014f,0.0005f,0.0000f), glm::vec3(0.0010f,0.0004f,0.0000f),
  glm::vec3(0.0007f,0.0002f,0.0000f), glm::vec3(0.0005f,0.0002f,0.0000f), glm::vec3(0.0003f,0.0001f,0.0000f),
  glm::vec3(0.0002f,0.0001f,0.0000f), glm::vec3(0.0002f,0.0001f,0.0000f), glm::vec3(0.0001f,0.0000f,0.0000f),
  glm::vec3(0.0001f,0.0000f,0.0000f), glm::vec3(0.0001f,0.0000f,0.0000f), glm::vec3(0.0000f,0.0000f,0.0000f)
};

glm::vec3 rgb_to_xyz(const glm::vec3& rgb)
{
  const glm::mat3 M = glm::mat3(0.4887180, 0.1762044, 0.0,
                      0.3106803, 0.8129847, 0.0102048,
                      0.2006017, 0.0108109, 0.9897952);
  return M * rgb;
}

glm::vec3 xyz_to_rgb(const glm::vec3& xyz)
{
  const glm::mat3 M = glm::mat3(2.3706743, -0.5138850, 0.0052982,
                      -0.9000405, 1.4253036, -0.0146949,
                      -0.4706338, 0.0885814, 1.0093968);
  return M * xyz;
}

glm::vec3 wavelength_to_xyz(uint32_t wavelength)
{
  return cie_colour_match[(wavelength - 380) / 5];
}

glm::vec3 wavelength_to_rgb(uint32_t wavelength)
{
  return xyz_to_rgb(wavelength_to_xyz(wavelength));
}

uint32_t get_random_wavelength(float random)
{
  return 380 + uint32_t(random * 80.999) * 5;
}

// uniform samples of wavelength -> every sample has the same probability
glm::vec3 get_inv_wavelength_probability()
{
  return 80.0f / glm::vec3(21.3714, 21.3711, 21.3715);
}
