local TARGET_NAME = "Air780E"
local LIB_DIR = "$(buildir)/".. TARGET_NAME .. "/"
local LIB_NAME = "lib" .. TARGET_NAME .. ".a "

includes(SDK_TOP.."/luatos_lwip_socket")
includes(SDK_TOP.."/thirdparty/libhttp")
includes(SDK_TOP .. "/thirdparty/libemqtt")
includes(SDK_TOP .. "../rc")
includes(SDK_TOP .. "../hcppbox")

target(TARGET_NAME)
    set_kind("static")
    set_targetdir(LIB_DIR)
    
    if os.getenv("USE_KCONFIG") == "1" 
    then
       add_includedirs("./../",{public = true})
       add_defines("USE_KCONFIG")        
    end
        
    --加入代码和头文件
    add_includedirs("./",{public = true})
    add_files("./*.c",{public = true})
    add_files("./*.cpp",{public = true})

    --添加cJSON
    add_includedirs(SDK_TOP .. "/thirdparty/cJSON")
    add_files(SDK_TOP .. "/thirdparty/cJSON/**.c")

    --添加jsoncpp
    add_includedirs("./jsoncpp/include",{public = true})
    add_files("./jsoncpp/src/lib_json/*.cpp",{public = true})
    add_deps("RC") --加入RC支持
    add_deps("HCppBox") --加入HCppBox支持
    add_deps("luatos_lwip_socket") --socket依赖
    add_deps("libhttp") --加入HTTP客户端支持，自动加载了socket依赖
    add_deps("libemqtt") --加入MQTT支持


    --可以继续增加add_includedirs和add_files
    --自动链接
    LIB_USER = LIB_USER .. SDK_TOP ..  "/".. LIB_DIR .. LIB_NAME .. " "
    --甚至可以加入自己的库
target_end()
