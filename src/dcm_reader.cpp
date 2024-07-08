#include "dcm_reader.h"

#include <vtkDICOMMetaData.h>
#include <vtkDICOMReader.h>
#include <vtkImageResample.h>
#include <vtkImageReslice.h>
#include <vtkMatrix4x4.h>
#include <vtkStringArray.h>
#include <vtkTransform.h>

#include <filesystem>

/*****************************************************************************/
DcmReader::DcmReader(const std::string& path) {
  dcm_dir_path = path;
  if (!std::filesystem::exists(dcm_dir_path)) {
    throw std::runtime_error("No data found at " + dcm_dir_path);
  }
  initDcmDirectory();
  checkSeveralStudies();
  checkSeveralSeries();
  initImageData();
}
/*****************************************************************************/
vtkSmartPointer<vtkImageData> DcmReader::getImageData() { return image_data; }
/*****************************************************************************/
vtkDICOMValue DcmReader::getMetaData(const vtkDICOMTag& tag) {
  vtkDICOMMetaData* meta = dcm_dir->GetMetaDataForSeries(series_number);
  return meta->Get(tag);
}
/*****************************************************************************/
void DcmReader::initDcmDirectory() {
  dcm_dir = vtkSmartPointer<vtkDICOMDirectory>::New();
  dcm_dir->RequirePixelDataOn();
  dcm_dir->SetScanDepth(6);
  dcm_dir->SetDirectoryName(dcm_dir_path.c_str());
  dcm_dir->Update();
}
/*****************************************************************************/
void DcmReader::checkSeveralStudies() {
  int number_of_studies = dcm_dir->GetNumberOfStudies();
  if (number_of_studies == 0) {
    throw std::runtime_error("No studies in directory");
  }
  if (number_of_studies > 1) {
    throw std::runtime_error("Обнаружено " + std::to_string(number_of_studies) +
                             " исследований в директории. Необходима "
                             "директория с одним исследованием");
  }
  study_number = 0;
}
/*****************************************************************************/
void DcmReader::checkSeveralSeries() {
  int first_series = dcm_dir->GetFirstSeriesForStudy(study_number);
  int last_series = dcm_dir->GetLastSeriesForStudy(study_number);
  if (first_series != last_series) {
    std::cout << "Обнаружено " << last_series - first_series + 1
              << " серий в исследовании. Выберите необходимую серию:"
              << std::endl;
    std::cout << "№ |" << " description |" << " modality |" << " rows x cols |"
              << " slices " << std::endl;
    for (int i = first_series; i <= last_series; ++i) {
      vtkDICOMMetaData* meta = dcm_dir->GetMetaDataForSeries(i);
      vtkDICOMValue description = meta->Get(description_tag);
      vtkDICOMValue modality = meta->Get(modality_tag);
      vtkDICOMValue rows = meta->Get(rows_tag);
      vtkDICOMValue cols = meta->Get(cols_tag);
      int slices = dcm_dir->GetFileNamesForSeries(i)->GetNumberOfValues();
      std::cout << i << " | " << description.AsString() << " | "
                << modality.AsString() << " | " << rows.AsString() << "x"
                << cols.AsString() << " | " << slices << std::endl;
    }
    std::cin >> series_number;
    std::cout << "Выбрана серия №" << series_number << std::endl;
    return;
  }
  series_number = first_series;
}
/*****************************************************************************/
void DcmReader::initImageData() {
  vtkSmartPointer<vtkStringArray> filenames =
      dcm_dir->GetFileNamesForSeries(series_number);

  vtkNew<vtkDICOMReader> reader;
  reader->SetFileNames(filenames);
  reader->SetMemoryRowOrderToFileNative();
  reader->UpdateInformation();
  reader->Update();

  vtkNew<vtkMatrix4x4> patient_matrix;
  patient_matrix->DeepCopy(reader->GetPatientMatrix());
  patient_matrix->Invert();

  vtkNew<vtkTransform> transform;
  transform->SetMatrix(patient_matrix);
  transform->Update();

  vtkNew<vtkImageReslice> reslice;
  reslice->SetInputData(reader->GetOutput());
  reslice->SetResliceTransform(transform);
  reslice->SetInterpolationModeToLinear();
  reslice->AutoCropOutputOn();
  reslice->Update();

  int dims_src[3];
  reslice->GetOutput()->GetDimensions(dims_src);
  std::cout << "dims_src: [" << dims_src[0] << ", " << dims_src[1] << ", "
            << dims_src[2] << "]" << std::endl;

  double bounds_src[6];
  reslice->GetOutput()->GetBounds(bounds_src);
  std::cout << "bounds_src: [" << bounds_src[0] << ", " << bounds_src[1] << ", "
            << bounds_src[2] << "," << bounds_src[3] << ", " << bounds_src[4]
            << ", " << bounds_src[5] << "]" << std::endl;

  int max_dim = *std::max(dims_src + 0, dims_src + 2);
  double reduction_coef = 255.0 / static_cast<double>(max_dim);

  std::cout << "reduction coef: " << reduction_coef << std::endl;

  vtkNew<vtkImageResample> resample;
  resample->SetInputData(reslice->GetOutput());
  resample->SetAxisMagnificationFactor(0, reduction_coef);
  resample->SetAxisMagnificationFactor(1, reduction_coef);
  resample->SetAxisMagnificationFactor(2, reduction_coef);
  resample->SetInterpolationModeToLinear();
  resample->Update();

  int dims_resample[3];
  resample->GetOutput()->GetDimensions(dims_resample);
  std::cout << "dims_resample: [" << dims_resample[0] << ", "
            << dims_resample[1] << ", " << dims_resample[2] << "]" << std::endl;

  double bounds_resample[6];
  resample->GetOutput()->GetBounds(bounds_resample);
  std::cout << "bounds_resample: [" << bounds_resample[0] << ", "
            << bounds_resample[1] << ", " << bounds_resample[2] << ","
            << bounds_resample[3] << ", " << bounds_resample[4] << ", "
            << bounds_resample[5] << "]" << std::endl;

  image_data = resample->GetOutput();
}
/*****************************************************************************/
