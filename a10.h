#ifndef A10_H_PHUDVTKB
#define A10_H_PHUDVTKB

#include <stdlib.h>

#include "basicImageManipulation.h"
#include "filtering.h"
#include "matrix.h"
#include "Image.h"

void brush(Image & out, const int x, const int y, const float color[], const Image & texture);

void singleScalePaint(const Image & im, Image & out, const Image & importance, const Image & texture, const int size=10, const int N=1000, const float noise=0.3);

Image painterly(const Image & im, const Image & texture, const int size=50, const int N=10000, const float noise=0.3);

Image computeAngles(const Image & im);

void singleScaleOrientedPaint(const Image & im, Image & out, const Image & importance, const Image & texture, const int size=10, const int N=1000, const float noise=0.3, const int nAngles=36);

Image orientedPaint(const Image & im, const Image & texture, const int size=50, const int N=7000, const float noise=0.3);

#endif /* end of include guard: A10_H_PHUDVTKB */

