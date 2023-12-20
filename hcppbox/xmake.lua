local TARGET_NAME = "HCppBox"
local LIB_DIR = "$(buildir)/".. TARGET_NAME .. "/"
local LIB_NAME = "lib" .. TARGET_NAME .. ".a "

target(TARGET_NAME)
    set_kind("static")
    set_targetdir(LIB_DIR)
    
    --添加宏定义
    add_defines("HDEFAULTS_MUTEX_LOCK=main_enter_critical")
    add_defines("HDEFAULTS_MUTEX_UNLOCK=main_exit_critical")
    add_defines("HDEFAULTS_MALLOC=main_malloc")
    add_defines("HDEFAULTS_FREE=main_free")
    --加入代码和头文件
    add_includedirs("./HCppBox/hbox",{public = true})
    add_files("./HCppBox/hbox/*.c",{public = true})

    --可以继续增加add_includedirs和add_files
    --自动链接
    LIB_USER = LIB_USER .. SDK_TOP ..  "/".. LIB_DIR .. LIB_NAME .. " "
    --甚至可以加入自己的库
target_end()