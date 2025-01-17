
/*=========================================================================

  Program:   Lesion Sizing Toolkit
  Module:    itkMaximumFeatureAggregatorTest1.cxx

  Copyright (c) Kitware Inc.
  All rights reserved.
  See Copyright.txt or https://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "itkMaximumFeatureAggregator.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkLungWallFeatureGenerator.h"
#include "itkSatoVesselnessSigmoidFeatureGenerator.h"
#include "itkSigmoidFeatureGenerator.h"
#include "itkTestingMacros.h"


int
itkMaximumFeatureAggregatorTest1(int argc, char * argv[])
{

  if (argc < 3)
  {
    std::cerr << "Missing parameters." << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " landmarksFile inputImage outputImage ";
    std::cerr << " [lowerThreshold upperThreshold] " << std::endl;
    return EXIT_FAILURE;
  }


  constexpr unsigned int Dimension = 3;
  using InputPixelType = signed short;

  using InputImageType = itk::Image<InputPixelType, Dimension>;

  using InputImageReaderType = itk::ImageFileReader<InputImageType>;
  InputImageReaderType::Pointer inputImageReader = InputImageReaderType::New();

  inputImageReader->SetFileName(argv[2]);

  ITK_TRY_EXPECT_NO_EXCEPTION(inputImageReader->Update());

  using AggregatorType = itk::MaximumFeatureAggregator<Dimension>;

  AggregatorType::Pointer featureAggregator = AggregatorType::New();

  ITK_EXERCISE_BASIC_OBJECT_METHODS(featureAggregator, MaximumFeatureAggregator, FeatureAggregator);

  using VesselnessGeneratorType = itk::SatoVesselnessSigmoidFeatureGenerator<Dimension>;
  VesselnessGeneratorType::Pointer vesselnessGenerator = VesselnessGeneratorType::New();

  using LungWallGeneratorType = itk::LungWallFeatureGenerator<Dimension>;
  LungWallGeneratorType::Pointer lungWallGenerator = LungWallGeneratorType::New();

  using SigmoidFeatureGeneratorType = itk::SigmoidFeatureGenerator<Dimension>;
  SigmoidFeatureGeneratorType::Pointer sigmoidGenerator = SigmoidFeatureGeneratorType::New();

  featureAggregator->AddFeatureGenerator(lungWallGenerator);
  featureAggregator->AddFeatureGenerator(vesselnessGenerator);
  featureAggregator->AddFeatureGenerator(sigmoidGenerator);


  using SpatialObjectType = AggregatorType::SpatialObjectType;

  using InputImageSpatialObjectType = itk::ImageSpatialObject<Dimension, InputPixelType>;
  InputImageSpatialObjectType::Pointer inputObject = InputImageSpatialObjectType::New();

  InputImageType::Pointer inputImage = inputImageReader->GetOutput();

  inputImage->DisconnectPipeline();

  inputObject->SetImage(inputImage);

  lungWallGenerator->SetInput(inputObject);
  vesselnessGenerator->SetInput(inputObject);
  sigmoidGenerator->SetInput(inputObject);

  lungWallGenerator->SetLungThreshold(-400);

  vesselnessGenerator->SetSigma(1.0);
  vesselnessGenerator->SetAlpha1(0.5);
  vesselnessGenerator->SetAlpha2(2.0);

  sigmoidGenerator->SetAlpha(1.0);
  sigmoidGenerator->SetBeta(-200.0);


  ITK_TRY_EXPECT_NO_EXCEPTION(featureAggregator->Update());


  SpatialObjectType::ConstPointer finalFeature = featureAggregator->GetFeature();

  using OutputImageSpatialObjectType = AggregatorType::OutputImageSpatialObjectType;
  OutputImageSpatialObjectType::ConstPointer outputObject =
    dynamic_cast<const OutputImageSpatialObjectType *>(finalFeature.GetPointer());

  using OutputImageType = AggregatorType::OutputImageType;
  OutputImageType::ConstPointer outputImage = outputObject->GetImage();

  using OutputWriterType = itk::ImageFileWriter<OutputImageType>;
  OutputWriterType::Pointer writer = OutputWriterType::New();

  writer->SetFileName(argv[3]);
  writer->SetInput(outputImage);
  writer->UseCompressionOn();

  ITK_TRY_EXPECT_NO_EXCEPTION(writer->Update());

  std::cout << "Test finished." << std::endl;
  return EXIT_SUCCESS;
}