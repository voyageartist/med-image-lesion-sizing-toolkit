
/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
// Some DICOM files have direction cosines embedded within them, however often
// people have (incorrectly) ignored them and provided us seeds etc.
// The "-IgnoreDirection" is to accomodate these mistakes.

#if defined(_MSC_VER)
#  pragma warning(disable : 4786)
#endif


#include "itkIncludeRequiredIOFactories.h"
#include "itkImage.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkImageSeriesReader.h"
#include "itkImageFileWriter.h"

int
main(int argc, char * argv[])
{

  RegisterRequiredFactories();


  if (argc < 3)
  {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " DicomDirectory  outputFileName  [seriesName] [-IgnoreDirection]" << std::endl;
    return EXIT_FAILURE;
  }

  using PixelType = signed short;
  constexpr unsigned int Dimension = 3;

  using ImageType = itk::Image<PixelType, Dimension>;

  using ReaderType = itk::ImageSeriesReader<ImageType>;
  ReaderType::Pointer reader = ReaderType::New();

  using ImageIOType = itk::GDCMImageIO;
  ImageIOType::Pointer dicomIO = ImageIOType::New();

  reader->SetImageIO(dicomIO);

  using NamesGeneratorType = itk::GDCMSeriesFileNames;
  NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();

  nameGenerator->SetUseSeriesDetails(true);
  nameGenerator->AddSeriesRestriction("0008|0021");

  nameGenerator->SetDirectory(argv[1]);

  try
  {
    std::cout << std::endl << "The directory: " << std::endl;
    std::cout << std::endl << argv[1] << std::endl << std::endl;
    std::cout << "Contains the following DICOM Series: ";
    std::cout << std::endl << std::endl;

    using SeriesIdContainer = std::vector<std::string>;

    const SeriesIdContainer & seriesUID = nameGenerator->GetSeriesUIDs();

    auto seriesItr = seriesUID.begin();
    auto seriesEnd = seriesUID.end();
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
      return EXIT_FAILURE;
    }


    ImageType::Pointer       image = reader->GetOutput();
    ImageType::DirectionType direction;
    direction.SetIdentity();
    image->DisconnectPipeline();

    for (int i = 0; i < argc; i++)
    {
      if (strcmp("-IgnoreDirection", argv[i]) == 0)
      {
        image->SetDirection(direction);
      }
    }


    using WriterType = itk::ImageFileWriter<ImageType>;
    WriterType::Pointer writer = WriterType::New();

    writer->SetFileName(argv[2]);
    writer->UseCompressionOn();
    writer->SetInput(image);

    std::cout << "Writing the image as " << std::endl << std::endl;
    std::cout << argv[2] << std::endl << std::endl;

    try
    {
      writer->Update();
    }
    catch (itk::ExceptionObject & ex)
    {
      std::cout << ex << std::endl;
      return EXIT_FAILURE;
    }
  }
  catch (itk::ExceptionObject & ex)
  {
    std::cout << ex << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}