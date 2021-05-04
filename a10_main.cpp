#include <iostream>

#include "a10.h"
#include "Image.h"

using namespace std;

// Tests a simple brush consisting of a single point.
// The resulting image should be black with a white pixel in the center.
void testPointBrush() {
  Image blackImage(5, 5, 1);
  Image texture(1, 1, 1);
  const float whiteColor[] = {1.0f};
  texture(0, 0, 0) = 1.0f;
  brush(blackImage, 2, 2, whiteColor, texture);
  blackImage.write("Output/point-brush.png");
};

// Tests overlapping brush strokes with some transparency and different colors.
// The resulting image should be red, green, and blue strokes slightly offset from each other. Note that the colors should blend a little bit.
void testColoredBrushStrokes() {
  Image blackImage(100, 100, 3);
  Image texture = Image("Input/brush-fade.png");
  const float redColor[] = {1.0f, 0.0f, 0.0f};
  const float greenColor[] = {0.0f, 1.0f, 0.0f};
  const float blueColor[] = {0.0f, 0.0f, 1.0f};
  brush(blackImage, 40, 50, redColor, texture);
  blackImage.write("Output/red-stroke.png");
  brush(blackImage, 50, 50, greenColor, texture);
  blackImage.write("Output/red-green-strokes.png");
  brush(blackImage, 60, 50, blueColor, texture);
  blackImage.write("Output/colored-strokes.png");
};

// Tests brush strokes which are too close to the edge of an image.
// The resulting image should be entirely black.
void testBrushEdge() {
  Image blackImage(5, 5, 3);
  Image texture(3, 3, 1);
  const float whiteColor[] = {1.0f, 1.0f, 1.0f};
  texture(0, 0, 0) = 1.0f;
  brush(blackImage, 1, 3, whiteColor, texture); // top side
  brush(blackImage, 4, 3, whiteColor, texture); // bottom side
  brush(blackImage, 3, 1, whiteColor, texture); // left side
  brush(blackImage, 3, 4, whiteColor, texture); // right side
  blackImage.write("Output/edge-case.png");
};

// Tests single scale painting with a red image.
// The resulting image should be a black background with a few red brush strokes.
void testSingleScaleSimple() {
  Image blackImage(100, 100, 3);
  Image whiteImage(100, 100, 3);
  for (int x = 0; x < whiteImage.width(); x++) {
    for (int y = 0; y < whiteImage.height(); y++) {
      whiteImage(x, y, 0) = 1.0;
      whiteImage(x, y, 1) = 1.0;
      whiteImage(x, y, 2) = 1.0;
    }
  }
  Image redImage(100, 100, 3);
  for (int x = 0; x < redImage.width(); x++) {
    for (int y = 0; y < redImage.height(); y++) {
      redImage(x, y, 0) = 1.0;
    }
  }
  Image texture = Image("Input/brush.png");
  singleScalePaint(redImage, blackImage, whiteImage, texture, 100, 4);
  blackImage.write("Output/single-scale-red-simple.png");
}

// Test single scale painting with scaled texture.
// The resulting image should be a black background with a few small red brush strokes.
void testSingleScaleScaling() {
  Image blackImage(100, 100, 3);
  Image whiteImage(100, 100, 3);
  for (int x = 0; x < whiteImage.width(); x++) {
    for (int y = 0; y < whiteImage.height(); y++) {
      whiteImage(x, y, 0) = 1.0;
      whiteImage(x, y, 1) = 1.0;
      whiteImage(x, y, 2) = 1.0;
    }
  }
  Image redImage(100, 100, 3);
  for (int x = 0; x < redImage.width(); x++) {
    for (int y = 0; y < redImage.height(); y++) {
      redImage(x, y, 0) = 1.0;
    }
  }
  Image texture = Image("Input/brush.png");
  singleScalePaint(redImage, blackImage, whiteImage, texture, 20, 4);
  blackImage.write("Output/single-scale-red-scaling.png");
}

// Test single scale painting with noise.
// The resulting image should be a black background with a few brush strokes of random colors.
void testSingleScaleNoisy() {
  Image blackImage(100, 100, 3);
  Image whiteImage(100, 100, 3);
  for (int x = 0; x < whiteImage.width(); x++) {
    for (int y = 0; y < whiteImage.height(); y++) {
      whiteImage(x, y, 0) = 1.0;
      whiteImage(x, y, 1) = 1.0;
      whiteImage(x, y, 2) = 1.0;
    }
  }
  Image grayImage(100, 100, 3);
  for (int x = 0; x < grayImage.width(); x++) {
    for (int y = 0; y < grayImage.height(); y++) {
      grayImage(x, y, 0) = .5;
      grayImage(x, y, 1) = .5;
      grayImage(x, y, 2) = .5;
    }
  }
  Image texture = Image("Input/brush.png");
  singleScalePaint(grayImage, blackImage, whiteImage, texture, 100, 4, 100);
  blackImage.write("Output/single-scale-gray-noisy.png");
}

// Test single scale painting when the requested texture size is larger than actual size.
// The resulting image should be a black background with a few brush strokes of the actual brush size (the brush should not have been made bigger).
void testSingleScaleTooBigScaling() {
  Image blackImage(100, 100, 3);
  Image redImage(100, 100, 3);
  Image whiteImage(100, 100, 3);
  for (int x = 0; x < whiteImage.width(); x++) {
    for (int y = 0; y < whiteImage.height(); y++) {
      whiteImage(x, y, 0) = 1.0;
      whiteImage(x, y, 1) = 1.0;
      whiteImage(x, y, 2) = 1.0;
    }
  }
  for (int x = 0; x < redImage.width(); x++) {
    for (int y = 0; y < redImage.height(); y++) {
      redImage(x, y, 0) = 1.0;
    }
  }
  Image texture = Image("Input/brush.png");
  singleScalePaint(redImage, blackImage, whiteImage, texture, 100, 100);
  blackImage.write("Output/single-scale-red-too-big-scaling.png");
}

// Test single scale painting with an importance map, where only the top left corner of the image is important.
// The resulting image should be a black background with brushstrokes in the upper left and nowhere else.
void testSingleScaleImportance() {
  Image blackImage(100, 100, 3);
  Image redImage(100, 100, 3);
  Image topLeft(100, 100, 3);
  for (int x = 0; x < topLeft.width()/2; x++) {
    for (int y = 0; y < topLeft.height()/2; y++) {
      topLeft(x, y, 0) = 1.0;
      topLeft(x, y, 1) = 1.0;
      topLeft(x, y, 2) = 1.0;
    }
  }
  for (int x = 0; x < redImage.width(); x++) {
    for (int y = 0; y < redImage.height(); y++) {
      redImage(x, y, 0) = 1.0;
    }
  }
  Image texture = Image("Input/brush.png");
  singleScalePaint(redImage, blackImage, topLeft, texture);
  blackImage.write("Output/single-scale-importance.png");
}

// Test a "painterly" style (big brush for base layer, small brush for details).
// The resulting image should be Boston in a painterly style.
void testPainterlyBoston() {
  Image boston = Image("Input/boston.png");
  Image texture = Image("Input/brush-fade.png");
  Image painterlyBoston = painterly(boston, texture);
  painterlyBoston.write("Output/painterly-boston.png");
}

// Test a "painterly" style.
// The resulting image should be Ville Perdue in a painterly style.
void testPainterlyVillePerdue() {
  Image villePerdue = Image("Input/villeperdue.png");
  Image texture = Image("Input/brush.png");
  Image painterlyVillePerdue = painterly(villePerdue, texture);
  painterlyVillePerdue.write("Output/painterly-villeperdue.png");
}

// Test single scale with brush orientations.
// The input image is a black background with white horizontal lines.
// The output image should have horizontal paint strokes.
void testSingleScaleOrientedLines() {
  Image lines = Image("Input/horizontal-lines.png");
  Image out(lines.width(), lines.height(), lines.channels());
  Image texture = Image("Input/longBrush.png");
  Image importance(lines.width(), lines.height(), 1);
  for (int i = 0; i < importance.number_of_elements(); i++) {
    importance(i) = 1.0f;
  }
  singleScaleOrientedPaint(lines, out, importance, texture);
  out.write("Output/single-scale-oriented-lines.png");
}

// Test single scale with 4 brush orientations.
// The output image should have only vertical and horizontal paint strokes.
void testSingleScaleOrientedVillePerdue4() {
  Image villeperdue = Image("Input/villeperdue.png");
  Image out(villeperdue.width(), villeperdue.height(), villeperdue.channels());
  Image texture = Image("Input/longBrush.png");
  Image importance(villeperdue.width(), villeperdue.height(), 1);
  for (int i = 0; i < importance.number_of_elements(); i++) {
    importance(i) = 1.0f;
  }
  singleScaleOrientedPaint(villeperdue, out, importance, texture, 50, 10000, 0.3, 4);
  out.write("Output/single-scale-oriented-villeperdue-4.png");
}

// Test single scale with 36 (default number) orientations.
// The output image should have many different angles of paint strokes.
void testSingleScaleOrientedVillePerdue36() {
  Image villeperdue = Image("Input/villeperdue.png");
  Image out(villeperdue.width(), villeperdue.height(), villeperdue.channels());
  Image texture = Image("Input/longBrush.png");
  Image importance(villeperdue.width(), villeperdue.height(), 1);
  for (int i = 0; i < importance.number_of_elements(); i++) {
    importance(i) = 1.0f;
  }
  singleScaleOrientedPaint(villeperdue, out, importance, texture, 50, 10000, 0.3);
  out.write("Output/single-scale-oriented-villeperdue-36.png");
}

// Test oriented paint on Boston
void testOrientedPaintBoston() {
  Image boston = Image("Input/boston.png");
  Image texture = Image("Input/longBrush.png");
  clock_t start = clock();
  Image out = orientedPaint(boston, texture);
  clock_t end = clock();
  double duration_sep = (end - start) * 1.0f / CLOCKS_PER_SEC;
  cout << "oriented paint on Boston took: " << duration_sep << "s" << endl;
  out.write("Output/oriented-paint-boston.png");
}

// Test oriented paint on Ville Perdue
void testOrientedPaintVillePerdue() {
  Image villeperdue = Image("Input/villeperdue.png");
  Image texture = Image("Input/longBrush2.png");
  clock_t start = clock();
  Image out = orientedPaint(villeperdue, texture);
  clock_t end = clock();
  double duration_sep = (end - start) * 1.0f / CLOCKS_PER_SEC;
  cout << "oriented paint on Ville Perdue took: " << duration_sep << "s" << endl;
  out.write("Output/oriented-paint-villeperdue.png");
}

// Make Simmons pretty :D
void testPrettyPaintSimmons() {
  Image simmons = Image("Input/simmons.png");
  Image out(simmons.width(), simmons.height(), simmons.channels());
  Image opaque = Image("Input/longBrush2.png");
  Image slightFade = Image("Input/longBrush2-slight-fade.png");
  Image fade = Image("Input/longBrush2-fade.png");
  // Create a constant importance map
  Image importance(simmons.width(), simmons.height(), 1);
  for (int x = 0; x < importance.width(); x++) {
    for (int y = 0; y < importance.height(); y++) {
      importance(x, y, 0) = 1.0f;
    }
  }
  // Get base (low frequency) with opaque brush
  singleScaleOrientedPaint(simmons, out, importance, opaque, 50, 70000);
  // Add a layer with the faded brush for better blurring and nicer appearance
  singleScaleOrientedPaint(simmons, out, importance, fade, 50, 50000);
  // Get details (high frequency)
  Image lowPass = gaussianBlur_separable(simmons, 9.0, 3.0, true);
  Image highPass = simmons - lowPass;
  singleScaleOrientedPaint(simmons, out, highPass, slightFade, 30, 50000);
  out.write("Output/simmons-painted.png");
}

int main() {
  
  // Test the brush method.
  testPointBrush();
  testColoredBrushStrokes();
  testBrushEdge();
  
  // Test the single scale paint method with no importance.
  testSingleScaleSimple();
  testSingleScaleScaling();
  testSingleScaleNoisy();
  testSingleScaleTooBigScaling();
  
  // Test the single scale paint method with importance.
  testSingleScaleImportance();
  
  // Test painterly rendering
  testPainterlyBoston();
  testPainterlyVillePerdue();
  
  // Test single scale paint method with orientation.
  testSingleScaleOrientedLines();
  testSingleScaleOrientedVillePerdue4();
  testSingleScaleOrientedVillePerdue36();
  
  // Test two-scale ("painterly") method with orientation.
  testOrientedPaintBoston();
  testOrientedPaintVillePerdue();
  
  // Make a pretty painting of Simmons :)
  testPrettyPaintSimmons();
   
  return EXIT_SUCCESS;
};
