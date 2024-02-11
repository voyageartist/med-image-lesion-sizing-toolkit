/*=========================================================================

  Program:   Lesion Sizing Toolkit
  Module:    itkSegmentationVolumeEstimatorTest1.cxx

  Copyright (c) Kitware Inc.
  All rights reserved.
  See Copyright.txt or https://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "itkSegmentationVolumeEstimator.h"
#include "itkImage.h"
#include "itkSpatialObject.h"
#include "itkImageSpatialObject.h"
#include "itkImageMaskSpatialObject.h"
#include "itkTestingMacros.h"


namespace itk
{

class VolumeEstimatorSurrogate : public SegmentationVolumeEstimator<3>
{
public:
  /** Standard class type alias. */
  using Self = VolumeEstimatorSurrogate;
  using Superclass = SegmentationVolumeEstimator;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  itkNewMacro(Self);

  itkTypeMacro(VolumeEstimatorSurrogate, SegmentationVolumeEstimator);
};

} // namespace itk


int
itkSegmentationVolumeEstimatorTest1(int itkNotUsed(argc), char * itkNotUsed(argv)[])
{
  constexpr unsigned int Dimension = 3;

  using Volume