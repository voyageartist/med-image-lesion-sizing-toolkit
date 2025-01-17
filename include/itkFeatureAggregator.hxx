
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
#ifndef itkFeatureAggregator_hxx
#define itkFeatureAggregator_hxx

#include "itkImageSpatialObject.h"
#include "itkImageRegionIterator.h"


namespace itk
{

/**
 * Constructor
 */
template <unsigned int NDimension>
FeatureAggregator<NDimension>::FeatureAggregator()
{
  this->SetNumberOfRequiredOutputs(1);

  typename OutputImageSpatialObjectType::Pointer outputObject = OutputImageSpatialObjectType::New();

  this->ProcessObject::SetNthOutput(0, outputObject.GetPointer());

  this->m_ProgressAccumulator = ProgressAccumulator::New();
  this->m_ProgressAccumulator->SetMiniPipelineFilter(this);
}


/**
 * Destructor
 */
template <unsigned int NDimension>
FeatureAggregator<NDimension>::~FeatureAggregator() = default;


/**
 * Add a feature generator that will compute the Nth feature to be passed to
 * the segmentation module.
 */
template <unsigned int NDimension>
void
FeatureAggregator<NDimension>::AddFeatureGenerator(FeatureGeneratorType * generator)
{
  this->m_FeatureGenerators.push_back(generator);
}


template <unsigned int NDimension>
unsigned int
FeatureAggregator<NDimension>::GetNumberOfInputFeatures() const
{
  return this->m_FeatureGenerators.size();
}


template <unsigned int NDimension>
const typename FeatureAggregator<NDimension>::InputFeatureType *
FeatureAggregator<NDimension>::GetInputFeature(unsigned int featureId) const
{
  if (featureId >= this->GetNumberOfInputFeatures())
  {
    itkExceptionMacro("Feature Id" << featureId << " doesn't exist");
  }
  return this->m_FeatureGenerators[featureId]->GetFeature();
}


/**
 * PrintSelf
 */
template <unsigned int NDimension>
void
FeatureAggregator<NDimension>::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Feature generators = ";

  auto gitr = this->m_FeatureGenerators.begin();
  auto gend = this->m_FeatureGenerators.end();

  while (gitr != gend)
  {
    os << indent << gitr->GetPointer() << std::endl;
    ++gitr;
  }
}


/*
 * Generate Data
 */
template <unsigned int NDimension>
void
FeatureAggregator<NDimension>::GenerateData()
{
  this->UpdateAllFeatureGenerators();
  this->ConsolidateFeatures();
}

template <unsigned int NDimension>
ModifiedTimeType
FeatureAggregator<NDimension>::GetMTime() const
{
  // MTime is the max of mtime of all feature generators.
  ModifiedTimeType mtime = this->Superclass::GetMTime();
  auto             gitr = this->m_FeatureGenerators.begin();
  auto             gend = this->m_FeatureGenerators.end();
  while (gitr != gend)
  {
    const ModifiedTimeType t = (*gitr)->GetMTime();
    if (t > mtime)
    {
      mtime = t;
    }
    ++gitr;
  }

  return mtime;
}


/**
 * Update feature generators
 */
template <unsigned int NDimension>
void
FeatureAggregator<NDimension>::UpdateAllFeatureGenerators()
{
  auto gitr = this->m_FeatureGenerators.begin();
  auto gend = this->m_FeatureGenerators.end();

  while (gitr != gend)
  {
    // Assuming that most of the time is spent in generating the features and
    // hardly negligible time is spent in consolidating the features
    this->m_ProgressAccumulator->RegisterInternalFilter(*gitr, 1.0 / this->m_FeatureGenerators.size());

    (*gitr)->Update();
    ++gitr;
  }
}

} // end namespace itk

#endif