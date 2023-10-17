/*=========================================================================

  Program:   Lesion Sizing Toolkit
  Module:    itkLesionSegmentationMethodTest2.cxx

  Copyright (c) Kitware Inc.
  All rights reserved.
  See Copyright.txt or https://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "itkLesionSegmentationMethod.h"
#include "itkImage.h"
#include "itkSpatialObject.h"
#include "itkImageMaskSpatialObject.h"
#include "itkDescoteauxSheetnessFeatureGenerator.h"
#include "itkFrangiTubularnessFeatureGenerator.h"
#include "itkGradientMagnitudeSigmoidFeatureGenerator.h"
#include "itkSatoLocalStructureFeatureGenerator.h"
#include "itkSatoVesselnessFeatureGenerator.h"
#include "itkTestingMacros.h"


int
itkLesionSegmentationMethodTest2(int itkNotUsed(argc), char * itkNotUsed(argv)[])
{
  constexpr unsigned int Dimension = 3;

  using MethodType = itk::LesionSegmentationMethod<Dimension>;

  MethodType::Pointer segmentationMethod = MethodType::New();

  ITK_EXERCISE_BASIC_OBJECT_METHODS(segmentationMethod, LesionSegmentationMethod, ProcessObject);

  using ImageMaskSpatialObjectType = itk::ImageMaskSpatialObject<Dimension>;

  ImageMaskSpatialObjectType::Pointer regionOfInterest = ImageMaskSpatialObjectType::New();

 