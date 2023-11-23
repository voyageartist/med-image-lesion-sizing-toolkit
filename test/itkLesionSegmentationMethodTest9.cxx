/*=========================================================================

  Program:   Lesion Sizing Toolkit
  Module:    itkLesionSegmentationMethodTest9.cxx

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

#include "itkLesionSegmentationMethod.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkLandmarksReader.h"
#include "itkImageMaskSpatialObject.h"
#include "itkLungWallFeatureGenerator.h"
#include "itkSatoVesselnessSigmoidFeatureGenerator.h"
#include "itkGradientMagnitudeSigmoidFeatureGenerator.h"
#include "itkSigmoidFeatureGenerator.h"
#include "itkFastMarchingAndGeodesicActiveContourLevelSetSegmentationModule.h"
#include "itkMinimumFeatureAggregator.h"
#include "itkTestingMacros.h"

// Applies fast marhching followed by segmentation using geodesic active contours.
int
itkLesionSegmentationMethodTest9(int argc, char * argv[])
{
  if (argc < 3)
  {
    std::cerr << "Missing parameters." << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " landmarksFile inputImage outputImage";
    std::cerr << " [RMSErrorForGeodesicActiveContour]"
              << " [IterationsForGeodesicActiveContour]"
              << " [CurvatureScalingForGeodesicActiveContour]"
              << " [PropagationScalingForGeodesicActiveContour]"
              << " [AdvectionScalingForGeodesicActiveContour]";
    std::cerr << " [stoppingTime]";
    std::cerr << " [distanceFromSeeds]" << std::endl;
    return EXIT_FAILURE;
  }


  constexpr unsigned int Dimension = 3;
  using InputPixelType = signed short;

  using InputImageType = itk::Image<InputPixelType, Dimension>;

  using InputImageReaderType = itk::ImageFileReader<InputImageType>;
  InputImageReaderType::Pointer inputImageReader = InputImageReaderType::New();

  inputImageReader->SetFileName(argv[2]);

  ITK_TRY_EXPECT_NO_EXCEPTION(inputImageReader->Update());

  using MethodType = itk::LesionSegmentationMethod<Dimension>;

  MethodType::Pointer lesionSegmentationMethod = MethodType::New();

  using ImageMaskSpatialObjectType = itk::ImageMaskSpatialObject<Dimension>;

  ImageMaskSpatialObjectType::Pointer regionOfInterest = ImageMaskSpatialObjectType::New();

  lesionSegmentationMethod->SetRegionOfInterest(regionOfInterest);

  using VesselnessGeneratorType = itk::SatoVesselnessSigmoidFeatureGenerator<Dimension>;
  VesselnessGeneratorType::Pointer vesselnessGenerator = VesselnessGeneratorType::New();

  using LungWallGeneratorType = itk::LungWallFeatureGenerator<Dimension>;
  LungWallGeneratorType::Pointer lungWallGenerator = LungWallGeneratorType::New();

  using SigmoidFeatureGeneratorType = itk::SigmoidFeatureGenerator<Dimension>;
  SigmoidFeatureGeneratorType::Pointer sigmoidGenerator = SigmoidFeatureGeneratorType::New();

  using GradientMagnitudeSigmoidFeatureGeneratorType = itk::GradientMagnitudeSigmoidFeatureGenerator<Dimension>;
  GradientMagnitudeSigmoidFeatureGeneratorType::Pointer gradientMagnitudeSigmoidGenerator =
    GradientMagnitudeSigmoidFeatureGeneratorType::New();

  using FeatureAggregatorType = itk::MinimumFeatureAggregator<Dimension>;
  FeatureAggregatorType::Pointer featureAggregator = FeatureAggregatorType::New();
  featureAggregator->AddFeatureGenerator(lungWallGenerator);
  featureAggregator->AddFeatureGenerator(vesselnessGenerator);
  featureAggregator->AddFeatureGenerator(sigmoidGenerator);
  featureAggregator->AddFeatureGenerator(gradientMagnitudeSigmoidGenerator);
  lesionSegmentationMethod->AddFeatureGenerator(featureAggregator);

  using SpatialObjectType = MethodType::SpatialObjectType;
  using InputImageSpatialObjectType = itk::ImageSpatialObject<Dimension, InputPixelType>;
  InputImageSpatialObjectType::Pointer inputObject = InputImageSpatialObjectType::New();

  InputImageType::Pointer inputImage = inputImageReader->GetOutput();

  inputImage->DisconnectPipeline();

  inputObject->SetImage(inputImage);

  lungWallGenerator->SetInput(inputObject);
  vesselnessGenerator->SetInput(inputObject);
  sigmoidGenerator->SetInput(inputObject);
  gradientMagnitudeSigmoidGenerator->SetInput(inputObject);
  lungWallGenerator->SetLungThreshold(-400);
  vesselnessGenerator->SetSigma(1.0);
  vesselnessGenerator->SetAlpha1(0.5);
  vesselnessGenerator->SetAlpha2(2.0);
  vesselnessGenerator->SetSigmoidAlpha(-10.0);
  vesselnessGenerator->SetSigmoidBeta(80.0);
  sigmoidGenerator->SetAlpha(1.0);
  sigmoidGenerator->SetBeta(-200.0);
  gradientMagnitudeSigmoidGenerator->SetSigma(1.0);
  gradientMagnitudeSigmoidGenerator->SetAlpha(-100.0);
  gradientMagnitudeSigmoidGenerator->SetBeta(300);

  using SegmentationModuleType = itk::FastMarchingAndGeodesicActiveContourLevelSetSegmentationModule<Dimension>;
  SegmentationModuleType::Pointer segmentati