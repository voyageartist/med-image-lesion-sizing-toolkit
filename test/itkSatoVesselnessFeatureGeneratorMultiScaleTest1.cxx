/*=========================================================================

  Program:   Lesion Sizing Toolkit
  Module:    itkSatoVesselnessFeatureGeneratorMultiScaleTest1.cxx

  Copyright (c) Kitware Inc.
  All rights reserved.
  See Copyright.txt or https://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSatoVesselnessFeatureGenerator.h"
#include "itkMaximumFeatureAggregator.h"
#include "itkTestingMacros.h"


int
itkSatoVesselnessFeatureGeneratorMultiScaleTest1(int argc, char * argv[])
{
  if (argc < 3)
  {
    std::cerr << "Missing parameters." << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " inputImage outputImage ";
    std::cerr << " [sigma alpha1 alpha2]" << std::endl;
    return EXIT_FAILURE;
  }


  constexpr unsigned int Dimension = 3;
  using InputPixelType = signed short;

  using InputImageType = itk::Image<InputPixelType, Dimension>;

  using InputImageReaderType = itk::ImageFileReader<InputImageType>;
  InputImageReaderType::Pointer inputImageReader = InputImageReaderType::New();

  inputImageReader->SetFileName(argv[1]);

  ITK_TRY_EXPECT_NO_EXCEPTION(inputImageReader->Update());


  using AggregatorType = itk::MaximumFeatureAggregator<Dimension>;

  AggregatorType::Pointer featureAggregator = AggregatorType::New();

  using FeatureGeneratorType = itk::SatoVesselnessFeatureGenerator<Dimension>;
  using SpatialObjectType = FeatureGeneratorType::SpatialObjectType;

  FeatureGeneratorType::Pointer featureGenerator1 = FeatureGeneratorType::New();

  ITK_EXERCISE_BASIC_OBJECT_METHODS(featureGenerator1, SatoVesselnessFeatureGenerator, FeatureGenerator);

  FeatureGeneratorType::Pointer featureGenerator2 = FeatureGeneratorType::New();

  ITK_EXERCISE_BASIC_OBJECT_METHODS(featureGenerator2, SatoVesselnessFeatureGenerator, FeatureGenerator);

  FeatureGeneratorType::Pointer featureGenerator3 = FeatureGeneratorType::New();

  ITK_EXERCISE_BASIC_OBJECT_METHODS(featureGenerator3, SatoVesselnessFeatureGenerator, FeatureGenerator);

  FeatureGeneratorType::Pointer featureGenerator4 = FeatureGeneratorType::New();
  ITK_EXERCISE_BASIC_OBJECT_METHODS(featureGenerator4, SatoVesselnessFeatureGenerator, FeatureGenerator);


  double smallestSigma = 1.0;
  if (argc > 3)
  {
    smallestSigma = std::stod(argv[3]);
  }
  featureGenerator1->SetSigma(smallestSigma);
  ITK_TEST_SET_GET_VALUE(smallestSigma, featureGenerator1