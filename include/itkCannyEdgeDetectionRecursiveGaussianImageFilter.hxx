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
#ifndef itkCannyEdgeDetectionRecursiveGaussianImageFilter_hxx
#define itkCannyEdgeDetectionRecursiveGaussianImageFilter_hxx

#include "itkZeroCrossingImageFilter.h"
#include "itkNeighborhoodInnerProduct.h"
#include "itkNumericTraits.h"
#include "itkProgressReporter.h"
#include "itkGradientMagnitudeImageFilter.h"
#include <iostream>
namespace itk
{

template <typename TInputImage, typename TOutputImage>
CannyEdgeDetectionRecursiveGaussianImageFilter<TInputImage,
                                               TOutputImage>::CannyEdgeDetectionRecursiveGaussianImageFilter()
{
  unsigned int i;

  m_Sigma.Fill(1.0);

  m_OutsideValue = NumericTraits<OutputImagePixelType>::Zero;
  m_Threshold = NumericTraits<OutputImagePixelType>::Zero;
  m_UpperThreshold = NumericTraits<OutputImagePixelType>::Zero;
  m_LowerThreshold = NumericTraits<OutputImagePixelType>::Zero;

  m_GaussianFilter = GaussianImageFilterType::New();
  m_MultiplyImageFilter = MultiplyImageFilterType::New();
  m_UpdateBuffer1 = OutputImageType::New();

  // Set up neighborhood slices for all the dimensions.
  typename Neighborhood<OutputImagePixelType, ImageDimension>::RadiusType r;
  r.Fill(1);

  // Dummy neighborhood used to set up the slices.
  Neighborhood<OutputImagePixelType, ImageDimension> it;
  it.SetRadius(r);

  // Slice the neighborhood
  m_Center = it.Size() / 2;

  for (i = 0; i < ImageDimension; ++i)
  {
    m_Stride[i] = it.GetStride(i);
  }

  for (i = 0; i < ImageDimension; ++i)
  {
    m_ComputeCannyEdgeSlice[i] = std::slice(m_Center - m_Stride[i], 3, m_Stride[i]);
  }

  // Allocate the derivative operator.
  m_ComputeCannyEdge1stDerivativeOper.SetDirection(0);
  m_ComputeCannyEdge1stDerivativeOper.SetOrder(1);
  m_ComputeCannyEdge1stDerivativeOper.CreateDirectional();

  m_ComputeCannyEdge2ndDerivativeOper.SetDirection(0);
  m_ComputeCannyEdge2ndDerivativeOper.SetOrder(2);
  m_ComputeCannyEdge2ndDerivativeOper.CreateDirectional();

  // Initialize the list
  m_NodeStore = ListNodeStorageType::New();
  m_NodeList = ListType::New();
}

template <typename TInputImage, typename TOutputImage>
void
CannyEdgeDetectionRecursiveGaussianImageFilter<TInputImage, TOutputImage>::AllocateUpdateBuffer()
{
  // The update buffer looks just like the input.

  typename TInputImage::ConstPointer input = this->GetInput();

  m_UpdateBuffer1->CopyInformation(input);
  m_UpdateBuffer1->SetRequestedRegion(input->GetRequestedRegion());
  m_UpdateBuffer1->SetBufferedRegion(input->GetBufferedRegion());
  m_UpdateBuffer1->Allocate();
}

template <typename TInputImage, typename TOutputImage>
void
CannyEdgeDetectionRecursiveGaussianImageFilter<TInputImage, TOutputImage>::GenerateInputRequestedRegion() throw(