#ifndef _ASSIGNMENT_3_H_
#define _ASSIGNMENT_3_H_

#include <stdio.h>
#include <vector>

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/OpenGL.h>
#elif defined _WIN32 || defined _WIN64
#include <GLFW/glfw3.h>
#endif

#define TRANSFORM_NONE      0 
#define TRANSFORM_ROTATE    1
#define TRANSFORM_SCALE     2 
#define TRANSFORM_TRANSLATE 3

#define OBJ_WIREFRAME       0
#define OBJ_SOLID           1
#define OBJ_EDGE            2 
#define OBJ_BBOX            3

#define IMG_WIDTH  64
#define IMG_HEIGHT 64

#define PI 3.1415926535

struct Vector
{
  private:

    float data[4];

  public:
  
    Vector(void)
    {
        data[0] = 
        data[1] =
        data[2] = 
        data[3] = 0.f;
    }

    Vector(float x, float y, float z, float w)
    {
        data[0] = x;
        data[1] = y;
        data[2] = z;
        data[3] = w;
        
    }

    float &operator [](unsigned int idx)
    {
        return data[idx];
    }  
};

struct Point
{
  private:

    float data[4];

  public:
  
    Point(void)
    {
        data[0] = 
        data[1] =
        data[2] = 0.f; 
        data[3] = 1.f;
    }

    Point(float x, float y, float z, float w)
    {
        data[0] = x;
        data[1] = y;
        data[2] = z;
        data[3] = w;
    }

    float &operator [](unsigned int idx)
    {
        return data[idx];
    }  

    const float *gldata(void)
    {
        return data;
    }
};

struct Color
{
  private:

    float data[3];

  public:
  
    Color(void)
    {
        data[0] =
        data[1] =
        data[2] = 0.f;
    }

    Color(float r, float g, float b)
    {
        data[0] = r;
        data[1] = g;
        data[2] = b;
    }

    float &operator [](unsigned int idx)
    {
        return data[idx];
    }  
};

typedef float TexCoord[2];
typedef float GLColor4[4];

typedef float Matrix[16];

struct Vertex
{
    // Vertex Coordinate
    Point    v;

    // Vertex Normal
    Vector   n;

    // Vertex TextureCorrdinate
    TexCoord t;

    // Vertex Color 
    Color    c;
};

struct BoundingBox
{
    Point  minP;
    Point  maxP;
    Vector vSize;
};

struct Face
{
    // Vertex Index
    int v1;
    int v2;
    int v3;
};

struct Object
{
    std::vector<Vertex> vertices;
    std::vector<Face  > faces;

    BoundingBox         bBox;
};

inline
void subPnt(Vector &vResult, Point &p1, Point &p2)
{
    vResult[0] = p1[0] - p2[0];
    vResult[1] = p1[1] - p2[1];
    vResult[2] = p1[2] - p2[2];
    vResult[3] = p1[3] - p2[3];
}

inline
void add(Vector &vResult, Vector &v1, Vector &v2)
{
    vResult[0] = v1[0] + v2[0];
    vResult[1] = v1[1] + v2[1];
    vResult[2] = v1[2] + v2[2];
    vResult[3] = v1[3] + v2[3];
}

inline
void addScaled(Point &result, Point &p, Vector &v, float s)
{
    result[0] = p[0] + (v[0] * s);
    result[1] = p[1] + (v[1] * s);
    result[2] = p[2] + (v[2] * s);
    result[3] = p[3] + (v[3] * s);
}

inline
void mult(Point &result, Matrix m, Point &p)
{
    for(int i = 0; i < 4; ++i)
    { 
        result[i] = 
            m[i +  0] * p[0] + 
            m[i +  4] * p[1] + 
            m[i +  8] * p[2] + 
            m[i + 12] * p[3];
    }
}

inline
void mult(Vector &result, Matrix m, Vector &v)
{
    for(int i = 0; i < 4; ++i)
    { 
        result[i] = 
            m[i +  0] * v[0] + 
            m[i +  4] * v[1] + 
            m[i +  8] * v[2] + 
            m[i + 12] * v[3];
    }
}

inline
void assign(Vector &v1, Vector &v2)
{
    v1[0] = v2[0];
    v1[1] = v2[1];
    v1[2] = v2[2];
    v1[3] = v2[3];
}

inline
float dot(Vector &v1, Vector &v2)
{
    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

inline
float length(Vector &v)
{
    return sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

inline
void normalize(Vector &v)
{
    float fLength = length(v);

    fLength = 1.f / fLength;

    v[0] *= fLength;
    v[1] *= fLength;
    v[2] *= fLength;
    v[3] *= fLength;
}

inline
float maxComponent3(Vector &v)
{
    float returnValue = v[0];

    if(v[1] > returnValue)
    {
        returnValue = v[1];
    }
    
    if(v[2] > returnValue)
    {
        returnValue = v[2];
    }

    return returnValue;
}

inline
void dumpMatrix(Matrix m)
{
    for(int i = 0; i < 4; ++i)
    {
        fprintf(stderr, "%f %f %f %f\n",
                m[i * 4 + 0],
                m[i * 4 + 1],
                m[i * 4 + 2],
                m[i * 4 + 3]);
    }
}

inline
void dumpPnt(Point &p)
{
    fprintf(stderr, "%f %f %f %f\n",
            p[0],
            p[1],
            p[2],
            p[3]);
}

inline
void dumpPnt(const char *szInfo, Point &p)
{
    fprintf(stderr, "%s\n%f %f %f %f\n",
            szInfo,
            p[0],
            p[1],
            p[2],
            p[3]);
}

inline
void combineColor(Color &result, GLColor4 lc, GLColor4 mc, float s)
{
    result[0] += lc[0] * s * mc[0];
    result[1] += lc[1] * s * mc[1];
    result[2] += lc[2] * s * mc[2];
}

inline
bool invert(Matrix result, Matrix m)
{
	float inv[16], det;
	inv[0] =   m[5]*m[10]*m[15] - m[5]*m[11]*m[14] - m[9]*m[6]*m[15] + m[9]*m[7]*m[14] + m[13]*m[6]*m[11] - m[13]*m[7]*m[10];
	inv[4] =  -m[4]*m[10]*m[15] + m[4]*m[11]*m[14] + m[8]*m[6]*m[15] - m[8]*m[7]*m[14] - m[12]*m[6]*m[11] + m[12]*m[7]*m[10];
	inv[8] =   m[4]*m[9]*m[15] - m[4]*m[11]*m[13] - m[8]*m[5]*m[15] + m[8]*m[7]*m[13] + m[12]*m[5]*m[11] - m[12]*m[7]*m[9];
	inv[12] = -m[4]*m[9]*m[14] + m[4]*m[10]*m[13] + m[8]*m[5]*m[14] - m[8]*m[6]*m[13] - m[12]*m[5]*m[10] + m[12]*m[6]*m[9];
	inv[1] =  -m[1]*m[10]*m[15] + m[1]*m[11]*m[14] + m[9]*m[2]*m[15] - m[9]*m[3]*m[14] - m[13]*m[2]*m[11] + m[13]*m[3]*m[10];
	inv[5] =   m[0]*m[10]*m[15] - m[0]*m[11]*m[14] - m[8]*m[2]*m[15] + m[8]*m[3]*m[14] + m[12]*m[2]*m[11] - m[12]*m[3]*m[10];
	inv[9] =  -m[0]*m[9]*m[15] + m[0]*m[11]*m[13] + m[8]*m[1]*m[15] - m[8]*m[3]*m[13] - m[12]*m[1]*m[11] + m[12]*m[3]*m[9];
	inv[13] =  m[0]*m[9]*m[14] - m[0]*m[10]*m[13] - m[8]*m[1]*m[14] + m[8]*m[2]*m[13] + m[12]*m[1]*m[10] - m[12]*m[2]*m[9];
	inv[2] =   m[1]*m[6]*m[15] - m[1]*m[7]*m[14] - m[5]*m[2]*m[15] + m[5]*m[3]*m[14] + m[13]*m[2]*m[7] - m[13]*m[3]*m[6];
	inv[6] =  -m[0]*m[6]*m[15] + m[0]*m[7]*m[14] + m[4]*m[2]*m[15] - m[4]*m[3]*m[14] - m[12]*m[2]*m[7] + m[12]*m[3]*m[6];
	inv[10] =  m[0]*m[5]*m[15] - m[0]*m[7]*m[13] - m[4]*m[1]*m[15] + m[4]*m[3]*m[13] + m[12]*m[1]*m[7] - m[12]*m[3]*m[5];
	inv[14] = -m[0]*m[5]*m[14] + m[0]*m[6]*m[13] + m[4]*m[1]*m[14] - m[4]*m[2]*m[13] - m[12]*m[1]*m[6] + m[12]*m[2]*m[5];
	inv[3] =  -m[1]*m[6]*m[11] + m[1]*m[7]*m[10] + m[5]*m[2]*m[11] - m[5]*m[3]*m[10] - m[9]*m[2]*m[7] + m[9]*m[3]*m[6];
	inv[7] =   m[0]*m[6]*m[11] - m[0]*m[7]*m[10] - m[4]*m[2]*m[11] + m[4]*m[3]*m[10] + m[8]*m[2]*m[7] - m[8]*m[3]*m[6];
	inv[11] = -m[0]*m[5]*m[11] + m[0]*m[7]*m[9] + m[4]*m[1]*m[11] - m[4]*m[3]*m[9] - m[8]*m[1]*m[7] + m[8]*m[3]*m[5];
	inv[15] =  m[0]*m[5]*m[10] - m[0]*m[6]*m[9] - m[4]*m[1]*m[10] + m[4]*m[2]*m[9] + m[8]*m[1]*m[6] - m[8]*m[2]*m[5];

	det = m[0]*inv[0] + m[1]*inv[4] + m[2]*inv[8] + m[3]*inv[12];
	if (det == 0)
		return false;

	det = 1.0 / det;
	for (int i = 0; i < 16; i++)
		result[i] = inv[i] * det;

	return true;
}

inline 
void transpose(Matrix result, Matrix m)
{
	for(int i = 0; i < 4; i++)
		for (int j = 0; j<4; j++)
		{
			result[i*4+j] = m[j*4+i];
		}
}




#endif
