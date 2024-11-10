#include "Swapchain.h"
#include "Shader.h"
#include "Buffer.h"

class Pipeline
{
protected:
	Shader* _shader;

	std::vector<VkDescriptorSetLayout> _descriptor_set_layout;
	VkDescriptorPool _descriptor_pool;
	std::vector<VkDescriptorSet> _descriptor_sets;
	VkPipelineLayout _pipeline_layout;
	VkPipeline _pipeline{ nullptr };

	bool is_changed = false;
	std::vector<VkDescriptorBufferInfo> _descriptor_buffer_infos;
	std::vector<VkDescriptorImageInfo> _descriptor_image_infos;
	std::vector<VkWriteDescriptorSet> _write_descriptor_sets;

public:
	Pipeline();
	Pipeline(Shader& shader);
	~Pipeline();


	const VkPipeline& getPipeline() const;
	const VkPipelineLayout& getPipelineLayout() const;
	const std::vector<VkDescriptorSet>& getDescriptorSets() const;
	Shader* getShader();

	void create();
	void destroy();

	void setUniformBuffer(
		const std::string& name,
		const Buffer<IGEBufferUsage::uniform_buffer>& buff
	);
	void setStorageBuffer(
		const std::string& name,
		const Buffer<IGEBufferUsage::storage_buffer>& buff
	);
	void setSamplers(
		const std::string& name,
		const std::vector<Sampler&>& samp
	);
	void setTextures2D(
		const std::string& name,
		const std::vector<Texture2D&>& textures
	);

private:
	VkWriteDescriptorSet setWriteDescriptorSet(
		const std::string& name
	);

	void setDescriptorSetLayoutBinding(
		std::vector<VkDescriptorSetLayoutBinding>& descriptor_set_binding_arr,
		const std::unordered_map<std::string, VkDescriptorSetLayoutBinding>& buffer_arr
	);

	void createDescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding>& descriptor_set_layout_binding_arr);
	void createDescriptorSet(std::vector<VkDescriptorSetLayoutBinding>& descriptor_set_layout_binding_arr);
	
	void update();

	void destroyDescriptorSetLayout();

	void createDescriptorSet();
	void destroyDescriptorSet();

	void createPipelineLayout();
	void destroyPipelineLayout();
	void destroyPipeline();

protected:
	virtual void createPipeline() = 0;
};
