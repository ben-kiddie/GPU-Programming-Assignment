# GPU-Programming-Assignment
Submitted as part of assessment within the "GPU Programming" module at University.

This application was developed using Visual Studio 2019 under an x86 solution platform.

Main features of the application include:
- Model importing, 
- Lighting (directional, point, spot lights), 
- Multi-pass rendering using image filters in GLSL shaders (e.g., sharpen and blur filters),
- Capturing framebuffers in textures.

How to use:
- Use WASD to move the camera around.
- Use the mouse to look around.
- Hold '1' to activate the sharpen filter. This will produce a filter over the left side of the screen which sharpens edges.
- Hold '2' to activate the blur filter. This produces a filter on the right side of the screen, blurring edges slightly.
- Hold '3' to mirror the right side of the screen to the left side. This can be used in tandem with the blur filter, and will let you compare and contrast the effects of blur.
