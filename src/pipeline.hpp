#pragma once
#include <string>
#include <vector>
namespace lontoone
{
  class Pipeline
  {
    
  public:
    Pipeline( const std::string& vertfile , const std::string& fragFile);
    ~Pipeline(){};

  private:
    static std::vector<char> readFile(const std::string& filepath);
    void createGraphicPipeline(const std::string& vertfile , const std::string& fragFile);
  };
      
} 
