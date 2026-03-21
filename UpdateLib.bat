

xcopy /y/s	.\Engine\Public\*.*			.\Reference\Headers\
xcopy /y	.\Engine\Bin\Engine.lib		.\Reference\Lib\
xcopy /y	.\Engine\ThirdpartyLib\*.lib	.\Reference\Lib\
xcopy /y	.\Engine\Bin\Engine.dll		.\Client\Bin\
xcopy /y	.\Engine\Bin\Engine.dll		.\MapEditor\Bin\
xcopy /y	.\Engine\Bin\Engine.dll		.\Effect_Editor\Bin\
xcopy /y	.\Engine\Bin\Engine.dll		.\AnimTool\Bin\
xcopy /y	.\Engine\Bin\Engine.dll		.\Shader_Editor\Bin\
xcopy /y	.\Engine\Bin\ShaderFiles\*.*		.\Client\Bin\ShaderFiles\
xcopy /y	.\Engine\Bin\ShaderFiles\*.*		.\Effect_Editor\Bin\ShaderFiles\
xcopy /y	.\Engine\Bin\ShaderFiles\*.*		.\AnimTool\Bin\ShaderFiles\
xcopy /y	.\Engine\Bin\ShaderFiles\*.*		.\MapEditor\Bin\ShaderFiles\
xcopy /y	.\Engine\Bin\ShaderFiles\*.*		.\Shader_Editor\Bin\ShaderFiles\
