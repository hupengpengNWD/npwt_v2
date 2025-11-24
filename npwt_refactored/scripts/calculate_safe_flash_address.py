#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Flash地址安全计算工具

功能：
1. 从map文件中自动提取程序代码的最大结束地址
2. 计算安全的Flash存储地址
3. 验证地址是否在安全范围内

使用方法：
    python3 calculate_safe_flash_address.py [map_file_path] [safety_margin_kb]

参数：
    map_file_path: map文件路径（可选，默认查找最新编译的map文件）
    safety_margin_kb: 安全边距（KB，可选，默认2KB）

示例：
    python3 calculate_safe_flash_address.py
    python3 calculate_safe_flash_address.py NWPT_VR.X/dist/default/production/NWPT_VR.X.production.map 2
"""

import sys
import os
import re
import glob
from pathlib import Path

# 默认配置
DEFAULT_SAFETY_MARGIN_KB = 2  # 默认安全边距2KB
FLASH_BLOCK_SIZE = 64  # Flash块大小（字节）
MIN_SAFETY_MARGIN = 64  # 最小安全边距（字节）

# Flash地址范围限制（从map文件分析得出）
UNUSED_CODE_START = 0xC5BC  # 未使用CODE区域开始地址
UNUSED_CODE_END = 0xE13D    # 未使用CODE区域结束地址
CONST_DATA_START = 0xE13E   # 常量数据开始地址


def find_latest_map_file():
    """查找最新的map文件"""
    # 在当前目录及子目录中查找map文件
    map_files = []
    for root, dirs, files in os.walk('.'):
        for file in files:
            if file.endswith('.map') and 'production' in root:
                map_path = os.path.join(root, file)
                map_files.append((map_path, os.path.getmtime(map_path)))
    
    if not map_files:
        return None
    
    # 返回最新的map文件
    map_files.sort(key=lambda x: x[1], reverse=True)
    return map_files[0][0]


def parse_map_file(map_file_path):
    """解析map文件，提取CODE段信息"""
    code_segments = []
    
    try:
        with open(map_file_path, 'r', encoding='latin-1') as f:
            lines = f.readlines()
    except Exception as e:
        print(f"错误：无法读取map文件 {map_file_path}: {e}")
        return None
    
    # 查找CODE段信息
    # 格式示例：text59                         00C4E2  0000DA  00C5BC      6271       0  CODE
    code_pattern = re.compile(r'^\s+(\w+)\s+([0-9A-Fa-f]{6})\s+([0-9A-Fa-f]{6})\s+([0-9A-Fa-f]{6})', re.IGNORECASE)
    
    for line in lines:
        if 'CODE' in line and not 'CLASS' in line and not 'UNUSED' in line:
            match = code_pattern.match(line)
            if match:
                segment_name = match.group(1)
                start_addr = int(match.group(2), 16)
                length = int(match.group(3), 16)
                end_addr = int(match.group(4), 16)
                
                # 计算实际结束地址（end_addr是起始地址+长度，所以实际结束地址是end_addr-1）
                actual_end = end_addr - 1
                
                code_segments.append({
                    'name': segment_name,
                    'start': start_addr,
                    'length': length,
                    'end': end_addr,
                    'actual_end': actual_end
                })
    
    return code_segments


def find_max_code_end(code_segments):
    """找到最大的CODE段结束地址"""
    if not code_segments:
        return None
    
    max_end = 0
    max_segment = None
    
    for segment in code_segments:
        if segment['actual_end'] > max_end:
            max_end = segment['actual_end']
            max_segment = segment
    
    return max_end, max_segment


def calculate_safe_address(max_code_end, safety_margin_bytes):
    """计算安全的Flash地址"""
    # 计算目标地址（最大代码结束地址 + 安全边距）
    target_address = max_code_end + safety_margin_bytes
    
    # 向上对齐到64字节边界
    safe_address = ((target_address + FLASH_BLOCK_SIZE - 1) // FLASH_BLOCK_SIZE) * FLASH_BLOCK_SIZE
    
    return safe_address


def validate_address(address):
    """验证地址是否在安全范围内"""
    issues = []
    
    if address < UNUSED_CODE_START:
        issues.append(f"地址0x{address:X}在未使用CODE区域开始地址0x{UNUSED_CODE_START:X}之前")
    
    if address > UNUSED_CODE_END:
        issues.append(f"地址0x{address:X}在未使用CODE区域结束地址0x{UNUSED_CODE_END:X}之后")
    
    if address >= CONST_DATA_START:
        issues.append(f"地址0x{address:X}在常量数据开始地址0x{CONST_DATA_START:X}之后或等于")
    
    if address % FLASH_BLOCK_SIZE != 0:
        issues.append(f"地址0x{address:X}不是64字节对齐（必须是{FLASH_BLOCK_SIZE}的倍数）")
    
    return len(issues) == 0, issues


def main():
    # 解析命令行参数
    map_file_path = None
    safety_margin_kb = DEFAULT_SAFETY_MARGIN_KB
    
    if len(sys.argv) > 1:
        map_file_path = sys.argv[1]
    else:
        map_file_path = find_latest_map_file()
        if not map_file_path:
            print("错误：未找到map文件，请指定map文件路径")
            print(f"使用方法: {sys.argv[0]} [map_file_path] [safety_margin_kb]")
            return 1
    
    if len(sys.argv) > 2:
        try:
            safety_margin_kb = float(sys.argv[2])
        except ValueError:
            print(f"错误：无效的安全边距值 '{sys.argv[2]}'")
            return 1
    
    safety_margin_bytes = int(safety_margin_kb * 1024)
    
    # 确保安全边距不小于最小值
    if safety_margin_bytes < MIN_SAFETY_MARGIN:
        safety_margin_bytes = MIN_SAFETY_MARGIN
        print(f"警告：安全边距小于最小值，已调整为{MIN_SAFETY_MARGIN}字节")
    
    print("=" * 70)
    print("Flash地址安全计算工具")
    print("=" * 70)
    print()
    print(f"Map文件: {map_file_path}")
    print(f"安全边距: {safety_margin_kb}KB ({safety_margin_bytes}字节)")
    print()
    
    # 解析map文件
    print("正在解析map文件...")
    code_segments = parse_map_file(map_file_path)
    
    if not code_segments:
        print("错误：无法从map文件中提取CODE段信息")
        return 1
    
    # 找到最大的CODE段结束地址
    max_code_end, max_segment = find_max_code_end(code_segments)
    
    if max_code_end is None:
        print("错误：无法找到CODE段结束地址")
        return 1
    
    print(f"找到最大CODE段: {max_segment['name']}")
    print(f"  起始地址: 0x{max_segment['start']:X}")
    print(f"  结束地址: 0x{max_segment['end']:X}")
    print(f"  实际结束地址: 0x{max_code_end:X}")
    print()
    
    # 计算安全地址
    safe_address = calculate_safe_address(max_code_end, safety_margin_bytes)
    
    print("计算结果：")
    print(f"  程序代码最大结束地址: 0x{max_code_end:X}")
    print(f"  安全边距: {safety_margin_bytes}字节 ({safety_margin_kb}KB)")
    print(f"  计算出的安全地址: 0x{safe_address:X}")
    print()
    
    # 验证地址
    is_valid, issues = validate_address(safe_address)
    
    if is_valid:
        print("✓ 地址验证通过")
        print()
        print("推荐配置：")
        print(f"  #define HAL_FLASH_CONFIG_ADDRESS    0x{safe_address:X}U")
        print()
        print("地址信息：")
        print(f"  - 距离程序代码结束: {safe_address - max_code_end} 字节 ({round((safe_address - max_code_end)/1024, 2)}KB)")
        print(f"  - 距离常量数据开始: {CONST_DATA_START - safe_address} 字节 ({round((CONST_DATA_START - safe_address)/1024, 2)}KB)")
        print(f"  - 可用空间: {UNUSED_CODE_END - safe_address + 1} 字节 ({round((UNUSED_CODE_END - safe_address + 1)/1024, 2)}KB)")
    else:
        print("✗ 地址验证失败：")
        for issue in issues:
            print(f"  - {issue}")
        print()
        print("建议：")
        if safe_address < UNUSED_CODE_START:
            print(f"  - 地址太小，建议使用最小安全地址: 0x{UNUSED_CODE_START:X}")
        elif safe_address > UNUSED_CODE_END:
            print(f"  - 地址超出未使用CODE区域，建议减小安全边距或检查代码大小")
        elif safe_address >= CONST_DATA_START:
            print(f"  - 地址在常量数据区域内，建议减小安全边距")
    
    print()
    print("=" * 70)
    
    return 0 if is_valid else 1


if __name__ == '__main__':
    sys.exit(main())

