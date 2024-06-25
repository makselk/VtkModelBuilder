#include "config_reader.h"
#include "dcm_reader.h"
#include "model_builder.h"
#include "scene_provider.h"

/*****************************************************************************/
int main(int, char*[]) {
  ConfigReader::getInstance("../import/config.json");
  DcmReader dcm_reader(ConfigReader::getInstance()->getMriPath());
  ModelBuilder model_builder(dcm_reader.getImageData());
  SceneProvider::getInstance(&model_builder);
  SceneProvider::getInstance()->start();
  return EXIT_SUCCESS;
}
/*****************************************************************************/
