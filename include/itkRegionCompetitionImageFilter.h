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
#ifndef itkRegionCompetitionImageFilter_h
#define itkRegionCompetitionImageFilter_h

#include "itkImage.h"
#include "itkImageToImageFilter.h"

#include <vector>

namespace itk
{

/** \class RegionCompetitionImageFilter
 *
 * \brief Perform front-propagation from different starting labeled regions.
 *
 * The filter expects two inputs: One gray-scale image and a labeled image.
 * The labels will be used as initial regions from which the fronts will be
 * propagated until they collide with other labeled regions. Each labeled front
 * will compete for pixels against other labels.
 *
 * \ingroup RegionGrowingSegmentation
 * \ingroup LesionSizingToolkit
 */
template <typename TInputImage, typename TOutputImage>
class RegionCompetitionImageFilter : public ImageToImageFilter<TInputImage, TOutputImage>
{
public:
  ITK_DISALLOW_COPY_AND_MOVE(RegionCompetitionImageFilter);

