#include "IndexBuffer.h"
#include <glad/glad.h>


IndexBuffer::IndexBuffer()
	:	m_RendererID(0), m_Count(0)
{}

//generates the index buffer and sets how it is organized based on the count arg
void IndexBuffer::Set_Data(const unsigned int* data, unsigned int count)
{
	m_Count = count;
	glGenBuffers(1, &m_RendererID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
}

//destructor that deletes buffer after use
IndexBuffer::~IndexBuffer()
{
	glDeleteBuffers(1,&m_RendererID);
}

//binds the index buffer
void IndexBuffer::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

//unbinds index buffer
void IndexBuffer::Unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
