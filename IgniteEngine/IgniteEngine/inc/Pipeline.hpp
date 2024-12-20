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

public:
	Pipeline();
	~Pipeline();

	void setShader(Shader* shader);

	const VkPipeline& getPipeline() const;
	const VkPipelineLayout& getPipelineLayout() const;
	const std::vector<VkDescriptorSet>& getDescriptorSets() const;
	Shader* getShader();

	void create();
	void destroy();

private:
	void setDescriptorSetLayoutBinding(
		std::vector<VkDescriptorSetLayoutBinding>& descriptor_set_binding_arr,
		std::unordered_map<std::string, ArrayBufferInfo>& buffer_arr
	);
	std::vector<VkDescriptorSetLayoutBinding> setDescriptorSetLayoutBindingArray();
	void createDescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding>& descriptor_set_layout_binding_arr);
	void createDescriptorSet(std::vector<VkDescriptorSetLayoutBinding>& descriptor_set_layout_binding_arr);
	
	template<IGEBufferUsage U>
	void setWriteDescriptorSet(
		std::unordered_map<std::string, Buffer<U>*>& buffer_arr,
		std::unordered_map<std::string, ArrayBufferInfo>& buffer_info_arr,
		std::vector<VkWriteDescriptorSet>& write_descriptor_set_arr
	);
	
	void setWriteDescriptorSet(
		std::unordered_map<std::string, std::vector<Sampler*>>& sampler_arr,
		std::unordered_map<std::string, SamplerInfo>& sampler_info_arr,
		std::vector<VkWriteDescriptorSet>& write_descriptor_set_arr
	);

	void setWriteDescriptorSet(
		std::unordered_map<std::string, std::vector<Texture2D*>>& texture_arr,
		std::unordered_map<std::string, TextureInfo>& texture_info_arr,
		std::vector<VkWriteDescriptorSet>& write_descriptor_set_arr
	);

	void updateDescriptorSets();

	void destroyDescriptorSetLayout();

	void createDescriptorSet();
	void destroyDescriptorSet();

	void createPipelineLayout();
	void destroyPipelineLayout();
	void destroyPipeline();

protected:
	virtual void createPipeline() = 0;
};
