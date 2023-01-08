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
#ifndef itkLesionSegmentationImageFilter8_hxx
#define itkLesionSegmentationImageFilter8_hxx

#include "itkNumericTraits.h"
#include "itkProgressReporter.h"
#include "itkGradientMagnitudeImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

namespace itk
{

template <typename TInputImage, typename TOutputImage>
LesionSegmentationImageFilter8<TInputImage, TOutputImage>::LesionSegmentationImageFilter8()
{
  m_CannyEdgesFeatureGenerator = CannyEdgesFeatureGeneratorType::New();
  m_LesionSegmentationMethod = LesionSegmentationMethodType::New();
  m_LungWallFeatureGenerator = LungWallGeneratorType::New();
  m_VesselnessFeatureGenerator = VesselnessGeneratorType::New();
  m_SigmoidFeatureGenerator = SigmoidFeatureGeneratorType::New();
  m_FeatureAggregator = FeatureAggregatorType::New();
  m_SegmentationModule = SegmentationModuleType::New();
  m_CropFilter = CropFilterType::New();
  m_IsotropicResampler = IsotropicResamplerType::New();
  m_InputSpatialObject = InputImageSpatialObjectType::New();

  // Report progress.
  m_CommandObserver = CommandType::New();
  m_CommandObserver->SetCallbackFunction(this, &Self::ProgressUpdate);
  m_LungWallFeatureGenerator->AddObserver(itk::ProgressEvent(), m_CommandObserver);
  m_SigmoidFeatureGenerator->AddObserver(itk::ProgressEvent(), m_CommandObserver);
  m_VesselnessFeatureGenerator->AddObserver(itk::ProgressEvent(), m_CommandObserver);
  m_CannyEdgesFeatureGenerator->AddObserver(itk::ProgressEvent(), m_CommandObserver);
  m_SegmentationModule->AddObserver(itk::ProgressEvent(), m_CommandObserver);
  m_CropFilter->AddObserver(itk::ProgressEvent(), m_CommandObserver);
  m_IsotropicResampler->AddObserver(itk::ProgressEvent(), m_CommandObserver);

  // Connect pipeline
  m_LungWallFeatureGenerator->SetInput(m_InputSpatialObject);
  m_SigmoidFeatureGenerator->SetInput(m_InputSpatialObject);
  m_VesselnessFeatureGenerator->SetInput(m_InputSpatialObject);
  m_CannyEdgesFeatureGenerator->SetInput(m_InputSpatialObject);
  m_FeatureAggregator->AddFeatureGenerator(m_LungWallFeatureGenerator);
  m_FeatureAggregator->AddFeatureGenerator(m_VesselnessFeatureGenerator);
  m_FeatureAggregator->AddFeatureGenerator(m_SigmoidFeatureGenerator);
  m_FeatureAggregator->AddFeatureGenerator(m_CannyEdgesFeatureGenerator);
  m_LesionSegmentationMethod->AddFeatureGenerator(m_FeatureAggregator);
  m_LesionSegmentationMethod->SetSegmentationModule(m_SegmentationModule);

  // Populate some parameters
  m_LungWallFeatureGenerator->SetLungThreshold(-400);
  m_VesselnessFeatureGenerator->SetSigma(1.0);
  m_VesselnessFeatureGenerator->SetAlpha1(0.1);
  m_VesselnessFeatureGenerator->SetAlpha2(2.0);
  m_VesselnessFeatureGenerator->SetSigmoidAlpha(-10.0);
  m_VesselnessFeatureGenerator->SetSigmoidBeta(40.0);
  m_SigmoidFeatureGenerator->SetAlpha(100.0);
  m_SigmoidFeatureGenerator->SetBeta(-500.0);
  m_CannyEdgesFeatureGenerator->SetSigma(1.0);
  m_CannyEdgesFeatureGenerator->SetUpperThreshold(150.0);
  m_CannyEdgesFeatureGenerator->SetLowerThreshold(75.0);
  m_FastMarchingStoppingTime = 5.0;
  m_FastMarchingDistanceFromSeeds = 0.5;
  m_SigmoidBeta = -500.0;
  m_StatusMessage = "";
  m_SegmentationModule->SetCurvatureScaling(1.0);
  m_SegmentationModule->SetAdvectionScaling(0.0);
  m_SegmentationModule->SetPropagationScaling(500.0);
  m_SegmentationModule->SetMaximumRMSError(0.0002);
  m_SegmentationModule->SetMaximumNumberOfIterations(300);
  m_ResampleThickSliceData = true;
  m_AnisotropyThreshold = 1.0;
  m_UserSpecifiedSigmas = false;
}

template <typename TInputImage, typename TOutputImage>
void
LesionSegmentationImageFilter8<TInputImage, TOutputImage>::GenerateInputRequestedRegion() throw(
  InvalidRequestedRegionError)
{
  // call the superclass' implementation of this method
  Superclass::GenerateInputRequestedRegion();

  if (!this->GetInput())
  {
    typename InputImageType::Pointer inputPtr = const_cast<TInputImage *>(this->GetInput());

    // Request the entire input image
    inputPtr->SetRequestedRegion(inputPtr->GetLargestPossibleRegion());
  }
}

template <typename TInputImage, typename TOutputImage>
void
LesionSegmentationImageFilter8<TInputImage, TOutputImage>::SetSigma(SigmaArrayType s)
{
  this->m_UserSpecifiedSigmas = true;
  m_CannyEdgesFeatureGenerator->SetSigmaArray(s);
}

template <typename TInputImage, typename TOutputImage>
void
LesionSegmentationImageFilter8<TInputImage, TOutputImage>::GenerateOutputInformation()
{
  // get pointers to the input and output
  typename Superclass::OutputImagePointer     outputPtr = this->GetOutput();
  typename Superclass::InputImageConstPointer inputPtr = this->GetInput();
  if (!outputPtr || !inputPtr)
  {
    return;
  }

  // Minipipeline is :
  //   Input -> Crop -> Resample_if_too_anisotropic -> Segment

  m_CropFilter->SetInput(inputPtr);
  m_CropFilter->SetRegionOfInterest(m_RegionOfInterest);

  // Compute the spacing after isotropic resampling.
  double minSpacing = NumericTraits<double>::max();
  for (unsigned int i = 0; i < ImageDimension; i++)
  {
    minSpacing = (minSpacing > inputPtr->GetSpacing()[i] ? inputPtr->GetSpacing()[i] : minSpacing);
  }

  // Try and reduce the anisotropy.
  SpacingType outputSpacing = inputPtr->GetSpacing();
  for (unsigned int i = 0; i < ImageDimension; i++)
  {
    if (outputSpacing[i] / minSpacing > m_AnisotropyThreshold && m_ResampleThickSliceData)
    {
      outputSpacing[i] = minSpacing * m_AnisotropyThreshold;
    }
  }

  if (m_ResampleThickSliceData)
  {
    m_IsotropicResampler->SetInput(m_CropFilter->GetOutput());
    m_IsotropicResampler->SetOutputSpacing(outputSpacing);
    m_IsotropicResampler->GenerateOutputInformation();
    outputPtr->CopyInformation(m_IsotropicResampler->GetOutput());
  }
  else
  {
    outputPtr->Co