#!/bin/bash

# 自动生成compile_commands.json的脚本
# 用于VSCode的C/C++智能感知和跳转功能

echo "🔄 正在自动生成compile_commands.json..."

# 进入项目根目录
cd "$(dirname "$0")"

# 清理之前的文件
rm -f compile_commands.json

# 进入MPLAB项目目录
cd NWPT_VR.X

# 清理编译输出以确保获取完整的编译命令
echo "🧹 清理编译输出..."
make clean > /dev/null 2>&1

# 使用make的dry-run模式获取所有编译命令
echo "📝 获取编译命令..."
make -n 2>/dev/null | grep -E "xc8-cc.*\.c" > ../temp_commands.txt

# 回到项目根目录
cd ..

# 检查是否获取到编译命令
if [ ! -s temp_commands.txt ]; then
    echo "❌ 未能获取到编译命令，请检查makefile配置"
    rm -f temp_commands.txt
    exit 1
fi

echo "📋 找到 $(wc -l < temp_commands.txt) 个源文件的编译命令"

# 开始构建JSON文件
echo "[" > compile_commands.json

# 处理每个编译命令
line_count=0
total_lines=$(wc -l < temp_commands.txt)

while IFS= read -r line; do
    line_count=$((line_count + 1))
    
    # 提取源文件路径
    source_file=$(echo "$line" | grep -o '[^[:space:]]*\.c' | head -1)
    
    if [ ! -z "$source_file" ]; then
        # 处理相对路径，去掉../前缀
        if [[ "$source_file" == ../* ]]; then
            abs_source_file="$(pwd)/${source_file#../}"
        else
            abs_source_file="$(pwd)/$source_file"
        fi
        
        # 转义command中的引号
        escaped_command=$(echo "$line" | sed 's/"/\\"/g')
        
        # 构建JSON条目
        echo "  {" >> compile_commands.json
        echo "    \"directory\": \"$(pwd)/NWPT_VR.X\"," >> compile_commands.json
        echo "    \"command\": \"$escaped_command\"," >> compile_commands.json
        echo "    \"file\": \"$abs_source_file\"" >> compile_commands.json
        
        # 如果不是最后一个条目，添加逗号
        if [ $line_count -lt $total_lines ]; then
            echo "  }," >> compile_commands.json
        else
            echo "  }" >> compile_commands.json
        fi
    fi
done < temp_commands.txt

# 结束JSON数组
echo "]" >> compile_commands.json

# 清理临时文件
rm -f temp_commands.txt

# 验证JSON格式
if python3 -m json.tool compile_commands.json > /dev/null 2>&1; then
    echo "✅ compile_commands.json 生成成功！"
    echo "📁 文件位置: $(pwd)/compile_commands.json"
    echo "📊 包含 $(grep -c '"file":' compile_commands.json) 个源文件"
    
    # 显示包含的文件列表
    echo "📋 包含的源文件："
    grep '"file":' compile_commands.json | sed 's/.*"file": "\(.*\)".*/\1/' | sed 's|.*/||' | sort | uniq
else
    echo "❌ JSON格式验证失败"
    exit 1
fi

echo ""
echo "💡 提示："
echo "   - 每次添加新源文件后，运行此脚本更新compile_commands.json"
echo "   - 在VSCode中按 Cmd+Shift+P，输入 'C/C++: Reload IntelliSense Database' 重新加载"
echo "   - 或者重启VSCode以应用新的配置"
