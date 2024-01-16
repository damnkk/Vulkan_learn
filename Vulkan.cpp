#include "dogEngine/DeviceContext.h"
#include "dogEngine/Renderer.h"
#include "Vertex.h"
#include "ModelLoader.h"
#include "glslang_c_interface.h"

using namespace dg;

bool isMinimized = false;
void windowIconifyCallback(GLFWwindow* window, int iconified){
  if(iconified){
    isMinimized = true;
  }else {
    isMinimized = false;
  }
}

int main() {
  auto window = createGLFWWindow(1380,720);
  glfwSetWindowIconifyCallback(window, windowIconifyCallback);
  ContextCreateInfo contextInfo;
  int width,height;
  glfwGetWindowSize(window, &width, &height);
  contextInfo.set_window(width, height, window);
  contextInfo.m_applicatonName = "God Engine";
  contextInfo.m_debug = true;
  std::shared_ptr<DeviceContext> context = std::make_shared<DeviceContext>();
  context->init(contextInfo);
  Renderer renderer;
  renderer.init(context);

  //renderer.loadFromPath("./models/BoomBoxWithAxes/BoomBoxWithAxes.gltf");
  //renderer.loadFromPath("./models/Sponza/Sponza.gltf");
  //renderer.loadFromPath("./models/Suzanne/Suzanne.gltf");
  //renderer.loadFromPath("./models/Camera_01_2k/Camera_01_2k.gltf");
  renderer.loadFromPath("./models/DamagedHelmet/DamagedHelmet.gltf");
  //renderer.loadFromPath("./models/MetalRoughSpheres/MetalRoughSpheres.gltf");
  //renderer.loadFromPath("./models/ship_pinnace_1k/ship_pinnace_4k.gltf");


  //renderer.addSkyBox("./models/skybox/small_empty_room_4_2k.hdr");
  //renderer.addSkyBox("./models/skybox/small_empty_room_1_2k.hdr");
  renderer.addSkyBox("./models/skybox/graveyard_pathways_2k.hdr");
  renderer.executeScene();

  while(!glfwWindowShouldClose(context->m_window)){
    glfwPollEvents();
    GUI::getInstance().eventListen();
    if(isMinimized) continue;
    renderer.newFrame();
    ImGui::BeginChild("GameView",ImVec2(400,300),true);
    renderer.drawScene();
    ImGui::EndChild();
    renderer.drawUI();
    renderer.present();
  }

// context的销毁还没有实现完。
  renderer.destroy();
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
