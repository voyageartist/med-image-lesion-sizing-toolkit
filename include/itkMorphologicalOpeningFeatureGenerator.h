
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
#ifndef itkMorphologicalOpeningFeatureGenerator_h
#define itkMorphologicalOpeningFeatureGenerator_h

#include "itkFeatureGenerator.h"
#include "itkImage.h"
#include "itkImageSpatialObject.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkVotingBinaryHoleFillFloodingImageFilter.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkBinaryMorphologicalOpeningImageFilter.h"
#include "itkCastImageFilter.h"

namespace itk
{

/** \class MorphologicalOpeningFeatureGenerator
 * \brief Generates a feature image based on intensity and removes small pieces from it.
 *
 * This feature generator thresholds the input image, runs an Openning
 * Mathematical Morphology Filter and then a Voting Hole Filling filter.
 * The net effect is the elimination of small islands and small holes
 * from the thresholded image.
 *
 * SpatialObjects are used as inputs and outputs of this class.
 *
 * \ingroup SpatialObjectFilters
 * \ingroup LesionSizingToolkit
 */
template <unsigned int NDimension>
class ITK_TEMPLATE_EXPORT MorphologicalOpeningFeatureGenerator : public FeatureGenerator<NDimension>
{
public:
  ITK_DISALLOW_COPY_AND_MOVE(MorphologicalOpeningFeatureGenerator);
