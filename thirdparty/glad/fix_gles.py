#!/usr/bin/env python

import os
import re

pattern = re.compile(".*glad_[^GLES2][a-zA-Z0-9_]+ = NULL;\n")

exceptions = [
    "glad_glClearDepthf",
    "glad_glDepthRangef",
    "glad_glGetShaderPrecisionFormat",
    "glad_glReleaseShaderCompiler",
    "glad_glShaderBinary",
    "glad_glBindSampler",
    "glad_glBindTransformFeedback",
    "glad_glDeleteSamplers",
    "glad_glDeleteTransformFeedbacks",
    "glad_glGenSamplers",
    "glad_glGenTransformFeedbacks",
    "glad_glGetInternalformativ",
    "glad_glGetProgramBinary",
    "glad_glGetSamplerParameterfv",
    "glad_glGetSamplerParameteriv",
    "glad_glInvalidateFramebuffer",
    "glad_glInvalidateSubFramebuffer",
    "glad_glIsSampler",
    "glad_glIsTransformFeedback",
    "glad_glPauseTransformFeedback",
    "glad_glProgramBinary",
    "glad_glProgramParameteri",
    "glad_glResumeTransformFeedback",
    "glad_glSamplerParameterf",
    "glad_glSamplerParameterfv",
    "glad_glSamplerParameteri",
    "glad_glSamplerParameteriv",
    "glad_glTexStorage2D",
    "glad_glTexStorage3D",
    "glad_glVertexAttribDivisor",
    "glad_glActiveShaderProgram",
    "glad_glBindImageTexture",
    "glad_glBindProgramPipeline",
    "glad_glBindVertexBuffer",
    "glad_glCreateShaderProgramv",
    "glad_glDeleteProgramPipelines",
    "glad_glDispatchCompute",
    "glad_glDispatchComputeIndirect",
    "glad_glDrawArraysIndirect",
    "glad_glDrawElementsIndirect",
    "glad_glFramebufferParameteri",
    "glad_glGenProgramPipelines",
    "glad_glGetFramebufferParameteriv",
    "glad_glGetProgramInterfaceiv",
    "glad_glGetProgramPipelineInfoLog",
    "glad_glGetProgramPipelineiv",
    "glad_glGetProgramResourceIndex",
    "glad_glGetProgramResourceLocation",
    "glad_glGetProgramResourceName",
    "glad_glGetProgramResourceiv",
    "glad_glIsProgramPipeline",
    "glad_glMemoryBarrier",
    "glad_glMemoryBarrierByRegion",
    "glad_glProgramUniform1f",
    "glad_glProgramUniform1fv",
    "glad_glProgramUniform1i",
    "glad_glProgramUniform1iv",
    "glad_glProgramUniform1ui",
    "glad_glProgramUniform1uiv",
    "glad_glProgramUniform2f",
    "glad_glProgramUniform2fv",
    "glad_glProgramUniform2i",
    "glad_glProgramUniform2iv",
    "glad_glProgramUniform2ui",
    "glad_glProgramUniform2uiv",
    "glad_glProgramUniform3f",
    "glad_glProgramUniform3fv",
    "glad_glProgramUniform3i",
    "glad_glProgramUniform3iv",
    "glad_glProgramUniform3ui",
    "glad_glProgramUniform3uiv",
    "glad_glProgramUniform4f",
    "glad_glProgramUniform4fv",
    "glad_glProgramUniform4i",
    "glad_glProgramUniform4iv",
    "glad_glProgramUniform4ui",
    "glad_glProgramUniform4uiv",
    "glad_glProgramUniformMatrix2fv",
    "glad_glProgramUniformMatrix2x3fv",
    "glad_glProgramUniformMatrix2x4fv",
    "glad_glProgramUniformMatrix3fv",
    "glad_glProgramUniformMatrix3x2fv",
    "glad_glProgramUniformMatrix3x4fv",
    "glad_glProgramUniformMatrix4fv",
    "glad_glProgramUniformMatrix4x2fv",
    "glad_glProgramUniformMatrix4x3fv",
    "glad_glTexStorage2DMultisample",
    "glad_glUseProgramStages",
    "glad_glValidateProgramPipeline",
    "glad_glVertexAttribBinding",
    "glad_glVertexAttribFormat",
    "glad_glVertexAttribIFormat",
    "glad_glVertexBindingDivisor",
    "glad_glBlendBarrier",
    "glad_glBlendEquationSeparatei",
    "glad_glBlendEquationi",
    "glad_glBlendFuncSeparatei",
    "glad_glBlendFunci",
    "glad_glCopyImageSubData",
    "glad_glDebugMessageCallback",
    "glad_glDebugMessageControl",
    "glad_glDebugMessageInsert",
    "glad_glGetDebugMessageLog",
    "glad_glGetGraphicsResetStatus",
    "glad_glGetObjectLabel",
    "glad_glGetObjectPtrLabel",
    "glad_glGetPointerv",
    "glad_glGetSamplerParameterIiv",
    "glad_glGetSamplerParameterIuiv",
    "glad_glGetnUniformfv",
    "glad_glGetnUniformiv",
    "glad_glGetnUniformuiv",
    "glad_glMinSampleShading",
    "glad_glObjectLabel",
    "glad_glObjectPtrLabel",
    "glad_glPatchParameteri",
    "glad_glPopDebugGroup",
    "glad_glPrimitiveBoundingBox",
    "glad_glPushDebugGroup",
    "glad_glReadnPixels",
    "glad_glSamplerParameterIiv",
    "glad_glSamplerParameterIuiv",
    "glad_glTexBufferRange",
    "glad_glTexStorage3DMultisample",
]

with open("gles2.c", "r") as file:
    lines = file.readlines()
with open("gles2.c", "w") as file:
    for line in lines:
        if pattern.match(line):
            func_name = line.split(" ")[1]
            if func_name in exceptions:
                file.write(line)
                continue
            line = line.replace(" = NULL", "")
            line = "extern " + line
            file.write(line)
            continue
        
        file.write(line)

with open("glad/gles2.h", "r") as file:
    lines = file.readlines()
with open("glad/gles2.h", "w") as file:
    for line in lines:
        if "OpenGL ES 3 header already included (API: gles2), remove previous include!" in line:
            continue
        file.write(line)