import sys
import os
import re

def create_files(class_name):
    h_file = f"{class_name}.h"
    cpp_file = f"{class_name}.cpp"
    upper_name = class_name.upper()

    # 自动生成的头文件模板（自带防卫宏和 QGraphicsItem 基础框架）
    h_content = f"""#ifndef {upper_name}_H
#define {upper_name}_H

#include <QGraphicsItem>
#include <QPainter>

class {class_name} : public QGraphicsItem {{
public:
    {class_name}();
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
}};

#endif // {upper_name}_H
"""

    # 自动生成的源文件模板
    cpp_content = f"""#include "{class_name}.h"

{class_name}::{class_name}() {{
    // TODO: 初始化逻辑
    // setFlag(QGraphicsItem::ItemIsSelectable);
}}

QRectF {class_name}::boundingRect() const {{
    // TODO: 返回图元的精确包围盒
    return QRectF(0, 0, 0, 0); 
}}

void {class_name}::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {{
    // TODO: 执行具体的绘制指令
}}
"""

    if not os.path.exists(h_file):
        with open(h_file, 'w', encoding='utf-8') as f:
            f.write(h_content)
        print(f"✅ 成功创建: {h_file}")
    else:
        print(f"⚠️ 文件已存在, 跳过: {h_file}")

    if not os.path.exists(cpp_file):
        with open(cpp_file, 'w', encoding='utf-8') as f:
            f.write(cpp_content)
        print(f"✅ 成功创建: {cpp_file}")
    else:
        print(f"⚠️ 文件已存在, 跳过: {cpp_file}")

def update_cmake(class_name):
    cmake_file = "CMakeLists.txt"
    if not os.path.exists(cmake_file):
        print("❌ 找不到 CMakeLists.txt!")
        return

    with open(cmake_file, 'r', encoding='utf-8') as f:
        content = f.read()

    # 防止重复添加
    if f"{class_name}.h" in content:
        print("⚠️ CMakeLists.txt 中已包含该文件，无需更新。")
        return

    # 使用正则魔法：找到 add_executable 块，把新文件塞到右括号前面
    pattern = re.compile(r'(add_executable\s*\([^)]+)(\))', re.MULTILINE | re.DOTALL)
    match = pattern.search(content)
    
    if match:
        replacement = f"\\1    {class_name}.h\n    {class_name}.cpp\n\\2"
        new_content = pattern.sub(replacement, content, count=1)
        
        with open(cmake_file, 'w', encoding='utf-8') as f:
            f.write(new_content)
        print("✅ CMakeLists.txt 自动更新完毕！")
    else:
        print("❌ 更新 CMake 失败：找不到 add_executable 块。")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("💡 用法: py gen.py <ClassName>")
        print("   例如: py gen.py EdgeItem")
        sys.exit(1)
    
    target_class = sys.argv[1]
    print(f"🚀 开始生成 {target_class} 模块...")
    create_files(target_class)
    update_cmake(target_class)
    print("🎉 自动化流程结束，请在 VS Code 中点击 Build 重新编译！")