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
  InvalidRequestedRegionError)
{
  // call the superclass' implementation of this method
  Superclass::GenerateInputRequestedRegion();
  return;
  // get pointers to the input and output
  typename Superclass::InputImagePointer  inputPtr = const_cast<TInputImage *>(this->GetInput());
  typename Superclass::OutputImagePointer outputPtr = this->GetOutput();

  if (!inputPtr || !outputPtr)
  {
    return;
  }

  // Set the kernel size.
  unsigned long radius = 1;

  // get a copy of the input requested region (should equal the output
  // requested region)
  typename TInputImage::RegionType inputRequestedRegion;
  inputRequestedRegion = inputPtr->GetRequestedRegion();

  // pad the input requested region by the operator radius
  inputRequestedRegion.PadByRadius(radius);

  // crop the input requested region at the input's largest possible region
  if (inputRequestedRegion.Crop(inputPtr->GetLargestPossibleRegion()))
  {
    inputPtr->SetRequestedRegion(inputRequestedRegion);
    return;
  }
  else
  {
    // Couldn't crop the region (requested region is outside the largest
    // possible region).  Throw an exception.

    // store what we tried to request (prior to trying to crop)
    inputPtr->SetRequestedRegion(inputRequestedRegion);

    // build an exception
    InvalidRequestedRegionError e(__FILE__, __LINE__);
    std::ostringstream          msg;
    msg << this->GetNameOfClass() << "::GenerateInputRequestedRegion()";
    e.SetLocation(msg.str().c_str());
    e.SetDescription("Requested region is (at least partially) outside the largest possible region.");
    e.SetDataObject(inputPtr);
    throw e;
  }
}

template <typename TInputImage, typename TOutputImage>
void
CannyEdgeDetectionRecursiveGaussianImageFilter<TInputImage, TOutputImage>::ThreadedCompute2ndDerivative(
  const OutputImageRegionType & outputRegionForThread,
  int                           threadId)
{
  ZeroFluxNeumannBoundaryCondition<TInputImage> nbc;

  ImageRegionIterator<TOutputImage> it;

  void * globalData = nullptr;

  // Here input is the result from the gaussian filter
  //      output is the update buffer.
  typename OutputImageType::Pointer input = m_GaussianFilter->GetOutput();
  typename OutputImageType::Pointer output = this->GetOutput();

  // set iterator radius
  Size<ImageDimension> radius;
  radius.Fill(1);

  // Find the data-set boundary "faces"
  typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<TInputImage>::FaceListType faceList;
  NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<TInputImage>                        bC;
  faceList = bC(input, outputRegionForThread, radius);

  typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<TInputImage>::FaceListType::iterator fit;

  // support progress methods/callbacks
  ProgressReporter progress(this, threadId, outputRegionForThread.GetNumberOfPixels(), 100, 0.0f, 0.5f);

  // Process the non-boundady region and then each of the boundary faces.
  // These are N-d regions which border the edge of the buffer.
  for (fit = faceList.begin(); fit != faceList.end(); ++fit)
  {
    NeighborhoodType bit(radius, input, *fit);

    it = ImageRegionIterator<OutputImageType>(output, *fit);
    bit.OverrideBoundaryCondition(&nbc);
    bit.GoToBegin();

    while (!bit.IsAtEnd())
    {
      it.Value() = ComputeCannyEdge(bit, globalData);
      ++bit;
      ++it;
      progress.CompletedPixel();
    }
  }
}

template <typename TInputImage, typename TOutputImage>
typename CannyEdgeDetectionRecursiveGaussianImageFilter<TInputImage, TOutputImage>::OutputImagePixelType
CannyEdgeDetectionRecursiveGaussianImageFilter<TInputImage, TOutputImage>::ComputeCannyEdge(
  const NeighborhoodType & it,
  void *                   itkNotUsed(globalData))
{
  unsigned int                              i, j;
  NeighborhoodInnerProduct<OutputImageType> innerProduct;

  OutputImagePixelType dx[ImageDimension];
  OutputImagePixelType dxx[ImageDimension];
  OutputImagePixelType dxy[ImageDimension * (ImageDimension - 1) / 2];
  OutputImagePixelType deriv;
  OutputImagePixelType gradMag;

  //  double alpha = 0.01;

  // Calculate 1st & 2nd order derivative
  for (i = 0; i < ImageDimension; i++)
  {
    dx[i] = innerProduct(m_ComputeCannyEdgeSlice[i], it, m_ComputeCannyEdge1stDerivativeOper);
    dxx[i] = innerProduct(m_ComputeCannyEdgeSlice[i], it, m_ComputeCannyEdge2ndDerivativeOper);
  }

  deriv = NumericTraits<OutputImagePixelType>::Zero;
  int k = 0;

  // Calculate the 2nd derivative
  for (i = 0; i < ImageDimension - 1; i++)
  {
    for (j = i + 1; j < ImageDimension; j++)
    {
      dxy[k] = 0.25 * it.GetPixel(m_Center - m_Stride[i] - m_Stride[j]) -
               0.25 * it.GetPixel(m_Center - m_Stride[i] + m_Stride[j]) -
               0.25 * it.GetPixel(m_Center + m_Stride[i] - m_Stride[j]) +
               0.25 * it.