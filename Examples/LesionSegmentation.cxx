#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMImageIOFactory.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkMetaImageIOFactory.h"
#include "itkImageSeriesReader.h"
#include "itkEventObject.h"
#include "itkImageToVTKImageFilter.h"
#include "vtkMassProperties.h"
#include "vtkImageData.h"
#include "vtkMarchingCubes.h"
#include "vtkPolyData.h"
#include "vtkSmartPointer.h"
#include "vtkSTLWriter.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkActor.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkImageData.h"
#include "vtkProperty.h"
#include "vtkImagePlaneWidget.h"
#include "vtkCellPicker.h"
#include "vtkPolyDataNormals.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkOutlineSource.h"
#include "vtkCommand.h"
#include "vtkWindowToImageFilter.h"
#include "vtkPNGWriter.h"
#include "itkOrientImageFilter.h"
#include "vtkVersion.h"

// This needs to come after the other includes to prevent the global definitions
// of PixelType to be shadowed by other declarations.
#include "itkLesionSegmentationImageFilter8.h"
#include "itkLesionSegmentationCommandLineProgressReporter.h"
#include "LesionSegmentationCLI.h"

#define VTK_CREATE(type, name) vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

// --------------------------------------------------------------------------
class SwitchVisibilityCallback : public vtkCommand
{
public:
  static SwitchVisibilityCallback *
  New()
  {
    return new SwitchVisibilityCallback;
  }

  void
  SetActor(vtkActor * aActor)
  {
    this->Actor = aActor;
  }
  void
  SetRenderWindow(vtkRenderWindow * aRenWin)
  {
    this->RenWin = aRenWin;
  }

  virtual void
  Execute(vtkObject * vtkNotUsed(caller), unsigned long, void *)
  {
    this->Actor->SetVisibility(1 - this->Actor->GetVisibility());
    this->RenWin->Render();
  }

protected:
  vtkActor *        Actor;
  vtkRenderWindow * RenWin;
};


// --------------------------------------------------------------------------
LesionSegmentationCLI::InputImageType::Pointer
GetImage(std::string dir, bool ignoreDirection)
{
  const unsigned int Dimension = LesionSegmentationCLI::ImageDimension;
  using ImageType = itk::Image<LesionSegmentationCLI::PixelType, Dimension>;

  using ReaderType = itk::ImageSeriesReader<ImageType>;
  ReaderType::Pointer reader = ReaderType::New();

  using ImageIOType = itk::GDCMImageIO;
  ImageIOType::Pointer dicomIO = ImageIOType::New();

  reader->SetImageIO(dicomIO);

  using NamesGeneratorType = itk::GDCMSeriesFileNames;
  NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();

  nameGenerator->SetUseSeriesDetails(true);
  nameGenerator->AddSeriesRestriction("0008|0021");
  nameGenerator->SetDirectory(dir);

  try
  {
    std::cout << std::endl << "The directory: " << std::endl;
    std::cout << std::endl << dir << std::endl << std::endl;
    std::cout << "Contains the following DICOM Series: ";
    std::cout << std::endl << std::endl;

    using SeriesIdContainer = std::vector<std::string>;

    const SeriesIdContainer & seriesUID = nameGenerator->GetSeriesUIDs();

    SeriesIdContainer::const_iterator seriesItr = seriesUID.begin();
    SeriesIdContainer::const_iterator seriesEnd = seriesUID.end();
    while (seriesItr != seriesEnd)
    {
      std::cout << seriesItr->c_str() << std::endl;
      seriesItr++;
    }


    std::string seriesIdentifier;
    seriesIdentifier = seriesUID.begin()->c_str();


    std::cout << std::endl << std::endl;
    std::cout << "Now reading series: " << std::endl << std::endl;
    std::cout << seriesIdentifier << std::endl;
    std::cout << std::endl << std::endl;


    using FileNamesContainer = std::vector<std::string>;
    FileNamesContainer fileNames;

    fileNames = nameGenerator->GetFileNames(seriesIdentifier);

    FileNamesContainer::const_iterator fitr = fileNames.begin();
    FileNamesContainer::const_iterator fend = fileNames.end();

    while (fitr != fend)
    {
      std::cout << *fitr << std::endl;
      ++fitr;
    }


    reader->SetFileNames(fileNames);

    try
    {
      reader->Update();
    }
    catch (itk::ExceptionObject & ex)
    {
      std::cout << ex << std::endl;
      return NULL;
    }


    ImageType::Pointer       image = reader->GetOutput();
    ImageType::DirectionType direction;
    direction.SetIdentity();
    image->DisconnectPipeline();
    std::cout << "Image Direction:" << image->GetDirection() << std::endl;


    if (ignoreDirection)
    {
      std::cout << "Ignoring the direction of the DICOM image and using identity." << std::endl;
      image->SetDirection(direction);
    }
    return image;
  }
  catch (itk::ExceptionObject & ex)
  {
    std::cout << ex << std::endl;
    return NULL;
  }

  return NULL;
}

// --------------------------------------------------------------------------
int
ViewImageAndSegmentationSurface(LesionSegmentationCLI::InputImageType::Pointer image,
                                vtkPolyData *                                  pd,
                                double *                                       roi,
                                LesionSegmentationCLI &                        args)
{

  std::cout << "Setting up visualization..." << std::endl;


  using InputImageType = LesionSegmentationCLI::InputImageType;

  using RealITKToVTKFilterType = itk::ImageToVTKImageFilter<LesionSegmentationCLI::InputImageType>;
  RealITKToVTKFilterType::Pointer itk2vtko = RealITKToVTKFilterType::New();
  itk2vtko->SetInput(image);
  itk2vtko->Update();

  // display the results.
  VTK_CREATE(vtkRenderer, renderer);
  VTK_CREATE(vtkRenderWindow, renWin);
  VTK_CREATE(vtkRenderWindowInteractor, iren);

  renderer->GetActiveCamera()->ParallelProjectionOn();

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
  vtkSmartPointer<vtkImagePlaneWidget> imagePlaneWidget[3];
  for (unsigned int i = 0; i < 3; i++)
  {
    imagePlaneWidget[i] = vtkSmartPointer<vtkImagePlaneWidget>::New();

    imagePlaneWidget[i]->DisplayTextOn();
#if VTK_MAJOR_VERSION <= 5
    imagePlaneWidget[i]->SetInput(itk2vtko->GetOutput());
#else
    imagePlaneWidget[i]->SetInputData(itk2vtko->GetOutput());
#endif


    imagePlaneWidget[i]->SetPlaneOrientation(i);
    imagePlaneWidget[i]->SetSlicePosition(pd->GetCenter()[i]);
    imagePlaneWidget[i]->SetPicker(picker);
    imagePlaneWidget[i]->RestrictPlaneToVolumeOn();
    double color[3] = { 0, 0, 0 };
    color[i] = 1;
    imagePlaneWidget[i]->GetPlaneProperty()->SetColor(color);
    imagePlaneWidget[i]->SetTexturePlaneProperty(ipwProp);
    imagePlaneWidget[i]->SetResliceInterpolateToLinear();
    imagePlaneWidget[i]->SetWindowLevel(1700, -500);
    imagePlaneWidget[i]->SetInteractor(iren);
    imagePlaneWidget[i]->On();
  }

  imagePlaneWidget[0]->SetKeyPressActivationValue('x');
  imagePlaneWidget[1]->SetKeyPressActivationValue('y');
  imagePlaneWidget[2]->SetKeyPressActivationValue('z');


  // Set the background to something grayish
  renderer->SetBackground(0.4392, 0.5020, 0.5647);

  VTK_CREATE(vtkPolyDataMapper, polyMapper);
  VTK_CREATE(vtkActor, polyActor);

  std::cout << "Computing surface normals for shading.." << std::endl;
  VTK_CREATE(vtkPolyDataNormals, normals);
#if VTK_MAJOR_VERSION <= 5
  normals->SetInput(pd);
#else
  normals->SetInputData(pd);
#endif
  normals->SetFeatureAngle(60.0);
  normals->Update();

  polyActor->SetMapper(polyMapper);
#if VTK_MAJOR_VERSION <= 5
  polyMapper->SetInput(normals->GetOutput());
#else
  polyMapper->SetInputData(normals->GetOutput());
#endif

  VTK_CREATE(vtkProperty, property);
  property->SetAmbient(0.1);
  property->SetDiffuse(0.1);
  property->SetSpecular(0.5);
  property->SetColor(0, 1, 0);
  property->SetLineWidth(2.0);
  property->SetRepresentationToSurface();

  polyActor->SetProperty(property);
  renderer->AddActor(polyActor);

  if (args.GetOptionWasSet("Wireframe"))
  {
    property->SetRepresentationToWireframe();
  }

  // Bring up the render window and b