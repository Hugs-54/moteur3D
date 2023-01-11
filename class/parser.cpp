#include "parser.h"

Parser::Parser() {}

void Parser::parse(string fileName)
{
    fstream file;
    file.open(fileName, ios::in);
    string line, tmp;
    while (getline(file, line))
    {
        if (!line.compare(0, 2, "v "))
        {
            vector<string> numbers;
            stringstream stream(line);
            while (getline(stream, tmp, ' '))
            {
                numbers.push_back(tmp);
            }

            float x = stof(numbers.at(1));
            float y = stof(numbers.at(2));
            float z = stof(numbers.at(3));
            Vertex v(x, y, z);
            points.push_back(v);
        }
        else if (!line.compare(0, 2, "f "))
        {
            vector<string> tuples;
            stringstream stream(line);
            while (getline(stream, tmp, ' '))
            {
                tuples.push_back(tmp);
            }
            Tuple t1 = parseTuple(tuples.at(1));
            Tuple t2 = parseTuple(tuples.at(2));
            Tuple t3 = parseTuple(tuples.at(3));
            Triangle tri(t1.getI1(), t2.getI1(), t3.getI1());
            triangles.push_back(tri);
        }
    }
    file.close();
}

Tuple Parser::parseTuple(string t)
{
    string tmp;
    vector<int> integers;
    stringstream stream(t);
    while (getline(stream, tmp, '/'))
    {
        integers.push_back(stoi(tmp));
    }
    Tuple tu(points.at(integers.at(0) - 1), integers.at(1), integers.at(2));
    return tu;
}

vector<Vertex> Parser::getPoints()
{
    return points;
}

vector<Triangle> Parser::getTriangles()
{
    return triangles;
}