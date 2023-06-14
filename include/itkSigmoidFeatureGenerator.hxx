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
#ifndef itkSigmoidFeatureGenerator_hxx
#define itkSigmoidFeatureGenerator_hxx

#include "itkProgressAccumulator.h"

namespace itk
{

/**
 * Constructor
 */
template <unsigned int NDimension>
SigmoidFeatureGenerator<NDimension>::SigmoidFeatureGenerator()
{
  this->SetNumberOfRequiredInputs(1);
  this->SetNumberOfRequiredOutputs(1);

  this->m_SigmoidFilter = SigmoidFilterType::New();

  this->m_SigmoidFilter->ReleaseDataFlagOn();

  typename OutputImageSpatialObjectType::Pointer outputObject = OutputImageSpatialObjectType::New();

  this->ProcessObject::SetNthOutput(0, outputObject.GetPointer());

  this->m_Alpha = -1.0;
  this->m_Beta = 128.0;
}


/*
 * Destructor
 */
template <unsigned int NDimension>
SigmoidFeatureGenerator<NDimension>::~SigmoidFeatureGenerator() = default;

template <unsigned int NDimension>
void
SigmoidFeat