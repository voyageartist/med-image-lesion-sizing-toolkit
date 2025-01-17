
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
#ifndef itkRegionCompetitionImageFilter_hxx
#define itkRegionCompetitionImageFilter_hxx

#include "itkConstNeighborhoodIterator.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionExclusionIteratorWithIndex.h"
#include "itkNeighborhoodAlgorithm.h"
#include "itkOffset.h"

namespace itk
{

/**
 * Constructor
 */
template <typename TInputImage, typename TOutputImage>
RegionCompetitionImageFilter<TInputImage, TOutputImage>::RegionCompetitionImageFilter()
{
  this->SetNumberOfRequiredInputs(1);

  this->m_MaximumNumberOfIterations = 10;
  this->m_CurrentIterationNumber = 0;

  this->m_NumberOfPixelsChangedInLastIteration = 0;
  this->m_TotalNumberOfPixelsChanged = 0;

  this->m_SeedArray1 = nullptr;
  this->m_SeedArray2 = nullptr;
  this->m_SeedsNewValues = nullptr;

  this->m_CurrentPixelIndex.Fill(0);

  this->m_OutputImage = nullptr;

  this->m_NumberOfLabels = 0;
  this->m_inputLabelsImage = nullptr;
}

/**
 * Destructor
 */
template <typename TInputImage, typename TOutputImage>
RegionCompetitionImageFilter<TInputImage, TOutputImage>::~RegionCompetitionImageFilter()
{
  if (this->m_SeedArray1)
  {
    delete[] this->m_SeedArray1;
    this->m_SeedArray1 = nullptr;
  }

  if (this->m_SeedArray2)
  {
    delete[] this->m_SeedArray2;
    this->m_SeedArray2 = nullptr;
  }

  if (this->m_SeedsNewValues)
  {
    delete[] this->m_SeedsNewValues;
    this->m_SeedsNewValues = nullptr;
  }
}


/**
 * Standard PrintSelf method.
 */
template <typename TInputImage, typename TOutputImage>
void
RegionCompetitionImageFilter<TInputImage, TOutputImage>::PrintSelf(std::ostream & os, Indent indent) const
{
  this->Superclass::PrintSelf(os, indent);
}


/**
 * Set the input image containing initial labeled regions
 */
template <typename TInputImage, typename TOutputImage>
void
RegionCompetitionImageFilter<TInputImage, TOutputImage>::SetInputLabels(const TOutputImage * inputLabeledImage)
{
  m_inputLabelsImage = inputLabeledImage;
}


template <typename TInputImage, typename TOutputImage>
void
RegionCompetitionImageFilter<TInputImage, TOutputImage>::GenerateData()
{
  this->AllocateOutputImageWorkingMemory();
  this->ComputeNumberOfInputLabels();
  this->AllocateFrontsWorkingMemory();
  this->InitializeNeighborhood();
  this->ComputeArrayOfNeighborhoodBufferOffsets();
  this->FindAllPixelsInTheBoundaryAndAddThemAsSeeds();
  this->IterateFrontPropagations();
}


template <typename TInputImage, typename TOutputImage>
void
RegionCompetitionImageFilter<TInputImage, TOutputImage>::IterateFrontPropagations()
{
  this->m_CurrentIterationNumber = 0;
  this->m_TotalNumberOfPixelsChanged = 0;
  this->m_NumberOfPixelsChangedInLastIteration = 0;

  while (this->m_CurrentIterationNumber < this->m_MaximumNumberOfIterations)
  {
    this->VisitAllSeedsAndTransitionTheirState();
    this->m_CurrentIterationNumber++;
    if (this->m_NumberOfPixelsChangedInLastIteration == 0)
    {
      break;
    }
  }
}


template <typename TInputImage, typename TOutputImage>
void
RegionCompetitionImageFilter<TInputImage, TOutputImage>::AllocateOutputImageWorkingMemory()
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
RegionCompetitionImageFilter<TInputImage, TOutputImage>::ComputeNumberOfInputLabels()
{

  using IteratorType = itk::ImageRegionConstIterator<TOutputImage>;

  IteratorType itr(m_inputLabelsImage, m_inputLabelsImage->GetBufferedRegion());

  itr.GoToBegin();

  this->m_NumberOfLabels = 0;

  while (!itr.IsAtEnd())
  {
    if (itr.Get() > this->m_NumberOfLabels)
    {
      this->m_NumberOfLabels = itr.Get();
    }
    ++itr;
  }
}

template <typename TInputImage, typename TOutputImage>
void
RegionCompetitionImageFilter<TInputImage, TOutputImage>::AllocateFrontsWorkingMemory()
{
  this->m_SeedArray1 = new SeedArrayType[this->m_NumberOfLabels];
  this->m_SeedArray2 = new SeedArrayType[this->m_NumberOfLabels];
  this->m_SeedsNewValues = new SeedNewValuesArrayType[this->m_NumberOfLabels];
}

template <typename TInputImage, typename TOutputImage>
void
RegionCompetitionImageFilter<TInputImage, TOutputImage>::InitializeNeighborhood()
{
  InputSizeType radius;
  radius.Fill(1);
  this->m_Neighborhood.SetRadius(radius);
}


template <typename TInputImage, typename TOutputImage>
void
RegionCompetitionImageFilter<TInputImage, TOutputImage>::FindAllPixelsInTheBoundaryAndAddThemAsSeeds()
{

  OutputImageRegionType region = m_inputLabelsImage->GetRequestedRegion();

  ConstNeighborhoodIterator<TOutputImage> bit;
  ImageRegionIterator<TOutputImage>       itr;
  ImageRegionIterator<SeedMaskImageType>  mtr;

  InputSizeType radius;
  radius.Fill(1);

  // Find the data-set boundary "faces"
  typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<TOutputImage>::FaceListType faceList;
  NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<TOutputImage>                        bC;
  faceList = bC(m_inputLabelsImage, region, radius);

  typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<TOutputImage>::FaceListType::iterator fit;


  // Process only the internal face
  fit = faceList.begin();

  this->m_InternalRegion = *fit;

  // Mark all the pixels in the boundary of the seed image as visited
  using ExclusionIteratorType = itk::ImageRegionExclusionIteratorWithIndex<SeedMaskImageType>;

  ExclusionIteratorType exIt(this->m_SeedsMask, region);

  exIt.SetExclusionRegion(this->m_InternalRegion);

  exIt.GoToBegin();

  while (!exIt.IsAtEnd())
  {
    exIt.Set(255);
    ++exIt;
  }

  bit = ConstNeighborhoodIterator<TOutputImage>(radius, m_inputLabelsImage, this->m_InternalRegion);
  itr = ImageRegionIterator<TOutputImage>(this->m_OutputImage, this->m_InternalRegion);
  mtr = ImageRegionIterator<SeedMaskImageType>(this->m_SeedsMask, this->m_InternalRegion);

  bit.GoToBegin();
  itr.GoToBegin();
  mtr.GoToBegin();

  unsigned int neighborhoodSize = bit.Size();

  constexpr OutputImagePixelType backgroundValue = 0; // no-label value.

  for (unsigned int lb = 0; lb < this->m_NumberOfLabels; lb++)
  {
    this->m_SeedArray1[lb].clear();
    this->m_SeedsNewValues[lb].clear();
  }


  while (!bit.IsAtEnd())
  {
    if (bit.GetCenterPixel() != backgroundValue)
    {
      itr.Set(bit.GetCenterPixel());
      mtr.Set(255);
    }
    else
    {
      itr.Set(backgroundValue);
      mtr.Set(0);

      // Search for foreground pixels in the neighborhood
      for (unsigned int i = 0; i < neighborhoodSize; ++i)
      {
        OutputImagePixelType value = bit.GetPixel(i);
        if (value != backgroundValue)
        {
          this->m_SeedArray1[value - 1].push_back(bit.GetIndex());
          break;
        }
      }
    }
    ++bit;
    ++itr;
    ++mtr;
  }


  for (unsigned int lb = 0; lb < this->m_NumberOfLabels; lb++)
  {
    this->m_SeedsNewValues[lb].reserve(this->m_SeedArray1[lb].size());
  }
}


template <typename TInputImage, typename TOutputImage>
void
RegionCompetitionImageFilter<TInputImage, TOutputImage>::VisitAllSeedsAndTransitionTheirState()
{
  for (unsigned int lb = 0; lb < this->m_NumberOfLabels; lb++)
  {
    using SeedIterator = typename SeedArrayType::const_iterator;

    SeedIterator seedItr = this->m_SeedArray1[lb].begin();

    this->m_NumberOfPixelsChangedInLastIteration = 0;

    // Clear the array of new values
    this->m_SeedsNewValues[lb].clear();

    while (seedItr != this->m_SeedArray1[lb].end())
    {
      this->SetCurrentPixelIndex(*seedItr);

      if (this->TestForAvailabilityAtCurrentPixel())
      {
        this->m_SeedsNewValues[lb].push_back(255); // FIXME: Use label value here
        this->PutCurrentPixelNeighborsIntoSeedArray();
        this->m_NumberOfPixelsChangedInLastIteration++;
      }
      else
      {
        this->m_SeedsNewValues[lb].push_back(0); // FIXME: Use No-label value here
        // Keep the seed to try again in the next iteration.
        this->m_SeedArray2->push_back(this->GetCurrentPixelIndex());
      }

      ++seedItr;
    }

    this->PasteNewSeedValuesToOutputImage();

    this->m_TotalNumberOfPixelsChanged += this->m_NumberOfPixelsChangedInLastIteration;

    // Now that the values have been copied to the output image, we can empty the
    // array in preparation for the next iteration
    this->m_SeedsNewValues[lb].clear();
  }

  this->SwapSeedArrays();
  this->ClearSecondSeedArray();
}


template <typename TInputImage, typename TOutputImage>
void
RegionCompetitionImageFilter<TInputImage, TOutputImage>::PasteNewSeedValuesToOutputImage()
{
  //
  // For each one of the label values
  //
  for (unsigned int lb = 0; lb < this->m_NumberOfLabels; lb++)
  {
    //
    //  Paste new values into the output image
    //
    using SeedIterator = typename SeedArrayType::const_iterator;

    SeedIterator seedItr = this->m_SeedArray1[lb].begin();

    using SeedsNewValuesIterator = typename SeedNewValuesArrayType::const_iterator;

    SeedsNewValuesIterator newValueItr = this->m_SeedsNewValues[lb].begin();

    while (seedItr != this->m_SeedArray1[lb].end())
    {
      this->m_OutputImage->SetPixel(*seedItr, *newValueItr);
      ++seedItr;
      ++newValueItr;
    }
  }
}

template <typename TInputImage, typename TOutputImage>
void
RegionCompetitionImageFilter<TInputImage, TOutputImage>::SwapSeedArrays()
{
  SeedArrayType * temporary = this->m_SeedArray1;
  this->m_SeedArray1 = this->m_SeedArray2;
  this->m_SeedArray2 = temporary;
}


template <typename TInputImage, typename TOutputImage>
void
RegionCompetitionImageFilter<TInputImage, TOutputImage>::ClearSecondSeedArray()
{
  if (this->m_SeedArray2)
  {
    delete[] this->m_SeedArray2;
  }
  this->m_SeedArray2 = new SeedArrayType[this->m_NumberOfLabels];
}


template <typename TInputImage, typename TOutputImage>
bool
RegionCompetitionImageFilter<TInputImage, TOutputImage>::TestForAvailabilityAtCurrentPixel() const
{
  return true; // FIXME
}


template <typename TInputImage, typename TOutputImage>
void
RegionCompetitionImageFilter<TInputImage, TOutputImage>::PutCurrentPixelNeighborsIntoSeedArray()
{
  //
  // Find the location of the current pixel in the image memory buffer
  //
  const OffsetValueType pixelOffset = this->m_OutputImage->ComputeOffset(this->GetCurrentPixelIndex());

  const OutputImagePixelType * buffer = this->m_OutputImage->GetBufferPointer();

  const OutputImagePixelType * currentPixelPointer = buffer + pixelOffset;


  const unsigned int neighborhoodSize = this->m_Neighborhood.Size();

  //
  // Visit the offset of each neighbor in Index as well as buffer space
  // and if they are backgroundValue then insert them as new seeds
  //
  using NeighborOffsetType = typename NeighborhoodType::OffsetType;

  using NeigborOffsetIterator = typename NeighborOffsetArrayType::const_iterator;

  NeigborOffsetIterator neighborItr = this->m_NeighborBufferOffset.begin();

  constexpr OutputImagePixelType backgroundValue = 0; // FIXME: replace with NO-Label.

  for (unsigned int i = 0; i < neighborhoodSize; ++i, ++neighborItr)
  {
    const OutputImagePixelType * neighborPixelPointer = currentPixelPointer + *neighborItr;

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
void
RegionCompetitionImageFilter<TInputImage, TOutputImage>::ComputeArrayOfNeighborhoodBufferOffsets()
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


} // end namespace itk

#endif