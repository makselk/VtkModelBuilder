#include "model_builder.h"

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
  if (slider == smooth_slider_callback) {
    setSmoothKernel(value);
  } else if (slider == hist_slider_callback) {
    setHistThreshold(value);
  }
}
/*****************************************************************************/
vtkSmartPointer<vtkButtonCallback> ModelBuilder::getBuildButtonCallback()
    const {
  return build_button_callback;
}
/*****************************************************************************/
vtkSmartPointer<vtkSliderCallback> ModelBuilder::getSmoothSliderCallback()
    const {
  return smooth_slider_callback;
}
/*****************************************************************************/
vtkSmartPointer<vtkSliderCallback> ModelBuilder::getHistSliderCallback() const {
  return hist_slider_callback;
}
/*****************************************************************************/
vtkSmartPointer<vtkPolyData> ModelBuilder::getModel() { return model; }
/*****************************************************************************/
void ModelBuilder::initCallbacks() {
  build_button_callback = vtkSmartPointer<vtkButtonCallback>::New();
  smooth_slider_callback = vtkSmartPointer<vtkSliderCallback>::New();
  hist_slider_callback = vtkSmartPointer<vtkSliderCallback>::New();
  build_button_callback->setParent(this);
  smooth_slider_callback->setParent(this);
  hist_slider_callback->setParent(this);
}
/*****************************************************************************/
void ModelBuilder::initParameters() {
  try {
    hist_threshold = ConfigReader::getInstance()->getHistogramThreshold();
    smooth_kernel = ConfigReader::getInstance()->getSmoothKernel();
  } catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
    hist_threshold = 13;
    smooth_kernel = 7;
  }
}
/*****************************************************************************/
void ModelBuilder::buildModel() { model = vtkSmartPointer<vtkPolyData>::New(); }
/*****************************************************************************/
void ModelBuilder::setSmoothKernel(double value) { smooth_kernel = value; }
/*****************************************************************************/
void ModelBuilder::setHistThreshold(double value) { hist_threshold = value; }
/*****************************************************************************/
