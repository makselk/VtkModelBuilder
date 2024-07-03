#include "dcm_reader.h"

#include <vtkDICOMMetaData.h>
#include <vtkDICOMReader.h>
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

  image_data = reslice->GetOutput();
}
/*****************************************************************************/
