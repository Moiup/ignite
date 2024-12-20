#pragma once
#include <thread>
#include <mutex>

#include "EngineEntity.h"
#include "Module.h"
#include "Object3D.h"
#include "Renderer.h"
#include "DefaultRenderer.h"

#include "esocket/inc/EServerSocket.h"
//#include "InitNetworkProtocole.h"
//#include "MocapMessageProtocolNetwork.h"

#include "SkeletonDebug.h"

#include <string>
#include <fstream>

namespace mdph {
    const static int32_t NB_JOINTS = 21;
    const static int32_t NB_JOINTS_LFS = NB_JOINTS + 5;
    //const static int32_t NB_JOINTS_LFS = 6;

    struct DimMsg {
        uint32_t _width;
        uint32_t _height;
    };

    struct OkMsg {
        uint8_t _is_ok;
    };

    struct Desc {
        int32_t _nb_joints;
        int32_t _nb_hierarchy;
    };

    struct Hierarchy {
        std::vector<std::vector<int32_t>> _hierarchy;
    };

    struct Landmarks {
        uint32_t _handedness; // 0 -> right; 1 -> left
        glm::vec3 _landmarks[2][21];
        glm::vec3 _world_landmarks[2][21];
    };

    struct LandmarksForSkinning {
        Hierarchy _hierarchy;
        uint32_t _handedness; // 0 -> right; 1 -> left
        glm::vec3 _landmarks[2][NB_JOINTS_LFS];
        glm::vec3 _world_landmarks[2][NB_JOINTS_LFS];
    };

    struct Init {
        DimMsg _dim;
        Desc _desc;
        Hierarchy _hierarchy;
    };

    struct ImgSumPC {
        uint32_t width;
        uint32_t height;
        float intensity{ 1.0f };
        float hardness{ 1.0f };
        glm::vec4 color_chroma_key{ 0.0, 1.0, 0.0, 1.0 };
    };

    struct ImgDiffPC {
        uint32_t width;
        uint32_t height;
    };

    struct DebugBuf {
        glm::uvec2 pos;
        glm::uvec2 work_group;
        uint32_t local_index;
    };
}

class MediapipeAndGLTF : public Module
{
public:
    const std::string SERVER_ADDRESS{ "127.0.0.1" };
    const std::string SERVER_PORT{ "8013" };
    const float SCALE{ 0.25f };
private:
    EServerSocket _socket;
    EStream _mediapipe_stream;
    std::thread _network_thread;

    std::mutex _data_mutex;
    bool is_new_data{ false };

    const uint32_t _FRAME_NB_CHANNEL{ 4 };
    std::vector<uint8_t> _frame_data;
    StagingBuffer<IGEBufferUsage::transfer> _recv_frame_stag_buff;
    Image _recv_image;

    LoadedObjectInfo _red_sphere_info;
    Object3D _red_sphere;
    LoadedObjectInfo _blue_sphere_info;
    Object3D _blue_sphere;
    Mesh _cross_mesh;
    std::vector<Material> _cross_material;
    std::vector<uint32_t> _cross_material_indices;
    std::vector<Object3D> _mediapipe_cross;
    float _size;

    LoadedObjectInfo _hand_obj_info;
    LoadedObjectInfo _hand_info_to_gltf;
    Object3D _hand;

    SkeletonDebug _hand_skeleton;

    std::vector<std::vector<Object3D>> _hands;
    std::vector<Object3D> _mediapipe_red_sphere;
    std::vector<Object3D> _hand_blue_sphere;
    Object3D _parent_hand_blue_sphere;

    uint32_t _frame_i;

    mdph::Landmarks _landmarks;
    mdph::LandmarksForSkinning _lfs;
    mdph::Init _mediapipe_info;
    std::vector<glm::vec3> _posGlobalesMediapipe;


    // -- LBS SHADER --
    GraphicShader _lbs_shader{};
    GraphicsPipeline _lbs_pipeline;
    // Buffers
    StagingBuffer<IGEBufferUsage::vertex_buffer> _coord_buffer_lbs{};
    StagingBuffer<IGEBufferUsage::vertex_buffer> _object_id_buffer_lbs{};
    StagingBuffer<IGEBufferUsage::vertex_buffer> _material_indices_buffer_lbs{};
    StagingBuffer<IGEBufferUsage::vertex_buffer> _uv_buffer_lbs{};
    StagingBuffer<IGEBufferUsage::vertex_buffer> _joints_buffer_lbs{};
    StagingBuffer<IGEBufferUsage::vertex_buffer> _weights_buffer_lbs{};
    StagingBuffer<IGEBufferUsage::index_buffer> _index_buffer_lbs{};
    // Uniform buffer
    StagingBuffer<IGEBufferUsage::storage_buffer> _obj_tr_buffer_lbs{};
    StagingBuffer<IGEBufferUsage::storage_buffer> _joint_tr_buffer_lbs{};
    StagingBuffer<IGEBufferUsage::storage_buffer> _materials_buffer_lbs{};
    Sampler _sampler_hand{};

    glm::mat4 _camera;

    // -- Debug shader --
    GraphicShader _debug_shader;
    GraphicsPipeline _debug_pipeline;
    StagingBuffer<IGEBufferUsage::vertex_buffer> _coord_buffer;
    StagingBuffer<IGEBufferUsage::vertex_buffer> _object_id_buffer;
    StagingBuffer<IGEBufferUsage::vertex_buffer> _material_indices_buffer;
    StagingBuffer<IGEBufferUsage::vertex_buffer> _uv_buffer;
    StagingBuffer<IGEBufferUsage::index_buffer> _index_buffer;
    StagingBuffer<IGEBufferUsage::storage_buffer> _obj_tr_buffer;
    StagingBuffer<IGEBufferUsage::storage_buffer> _materials_buffer;

    // -- HAND SHADER --
    DefaultRenderer _fake_renderer;
    GraphicShader _hand_shader;
    GraphicsPipeline _hand_pipeline;
    StagingBuffer<IGEBufferUsage::vertex_buffer> _coord_hand;
    StagingBuffer<IGEBufferUsage::vertex_buffer> _joints_hand;
    StagingBuffer<IGEBufferUsage::vertex_buffer> _weights_hand;
    StagingBuffer<IGEBufferUsage::index_buffer> _index_hand;
    StagingBuffer<IGEBufferUsage::storage_buffer> _obj_tr_hand;
    StagingBuffer<IGEBufferUsage::storage_buffer> _joint_tr_hand;

    std::vector<VkSemaphore> _sem_rend_start;
    std::vector<VkSemaphore> _sem_rend_end;
    std::vector<VkSemaphore> _sem_copy_swap_end;
    std::vector<VkSemaphore> _sem_comp_sum_end;
    std::vector<VkSemaphore> _sem_img_error;
    uint64_t _sem_img_error_sig_val{ 0 };
    uint32_t _current_queue_i{0};
    uint32_t _to_present_img_i{0};

    // -- IMAGE SUM --
    ComputeShader _image_sum_shader;
    ComputePipeline _image_sum_pipeline;
    Texture2D _video_img;
    Texture2D _flipped_img;
    Texture2D _rendered_img;
    Texture2D _sum_img;
    mdph::ImgSumPC _img_sum_pc;

    // -- IMAGE ERROR --
    ComputeShader _image_error_shader;
    ComputePipeline _image_error_pipeline;
    DeviceBuffer<IGEBufferUsage::storage_buffer> _error_dev_buf;
    StagingBuffer<IGEBufferUsage::storage_buffer> _error_stag_buf;
    StagingBuffer<IGEBufferUsage::storage_buffer> _debug_buf;
    mdph::ImgDiffPC _img_diff_pc;

public:
    MediapipeAndGLTF();

    void init();
    void start();
    void update();
    void close();

    void networkInit();
    void networkProcess();

    void retargeting(
        const mdph::LandmarksForSkinning& lfs,
        Skeleton& skeleton
    );

    void createWrist(
        const mdph::Landmarks& mediapipe_landmarks,
        mdph::LandmarksForSkinning& lfs
    );
    void readMediapipeFile(const std::string& path);
    void fromGLTFToLandmarks(const std::string& path);

private:
    glm::mat4 findRotationMatrix(
        const glm::vec3& from,
        const glm::vec3& to
    );

    /**
    * Compute the rotation matrix to align vector 'from' on vector 'to'
    */
    glm::mat3 alignVectorMatrix(const glm::vec3& from, const glm::vec3& to);

    std::vector<glm::vec3> cube();
    std::vector<uint32_t> cubeIndex();

    void createLBSShader();
    void createDebugShader();
    void createHandShader();
    void createCompImageSumShader();

    void menu();
};

