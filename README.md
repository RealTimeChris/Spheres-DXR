# Summary
Uses 1 ray per pixel and the scene data can be modified by changing the values contained within Main.cpp.

In terms of DirectX 12, it makes use of a pipeline state with a global root signature, a ray generation shader, a Lambertian material hit group and miss shader, and writes the output into a 2D texture bound via unordered access. This output is presented to a Win32 window using a Vsync-enabled swap chain. The per-frame scene data is accessed in the shaders as a structure that is passed as global inline root constants.

Requires a GPU with DXR support as I have not implemented the fallback layer.


# Sample Output
![Lambertian 01](https://github.com/RealTimeChris/Spheres-DXR/blob/main/Sample%20Output/Lambertian%2001.png?raw=true)

![Lambertian 02](https://github.com/RealTimeChris/Spheres-DXR/blob/main/Sample%20Output/Lambertian%2002.png?raw=true)

![Lambertian 03](https://github.com/RealTimeChris/Spheres-DXR/blob/main/Sample%20Output/Lambertian%2003.png?raw=true)
