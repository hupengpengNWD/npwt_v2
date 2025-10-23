#!/bin/bash

# 智能编译脚本 - 自动更新compile_commands.json
# 使用方法: ./smart_build.sh [clean]

echo "🚀 智能编译脚本启动..."

# 进入项目根目录
cd "$(dirname "$0")"

# 检查是否需要清理
if [ "$1" == "clean" ]; then
    echo "🧹 执行清理编译..."
    make -C NWPT_VR.X clean
    echo "✅ 清理完成"
fi

# 编译项目
echo "🔨 开始编译..."
make -C NWPT_VR.X

# 检查编译结果
if [ $? -eq 0 ]; then
    echo "✅ 编译成功！"
    
    # 检查是否有新文件或修改
    echo "🔍 检查是否需要更新compile_commands.json..."
    
    # 检查compile_commands.json是否存在
    if [ ! -f compile_commands.json ]; then
        echo "📝 首次生成compile_commands.json..."
        ./generate_compile_commands_clangd.sh
    else
        # 检查源文件是否有变化
        source_files_count=$(find . -name "*.c" -not -path "./NWPT_VR.X/*" | wc -l)
        json_files_count=$(grep -c '"file":' compile_commands.json)
        
        if [ $source_files_count -ne $json_files_count ]; then
            echo "📝 检测到源文件变化，更新compile_commands.json..."
            ./generate_compile_commands_clangd.sh
        else
            echo "✅ compile_commands.json已是最新"
        fi
    fi
    
    echo ""
    echo "🎉 编译完成！"
    echo "💡 提示："
    echo "   - 如果clangd没有更新，请重启: Cmd+Shift+P → 'clangd: Restart language server'"
    echo "   - 或者重启VSCode"
    
else
    echo "❌ 编译失败！"
    exit 1
fi
