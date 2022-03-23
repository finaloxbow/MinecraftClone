#pragma once

#include "VertexBuffer.h"
#include "VerterBufferLayout.h"

class VertexArray
{
private:
	unsigned int m_RendererID;
public:
	VertexArray();
	~VertexArray();

	//add vertex buffer and its layout to vertex array
	void AddBuffer(VertexBuffer& vb, VertexBufferLayout& layout);

	void Bind() const;
	void Unbind() const;
};