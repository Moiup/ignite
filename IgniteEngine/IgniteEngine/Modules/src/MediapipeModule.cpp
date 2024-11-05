//#include "MediapipeModule.h"
//
//MediapipeModule::MediapipeModule() : Module::Module() {
//;
//}
//
//void MediapipeModule::init() {
//    Module::init();
//
//    {
//        glm::vec3 v_s = glm::vec3(0, 1, 0);
//        glm::vec3 v_m = glm::vec3(1, 1, 1);
//        glm::mat4 alignment_m = findRotationMatrix(v_s, v_m);
//        const glm::vec4 v_aligned = alignment_m * glm::vec4(glm::normalize(v_s), 1.0);
//        std::cout << makeString(glm::normalize(v_s)) << " -- ";
//        std::cout << makeString(glm::normalize(v_m)) << " -- ";
//        std::cout << makeString(glm::normalize(glm::vec3(v_aligned))) << " -- ";
//        std::cout << makeString(glm::vec4(glm::normalize(v_m), 1.0) - v_aligned) << std::endl;
//    }
//
//    _server_socket.Port() = SERVER_PORT;
//    _server_socket.Address() = SERVER_ADDRESS;
//    _server_socket.Initialise();
//    _server_socket.Bind();
//    _server_socket.Listen();
//
//    std::cout << "Listening on " << SERVER_ADDRESS << " -- " << SERVER_PORT << std::endl;
//    std::cout << "Waiting for mediapipe client to connect...";
//    _mediapipe_stream = _server_socket.Accept();
//    std::cout << "connected!" << std::endl;
//
//    mdp::DimMsg dim_msg;
//    _mediapipe_stream.Recv(sizeof(dim_msg), reinterpret_cast<char*>(&dim_msg));
//    std::cout << "received: " << std::endl;
//    std::cout << "    width: " << dim_msg._width << std::endl;
//    std::cout << "    height: " << dim_msg._height << std::endl;
//
//    DefaultConf::render_window_width = dim_msg._width * 2;
//    DefaultConf::render_window_height = dim_msg._height * 2;
//}
//
//void MediapipeModule::start() {
//    Module::start();
//
//    _obj_info.loadWavefont("../../assets/3d_objects/cube_textured/cube_tex.obj");
//
//    //SkeletonDebug::createCrossMesh(_cross_mesh, 0.1);
//    //SkeletonDebug::createCrossMaterial(_cross_material, _cross_material_indices);
//
//    for (glm::vec3& coord : _obj_info._meshes[0].getCoords()) {
//        coord = coord * 0.25f;
//    }
//
//    SkeletonDebug::createCrossMesh(_cross_mesh, 1.0f);
//    SkeletonDebug::createCrossMaterial(_cross_material, _cross_material_indices);
//
//    _right_hand.resize(21 + 5); // 21 landmarks + 5 wrists
//    _alignment_matrices.resize(21 + 5);
//    std::fill(
//        _alignment_matrices.begin(),
//        _alignment_matrices.end(),
//        glm::mat4(1.0)
//    );
//    _transform_matrices.resize(21 + 5);
//    std::fill(
//        _transform_matrices.begin(),
//        _transform_matrices.end(),
//        glm::mat4(1.0)
//    );
//
//    for (Object3D& obj : _right_hand) {
//        //obj.createFromObjectInfo(_obj_info);
//        obj.setMesh(&_cross_mesh);
//        obj.setMaterial(_cross_material, &_cross_material_indices);
//        obj.setRenderer(DefaultConf::renderer);
//        obj.addShader(&_debug_shader);
//        //obj.setScaleLocale(glm::vec3(0.25f));
//    }
//
//    //for (Object3D& w : _wrists) {
//    //    //w.createFromObjectInfo(_obj_info);
//    //    w.setMesh(&_cross_mesh);
//    //    w.setMaterial(_cross_material, &_cross_material_indices);
//    //    w.setRenderer(DefaultConf::renderer);
//    //    w.addShader(DefaultConf::graphic_shader);
//    //}
//
//    //for (uint32_t i = 1; i < _right_hand.size(); ++i) {
//    //    _right_hand[0].addChild(&_right_hand[i]);
//    //}
//
//    // one
//    //_right_hand[_wrist_index + 0].addChild(&_right_hand[1]);
//    //_right_hand[1].addChild(&_right_hand[2]);
//    //_right_hand[2].addChild(&_right_hand[3]);
//    //_right_hand[3].addChild(&_right_hand[4]);
//    // two
//    _right_hand[_wrist_index + 1].addChild(&_right_hand[5]);
//    _right_hand[5].addChild(&_right_hand[6]);
//    _right_hand[6].addChild(&_right_hand[7]);
//    _right_hand[7].addChild(&_right_hand[8]);
//    // three
//    _right_hand[_wrist_index + 2].addChild(&_right_hand[9]);
//    _right_hand[9].addChild(&_right_hand[10]);
//    _right_hand[10].addChild(&_right_hand[11]);
//    _right_hand[11].addChild(&_right_hand[12]);
//    // four
//    _right_hand[_wrist_index + 3].addChild(&_right_hand[13]);
//    _right_hand[13].addChild(&_right_hand[14]);
//    _right_hand[14].addChild(&_right_hand[15]);
//    _right_hand[15].addChild(&_right_hand[16]);
//    // five
//    _right_hand[_wrist_index + 4].addChild(&_right_hand[17]);
//    _right_hand[17].addChild(&_right_hand[18]);
//    _right_hand[18].addChild(&_right_hand[19]);
//    _right_hand[19].addChild(&_right_hand[20]);
//
//    for (int32_t i = 1; i <= 20; ++i) {
//        _right_hand[i].setPositionLocale(0, 1, 0);
//    }
//
//    _network_thread = std::thread(&MediapipeModule::networkProcess, this);
//
//    //_debug_shader.setPolygonMode(VK_POLYGON_MODE_LINE);
//    _debug_shader.setTopology(VK_PRIMITIVE_TOPOLOGY_LINE_LIST);
//    createShader();
//
//}
//
//void MediapipeModule::update() {
//    Module::update();
//
//    //std::cout << _right_hand[0].getPositionLocale().x << std::endl;
//
//    //for (int32_t i = 1; i < _transform_matrices.size(); ++i) {
//    //    _transform_matrices[i] = _right_hand[i].getTransform() * _alignment_matrices[i];
//    //}
//
//    _transform_matrices[_wrist_index + 0] = _right_hand[_wrist_index + 0].getTransform() * _alignment_matrices[_wrist_index + 0];
//    _transform_matrices[1] = _transform_matrices[_wrist_index + 0] * _right_hand[5].getTransformLocale() * _alignment_matrices[1];
//    _transform_matrices[2] = _transform_matrices[1] * _right_hand[2].getTransformLocale() * _alignment_matrices[2];
//    _transform_matrices[3] = _transform_matrices[2] * _right_hand[3].getTransformLocale() * _alignment_matrices[3];
//    _transform_matrices[4] = _transform_matrices[3] * _right_hand[4].getTransformLocale();
//
//    _transform_matrices[_wrist_index + 1] = _right_hand[_wrist_index + 1].getTransform() * _alignment_matrices[_wrist_index + 1];
//    _transform_matrices[5] = _transform_matrices[_wrist_index + 1] * _right_hand[5].getTransformLocale() * _alignment_matrices[5];
//    _transform_matrices[6] = _transform_matrices[5] * _right_hand[6].getTransformLocale() * _alignment_matrices[6];
//    _transform_matrices[7] = _transform_matrices[6] * _right_hand[7].getTransformLocale() * _alignment_matrices[7];
//    _transform_matrices[8] = _transform_matrices[7] * _right_hand[8].getTransformLocale();
//
//    _transform_matrices[_wrist_index + 2] = _right_hand[_wrist_index + 2].getTransform() * _alignment_matrices[_wrist_index + 2];
//    _transform_matrices[9] = _transform_matrices[_wrist_index + 2] * _right_hand[9].getTransformLocale() * _alignment_matrices[9];
//    _transform_matrices[10] = _transform_matrices[9] * _right_hand[10].getTransformLocale() * _alignment_matrices[10];
//    _transform_matrices[11] = _transform_matrices[10] * _right_hand[11].getTransformLocale() * _alignment_matrices[11];
//    _transform_matrices[12] = _transform_matrices[11] * _right_hand[12].getTransformLocale();
//
//    _transform_matrices[_wrist_index + 3] = _right_hand[_wrist_index + 3].getTransform() * _alignment_matrices[_wrist_index + 3];
//    _transform_matrices[13] = _transform_matrices[_wrist_index + 3] * _right_hand[13].getTransformLocale() * _alignment_matrices[13];
//    _transform_matrices[14] = _transform_matrices[13] * _right_hand[14].getTransformLocale() * _alignment_matrices[14];
//    _transform_matrices[15] = _transform_matrices[14] * _right_hand[15].getTransformLocale() * _alignment_matrices[15];
//    _transform_matrices[16] = _transform_matrices[15] * _right_hand[16].getTransformLocale();
//
//    _transform_matrices[_wrist_index + 4] = _right_hand[_wrist_index + 4].getTransform() * _alignment_matrices[_wrist_index + 4];
//    _transform_matrices[17] = _transform_matrices[_wrist_index + 4] * _right_hand[17].getTransformLocale() * _alignment_matrices[17];
//    _transform_matrices[18] = _transform_matrices[17] * _right_hand[18].getTransformLocale() * _alignment_matrices[18];
//    _transform_matrices[19] = _transform_matrices[18] * _right_hand[19].getTransformLocale() * _alignment_matrices[19];
//    _transform_matrices[20] = _transform_matrices[19] * _right_hand[20].getTransformLocale();
//
//
//    _obj_tr_buffer.setValues(
//        _transform_matrices.data()
//    );
//
//    _camera = DefaultConf::camera->getMVPC();
//}
//
//void MediapipeModule::close() {
//    Module::close();
//
//    _network_thread.join();
//
//    _mediapipe_stream.Close();
//    _server_socket.Close();
//
//}
//
//void MediapipeModule::networkProcess() {
//    for (;;) {
//        // Asking for new landmarks
//        mdp::OkMsg ok_msg;
//        ok_msg._is_ok = true;
//        int32_t is_sent = _mediapipe_stream.Send(reinterpret_cast<char*>(&ok_msg), sizeof(ok_msg));
//        if (is_sent < sizeof(ok_msg)) {
//            break;
//        }
//
//        // Receiving new landmarks
//        mdp::Landmarks landmarks;
//        int32_t is_recv = _mediapipe_stream.Recv(sizeof(landmarks), reinterpret_cast<char*>(&landmarks));
//        if (is_recv < sizeof(landmarks)) {
//            break;
//        }
//
//        //for (uint32_t i = 0; i < _right_hand.size(); ++i) {
//        //    _right_hand[i].setPositionLocale(landmarks._landmarks[0][i] * 10.0f);
//        //}
//         
//
//        //landmarksToLocal(landmarks);
//
//
//        //landmarksToHand(landmarks);
//
//        landmarksRotationMatrices(landmarks);
//    }
//}
//
//void MediapipeModule::landmarksRotationMatrices(const mdp::Landmarks& landmarks) {
//    // Let us work with the index finger
//    // Let us consider that the position of a child, is (0, 1, 0) to the father
//
//    {
//        // --- wrist -> thumb ---
//        // Mediapipe vector
//        glm::vec3 v_m = landmarks._landmarks[0][1] - landmarks._landmarks[0][0];
//        // Skinning vector
//        glm::vec3 v_s = glm::vec3(0, 1, 0);
//        // Find the rotation matrix to go from Skinning vector to Mediapipe vector
//        _alignment_matrices[_wrist_index + 0] = findRotationMatrix(v_s, v_m);
//    }
//    {
//        glm::vec3 v_m = landmarks._landmarks[0][2] - landmarks._landmarks[0][1];
//        glm::vec3 v_s = _alignment_matrices[_wrist_index + 0] * glm::vec4(glm::vec3(0, 1, 0), 1.0f);
//        _alignment_matrices[1] = findRotationMatrix(v_s, v_m);
//    }
//    {
//        glm::vec3 v_m = landmarks._landmarks[0][3] - landmarks._landmarks[0][2];
//        glm::vec3 v_s = _alignment_matrices[1] * _alignment_matrices[_wrist_index + 0] * glm::vec4(glm::vec3(0, 1, 0), 1.0);
//        _alignment_matrices[2] = findRotationMatrix(v_s, v_m);
//    }
//    {
//        glm::vec3 v_m = landmarks._landmarks[0][4] - landmarks._landmarks[0][3];
//        glm::vec3 v_s = _alignment_matrices[2] * _alignment_matrices[1] * _alignment_matrices[_wrist_index + 0] * glm::vec4(glm::vec3(0, 1, 0), 1.0);
//        _alignment_matrices[3] = findRotationMatrix(v_s, v_m);
//    }
//
//    {
//        // --- wrist -> index ---
//        // Mediapipe vector
//        glm::vec3 v_m = landmarks._landmarks[0][5] - landmarks._landmarks[0][0];
//        // Skinning vector
//        glm::vec3 v_s = glm::vec3(0, 1, 0);
//        // Find the rotation matrix to go from Skinning vector to Mediapipe vector
//        _alignment_matrices[_wrist_index + 1] = findRotationMatrix(v_s, v_m);
//    }
//    {
//        glm::vec3 v_m = landmarks._landmarks[0][6] - landmarks._landmarks[0][5];
//        glm::vec3 v_s = _alignment_matrices[_wrist_index + 1] * glm::vec4(glm::vec3(0, 1, 0), 1.0f);
//        _alignment_matrices[5] = findRotationMatrix(v_s, v_m);
//    }
//    {
//        glm::vec3 v_m = landmarks._landmarks[0][7] - landmarks._landmarks[0][6];
//        glm::vec3 v_s = _alignment_matrices[5] * _alignment_matrices[_wrist_index + 1] * glm::vec4(glm::vec3(0, 1, 0), 1.0);
//        _alignment_matrices[6] = findRotationMatrix(v_s, v_m);
//    }
//    {
//        glm::vec3 v_m = landmarks._landmarks[0][8] - landmarks._landmarks[0][7];
//        glm::vec3 v_s = _alignment_matrices[6] * _alignment_matrices[5] * _alignment_matrices[_wrist_index + 1] * glm::vec4(glm::vec3(0, 1, 0), 1.0);
//        _alignment_matrices[7] = findRotationMatrix(v_s, v_m);
//    }
//
//    
//    {
//        // --- wrist -> middle finger ---
//        glm::vec3 v_m = landmarks._landmarks[0][9] - landmarks._landmarks[0][0];
//        glm::vec3 v_s = glm::vec3(0, 1, 0);
//        _alignment_matrices[_wrist_index + 2] = findRotationMatrix(v_s, v_m);
//    }
//    {
//        glm::vec3 v_m = landmarks._landmarks[0][10] - landmarks._landmarks[0][9];
//        glm::vec3 v_s = _alignment_matrices[_wrist_index + 2] * glm::vec4(glm::vec3(0, 1, 0), 1.0f);
//        _alignment_matrices[9] = findRotationMatrix(v_s, v_m);
//    }
//    {
//        glm::vec3 v_m = landmarks._landmarks[0][11] - landmarks._landmarks[0][10];
//        glm::vec3 v_s = _alignment_matrices[9] * _alignment_matrices[_wrist_index + 2] * glm::vec4(glm::vec3(0, 1, 0), 1.0);
//        _alignment_matrices[10] = findRotationMatrix(v_s, v_m);
//    }
//    {
//        glm::vec3 v_m = landmarks._landmarks[0][12] - landmarks._landmarks[0][11];
//        glm::vec3 v_s = _alignment_matrices[10] * _alignment_matrices[9] * _alignment_matrices[_wrist_index + 2] * glm::vec4(glm::vec3(0, 1, 0), 1.0);
//        _alignment_matrices[11] = findRotationMatrix(v_s, v_m);
//    }
//
//    {
//        // --- wrist -> ring finger ---
//        glm::vec3 v_m = landmarks._landmarks[0][13] - landmarks._landmarks[0][0];
//        glm::vec3 v_s = glm::vec3(0, 1, 0);
//        _alignment_matrices[_wrist_index + 3] = findRotationMatrix(v_s, v_m);
//    }
//    {
//        glm::vec3 v_m = landmarks._landmarks[0][14] - landmarks._landmarks[0][13];
//        glm::vec3 v_s = _alignment_matrices[_wrist_index + 3] * glm::vec4(glm::vec3(0, 1, 0), 1.0f);
//        _alignment_matrices[13] = findRotationMatrix(v_s, v_m);
//    }
//    {
//        glm::vec3 v_m = landmarks._landmarks[0][15] - landmarks._landmarks[0][14];
//        glm::vec3 v_s = _alignment_matrices[13] * _alignment_matrices[_wrist_index + 3] * glm::vec4(glm::vec3(0, 1, 0), 1.0);
//        _alignment_matrices[14] = findRotationMatrix(v_s, v_m);
//    }
//    {
//        glm::vec3 v_m = landmarks._landmarks[0][16] - landmarks._landmarks[0][15];
//        glm::vec3 v_s = _alignment_matrices[14] * _alignment_matrices[13] * _alignment_matrices[_wrist_index + 3] * glm::vec4(glm::vec3(0, 1, 0), 1.0);
//        _alignment_matrices[15] = findRotationMatrix(v_s, v_m);
//    }
//
//    {
//        // --- wrist -> pinky ---
//        glm::vec3 v_m = landmarks._landmarks[0][17] - landmarks._landmarks[0][0];
//        glm::vec3 v_s = glm::vec3(0, 1, 0);
//        _alignment_matrices[_wrist_index + 4] = findRotationMatrix(v_s, v_m);
//    }
//    {
//        glm::vec3 v_m = landmarks._landmarks[0][18] - landmarks._landmarks[0][17];
//        glm::vec3 v_s = _alignment_matrices[_wrist_index + 4] * glm::vec4(glm::vec3(0, 1, 0), 1.0f);
//        _alignment_matrices[17] = findRotationMatrix(v_s, v_m);
//    }
//    {
//        glm::vec3 v_m = landmarks._landmarks[0][19] - landmarks._landmarks[0][18];
//        glm::vec3 v_s = _alignment_matrices[17] * _alignment_matrices[_wrist_index + 4] * glm::vec4(glm::vec3(0, 1, 0), 1.0);
//        _alignment_matrices[18] = findRotationMatrix(v_s, v_m);
//    }
//    {
//        glm::vec3 v_m = landmarks._landmarks[0][20] - landmarks._landmarks[0][19];
//        glm::vec3 v_s = _alignment_matrices[18] * _alignment_matrices[17] * _alignment_matrices[_wrist_index + 4] * glm::vec4(glm::vec3(0, 1, 0), 1.0);
//        _alignment_matrices[19] = findRotationMatrix(v_s, v_m);
//    }
//}
//
//glm::mat4 MediapipeModule::findRotationMatrix(
//    const glm::vec3& from,
//    const glm::vec3& to
//) {
//    glm::mat4 rot = alignVectorMatrix(
//        glm::normalize(from),
//        glm::normalize(to)
//    );
//    glm::mat4 res = rot;
//    res[3][3] = 1.0f;
//    return rot;
//}
//
//
//glm::mat3 MediapipeModule::alignVectorMatrix(
//    const glm::vec3& from,
//    const glm::vec3& to
//) {
//    glm::vec3 refl = glm::vec3(0, 0, 1);
//    if (std::abs(from.x) < 0.72f && std::abs(to.x) < 0.72f) {
//        refl = glm::vec3(1, 0, 0);
//    }
//    else if (std::abs(from.y) < 0.72 && std::abs(to.y) < 0.72) {
//        refl = glm::vec3(0, 1, 0);
//    }
//
//    glm::vec3 u = refl - from;
//    glm::vec3 v = refl - to;
//    glm::mat3 r = glm::mat3(1);
//    for (int32_t i = 0; i < 3; ++i) {
//        for (int32_t j = 0; j < 3; ++j) {
//            r[i][j] = ((i == j) ? 1 : 0) -
//                2 / glm::dot(u, u) * u[i] * u[j] -
//                2 / glm::dot(v, v) * v[i] * v[j] +
//                4 * glm::dot(u, v) / (glm::dot(u, u) * glm::dot(v, v)) * v[i] * u[j];
//        }
//    }
//    
//    return glm::transpose(r);
//}
//
//void MediapipeModule::landmarksToHand(const mdp::Landmarks& landmarks) {
//    glm::vec3 avg = (
//        landmarks._landmarks[0][5] +
//        landmarks._landmarks[0][9] +
//        landmarks._landmarks[0][13] +
//        landmarks._landmarks[0][17]
//        ) / 4.0f;
//
//    // Wrist
//
//    // Thumb
//    wristPose(
//        _wrists[0],
//        landmarks._landmarks[0][0] + avg,
//        landmarks._landmarks[0][0],
//        landmarks._landmarks[0][1]
//    );
//    // 1
//    jointPose(
//        _right_hand[1],
//        landmarks._landmarks[0][0],
//        landmarks._landmarks[0][1],
//        landmarks._landmarks[0][2]
//    );
//
//    // 2
//    jointPose(
//        _right_hand[2],
//        landmarks._landmarks[0][1],
//        landmarks._landmarks[0][2],
//        landmarks._landmarks[0][3]
//    );
//    // 3
//    jointPose(
//        _right_hand[3],
//        landmarks._landmarks[0][2],
//        landmarks._landmarks[0][3],
//        landmarks._landmarks[0][4]
//    );
//
//    // --- index finger ---
//    wristPose(
//        _wrists[1],
//        landmarks._landmarks[0][0] + avg,
//        landmarks._landmarks[0][0],
//        landmarks._landmarks[0][5]
//    );
//
//    // 5
//    jointPose(
//        _right_hand[5],
//        landmarks._landmarks[0][0],
//        landmarks._landmarks[0][5],
//        landmarks._landmarks[0][6]
//    );
//    // 6
//    jointPose(
//        _right_hand[6],
//        landmarks._landmarks[0][5],
//        landmarks._landmarks[0][6],
//        landmarks._landmarks[0][7]
//    );
//    // 7
//    jointPose(
//        _right_hand[7],
//        landmarks._landmarks[0][6],
//        landmarks._landmarks[0][7],
//        landmarks._landmarks[0][8]
//    );
//    //// 8
//    //// Should not be done
//
//
//    // Middle finger
//    wristPose(
//        _wrists[2],
//        landmarks._landmarks[0][0] + avg,
//        landmarks._landmarks[0][0],
//        landmarks._landmarks[0][9]
//    );
//    // 9
//    jointPose(
//        _right_hand[9],
//        landmarks._landmarks[0][0],
//        landmarks._landmarks[0][9],
//        landmarks._landmarks[0][10]
//    );
//    std::cout << makeString(_wrists[2].getRotationLocale()) << std::endl;;
//
//    // 10
//    jointPose(
//        _right_hand[10],
//        landmarks._landmarks[0][9],
//        landmarks._landmarks[0][10],
//        landmarks._landmarks[0][11]
//    );
//    // 11
//    jointPose(
//        _right_hand[11],
//        landmarks._landmarks[0][10],
//        landmarks._landmarks[0][11],
//        landmarks._landmarks[0][12]
//    );
//
//    // Ring finger
//    wristPose(
//        _wrists[3],
//        landmarks._landmarks[0][0] + avg,
//        landmarks._landmarks[0][0],
//        landmarks._landmarks[0][13]
//    );
//    // 13
//    jointPose(
//        _right_hand[13],
//        landmarks._landmarks[0][0],
//        landmarks._landmarks[0][13],
//        landmarks._landmarks[0][14]
//    );
//    // 14
//    jointPose(
//        _right_hand[14],
//        landmarks._landmarks[0][13],
//        landmarks._landmarks[0][14],
//        landmarks._landmarks[0][15]
//    );
//    // 15
//    jointPose(
//        _right_hand[15],
//        landmarks._landmarks[0][14],
//        landmarks._landmarks[0][15],
//        landmarks._landmarks[0][16]
//    );
//
//    // Little finger
//    wristPose(
//        _wrists[4],
//        landmarks._landmarks[0][0] + avg,
//        landmarks._landmarks[0][0],
//        landmarks._landmarks[0][17]
//    );
//    // 17
//    jointPose(
//        _right_hand[17],
//        landmarks._landmarks[0][0],
//        landmarks._landmarks[0][17],
//        landmarks._landmarks[0][18]
//    );
//    // 18
//    jointPose(
//        _right_hand[18],
//        landmarks._landmarks[0][17],
//        landmarks._landmarks[0][18],
//        landmarks._landmarks[0][19]
//    );
//    // 19
//    jointPose(
//        _right_hand[19],
//        landmarks._landmarks[0][18],
//        landmarks._landmarks[0][19],
//        landmarks._landmarks[0][20]
//    );
//}
//
//void MediapipeModule::wristPose(
//    Entity3D& wrist,
//    const glm::vec3& point1,
//    const glm::vec3& point2,
//    const glm::vec3& point3
//) {
//    glm::vec3 A = point2 - point1;
//    glm::vec3 B = point3 - point2;
//
//    glm::vec3 angles = vectorAngle(A, B);
//    float dist = glm::l2Norm(A);
//
//    wrist.setPositionLocale(-point2 * 5.0f);
//    wrist.setRotationLocale(angles);
//}
//
//void MediapipeModule::jointPose(
//    Entity3D& joint,
//    const glm::vec3& point1,
//    const glm::vec3& point2,
//    const glm::vec3& point3
//) {
//    glm::vec3 A = point2 - point1;
//    glm::vec3 B = point3 - point2;
//
//    glm::vec3 angles = vectorAngle(A, B);
//    float dist = glm::l2Norm(A);
//
//    joint.setPositionLocale(glm::vec3(0, 1, 0) * dist * 10.0f);
//    joint.setRotationLocale(angles);
//}
//
//glm::vec3 MediapipeModule::vectorAngle(glm::vec3 A, glm::vec3 B) {
//    float angle_z = 0;
//    if (A.x * B.x + A.y * B.y > 0 || A.x * B.x + A.y * B.y < 0) {
//        angle_z = std::acos(
//            (A.x * B.x + A.y * B.y)
//            /
//            (std::sqrt(A.x * A.x + A.y * A.y) * std::sqrt(B.x * B.x + B.y * B.y))
//        );
//    }
//
//    float angle_y = 0;
//    if (A.x * B.x + A.z * B.z > 0 || A.x * B.x + A.z * B.z < 0) {
//        angle_y = std::acos(
//            (A.x * B.x + A.z * B.z)
//            /
//            (std::sqrt(A.x * A.x + A.z * A.z) * std::sqrt(B.x * B.x + B.z * B.z))
//        );
//    }
//
//    float angle_x = 0;
//    if (A.y * B.y + A.z * B.z > 0 || A.y * B.y + A.z * B.z < 0) {
//        angle_x = std::acos(
//            (A.y * B.y + A.z * B.z)
//            /
//            (std::sqrt(A.y * A.y + A.z * A.z) * std::sqrt(B.y * B.y + B.z * B.z))
//        );
//    }
//    return glm::vec3(angle_x, angle_y, angle_z);
//}
//
//
//void MediapipeModule::landmarksToLocal(const mdp::Landmarks& landmarks) {
//    const float offset_size = 20.0f;
//    _right_hand[0].setPositionLocale(landmarks._landmarks[0][0] * 10.f);
//    // One
//    _right_hand[1].setPositionLocale(
//        (landmarks._landmarks[0][1] - landmarks._landmarks[0][0]) * offset_size
//    );
//    _right_hand[2].setPositionLocale(
//        (landmarks._landmarks[0][2] - landmarks._landmarks[0][1]) * offset_size
//    );
//    _right_hand[3].setPositionLocale(
//        (landmarks._landmarks[0][3] - landmarks._landmarks[0][2]) * offset_size
//    );
//    _right_hand[4].setPositionLocale(
//        (landmarks._landmarks[0][4] - landmarks._landmarks[0][3]) * offset_size
//    );
//    // two
//    _right_hand[5].setPositionLocale(
//        (landmarks._landmarks[0][5] - landmarks._landmarks[0][0]) * offset_size
//    );
//    _right_hand[6].setPositionLocale(
//        (landmarks._landmarks[0][6] - landmarks._landmarks[0][5]) * offset_size
//    );
//    _right_hand[7].setPositionLocale(
//        (landmarks._landmarks[0][7] - landmarks._landmarks[0][6]) * offset_size
//    );
//    _right_hand[7].setPositionLocale(
//        (landmarks._landmarks[0][8] - landmarks._landmarks[0][7]) * offset_size
//    );
//    // three
//    _right_hand[9].setPositionLocale(
//        (landmarks._landmarks[0][9] - landmarks._landmarks[0][0]) * offset_size
//    );
//    _right_hand[10].setPositionLocale(
//        (landmarks._landmarks[0][10] - landmarks._landmarks[0][9]) * offset_size
//    );
//    _right_hand[11].setPositionLocale(
//        (landmarks._landmarks[0][11] - landmarks._landmarks[0][10]) * offset_size
//    );
//    _right_hand[12].setPositionLocale(
//        (landmarks._landmarks[0][12] - landmarks._landmarks[0][11]) * offset_size
//    );
//    // four
//    _right_hand[13].setPositionLocale(
//        (landmarks._landmarks[0][13] - landmarks._landmarks[0][0]) * offset_size
//    );
//    _right_hand[14].setPositionLocale(
//        (landmarks._landmarks[0][14] - landmarks._landmarks[0][13]) * offset_size
//    );
//    _right_hand[15].setPositionLocale(
//        (landmarks._landmarks[0][15] - landmarks._landmarks[0][14]) * offset_size
//    );
//    _right_hand[16].setPositionLocale(
//        (landmarks._landmarks[0][16] - landmarks._landmarks[0][15]) * offset_size
//    );
//    // five
//    _right_hand[17].setPositionLocale(
//        (landmarks._landmarks[0][17] - landmarks._landmarks[0][0]) * offset_size
//    );
//    _right_hand[18].setPositionLocale(
//        (landmarks._landmarks[0][18] - landmarks._landmarks[0][17]) * offset_size
//    );
//    _right_hand[19].setPositionLocale(
//        (landmarks._landmarks[0][19] - landmarks._landmarks[0][18]) * offset_size
//    );
//    _right_hand[20].setPositionLocale(
//        (landmarks._landmarks[0][20] - landmarks._landmarks[0][19]) * offset_size
//    );
//}
//
//std::vector<glm::vec3> MediapipeModule::cube() {
//    std::vector<glm::vec3> cube = {
// 	    // Front
// 	    glm::vec3(-1, -1, -1),
// 	    glm::vec3(1, -1, -1),
// 	    glm::vec3(1, 1, -1),
// 	    glm::vec3(-1, 1, -1),
// 	    // Right
// 	    glm::vec3(1, -1, -1),
// 	    glm::vec3(1, -1, 1),
// 	    glm::vec3(1, 1, 1),
// 	    glm::vec3(1, 1, -1),
// 	    // Back
// 	    glm::vec3(1, -1, 1),
// 	    glm::vec3(1, 1, 1),
// 	    glm::vec3(-1, 1, 1),
// 	    glm::vec3(-1, -1, 1),
// 	    // Left
// 	    glm::vec3(-1, -1, 1),
// 	    glm::vec3(-1, -1, -1),
// 	    glm::vec3(-1, 1, -1),
// 	    glm::vec3(-1, 1, 1),
// 	    // Top
// 	    glm::vec3(-1, 1, -1),
// 	    glm::vec3(1, 1, -1),
// 	    glm::vec3(1, 1, 1),
// 	    glm::vec3(-1, 1, 1),
// 	    // Bottom
// 	    glm::vec3(-1, -1, 1),
// 	    glm::vec3(1, -1, 1),
// 	    glm::vec3(1, -1, -1),
// 	    glm::vec3(-1, -1, -1)
//    };
//    return cube;
//}
//
//std::vector<uint32_t> MediapipeModule::cubeIndex() {
//    std::vector<uint32_t> index{};
//
//    for (uint32_t i = 0; i < 6; i++) {
// 	    index.push_back(0 + i * 4);
// 	    index.push_back(1 + i * 4);
// 	    index.push_back(3 + i * 4);
// 	    index.push_back(1 + i * 4);
// 	    index.push_back(2 + i * 4);
// 	    index.push_back(3 + i * 4);
//    }
//
//    return index;
//}
//
//void MediapipeModule::createShader() {
//    _debug_shader.setNbFrame(DefaultConf::NB_FRAME);
//    _debug_shader.setLogicalDevice(DefaultConf::logical_device);
//    _debug_shader.read(
//        "../../shaders/debug/debug.vert",
//        "../../shaders/debug/debug.frag"
//    );
//    // -- Vertex Shader -- //
//    // Configuring the Graphic Shader
//    _debug_shader.addVertexBufferInfo(
//        "coord",
//        Object3D::getCoordsStride(DefaultConf::renderer, &_debug_shader),
//        VK_FORMAT_R32G32B32_SFLOAT,
//        0
//    );
//    _debug_shader.addVertexBufferInfo(
//        "object_id",
//        Object3D::getMeshOffsetsStride(DefaultConf::renderer, &_debug_shader),
//        VK_FORMAT_R32_UINT,
//        1
//    );
//    _debug_shader.addVertexBufferInfo(
//        "material_id",
//        Object3D::getMaterialIndicesStride(DefaultConf::renderer, &_debug_shader),
//        VK_FORMAT_R32_UINT,
//        2
//    );
//
//    // Index Buffer
//    _debug_shader.addIndexBufferInfo(
//        "index",
//        Object3D::getIndicesNbElem(DefaultConf::renderer, &_debug_shader)
//    );
//
//    // Push constant
//    _debug_shader.addPushConstantInfo(
//        "pc",
//        VK_SHADER_STAGE_VERTEX_BIT,
//        0,
//        sizeof(DefaultConf::camera->getMVPC())
//    );
//
//    // Storage Buffers
//// transform
//    _debug_shader.addStorageBufferInfo(
//        "obj_tr",
//        1,
//        VK_SHADER_STAGE_VERTEX_BIT
//    );
//
//    // -- Fragment shader -- //
//    // materials
//    _debug_shader.addStorageBufferInfo(
//        "MaterialsBuffer",
//        2,
//        VK_SHADER_STAGE_FRAGMENT_BIT
//    );
//
//    //----------------------//
//    // Creating the buffers //
//    //----------------------//
//    // Coord
//    // Mesh offsets
//    _coord_buffer.setQueue(DefaultConf::graphics_queue);
//    _coord_buffer.setSize(Object3D::getCoordsSize(DefaultConf::renderer, &_debug_shader));
//    _coord_buffer.create();
//    _coord_buffer.setValues(Object3D::getCoords(DefaultConf::renderer, &_debug_shader).data());
//    _debug_shader.addVertexBuffer("coord", &_coord_buffer);
//
//    _object_id_buffer.setQueue(DefaultConf::graphics_queue);
//    _object_id_buffer.setSize(Object3D::getObjectIdSize(DefaultConf::renderer, &_debug_shader));
//    _object_id_buffer.create();
//    _object_id_buffer.setValues(Object3D::getObjectId(DefaultConf::renderer, &_debug_shader).data());
//    _debug_shader.addVertexBuffer("object_id", &_object_id_buffer);
//
//    _material_indices_buffer.setQueue(DefaultConf::graphics_queue);
//    _material_indices_buffer.setSize(Object3D::getMaterialIndicesSize(DefaultConf::renderer, &_debug_shader));
//    _material_indices_buffer.create();
//    _material_indices_buffer.setValues(Object3D::getMaterialIndices(DefaultConf::renderer, &_debug_shader).data());
//    _debug_shader.addVertexBuffer("material_id", &_material_indices_buffer);
//
//
//    // Index buffer
//    _index_buffer.setQueue(DefaultConf::graphics_queue);
//    _index_buffer.setSize(Object3D::getIndicesSize(DefaultConf::renderer, &_debug_shader));
//    _index_buffer.create();
//    _index_buffer.setValues(Object3D::getIndices(DefaultConf::renderer, &_debug_shader).data());
//    _debug_shader.addIndexBuffer("index", &_index_buffer);
//
//    // Push Constant
//    _camera = DefaultConf::camera->getMVPC();
//    _debug_shader.addPushConstant("pc", &_camera[0][0]);
//
//
//    // Storage Buffers
//    // transform
//    _obj_tr_buffer.setQueue(DefaultConf::graphics_queue);;
//    _obj_tr_buffer.setSize(
//        _transform_matrices.size() * sizeof(glm::mat4)
//    );
//    _obj_tr_buffer.create();
//    _obj_tr_buffer.setValues(_transform_matrices.data());
//    _debug_shader.addStorageBuffer("obj_tr", &_obj_tr_buffer);
//
//    // materials
//    _materials_buffer.setQueue(DefaultConf::graphics_queue);
//    _materials_buffer.setSize(Object3D::getMaterialsSize(DefaultConf::renderer, &_debug_shader));
//    _materials_buffer.create();
//    _materials_buffer.setValues(Object3D::getMaterials(DefaultConf::renderer, &_debug_shader).data());
//    _debug_shader.addStorageBuffer("MaterialsBuffer", &_materials_buffer);
//}