#include "renderer.h"
#include <iostream>

Vertex light_dir(0, 0, -1);

Renderer::Renderer(int w, int h) : width{w}, heigth{h}
{
}

void Renderer::line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
    bool steep = false;
    if (std::abs(x0 - x1) < std::abs(y0 - y1))
    { // if the line is steep, we transpose the image
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0 > x1)
    { // make it left−to−right
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    for (int x = x0; x <= x1; x++)
    {
        float t = (x - x0) / (float)(x1 - x0);
        int y = y0 + (y1 - y0) * t;
        if (steep)
        {
            image.set(y, x, color); // if transposed, de−transpose
        }
        else
        {
            image.set(x, y, color);
        }
    }
}

void Renderer::renderPoints(vector<Vertex> vertexs, TGAImage &image, TGAColor color)
{
    float x, y;
    for (size_t i = 0; i < vertexs.size(); i++)
    {
        Vertex v = vertexs.at(i);
        x = v.getX();
        y = v.getY();
        image.set(x, y, color);
    }
}

void Renderer::renderTriangles(vector<Triangle> triangles, TGAImage &image, TGAColor color)
{
    for (size_t i = 0; i < triangles.size(); i++)
    {
        Triangle t = triangles.at(i);
        for (size_t i = 0; i < 3; i++)
        {
            Vertex v0 = t.getPoint(i);
            Vertex v1 = t.getPoint((i + 1) % 3);
            int x0 = v0.getX();
            int y0 = v0.getY();
            int x1 = v1.getX();
            int y1 = v1.getY();
            line(x0, y0, x1, y1, image, color);
        }
    }
}

vector<int> Renderer::createBox(Triangle t)
{
    int xmin = t.getPoint(0).getX();
    int ymin = t.getPoint(0).getY();
    int xmax = t.getPoint(0).getX();
    int ymax = t.getPoint(0).getY();

    for (size_t i = 1; i < 3; i++)
    {
        Vertex v = t.getPoint(i);
        if (v.getX() < xmin)
            xmin = v.getX();
        if (v.getY() < ymin)
            ymin = v.getY();
        if (v.getX() > xmax)
            xmax = v.getX();
        if (v.getY() > ymax)
            ymax = v.getY();
    }
    vector<int> p = {xmin, xmax, ymin, ymax};
    return p;
}

void Renderer::fillTriangles(vector<Triangle> triangles, TGAImage &image, TGAImage &texture, TGAImage &normalmap, float *zbuffer)
{
    Vertex b;
    int width = image.get_width();
    int height = image.get_height();
    int widthTexture = texture.get_width();
    int heightTexture = texture.get_height();

    for (Triangle &t : triangles)
    {
        // double intensity = getIntensity(t.getPoint(0), t.getPoint(1), t.getPoint(2));
        t.movingCamera(width, height);
        TGAColor colorTexture;
        TGAColor colorNormal;
        vector<int> bbox = createBox(t);

        // Pour chaque pixel de la boite
        for (int i = std::max(0, bbox.at(0)); i <= std::min(width - 1, bbox.at(1)); i++)
        {
            for (int j = std::max(0, bbox.at(2)); j <= std::min(height - 1, bbox.at(3)); j++)
            {
                // Si pixel dans le triangle alors remplir
                if (isPointInsideTriangle(t, i, j, b))
                {
                    double X = b.getX() * t.getVertexTexture(1).getX() * widthTexture + b.getY() * t.getVertexTexture(2).getX() * widthTexture + b.getZ() * t.getVertexTexture(0).getX() * widthTexture;    // X = alpha * vt[1].X + beta * vt[2].X + gamma * vt[0].X
                    double Y = b.getX() * t.getVertexTexture(1).getY() * heightTexture + b.getY() * t.getVertexTexture(2).getY() * heightTexture + b.getZ() * t.getVertexTexture(0).getY() * heightTexture; // Y = alpha * vt[1].Y + beta * vt[2].Y + gamma * vt[0].Y
                    colorTexture = texture.get(X, Y);
                    colorNormal = normalmap.get(X, Y);

                    double Z = b.getX() * t.getPoint(0).getZ();
                    Z += b.getY() * t.getPoint(1).getZ();
                    Z += b.getZ() * t.getPoint(2).getZ();

                    if (zbuffer[int(i + j * width)] < Z)
                    {
                        zbuffer[int(i + j * width)] = Z;
                        bool discard = fragment(colorNormal, colorTexture, b);
                        if (!discard)
                        {
                            image.set(i, j, colorTexture);
                        }
                    }
                }
            }
        }
    }
}

bool Renderer::fragment(TGAColor &normal, TGAColor &color, Vertex &bary)
{
    Vertex norm(normal.r * 2. / 255. - 1., normal.g * 2. / 255. - 1., normal.b * 2. / 255. - 1.);
    norm = norm.normalize();
    Vertex light = light_dir.normalize();
    double produitSc = norm.getX() * light.getX() + norm.getY() * light.getY() + norm.getZ() * light.getZ();
    double intensity = std::max(1.2, produitSc);
    Vertex rgb(color.r, color.g, color.b);
    double v = rgb.getX() * norm.getX() + rgb.getY() * norm.getY() + rgb.getZ() * norm.getZ();
    double diffuse = std::min(1., std::max(0., v) / 255.);
    diffuse *= intensity;
    color.r *= diffuse;
    color.g *= diffuse;
    color.b *= diffuse;
    return false;
}

double Renderer::getIntensity(Vertex v1, Vertex v2, Vertex v3)
{
    Vertex vAB(v2.getX() - v1.getX(), v2.getY() - v1.getY(), v2.getZ() - v1.getZ());
    Vertex vAC(v3.getX() - v1.getX(), v3.getY() - v1.getY(), v3.getZ() - v1.getZ());
    double X = vAB.getY() * vAC.getZ() - vAB.getZ() * vAC.getY();
    double Y = vAB.getZ() * vAC.getX() - vAB.getX() * vAC.getZ();
    double Z = vAB.getX() * vAC.getY() - vAB.getY() * vAC.getX();
    Vertex n(X, Y, Z);
    double length = sqrt((n.getX() * n.getX()) + (n.getY() * n.getY()) + (n.getZ() * n.getZ()));
    Vertex n2(n.getX() / length, n.getY() / length, n.getZ() / length);
    double intensity = -((light_dir.getX() * n2.getX()) + (light_dir.getY() * n2.getY()) + (light_dir.getZ() * n2.getZ()));
    return intensity;
}

double Renderer::areaOfTriangle(Vertex v1, Vertex v2, Vertex v3)
{
    double x1 = v1.getX();
    double x2 = v2.getX();
    double x3 = v3.getX();
    double y1 = v1.getY();
    double y2 = v2.getY();
    double y3 = v3.getY();
    return (x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2)) / 2.0;
}

bool Renderer::isPointInsideTriangle(Triangle &t, double px, double py, Vertex &bary)
{
    Vertex &v1 = t.getPoint(0);
    Vertex &v2 = t.getPoint(1);
    Vertex &v3 = t.getPoint(2);
    Vertex v4(px, py, 0);

    double A = areaOfTriangle(v1, v2, v3);
    double A1 = areaOfTriangle(v4, v2, v3);
    double A2 = areaOfTriangle(v1, v4, v3);
    double A3 = areaOfTriangle(v1, v2, v4);

    double alpha = A2 / A;
    double beta = A3 / A;
    double gamma = A1 / A;

    bary.setX(alpha);
    bary.setY(beta);
    bary.setZ(gamma);

    double marge = -0.0001;
    return alpha > marge && beta > marge && gamma > marge;
}