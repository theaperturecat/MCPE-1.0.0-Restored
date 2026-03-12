cmake_minimum_required(VERSION 3.25)


add_library(Renderer STATIC)

target_include_directories(Renderer
PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(Renderer
PRIVATE
color.cpp
enablescissortest.cpp
globalconstantbuffermanager.cpp
globalconstantbuffers.cpp
materialptr.cpp
matrixstack.cpp
Mesh.cpp
RendererSettings.cpp
rendermaterial.cpp
TextureGroup.cpp
texturepair.cpp
TexturePtr.cpp
uniformmetadata.cpp
VertexFormat.cpp
constantbuffermetadata.cpp
constantbuffermetadatamanager.cpp

constantbuffers/alignmenthelper.cpp
constantbuffers/entityconstants.cpp
constantbuffers/holoconstants.cpp
constantbuffers/perframeconstants.cpp
constantbuffers/renderchunkconstants.cpp
constantbuffers/shaderconstants.cpp
constantbuffers/weatherconstants.cpp
constantbuffers/worldconstants.cpp
constantbuffers/worldconstantsholographic.cpp

hal/framebufferattachmentdescription.cpp
hal/framebufferdescription.cpp
hal/sampledescription.cpp
hal/texturedescription.cpp

hal/base/blendstatebase.cpp
hal/base/bufferbase.cpp
hal/base/constantbuffercontainerbase.cpp
hal/base/depthstencilstatebase.cpp
hal/base/deviceinformationbase.cpp
hal/base/framebufferattachmentbase.cpp
hal/base/framebufferobjectbase.cpp
hal/base/rasterizerstatebase.cpp
hal/base/rendercontextbase.cpp
hal/base/rendercontextstatebase.cpp
hal/base/renderdevicebase.cpp
hal/base/samplerstatebase.cpp
hal/base/shaderbase.cpp
hal/base/shaderconstantbase.cpp
hal/base/shaderprogrambase.cpp
hal/base/swapchainbase.cpp
hal/base/texturebase.cpp

hal/enums/shaderprimitivetypes.cpp

hal/helpers/texturehelper.cpp

hal/interface/attribute.cpp
hal/interface/blendstate.cpp
hal/interface/blendstatedescription.cpp
hal/interface/buffer.cpp
hal/interface/constantbuffercontainer.cpp
hal/interface/depthstencilstate.cpp
hal/interface/depthstencilstatedescription.cpp
hal/interface/deviceinformation.cpp
hal/interface/framebufferattachment.cpp
hal/interface/framebufferobject.cpp
hal/interface/immediatebuffer.cpp
hal/interface/rasterizerstate.cpp
hal/interface/rasterizerstatedescription.cpp
hal/interface/rendercontext.cpp
hal/interface/rendercontextimmediate.cpp
hal/interface/renderdevice.cpp
hal/interface/samplerstate.cpp
hal/interface/samplerstatedescription.cpp
hal/interface/shader.cpp
hal/interface/shaderprogram.cpp
hal/interface/stencilfacedescription.cpp
hal/interface/swapchain.cpp
hal/interface/texture.cpp

hal/ogl/blendstateogl.cpp
hal/ogl/bufferogl.cpp
hal/ogl/constantbuffercontainerogl.cpp
hal/ogl/depthstencilstateogl.cpp
hal/ogl/deviceinformationogl.cpp
hal/ogl/errorhandlingogl.cpp
hal/ogl/featuresupportogl.cpp
hal/ogl/framebufferattachmentogl.cpp
hal/ogl/framebufferobjectogl.cpp
hal/ogl/gl_header.cpp
hal/ogl/gl_version_info.cpp
hal/ogl/helpers/maskhelperogl.cpp
hal/ogl/helpers/texturehelperogl.cpp
hal/ogl/immediatebufferogl.cpp
hal/ogl/rasterizerstateogl.cpp
hal/ogl/rendercontextogl.cpp
hal/ogl/renderdeviceogl.cpp
hal/ogl/samplerstateogl.cpp
hal/ogl/shaderconstantogl.cpp
hal/ogl/shaderogl.cpp
hal/ogl/shaderprogramogl.cpp
hal/ogl/shaderuniformogl.cpp
hal/ogl/swapchainogl.cpp
hal/ogl/textureogl.cpp


PUBLIC

inheritancetree.h
materialptr.h
matrix.h
matrixstack.h
rect2d.h
rendermaterial.h
RendererSettings.h
Mesh.h
TexturePtr.h
GlobalConstantBufferManager.h
VertexFormat.h
TextureGroup.h
pch_renderer.h

HAL/base/rendercontextstatebase.h
HAL/base/shaderconstantwithdatabase.h
HAL/base/texturebase.h

HAL/ogl/shaderconstantwithdataogl.h

HAL/Enums/RenderFeature.h
HAL/Enums/PrimitiveMode.h
HAL/Enums/RenderFeature.h
HAL/Enums/BindFlagsBit.h
HAL/Enums/TextureFormat.h

HAL/Interface/RenderContextImmediate.h
HAL/Interface/RenderContext.h
HAL/Interface/Texture.h
HAL/Interface/ProfileSection.h
HAL/Interface/RenderContext.h
HAL/Interface/RenderContextImmediate.h
HAL/Interface/RenderDevice.h
HAL/Interface/constantbuffercontainer.h

constantbuffers/worldconstantsholographic.h
)

target_compile_definitions(Renderer
    PRIVATE
    MCPE_PLATFORM_WIN32
    MCPE_PLATFORM_WINDOWS_FAMILY
)