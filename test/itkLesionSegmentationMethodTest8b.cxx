/*=========================================================================

  Program:   Lesion Sizing Toolkit
  Module:    itkLesionSegmentationMethodTest8b.cxx

  Copyright (c) Kitware Inc.
  All rights reserved.
  See Copyright.txt or https://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// The test runs a shape detection level set from user supplied seed points
// and then runs the shape detection level set with the results from the
// fast marching to get the final segmentation.

#include "itkLesionSegmentationImageFilter8.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkLandmarksReader.h"
#include "itkTestingMacros.h"


// Applies fast marhching followed by segmentation using geodesic active contours.
int
itkLesionSegmentationMethodTest8b(int argc, char * argv[])
{
  if (argc < 3)
  {
    std::cerr << "Missing parameters." << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " landmarksFile inputImage outputImage";
    std::cerr << " [SigmoidBeta] [-ResampleThickSliceData] [-UseVesselEnhancingDiffusion]" << std::endl;
    return EXIT_FAILURE;
  }

  bool useVesselEnhancingDiffusion = false, resampleThickSliceData = false;
  for (int i = 1; i < argc; i++)
  {
    useVesselEnhancingDi