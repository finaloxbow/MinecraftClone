#include "Renderer.h"

//clears color bit before drawing
void Renderer::Clear() const
{
	glClear(GL_COLOR_BUFFER_BIT);
}

//handles draw call
void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
	//binds all three things
	shader.Bind();
	va.Bind();
	ib.Bind();

	glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, 0);

}
