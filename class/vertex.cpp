#include "vertex.h"

Vertex::Vertex(double x, double y, double z) : f_x{x}, f_y(y), f_z(z)
{
}

Vertex::Vertex() : f_x{0}, f_y(0), f_z(0)
{
}

void Vertex::resize(int width, int heigth)
{
    int w = width / 2;
    int h = heigth / 2;
    p_x = f_x * w + w;
    p_y = f_y * h + h;
}

double Vertex::getX()
{
    return f_x;
}

double Vertex::getY()
{
    return f_y;
}

double Vertex::getZ()
{
    return f_z;
}

int Vertex::getPixelX()
{
    return p_x;
}

int Vertex::getPixelY()
{
    return p_y;
}

int Vertex::getPixelZ()
{
    return p_z;
}

void Vertex::setPixel(int x, int y)
{
    p_x = x;
    p_y = y;
}

void Vertex::setX(double x)
{
    f_x = x;
}

void Vertex::setY(double y)
{
    f_y = y;
}

void Vertex::setZ(double z)
{
    f_z = z;
}