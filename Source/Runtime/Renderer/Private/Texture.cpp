#include "Texture.hpp"
#include <GL/glew.h>
#include <SOIL2/SOIL2.h>
#include "Format.hpp"

namespace oeng
{
	Texture::Texture(Path file)
	{
		Vector<int, 2> size;
		auto num_channels = 0;

		auto str = file->string();
		const std::unique_ptr<unsigned char[]> image{SOIL_load_image(str.c_str(), &size[0], &size[1], &num_channels, SOIL_LOAD_AUTO)};
		if (!image) throw std::runtime_error{format("Failed to load image '{}': {}", str, SOIL_last_result())};

		size_ = Vec2u16{size};

		glGenTextures(1, &id_);
		Activate();

		int img_format;
		int alignment;
		
		switch (num_channels)
		{
		case 3:
			img_format = GL_RGB;
			alignment = 1;
			break;
		case 4:
			img_format = GL_RGBA;
			alignment = 4;
			break;
		default:
			throw std::runtime_error{format("Invalid image format for '{}'. num_channels was {}", str, num_channels)};
		}

		glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);
		glTexImage2D(GL_TEXTURE_2D, 0, img_format, size[0], size[1], 0, img_format, GL_UNSIGNED_BYTE, image.get());
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	Texture::~Texture()
	{
		glDeleteTextures(1, &id_);
	}

	void Texture::Activate() const
	{
		glBindTexture(GL_TEXTURE_2D, id_);
	}
}
