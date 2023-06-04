
glEnableVertexAttribArray(0){
    m_apiState.pCurrentVAO->EnableAttribute(index);
}

MaxAttributeVectors=16  // Max Vec4 Vertex Attributes Accessible to Vertex Shader
MaxVertexBuffers=16     // Max Vertex Buffers that can be bound to a Vertex Array Object

All the vertex attribute will be accessed by vertex shader, for this tutorial, the only attribute is position and 
it use the attribute index 0.

Also, about 16 vertex buffers can be bound to the VAO, and all the atributes are initilized to bind the buffer 0 by default.
