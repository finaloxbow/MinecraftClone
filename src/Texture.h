#pragma once

#include <iostream>

class Texture 
{
private:
	unsigned int m_RendererID;		//renderer ID
	std::string m_Filepath;			//cached file path
	unsigned char* m_LocalBuffer;	//cached image data
	int m_Width, m_Height, m_BPP;	//width, height, bytes per pixel of image
public:
	//creates/destroys texture from file path
	Texture(const std::string& path);
	~Texture();

	//binds/unbinds texture from texture slot (default = 0)
	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	//getter methods
	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
};