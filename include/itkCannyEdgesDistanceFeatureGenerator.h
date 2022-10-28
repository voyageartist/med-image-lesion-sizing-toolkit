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
#ifndef itkCannyEdgesDistanceFeatureGenerator_h
#define itkCannyEdgesDistanceFeatureGenerator_h

#include "itkFeatureGenerator.h"
#include "itkImage.h"
#include "itkImageSpatialObject.h"
#include "itkCastImageFilter.h"
#include "itkCannyEdgeDetectionRecursiveGaussianImageFilter.h"
#include "itkSignedMaurerDistanceMapImageFilter.h"
#include "itkFixedArray.h"
#include "itkNumericTraits.h"

namespace itk
{

/** \class CannyEdgesDistanceFeatureGenerator
 * \brief Generates a feature image by computing the distance map to the canny
 * edges in the image.
 *
 * \par Overview
 * The class generates features that can be used as the speed term for
 * computing a canny level set. The class takes an input image
 *
 *    Input -> CastToFloat -> CannyEdgeFilter -> UnsignedDistanceMap
 *
 * The