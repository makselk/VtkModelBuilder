#ifndef CONFIG_READER
#define CONFIG_READER

#include <jsoncpp/json/json.h>

/*****************************************************************************/
class ConfigReader {
 private:
  explicit ConfigReader(const std::string& path);

 public:
  static ConfigReader* getInstance(const std::string& path);
  static ConfigReader* getInstance();
  ConfigReader(ConfigReader const&) = delete;
  void operator=(ConfigReader const&) = delete;

  Json::Value getParamByName(const std::string& name);
  std::string getMriPath();
  std::string getModelPath();
  std::string getModelName();
  double getThreshold();
  double getGaussRadius();
  double getGaussDeviation();
  bool getVisualizateHistogram();

 private:
  inline static ConfigReader* reader = nullptr;
  std::string path;
  Json::Value config;
};
/*****************************************************************************/
#endif  // CONFIG_READER