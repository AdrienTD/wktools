// (C) 2016-2017 AdrienTD
// Licensed under the MIT license (see license.txt for more information)

struct Matrix;
struct Vector3;

void CreateIdentityMatrix(Matrix *m);
void CreateZeroMatrix(Matrix *m);
void CreateTranslationMatrix(Matrix *m, float x, float y, float z);
void CreateScaleMatrix(Matrix *m, float x, float y, float z);
void CreateRotationXMatrix(Matrix *m, float a);
void CreateRotationYMatrix(Matrix *m, float a);
void CreateRotationZMatrix(Matrix *m, float a);
void MultiplyMatrices(Matrix *m, Matrix *a, Matrix *b);
void TransposeMatrix(Matrix *m, Matrix *a);
void TransformVector3(Vector3 *v, Vector3 *a, Matrix *m);
void TransformNormal3(Vector3 *v, Vector3 *a, Matrix *m);
void CreatePerspectiveMatrix(Matrix *m, float fovy, float aspect, float zn, float zf);
void CreateLookAtLHViewMatrix(Matrix *m, Vector3 *eye, Vector3 *at, Vector3 *up);
void CreateRotationYXZMatrix(Matrix *m, float y, float x, float z);
void TransformCoord3(Vector3 *r, Vector3 *v, Matrix *m);
//void InverseMatrix(Matrix *o, Matrix *i);
void Vec3Cross(Vector3 *r, Vector3 *a, Vector3 *b);
void NormalizeVector3(Vector3 *o, Vector3 *i);
int SphereIntersectsRay(Vector3 *sphPos, float radius, Vector3 *raystart, Vector3 *raydir);

struct Matrix
{
	union {
		float v[16];
		float m[4][4];
		struct {
			float _11, _12, _13, _14, _21, _22, _23, _24, 
				_31, _32, _33, _34, _41, _42, _43, _44;
		};
	};
	Matrix operator*(Matrix &a)
	{
		Matrix m;
		MultiplyMatrices(&m, this, &a);
		return m;
	}
	void operator*=(Matrix &a)
	{
		Matrix m = *this;
		MultiplyMatrices(this, &m, &a);
	}
};

struct Vector3
{
	float x, y, z; //, w; // w used to align size.
	Vector3() {x = y = z = 0;}
	Vector3(float a, float b) {x = a; y = b; z = 0;}
	Vector3(float a, float b, float c) {x = a; y = b; z = c;}

	Vector3 operator+(Vector3 a) {return Vector3(x + a.x, y + a.y, z + a.z);}
	Vector3 operator-(Vector3 a) {return Vector3(x - a.x, y - a.y, z - a.z);}
	Vector3 operator*(Vector3 a) {return Vector3(x * a.x, y * a.y, z * a.z);}
	Vector3 operator/(Vector3 a) {return Vector3(x / a.x, y / a.y, z / a.z);}
	Vector3 operator+(float a) {return Vector3(x + a, y + a, z + a);}
	Vector3 operator-(float a) {return Vector3(x - a, y - a, z - a);}
	Vector3 operator*(float a) {return Vector3(x * a, y * a, z * a);}
	Vector3 operator/(float a) {return Vector3(x / a, y / a, z / a);}
	Vector3 operator-() {return Vector3(-x, -y, -z);}

	Vector3 operator+=(Vector3 a) {x += a.x; y += a.y; z += a.z; return *this;}
	Vector3 operator-=(Vector3 a) {x -= a.x; y -= a.y; z -= a.z; return *this;}
	Vector3 operator*=(Vector3 a) {x *= a.x; y *= a.y; z *= a.z; return *this;}
	Vector3 operator/=(Vector3 a) {x /= a.x; y /= a.y; z /= a.z; return *this;}
	Vector3 operator+=(float a) {x += a; y += a; z += a; return *this;}
	Vector3 operator-=(float a) {x -= a; y -= a; z -= a; return *this;}
	Vector3 operator*=(float a) {x *= a; y *= a; z *= a; return *this;}
	Vector3 operator/=(float a) {x /= a; y /= a; z /= a; return *this;}

	int operator==(Vector3 a) {return (x==a.x) && (y==a.y) && (z==a.z);}
	int operator!=(Vector3 a) {return !( (x==a.x) && (y==a.y) && (z==a.z) );}

	void print() {printf("(%f, %f, %f)\n", x, y, z);}
	float len2xy() {return sqrt(x*x + y*y);}
	float sqlen2xy() {return x*x + y*y;}
	float len2xz() {return sqrt(x*x + z*z);}
	float sqlen2xz() {return x*x + z*z;}
	float len3() {return sqrt(x*x + y*y + z*z);}
	float sqlen3() {return x*x + y*y + z*z;}
	Vector3 normal() {float l = len3(); return Vector3(x/l, y/l, z/l);}
	Vector3 normal2xz() {float l = len2xz(); return Vector3(x/l, 0, z/l);}
	float dot(Vector3 a) {return a.x * x + a.y * y + a.z * z;}
	float dot2xz(Vector3 a) {return a.x * x + a.z * z;}
};

void TransformBackFromViewMatrix(Vector3 *r, Vector3 *o, Matrix *m);