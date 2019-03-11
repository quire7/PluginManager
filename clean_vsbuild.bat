@echo off
echo 清除所有obj pch idb pdb ncb opt plg res sbr ilk 文件，请稍等......
pause


del /f /s /q .\*.obj
del /f /s /q .\*.iobj
del /f /s /q .\*.ipdb
del /f /s /q .\*.pch
del /f /s /q .\*.ipch
del /f /s /q .\*.sdf
del /f /s /q .\*.opensdf
del /f /s /q .\*.suo
del /f /s /q .\*.user
del /f /s /q .\*.tlog
del /f /s /q .\*.exp
del /f /s /q .\*.idb
del /f /s /q .\*.pdb
del /f /s /q .\*.ncb 
del /f /s /q .\*.opt 
del /f /s /q .\*.plg
del /f /s /q .\*.res
del /f /s /q .\*.sbr
del /f /s /q .\*.ilk
del /f /s /q .\*.dep
del /f /s /q .\*.manifest
del /f /s /q .\*.ib_tag
del /f /s /q .\*.ib_pdb_index
del /f /s /q buildlog.htm

rd  /s /q 	".\x64/Debug"
rd  /s /q 	".\x64/Release"
rd  /s /q 	".\x64/UnityRelease"

del /f /s /q "bin/win/Release\PluginManager.exe"
del /f /s /q "bin/win/Release64\PluginManager.exe"
del /f /s /q "bin/win/UnityRelease64\PluginManager.exe"

del /f /s /q "bin/win/Release\PluginManager.lib"
del /f /s /q "bin/win/Release64\PluginManager.lib"
del /f /s /q "bin/win/UnityRelease64\PluginManager.lib"




echo 清除文件完成！
echo. & pause