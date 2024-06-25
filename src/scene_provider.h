#ifndef SCENE_PROVIDER
#define SCENE_PROVIDER

#include <vtkActor.h>
#include <vtkButtonWidget.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSliderWidget.h>
#include <vtkSmartPointer.h>

/*****************************************************************************/
class ModelBuilder;
/*****************************************************************************/
class SceneProvider {
 private:
  explicit SceneProvider(ModelBuilder* model_builder);

 public:
  static SceneProvider* getInstance(ModelBuilder* model_builder);
  static SceneProvider* getInstance();
  SceneProvider(SceneProvider const&) = delete;
  void operator=(SceneProvider const&) = delete;

 public:
  void start();
  void setPolyData(vtkSmartPointer<vtkPolyData> polydata);

 private:
  inline static SceneProvider* provider = nullptr;

  vtkSmartPointer<vtkRenderer> renderer;
  vtkSmartPointer<vtkRenderWindow> render_window;
  vtkSmartPointer<vtkRenderWindowInteractor> interactor;

  vtkSmartPointer<vtkPolyDataMapper> mapper;
  vtkSmartPointer<vtkActor> actor;

  vtkSmartPointer<vtkSliderWidget> hist_widget;
  vtkSmartPointer<vtkSliderWidget> smooth_widget;
  vtkSmartPointer<vtkButtonWidget> button_widget;
};
/*****************************************************************************/
#endif  // SCENE_PROVIDER