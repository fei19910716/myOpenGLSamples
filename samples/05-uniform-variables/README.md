
## 透视除法

Clip Space是一个顶点乘以MVP矩阵之后所在的空间，<font color=Red>Vertex Shader的输出就是在Clip Space上</font>，接着由GPU自己做<font color=Red>透视除法</font>将顶点转到NDC。

透视除法将Clip Space顶点的4个分量都除以w分量，就从Clip Space转换到了NDC了。

而NDC是一个长宽高取值范围为[-1,1]的立方体，超过这个范围的顶点，会被GPU剪裁。


## 视口变换

想把[-1,1]的立方体的内容在屏幕上绘制出来，首先我们要把经典立方体映射到屏幕，即进行以下的过程：

1、忽略z
2、把xy平面上的[ − 1 , 1 ]转化（进行缩放和平移）成(0,width) X (0,height)（这个过程也称为视口变换）

然后就开始光栅化