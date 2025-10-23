#!/bin/bash

# 生成compile_commands.json的脚本
# 用于VSCode的C/C++智能感知和跳转功能

echo "正在生成compile_commands.json..."

# 清理之前的文件
rm -f compile_commands.json

# 进入MPLAB项目目录
cd NWPT_VR.X

# 使用make的dry-run模式获取编译命令
make -n 2>/dev/null | grep -E "xc8-cc.*\.c" | while read line; do
    # 提取源文件路径
    source_file=$(echo "$line" | grep -o '[^[:space:]]*\.c' | head -1)
    
    if [ ! -z "$source_file" ]; then
        # 构建JSON条目
        echo "  {" >> ../compile_commands.json
        echo "    \"directory\": \"$(pwd)\"," >> ../compile_commands.json
        echo "    \"command\": \"$line\"," >> ../compile_commands.json
        echo "    \"file\": \"$(realpath $source_file)\"" >> ../compile_commands.json
        echo "  }," >> ../compile_commands.json
    fi
done

# 回到项目根目录
cd ..

# 添加JSON数组的开始和结束
if [ -f compile_commands.json ]; then
    # 移除最后一个逗号
    sed -i '' '$ s/,$//' compile_commands.json
    
    # 添加JSON数组结构
    echo "[" > temp.json
    cat compile_commands.json >> temp.json
    echo "]" >> temp.json
    mv temp.json compile_commands.json
    
    echo "✅ compile_commands.json 生成完成！"
    echo "📁 文件位置: $(pwd)/compile_commands.json"
else
    echo "❌ 未能生成compile_commands.json"
fi
