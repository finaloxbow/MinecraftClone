#include "Renderer.h"

//clears color bit and depth bit before drawing
void Renderer::Clear() const
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//handles draw call
void Renderer::DrawElements(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
	//binds all three things
	shader.Bind();
	va.Bind();
	ib.Bind();

	glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, 0);

}

void Renderer::DrawTriangles(const VertexArray& va, int count, const Shader& shader) const
{
	//binds all three things
	shader.Bind();
	va.Bind();

	glDrawArrays(GL_TRIANGLES, 0, count);

}
