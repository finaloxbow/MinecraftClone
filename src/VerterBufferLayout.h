#pragma once

#include <vector>
#include <glad/glad.h>


struct VertexBufferElement
{
	//count of elements
	unsigned int count;
	//type of elements
	unsigned int type;
	//whether normalized to (-1, 1)
	bool normalized;

	//gets size of type given by GL enum
	static unsigned int GetSizeOfType(unsigned int type) {
		switch (type) {
		case GL_FLOAT:			return 4;
		case GL_UNSIGNED_INT:	return 4;
		case GL_UNSIGNED_BYTE:	return 1;
		}

		return 0;
	}
};

//abstraction of buffer layouts
class VertexBufferLayout 
{
private:
	//layouts stored into vector to be used in draw calls, etc.
	std::vector<VertexBufferElement> m_Elements;
	unsigned int m_Stride;
public:
	VertexBufferLayout() 
	{
		m_Stride = 0;
	}

	template<typename T>
	void Push(unsigned int count) {
		static_assert(false);
	}
	template<>
	void Push<float>(unsigned int count) {
		struct VertexBufferElement VBE = { count, GL_FLOAT, false };
		m_Elements.push_back(VBE);
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
	}
	template<>
	void Push<unsigned int>(unsigned int count) {
		struct VertexBufferElement VBE = { count, GL_UNSIGNED_INT, false };
		m_Elements.push_back(VBE);
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
	}


	std::vector<VertexBufferElement> GetElements() const { return m_Elements; }
	inline unsigned int GetStride() const { return m_Stride; }
};