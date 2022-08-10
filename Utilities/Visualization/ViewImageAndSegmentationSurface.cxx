
/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    ViewImageAndSegmentationSurface.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or https://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "itkCommand.h"
#include "itkImage.h"
#include "itkVTKImageExport.h"
#include "itkVTKImageImport.h"
#include "itkConfidenceConnectedImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "vtkImageImport.h"
#include "vtkImageExport.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkActor.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkContourFilter.h"
#include "vtkImageData.h"
#include "vtkDataSet.h"
#include "vtkProperty.h"
#include "vtkImagePlaneWidget.h"
#include "vtkCellPicker.h"
#include "vtkPolyDataWriter.h"
#include "vtkSTLWriter.h"
#include "vtkSmartPointer.h"
#include "vtkVersion.h"


#define VTK_CREATE(type, name) vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

/**
 * This function will connect the given itk::VTKImageExport filter to
 * the given vtkImageImport filter.
 */
template <typename ITK_Exporter, typename VTK_Importer>
void
ConnectPipelines(ITK_Exporter exporter, VTK_Importer * importer)
{
  importer->SetUpdateInformationCallback(exporter->GetUpdateInformationCallback());
  importer->SetPipelineModifiedCallback(exporter->GetPipelineModifiedCallback());
  importer->SetWholeExtentCallback(exporter->GetWholeExtentCallback());
  importer->SetSpacingCallback(exporter->GetSpacingCallback());
  importer->SetOriginCallback(exporter->GetOriginCallback());
  importer->SetScalarTypeCallback(exporter->GetScalarTypeCallback());
  importer->SetNumberOfComponentsCallback(exporter->GetNumberOfComponentsCallback());
  importer->SetPropagateUpdateExtentCallback(exporter->GetPropagateUpdateExtentCallback());
  importer->SetUpdateDataCallback(exporter->GetUpdateDataCallback());
  importer->SetDataExtentCallback(exporter->GetDataExtentCallback());
  importer->SetBufferPointerCallback(exporter->GetBufferPointerCallback());
  importer->SetCallbackUserData(exporter->GetCallbackUserData());
}

/**
 * This function will connect the given vtkImageExport filter to
 * the given itk::VTKImageImport filter.
 */
template <typename VTK_Exporter, typename ITK_Importer>
void
ConnectPipelines(VTK_Exporter * exporter, ITK_Importer importer)
{
  importer->SetUpdateInformationCallback(exporter->GetUpdateInformationCallback());
  importer->SetPipelineModifiedCallback(exporter->GetPipelineModifiedCallback());
  importer->SetWholeExtentCallback(exporter->GetWholeExtentCallback());
  importer->SetSpacingCallback(exporter->GetSpacingCallback());
  importer->SetOriginCallback(exporter->GetOriginCallback());
  importer->SetScalarTypeCallback(exporter->GetScalarTypeCallback());
  importer->SetNumberOfComponentsCallback(exporter->GetNumberOfComponentsCallback());
  importer->SetPropagateUpdateExtentCallback(exporter->GetPropagateUpdateExtentCallback());
  importer->SetUpdateDataCallback(exporter->GetUpdateDataCallback());
  importer->SetDataExtentCallback(exporter->GetDataExtentCallback());
  importer->SetBufferPointerCallback(exporter->GetBufferPointerCallback());
  importer->SetCallbackUserData(exporter->GetCallbackUserData());
}


/**
 * This program implements an example connection between ITK and VTK
 * pipelines.  The combined pipeline flows as follows:
 *
 * itkImageFileReader ==> itkVTKImageExport ==>
 *    vtkImageImport ==> vtkImagePlaneWidget
 *
 * The resulting vtkImagePlaneWidget is displayed in a vtkRenderWindow.
 * Whenever the VTK pipeline executes, information is propagated
 * through the ITK pipeline.  If the ITK pipeline is out of date, it
 * will re-execute and cause the VTK pipeline to update properly as
 * well.
 */
int
main(int argc, char * argv[])
{

  // Load a scalar image and a segmentation mask and display the
  // scalar image with the contours of the mask overlaid on it.

  if (argc < 3)
  {
    std::cerr << "Missing parameters" << std::endl;
    std::cerr << "Usage: " << argv[0] << " imageFileName maskFileName";
    std::cerr << " [isocontourValue] [representation:0=surface/1=wireframe]";
    std::cerr << " [outputSurface.stl]";
    std::cerr << std::endl;
    return 1;
  }

  try
  {
    using PixelType = signed short;
    using MaskPixelType = float;

    constexpr unsigned int Dimension = 3;

    using ImageType = itk::Image<PixelType, Dimension>;

    using MaskImageType = itk::Image<MaskPixelType, Dimension>;

    using ReaderType = itk::ImageFileReader<ImageType>;

    using MaskReaderType = itk::ImageFileReader<MaskImageType>;

    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(argv[1]);
    reader->Update();

    MaskReaderType::Pointer maskReader = MaskReaderType::New();
    maskReader->SetFileName(argv[2]);
    maskReader->Update();


    using ExportFilterType = itk::VTKImageExport<ImageType>;
    using MaskExportFilterType = itk::VTKImageExport<MaskImageType>;

    ExportFilterType::Pointer     itkExporter1 = ExportFilterType::New();
    MaskExportFilterType::Pointer itkExporter2 = MaskExportFilterType::New();

    itkExporter1->SetInput(reader->GetOutput());
    itkExporter2->SetInput(maskReader->GetOutput());

    // Create the vtkImageImport and connect it to the
    // itk::VTKImageExport instance.
    VTK_CREATE(vtkImageImport, vtkImporter1);
    ConnectPipelines(itkExporter1, vtkImporter1.GetPointer());

    VTK_CREATE(vtkImageImport, vtkImporter2);
    ConnectPipelines(itkExporter2, vtkImporter2.GetPointer());


    vtkImporter1->Update();

    //------------------------------------------------------------------------
    // VTK pipeline.
    //------------------------------------------------------------------------

    // Create a renderer, render window, and render window interactor to
    // display the results.
    VTK_CREATE(vtkRenderer, renderer);
    VTK_CREATE(vtkRenderWindow, renWin);
    VTK_CREATE(vtkRenderWindowInteractor, iren);

    renWin->SetSize(600, 600);
    renWin->AddRenderer(renderer);
    iren->SetRenderWindow(renWin);


    // use cell picker for interacting with the image orthogonal views.
    //
    VTK_CREATE(vtkCellPicker, picker);
    picker->SetTolerance(0.005);


    // assign default props to the ipw's texture plane actor
    VTK_CREATE(vtkProperty, ipwProp);


    // Create 3 orthogonal view using the ImagePlaneWidget
    //
    VTK_CREATE(vtkImagePlaneWidget, xImagePlaneWidget);
    VTK_CREATE(vtkImagePlaneWidget, yImagePlaneWidget);
    VTK_CREATE(vtkImagePlaneWidget, zImagePlaneWidget);

    // The 3 image plane widgets are used to probe the dataset.
    //
    xImagePlaneWidget->DisplayTextOn();
#if VTK_MAJOR_VERSION <= 5
    xImagePlaneWidget->SetInput(vtkImporter1->GetOutput());
#else
    xImagePlaneWidget->SetInputData(vtkImporter1->GetOutput());
#endif
    xImagePlaneWidget->SetPlaneOrientationToXAxes();
    xImagePlaneWidget->SetSliceIndex(0);