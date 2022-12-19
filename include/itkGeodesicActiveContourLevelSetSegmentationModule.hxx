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
#ifndef itkGeodesicActiveContourLevelSetSegmentationModule_hxx
#define itkGeodesicActiveContourLevelSetSegmentationModule_hxx

#include "itkGeodesicActiveContourLevelSetImageFilter.h"
#include "itkProgressAccumulator.h"


namespace itk
{


/**
 * Constructor
 */
template <unsigned int NDimension>
GeodesicActiveContourLevelSetSegmentationModule<NDimension>::GeodesicActiveContourLevelSetSegmentationModule() =
  default;


/**
 * Destructor
 */
template <unsigned int NDimension>
GeodesicActiveContourLevelSetSegmentationModule<NDimension>::~GeodesicActiveContourLevelSetSegmentationModule() =
  default;


/**
 * PrintSelf
 */
template <unsigned int NDimension>
void
GeodesicActiveContourLevelSetSegmentationModule<NDimension>::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}


/**
 * Generate Data
 */
template <unsigned int NDimension>
void
GeodesicActiveContourLevelSetSegmentationModule<NDimension>::GenerateData()
{
  using FilterType = GeodesicActiveContourLevelSetImageFilter<InputImageType, FeatureImageType, OutputPixelType>;

  typename FilterType::Pointer filter = FilterType::New();

  filter->SetInput(this->GetInternalInputImage());
  filter->SetFeatureImage(this->GetInternalFeatureImage());

  filter->SetMaximumRMSError(this->GetMaximumRMSError());
  filter->SetNumberOfIterations(this->GetMaximumNumberOfIterations());
  filter->SetPropagationScaling(this->GetPropagationScaling());
  filter->SetCurvatureScaling(this->GetCurvatureScaling());
  filter->SetAdvectionScaling(this->GetAdvectionScaling());
  filter->UseImageSpacingOn();

  // Prog