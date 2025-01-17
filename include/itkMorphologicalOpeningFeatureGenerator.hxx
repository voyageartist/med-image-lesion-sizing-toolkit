
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
#ifndef itkMorphologicalOpeningFeatureGenerator_hxx
#define itkMorphologicalOpeningFeatureGenerator_hxx

#include "itkProgressAccumulator.h"


namespace itk
{

/**
 * Constructor
 */
template <unsigned int NDimension>
MorphologicalOpeningFeatureGenerator<NDimension>::MorphologicalOpeningFeatureGenerator()
{
  this->SetNumberOfRequiredInputs(1);
  this->SetNumberOfRequiredOutputs(1);

  this->m_ThresholdFilter = ThresholdFilterType::New();
  this->m_OpenningFilter = OpenningFilterType::New();
  this->m_VotingHoleFillingFilter = VotingHoleFillingFilterType::New();
  this->m_CastingFilter = CastingFilterType::New();

  this->m_ThresholdFilter->ReleaseDataFlagOn();
  this->m_OpenningFilter->ReleaseDataFlagOn();
  this->m_VotingHoleFillingFilter->ReleaseDataFlagOn();
  this->m_CastingFilter->ReleaseDataFlagOn();

  typename OutputImageSpatialObjectType::Pointer outputObject = OutputImageSpatialObjectType::New();

  this->ProcessObject::SetNthOutput(0, outputObject.GetPointer());

  this->m_LungThreshold = -400;
}


/*
 * Destructor
 */
template <unsigned int NDimension>
MorphologicalOpeningFeatureGenerator<NDimension>::~MorphologicalOpeningFeatureGenerator() = default;

template <unsigned int NDimension>
void
MorphologicalOpeningFeatureGenerator<NDimension>::SetInput(const SpatialObjectType * spatialObject)
{
  // Process object is not const-correct so the const casting is required.
  this->SetNthInput(0, const_cast<SpatialObjectType *>(spatialObject));
}

template <unsigned int NDimension>
const typename MorphologicalOpeningFeatureGenerator<NDimension>::SpatialObjectType *
MorphologicalOpeningFeatureGenerator<NDimension>::GetFeature() const
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
MorphologicalOpeningFeatureGenerator<NDimension>::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Lung threshold " << this->m_ThresholdFilter << std::endl;
}


/*
 * Generate Data
 */
template <unsigned int NDimension>
void
MorphologicalOpeningFeatureGenerator<NDimension>::GenerateData()
{
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

  // Report progress.
  ProgressAccumulator::Pointer progress = ProgressAccumulator::New();
  progress->SetMiniPipelineFilter(this);
  progress->RegisterInternalFilter(this->m_ThresholdFilter, 0.1);
  progress->RegisterInternalFilter(this->m_OpenningFilter, 0.2);
  progress->RegisterInternalFilter(this->m_VotingHoleFillingFilter, 0.6);
  progress->RegisterInternalFilter(this->m_CastingFilter, 0.1);

  this->m_ThresholdFilter->SetInput(inputImage);
  this->m_OpenningFilter->SetInput(this->m_ThresholdFilter->GetOutput());
  this->m_VotingHoleFillingFilter->SetInput(this->m_OpenningFilter->GetOutput());
  this->m_CastingFilter->SetInput(this->m_VotingHoleFillingFilter->GetOutput());

  this->m_ThresholdFilter->SetLowerThreshold(this->m_LungThreshold);
  this->m_ThresholdFilter->SetUpperThreshold(3000);

  this->m_ThresholdFilter->SetInsideValue(1.0);
  this->m_ThresholdFilter->SetOutsideValue(0.0);


  typename InternalImageType::SizeType ballManhattanRadius;

  ballManhattanRadius.Fill(1);

  KernelType                    ball;
  typename KernelType::SizeType ballSize;
  ballSize.Fill(1);
  ball.SetRadius(ballSize);
  ball.CreateStructuringElement();

  this->m_OpenningFilter->SetKernel(ball);
  this->m_OpenningFilter->SetBackgroundValue(0);
  this->m_OpenningFilter->SetForegroundValue(1);

  this->m_VotingHoleFillingFilter->SetRadius(ballManhattanRadius);
  this->m_VotingHoleFillingFilter->SetBackgroundValue(0);
  this->m_VotingHoleFillingFilter->SetForegroundValue(1);
  this->m_VotingHoleFillingFilter->SetMajorityThreshold(1);
  this->m_VotingHoleFillingFilter->SetMaximumNumberOfIterations(1000);

  this->m_CastingFilter->Update();

  std::cout << "Used " << this->m_VotingHoleFillingFilter->GetCurrentIterationNumber() << " iterations " << std::endl;
  std::cout << "Changed " << this->m_VotingHoleFillingFilter->GetTotalNumberOfPixelsChanged() << " pixels "
            << std::endl;

  typename OutputImageType::Pointer outputImage = this->m_CastingFilter->GetOutput();

  outputImage->DisconnectPipeline();

  auto * outputObject = dynamic_cast<OutputImageSpatialObjectType *>(this->ProcessObject::GetOutput(0));

  outputObject->SetImage(outputImage);
}

} // end namespace itk

#endif