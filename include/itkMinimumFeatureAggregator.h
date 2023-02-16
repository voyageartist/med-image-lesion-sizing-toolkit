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
#ifndef itkMinimumFeatureAggregator_h
#define itkMinimumFeatureAggregator_h

#include "itkFeatureAggregator.h"

namespace itk
{

/** \class MinimumFeatureAggregator
 * \brief Class for combining multiple features into a single one by computing
 * the pixel-wise minimum.
 *
 * This class generates a new feature object containing an image that is
 * computed as the pixel-wise minimum of all the input feature images.
 *
 * \warning This class assumes that all the images have the same: origin,
 * spacing, orientation, and that they are represented in the same image grid.
 * mixing strategies.
 *
 * SpatialObjects are used as inputs and outputs of th