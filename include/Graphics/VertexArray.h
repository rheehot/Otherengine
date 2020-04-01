#pragma once
#include <gsl/span>
#include "Vector.h"

namespace game::graphics
{
	class VertexArray
	{
	public:
		VertexArray(gsl::span<const FVector3> verts, gsl::span<const Vector3<uint16_t>> indices);
		~VertexArray();

	private:
		unsigned vertex_buffer_;
		unsigned index_buffer_;
		unsigned vertex_array_;
	};
}
