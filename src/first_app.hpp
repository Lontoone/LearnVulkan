#pragma once

#include "display_window.hpp"
#include "pipeline.hpp"

namespace lontoone{
    class FirstApp{
        public:
            static constexpr int WIDTH = 800;
            static constexpr int HEIGHT = 600;

            void run();
        private : 
            Window window{WIDTH , HEIGHT , "Hello"};
            Pipeline pipeline{"./shaders/simple_shader.vert.spv" ,"./shaders/simple_shader.frag.spv" };
    };
}