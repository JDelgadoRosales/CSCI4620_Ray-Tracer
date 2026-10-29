/// Include the image write library and enable it
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

/// Uncomment these lines if you want to debug using couts
#include <iostream>
#include <cmath> //added
using namespace std;

/// Uncomment these lines if you happen to add a Vector3 and want to debug :)

// This holds a vertex (a position) or a vector (a direction) in 3D
class Vector3
{
public:
  float x, y, z;

  Vector3 &operator+=(const Vector3 &rhs)
  {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
  }

  Vector3 &operator-=(const Vector3 &rhs)
  {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
  }

  Vector3 &operator*=(const float rhs)
  {
    x *= rhs;
    y *= rhs;
    z *= rhs;
    return *this;
  }

  // Measures how similar two directions are
  float dot(const Vector3 &rhs)
  {
    return x * rhs.x + y * rhs.y + z * rhs.z;
  }

  // Gives a vector that's perpendicular to both, used for normals and areas
  Vector3 cross(const Vector3 &rhs)
  {
    return  Vector3{
        y * rhs.z - z * rhs.y,
        z * rhs.x - x * rhs.z,
        x * rhs.y - y * rhs.x};
  }

  float length()
  {
    return sqrt(x * x + y * y + z * z);
  }

  // Keeps the same direction but makes the length 1
  Vector3 normalize()
  {
    float l = length();
    // cout << l << endl;
    return Vector3{x / l, y / l, z / l};
  }
};

inline Vector3 operator+(Vector3 lhs, const Vector3 &rhs) // removed & from operator
{
  return lhs += rhs;
}

inline Vector3 operator-(Vector3 lhs, const Vector3 &rhs) // removed & from operator
{
  return lhs -= rhs;
}

inline Vector3 operator*(Vector3 lhs, const float rhs) // removed & from operator
{
  return lhs *= rhs;
}

// Prints a vector with cout
inline std::ostream &operator<<(std::ostream &os, const Vector3 &v)
{
  return os << '(' << v.x << ", " << v.y << ", " << v.z << ')';
}

// A triangle is just its three corners
class Triangle{
  public:
  Vector3 one, two, three;
};

int main()
{
  /// Example of how to debug with couts.
  /// You must uncomment the include and namespace lines above
  // cout << "Hello, world" << endl;

  // A quick test print, it isn't part of the render
  Vector3 temp = Vector3{0, 0, 0};
  temp += Vector3{1, 1, 1};

  cout << temp << endl;

  /// Width of the final image
  const int w = 640;

  /// Height of the final image
  const int h = 640;

  /// The 1d array for storing the pixel we have generated
  unsigned char pixels[w * h * 3];

  Triangle triangle = Triangle{Vector3{-1, 1, 0}, Vector3{-1, -1, 0}, Vector3{1, -1, 0}};

  // This is the color used when a ray doesn't hit anything
  Vector3 background_color = Vector3{150, 0, 255};

  /// Loop over each line doing down
  for (int y = 0; y < h; y++)
  {
    /// While going down, loop over every pixel going right
    for (int x = 0; x < w; x++)
    {

      /// Figure out our index in the 1d array of pixels
      int index = y * w * 3 + x * 3;

      // Every pixel starts as the background and only changes if the ray hits the triangle

      /// The value of the red channel (0-255)
      unsigned char r = background_color.x;

      /// The value of the green channel (0-255)
      unsigned char g = background_color.y;

      /// The value of the blue channel (0-255)
      unsigned char b = background_color.z;

      // The camera sits at (0, 0, 1), and look_at and up aren't being used yet
      Vector3 camera_origin = Vector3{0, 0, 1};
      Vector3 camera_look_at = Vector3{0, 0, 0};
      Vector3 camera_up = Vector3{0, 1, 0};

      // Turn this pixel into a point between -1 and 1 on the image plane (z = 0), flipping y so up stays up
      Vector3 target{
        x/(float)w*2-1, -(y/(float)h*2-1), 0
      };

      // The ray goes from the camera toward this pixel, scaled to a length of 1
      // cout<<"One"<<endl;
      Vector3 temp = (target - camera_origin);
      // cout<<temp<<endl;
      Vector3 ray = temp.normalize();
      // cout << ray << endl;
      // cout<<"Two"<<endl;

      // Find the triangle's plane: the cross product gives the normal (abc), then solve for d
      Vector3 edge_one = triangle.one - triangle.two;
      Vector3 edge_two = triangle.three - triangle.two;
      Vector3 p = edge_two.cross(edge_one);
      Vector3 abc = p.normalize();
      float d = -abc.dot(triangle.one);

      // This is how far the ray travels before it reaches the plane
      float t = -(abc.dot(camera_origin)+d)/abc.dot(ray);

      // cout << abc << " " << d << " " << t << endl;

      // This is the exact spot where the ray hits the plane
      Vector3 collision_point = ray*t+camera_origin;

      // To check if the point is inside, split the triangle into three smaller ones around it.
      // If their areas add up to the whole triangle, the point is inside.
      Vector3 minus_one = collision_point - triangle.one;
      Vector3 minus_two = collision_point - triangle.two;
      Vector3 minus_three = collision_point - triangle.three;

      float triangle_area = p.length()/2;

      float area_one = minus_two.cross(minus_three).length()/2;
      float area_two = minus_three.cross(minus_one).length()/2;
      float area_three = minus_one.cross(minus_two).length()/2;

      // Each corner's share of the triangle, from 0 to 1
      float u = area_one/triangle_area;
      float v = area_two/triangle_area;
      float a = area_three/triangle_area;

      float sum = u + v + a;

      // How far the three shares are from adding up to exactly 1
      float diff = abs(1-sum);

      // A tiny bit of error is allowed, since floats are never exact
      if(diff > .00001){
        //I'm not in the triangle
      }
      else{
        //I'm in the triangle

        // Blend the corner colors together: yellow at one, green at two, and red at three
        r = 255 * u + 0 * v + 255 * a;
        g = 255 * u + 255 * v + 0 * a;
        b = 0 * u + 0 * v + 0 * a;
      }

      /// Assign the r, g, and b values to the pixel at index
      pixels[index] = r;
      pixels[index + 1] = g;
      pixels[index + 2] = b;

    }

  }

  /// Write the final image as a png
  stbi_write_png("image.png", w, h, 3, pixels, w * 3);
  return 0;
}
