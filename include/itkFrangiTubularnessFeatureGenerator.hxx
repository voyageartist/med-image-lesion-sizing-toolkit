
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
#ifndef itkFrangiTubularnessFeatureGenerator_hxx
#define itkFrangiTubularnessFeatureGenerator_hxx



namespace itk
{

/**
 * Constructor
 */
template <unsigned int NDimension>
FrangiTubularnessFeatureGenerator<NDimension>::FrangiTubularnessFeatureGenerator()
{
  this->SetNumberOfRequiredInputs(1);

  this->m_HessianFilter = HessianFilterType::New();
  this->m_EigenAnalysisFilter = EigenAnalysisFilterType::New();
  this->m_SheetnessFilter = SheetnessFilterType::New();

  this->m_HessianFilter->ReleaseDataFlagOn();
  this->m_EigenAnalysisFilter->ReleaseDataFlagOn();
  this->m_SheetnessFilter->ReleaseDataFlagOn();

  typename OutputImageSpatialObjectType::Pointer outputObject = OutputImageSpatialObjectType::New();

  this->ProcessObject::SetNthOutput(0, outputObject.GetPointer());

  this->m_Sigma = 1.0;
  this->m_SheetnessNormalization = 0.5;
  this->m_BloobinessNormalization = 2.0;
  this->m_NoiseNormalization = 1.0;
}


/**
 * Destructor
 */
template <unsigned int NDimension>
FrangiTubularnessFeatureGenerator<NDimension>::~FrangiTubularnessFeatureGenerator() = default;

template <unsigned int NDimension>
void
FrangiTubularnessFeatureGenerator<NDimension>::SetInput(const SpatialObjectType * spatialObject)
{
  // Process object is not const-correct so the const casting is required.
  this->SetNthInput(0, const_cast<SpatialObjectType *>(spatialObject));
}

template <unsigned int NDimension>
const typename FrangiTubularnessFeatureGenerator<NDimension>::SpatialObjectType *
FrangiTubularnessFeatureGenerator<NDimension>::GetFeature() const
{
  if (this->GetNumberOfOutputs() < 1)
  {
    return nullptr;
  }

  return static_cast<const SpatialObjectType *>(this->ProcessObject::GetOutput(0));
}


/*
 * PrintSelf
 */
template <unsigned int NDimension>
void
FrangiTubularnessFeatureGenerator<NDimension>::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}


/*
 * Generate Data
 */
template <unsigned int NDimension>
void
FrangiTubularnessFeatureGenerator<NDimension>::GenerateData()
{
  // Report progress.
  ProgressAccumulator::Pointer progress = ProgressAccumulator::New();
  progress->SetMiniPipelineFilter(this);
  progress->RegisterInternalFilter(this->m_HessianFilter, .5);
  progress->RegisterInternalFilter(this->m_EigenAnalysisFilter, .25);
  progress->RegisterInternalFilter(this->m_SheetnessFilter, .25);

  typename InputImageSpatialObjectType::ConstPointer inputObject =
    dynamic_cast<const InputImageSpatialObjectType *>(this->ProcessObject::GetInput(0));

  if (!inputObject)
  {
    itkExceptionMacro("Missing input spatial object");
  }

  const InputImageType * inputImage = inputObject->GetImage();

  if (!inputImage)
  {
    itkExceptionMacro("Missing input image");
  }

  this->m_HessianFilter->SetInput(inputImage);
  this->m_EigenAnalysisFilter->SetInput(this->m_HessianFilter->GetOutput());
  this->m_SheetnessFilter->SetInput(this->m_EigenAnalysisFilter->GetOutput());

  this->m_HessianFilter->SetSigma(this->m_Sigma);
  this->m_EigenAnalysisFilter->SetDimension(Dimension);
  this->m_SheetnessFilter->SetSheetnessNormalization(this->m_SheetnessNormalization);
  this->m_SheetnessFilter->SetBloobinessNormalization(this->m_BloobinessNormalization);
  this->m_SheetnessFilter->SetNoiseNormalization(this->m_NoiseNormalization);

  this->m_SheetnessFilter->Update();

  typename OutputImageType::Pointer outputImage = this->m_SheetnessFilter->GetOutput();

  outputImage->DisconnectPipeline();

  auto * outputObject = dynamic_cast<OutputImageSpatialObjectType *>(this->ProcessObject::GetOutput(0));

  outputObject->SetImage(outputImage);
}

} // end namespace itk

#endif