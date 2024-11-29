#pragma once
#include <thread>
#include "EngineEntity.h"
#include "Module.h"
#include "Object3D.h"
#include "Renderer.h"

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
    std::vector<glm::vec3> posGlobalesMediapipe;


    // -- HAND SHADER --
    GraphicShader _lbs_shader{};
    // Buffers
    StagingBuffer<IGEBufferUsage::vertex_buffer> _coord_buffer_hand{};
    StagingBuffer<IGEBufferUsage::vertex_buffer> _object_id_buffer_hand{};
    StagingBuffer<IGEBufferUsage::vertex_buffer> _material_indices_buffer_hand{};
    StagingBuffer<IGEBufferUsage::vertex_buffer> _uv_buffer_hand{};
    StagingBuffer<IGEBufferUsage::vertex_buffer> _joints_buffer_hand{};
    StagingBuffer<IGEBufferUsage::vertex_buffer> _weights_buffer_hand{};
    StagingBuffer<IGEBufferUsage::index_buffer> _index_buffer_hand{};
    // Uniform buffer
    StagingBuffer<IGEBufferUsage::storage_buffer> _obj_tr_buffer_hand{};
    StagingBuffer<IGEBufferUsage::storage_buffer> _joint_tr_buffer_hand{};
    StagingBuffer<IGEBufferUsage::storage_buffer> _materials_buffer_hand{};
    Sampler _sampler_hand{};

    glm::mat4 _camera;

    // Debug shader
    GraphicShader _debug_shader;
    StagingBuffer<IGEBufferUsage::vertex_buffer> _coord_buffer;
    StagingBuffer<IGEBufferUsage::vertex_buffer> _object_id_buffer;
    StagingBuffer<IGEBufferUsage::vertex_buffer> _material_indices_buffer;
    StagingBuffer<IGEBufferUsage::vertex_buffer> _uv_buffer;
    StagingBuffer<IGEBufferUsage::index_buffer> _index_buffer;
    StagingBuffer<IGEBufferUsage::storage_buffer> _obj_tr_buffer;
    StagingBuffer<IGEBufferUsage::storage_buffer> _materials_buffer;

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
        const mdph::Hierarchy& mediapipe_h,
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
};

