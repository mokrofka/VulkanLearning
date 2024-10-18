@echo off
set SHADER_DIR=%~dp0Editor\shaders
set COMPILED_SHADER_DIR=%~dp0Editor\shaders\compiled

for %%f in (%SHADER_DIR%\*.vert) do (
    glslangValidator -V %%f -o %COMPILED_SHADER_DIR%\%%~nf.vert.spv
)
for %%f in (%SHADER_DIR%\*.frag) do (
    glslangValidator -V %%f -o %COMPILED_SHADER_DIR%\%%~nf.frag.spv
)
