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

	bool _is_changed = false;
	std::unordered_map<std::string, std::vector<VkDescriptorBufferInfo>> _descriptor_buffer_infos;
	std::unordered_map<std::string, std::vector<VkDescriptorImageInfo>> _descriptor_image_infos;
	std::vector<VkWriteDescriptorSet> _write_descriptor_sets;

	void* _push_constants;

	int32_t* _shared_count;

protected:
	Pipeline();
	Pipeline(const Pipeline& pipeline);
	Pipeline(Shader& shader);
	~Pipeline();

	Pipeline& operator=(const Pipeline& pipeline);

	void create();
	void destroy();
public:
	void setPushConstants(void* push_constant);
	const void* getPushConstants() const;

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
		const std::vector<Sampler*>& samp
	);
	void setTextures2D(
		const std::string& name,
		const std::vector<Texture2D*>& textures
	);

	virtual void createPipeline() = 0;

	const VkPipeline getPipeline() const;
	const VkPipelineLayout& getPipelineLayout() const;
	const std::vector<VkDescriptorSet>& getDescriptorSets() const;
	const Shader& getShader() const;

	void update();
private:
	VkWriteDescriptorSet& setWriteDescriptorSet(
		const std::string& name
	);

	void setDescriptorSetLayoutBinding(
		std::vector<VkDescriptorSetLayoutBinding>& descriptor_set_binding_arr,
		const std::unordered_map<std::string, VkDescriptorSetLayoutBinding>& buffer_arr
	);

	void createDescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding>& descriptor_set_layout_binding_arr);
	void createDescriptorSet(std::vector<VkDescriptorSetLayoutBinding>& descriptor_set_layout_binding_arr);
	

	void destroyDescriptorSetLayout();

	void createDescriptorSet();
	void destroyDescriptorSet();

	void createPipelineLayout();
	void destroyPipelineLayout();
	void destroyPipeline();

protected:
};
