#pragma once

#include "vulkantypes.h"

struct Mesh {

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

    void setupMesh();
};


Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
{
    this->vertices = vertices;
    this->indices = indices;
    //this->textures = textures;

    setupMesh();
}