#ifndef MODEL_BUILDER
#define MODEL_BUILDER

#include <vtkCommand.h>
#include <vtkImageData.h>
#include <vtkPolyData.h>
#include <vtkSliderRepresentation.h>
#include <vtkSliderWidget.h>
#include <vtkSmartPointer.h>

/*****************************************************************************/
class ModelBuilder;
/*****************************************************************************/
class vtkButtonCallback : public vtkCommand {
 public:
  static vtkButtonCallback* New() { return new vtkButtonCallback; }
  virtual void Execute(vtkObject* caller, unsigned long, void*);
  vtkButtonCallback();
  void setParent(ModelBuilder* parent);

 private:
  ModelBuilder* parent = nullptr;
};
/*****************************************************************************/
class vtkSliderCallback : public vtkCommand {
 public:
  static vtkSliderCallback* New() { return new vtkSliderCallback; }
  virtual void Execute(vtkObject* caller, unsigned long, void*);
  vtkSliderCallback();
  void setParent(ModelBuilder* parent);

 private:
  ModelBuilder* parent = nullptr;
};
/*****************************************************************************/
class ModelBuilder {
 public:
  explicit ModelBuilder(vtkSmartPointer<vtkImageData> image_data_);
  void buttonEvent(vtkSmartPointer<vtkButtonCallback> button);
  void sliderEvent(vtkSmartPointer<vtkSliderCallback> slider, double value);

 public:
  vtkSmartPointer<vtkButtonCallback> getBuildButtonCallback() const;
  vtkSmartPointer<vtkSliderCallback> getSmoothSliderCallback() const;
  vtkSmartPointer<vtkSliderCallback> getHistSliderCallback() const;

 public:
  vtkSmartPointer<vtkPolyData> getModel();

 private:
  void initCallbacks();
  void initParameters();
  void buildModel();
  void setSmoothKernel(double value);
  void setHistThreshold(double value);

 private:
  double hist_threshold;
  double smooth_kernel;
  vtkSmartPointer<vtkImageData> image_data;
  vtkSmartPointer<vtkPolyData> model;

  vtkSmartPointer<vtkButtonCallback> build_button_callback;
  vtkSmartPointer<vtkSliderCallback> smooth_slider_callback;
  vtkSmartPointer<vtkSliderCallback> hist_slider_callback;
};
/*****************************************************************************/
#endif  // MODEL_BULDER