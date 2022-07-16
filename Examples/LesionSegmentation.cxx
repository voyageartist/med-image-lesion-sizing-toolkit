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
  Execu