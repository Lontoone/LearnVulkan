#include "lve_pipeline.hpp"
#include <fstream>
#include <stdexcept>
#include <iostream>
namespace lve{


    void LvePipeline::createGraphicPipeline(
        const std::string& vertfile,
        const std::string& fragFile,
        const PipelineConfigInfo& configInfo){
        auto vertCode = readFile(vertfile);
        auto fragCode = readFile(fragFile);
        std::cout<<"file size "<< vertCode.size()<<"\n";
    }

    void LvePipeline::createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule)
    {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        if (vkCreateShaderModule(lveDevice.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module");
        }
    }

    LvePipeline::LvePipeline(
        LveDevice& device,
        const std::string& vertfile,
        const std::string& fragFile,
        const PipelineConfigInfo& configInfo) :lveDevice{device} {
        createGraphicPipeline(vertfile , fragFile , configInfo);
    }

    PipelineConfigInfo LvePipeline::defaultPipelineConfigInfo(uint32_t width, uint32_t height)
    {
        PipelineConfigInfo configInfo{};
        return configInfo;
    }

    std::vector<char> LvePipeline::readFile(const std::string& filepath){
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