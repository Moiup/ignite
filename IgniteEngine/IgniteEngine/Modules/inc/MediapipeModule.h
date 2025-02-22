//#pragma once
//#include <thread>
//#include "EngineEntity.h"
//#include "Module.h"
//#include "Object3D.h"
//#include "Renderer.h"
//
//#include "esocket/inc/EServerSocket.h"
// //#include "InitNetworkProtocole.h"
// //#include "MocapMessageProtocolNetwork.h"
//
//#include "SkeletonDebug.h"
//
//#include <string>
//
//namespace mdp {
//    struct DimMsg {
//        uint32_t _width;
//        uint32_t _height;
//    };
//
//    struct OkMsg {
//        uint8_t _is_ok;
//    };
//
//    struct Landmarks {
//        uint32_t _handedness; // 0 -> right; 1 -> left
//        glm::vec3 _landmarks[2][21];
//        glm::vec3 _world_landmarks[2][21];
//    };
//}
//
//class MediapipeModule : public Module
//{
//public:
//    const std::string SERVER_ADDRESS{ "127.0.0.1" };
//    const std::string SERVER_PORT{ "8080" };
//    const float RAD_TO_DEG = 180 / M_PI;
//private:
//    EServerSocket _server_socket;
//    EStream _mediapipe_stream;
//    std::thread _network_thread;
//
//    Mesh _cube;
//    Mesh _cross_mesh;
//    std::vector<Object3D> _cross_objs;
//    std::vector<Material> _cross_material;
//    std::vector<uint32_t> _cross_material_indices;
//    float _size;
//
//    LoadedObjectInfo _obj_info;
//
//    const uint32_t _wrist_index = 21;
//
//    std::vector<std::vector<Object3D>> _hands;
//    std::vector<Object3D> _right_hand;
//    std::vector<Object3D> _wrists;
//    std::vector<glm::mat4> _alignment_matrices;
//    std::vector<glm::mat4> _transform_matrices;
//
//    mdp::Landmarks _landmarks;
//
//    uint32_t _frame_i;
//    std::vector<glm::vec3> _wrist_pos;
//
//    // -- DEBUG SHADER --
//    GraphicShader _debug_shader;
//    StagingBuffer<IGEBufferUsage::vertex_buffer> _coord_buffer_debug{};
//    StagingBuffer<IGEBufferUsage::vertex_buffer> _object_id_buffer_debug{};
//    StagingBuffer<IGEBufferUsage::vertex_buffer> _material_indices_buffer_debug{};
//    StagingBuffer<IGEBufferUsage::vertex_buffer> _uv_buffer_debug{};
//    StagingBuffer<IGEBufferUsage::index_buffer> _index_buffer_debug{};
//    StagingBuffer<IGEBufferUsage::storage_buffer> _obj_tr_buffer_debug{};
//    StagingBuffer<IGEBufferUsage::storage_buffer> _materials_buffer_debug{};
//
//    glm::mat4 _camera;
//   
//public:
//    MediapipeModule();
//
//    void init();
//    void start();
//    void update();
//    void close();
//
//    void networkProcess();
//    
//private:
//    void landmarksToLocal(const mdp::Landmarks& landmarks);
//    void landmarksToHand(const mdp::Landmarks& landmarks);
//    void landmarksRotationMatrices(const mdp::Landmarks& landmarks);
//
//    glm::mat4 findRotationMatrix(
//        const glm::vec3& from,
//        const glm::vec3& to
//    );
//
//    /**
//    * Compute the rotation matrix to align vector 'from' on vector 'to'
//    */
//    glm::mat3 alignVectorMatrix(const glm::vec3& from, const glm::vec3& to);
//
//    void wristPose(
//        Entity3D& wrist,
//        const glm::vec3& point1,
//        const glm::vec3& point2,
//        const glm::vec3& point3
//    );
//    void jointPose(
//        Entity3D& joint,
//        const glm::vec3& point1,
//        const glm::vec3& point2,
//        const glm::vec3& point3
//    );
//
//    glm::vec3 vectorAngle(glm::vec3 A, glm::vec3 B);
//
//    std::vector<glm::vec3> cube();
//    std::vector<uint32_t> cubeIndex();
//
//    void createShaderDebug();
//
//    void readMediapipeFile(const std::string& path);
//};
//
