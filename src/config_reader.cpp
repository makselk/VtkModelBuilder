#include "config_reader.h"

#include <filesystem>
#include <fstream>

/*****************************************************************************/
ConfigReader::ConfigReader(const std::string& path) {
  if (!std::filesystem::exists(path)) {
    throw std::runtime_error("Config not found at " + path);
  }
  std::ifstream file(path);
  if (!file.is_open()) {
    file.close();
    throw std::runtime_error("Can't open file to read " + path);
  }
  file >> config;
  file.close();
}
/*****************************************************************************/
ConfigReader* ConfigReader::getInstance(const std::string& path) {
  if (reader == nullptr) {
    reader = new ConfigReader(path);
  }
  return reader;
}
/*****************************************************************************/
ConfigReader* ConfigReader::getInstance() { return reader; }
/*****************************************************************************/
Json::Value ConfigReader::getParamByName(const std::string& name) {
  if (!config.isMember(name)) {
    throw std::runtime_error("No member with key " + name);
  }
  return config[name];
}
/*****************************************************************************/
std::string ConfigReader::getMriPath() {
  return getParamByName("mri_path").asString();
}
/*****************************************************************************/
std::string ConfigReader::getModelPath() {
  return getParamByName("model_path").asString();
}
/*****************************************************************************/
std::string ConfigReader::getModelName() {
  return getParamByName("model_name").asString();
}
/*****************************************************************************/
double ConfigReader::getHistogramThreshold() {
  return getParamByName("histogram_threshold_percent").asDouble();
}
/*****************************************************************************/
double ConfigReader::getSmoothKernel() {
  return getParamByName("smooth_kernel").asDouble();
}
/*****************************************************************************/