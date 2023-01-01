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
  m_SigmoidFeatureGenerator->SetInput(m_InputSpatialObje