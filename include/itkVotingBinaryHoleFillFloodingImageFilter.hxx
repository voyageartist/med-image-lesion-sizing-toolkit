
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
#ifndef itkVotingBinaryHoleFillFloodingImageFilter_hxx
#define itkVotingBinaryHoleFillFloodingImageFilter_hxx

#include "itkConstNeighborhoodIterator.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionExclusionIteratorWithIndex.h"
#include "itkNeighborhoodAlgorithm.h"
#include "itkOffset.h"
#include "itkProgressReporter.h"

namespace itk
{

/**
 * Constructor
 */
template <typename TInputImage, typename TOutputImage>
VotingBinaryHoleFillFloodingImageFilter<TInputImage, TOutputImage>::VotingBinaryHoleFillFloodingImageFilter()
{
  this->m_MaximumNumberOfIterations = 10;
  this->m_CurrentIterationNumber = 0;

  this->m_NumberOfPixelsChangedInLastIteration = 0;
  this->m_TotalNumberOfPixelsChanged = 0;

  this->m_SeedArray1 = new SeedArrayType;
  this->m_SeedArray2 = new SeedArrayType;

  this->m_OutputImage = nullptr;

  this->m_MajorityThreshold = 1;
}

/**
 * Destructor
 */
template <typename TInputImage, typename TOutputImage>
VotingBinaryHoleFillFloodingImageFilter<TInputImage, TOutputImage>::~VotingBinaryHoleFillFloodingImageFilter()
{
  delete this->m_SeedArray1;
  delete this->m_SeedArray2;
}


/**
 * Standard PrintSelf method.
 */
template <typename TInputImage, typename TOutputImage>
void
VotingBinaryHoleFillFloodingImageFilter<TInputImage, TOutputImage>::PrintSelf(std::ostream & os, Indent indent) const
{
  this->Superclass::PrintSelf(os, indent);
}


template <typename TInputImage, typename TOutputImage>
void
VotingBinaryHoleFillFloodingImageFilter<TInputImage, TOutputImage>::GenerateData()
{
  this->AllocateOutputImageWorkingMemory();
  this->InitializeNeighborhood();
  this->ComputeBirthThreshold();
  this->ComputeArrayOfNeighborhoodBufferOffsets();
  this->FindAllPixelsInTheBoundaryAndAddThemAsSeeds();
  this->IterateFrontPropagations();
}


template <typename TInputImage, typename TOutputImage>
void
VotingBinaryHoleFillFloodingImageFilter<TInputImage, TOutputImage>::IterateFrontPropagations()
{
  this->m_CurrentIterationNumber = 0;
  this->m_TotalNumberOfPixelsChanged = 0;
  this->m_NumberOfPixelsChangedInLastIteration = 0;

  // Progress reporting
  ProgressReporter progress(this, 0, m_MaximumNumberOfIterations, 100000);

  while (this->m_CurrentIterationNumber < this->m_MaximumNumberOfIterations)
  {
    this->VisitAllSeedsAndTransitionTheirState();
    this->m_CurrentIterationNumber++;

    progress.CompletedPixel(); // not really a pixel but an iteration
    this->InvokeEvent(IterationEvent());

    if (this->m_NumberOfPixelsChangedInLastIteration == 0)
    {
      break;
    }
  }
}


template <typename TInputImage, typename TOutputImage>
void
VotingBinaryHoleFillFloodingImageFilter<TInputImage, TOutputImage>::AllocateOutputImageWorkingMemory()
{
  this->m_OutputImage = this->GetOutput();
  OutputImageRegionType region = this->m_OutputImage->GetRequestedRegion();

  // Allocate memory for the output image itself.
  this->m_OutputImage->SetBufferedRegion(region);
  this->m_OutputImage->Allocate();
  this->m_OutputImage->FillBuffer(0);

  this->m_SeedsMask = SeedMaskImageType::New();
  this->m_SeedsMask->SetRegions(region);
  this->m_SeedsMask->Allocate();
  this->m_SeedsMask->FillBuffer(0);
}


template <typename TInputImage, typename TOutputImage>
void
VotingBinaryHoleFillFloodingImageFilter<TInputImage, TOutputImage>::InitializeNeighborhood()
{
  this->m_Neighborhood.SetRadius(this->GetRadius());
}


template <typename TInputImage, typename TOutputImage>
void
VotingBinaryHoleFillFloodingImageFilter<TInputImage, TOutputImage>::FindAllPixelsInTheBoundaryAndAddThemAsSeeds()
{
  const InputImageType * inputImage = this->GetInput();

  OutputImageRegionType region = inputImage->GetRequestedRegion();

  ConstNeighborhoodIterator<InputImageType> bit;
  ImageRegionIterator<OutputImageType>      itr;
  ImageRegionIterator<SeedMaskImageType>    mtr;

  const InputSizeType & radius = this->GetRadius();

  // Find the data-set boundary "faces"
  typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType>::FaceListType faceList;
  NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType>                        bC;
  faceList = bC(inputImage, region, radius);

  typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType>::FaceListType::iterator fit;

  // Process only the internal face
  fit = faceList.begin();

  this->m_InternalRegion = *fit;

  // Mark all the pixels in the boundary of the seed image as visited
  using ExclusionIteratorType = itk::ImageRegionExclusionIteratorWithIndex<SeedMaskImageType>;
  ExclusionIteratorType exIt(this->m_SeedsMask, region);
  exIt.SetExclusionRegion(this->m_InternalRegion);
  for (exIt.GoToBegin(); !exIt.IsAtEnd(); ++exIt)
  {
    exIt.Set(255);
  }

  bit = ConstNeighborhoodIterator<InputImageType>(radius, inputImage, this->m_InternalRegion);
  itr = ImageRegionIterator<OutputImageType>(this->m_OutputImage, this->m_InternalRegion);
  mtr = ImageRegionIterator<SeedMaskImageType>(this->m_SeedsMask, this->m_InternalRegion);

  bit.GoToBegin();
  itr.GoToBegin();
  mtr.GoToBegin();

  unsigned int neighborhoodSize = bit.Size();

  const InputImagePixelType foregroundValue = this->GetForegroundValue();
  const InputImagePixelType backgroundValue = this->GetBackgroundValue();

  this->m_SeedArray1->clear();
  this->m_SeedsNewValues.clear();

  while (!bit.IsAtEnd())
  {
    if (bit.GetCenterPixel() == foregroundValue)
    {
      itr.Set(foregroundValue);
      mtr.Set(255);
    }
    else
    {
      itr.Set(backgroundValue);
      mtr.Set(0);

      // Search for foreground pixels in the neighborhood
      for (unsigned int i = 0; i < neighborhoodSize; ++i)
      {
        InputImagePixelType value = bit.GetPixel(i);
        if (value == foregroundValue)
        {
          this->m_SeedArray1->push_back(bit.GetIndex());
          break;
        }
      }
    }
    ++bit;
    ++itr;
    ++mtr;
  }
  this->m_SeedsNewValues.reserve(this->m_SeedArray1->size());
}


template <typename TInputImage, typename TOutputImage>
void
VotingBinaryHoleFillFloodingImageFilter<TInputImage, TOutputImage>::VisitAllSeedsAndTransitionTheirState()
{
  using SeedIterator = typename SeedArrayType::const_iterator;

  SeedIterator seedItr = this->m_SeedArray1->begin();

  this->m_NumberOfPixelsChangedInLastIteration = 0;

  // Clear the array of new values
  this->m_SeedsNewValues.clear();

  while (seedItr != this->m_SeedArray1->end())
  {
    this->m_CurrentPixelIndex = *seedItr;

    if (this->TestForQuorumAtCurrentPixel())
    {
      this->m_SeedsNewValues.push_back(this->GetForegroundValue());
      this->PutCurrentPixelNeighborsIntoSeedArray();
      this->m_NumberOfPixelsChangedInLastIteration++;
    }
    else
    {
      this->m_SeedsNewValues.push_back(this->GetBackgroundValue());
      // Keep the seed to try again in the next iteration.
      this->m_SeedArray2->push_back(this->GetCurrentPixelIndex());
    }

    ++seedItr;
  }

  this->PasteNewSeedValuesToOutputImage();

  this->m_TotalNumberOfPixelsChanged += this->m_NumberOfPixelsChangedInLastIteration;

  // Now that the values have been copied to the output image, we can empty the
  // array in preparation for the next iteration
  this->m_SeedsNewValues.clear();

  this->SwapSeedArrays();
  this->ClearSecondSeedArray();
}


template <typename TInputImage, typename TOutputImage>
void
VotingBinaryHoleFillFloodingImageFilter<TInputImage, TOutputImage>::PasteNewSeedValuesToOutputImage()
{
  //
  //  Paste new values into the output image
  //
  using SeedIterator = typename SeedArrayType::const_iterator;

  SeedIterator seedItr = this->m_SeedArray1->begin();

  using SeedsNewValuesIterator = typename SeedNewValuesArrayType::const_iterator;

  SeedsNewValuesIterator newValueItr = this->m_SeedsNewValues.begin();

  while (seedItr != this->m_SeedArray1->end())
  {
    this->m_OutputImage->SetPixel(*seedItr, *newValueItr);
    ++seedItr;
    ++newValueItr;
  }
}

template <typename TInputImage, typename TOutputImage>
void
VotingBinaryHoleFillFloodingImageFilter<TInputImage, TOutputImage>::SwapSeedArrays()
{
  SeedArrayType * temporary = this->m_SeedArray1;
  this->m_SeedArray1 = this->m_SeedArray2;
  this->m_SeedArray2 = temporary;
}


template <typename TInputImage, typename TOutputImage>
void
VotingBinaryHoleFillFloodingImageFilter<TInputImage, TOutputImage>::ClearSecondSeedArray()
{
  delete this->m_SeedArray2;
  this->m_SeedArray2 = new SeedArrayType;
}


template <typename TInputImage, typename TOutputImage>
bool
VotingBinaryHoleFillFloodingImageFilter<TInputImage, TOutputImage>::TestForQuorumAtCurrentPixel() const
{
  //
  // Find the location of the current pixel in the image memory buffer
  //
  const OffsetValueType offset = this->m_OutputImage->ComputeOffset(this->GetCurrentPixelIndex());

  const InputImagePixelType * buffer = this->m_OutputImage->GetBufferPointer();

  const InputImagePixelType * currentPixelPointer = buffer + offset;

  unsigned int numberOfNeighborsAtForegroundValue = 0;

  //
  // From that buffer position, visit all other neighbor pixels
  // and check if they are set to the foreground value.
  //
  auto neighborItr = this->m_NeighborBufferOffset.begin();

  const InputImagePixelType foregroundValue = this->GetForegroundValue();

  while (neighborItr != this->m_NeighborBufferOffset.end())
  {
    const InputImagePixelType * neighborPixelPointer = currentPixelPointer + *neighborItr;

    if (*neighborPixelPointer == foregroundValue)
    {
      numberOfNeighborsAtForegroundValue++;
    }

    ++neighborItr;
  }

  bool quorum = (numberOfNeighborsAtForegroundValue > this->GetBirthThreshold());

  return quorum;
}


template <typename TInputImage, typename TOutputImage>
void
VotingBinaryHoleFillFloodingImageFilter<TInputImage, TOutputImage>::PutCurrentPixelNeighborsIntoSeedArray()
{
  //
  // Find the location of the current pixel in the image memory buffer
  //
  const OffsetValueType offset = this->m_OutputImage->ComputeOffset(this->GetCurrentPixelIndex());

  const InputImagePixelType * buffer = this->m_OutputImage->GetBufferPointer();

  const InputImagePixelType * currentPixelPointer = buffer + offset;


  const unsigned int neighborhoodSize = this->m_Neighborhood.Size();

  //
  // Visit the offset of each neighbor in Index as well as buffer space
  // and if they are backgroundValue then insert them as new seeds
  //
  using NeighborOffsetType = typename NeighborhoodType::OffsetType;

  using NeigborOffsetIterator = typename NeighborOffsetArrayType::const_iterator;

  NeigborOffsetIterator neighborItr = this->m_NeighborBufferOffset.begin();

  const InputImagePixelType backgroundValue = this->GetBackgroundValue();

  for (unsigned int i = 0; i < neighborhoodSize; ++i, ++neighborItr)
  {
    const InputImagePixelType * neighborPixelPointer = currentPixelPointer + *neighborItr;

    if (*neighborPixelPointer == backgroundValue)
    {
      NeighborOffsetType neighborOffset = this->m_Neighborhood.GetOffset(i);
      IndexType          neighborIndex = this->GetCurrentPixelIndex() + neighborOffset;

      // if( this->m_InternalRegion.IsInside( neighborIndex ) )
      {
        if (this->m_SeedsMask->GetPixel(neighborIndex) == 0)
        {
          this->m_SeedArray2->push_back(neighborIndex);
          this->m_SeedsMask->SetPixel(neighborIndex, 255);
        }
      }
    }
  }
}


template <typename TInputImage, typename TOutputImage>
unsigned int
VotingBinaryHoleFillFloodingImageFilter<TInputImage, TOutputImage>::GetNeighborhoodSize() const
{
  return this->m_Neighborhood.Size();
}


template <typename TInputImage, typename TOutputImage>
void
VotingBinaryHoleFillFloodingImageFilter<TInputImage, TOutputImage>::ComputeArrayOfNeighborhoodBufferOffsets()
{
  //
  // Copy the offsets from the Input image.
  // We assume that they are the same for the output image.
  //
  const size_t sizeOfOffsetTableInBytes = (InputImageDimension + 1) * sizeof(unsigned long);

  memcpy(this->m_OffsetTable, this->m_OutputImage->GetOffsetTable(), sizeOfOffsetTableInBytes);


  //
  // Allocate the proper amount of buffer offsets.
  //
  const unsigned int neighborhoodSize = this->m_Neighborhood.Size();

  this->m_NeighborBufferOffset.resize(neighborhoodSize);


  //
  // Visit the offset of each neighbor in Index space and convert it to linear
  // buffer offsets that can be used for pixel access
  //
  using NeighborOffsetType = typename NeighborhoodType::OffsetType;

  for (unsigned int i = 0; i < neighborhoodSize; i++)
  {
    NeighborOffsetType offset = this->m_Neighborhood.GetOffset(i);

    signed int bufferOffset = 0; // must be a signed number

    for (unsigned int d = 0; d < InputImageDimension; d++)
    {
      bufferOffset += offset[d] * this->m_OffsetTable[d];
    }
    this->m_NeighborBufferOffset[i] = bufferOffset;
  }
}

template <typename TInputImage, typename TOutputImage>
void
VotingBinaryHoleFillFloodingImageFilter<TInputImage, TOutputImage>::ComputeBirthThreshold()
{
  const unsigned int neighborhoodSize = this->GetNeighborhoodSize();

  // Take the number of neighbors, discount the central pixel, and take half of them.
  auto threshold = static_cast<unsigned int>((neighborhoodSize - 1) / 2.0);

  // add the majority threshold.
  threshold += this->GetMajorityThreshold();

  // Set that number as the Birth Threshold
  this->SetBirthThreshold(threshold);
}

} // end namespace itk

#endif