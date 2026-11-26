/* Minimal stb_image build unit for the world-icon decoder.
 * Only JPEG + PNG, decoded from memory (no stdio). */
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STBI_NO_STDIO
#define STBI_NO_THREAD_LOCALS
#define STBI_FAILURE_USERMSG
#include "vendor/stb_image.h"
