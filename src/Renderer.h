#pragma once

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"

class Renderer
{
public:
	void Clear() const;
	void DrawElements(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
	void DrawTriangles(const VertexArray& va, int count, const Shader& shader) const;
};