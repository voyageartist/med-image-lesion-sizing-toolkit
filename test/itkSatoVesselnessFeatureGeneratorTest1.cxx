/*=========================================================================

  Program:   Lesion Sizing Toolkit
  Module:    itkSatoVesselnessFeatureGeneratorTest1.cxx

  Copyright (c) Kitware Inc.
  All rights reserved.
  See Copyright.txt or https://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "itkSatoVesselnessFeatureGenerator.h"
#include "itkImage.h"
#include "itkSpatialObject.h"
#include "itkImageSpatialObject.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkTestingMacros.h"


int
itkSatoVesselnessFeatureGeneratorTest1(int argc, char * argv[])
{
  if (argc < 3)
  {
    std::cerr << "Missing parameters." << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " inputImage outputImage [sigma alpha1 alpha2]" << std::endl;
    return EXIT_FAILURE;
  }

  constexpr unsigned int Dimension = 3;

  using InputPixelType = signed short;
  using OutputPixelType = float;

  using InputImageType = itk::Image<InputPixelType, Dimension>;
  using OutputImageType = itk::Image<OutputPixelType, Dimension>;

  using ReaderType = itk::ImageFileReader<InputImageType>;
  using WriterType = itk::ImageFileWriter<OutputImageType>;

  using InputImageSpatialObjectType = itk::ImageSpatialObject<Dimension, InputPixelType>;
  using OutputImageSpatialObjectType = itk::ImageSpatialObject<Dimension, OutputPixelType>;

  ReaderType::Pointer reader = ReaderType::New();

  reader->SetFileName(argv[1]);

  ITK_TRY_EXPECT_NO_EXCEPTION(reader->Update());


  using SatoVesselnessFeatureGeneratorType = itk::SatoVesselnessFeatureGenerator<Dimension>;
  using SpatialObjectType = SatoVesselnessFeatureGeneratorType::SpatialObjectType;

  SatoVesselnessFeatureGeneratorType::Pointer featureGenerator = SatoVesselnessFeatureGeneratorType::New();

  ITK_EXERCISE_BASIC_OBJECT_METHODS(featureGenerator, SatoVesselnessFeatureGenerator, FeatureGenerator);


  InputImageSpatialObjectType::Pointer inputObject = InputImageSpatialObjectType::New();

  InputImageType::Pointer inputImage = reader->GetOutput();

  inputImage->DisconnectPipeline();

  inputObject->SetImage(inputImage);

  featureGenerator->SetInput(inputObject);


  double sigma = 1.0;
  if (argc > 3)
  {
    sigma = std::stod(argv[3]);
  }
  featureGenerator->SetSigma(sigma)