
/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    CannyEdgeDetectionImageFilter2.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or https://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#  pragma warning(disable : 4786)
#endif

#ifdef __BORLANDC__
#  define ITK_LEAN_AND_MEAN
#endif

#include "itkIncludeRequiredIOFactories.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCastImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkCannyEdgeDetectionRecursiveGaussianImageFilter.h"

#include <string>


int
main(int argc, char * argv[])
{

  RegisterRequiredFactories();

  if (argc < 3)
  {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " inputImage outputImage [sigma upperThrehold lowerThreshold]" << std::endl;
    return EXIT_FAILURE;
  }

  const char * inputFilename = argv[1];
  const char * outputFilename = argv[2];

  float sigma = 2.0;
  float lowerThreshold = 10.0;
  float upperThreshold = 20.0;

  if (argc > 3)
  {
    sigma = std::stod(argv[3]);
  }

  if (argc > 4)
  {
    upperThreshold = std::stod(argv[4]);
  }

  if (argc > 5)