#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <math.h>
#include <string>
#include <sstream>
#include <vector>
#include <cstdio>
#include <map>
#include <algorithm>

#include "structs.h"
#include "timer.h"

using namespace std;

/** OPERATOR OVERLOADING **/
/** operator== might not make sense for some structs that have a > comparison built out of && element comparisons **/
bool operator>(rgb a, rgb b);
bool operator<(rgb a, rgb b);
bool operator<=(rgb a, rgb b);
bool operator>=(rgb a, rgb b);
bool operator==(rgb a, rgb b);
bool operator!=(rgb a, rgb b);

dim2 operator+(dim2 lhs, dim2 rhs);
dim2 operator-(dim2 lhs, dim2 rhs);
dim2 operator*(dim2 lhs, float rhs);
dim2 operator*(float lhs, dim2 rhs);
dim2 operator*(dim2 lhs, dim2 rhs);
dim2 operator/(dim2 lhs, float rhs);
dim2 operator/(dim2 lhs, dim2 rhs);
bool operator==(dim2 lhs, dim2 rhs);
bool operator!=(dim2 lhs, dim2 rhs);
bool operator>(dim2 lhs, dim2 rhs);
bool operator<(dim2 lhs, dim2 rhs);
bool operator<=(dim2 lhs, dim2 rhs);
bool operator>=(dim2 lhs, dim2 rhs);

dim3 operator+(dim3 lhs, dim3 rhs);
dim3 operator-(dim3 lhs, dim3 rhs);
dim3 operator*(dim3 lhs, dim3 rhs);
dim3 operator*(dim3 lhs, float rhs);
dim3 operator*(float lhs, dim3 rhs);
dim3 operator*(dim3 lhs, int rhs);
dim3 operator*(int lhs, dim3 rhs);
dim3 operator/(dim3 lhs, float rhs);
bool operator==(dim3 lhs, dim3 rhs);
bool operator!=(dim3 lhs, dim3 rhs);
bool operator>(dim3 lhs, dim3 rhs);
bool operator<(dim3 lhs, dim3 rhs);
bool operator<=(dim3 lhs, dim3 rhs);
bool operator>=(dim3 lhs, dim3 rhs);

bool operator==(coord3 lhs, coord3 rhs);
bool operator!=(coord3 lhs, coord3 rhs);
bool operator>(coord3 lhs, coord3 rhs);
bool operator<(coord3 lhs, coord3 rhs);
bool operator<=(coord3 lhs, coord3 rhs);
bool operator>=(coord3 lhs, coord3 rhs);

/** FUNCTIONS **/
int randInt(int lower, int upper);
bool computeChance(float chance, int precision);

dim3 add(dim3 a, dim3 b);
dim2 add(dim2 a, dim2 b);
rgb add(rgb a, rgb b);
dim3 subtract(dim3 a, dim3 b);
rgb subtract(rgb a, rgb b);
dim3 cross(dim3 a, dim3 b);
quaternion mult(quaternion a, quaternion b);
quaternion mult(quaternion a, dim3 b);
rgb mult(rgb a, rgb b);
float dot(dim3 a, dim3 b);
float dot(dim2 a, dim2 b);
dim3 scale(dim3 a, float b);
dim2 scale(dim2 a, float b);
rgb scale(rgb a, float b);
dim2 normalise(dim2 a);
dim3 normalise(dim3 a);
quaternion normalise(quaternion a);
quaternion conjugate(quaternion a);
dim2 invert(dim2 a);
dim3 invert(dim3 a);
dim3 reflect(dim3 direction, dim3 normal);
dim3 fabs3(dim3 a);
float max3(dim3 a);
float *biggestDirection3(dim3 *a);
dim3 maxDim3(dim3 a, dim3 b);
dim3 makeDim3(float a, float b, float c);
dim2 makeDim2(float a, float b);
float length(dim3 a);
float length(dim2 a);

void printDim2(string text, dim2 a);
void printDim3(string text, dim3 a);

void clamp(float *a, float min, float max);
float clampAngleDeg(float a);
rgb clamp(rgb a);
rgb makeRGB(int name);
rgb makeRGB(float r, float g, float b);
rgb repeatRGB(float value);

float percentToPortion(float percent);

float detectCollisionCircle(dim2 c1, float r1, dim2 c2, float r2);

float computeAngle(dim2 target, dim2 center);
float computeDistance(dim2 target, dim2 center);
float computeDistance(dim3 target, dim3 center);
float distancePointToLine(dim2 target, dim2 a, dim2 b);
float angleDifference(float a, float b);
float toDegrees(float rad);
float toRadians(float deg);
float radiansPositiveOnly(float rad);
vector<dim2> convertPixelToDim2(vector<pixel> a);

bool pointInPolygon(dim2 point, vector<dim2> polygon);

vector<string> splitString(string s, char token);

void putPixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
void colourPixel(SDL_Surface *surface, int x, int y, rgb colour, float alpha);
Uint32 getPixel(SDL_Surface *surface, int x, int y);
rgb getPixelColour(SDL_Surface *surface, int x, int y);

void makeColourTransparent(SDL_Surface *surface, rgb colour);

#endif
