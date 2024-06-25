#ifndef DCM_READER
#define DCM_READER

#include <vtkDICOMDirectory.h>
#include <vtkDICOMTag.h>
#include <vtkDICOMValue.h>
#include <vtkImageData.h>
#include <vtkSmartPointer.h>

/*****************************************************************************/
class DcmReader {
 public:
  explicit DcmReader(const std::string& path);

 public:
  vtkSmartPointer<vtkImageData> getImageData();
  vtkDICOMValue getMetaData(const vtkDICOMTag& tag);

 private:
  void initDcmDirectory();
  void checkSeveralStudies();
  void checkSeveralSeries();
  void initImageData();

 private:
  int study_number;
  int series_number;
  std::string dcm_dir_path;
  vtkSmartPointer<vtkDICOMDirectory> dcm_dir;
  vtkSmartPointer<vtkImageData> image_data;

 public:
  const vtkDICOMTag rows_tag = vtkDICOMTag(0x0028, 0x0010);
  const vtkDICOMTag cols_tag = vtkDICOMTag(0x0028, 0x0011);
  const vtkDICOMTag slices_tag = vtkDICOMTag(0x0054, 0x0081);
  const vtkDICOMTag modality_tag = vtkDICOMTag(0x0008, 0x0060);
  const vtkDICOMTag description_tag = vtkDICOMTag(0x0008, 0x103e);
};
/*****************************************************************************/
#endif  //  DCM_READER