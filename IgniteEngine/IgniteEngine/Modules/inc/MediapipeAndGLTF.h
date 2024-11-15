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
    struct DimMsg {
        uint32_t _width;
        uint32_t _height;
    };

    struct OkMsg {
        uint8_t _is_ok;
    };

    struct Landmarks {
        uint32_t _handedness; // 0 -> right; 1 -> left
        glm::vec3 _landmarks[2][21];
        glm::vec3 _world_landmarks[2][21];
    };
}

class MediapipeAndGLTF : public Module
{
public:
    const std::string SERVER_ADDRESS{ "127.0.0.1" };
    const std::string SERVER_PORT{ "8013" };
    const float RAD_TO_DEG = 180 / M_PI;
private:
    EServerSocket _socket;
    EStream _mediapipe_stream;
    std::thread _network_thread;

    LoadedObjectInfo _cube_info;
    Object3D _cube;
    Mesh _cross_mesh;
    std::vector<Object3D> _cross_objs;
    std::vector<Material> _cross_material;
    std::vector<uint32_t> _cross_material_indices;
    float _size;

    LoadedObjectInfo _hand_obj_info;
    Object3D _hand;

    SkeletonDebug _hand_skeleton;

    const uint32_t _wrist_index = 21;

    std::vector<std::vector<Object3D>> _hands;
    std::vector<Object3D> _right_hand;
    std::vector<Object3D> _wrists;
    std::vector<glm::mat4> _alignment_matrices;
    std::vector<glm::mat4> _transform_matrices;
    std::vector<glm::mat4> _transform_matrices_debug;


    uint32_t _frame_i;
    std::vector<glm::vec3> _wrist_pos;

    mdph::Landmarks _landmarks;


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

    void readMediapipeFile(const std::string& path);

private:
    void landmarksLookAtMatrices(const mdph::Landmarks& landmarks);

    void landmarksToLocal(const mdph::Landmarks& landmarks);
    void landmarksRotationMatrices(const mdph::Landmarks& landmarks);

    void fingerAlignement(
        const mdph::Landmarks& landmarks,
        const Joint* wrist,
        const Joint* wrist_bis,
        const glm::vec3 vm1,
        const glm::vec3 vm2,
        const glm::vec3 vm3,
        const glm::vec3 vm4
    );

    glm::mat4 findRotationMatrix(
        const glm::vec3& from,
        const glm::vec3& to
    );

    /**
    * Compute the rotation matrix to align vector 'from' on vector 'to'
    */
    glm::mat3 alignVectorMatrix(const glm::vec3& from, const glm::vec3& to);

    glm::vec3 vectorAngle(glm::vec3 A, glm::vec3 B);

    std::vector<glm::vec3> cube();
    std::vector<uint32_t> cubeIndex();

    void createShaderHand();
    void createDebugShader();
};

