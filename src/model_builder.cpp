#include "model_builder.h"

#include <vtkFlyingEdges3D.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkImageThreshold.h>
#include <vtkPointData.h>
#include <vtkPolyDataConnectivityFilter.h>

#include "config_reader.h"
#include "scene_provider.h"

/*****************************************************************************/
void vtkButtonCallback::Execute(vtkObject* caller, unsigned long, void*) {
  parent->buttonEvent(this);
}
/*****************************************************************************/
vtkButtonCallback::vtkButtonCallback() {}
/*****************************************************************************/
void vtkButtonCallback::setParent(ModelBuilder* parent) {
  this->parent = parent;
}
/*****************************************************************************/
void vtkSliderCallback::Execute(vtkObject* caller, unsigned long, void*) {
  vtkSliderWidget* sliderWidget = reinterpret_cast<vtkSliderWidget*>(caller);
  double value =
      static_cast<vtkSliderRepresentation*>(sliderWidget->GetRepresentation())
          ->GetValue();
  parent->sliderEvent(this, value);
}
/*****************************************************************************/
vtkSliderCallback::vtkSliderCallback() {}
/*****************************************************************************/
void vtkSliderCallback::setParent(ModelBuilder* parent) {
  this->parent = parent;
}
/*****************************************************************************/
ModelBuilder::ModelBuilder(vtkSmartPointer<vtkImageData> image_data_) {
  image_data = image_data_;
  initHistogram();
  initCallbacks();
  initParameters();
  buildModel();
}
/*****************************************************************************/
void ModelBuilder::buttonEvent(vtkSmartPointer<vtkButtonCallback> button) {
  if (button == build_button_callback) {
    buildModel();
    SceneProvider::getInstance()->setPolyData(model);
  }
}
/*****************************************************************************/
void ModelBuilder::sliderEvent(vtkSmartPointer<vtkSliderCallback> slider,
                               double value) {
  if (slider == radius_slider_callback) {
    setGaussRadius(value);
  } else if (slider == deviation_slider_callback) {
    setGaussDeviation(value);
  } else if (slider == threshold_slider_callback) {
    setTreshold(value);
  }
}
/*****************************************************************************/
vtkSmartPointer<vtkButtonCallback> ModelBuilder::getBuildButtonCallback()
    const {
  return build_button_callback;
}
/*****************************************************************************/
vtkSmartPointer<vtkSliderCallback> ModelBuilder::getRadiusSliderCallback()
    const {
  return radius_slider_callback;
}
/*****************************************************************************/
vtkSmartPointer<vtkSliderCallback> ModelBuilder::getThresholdSliderCallback()
    const {
  return threshold_slider_callback;
}
/*****************************************************************************/
vtkSmartPointer<vtkSliderCallback> ModelBuilder::getDeviationSliderCallback()
    const {
  return deviation_slider_callback;
}
/*****************************************************************************/
double ModelBuilder::getUpperScalarRange() {
  return image_data->GetScalarRange()[1];
}
/*****************************************************************************/
double ModelBuilder::getLowerScalarRange() {
  return image_data->GetScalarRange()[0];
}
/*****************************************************************************/
vtkSmartPointer<vtkPolyData> ModelBuilder::getModel() { return model; }
/*****************************************************************************/
vtkSmartPointer<vtkImageHistogram> ModelBuilder::getHistogram() {
  return histogram;
}
/*****************************************************************************/
void ModelBuilder::initHistogram() {
  // Некоторая инфа о vtkImageData, на основе которого строится гистограмма
  double range[2];
  int dims[3];
  image_data->GetScalarRange(range);
  image_data->GetDimensions(dims);
  std::cout << "scalar range: " << "[" << range[0] << ", " << range[1] << "]"
            << std::endl;
  std::cout << "dims: " << dims[0] << ", " << dims[1] << ", " << dims[2]
            << std::endl;
  std::cout << "points: " << image_data->GetNumberOfPoints() << std::endl;

  histogram = vtkSmartPointer<vtkImageHistogram>::New();
  histogram->SetInputData(image_data);
  histogram->GenerateHistogramImageOn();
  histogram->SetHistogramImageScaleToSqrt();
  histogram->AutomaticBinningOn();
  histogram->Update();

  // Некоторая инфа, которую можно достать из гистограммы
  std::cout << "bins: " << histogram->GetNumberOfBins() << std::endl;
  vtkIdTypeArray* hist = histogram->GetHistogram();

  long long sum = 0;
  for (int i = 0; i != hist->GetNumberOfValues(); ++i) {
    sum += hist->GetValue(i);
  }
  std::cout << "sum: " << sum << std::endl;

  std::cout << "example data:" << std::endl;
  for (int i = 0; i != 10; ++i) {
    std::cout << hist->GetValue(i) << ", ";
  }
  std::cout << std::endl;
}
/*****************************************************************************/
void ModelBuilder::initCallbacks() {
  build_button_callback = vtkSmartPointer<vtkButtonCallback>::New();
  radius_slider_callback = vtkSmartPointer<vtkSliderCallback>::New();
  deviation_slider_callback = vtkSmartPointer<vtkSliderCallback>::New();
  threshold_slider_callback = vtkSmartPointer<vtkSliderCallback>::New();

  build_button_callback->setParent(this);
  radius_slider_callback->setParent(this);
  deviation_slider_callback->setParent(this);
  threshold_slider_callback->setParent(this);
}
/*****************************************************************************/
void ModelBuilder::initParameters() {
  try {
    gauss_radius = ConfigReader::getInstance()->getGaussRadius();
    gauss_deviation = ConfigReader::getInstance()->getGaussDeviation();
    threshold = ConfigReader::getInstance()->getThreshold();
  } catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
    gauss_radius = 5;
    gauss_deviation = 2;
    threshold = 13;
  }
}
/*****************************************************************************/
void ModelBuilder::buildModel() {
  model = vtkSmartPointer<vtkPolyData>::New();

  vtkNew<vtkImageGaussianSmooth> gauss;
  gauss->SetInputData(image_data);
  gauss->SetRadiusFactor(gauss_radius);
  gauss->SetStandardDeviation(gauss_deviation);
  gauss->Update();

  vtkNew<vtkImageThreshold> thresh_filter;
  thresh_filter->SetInputData(gauss->GetOutput());
  thresh_filter->ThresholdByLower(threshold);
  thresh_filter->SetOutValue(0);
  thresh_filter->Update();

  // Тут бы неплохо ыбло реализовать морфологию

  vtkNew<vtkFlyingEdges3D> flying_edges;
  flying_edges->SetInputData(gauss->GetOutput());
  flying_edges->ComputeNormalsOn();
  flying_edges->ComputeScalarsOff();
  flying_edges->SetValue(0, threshold / 2);
  flying_edges->Update();

  vtkNew<vtkPolyDataConnectivityFilter> confilter;
  confilter->SetInputData(flying_edges->GetOutput());
  confilter->SetExtractionModeToLargestRegion();
  confilter->Update();

  model = confilter->GetOutput();
}
/*****************************************************************************/
void ModelBuilder::setGaussRadius(double value) { gauss_radius = value; }
/*****************************************************************************/
void ModelBuilder::setGaussDeviation(double value) { gauss_deviation = value; }
/*****************************************************************************/
void ModelBuilder::setTreshold(double value) { threshold = value; }
/*****************************************************************************/
