local TARGET_NAME = "RC"
local LIB_DIR = "$(buildir)/".. TARGET_NAME .. "/"
local LIB_NAME = "lib" .. TARGET_NAME .. ".a "

target(TARGET_NAME)
    set_kind("static")
    set_targetdir(LIB_DIR)
    
    --加入代码和头文件
    add_includedirs("./",{public = true})
    add_files("./RC.c",{public = true})
    add_files("./RC_fs.c",{public = true})

    --可以继续增加add_includedirs和add_files
    --自动链接
    LIB_USER = LIB_USER .. SDK_TOP ..  "/".. LIB_DIR .. LIB_NAME .. " "
    --甚至可以加入自己的库
target_end()