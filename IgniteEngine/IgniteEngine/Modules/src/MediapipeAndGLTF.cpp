#include "MediapipeAndGLTF.h"

MediapipeAndGLTF::MediapipeAndGLTF() : Module::Module() {
    ;
}

void MediapipeAndGLTF::init() {
    Module::init();

    {
        glm::vec3 v_s = glm::vec3(0, 1, 0);
        glm::vec3 v_m = glm::vec3(1, 1, 1);
        glm::mat4 alignment_m = findRotationMatrix(v_s, v_m);
        const glm::vec4 v_aligned = alignment_m * glm::vec4(glm::normalize(v_s), 1.0);
        std::cout << makeString(glm::normalize(v_s)) << " -- ";
        std::cout << makeString(glm::normalize(v_m)) << " -- ";
        std::cout << makeString(glm::normalize(glm::vec3(v_aligned))) << " -- ";
        std::cout << makeString(glm::vec4(glm::normalize(v_m), 1.0) - v_aligned) << std::endl;
    }

    _server_socket.Port() = SERVER_PORT;
    _server_socket.Address() = SERVER_ADDRESS;
    _server_socket.Initialise();
    _server_socket.Bind();
    _server_socket.Listen();

    std::cout << "Listening on " << SERVER_ADDRESS << " -- " << SERVER_PORT << std::endl;
    std::cout << "Waiting for mediapipe client to connect...";
    _mediapipe_stream = _server_socket.Accept();
    std::cout << "connected!" << std::endl;

    mdph::DimMsg dim_msg;
    _mediapipe_stream.Recv(sizeof(dim_msg), reinterpret_cast<char*>(&dim_msg));
    std::cout << "received: " << std::endl;
    std::cout << "    width: " << dim_msg._width << std::endl;
    std::cout << "    height: " << dim_msg._height << std::endl;

    DefaultConf::render_window_width = dim_msg._width * 2;
    DefaultConf::render_window_height = dim_msg._height * 2;
}

void MediapipeAndGLTF::start() {
    Module::start();

    _hand_obj_info.loadGLTF("../../assets/3d_objects/gltf/Hand/hand.gltf");
    _hand.createFromObjectInfo(_hand_obj_info);
    _hand.setRenderer(DefaultConf::renderer);
    _hand.addShader(&_lbs_shader);

    //SkeletonDebug::createCrossMesh(_cross_mesh, 0.1);
    //SkeletonDebug::createCrossMaterial(_cross_material, _cross_material_indices);

    _hand.setScaleLocale(glm::vec3(0.25f));

    SkeletonDebug::createCrossMesh(_cross_mesh, 1.0f);
    SkeletonDebug::createCrossMaterial(_cross_material, _cross_material_indices);

    _right_hand.resize(_hand.getSkeleton()->joints().size());
    _alignment_matrices.resize(_right_hand.size());
    std::fill(
        _alignment_matrices.begin(),
        _alignment_matrices.end(),
        glm::mat4(1.0)
    );
    _transform_matrices.resize(_right_hand.size());
    std::fill(
        _transform_matrices.begin(),
        _transform_matrices.end(),
        glm::mat4(1.0)
    );

    std::cout << *_hand.getSkeleton() << std::endl;

    _network_thread = std::thread(&MediapipeAndGLTF::networkProcess, this);


    _lbs_shader.setPolygonMode(VK_POLYGON_MODE_LINE);
    createShaderHand();

}

void MediapipeAndGLTF::update() {
    Module::update();

    const Joint* wrist = _hand.getSkeleton()->skeleton();
    glm::mat4 wrist_tr = wrist->getTransformLocale() * _alignment_matrices[wrist->id()];
    _transform_matrices[wrist->id()] = wrist_tr * wrist->inverseBindMatrices();

    for (int i = 0; i < 5; ++i) {
        // -- Thumb --
        const Joint* fing0 = static_cast<Joint*>(wrist->getChildren()[i]);
        const Joint* fing1 = static_cast<Joint*>(fing0->getChildren()[0]);
        const Joint* fing2 = static_cast<Joint*>(fing1->getChildren()[0]);
        const Joint* fing3 = static_cast<Joint*>(fing2->getChildren()[0]);

        glm::mat4 tr = wrist_tr * fing0->getTransformLocale() * _alignment_matrices[fing0->id()];
        _transform_matrices[fing0->id()] = tr * fing0->inverseBindMatrices();

        tr = tr * fing1->getTransformLocale() * _alignment_matrices[fing1->id()];
        _transform_matrices[fing1->id()] = tr * fing1->inverseBindMatrices();

        tr = tr * fing2->getTransformLocale() * _alignment_matrices[fing2->id()];
        _transform_matrices[fing2->id()] = tr * fing2->inverseBindMatrices();

        tr = tr * fing3->getTransformLocale() * _alignment_matrices[fing3->id()];
        _transform_matrices[fing3->id()] = tr * fing3->inverseBindMatrices();
    }

    //{// -- Index --
    //    const Joint* fing0 = static_cast<Joint*>(wrist->getChildren()[1]);
    //    const Joint* fing1 = static_cast<Joint*>(fing0->getChildren()[0]);
    //    const Joint* fing2 = static_cast<Joint*>(fing1->getChildren()[0]);
    //    const Joint* fing3 = static_cast<Joint*>(fing2->getChildren()[0]);

    //    glm::mat4 tr = wrist_tr * fing0->getTransformLocale() * _alignment_matrices[fing0->id()];
    //    _transform_matrices[fing0->id()] = tr * fing0->inverseBindMatrices();

    //    tr = tr * fing1->getTransformLocale() * _alignment_matrices[fing1->id()];
    //    _transform_matrices[fing1->id()] = tr * fing1->inverseBindMatrices();
    //    
    //    tr = tr * fing2->getTransformLocale() * _alignment_matrices[fing2->id()];
    //    _transform_matrices[fing2->id()] = tr * fing2->inverseBindMatrices();
    //    
    //    tr = tr * fing3->getTransformLocale() * _alignment_matrices[fing3->id()];
    //    _transform_matrices[fing3->id()] = tr * fing3->inverseBindMatrices();
    //}

    //{// -- Middle --
    //    const Joint* fing0 = static_cast<Joint*>(wrist->getChildren()[2]);
    //    const Joint* fing1 = static_cast<Joint*>(fing0->getChildren()[0]);
    //    const Joint* fing2 = static_cast<Joint*>(fing1->getChildren()[0]);
    //    const Joint* fing3 = static_cast<Joint*>(fing2->getChildren()[0]);

    //    glm::mat4 tr = wrist_tr * fing0->getTransformLocale() * _alignment_matrices[fing0->id()];
    //    _transform_matrices[fing0->id()] = tr * fing0->inverseBindMatrices();

    //    tr = tr * fing1->getTransformLocale() * _alignment_matrices[fing1->id()];
    //    _transform_matrices[fing1->id()] = tr * fing1->inverseBindMatrices();
    //    
    //    tr = tr * fing2->getTransformLocale() * _alignment_matrices[fing2->id()];
    //    _transform_matrices[fing2->id()] = tr * fing2->inverseBindMatrices();
    //    
    //    tr = tr * fing3->getTransformLocale() * _alignment_matrices[fing3->id()];
    //    _transform_matrices[fing3->id()] = tr * fing3->inverseBindMatrices();
    //}

    //{// -- Ring --
    //    const Joint* fing0 = static_cast<Joint*>(wrist->getChildren()[3]);
    //    const Joint* fing1 = static_cast<Joint*>(fing0->getChildren()[0]);
    //    const Joint* fing2 = static_cast<Joint*>(fing1->getChildren()[0]);
    //    const Joint* fing3 = static_cast<Joint*>(fing2->getChildren()[0]);

    //    glm::mat4 tr = wrist_tr * fing0->getTransformLocale() * _alignment_matrices[fing0->id()];
    //    _transform_matrices[fing0->id()] = tr * fing0->inverseBindMatrices();

    //    tr = tr * fing1->getTransformLocale() * _alignment_matrices[fing1->id()];
    //    _transform_matrices[fing1->id()] = tr * fing1->inverseBindMatrices();

    //    tr = tr * fing2->getTransformLocale() * _alignment_matrices[fing2->id()];
    //    _transform_matrices[fing2->id()] = tr * fing2->inverseBindMatrices();

    //    tr = tr * fing3->getTransformLocale() * _alignment_matrices[fing3->id()];
    //    _transform_matrices[fing3->id()] = tr * fing3->inverseBindMatrices();
    //}

    //{// -- Little --
    //    const Joint* fing0 = static_cast<Joint*>(wrist->getChildren()[4]);
    //    const Joint* fing1 = static_cast<Joint*>(fing0->getChildren()[0]);
    //    const Joint* fing2 = static_cast<Joint*>(fing1->getChildren()[0]);
    //    const Joint* fing3 = static_cast<Joint*>(fing2->getChildren()[0]);

    //    glm::mat4 tr = wrist_tr * fing0->getTransformLocale() * _alignment_matrices[fing0->id()];
    //    _transform_matrices[fing0->id()] = tr * fing0->inverseBindMatrices();

    //    tr = tr * fing1->getTransformLocale() * _alignment_matrices[fing1->id()];
    //    _transform_matrices[fing1->id()] = tr * fing1->inverseBindMatrices();

    //    tr = tr * fing2->getTransformLocale() * _alignment_matrices[fing2->id()];
    //    _transform_matrices[fing2->id()] = tr * fing2->inverseBindMatrices();

    //    tr = tr * fing3->getTransformLocale() * _alignment_matrices[fing3->id()];
    //    _transform_matrices[fing3->id()] = tr * fing3->inverseBindMatrices();
    //}

    _camera = DefaultConf::camera->getMVPC();

    _joint_tr_buffer_hand.setValues(
        _transform_matrices.data()
    );
}

void MediapipeAndGLTF::close() {
    Module::close();

    _network_thread.join();

    _mediapipe_stream.Close();
    _server_socket.Close();

}

void MediapipeAndGLTF::networkProcess() {
    for (;;) {
        // Asking for new landmarks
        mdph::OkMsg ok_msg;
        ok_msg._is_ok = true;
        int32_t is_sent = _mediapipe_stream.Send(reinterpret_cast<char*>(&ok_msg), sizeof(ok_msg));
        if (is_sent < sizeof(ok_msg)) {
            break;
        }

        // Receiving new landmarks
        mdph::Landmarks landmarks;
        int32_t is_recv = _mediapipe_stream.Recv(sizeof(landmarks), reinterpret_cast<char*>(&landmarks));
        if (is_recv < sizeof(landmarks)) {
            break;
        }

        //for (uint32_t i = 0; i < _right_hand.size(); ++i) {
        //    _right_hand[i].setPositionLocale(landmarks._landmarks[0][i] * 10.0f);
        //}


        //landmarksToLocal(landmarks);



        landmarksRotationMatrices(landmarks);
    }
}

void MediapipeAndGLTF::landmarksRotationMatrices(const mdph::Landmarks& landmarks) {
    // Let us work with the index finger
    // Let us consider that the position of a child, is (0, 1, 0) to the father
    {
        const Joint* wrist = _hand.getSkeleton()->skeleton();
        {
            // --- wrist -> thumb ---
            // Mediapipe vector
            glm::vec3 v_m = landmarks._landmarks[0][9] - landmarks._landmarks[0][0];
            // Skinning vector
            glm::vec3 v_s = wrist->getTransformLocale() * glm::vec4(0, 0, 0, 1);
            // Find the rotation matrix to go from Skinning vector to Mediapipe vector
            _alignment_matrices[wrist->id()] = findRotationMatrix(v_s, v_m);
        }
    }

    
    //{// -- THUMB --
    //    const Joint* wrist = _hand.getSkeleton()->skeleton();
    //    const Joint* fing0 = static_cast<Joint*>(wrist->getChildren()[0]);
    //    const Joint* fing1 = static_cast<Joint*>(fing0->getChildren()[0]);
    //    const Joint* fing2 = static_cast<Joint*>(fing1->getChildren()[0]);
    //    const Joint* fing3 = static_cast<Joint*>(fing2->getChildren()[0]);
    //    {
    //        // --- wrist -> thumb ---
    //        // Mediapipe vector
    //        glm::vec3 v_m = landmarks._landmarks[0][1] - landmarks._landmarks[0][0];
    //        // Skinning vector
    //        glm::vec3 v_s = _alignment_matrices[wrist->id()] * fing0->getTransformLocale() * glm::vec4(0, 0, 0, 1);
    //        // Find the rotation matrix to go from Skinning vector to Mediapipe vector
    //        _alignment_matrices[fing0->id()] = findRotationMatrix(v_s, v_m);
    //    }
    //    {
    //        glm::vec3 v_m = landmarks._landmarks[0][2] - landmarks._landmarks[0][1];
    //        glm::vec3 v_s = _alignment_matrices[fing0->id()] * _alignment_matrices[wrist->id()] * fing1->getTransformLocale() * glm::vec4(0, 0, 0, 1);
    //        _alignment_matrices[fing1->id()] = findRotationMatrix(v_s, v_m);
    //    }
    //    {
    //        glm::vec3 v_m = landmarks._landmarks[0][3] - landmarks._landmarks[0][2];
    //        glm::vec3 v_s = _alignment_matrices[fing1->id()] * _alignment_matrices[fing0->id()] * _alignment_matrices[wrist->id()] * fing2->getTransformLocale() * glm::vec4(0, 0, 0, 1);
    //        _alignment_matrices[fing2->id()] = findRotationMatrix(v_s, v_m);
    //    }
    //    {
    //        glm::vec3 v_m = landmarks._landmarks[0][4] - landmarks._landmarks[0][3];
    //        glm::vec3 v_s = _alignment_matrices[fing2->id()] * _alignment_matrices[fing1->id()] * _alignment_matrices[fing0->id()] * _alignment_matrices[wrist->id()] * fing3->getTransformLocale() * glm::vec4(0, 0, 0, 1);
    //        _alignment_matrices[fing3->id()] = findRotationMatrix(v_s, v_m);
    //    }
    //}

    //{// INDEX
    //    const Joint* wrist = _hand.getSkeleton()->skeleton();
    //    const Joint* fing0 = static_cast<Joint*>(wrist->getChildren()[1]);
    //    const Joint* fing1 = static_cast<Joint*>(fing0->getChildren()[0]);
    //    const Joint* fing2 = static_cast<Joint*>(fing1->getChildren()[0]);
    //    const Joint* fing3 = static_cast<Joint*>(fing2->getChildren()[0]);
    //    {
    //        // --- wrist -> index ---
    //        // Mediapipe vector
    //        glm::vec3 v_m = landmarks._landmarks[0][5] - landmarks._landmarks[0][0];
    //        // Skinning vector
    //        glm::vec3 v_s = _alignment_matrices[wrist->id()] * fing0->getTransformLocale() * glm::vec4(0, 0, 0, 1);
    //        // Find the rotation matrix to go from Skinning vector to Mediapipe vector
    //        _alignment_matrices[fing0->id()] = findRotationMatrix(v_s, v_m);
    //    }
    //    {
    //        glm::vec3 v_m = landmarks._landmarks[0][6] - landmarks._landmarks[0][5];
    //        glm::vec3 v_s = _alignment_matrices[fing0->id()] * _alignment_matrices[wrist->id()] * fing1->getTransformLocale() * glm::vec4(0, 0, 0, 1);
    //        _alignment_matrices[fing1->id()] = findRotationMatrix(v_s, v_m);
    //    }
    //    {
    //        glm::vec3 v_m = landmarks._landmarks[0][7] - landmarks._landmarks[0][6];
    //        glm::vec3 v_s = _alignment_matrices[fing1->id()] * _alignment_matrices[fing0->id()] * _alignment_matrices[wrist->id()] * fing2->getTransformLocale() * glm::vec4(0, 0, 0, 1);
    //        _alignment_matrices[fing2->id()] = findRotationMatrix(v_s, v_m);
    //    }
    //    {
    //        glm::vec3 v_m = landmarks._landmarks[0][8] - landmarks._landmarks[0][7];
    //        glm::vec3 v_s = _alignment_matrices[fing2->id()] * _alignment_matrices[fing1->id()] * _alignment_matrices[fing0->id()] * _alignment_matrices[wrist->id()] * fing3->getTransformLocale() * glm::vec4(0, 0, 0, 1);
    //        _alignment_matrices[fing3->id()] = findRotationMatrix(v_s, v_m);
    //    }
    //}

    //{// -- MIDDLE --
    //    const Joint* wrist = _hand.getSkeleton()->skeleton();
    //    const Joint* fing0 = static_cast<Joint*>(wrist->getChildren()[2]);
    //    const Joint* fing1 = static_cast<Joint*>(fing0->getChildren()[0]);
    //    const Joint* fing2 = static_cast<Joint*>(fing1->getChildren()[0]);
    //    const Joint* fing3 = static_cast<Joint*>(fing2->getChildren()[0]);
    //    {
    //        glm::vec3 v_m = landmarks._landmarks[0][9] - landmarks._landmarks[0][0];
    //        glm::vec3 v_s = _alignment_matrices[wrist->id()] * fing0->getTransformLocale() * glm::vec4(0, 0, 0, 1);
    //        _alignment_matrices[fing0->id()] = findRotationMatrix(v_s, v_m);
    //    }
    //    {
    //        glm::vec3 v_m = landmarks._landmarks[0][10] - landmarks._landmarks[0][9];
    //        glm::vec3 v_s = _alignment_matrices[fing0->id()] * _alignment_matrices[wrist->id()] * fing1->getTransformLocale() * glm::vec4(0, 0, 0, 1);
    //        _alignment_matrices[fing1->id()] = findRotationMatrix(v_s, v_m);
    //    }
    //    {
    //        glm::vec3 v_m = landmarks._landmarks[0][11] - landmarks._landmarks[0][10];
    //        glm::vec3 v_s = _alignment_matrices[fing1->id()] * _alignment_matrices[fing0->id()] * _alignment_matrices[wrist->id()] * fing2->getTransformLocale() * glm::vec4(0, 0, 0, 1);
    //        _alignment_matrices[fing2->id()] = findRotationMatrix(v_s, v_m);
    //    }
    //    {
    //        glm::vec3 v_m = landmarks._landmarks[0][12] - landmarks._landmarks[0][11];
    //        glm::vec3 v_s = _alignment_matrices[fing2->id()] * _alignment_matrices[fing1->id()] * _alignment_matrices[fing0->id()] * _alignment_matrices[wrist->id()] * fing3->getTransformLocale() * glm::vec4(0, 0, 0, 1);
    //        _alignment_matrices[fing3->id()] = findRotationMatrix(v_s, v_m);
    //    }
    //}

    //{// -- RING --
    //    const Joint* wrist = _hand.getSkeleton()->skeleton();
    //    const Joint* fing0 = static_cast<Joint*>(wrist->getChildren()[3]);
    //    const Joint* fing1 = static_cast<Joint*>(fing0->getChildren()[0]);
    //    const Joint* fing2 = static_cast<Joint*>(fing1->getChildren()[0]);
    //    const Joint* fing3 = static_cast<Joint*>(fing2->getChildren()[0]);
    //    {
    //        glm::vec3 v_m = landmarks._landmarks[0][13] - landmarks._landmarks[0][0];
    //        glm::vec3 v_s = _alignment_matrices[wrist->id()] * fing0->getTransformLocale() * glm::vec4(0, 0, 0, 1);
    //        _alignment_matrices[fing0->id()] = findRotationMatrix(v_s, v_m);
    //    }
    //    {
    //        glm::vec3 v_m = landmarks._landmarks[0][14] - landmarks._landmarks[0][13];
    //        glm::vec3 v_s = _alignment_matrices[fing0->id()] * _alignment_matrices[wrist->id()] * fing1->getTransformLocale() * glm::vec4(0, 0, 0, 1);
    //        _alignment_matrices[fing1->id()] = findRotationMatrix(v_s, v_m);
    //    }
    //    {
    //        glm::vec3 v_m = landmarks._landmarks[0][15] - landmarks._landmarks[0][14];
    //        glm::vec3 v_s = _alignment_matrices[fing1->id()] * _alignment_matrices[fing0->id()] * _alignment_matrices[wrist->id()] * fing2->getTransformLocale() * glm::vec4(0, 0, 0, 1);
    //        _alignment_matrices[fing2->id()] = findRotationMatrix(v_s, v_m);
    //    }
    //    {
    //        glm::vec3 v_m = landmarks._landmarks[0][16] - landmarks._landmarks[0][15];
    //        glm::vec3 v_s = _alignment_matrices[fing2->id()] * _alignment_matrices[fing1->id()] * _alignment_matrices[fing0->id()] * _alignment_matrices[wrist->id()] * fing3->getTransformLocale() * glm::vec4(0, 0, 0, 1);
    //        _alignment_matrices[fing3->id()] = findRotationMatrix(v_s, v_m);
    //    }
    //}

    //{// -- PINKY --
    //    const Joint* wrist = _hand.getSkeleton()->skeleton();
    //    const Joint* fing0 = static_cast<Joint*>(wrist->getChildren()[4]);
    //    const Joint* fing1 = static_cast<Joint*>(fing0->getChildren()[0]);
    //    const Joint* fing2 = static_cast<Joint*>(fing1->getChildren()[0]);
    //    const Joint* fing3 = static_cast<Joint*>(fing2->getChildren()[0]);
    //    {
    //        glm::vec3 v_m = landmarks._landmarks[0][17] - landmarks._landmarks[0][0];
    //        glm::vec3 v_s = _alignment_matrices[wrist->id()] * fing0->getTransformLocale() * glm::vec4(0, 0, 0, 1);
    //        _alignment_matrices[fing0->id()] = findRotationMatrix(v_s, v_m);
    //    }
    //    {
    //        glm::vec3 v_m = landmarks._landmarks[0][18] - landmarks._landmarks[0][17];
    //        glm::vec3 v_s = _alignment_matrices[fing0->id()] * _alignment_matrices[wrist->id()] * fing1->getTransformLocale() * glm::vec4(0, 0, 0, 1);
    //        _alignment_matrices[fing1->id()] = findRotationMatrix(v_s, v_m);
    //    }
    //    {
    //        glm::vec3 v_m = landmarks._landmarks[0][19] - landmarks._landmarks[0][18];
    //        glm::vec3 v_s = _alignment_matrices[fing1->id()] * _alignment_matrices[fing0->id()] * _alignment_matrices[wrist->id()] * fing2->getTransformLocale() * glm::vec4(0, 0, 0, 1);
    //        _alignment_matrices[fing2->id()] = findRotationMatrix(v_s, v_m);
    //    }
    //    {
    //        glm::vec3 v_m = landmarks._landmarks[0][20] - landmarks._landmarks[0][19];
    //        glm::vec3 v_s = _alignment_matrices[fing2->id()] * _alignment_matrices[fing1->id()] * _alignment_matrices[fing0->id()] * _alignment_matrices[wrist->id()] * fing3->getTransformLocale() * glm::vec4(0, 0, 0, 1);
    //        _alignment_matrices[fing3->id()] = findRotationMatrix(v_s, v_m);
    //    }
    //}
}

glm::mat4 MediapipeAndGLTF::findRotationMatrix(
    const glm::vec3& from,
    const glm::vec3& to
) {
    glm::mat4 rot = alignVectorMatrix(
        glm::normalize(from),
        glm::normalize(to)
    );
    glm::mat4 res = rot;
    res[3][3] = 1.0f;
    return rot;
}


glm::mat3 MediapipeAndGLTF::alignVectorMatrix(
    const glm::vec3& from,
    const glm::vec3& to
) {
    glm::vec3 refl = glm::vec3(0, 0, 1);
    if (std::abs(from.x) < 0.72f && std::abs(to.x) < 0.72f) {
        refl = glm::vec3(1, 0, 0);
    }
    else if (std::abs(from.y) < 0.72 && std::abs(to.y) < 0.72) {
        refl = glm::vec3(0, 1, 0);
    }

    glm::vec3 u = refl - from;
    glm::vec3 v = refl - to;
    glm::mat3 r = glm::mat3(1);
    for (int32_t i = 0; i < 3; ++i) {
        for (int32_t j = 0; j < 3; ++j) {
            r[i][j] = ((i == j) ? 1 : 0) -
                2 / glm::dot(u, u) * u[i] * u[j] -
                2 / glm::dot(v, v) * v[i] * v[j] +
                4 * glm::dot(u, v) / (glm::dot(u, u) * glm::dot(v, v)) * v[i] * u[j];
        }
    }

    return glm::transpose(r);
}

glm::vec3 MediapipeAndGLTF::vectorAngle(glm::vec3 A, glm::vec3 B) {
    float angle_z = 0;
    if (A.x * B.x + A.y * B.y > 0 || A.x * B.x + A.y * B.y < 0) {
        angle_z = std::acos(
            (A.x * B.x + A.y * B.y)
            /
            (std::sqrt(A.x * A.x + A.y * A.y) * std::sqrt(B.x * B.x + B.y * B.y))
        );
    }

    float angle_y = 0;
    if (A.x * B.x + A.z * B.z > 0 || A.x * B.x + A.z * B.z < 0) {
        angle_y = std::acos(
            (A.x * B.x + A.z * B.z)
            /
            (std::sqrt(A.x * A.x + A.z * A.z) * std::sqrt(B.x * B.x + B.z * B.z))
        );
    }

    float angle_x = 0;
    if (A.y * B.y + A.z * B.z > 0 || A.y * B.y + A.z * B.z < 0) {
        angle_x = std::acos(
            (A.y * B.y + A.z * B.z)
            /
            (std::sqrt(A.y * A.y + A.z * A.z) * std::sqrt(B.y * B.y + B.z * B.z))
        );
    }
    return glm::vec3(angle_x, angle_y, angle_z);
}


void MediapipeAndGLTF::landmarksToLocal(const mdph::Landmarks& landmarks) {
    const float offset_size = 20.0f;
    _right_hand[0].setPositionLocale(landmarks._landmarks[0][0] * 10.f);
    // One
    _right_hand[1].setPositionLocale(
        (landmarks._landmarks[0][1] - landmarks._landmarks[0][0]) * offset_size
    );
    _right_hand[2].setPositionLocale(
        (landmarks._landmarks[0][2] - landmarks._landmarks[0][1]) * offset_size
    );
    _right_hand[3].setPositionLocale(
        (landmarks._landmarks[0][3] - landmarks._landmarks[0][2]) * offset_size
    );
    _right_hand[4].setPositionLocale(
        (landmarks._landmarks[0][4] - landmarks._landmarks[0][3]) * offset_size
    );
    // two
    _right_hand[5].setPositionLocale(
        (landmarks._landmarks[0][5] - landmarks._landmarks[0][0]) * offset_size
    );
    _right_hand[6].setPositionLocale(
        (landmarks._landmarks[0][6] - landmarks._landmarks[0][5]) * offset_size
    );
    _right_hand[7].setPositionLocale(
        (landmarks._landmarks[0][7] - landmarks._landmarks[0][6]) * offset_size
    );
    _right_hand[7].setPositionLocale(
        (landmarks._landmarks[0][8] - landmarks._landmarks[0][7]) * offset_size
    );
    // three
    _right_hand[9].setPositionLocale(
        (landmarks._landmarks[0][9] - landmarks._landmarks[0][0]) * offset_size
    );
    _right_hand[10].setPositionLocale(
        (landmarks._landmarks[0][10] - landmarks._landmarks[0][9]) * offset_size
    );
    _right_hand[11].setPositionLocale(
        (landmarks._landmarks[0][11] - landmarks._landmarks[0][10]) * offset_size
    );
    _right_hand[12].setPositionLocale(
        (landmarks._landmarks[0][12] - landmarks._landmarks[0][11]) * offset_size
    );
    // four
    _right_hand[13].setPositionLocale(
        (landmarks._landmarks[0][13] - landmarks._landmarks[0][0]) * offset_size
    );
    _right_hand[14].setPositionLocale(
        (landmarks._landmarks[0][14] - landmarks._landmarks[0][13]) * offset_size
    );
    _right_hand[15].setPositionLocale(
        (landmarks._landmarks[0][15] - landmarks._landmarks[0][14]) * offset_size
    );
    _right_hand[16].setPositionLocale(
        (landmarks._landmarks[0][16] - landmarks._landmarks[0][15]) * offset_size
    );
    // five
    _right_hand[17].setPositionLocale(
        (landmarks._landmarks[0][17] - landmarks._landmarks[0][0]) * offset_size
    );
    _right_hand[18].setPositionLocale(
        (landmarks._landmarks[0][18] - landmarks._landmarks[0][17]) * offset_size
    );
    _right_hand[19].setPositionLocale(
        (landmarks._landmarks[0][19] - landmarks._landmarks[0][18]) * offset_size
    );
    _right_hand[20].setPositionLocale(
        (landmarks._landmarks[0][20] - landmarks._landmarks[0][19]) * offset_size
    );
}

std::vector<glm::vec3> MediapipeAndGLTF::cube() {
    std::vector<glm::vec3> cube = {
        // Front
        glm::vec3(-1, -1, -1),
        glm::vec3(1, -1, -1),
        glm::vec3(1, 1, -1),
        glm::vec3(-1, 1, -1),
        // Right
        glm::vec3(1, -1, -1),
        glm::vec3(1, -1, 1),
        glm::vec3(1, 1, 1),
        glm::vec3(1, 1, -1),
        // Back
        glm::vec3(1, -1, 1),
        glm::vec3(1, 1, 1),
        glm::vec3(-1, 1, 1),
        glm::vec3(-1, -1, 1),
        // Left
        glm::vec3(-1, -1, 1),
        glm::vec3(-1, -1, -1),
        glm::vec3(-1, 1, -1),
        glm::vec3(-1, 1, 1),
        // Top
        glm::vec3(-1, 1, -1),
        glm::vec3(1, 1, -1),
        glm::vec3(1, 1, 1),
        glm::vec3(-1, 1, 1),
        // Bottom
        glm::vec3(-1, -1, 1),
        glm::vec3(1, -1, 1),
        glm::vec3(1, -1, -1),
        glm::vec3(-1, -1, -1)
    };
    return cube;
}

std::vector<uint32_t> MediapipeAndGLTF::cubeIndex() {
    std::vector<uint32_t> index{};

    for (uint32_t i = 0; i < 6; i++) {
        index.push_back(0 + i * 4);
        index.push_back(1 + i * 4);
        index.push_back(3 + i * 4);
        index.push_back(1 + i * 4);
        index.push_back(2 + i * 4);
        index.push_back(3 + i * 4);
    }

    return index;
}

void MediapipeAndGLTF::createShaderHand() {
    //-----------//
    // Shader    //
    //-----------//
    _lbs_shader.setNbFrame(DefaultConf::NB_FRAME);
    _lbs_shader.setDevice(DefaultConf::logical_device->getDevice());
    _lbs_shader.read(
        "../../shaders/lbs.vert",
        "../../shaders/lbs.frag"
    );
    // -- Vertex Shader -- //
    // Configuring the Graphic Shader
    _lbs_shader.addVertexBufferInfo(
        "coord",
        Object3D::getCoordsStride(DefaultConf::renderer, &_lbs_shader),
        VK_FORMAT_R32G32B32_SFLOAT,
        0
    );
    _lbs_shader.addVertexBufferInfo(
        "object_id",
        Object3D::getMeshOffsetsStride(DefaultConf::renderer, &_lbs_shader),
        VK_FORMAT_R32_UINT,
        1
    );
    _lbs_shader.addVertexBufferInfo(
        "material_id",
        Object3D::getMaterialIndicesStride(DefaultConf::renderer, &_lbs_shader),
        VK_FORMAT_R32_UINT,
        2
    );
    _lbs_shader.addVertexBufferInfo(
        "uv",
        Object3D::getUVStride(DefaultConf::renderer, &_lbs_shader),
        VK_FORMAT_R32G32_SFLOAT,
        3
    );
    _lbs_shader.addVertexBufferInfo(
        "joints",
        Object3D::getJointsStride(DefaultConf::renderer, &_lbs_shader),
        VK_FORMAT_R32G32B32A32_UINT,
        4
    );
    _lbs_shader.addVertexBufferInfo(
        "weights",
        Object3D::getWeightsStride(DefaultConf::renderer, &_lbs_shader),
        VK_FORMAT_R32G32B32A32_SFLOAT,
        5
    );

    // Index Buffer
    _lbs_shader.addIndexBufferInfo(
        "index",
        Object3D::getIndicesNbElem(DefaultConf::renderer, &_lbs_shader)
    );

    // Push constant
    _lbs_shader.addPushConstantInfo(
        "pc",
        VK_SHADER_STAGE_VERTEX_BIT,
        0,
        sizeof(_camera)
    );

    // Storage Buffers
    // transform
    _lbs_shader.addStorageBufferInfo(
        "obj_tr",
        1,
        VK_SHADER_STAGE_VERTEX_BIT
    );

    _lbs_shader.addStorageBufferInfo(
        "joint_tr",
        2,
        VK_SHADER_STAGE_VERTEX_BIT
    );

    // -- Fragment shader -- //
    // materials
    _lbs_shader.addStorageBufferInfo(
        "MaterialsBuffer",
        3,
        VK_SHADER_STAGE_FRAGMENT_BIT
    );

    _lbs_shader.addSamplerInfo(
        "samp",
        4,
        VK_SHADER_STAGE_FRAGMENT_BIT
    );

    _lbs_shader.addTextureInfo(
        "textures",
        5,
        VK_SHADER_STAGE_FRAGMENT_BIT,
        Object3D::getTextures2D(DefaultConf::renderer, &_lbs_shader).size()
    );


    //----------------------//
    // Creating the buffers //
    //----------------------//
    _coord_buffer_hand = StagingBuffer<IGEBufferUsage::vertex_buffer>(
        DefaultConf::logical_device->getDevice(),
        Object3D::getCoordsSize(DefaultConf::renderer, &_lbs_shader),
        Object3D::getCoords(DefaultConf::renderer, &_lbs_shader).data()
    );
    _lbs_shader.addVertexBuffer("coord", &_coord_buffer_hand);

    _object_id_buffer_hand = StagingBuffer<IGEBufferUsage::vertex_buffer>(
        DefaultConf::logical_device->getDevice(),
        Object3D::getObjectIdSize(DefaultConf::renderer, &_lbs_shader),
        Object3D::getObjectId(DefaultConf::renderer, &_lbs_shader).data()
    );
    _lbs_shader.addVertexBuffer("object_id", &_object_id_buffer_hand);

    _material_indices_buffer_hand = StagingBuffer<IGEBufferUsage::vertex_buffer>(
        DefaultConf::logical_device->getDevice(),
        Object3D::getMaterialIndicesSize(DefaultConf::renderer, &_lbs_shader),
        Object3D::getMaterialIndices(DefaultConf::renderer, &_lbs_shader).data()
    );
    _lbs_shader.addVertexBuffer("material_id", &_material_indices_buffer_hand);

    _uv_buffer_hand = StagingBuffer<IGEBufferUsage::vertex_buffer>(
        DefaultConf::logical_device->getDevice(),
        Object3D::getUVSize(DefaultConf::renderer, &_lbs_shader),
        Object3D::getUV(DefaultConf::renderer, &_lbs_shader).data()
    );
    _lbs_shader.addVertexBuffer("uv", &_uv_buffer_hand);

    _joints_buffer_hand = StagingBuffer<IGEBufferUsage::vertex_buffer>(
        DefaultConf::logical_device->getDevice(),
        Object3D::getJointsSize(DefaultConf::renderer, &_lbs_shader),
        Object3D::getJoints(DefaultConf::renderer, &_lbs_shader).data()
    );
    _lbs_shader.addVertexBuffer("joints", &_joints_buffer_hand);

    _weights_buffer_hand = StagingBuffer<IGEBufferUsage::vertex_buffer>(
        DefaultConf::logical_device->getDevice(),
        Object3D::getWeightsSize(DefaultConf::renderer, &_lbs_shader),
        Object3D::getWeights(DefaultConf::renderer, &_lbs_shader).data()
    );
    _lbs_shader.addVertexBuffer("weights", &_weights_buffer_hand);

    // Index buffer
    _index_buffer_hand = StagingBuffer<IGEBufferUsage::index_buffer>(
        DefaultConf::logical_device->getDevice(),
        Object3D::getIndicesSize(DefaultConf::renderer, &_lbs_shader),
        Object3D::getIndices(DefaultConf::renderer, &_lbs_shader).data()
    );
    _lbs_shader.addIndexBuffer("index", &_index_buffer_hand);

    // Push Constant
    _lbs_shader.addPushConstant("pc", &_camera[0][0]);

    // Storage Buffers
    // transform
    _obj_tr_buffer_hand = StagingBuffer<IGEBufferUsage::storage_buffer>(
        DefaultConf::logical_device->getDevice(),
        Object3D::getTransformMatricesSize(DefaultConf::renderer, &_lbs_shader),
        &Object3D::getTransformMatrices(DefaultConf::renderer, &_lbs_shader)[0][0]
    );
    _lbs_shader.addStorageBuffer("obj_tr", &_obj_tr_buffer_hand);

    // Joint transform
    _joint_tr_buffer_hand = StagingBuffer<IGEBufferUsage::storage_buffer>(
        DefaultConf::logical_device->getDevice(),
        Object3D::getJointsTransformSize(DefaultConf::renderer, &_lbs_shader),
        &Object3D::getJointsTransform(DefaultConf::renderer, &_lbs_shader)[0][0]
    );
    _lbs_shader.addStorageBuffer("joint_tr", &_joint_tr_buffer_hand);

    // materials
    _materials_buffer_hand = StagingBuffer<IGEBufferUsage::storage_buffer>(
        DefaultConf::logical_device->getDevice(),
        Object3D::getMaterialsSize(DefaultConf::renderer, &_lbs_shader),
        Object3D::getMaterials(DefaultConf::renderer, &_lbs_shader).data()
    );
    _lbs_shader.addStorageBuffer("MaterialsBuffer", &_materials_buffer_hand);

    // Sampler
    _sampler_hand.setDevice(DefaultConf::logical_device->getDevice());
    _sampler_hand.create();
    _lbs_shader.addSampler("samp", &_sampler_hand);

    // Textures
    _lbs_shader.addTexture2D(
        "textures",
        Object3D::getTextures2D(DefaultConf::renderer, &_lbs_shader)
    );
}