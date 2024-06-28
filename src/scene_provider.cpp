#include "scene_provider.h"

#include <vtkCoordinate.h>
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

  // Build button
  vtkNew<vtkPNGReader> build_reader;
  build_reader->SetFileName("../import/build.png");
  build_reader->Update();
  vtkNew<vtkTexturedButtonRepresentation2D> build_representation;
  build_representation->SetNumberOfStates(1);
  build_representation->SetButtonTexture(0, build_reader->GetOutput());
  double button_bounds[6];
  calculateButtonBounds(0.0, 0.95, 100, button_bounds);
  build_representation->SetPlaceFactor(1.0);
  build_representation->PlaceWidget(button_bounds);
  build_widget = vtkSmartPointer<vtkButtonWidget>::New();
  build_widget->SetInteractor(interactor);
  build_widget->SetRepresentation(build_representation);
  build_widget->AddObserver(vtkCommand::StateChangedEvent,
                            model_builder->getBuildButtonCallback());
  build_widget->On();

  // Save button
  vtkNew<vtkPNGReader> save_reader;
  save_reader->SetFileName("../import/save.png");
  save_reader->Update();
  vtkNew<vtkTexturedButtonRepresentation2D> save_representation;
  save_representation->SetNumberOfStates(1);
  save_representation->SetButtonTexture(0, save_reader->GetOutput());
  double save_bounds[6];
  calculateButtonBounds(0.95, 0.95, 100, save_bounds);
  save_representation->SetPlaceFactor(1.0);
  save_representation->PlaceWidget(save_bounds);
  save_widget = vtkSmartPointer<vtkButtonWidget>::New();
  save_widget->SetInteractor(interactor);
  save_widget->SetRepresentation(save_representation);
  save_widget->AddObserver(vtkCommand::StateChangedEvent,
                           model_builder->getSaveButtonCallback());
  save_widget->On();

  // Morph slider
  vtkNew<vtkSliderRepresentation2D> morph_slider;
  morph_slider->SetMinimumValue(1.0);
  morph_slider->SetMaximumValue(13.0);
  morph_slider->SetValue(ConfigReader::getInstance()->getMorphRadius());
  morph_slider->SetTitleText("MorphRadius");
  morph_slider->GetPoint1Coordinate()->SetCoordinateSystemToNormalizedDisplay();
  morph_slider->GetPoint2Coordinate()->SetCoordinateSystemToNormalizedDisplay();
  morph_slider->GetPoint1Coordinate()->SetValue(0.025, 0.1);
  morph_slider->GetPoint2Coordinate()->SetValue(0.325, 0.1);
  morph_slider->SetSliderLength(0.025);
  morph_slider->SetSliderWidth(0.05);
  morph_widget = vtkSmartPointer<vtkSliderWidget>::New();
  morph_widget->SetInteractor(interactor);
  morph_widget->SetRepresentation(morph_slider);
  morph_widget->SetAnimationModeToAnimate();
  morph_widget->EnabledOn();
  morph_widget->AddObserver(vtkCommand::InteractionEvent,
                            model_builder->getMorphSliderCallback());

  // Threshold slider
  vtkNew<vtkSliderRepresentation2D> thresh_slider;
  thresh_slider->SetMinimumValue(model_builder->getLowerScalarRange());
  thresh_slider->SetMaximumValue(model_builder->getUpperScalarRange() / 5);
  thresh_slider->SetValue(ConfigReader::getInstance()->getThreshold());
  thresh_slider->SetTitleText("BinaryThreshold");
  thresh_slider->GetPoint1Coordinate()
      ->SetCoordinateSystemToNormalizedDisplay();
  thresh_slider->GetPoint2Coordinate()
      ->SetCoordinateSystemToNormalizedDisplay();
  thresh_slider->GetPoint1Coordinate()->SetValue(0.35, 0.1);
  thresh_slider->GetPoint2Coordinate()->SetValue(0.65, 0.1);
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
  radius_rep->SetTitleText("GaussRadius");
  radius_rep->GetPoint1Coordinate()->SetCoordinateSystemToNormalizedDisplay();
  radius_rep->GetPoint2Coordinate()->SetCoordinateSystemToNormalizedDisplay();
  radius_rep->GetPoint1Coordinate()->SetValue(0.675, 0.2);
  radius_rep->GetPoint2Coordinate()->SetValue(0.975, 0.2);
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
  deviation_rep->SetTitleText("GaussDeviation");
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
void SceneProvider::calculateButtonBounds(double x_pos, double y_pos,
                                          double size, double* bounds) {
  vtkNew<vtkCoordinate> coordinate;
  coordinate->SetCoordinateSystemToNormalizedDisplay();
  coordinate->SetValue(x_pos, y_pos);
  bounds[0] = coordinate->GetComputedDoubleDisplayValue(renderer)[0];
  bounds[1] = bounds[0] + size;
  bounds[2] = coordinate->GetComputedDoubleDisplayValue(renderer)[1];
  bounds[3] = bounds[2] + size;
  bounds[4] = bounds[5] = 0.0;
}
/*****************************************************************************/
