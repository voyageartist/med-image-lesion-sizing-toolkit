
/*=========================================================================

  Program:   Lesion Sizing Toolkit
  Module:    itkShapeDetectionLevelSetSegmentationModuleTest1.cxx

  Copyright (c) Kitware Inc.
  All rights reserved.
  See Copyright.txt or https://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "itkShapeDetectionLevelSetSegmentationModule.h"
#include "itkImage.h"
#include "itkSpatialObject.h"
#include "itkImageSpatialObject.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkTestingMacros.h"


int
itkShapeDetectionLevelSetSegmentationModuleTest1(int argc, char * argv[])
{
  if (argc < 4)
  {
    std::cerr << "Missing parameters." << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " inputImage featureImage outputImage ";
    std::cerr << " [propagationScaling curvatureScaling maximumNumberOfIterations]" << std::endl;
    return EXIT_FAILURE;
  }


  constexpr unsigned int Dimension = 3;
