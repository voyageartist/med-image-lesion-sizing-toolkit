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
#ifndef itkConnectedThresholdSegmentationModule_hxx
#define itkConnectedThresholdSegmentationModule_hxx

#include "itkProgressAccumulator.h"


namespace itk
{


/**
 * Constructor
 */
template <unsigned int NDimension>
ConnectedThresholdSegmentationModule<NDimension>::ConnectedThresholdSegmentationModule() = default;


/**
 * Destructor
 */
template <unsigned int NDimension>
ConnectedThresholdSegmentationModule<NDimension>::~ConnectedThresholdSegmentationModule() = default;


/**
 * PrintSelf
 */
template <unsigned int NDimension>
void
ConnectedThresholdSegmentationModule<NDimension>::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}


/**
 * Generate Data
 */
template <unsigned int NDimension>
void
ConnectedThresholdSegmentationModule<NDimension>::GenerateData()
{
  using FilterType = ConnectedThresholdImageFilter<FeatureImageType, OutputImageType>;

  typename FilterType::Pointer filter = FilterType::New();

  const FeatureImageType * featureImage = this->GetInternalFeatureImage();

  filter->SetInput(featureImage);

  const InputSpatialObjectType * inputSeeds = this->GetInternalInputLandmarks();

  const unsigned int numberOfPoints = inputSeeds->GetNumberOfPoints();

  using LandmarkPointListType = typename InputSpatialO