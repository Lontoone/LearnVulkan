#include "pipeline.hpp"
#include <fstream>
#include <stdexcept>
#include <iostream>
namespace lontoone{


    void Pipeline::createGraphicPipeline(const std::string& vertfile , const std::string& fragFile){
        auto vertCode = readFile(vertfile);
        auto fragCode = readFile(fragFile);
        std::cout<<"file size "<< vertCode.size()<<"\n";
    }

    Pipeline::Pipeline( const std::string& vertfile , const std::string& fragFile){
        createGraphicPipeline(vertfile , fragFile);
    }

    std::vector<char> Pipeline::readFile(const std::string& filepath){
        std::ifstream file(filepath , std::ios::ate | std::ios::binary);
        if(!file.is_open()){
            std::cout<<"can not open file " <<filepath <<"\n";
            throw std::runtime_error("fail to open file " + filepath);
        }

        size_t fileSize = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data() , fileSize);

        file.close();
        return buffer;
    }

}