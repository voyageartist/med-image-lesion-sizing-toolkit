/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef itkCannyEdgesDistanceAdvectionFieldFeatureGenerator_h
#define itkCannyEdgesDistanceAdvectionFieldFeatureGenerator_h

#include "itkFeatureGenerator.h"
#include "itkImage.h"
#include "itkImageSpatialObject.h"
#include "itkCastImageFilter.h"
#include "itkCannyEdgeDetectionRecursiveGaussianImageFilter.h"
#include "itkSignedMaurerDistanceMapImageFilter.h"
#include "itkGradientImageFilter.h"
#include "itkMultiplyImageFilter.h"

namespace itk
{

/** \class CannyEdgesDistanceAdvectionFieldFeatureGenerator
 * \brief Generates an advection feature field by computing the distance
 * map to the canny edges in the image and modulating it with the
 * gradient vectors of the distance map.
 *
 * \par Overview
 * The class generates features that can be used as the advection term for
 * computing a canny level set. The class takes an input image
 *
 *    Input -> CastToFloat -> DistanceMap  = ImageA
 *    ImageA -> Gradient = ImageB (of covariant vectors)
 *
 *   Advection Field = ImageA * ImageB
 *
 * The resulting feature is an image of covariant vectors and is ideally used
 * as the advection term for a level set segmentation module. The term
 * advects the level set along the gradient of the distance map, helping it
 * lock onto the edges (which are extracted by the canny filter).
 *
 * There are two parameters to this feature generator.
 * (1) UpperThreshold/LowerThreshold: These set the thresholding values of
 *     the Canny edge detection. The canny algorithm incorporates a
 *     hysteresis thresholding which is applied to the gradient magnitude
 *     of the smoothed image to find edges.
 * (2) Variance.  Controls the smoothing paramter of the gaussian filtering
 *     done during Canny edge detection. The first step of canny edge
 *     detection is to smooth the input with