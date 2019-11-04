# obj-loader
这是一个使用C++标准库读文件实现的加载obj文件模型的OpenGL库

## 项目目录结构说明
- bin：生成的可执行文件
- lib：编译所需库文件
- model：需要导入模型的文件夹
- shaders：光源着色器与物体着色器
- src：源码文件
    - Camera：用于控制镜头移动
    - main：项目入口
    - Shader：着色器类，封装了一些着色器的方法
    - stb_image：用于加载纹理文件的库
    - Mesh、MyModel：模型加载类，在cpp文件导入MyModel类后，可以模仿示例代码导入模型
        ```cpp
        MyModel ourModel("../model/dog/test.obj");
        Shader ourShader("../shaders/shader.vs", "../shaders/shader.fs");
        ......
        ourModel.draw(ourShader, vec3(0.1));
        ```

## 项目说明
1. 本项目编译环境为MinGW+GLFW+GLAD+GLM，请自行配置环境才能编译，编译方法为在项目根目录下执行`make`即可
2. 若开发环境为VS，配置好GLFW+GLAD+GLM后，仅需要将 src 中的源代码文件引入到项目中即可

## 程序运行
- bin目录下包含已经编译好的exe可执行文件，双击运行即可，通过W、A、S、D与鼠标控制视角查看模型

## 关于OpenGL
- 若本项目的代码对你来说难以理解，你也许可以参考[learnOpenGL-CN](https://learnopengl-cn.github.io/)进行opengl基础知识的学习