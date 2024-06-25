#include "scene_provider.h"

#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPNGReader.h>
#include <vtkProperty.h>
#include <vtkSliderRepresentation2D.h>
#include <vtkTexturedButtonRepresentation2D.h>

#include "model_builder.h"

/*****************************************************************************/
SceneProvider::SceneProvider(ModelBuilder* model_builder) {
  // Scene
  renderer = vtkSmartPointer<vtkRenderer>::New();
  renderer->SetBackground(0.2, 0.224, 0.278);

  render_window = vtkSmartPointer<vtkRenderWindow>::New();
  render_window->SetSize(1200, 900);
  render_window->AddRenderer(renderer);

  interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
  interactor->SetRenderWindow(render_window);

  vtkNew<vtkInteractorStyleTrackballCamera> style;
  interactor->SetInteractorStyle(style);

  // Actor
  mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputData(model_builder->getModel());
  mapper->Update();

  actor = vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);
  actor->GetProperty()->SetDiffuseColor(0.93, 0.71, 0.63);

  // Update button
  vtkNew<vtkPNGReader> icon_reader;
  icon_reader->SetFileName("../import/icon.png");
  icon_reader->Update();
  vtkNew<vtkTexturedButtonRepresentation2D> button_representation;
  button_representation->SetNumberOfStates(1);
  button_representation->SetButtonTexture(0, icon_reader->GetOutput());
  button_widget = vtkSmartPointer<vtkButtonWidget>::New();
  button_widget->SetInteractor(interactor);
  button_widget->SetRepresentation(button_representation);
  button_widget->AddObserver(vtkCommand::StateChangedEvent,
                             model_builder->getBuildButtonCallback());
  button_widget->On();

  // Histogram slider
  vtkNew<vtkSliderRepresentation2D> hist_slider;
  hist_slider->SetMinimumValue(0.0);
  hist_slider->SetMaximumValue(100.0);
  hist_slider->SetValue(13.0);
  hist_slider->SetTitleText("Histogram Threshold");
  hist_slider->GetPoint1Coordinate()->SetCoordinateSystemToNormalizedDisplay();
  hist_slider->GetPoint2Coordinate()->SetCoordinateSystemToNormalizedDisplay();
  hist_slider->GetPoint1Coordinate()->SetValue(0.05, 0.1);
  hist_slider->GetPoint2Coordinate()->SetValue(0.45, 0.1);
  hist_slider->SetSliderLength(0.025);
  hist_slider->SetSliderWidth(0.05);
  hist_widget = vtkSmartPointer<vtkSliderWidget>::New();
  hist_widget->SetInteractor(interactor);
  hist_widget->SetRepresentation(hist_slider);
  hist_widget->SetAnimationModeToAnimate();
  hist_widget->EnabledOn();
  hist_widget->AddObserver(vtkCommand::InteractionEvent,
                           model_builder->getHistSliderCallback());

  // Smooth slider
  vtkNew<vtkSliderRepresentation2D> smooth_rep;
  smooth_rep->SetMinimumValue(1.0);
  smooth_rep->SetMaximumValue(9.0);
  smooth_rep->SetValue(7.0);
  smooth_rep->SetTitleText("Smooth Kernel");
  smooth_rep->GetPoint1Coordinate()->SetCoordinateSystemToNormalizedDisplay();
  smooth_rep->GetPoint2Coordinate()->SetCoordinateSystemToNormalizedDisplay();
  smooth_rep->GetPoint1Coordinate()->SetValue(0.55, 0.1);
  smooth_rep->GetPoint2Coordinate()->SetValue(0.95, 0.1);
  smooth_rep->SetSliderLength(0.025);
  smooth_rep->SetSliderWidth(0.05);
  smooth_widget = vtkSmartPointer<vtkSliderWidget>::New();
  smooth_widget->SetInteractor(interactor);
  smooth_widget->SetRepresentation(smooth_rep);
  smooth_widget->SetAnimationModeToAnimate();
  smooth_widget->EnabledOn();
  smooth_widget->AddObserver(vtkCommand::InteractionEvent,
                             model_builder->getSmoothSliderCallback());
}
/*****************************************************************************/
SceneProvider* SceneProvider::getInstance(ModelBuilder* model_builder) {
  if (!provider) {
    provider = new SceneProvider(model_builder);
  }
  return provider;
}
/*****************************************************************************/
SceneProvider* SceneProvider::getInstance() { return provider; }
/*****************************************************************************/
void SceneProvider::start() {
  interactor->Initialize();
  interactor->Start();
}
/*****************************************************************************/
void SceneProvider::setPolyData(vtkSmartPointer<vtkPolyData> polydata) {
  if (!mapper) {
    return;
  }
  mapper->SetInputData(polydata);
  mapper->Update();
}
/*****************************************************************************/
