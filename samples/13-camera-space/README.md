Model矩阵表示对模型本身进行变换，例如：
p.Rotate(0.0f, Scale, 0.0f); // 将模型scale
p.WorldPos(0.0f, 0.0f, 1.0f); // 将模型进行平移


View矩阵定义摄像机如果观察，主要是相机的pos, LookAt矩阵进行构建
p.SetCamera(*pGameCamera);


Projection矩阵定义相机的视椎体，主要是width,height,fov,near,far这几个参数构成
p.SetPerspectiveProj(gPersProjInfo);