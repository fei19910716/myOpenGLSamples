
对于target GL_TEXTURE_2D来说，其有一定数量的texture unit，shader和cpu通过texture unit来进行bind。

// 指定shader中的sampler对应的unit
gSampler = glGetUniformLocation(ShaderProgram, "gSampler");
glUniform1i(gSampler, 1);

// 将纹理数据绑定到unit
pTexture->Bind(GL_TEXTURE1);