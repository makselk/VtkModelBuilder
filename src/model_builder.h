#ifndef MODEL_BUILDER
#define MODEL_BUILDER

#include <vtkCommand.h>
#include <vtkImageAccumulate.h>
#include <vtkImageData.h>
#include <vtkImageHistogram.h>
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
  vtkSmartPointer<vtkButtonCallback> getSaveButtonCallback() const;
  vtkSmartPointer<vtkSliderCallback> getRadiusSliderCallback() const;
  vtkSmartPointer<vtkSliderCallback> getDeviationSliderCallback() const;
  vtkSmartPointer<vtkSliderCallback> getThresholdSliderCallback() const;
  vtkSmartPointer<vtkSliderCallback> getMorphSliderCallback() const;

 public:
  double getUpperScalarRange();
  double getLowerScalarRange();
  vtkSmartPointer<vtkPolyData> getModel();
  vtkSmartPointer<vtkImageHistogram> getHistogram();

 private:
  void initHistogram();
  void initCallbacks();
  void initParameters();
  void saveModel();
  void buildModel();
  void setMorphRadius(double value);
  void setGaussRadius(double value);
  void setGaussDeviation(double value);
  void setTreshold(double value);

 private:
  double morph_radius;
  double gauss_radius;
  double gauss_deviation;
  double threshold;
  vtkSmartPointer<vtkImageData> image_data;
  vtkSmartPointer<vtkImageHistogram> histogram;
  vtkSmartPointer<vtkPolyData> model;

  vtkSmartPointer<vtkButtonCallback> save_button_callback;
  vtkSmartPointer<vtkButtonCallback> build_button_callback;
  vtkSmartPointer<vtkSliderCallback> morph_slider_callback;
  vtkSmartPointer<vtkSliderCallback> radius_slider_callback;
  vtkSmartPointer<vtkSliderCallback> deviation_slider_callback;
  vtkSmartPointer<vtkSliderCallback> threshold_slider_callback;
};
/*****************************************************************************/
#endif  // MODEL_BULDER