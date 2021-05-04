#include <iostream>
#include <stdlib.h>

#include "a10.h"
#include "basicImageManipulation.h"
#include "filtering.h"
#include "matrix.h"
#include "Image.h"


using namespace std;

namespace {
  float randomFloat() {
    return ((float)rand())/RAND_MAX;
  }
  int randomIntInRange(const int high) {
    return rand() % high;
  }
  Image computeTensor(const Image &im, float sigmaG, float factorSigma) {
    // // --------- HANDOUT  PS07 ------------------------------
    // Compute xx/xy/yy Tensor of an image. (stored in that order)
    const vector<Image> lumChrom = lumiChromi(im);
    const Image blurredLuminance = gaussianBlur_separable(lumChrom.at(0), sigmaG);
    const Image gradX = gradientX(blurredLuminance);
    const Image gradY = gradientY(blurredLuminance);
    Image tensor(im.width(), im.height(), 3);
    for (int x = 0; x < im.width(); x++) {
      for (int y = 0; y < im.height(); y++) {
        tensor(x, y, 0) = gradX(x, y, 0)*gradX(x, y, 0);
        tensor(x, y, 1) = gradX(x, y, 0)*gradY(x, y, 0);
        tensor(x, y, 2) = gradY(x, y, 0)*gradY(x, y, 0);
      }
    }
    return gaussianBlur_separable(tensor, sigmaG*factorSigma);
  }
  vector<Image> rotateBrushes(const Image & texture, const int nAngles) {
    vector<Image> result;
    result.push_back(texture);
    for (int i = 1; i < nAngles; i++) {
      result.push_back(rotate(result.at(i-1), 2.0f*M_PI/nAngles));
    }
    return result;
  }
}

void brush(Image & out, const int x, const int y, const float color[], const Image & texture) {
  if (y <= texture.height()/2 ||
      (out.height() - y) <= texture.height()/2 ||
      x <= texture.width()/2 ||
      (out.width() - x) <= texture.width()/2) {
    return;
  }
  for (int textureY = 0; textureY < texture.height(); textureY++) {
    for (int textureX = 0; textureX < texture.width(); textureX++) {
      const int xCoord = x + textureX - texture.width()/2;
      const int yCoord = y + textureY - texture.height()/2;
      const float opacity = texture(textureX, textureY, 0);
      for (int c = 0; c < out.channels(); c++) {
        out(xCoord, yCoord, c) = color[c]*opacity + (1.0f - opacity)*out(xCoord, yCoord, c);
      }
    }
  }
}

void singleScalePaint(const Image & im, Image & out, const Image & importance, const Image & texture, const int size, const int N, const float noise) {
  // Scale the texture so that it has maximum size size
  Image scaledTexture = Image(1, 1, 1);
  if (texture.width() <= size && texture.height() <= size) {
    scaledTexture = texture;
  } else {
    scaledTexture = scaleBicubic(texture, ((float) size)/max(texture.width(), texture.height()), 1.0f/3.0f, 1.0f/3.0f);
  }
  int numSplats = 0;
  while (numSplats < N) {
    const int x = randomIntInRange(im.width());
    const int y = randomIntInRange(im.height());
    if (importance(x, y) >= randomFloat()) {
      const float color[] = {im(x, y, 0)*(1.0f-noise/2.0f+noise*randomFloat()), im(x, y, 1)*(1.0f-noise/2.0f+noise*randomFloat()), im(x, y, 2)*(1.0f-noise/2.0f+noise*randomFloat())};
      brush(out, x, y, color, scaledTexture);
      numSplats++;
    }
  }
}

Image painterly(const Image & im, const Image & texture, const int size, const int N, const float noise) {
  // Create a constant importance map
  Image importance(im.width(), im.height(), 1);
  for (int x = 0; x < importance.width(); x++) {
    for (int y = 0; y < importance.height(); y++) {
      importance(x, y, 0) = 1.0f;
    }
  }
  Image out(im.width(), im.height(), im.channels());
  // Get base (low frequency)
  singleScalePaint(im, out, importance, texture, size, N, noise);
  // Get details (high frequency)
  Image lowPass = gaussianBlur_separable(im, 9.0, 3.0, true);
  Image highPass = im - lowPass;
  singleScalePaint(im, out, highPass, texture, size/4, N, noise);
  return out;
}

Image computeAngles(const Image & im) {
  Image angles(im.width(), im.height(), 1);
  Image tensor = computeTensor(im, 1, 4);
  for (int x = 0; x < tensor.width(); x++) {
    for (int y = 0; y < tensor.height(); y++) {
      // Make a matrix of the form:
      // [ gradX^2       gradX*gradY ]
      // [ gradX*gradY   gradY^2     ]
      Eigen::Matrix2f H;
      H(0, 0) = tensor(x, y, 0);
      H(1, 0) = tensor(x, y, 1);
      H(0, 1) = tensor(x, y, 1);
      H(1, 1) = tensor(x, y, 2);
      // Make a self-adjoint Eigen solver
      Eigen::SelfAdjointEigenSolver<Eigen::Matrix2f> solver;
      // Compute before fetching eigenvalues / eigenvectors
      solver.compute(H);
      // Eigenvalues and eigenvectors are in ascending sorted order; 1st <-> smallest
      Vec2f eigenvector = solver.eigenvectors().col(0);
      // Store the angles in an Image
      angles(x, y) = atan2(eigenvector.y(), eigenvector.x());
//      cout << eigenvector << endl;
//      cout << angles(x, y) << endl;
    }
  }
  return angles;
}

void singleScaleOrientedPaint(const Image & im, Image & out, const Image & importance, const Image & texture, const int size, const int N, const float noise, const int nAngles) {
  // Scale the texture so that it has maximum size size
  Image scaledTexture = Image(1, 1, 1);
  if (texture.width() <= size && texture.height() <= size) {
    scaledTexture = texture;
  } else {
    scaledTexture = scaleBicubic(texture, ((float) size)/max(texture.width(), texture.height()), 1.0f/3.0f, 1.0f/3.0f);
  }
  // Get the rotated & scaled brushes
  vector<Image> rotatedBrushes = rotateBrushes(scaledTexture, nAngles);
  // Get the angles of the lines in the image
  Image angles = computeAngles(im);
  int numSplats = 0;
  while (numSplats < N) {
    const int x = randomIntInRange(im.width());
    const int y = randomIntInRange(im.height());
    if (importance(x, y) >= randomFloat()) {
      const float color[] = {im(x, y, 0)*(1.0f-noise/2.0f+noise*randomFloat()), im(x, y, 1)*(1.0f-noise/2.0f+noise*randomFloat()), im(x, y, 2)*(1.0f-noise/2.0f+noise*randomFloat())};
      float normalizedAngle = angles(x, y);
      if (normalizedAngle < 0) {
        normalizedAngle += 2*M_PI;
      }
      int rotatedBrushIndex = ((int)round(normalizedAngle * ((float)nAngles)/(2*M_PI))) % nAngles;
      brush(out, x, y, color, rotatedBrushes.at(rotatedBrushIndex));
      numSplats++;
    }
  }
}

Image orientedPaint(const Image & im, const Image & texture, const int size, const int N, const float noise) {
  // Create a constant importance map
  Image importance(im.width(), im.height(), 1);
  for (int x = 0; x < importance.width(); x++) {
    for (int y = 0; y < importance.height(); y++) {
      importance(x, y, 0) = 1.0f;
    }
  }
  Image out(im.width(), im.height(), im.channels());
  // Get base (low frequency)
  singleScaleOrientedPaint(im, out, importance, texture, size, N, noise);
  // Get details (high frequency)
  Image lowPass = gaussianBlur_separable(im, 9.0, 3.0, true);
  Image highPass = im - lowPass;
  singleScaleOrientedPaint(im, out, highPass, texture, size/4, N, noise);
  return out;
}
