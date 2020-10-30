# Pigs In A Blanket | OpenGL ES 2.0 on the PS Vita by CBPS
#### A Piglet/ShaccCg Wrapper Library for OpenGL ES 2.0 Support on the Vita
---
##### Compiling
- Install [DolceSDK](https://github.com/DolceSDK/doc) (Has not been tested on VitaSDK as of yet)
- Run ```make -j4 install```
- Link libpib.a in your projects

Note when building your projects:

If using CMake, make sure to specify ```set(DOLCE_ELF_CREATE_FLAGS "${DOLCE_ELF_CREATE_FLAGS} -h 3194304")```<br>If using Makefile, make sure to specify ```dolce-elf-create -h 3194304```

Piglet needs an SceLibc heap size of at least 3MB to intialize. Without it, the module will fail to start. This heap needs to be larger if using ```-nostdlib``` as this becomes your main heap. More info on ```-nostdlib``` support in the headers.

To install Piglet on your Vita, just use the [Pigs in a Blanket Configuration Tool](https://github.com/SonicMastr/PIB-Configuration-Tool)

Documentation Provided in the Headers. I'm clean.

## Full GLES 2.0 Support, Finally on the Vita
After 2 months of Reverse Engineering and working to make this Library the best it could be, it's finally here.

Thanks to the efforts of [GrapheneCt](https://github.com/GrapheneCt) and [dots-tb](https://github.com/dots-tb) with Vita Piglet, and to [Zer0xFF](https://github.com/Zer0xFF) and [masterzorag](https://github.com/masterzorag) with the PS4 Piglet, we have finally obtained the first full OpenGL GLES 2.0 Compliant graphics library for the Vita.

#### What does this mean? 
Developers who already have vast knowledge in standard GLES 2.0 will have no issue applying their knowledge to the Vita. You no longer have to remember Vita specific functions. You can just code without a worry. The only difference is that libScePiglet uses CG formatted shaders instead of GLSL. For info, check the CG resources provided at the end of this README.

#### GLFW support
I ported [GLFW for the Vita](https://github.com/SonicMastr/glfw-vita) that exclusively uses Piglet. The only things that are wrong are keybinds (Yes. They will be changed). That is all.

#### What does this do for ports?
GLES 2.0 is a common Graphics Library that is present in almost anything you can think of, most notably in mobile devices like phones and tablets, for a cross-platform, accurate and consistent experience. I know some of the normal users will want to know what this means for games. Ports utilizing standard GLES 2.0 should see little to no modifications needed, no questions asked. This makes ports much easier on the graphics side of things, resulting in less visual bugs, less translations issues, and overall better support for the Vita. This was the purpose of Piglet in PSM, and it can can easily be applied to your favorite ports using GLES 2.0. And speaking of PSM... 

#### How is this different from PSM?
As I said, I ditched PSM. This was the alternative that we were working on for a few months now. PSM was missing quite a lot of functions, which was a pain when trying to put something together for an example. Piglet includes every function according to the GLES 2.0 standard without the PSM bloat. This in turn reduces the memory usage of a simple triangle from 54MB to just 25MB by eliminating PSM all together.

|PSM|Piglet|
|:-:|:-:|
|![PSM](https://cdn.discordapp.com/attachments/692425410142666764/771560532528463892/unknown.png)|![Piglet](https://cdn.discordapp.com/attachments/692425410142666764/771563777321074708/unknown.png)|

#### Why this over VitaGL?
VitaGL was, and still is an ambitious project to create an OpenGL wrapper from the ground up. It has been running the porting scene for quite a while due to being the closest thing to OpenGL on the Vita as possible, although its identity is not solid. It tries to tries to be both GL1 and GLES2, and because each function has to be manually recreated, it is still missing a lot functionality that a developer may expect. Along with this, VitaGL-specific functions and the lack of in-depth documentation of those functions further hinders its usage.

Piglet will remove the confusion of VitaGL and give developers a simpler, easier time with porting and its overall usage, allowing developers to use many existing GLES 2.0 resources. Piglet strives to be accurate to the standard, while also providing better memory utilization and performance by utilizing SceLibc and PTLA (Present and Texture Load Accelerator), an asynchronous hardware 2D processing unit, used for texture copy and format convertion/transfer. Its identity is GLES 2.0 and will always be, so there will be no guesswork in how the pipeline will function.

We want to give the developers the tools to make their homebrew the best they can be without the frustration of wondering if it *could* work.

## What is Pigs in a Blanket?
This library is a developer focused wrapper which provides easy initialization with some quality of life features to support resolutions up to 1080p native on the Playstion TV, and on the Vita with Sharpscale. Developers can choose make their applications with one resolution in mind, as all long as the code is made to scale by dimension, PIB will handle the rest with it's companion app, the [Pigs in a Blanket Configuration Tool](https://github.com/SonicMastr/PIB-Configuration-Tool), which allows the user to specify their own preference of resultion that will automatically override the original settings. 

It library doubles as the heart of Piglet's Shader compiling abilities, removed in the standalone module. I spent the time to rewrite the shader compiler code according to PSM specifications to re-enable the ShaccCg support that was removed. This includes proper return codes and regular log output, so you never have to wonder what's going on with your shaders.

This library also restores normal `eglGetProcAddress` functionality as the orignal Piglet only returns extensions. Thanks to dots-tb we were able to create a simple patch to support returning GLES functions as well.

This library will allow us to add extensions using native functions as we feel fit. We'll accept the requests of any developers who have ideas of what extensions to add utilizing PIB, and they will all be able to be accessed via `eglGetProcAddress`.

## Supported Extensions
|GL|EGL|
|:-:|:-:|
|GL_EXT_draw_instanced|EGL_SCE_piglet_sync|
|GL_EXT_instanced_arrays|EGL_SCE_piglet_vita_pre_swap_callback|
|GL_SCE_piglet_shader_binary|EGL_SCE_piglet_vita_vsync_callback|
|GL_SCE_texture_resource|
|GL_OES_texture_npot|
|GL_OES_rgb8_rgba8|
|GL_OES_depth_texture|
|GL_EXT_texture_format_BGRA8888|
|GL_EXT_read_format_bgra|
|GL_OES_texture_half_float|
|GL_OES_texture_half_float_linear|
|GL_OES_vertex_half_float|
|GL_OES_element_index_uint|
|GL_EXT_texture_compression_dxt1|
|GL_EXT_texture_compression_dxt3|
|GL_EXT_texture_compression_dxt5|
|GL_EXT_texture_compression_s3tc|
|GL_EXT_texture_storage|
|GL_IMG_texture_compression_pvrtc|

## Special Thanks
- **[GrapheneCt](https://github.com/GrapheneCt)** - Finding the Piglet Module and being a main part of the Project in reverse engineering and testing
- **[dots-tb](https://github.com/dots-tb)** - Initial idea of using PSM and efforts in getting all of the names for Piglet, as well as being a main part of the Project in reverse engineering and testing
- **[cuevavirus](https://github.com/cuevavirus)** - Help with debugging and sense of direction
- **[CreepNT](https://github.com/CreepNT)** - Help with debugging
- **[Princess-of-Sleeping](https://github.com/Princess-of-Sleeping)** - Dump tool and PrincessLog
- **[xyzz](https://github.com/xyzz)** - Initial deep dive into how ShaccCg works
- **[Zer0xFF](https://github.com/Zer0xFF) and [masterzorag](https://github.com/masterzorag)** - Their amazing work on the PS4 Piglet reverse engineering

## GLSL to CG Conversion Resources
Microsoft GLSL to HLSL: https://docs.microsoft.com/en-us/windows/uwp/gaming/glsl-to-hlsl-reference<br>Nvidia CG Standard Library: http://developer.download.nvidia.com/cg/index_stdlib.html<br>Nvidia CG GLSL Vert to CG: http://developer.download.nvidia.com/cg/glslv.html<br>Nvidia CG GLSL Frag to CG: http://developer.download.nvidia.com/cg/glslf.html

This requires both libshacccg.suprx and libScePiglet.suprx to be located in ur0:data/external
