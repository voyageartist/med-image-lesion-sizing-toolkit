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
#ifndef itkSegmentationVolumeEstimator_h
#define itkSegmentationVolumeEstimator_h

#include "itkImage.h"
#include "itkImageSpatialObject.h"
#include "itkSimpleDataObjectDecorator.h"

namespace itk
{

/** \class SegmentationVolumeEstimator
 * \brief Class for estimating the volume of a segmentation stored in a SpatialObject
 *
 * This class is the base class for specific implementation of volume estimators.
 *
 * SpatialObjects are used as input of this class.
 *
 * \ingroup SpatialObjectFilters
 * \ingroup LesionSizingToolkit
 */
template <unsigned int NDimension>
class ITK_TEMPLATE_EXPORT SegmentationVolumeEstimator : public ProcessObject
{
public:
  ITK_DISALLOW_COPY_AND_MOVE(SegmentationVolumeEstimator);

  /** Standard class type alias. */
  using Self = SegmentationVolumeEstimator;
  using Superclass = ProcessObject;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  /** This is an abstract class, therefore it doesn't need the itkNewMacro() */

  /** Run-time type information (and related methods). */
  itkTypeMacro(SegmentationVolumeEstimator, FeatureGenerator);

  /** Dimension of 