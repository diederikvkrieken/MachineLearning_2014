#include "functions.h"

/*static random_device rd; // obtain a random number from hardware*/
/*static default_random_engine eng; // seed the generator*/

/** OPERATOR OVERLOADING **/
bool operator>(rgb a, rgb b)
{
  if(a.r > b.r && a.g > b.g && a.b > b.b)
  { return true; }
  return false;
}
bool operator<(rgb a, rgb b)
{
  return b > a;
}
bool operator<=(rgb a, rgb b)
{
  /*return !(a > b);*/
  return (a < b || a == b);
}
bool operator>=(rgb a, rgb b)
{
  /*return !(a < b);*/
  return (a > b || a == b);
}
bool operator==(rgb a, rgb b)
{
  if(fabs(a.r - b.r) < EPSILON && fabs(a.g - b.g) < EPSILON && fabs(a.b - b.b) < EPSILON)
  { return true; }
  return false;
}
bool operator!=(rgb a, rgb b)
{
  return !(a == b);
}

dim2 operator+(dim2 lhs, dim2 rhs)
{
  lhs.x += rhs.x;
  lhs.y += rhs.y;
  return lhs;
}
dim2 operator-(dim2 lhs, dim2 rhs)
{
  lhs.x -= rhs.x;
  lhs.y -= rhs.y;
  return lhs;
}
dim2 operator*(dim2 lhs, float rhs)
{
  lhs.x *= rhs;
  lhs.y *= rhs;
  return lhs;
}
dim2 operator*(float lhs, dim2 rhs)
{
  rhs.x *= lhs;
  rhs.y *= lhs;
  return rhs;
}
dim2 operator*(dim2 lhs, dim2 rhs)
{
  lhs.x *= rhs.x;
  lhs.y *= rhs.y;
  return lhs;
}
dim2 operator/(dim2 lhs, float rhs)
{
  lhs.x /= rhs;
  lhs.y /= rhs;
  return lhs;
}
dim2 operator/(dim2 lhs, dim2 rhs)
{
  lhs.x /= rhs.x;
  lhs.y /= rhs.y;
  return lhs;
}
bool operator==(dim2 lhs, dim2 rhs)
{
  if(fabs(lhs.x - rhs.x) < EPSILON && fabs(lhs.y - rhs.y) < EPSILON)
  { return true; }
  return false;
}
bool operator!=(dim2 lhs, dim2 rhs)
{
  return !(lhs == rhs);
}
bool operator>(dim2 lhs, dim2 rhs)
{
  if(lhs.x > rhs.x && lhs.y > rhs.y)
  { return true; }
  return false;
}
bool operator<(dim2 lhs, dim2 rhs)
{
  return (rhs > lhs);
}
bool operator<=(dim2 lhs, dim2 rhs)
{
  /*return !(lhs > rhs);*/
  return (lhs < rhs || lhs == rhs);
}
bool operator>=(dim2 lhs, dim2 rhs)
{
  /*return !(lhs < rhs);*/
  return (lhs > rhs || lhs == rhs);
}

dim3 operator+(dim3 lhs, dim3 rhs)
{
  lhs.x += rhs.x;
  lhs.y += rhs.y;
  lhs.z += rhs.z;
  return lhs;
}
dim3 operator-(dim3 lhs, dim3 rhs)
{
  lhs.x -= rhs.x;
  lhs.y -= rhs.y;
  lhs.z -= rhs.z;
  return lhs;
}
dim3 operator*(dim3 lhs, dim3 rhs)
{
  lhs.x *= rhs.x;
  lhs.y *= rhs.y;
  lhs.z *= rhs.z;
  return lhs;
}
dim3 operator*(dim3 lhs, float rhs)
{
  lhs.x *= rhs;
  lhs.y *= rhs;
  lhs.z *= rhs;
  return lhs;
}
dim3 operator*(float lhs, dim3 rhs)
{
  rhs.x *= lhs;
  rhs.y *= lhs;
  rhs.z *= lhs;
  return rhs;
}
dim3 operator*(dim3 lhs, int rhs)
{
  lhs.x *= rhs;
  lhs.y *= rhs;
  lhs.z *= rhs;
  return lhs;
}
dim3 operator*(int lhs, dim3 rhs)
{
  rhs.x *= lhs;
  rhs.y *= lhs;
  rhs.z *= lhs;
  return rhs;
}
dim3 operator/(dim3 lhs, float rhs)
{
  lhs.x /= rhs;
  lhs.y /= rhs;
  lhs.z /= rhs;
  return lhs;
}
bool operator==(dim3 lhs, dim3 rhs)
{
  if(fabs(lhs.x - rhs.x) < EPSILON && fabs(lhs.y - rhs.y) < EPSILON && fabs(lhs.z - rhs.z) < EPSILON)
  { return true; }
  return false;
}
bool operator!=(dim3 lhs, dim3 rhs)
{
  return !(lhs == rhs);
}
bool operator>(dim3 lhs, dim3 rhs)
{
  if(lhs.x > rhs.x && lhs.y > rhs.y && lhs.z > rhs.z)
  { return true; }
  return false;
}
bool operator<(dim3 lhs, dim3 rhs)
{
  return (rhs > lhs);
}
bool operator<=(dim3 lhs, dim3 rhs)
{
  /*return !(lhs > rhs);*/
  return (lhs < rhs || lhs == rhs);
}
bool operator>=(dim3 lhs, dim3 rhs)
{
  /*return !(lhs < rhs);*/
  return (lhs > rhs || lhs == rhs);
}

bool operator==(coord3 lhs, coord3 rhs)
{
  if(lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z)
  { return true; }
  return false;
}
bool operator!=(coord3 lhs, coord3 rhs)
{
  return !(lhs == rhs);
}
bool operator>(coord3 lhs, coord3 rhs)
{
  /*printf("lhs: %d, %d, %d\n", lhs.x, lhs.y, lhs.z);
  printf("rhs: %d, %d, %d\n", rhs.x, rhs.y, rhs.z);*/
  if(lhs.x > rhs.x && lhs.y > rhs.y && lhs.z > rhs.z)
  { return true; }
  return false;
}
bool operator<(coord3 lhs, coord3 rhs)
{
  return (rhs > lhs);
}
bool operator<=(coord3 lhs, coord3 rhs)
{
  /*return !(lhs > rhs);*/
  return (lhs < rhs || lhs == rhs);
}
bool operator>=(coord3 lhs, coord3 rhs)
{
  /** (40,0,3) >= (0,0,0) fails **/
  /*return !(lhs < rhs);*/
  return (lhs > rhs || lhs == rhs);
}

/** FUNCTIONS **/
/** The returned number lies in the interval: [lower,upper] **/
/** When domain size is bigger than RAND_MAX this will loop forever **/
int randInt(int lower, int upper)
{
  int limit = upper - lower;
  int divisor = RAND_MAX / (limit+1);
  int value;

  do {
      value = rand() / divisor;
  } while (value > limit);

  return lower + value;
}
/*int randInt(int lower, int upper)
{
  return lower + (rand() % ((upper - lower) + 1));
}*/

bool computeChance(float chance, int precision)
{
  if(precision >= RAND_MAX)
  {
    fprintf(stderr, "[computeChance] Precision given is too high for this random generation method. returning false.\n");
    return false;
  }
  int boundary = chance * precision;
  int number = randInt(0, precision-1);
  if(number < boundary)
  {
    return true;
  }
  return false;
}

dim3 add(dim3 a, dim3 b)
{
  dim3 result;
  result.x = a.x + b.x;
  result.y = a.y + b.y;
  result.z = a.z + b.z;
  return result;
}

dim2 add(dim2 a, dim2 b)
{
  dim2 result;
  result.x = a.x + b.x;
  result.y = a.y + b.y;
  return result;
}

rgb add(rgb a, rgb b)
{
  rgb result;
  result.r = a.r + b.r;
  result.g = a.g + b.g;
  result.b = a.b + b.b;
  return result;
}

dim3 subtract(dim3 a, dim3 b)
{
  dim3 result;
  result.x = a.x - b.x;
  result.y = a.y - b.y;
  result.z = a.z - b.z;
  return result;
}

rgb subtract(rgb a, rgb b)
{
  rgb result;
  result.r = a.r - b.r;
  result.g = a.g - b.g;
  result.b = a.b - b.b;
  return result;
}

dim3 cross(dim3 a, dim3 b)
{
  dim3 result;
  result.x = a.y*b.z - a.z*b.y;
  result.y = a.z*b.x - a.x*b.z;
  result.z = a.x*b.y - a.y*b.x;
  return result;
}

quaternion mult(quaternion a, quaternion b)
{
  quaternion result;
  result.w = (a.w * b.w) - (a.x * b.x) - (a.y * b.y) - (a.z * b.z);
  result.x = (a.x * b.w) + (a.w * b.x) + (a.y * b.z) - (a.z * b.y);
  result.y = (a.y * b.w) + (a.w * b.y) + (a.z * b.x) - (a.x * b.z);
  result.z = (a.z * b.w) + (a.w * b.z) + (a.x * b.y) - (a.y * b.x);
  return result;
}

quaternion mult(quaternion a, dim3 b)
{
  quaternion result;
  result.w = - (a.x * b.x) - (a.y * b.y) - (a.z * b.z);
  result.x =   (a.w * b.x) + (a.y * b.z) - (a.z * b.y);
  result.y =   (a.w * b.y) + (a.z * b.x) - (a.x * b.z);
  result.z =   (a.w * b.z) + (a.x * b.y) - (a.y * b.x);
  return result;
}

rgb mult(rgb a, rgb b)
{
  rgb result;
  result.r = a.r * b.r;
  result.g = a.g * b.g;
  result.b = a.b * b.b;
  return result;
}

float dot(dim3 a, dim3 b)
{
  return a.x*b.x + a.y*b.y + a.z*b.z;
}

float dot(dim2 a, dim2 b)
{
  return a.x*b.x + a.y*b.y;
}

dim3 scale(dim3 a, float b)
{
  dim3 result;
  result.x = a.x * b;
  result.y = a.y * b;
  result.z = a.z * b;
  return result;
}

dim2 scale(dim2 a, float b)
{
  dim2 result;
  result.x = a.x * b;
  result.y = a.y * b;
  return result;
}

rgb scale(rgb a, float b)
{
  rgb result;
  result.r = a.r * b;
  result.g = a.g * b;
  result.b = a.b * b;
  return result;
}

dim2 normalise(dim2 a)
{
  dim2 result;
  float length = sqrt(a.x*a.x + a.y*a.y);
  result.x = a.x/length;
  result.y = a.y/length;
  return result;
}

dim3 normalise(dim3 a)
{
  dim3 result;
  float length = sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
  result.x = a.x/length;
  result.y = a.y/length;
  result.z = a.z/length;
  return result;
}

quaternion normalise(quaternion a)
{
  quaternion result;
  float length = sqrt(a.x*a.x + a.y*a.y + a.z*a.z + a.w*a.w);
  result.x = a.x/length;
  result.y = a.y/length;
  result.z = a.z/length;
  result.w = a.w/length;
  return result;
}

quaternion conjugate(quaternion a)
{
  quaternion result;
  result.w = a.w;
  result.x = -a.x;
  result.y = -a.y;
  result.z = -a.z;
  return result;
}

dim2 invert(dim2 a)
{
  dim2 result;
  result.set(a.x*-1, a.y*-1);
  return result;
}

dim3 invert(dim3 a)
{
  dim3 result;
  result.x = a.x*-1;
  result.y = a.y*-1;
  result.z = a.z*-1;
  return result;
}

dim3 reflect(dim3 direction, dim3 normal)
{
  return normalise(-2.0f * dot(direction, normal) * normal + direction);
}

dim3 fabs3(dim3 a)
{
  a.x = fabsf(a.x);
  a.y = fabsf(a.y);
  a.z = fabsf(a.z);
  return a;
}

float max3(dim3 a)
{
  float max_val = a.x;
  if(a.y > a.x)
  { max_val = a.y; }
  if(a.z > max_val)
  { max_val = a.z; }

  return max_val;
}

/** Absolute values are used **/
float *biggestDirection3(dim3 *a)
{
  float *max_val = &a->x;
  if(fabsf(a->y) > fabsf(a->x))
  { max_val = &a->y; }
  if(fabsf(a->z) > fabsf(*max_val))
  { max_val = &a->z; }

  return max_val;
}

/** Combines two dim3 into a single dim3 containing their maximum values **/
dim3 maxDim3(dim3 a, dim3 b)
{
  dim3 result;
  result.x = a.x > b.x ? a.x : b.x;
  result.y = a.y > b.y ? a.y : b.y;
  result.z = a.z > b.z ? a.z : b.z;
  return result;
}

dim3 makeDim3(float a, float b, float c)
{
  dim3 result;
  result.x = a;
  result.y = b;
  result.z = c;
  return result;
}

dim2 makeDim2(float a, float b)
{
  dim2 result;
  result.x = a;
  result.y = b;
  return result;
}

float length(dim3 a)
{
  return sqrtf(a.x*a.x + a.y*a.y + a.z*a.z);
}

float length(dim2 a)
{
  return sqrtf(a.x*a.x + a.y*a.y);
}

void printDim2(string text, dim2 a)
{
  printf("%s: x=%.4f, y=%.4f\n", text.c_str(), a.x, a.y);
}

void printDim3(string text, dim3 a)
{
  printf("%s: x=%.4f, y=%.4f, z=%.4f\n", text.c_str(), a.x, a.y, a.z);
}

void clamp(float *a, float min, float max)
{
  if(*a < min)
  { *a = min; }
  else if(*a > max)
  { *a = max; }
}

/** Uses range [0,360) **/
float clampAngleDeg(float a)
{
  while(a > 360)
  { a -= 360; }
  while(a < 0)
  { a += 360; }
  return a;
}

rgb clamp(rgb a)
{
  if(a.r > 1.0)
  { a.r = 1.0; }
  if(a.g > 1.0)
  { a.g = 1.0; }
  if(a.b > 1.0)
  { a.b = 1.0; }
  return a;
}

rgb makeRGB(int name)
{
  rgb colour;
  if(name == BLUE)
  {
    colour.r = 0;
    colour.g = 0;
    colour.b = 1;
    return colour;
  }
  if(name == RED)
  {
    colour.r = 1;
    colour.g = 0;
    colour.b = 0;
    return colour;
  }
  if(name == GREEN)
  {
    colour.r = 0;
    colour.g = 1;
    colour.b = 0;
    return colour;
  }
  if(name == YELLOW)
  {
    colour.r = 1;
    colour.g = 1;
    colour.b = 0;
    return colour;
  }
  if(name == WHITE)
  {
    colour.r = 1;
    colour.g = 1;
    colour.b = 1;
    return colour;
  }
  if(name == BLACK)
  {
    colour.r = 0;
    colour.g = 0;
    colour.b = 0;
    return colour;
  }
  else
  {
    fprintf(stderr, "[makeRGB] Given colour name was not known.\n");
  }
  return colour; // to avoid compiler warnings
}

rgb makeRGB(float r, float g, float b)
{
  rgb colour;
  colour.r = r;
  colour.g = g;
  colour.b = b;
  return colour;
}

rgb repeatRGB(float value)
{
  rgb colour;
  colour.r = value;
  colour.g = value;
  colour.b = value;
  return colour;
}

float percentToPortion(float percent)
{
  return percent * 0.01;
}

bool detectCollisionPointBoundingBox(dim2 point, SDL_Rect box)
{
  if(point.x < box.x)
  { return false; }
  if(point.x >= box.x + box.w)
  { return false; }
  if(point.y < box.y)
  { return false; }
  if(point.y >= box.y + box.h)
  { return false; }
  return true;
}

float detectCollisionCircle(dim2 c1, float r1, dim2 c2, float r2)
{
  /*return computeDistance(c1, c2) - (r1 + r2);*/
  float radiuses_add = r1 + r2;
  return computeDistanceSquared(c1, c2) - (radiuses_add*radiuses_add);
}

/** Expects angles to be in degrees in the range [0,360] **/
bool detectCollisionPointCone(dim2 point, dim2 pos_cone, float range, float degrees, float facing_angle)
{
  float angle = toDegrees(radiansPositiveOnly(computeAngle(point, pos_cone)));
  /*printf("angle: %.2f\n", angle);*/
  float angle_difference = angleDifference(angle, facing_angle);
  /*printf("difference: %.2f\n", angle_difference);*/
  if(angle_difference > degrees * 0.5f)
  { return false; } // Point is not in the field of view

  float distance = computeDistance(point, pos_cone);
  if(distance > range)
  { return false; } // Point is out of range
  return true;
}

/** Expects angles to be in degrees in the range [0,360] **/
bool detectCollisionLineCone(dim2 A, dim2 B, int precision, dim2 pos_cone, float range, float degrees, float facing_angle, float *closest_distance)
{
  /** More efficient would be to calculate intersections of line-circle first and see if those are in the cone **/
  bool intersecting = false;
  *closest_distance = 500000;
  dim2 line_vector = B - A;
  float line_length = length(line_vector);
  dim2 step = line_vector / (float)precision;
  dim2 point = A;
  while(length(point - A) < line_length)
  {
    if(detectCollisionPointCone(point, pos_cone, range, degrees, facing_angle))
    {
      intersecting = true;
      float distance = computeDistance(point, pos_cone);
      if(distance < *closest_distance)
      { *closest_distance = distance; }
    }
    point = point + step;
  }
  return intersecting;
}

float computeDistance(dim2 target, dim2 center)
{
  dim2 delta = target - center;
  return sqrtf(delta.x*delta.x + delta.y*delta.y);
}

float computeDistanceSquared(dim2 target, dim2 center)
{
  dim2 delta = target - center;
  return delta.x*delta.x + delta.y*delta.y;
}

float computeDistance(dim3 target, dim3 center)
{
  dim3 delta = target - center;
  return sqrtf(delta.x*delta.x + delta.y*delta.y + delta.z*delta.z);
}

float distancePointToLine(dim2 target, dim2 a, dim2 b)
{
  dim2 ab = b - a;
  dim2 ac = target - a;
  dim3 AB = makeDim3(ab.x, ab.y, 0);
  dim3 AC = makeDim3(ac.x, ac.y, 0);
  return length(cross(AB, AC)) / length(AB);
}

/** http://stackoverflow.com/questions/849211/shortest-distance-between-a-point-and-a-line-segment **/
float distancePointToLineSegment(dim2 p, dim2 v, dim2 w)
{
  // Return minimum distance between line segment vw and point p
  float l = length(w - v);
  float l2 = l*l; // i.e. |w-v|^2 -  avoid a sqrt
  if(l2 == 0.0)
  { return computeDistance(p, v); }   // v == w case

  // Consider the line extending the segment, parameterized as v + t (w - v).
  // We find projection of point p onto the line.
  // It falls where t = [(p-v) . (w-v)] / |w-v|^2
  float t = dot(p - v, w - v) / l2;
  if(t < 0.0)
  { return computeDistance(p, v); } // Beyond the 'v' end of the segment
  else if (t > 1.0)
  { return computeDistance(p, w); } // Beyond the 'w' end of the segment
  dim2 projection = v + t * (w - v);  // Projection falls on the segment
  return computeDistance(p, projection);
}

dim2 projectPointOntoLineSegment(dim2 p, dim2 v, dim2 w)
{
  float l = length(w - v);
  float l2 = l*l; // i.e. |w-v|^2 -  avoid a sqrt
  if(l2 == 0.0)
  { return v; }   // v == w case

  // Consider the line extending the segment, parameterized as v + t (w - v).
  // We find projection of point p onto the line.
  // It falls where t = [(p-v) . (w-v)] / |w-v|^2
  float t = dot(p - v, w - v) / l2;
  if(t < 0.0)
  { return v; } // Beyond the 'v' end of the segment
  else if (t > 1.0)
  { return w; } // Beyond the 'w' end of the segment
  return v + t * (w - v);  // Projection falls on the segment;
}

/** Expects an angle in degrees in the range [0,360] **/
float angleDifference(float a, float b)
{
  float result = fabsf(a - b);
  if(result > 180.0)
  { return 360.0 - result; }
  return result;
}

/** Transforms a Cartesian x,y-coordinate into an angle (in radians). **/
float computeAngle(dim2 target, dim2 center)
{
  float x = target.x - center.x;
  float y = center.y - target.y;
  return atan2f(y,x);
}

float toDegrees(float rad)
{
  return rad * 180 / PI;
}

float toRadians(float deg)
{
  return deg * PI / 180;
}

/** Translate radian value from [-pi,pi] to [0,2pi] **/
float radiansPositiveOnly(float rad)
{
  if(rad < 0)
  { return rad + 2.0*PI; }

  return rad;
}

vector<dim2> convertPixelToDim2(vector<pixel> a)
{
  vector<dim2> result;
  for(unsigned int i=0; i < a.size(); i++)
  {
    dim2 element;
    element.set(a[i].x, a[i].y);
    result.push_back(element);
  }
  return result;
}

dim2 convertPixelToDim2(pixel a)
{
  dim2 result;
  result.set(a.x, a.y);
  return result;
}

bool pointInPolygon(dim2 point, vector<dim2> polygon)
{
  int j;
  float x, y;
  bool oddNodes = false;
  j = polygon.size() - 1;
  x = point.x;
  y = point.y;

  for(unsigned int i=0; i < polygon.size(); i++)
  {
    if(((polygon[i].y < y && polygon[j].y >= y) || (polygon[j].y < y && polygon[i].y >= y)) &&
       (polygon[i].x <= x || polygon[j].x <= x))
    {
      if (polygon[i].x + (y - polygon[i].y) / (polygon[j].y - polygon[i].y) * (polygon[j].x - polygon[i].x) < x)
      {
        oddNodes =! oddNodes;
      }
    }
    j = i;
  }

  return oddNodes;
}

float calculateMean(vector<float> a)
{
  if(a.size() == 0)
  { return 0.0f; }

  float mean = 0.0f;
  for(unsigned int i=0; i < a.size(); i++)
  {
    mean += a[i];
  }
  return mean / a.size();
}

/** http://www.wikihow.com/Calculate-Variance **/
float calculateVariance(vector<float> a, float mean)
{
  if(a.size() <= 1)
  { return 0.0f; }

  float var = 0;
  float element;
  for(unsigned int i=0; i < a.size(); i++)
  {
    element = (mean - a[i]);
    var += element * element;
  }
  return var / (a.size() - 1);
}

vector<string> splitString(string s, char token)
{
  vector<string> split_string;

  stringstream ss(s);
  string item;
  while(getline(ss, item, token))
  {
    if(item != "")
    { split_string.push_back(item); } // Avoid empty strings
  }
  return split_string;
}

/* http://sdl.beuc.net/sdl.wiki/Pixel_Access */
void putPixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
  int bpp = surface->format->BytesPerPixel/* / 8*/;
  /* Here p is the address to the pixel we want to set */
  Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

  switch(bpp)
  {
    case 1:
      *p = pixel;
      break;

    case 2:
      *(Uint16 *)p = pixel;
      break;

    case 3:
      if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
      {
        p[0] = (pixel >> 16) & 0xff;
        p[1] = (pixel >> 8) & 0xff;
        p[2] = pixel & 0xff;
      }
      else
      {
        p[0] = pixel & 0xff;
        p[1] = (pixel >> 8) & 0xff;
        p[2] = (pixel >> 16) & 0xff;
      }
      break;

    case 4:
      *(Uint32 *)p = pixel;
      break;
  }
}

void colourPixel(SDL_Surface *surface, int x, int y, rgb colour, float alpha)
{
  Uint32 pixel = SDL_MapRGBA(surface->format, colour.r*255, colour.g*255, colour.b*255, alpha*255);
  if(SDL_MUSTLOCK(surface))
  { SDL_LockSurface(surface); }
  putPixel(surface, x, y, pixel);
  if(SDL_MUSTLOCK(surface))
  { SDL_UnlockSurface(surface); }
}

Uint32 getPixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;
        break;

    case 2:
        return *(Uint16 *)p;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
        break;

    case 4:
        return *(Uint32 *)p;
        break;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

rgb getPixelColour(SDL_Surface *surface, int x, int y)
{
  Uint32 pixel = getPixel(surface, x , y);
  Uint8 r, g, b;
  SDL_GetRGB(pixel, surface->format, &r, &g, &b);
  rgb colour;
  colour.set(r/255.0,g/255.0,b/255.0);
  return colour;
}

void makeColourTransparent(SDL_Surface *surface, rgb colour)
{
  int r = colour.r * 255;
  int g = colour.g * 255;
  int b = colour.b * 255;
  SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGBA(surface->format, r, g, b, 255));
}

float randomFloat(float min, float max, float precision)
{
	float divisor = 1/precision;
	return randInt(int(min*divisor),int(max*divisor)) / divisor;
}













