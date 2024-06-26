#include "scene_provider.h"

#include <vtkImageSlice.h>
#include <vtkImageSliceMapper.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkPNGReader.h>
#include <vtkProperty.h>
#include <vtkSliderRepresentation2D.h>
#include <vtkTexturedButtonRepresentation2D.h>

#include "config_reader.h"
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

  // Model actor
  mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  mapper->SetInputData(model_builder->getModel());
  mapper->Update();

  actor = vtkSmartPointer<vtkActor>::New();
  actor->SetMapper(mapper);
  actor->GetProperty()->SetDiffuseColor(0.93, 0.71, 0.63);
  renderer->AddActor(actor);

  // Histogram actor
  if (ConfigReader::getInstance()->getVisualizateHistogram()) {
    vtkNew<vtkImageSliceMapper> image_mapper;
    image_mapper->SetInputConnection(
        model_builder->getHistogram()->GetOutputPort());
    image_mapper->BorderOff();
    vtkNew<vtkImageSlice> image_slice;
    image_slice->SetMapper(image_mapper);
    renderer->AddViewProp(image_slice);
  }

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

  // Threshold slider
  vtkNew<vtkSliderRepresentation2D> thresh_slider;
  thresh_slider->SetMinimumValue(model_builder->getLowerScalarRange());
  thresh_slider->SetMaximumValue(model_builder->getUpperScalarRange() / 10);
  thresh_slider->SetValue(ConfigReader::getInstance()->getThreshold());
  thresh_slider->SetTitleText("Threshold");
  thresh_slider->GetPoint1Coordinate()
      ->SetCoordinateSystemToNormalizedDisplay();
  thresh_slider->GetPoint2Coordinate()
      ->SetCoordinateSystemToNormalizedDisplay();
  thresh_slider->GetPoint1Coordinate()->SetValue(0.025, 0.1);
  thresh_slider->GetPoint2Coordinate()->SetValue(0.325, 0.1);
  thresh_slider->SetSliderLength(0.025);
  thresh_slider->SetSliderWidth(0.05);
  threshold_widget = vtkSmartPointer<vtkSliderWidget>::New();
  threshold_widget->SetInteractor(interactor);
  threshold_widget->SetRepresentation(thresh_slider);
  threshold_widget->SetAnimationModeToAnimate();
  threshold_widget->EnabledOn();
  threshold_widget->AddObserver(vtkCommand::InteractionEvent,
                                model_builder->getThresholdSliderCallback());

  // Radius slider
  vtkNew<vtkSliderRepresentation2D> radius_rep;
  radius_rep->SetMinimumValue(0.0);
  radius_rep->SetMaximumValue(9.0);
  radius_rep->SetValue(ConfigReader::getInstance()->getGaussRadius());
  radius_rep->SetTitleText("Radius");
  radius_rep->GetPoint1Coordinate()->SetCoordinateSystemToNormalizedDisplay();
  radius_rep->GetPoint2Coordinate()->SetCoordinateSystemToNormalizedDisplay();
  radius_rep->GetPoint1Coordinate()->SetValue(0.35, 0.1);
  radius_rep->GetPoint2Coordinate()->SetValue(0.65, 0.1);
  radius_rep->SetSliderLength(0.025);
  radius_rep->SetSliderWidth(0.05);
  radius_widget = vtkSmartPointer<vtkSliderWidget>::New();
  radius_widget->SetInteractor(interactor);
  radius_widget->SetRepresentation(radius_rep);
  radius_widget->SetAnimationModeToAnimate();
  radius_widget->EnabledOn();
  radius_widget->AddObserver(vtkCommand::InteractionEvent,
                             model_builder->getRadiusSliderCallback());

  // Deviation slider
  vtkNew<vtkSliderRepresentation2D> deviation_rep;
  deviation_rep->SetMinimumValue(0.0);
  deviation_rep->SetMaximumValue(9.0);
  deviation_rep->SetValue(ConfigReader::getInstance()->getGaussDeviation());
  deviation_rep->SetTitleText("Deviation");
  deviation_rep->GetPoint1Coordinate()
      ->SetCoordinateSystemToNormalizedDisplay();
  deviation_rep->GetPoint2Coordinate()
      ->SetCoordinateSystemToNormalizedDisplay();
  deviation_rep->GetPoint1Coordinate()->SetValue(0.675, 0.1);
  deviation_rep->GetPoint2Coordinate()->SetValue(0.975, 0.1);
  deviation_rep->SetSliderLength(0.025);
  deviation_rep->SetSliderWidth(0.05);
  deviation_widget = vtkSmartPointer<vtkSliderWidget>::New();
  deviation_widget->SetInteractor(interactor);
  deviation_widget->SetRepresentation(deviation_rep);
  deviation_widget->SetAnimationModeToAnimate();
  deviation_widget->EnabledOn();
  deviation_widget->AddObserver(vtkCommand::InteractionEvent,
                                model_builder->getDeviationSliderCallback());
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
  if (!polydata) {
    return;
  }
  mapper->SetInputData(polydata);
  mapper->Update();
}
/*****************************************************************************/
