#include "dogEngine/DeviceContext.h"
#include "dogEngine/Renderer.h"
#include "Vertex.h"

using namespace dg;

GLFWwindow* createGLTFWindow(u32 width,u32 height) {
  DG_INFO("Window Initiation")
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  GLFWwindow* window = glfwCreateWindow(width, height, "DogEngine", nullptr, nullptr);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR);
  return window;
}

bool isMinimized = false;
void windowIconifyCallback(GLFWwindow* window, int iconified){
  if(iconified){
    isMinimized = true;
  }else {
    isMinimized = false;
  }
}

int main() {
  auto window = createGLTFWindow(1380,720);
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

  DescriptorSetLayoutCreateInfo descLayoutInfo;
  descLayoutInfo.reset();
  descLayoutInfo.addBinding({VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,0,1,"diffuseTexture"});
  descLayoutInfo.addBinding({VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,1,1,"MRTTexture"});
  descLayoutInfo.addBinding({VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,2,1,"normalTexture"});
  descLayoutInfo.addBinding({VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,3,1,"emissiveTexture"});
  descLayoutInfo.addBinding({VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,4,1,"occlusionTexture"});
  descLayoutInfo.addBinding({VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,5,1,"viewProjectUniform"});
  descLayoutInfo.addBinding({VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,6,1,"LUTTexture"});
  descLayoutInfo.addBinding({VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,7,1,"radianceTexture"});
  descLayoutInfo.addBinding({VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,8,1,"iradianceTexture"});
  descLayoutInfo.setName("textureAndUniformBufferDesc");

  pipelineCreateInfo pipelineInfo;
  //pipelineInfo.addDescriptorSetlayout(slHandle);
  pipelineInfo.m_renderPassHandle = context->m_swapChainPass;
  pipelineInfo.m_depthStencil.setDepth(true, VK_COMPARE_OP_LESS_OR_EQUAL);
  pipelineInfo.m_vertexInput.Attrib = Vertex::getAttributeDescriptions();
  pipelineInfo.m_vertexInput.Binding = Vertex::getBindingDescription();
  pipelineInfo.m_shaderState.reset();
  auto vsCode = readFile("./shaders/vert.spv");
  auto fsCode = readFile("./shaders/frag.spv");
  pipelineInfo.m_shaderState.addStage(vsCode.data(), vsCode.size(), VK_SHADER_STAGE_VERTEX_BIT);
  pipelineInfo.m_shaderState.addStage(fsCode.data(), fsCode.size(), VK_SHADER_STAGE_FRAGMENT_BIT);
  pipelineInfo.m_shaderState.setName("pbrPipeline");
  pipelineInfo.name = pipelineInfo.m_shaderState.name;
  ProgramCreateInfo programInfo{};
  programInfo.pipelineInfo = pipelineInfo;
  programInfo.dslyaoutInfo = descLayoutInfo;
  programInfo.setName(pipelineInfo.name.c_str());
  auto programe = renderer.createProgram(programInfo);
  std::shared_ptr<Material> material = std::make_shared<Material>();
  MaterialCreateInfo matInfo{};
  matInfo.setProgram(programe);
  matInfo.name = "pbr_mat";
  Material* mat = renderer.createMaterial(matInfo);
  renderer.setCurrentMaterial(mat);
  //renderer.loadFromPath("./models/nanosuit/nanosuit.obj");
  //renderer.loadFromPath("./models/Sponza/sponza.obj");
  //renderer.loadFromPath("./models/duck/12248_Bird_v1_L2.obj");
  //renderer.loadFromPath("./models/FlightHelmet/FlightHelmet.gltf");
  renderer.loadFromPath("./models/DamagedHelmet/DamagedHelmet.gltf");
  //renderer.loadFromPath("./models/glTFSponza/Sponza.gltf");
  //renderer.loadFromPath("./models/gltfSponza/sponza.gltf");
  renderer.executeScene();

  while(!glfwWindowShouldClose(context->m_window)){
    glfwPollEvents();
    keycallback(context->m_window);
    if(isMinimized) continue;
    renderer.newFrame();
    renderer.drawScene();
    renderer.drawUI();
    renderer.present();
    //std::cout<<"11";
  }

// context的销毁还没有实现完。
  renderer.destroy();
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
