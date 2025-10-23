#!/bin/bash

# 自动生成compile_commands.json的脚本（使用compiledb）
# 用于clangd代码导航

echo "🔄 正在使用compiledb生成compile_commands.json..."

# 进入项目根目录
cd "$(dirname "$0")"

# 进入MPLAB项目目录
cd NWPT_VR.X

# 清理编译输出以确保获取完整的编译命令
echo "🧹 清理编译输出..."
make clean > /dev/null 2>&1

# 使用compiledb生成compile_commands.json
echo "📝 使用compiledb生成compile_commands.json..."
compiledb -n make

# 检查是否生成成功
if [ $? -eq 0 ] && [ -f compile_commands.json ]; then
    echo "✅ compile_commands.json 生成成功！"
    
    # 复制到项目根目录（便于clangd查找）
    cp compile_commands.json ..
    
    # 回到项目根目录
    cd ..
    
    # 验证文件
    if [ -f compile_commands.json ]; then
        echo "📁 文件位置: $(pwd)/compile_commands.json"
        echo "📊 包含 $(grep -c '"file":' compile_commands.json) 个源文件"
        
        # 显示包含的文件列表
        echo "📋 包含的源文件："
        grep '"file":' compile_commands.json | sed 's/.*"file": "\(.*\)".*/\1/' | sed 's|.*/||' | sort | uniq | head -10
        
        echo ""
        echo "💡 提示："
        echo "   - clangd会自动检测compile_commands.json文件"
        echo "   - 每次添加新源文件后，运行此脚本更新"
        echo "   - 在VSCode中重启clangd: Cmd+Shift+P → 'clangd: Restart language server'"
    else
        echo "❌ 复制到根目录失败"
        exit 1
    fi
else
    echo "❌ compiledb生成失败"
    exit 1
fi
