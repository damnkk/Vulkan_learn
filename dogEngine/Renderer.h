#ifndef RENDERER_H
#define RENDERER_H
#include "DeviceContext.h"
#include "GUI/gui.h"
#include "ModelLoader.h"
#include "Vertex.h"
#include <chrono>
#include <unordered_map>

namespace dg {

    void keycallback(GLFWwindow *window);
    std::vector<char> readFile(const std::string &filename);

    struct Renderer;
    struct objLoader;
    struct gltfLoader;
    struct TextureResource : public Resource {
        u32 poolIdx;
        TextureHandle handle;
        static const std::string k_type;
    };

    struct BufferResource : public Resource {
        u32 poolIdx;
        BufferHandle handle;
        static const std::string k_type;
    };


    struct SamplerResource : public Resource {
        u32 poolIdx;
        SamplerHandle handle;
        static const std::string k_type;
    };

    struct ProgramPassCreateInfo {
        PipelineCreateInfo pipelineInfo;
    };

    struct ProgramPass {
        PipelineHandle pipeline;
        std::vector<DescriptorSetLayoutHandle> descriptorSetLayout;
    };

    struct Program {
        Program() {}
        Program(std::shared_ptr<DeviceContext> context, std::string name);
        ~Program();
        void addPass(ProgramPassCreateInfo &pipeInfo);
        std::string name;
        std::vector<ProgramPass> passes;
        std::shared_ptr<DeviceContext> context;
    };

    struct MaterialCreateInfo {
        MaterialCreateInfo &setRenderOrder(u32 renderOrder);
        MaterialCreateInfo &setName(std::string name);
        std::string name;
        u32 renderOrder = -1;
        Renderer *renderer;
    };

    struct uniformRenderData {
        alignas(16) glm::vec3 lightDirection;
    };

    struct TextureBind {
        TextureHandle texture = {k_invalid_index};
        u32 bindIdx = -1;
    };

    struct Material : public Resource {
        struct alignas(16) aliInt {
            int idx;
        };

        struct alignas(16) UniformMaterial {
            alignas(16) glm::mat4 modelMatrix = glm::mat4(1.0f);
            alignas(16) glm::vec4 baseColorFactor = glm::vec4(glm::vec3(0.0f), 1.0f);
            alignas(16) glm::vec3 emissiveFactor = glm::vec3(0.0f);
            //transparnt, uvscale, envRotate
            alignas(16) glm::vec3 tueFactor = glm::vec3(1.0f, 1.0f, 0.0f);
            //metallic, roughness, __undefined_placeholder__
            alignas(16) glm::vec3 mrFactor = glm::vec3(1.0f, 1.0f, 0.0f);
            alignas(16) aliInt textureIndices[k_max_bindless_resource];
        } uniformMaterial;
        Material();
        void addTexture(Renderer *renderer, std::string name, std::string path);
        void addDiffuseEnvMap(Renderer *renderer, TextureHandle handle);
        void addSpecularEnvMap(Renderer *renderer, TextureHandle handle);
        void setIblMap(Renderer *renderer, std::string path);
        void updateProgram();
        void setDiffuseTexture(TextureHandle handle) {
            textureMap["DiffuseTexture"].texture = handle;
        }
        void setNormalTexture(TextureHandle handle) {
            textureMap["NormalTexture"].texture = handle;
        }
        void setMRTexture(TextureHandle handle) {
            textureMap["MetallicRoughnessTexture"].texture = handle;
        }
        void setEmissiveTexture(TextureHandle handle) {
            textureMap["EmissiveTexture"].texture = handle;
        }
        void setAoTexture(TextureHandle handle) {
            textureMap["AOTexture"].texture = handle;
        }
        void setProgram(const std::shared_ptr<Program> &program);

    protected:
        void addLutTexture(Renderer *renderer);
        void setIblMap(Renderer *renderer, TextureHandle handle);

    public:
        Renderer *renderer;
        u32 poolIdx;
        std::shared_ptr<Program> program;
        u32 renderOrder;
        std::unordered_map<std::string, TextureBind> textureMap;


        //render state
        bool depthTest = true;
        bool depthWrite = true;
        VkCullModeFlagBits cullModel = VK_CULL_MODE_BACK_BIT;
        VkBlendOp blendOp;
    };


    struct ResourceCache {
        void destroy(Renderer *renderer);

        TextureResource* queryTexture(const std::string& name);
        SamplerResource* querySampler(const std::string& name);
        BufferResource* QueryBuffer(const std::string& name);
        Material* QueryMaterial(const std::string& name);


        std::unordered_map<std::string, TextureResource *> textures;
        std::unordered_map<std::string, SamplerResource *> samplers;
        std::unordered_map<std::string, BufferResource *> buffers;
        std::unordered_map<std::string, Material *> materials;
    };
    struct Renderer {
    public:
        void init(std::shared_ptr<DeviceContext> context);
        void destroy();

        TextureResource *createTexture(TextureCreateInfo &textureInfo);
        BufferResource *createBuffer(BufferCreateInfo &bufferInfo);
        SamplerResource *createSampler(SamplerCreateInfo &samplerInfo);
        std::shared_ptr<Program> createProgram(const std::string &name, ProgramPassCreateInfo progPassCI);
        Material *createMaterial(MaterialCreateInfo &matInfo);
        void addSkyBox(std::string skyTexPath);

        TextureHandle upLoadTextureToGPU(std::string &texPath, TextureCreateInfo &texInfo);
        void addImageBarrier(VkCommandBuffer cmdBuffer, Texture *texture, ResourceState oldState, ResourceState newState, u32 baseMipLevel, u32 mipCount, bool isDepth);
        template<typename T>
        BufferHandle upLoadBufferToGPU(std::vector<T> &bufferData, const char *meshName);


        void destroyTexture(TextureResource *textureRes);
        void destroyBuffer(BufferResource *bufferRes);
        void destroySampler(SamplerResource *samplerRes);
        void destroyMaterial(Material *material);

        std::shared_ptr<DeviceContext> getContext() { return m_context; }
        void setDefaultMaterial(Material *material);
        Material *getDefaultMaterial();
        std::shared_ptr<objLoader> getObjLoader() { return m_objLoader; }
        std::shared_ptr<gltfLoader> getGltfLoader() { return m_gltfLoader; }
        ResourceCache &getResourceCache() { return this->m_resourceCache; }
        RenderResourcePool<TextureResource> &getTextures() { return m_textures; }
        RenderResourcePool<BufferResource> &getBuffers() { return m_buffers; }
        RenderResourcePool<Material> &getMaterials() { return m_materials; }
        RenderResourcePool<SamplerResource> &getSamplers() { return m_samplers; }
        TextureHandle getSkyTexture() { return m_skyTexture; }
        void newFrame();
        void present();
        void drawScene();
        void drawUI();
        void loadFromPath(const std::string &path);
        void executeScene();


    protected:
        void executeSkyBox();
        void destroySkyBox();
        void makeDefaultMaterial();

    private:
        ResourceCache m_resourceCache;
        RenderResourcePool<TextureResource> m_textures;
        RenderResourcePool<BufferResource> m_buffers;
        RenderResourcePool<Material> m_materials;
        RenderResourcePool<SamplerResource> m_samplers;
        std::vector<RenderObject> m_renderObjects;
        std::shared_ptr<DeviceContext> m_context;
        std::shared_ptr<objLoader> m_objLoader;
        std::shared_ptr<gltfLoader> m_gltfLoader;
        Material *m_defaultMaterial;

    private:
        TextureHandle m_skyTexture;
        bool have_skybox;
    };


    template<typename T>
    BufferHandle Renderer::upLoadBufferToGPU(std::vector<T> &bufferData, const char *meshName) {
        if (bufferData.empty()) return {k_invalid_index};
        VkDeviceSize BufSize = bufferData.size() * sizeof(T);
        BufferCreateInfo bufferInfo;
        bufferInfo.reset();
        bufferInfo.setData(bufferData.data());
        bufferInfo.setDeviceOnly(true);
        if (std::is_same<T, Vertex>::value) {
            std::string vertexBufferName = "vt";
            vertexBufferName += meshName;
            bufferInfo.setUsageSize(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, BufSize);
            bufferInfo.setName(vertexBufferName.c_str());
        } else if (std::is_same<T, u32>::value) {
            std::string IndexBufferName = "id";
            IndexBufferName += meshName;
            bufferInfo.setUsageSize(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, BufSize);
            bufferInfo.setName(IndexBufferName.c_str());
        }
        BufferHandle GPUHandle = createBuffer(bufferInfo)->handle;
        return GPUHandle;
    }
}// namespace dg
#endif//RENDERER_H
