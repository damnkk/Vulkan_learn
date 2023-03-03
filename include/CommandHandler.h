#pragma once 
#include "common.h"
#include "GraphicsPipeline.h"
#include "RenderPassHandler.h"
#include "Mesh&Model.h"

class CommandHandler{
public:
    CommandHandler();
    CommandHandler(MainDevice* main_device, GraphicPipeline* pipeline, RenderPassHandler* RenderPassHandler);
    void CreateCommandPool(QueueFamilyIndices& queueIndices);
    void CreateCommandBuffers(size_t const numFrameBuffers);
    void RecordOffScreenCommands(ImDrawData* draw_data, uint32_t currImage, VkExtent2D& imageExtent,
    std::vector<VkFramebuffer>& offScreenFrameBuffers, std::vector<Model>& scene, std::vector<VkDescriptorSet>& descriptorsets);
    void RecordCommands(ImDrawData* draw_data,uint32_t currImage,VkExtent2D& imageExtent,
    std::vector<VkFramebuffer>& frameBuffers,
    std::vector<VkDescriptorSet>& vp_desc_sets,
    std::vector<VkDescriptorSet>& light_desc_sets,
    std::vector<VkDescriptorSet>& input_desc_sets,
    std::vector<VkDescriptorSet>& settings_desc_set);

    void RecordCommandBuffers_forward(ImDrawData* draw_data, uint32_t currFrame,uint32_t currImage, VkExtent2D imgExtent,
    std::vector<VkFramebuffer>& frameBuffers,
    std::vector<Model>& scene,
    std::vector<VkDescriptorSet>& descriptorSets);
    void DestroyCommandPool();
    void FreeCommandBuffers();

    VkCommandPool& GetCommandPool(){ return m_GraphicsCommandPool;}
    VkCommandBuffer& GetCommandBuffer(uint32_t const index) { return m_CommandBuffers[index];}
    std::vector<VkCommandBuffer>& GetCommandBuffers() {return m_CommandBuffers;}
private:
    MainDevice *m_MainDevice;
    RenderPassHandler *m_RenderPassHandler;
    GraphicPipeline *m_graphicPipeline;

    VkCommandPool m_GraphicsCommandPool;
    std::vector<VkCommandBuffer> m_CommandBuffers;
};