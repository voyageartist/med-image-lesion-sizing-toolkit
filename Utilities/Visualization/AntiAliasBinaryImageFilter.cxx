
/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    AntiAliasBinaryImageFilter.cxx
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

#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkAntiAliasBinaryImageFilter.h"

int
main(int argc, char * argv[])
{
  if (argc < 3)
  {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " inputImage outputImageFloatPixelType ";
    std::cerr << " [RMS] [numberOfIterations]" << std::endl;
    return EXIT_FAILURE;
  }

  const std::string inputFilename = argv[1];
  const std::string outputFilename = argv[2];