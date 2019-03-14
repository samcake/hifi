#define GPU_TRANSFORM_IS_STEREO
#define GPU_TRANSFORM_STEREO_CAMERA
#define GPU_TRANSFORM_STEREO_CAMERA_INSTANCED
#ifdef GPU_GL410
#define GPU_TRANSFORM_STEREO_SPLIT_SCREEN
#elif defined(GPU_GL450)
#define GPU_TRANSFORM_STEREO_LAYERED
#else
#endif
