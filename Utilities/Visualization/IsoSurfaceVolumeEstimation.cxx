
/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    IsoSurfaceVolumeEstimation.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or https://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "vtkMetaImageReader.h"
#include "vtkContourFilter.h"
#include "vtkImageData.h"
#include "vtkPolyDataWriter.h"
#include "vtkSTLWriter.h"
#include "vtkSmartPointer.h"
#include "vtksys/SystemTools.hxx"
#include "vtkMassProperties.h"
#include "vtkCleanPolyData.h"
#include "vtkTriangleFilter.h"
#include "itkMath.h"
#include "vtkVersion.h"


#define VTK_CREATE(type, name) vtkSmartPointer<type> name = vtkSmartPointer<type>::New()


int
main(int argc, char * argv[])
{

  if (argc < 7)
  {
    std::cerr << "Missing parameters" << std::endl;
    std::cerr << "Usage: " << argv[0] << " imageFileName isoValue ";
    std::cerr << " MethodID DatasetID ExpectedVolume ouputTextFile " << std::endl;
    std::cerr << std::endl;
    return 1;
  }

  VTK_CREATE(vtkMetaImageReader, imageReader);

  imageReader->SetFileName(argv[1]);
  imageReader->Update();

  float isoValue = atof(argv[2]);

  VTK_CREATE(vtkContourFilter, contourFilter);
  VTK_CREATE(vtkCleanPolyData, cleanPolyData);
  VTK_CREATE(vtkTriangleFilter, triangleFilter);
  contourFilter->SetValue(0, isoValue);
#if VTK_MAJOR_VERSION <= 5
  contourFilter->SetInput(imageReader->GetOutput());
  cleanPolyData->SetInput(contourFilter->GetOutput());
  triangleFilter->SetInput(cleanPolyData->GetOutput());
#else
  contourFilter->SetInputData(imageReader->GetOutput());
  cleanPolyData->SetInputData(contourFilter->GetOutput());
  triangleFilter->SetInputData(cleanPolyData->GetOutput());
#endif
  triangleFilter->PassVertsOff();
  triangleFilter->PassLinesOff();

  VTK_CREATE(vtkMassProperties, massProperties);
#if VTK_MAJOR_VERSION <= 5
  massProperties->SetInput(triangleFilter->GetOutput());
#else
  massProperties->SetInputData(triangleFilter->GetOutput());
#endif
  double volume = massProperties->GetVolume();

  std::cout << "Volume = " << volume << " mm3" << std::endl;

  //
  // Compute the radius of the equivalent-volume sphere
  //
  const double radius3 = ((volume * 3.0) / (4.0 * itk::Math::pi));
  const double radius = std::cbrt(radius3);


  const std::string segmentationMethodID = argv[3];
  const std::string datasetID = argv[4];
  const double      expectedVolume = atof(argv[5]);
  const std::string outpuFileName = argv[6];

  const double volumeDifference = volume - expectedVolume;

  const double ratio = volume / expectedVolume;

  const double errorPercent = volumeDifference / expectedVolume * 100.0;

  //
  // Append the value to the file containing estimations for this dataset.
  //
  std::ofstream ouputFile;

  // Check if the file exists. If it does not, let's print out the axis labels
  // right at the top of the file.
  const bool fileExists = vtksys::SystemTools::FileExists(outpuFileName.c_str());

  ouputFile.open(outpuFileName.c_str(), std::ios_base::app);

  if (!fileExists)
  {
    ouputFile << "SegmentationMethodID DatasetID ExpectedVolume ComputedVolume "
              << "PercentError RatioOfComputedVolumeToExpectedVolume "
              << "ComputedRadius " << std::endl;
  }

  ouputFile << segmentationMethodID << "  ";
  ouputFile << datasetID << "  ";
  ouputFile << expectedVolume << "   ";
  ouputFile << volume << "   ";
  ouputFile << errorPercent << "   ";
  ouputFile << ratio << "   ";
  ouputFile << radius << std::endl;

  ouputFile.close();

  return EXIT_SUCCESS;
}