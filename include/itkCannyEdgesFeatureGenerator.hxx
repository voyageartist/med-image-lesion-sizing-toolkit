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
#ifndef itkCannyEdgesFeatureGenerator_hxx
#define itkCannyEdgesFeatureGenerator_hxx



namespace itk
{

/**
 * Constructor
 */
template <unsigned int NDimension>
CannyEdgesFeatureGenerator<NDimension>::CannyEdgesFeatureGenerator()
{
  this->SetNumberOfRequiredInputs(1);

  this->m_CastFilter = CastFilterType::New();
  this->m_RescaleFilter = RescaleFilterType::New();
  this->m_CannyFilter = CannyEdgeFilterType::New();

  typename OutputImageSpatialObjectType::Pointer outputObject = OutputImageSpatialObjectType::New();

  this->ProcessObject::SetNthOutput(0, outputObject.GetPointer());

  this->m_Sigma.Fill(1.0);
  this->m_UpperThreshold = NumericTraits<InternalPixelType>::max();
  this->m_LowerThreshold = NumericTraits<InternalPixelType>::min();

  this->m_RescaleFilter->SetOutputMinimum(1.0);
  this->m_RescaleFilter->SetOutputMaximum(0.0);

