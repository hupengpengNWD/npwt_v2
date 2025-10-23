#!/bin/bash

echo "🔍 clangd诊断脚本"
echo "=================="

# 进入项目根目录
cd "$(dirname "$0")"

echo "1. 检查clangd是否安装："
if command -v clangd &> /dev/null; then
    echo "✅ clangd已安装: $(which clangd)"
    clangd --version
else
    echo "❌ clangd未安装"
    echo "请安装clangd: brew install llvm"
fi

echo ""
echo "2. 检查配置文件："
echo "✅ .clangd文件: $(ls -la .clangd 2>/dev/null || echo '❌ 不存在')"
echo "✅ compile_commands.json: $(ls -la compile_commands.json 2>/dev/null || echo '❌ 不存在')"
echo "✅ .vscode/settings.json: $(ls -la .vscode/settings.json 2>/dev/null || echo '❌ 不存在')"

echo ""
echo "3. 检查compile_commands.json内容："
if [ -f compile_commands.json ]; then
    echo "📊 文件大小: $(wc -c < compile_commands.json) 字节"
    echo "📊 源文件数: $(grep -c '"file":' compile_commands.json)"
    echo "📋 包含lcd_driver.c: $(grep -c 'lcd_driver.c' compile_commands.json)"
else
    echo "❌ compile_commands.json不存在"
fi

echo ""
echo "4. 检查LCD_SetBacklight函数定义："
if [ -f Drivers/Inc/lcd_driver.h ]; then
    echo "✅ lcd_driver.h存在"
    grep -n "LCD_SetBacklight" Drivers/Inc/lcd_driver.h || echo "❌ 未找到LCD_SetBacklight定义"
else
    echo "❌ lcd_driver.h不存在"
fi

echo ""
echo "5. 检查LCD_SetBacklight函数实现："
if [ -f Drivers/Src/lcd_driver.c ]; then
    echo "✅ lcd_driver.c存在"
    grep -n "LCD_SetBacklight" Drivers/Src/lcd_driver.c || echo "❌ 未找到LCD_SetBacklight实现"
else
    echo "❌ lcd_driver.c不存在"
fi

echo ""
echo "6. 测试clangd语法检查："
if command -v clangd &> /dev/null && [ -f compile_commands.json ]; then
    echo "🧪 测试clangd对main.c的语法检查..."
    echo "void test() { LCD_SetBacklight(true); }" > /tmp/test.c
    clangd --check=/tmp/test.c 2>&1 | head -5 || echo "clangd检查失败"
    rm -f /tmp/test.c
else
    echo "❌ 无法测试clangd"
fi

echo ""
echo "💡 解决建议："
echo "1. 确保已安装clangd扩展并禁用C/C++扩展"
echo "2. 重启VSCode: Cmd+Shift+P → 'Developer: Reload Window'"
echo "3. 重启clangd: Cmd+Shift+P → 'clangd: Restart language server'"
echo "4. 检查VSCode输出面板中的clangd日志"
