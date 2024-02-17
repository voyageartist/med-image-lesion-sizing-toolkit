/*=========================================================================

  Program:   Lesion Sizing Toolkit
  Module:    itkSinglePhaseLevelSetSegmentationModuleTest1.cxx

  Copyright (c) Kitware Inc.
  All rights reserved.
  See Copyright.txt or https://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "itkSinglePhaseLevelSetSegmentationModule.h"
#include "itkImage.h"
#include "itkSpatialObject.h"
#include "itkImageSpatialObject.h"
#include "itkImageMaskSpatialObject.h"
#include "itkTestingMacros.h"


int
itkSinglePhaseLevelSetSegmentationModuleTest1(int itkNotUsed(argc), char * itkNotUsed(argv)[])
{
  constexpr unsigned int Dimension = 3;

  using SegmentationModuleType = itk::SinglePhaseLevelSetSegmentationModule<Dimension>;

  SegmentationModuleType::Pointer segmentationModule = SegmentationModuleType::New();

  ITK_EXERCISE_BASIC_OBJECT_METHODS(segmentationModule, SinglePhaseLevelSetSegmentationModule, SegmentationModule);

  using ImageSpatialObjectType = itk::ImageSpatialObject<Dimension>;

  ImageSpatialObjectType::Pointer inputObject = ImageSpatialObjectType::New();

  segmentationModule->SetInput(inputObject);

  ImageSpatialObjectType::Pointer featureObject = ImageSpatialObjectType::New();

  segmentationModule->SetFeature(featureObject);

  constexpr double propagationScaling = 1.3;
  segmentationModule->SetPropagationScaling(propagationScaling);
  ITK_TEST