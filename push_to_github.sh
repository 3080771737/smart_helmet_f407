#!/bin/bash
# STM32智能安全帽项目 - GitHub推送脚本
# 使用方法: bash push_to_github.sh <你的GitHub用户名>

echo "========================================="
echo "GitHub推送脚本"
echo "========================================="
echo ""

# 检查参数
if [ -z "$1" ]; then
    echo "❌ 错误：请提供GitHub用户名"
    echo ""
    echo "使用方法:"
    echo "  bash push_to_github.sh <你的GitHub用户名>"
    echo ""
    echo "示例:"
    echo "  bash push_to_github.sh zhangsan"
    echo ""
    exit 1
fi

GITHUB_USER=$1
REPO_NAME="smart_helmet_f407"
REMOTE_URL="https://github.com/${GITHUB_USER}/${REPO_NAME}.git"

echo "📋 配置信息:"
echo "  GitHub用户名: $GITHUB_USER"
echo "  仓库名称: $REPO_NAME"
echo "  远程地址: $REMOTE_URL"
echo ""

# 检查是否已有remote
if git remote | grep -q "^origin$"; then
    echo "⚠️  检测到已存在remote 'origin'"
    echo "   当前remote地址:"
    git remote get-url origin
    echo ""
    read -p "是否要删除并重新设置？(y/n): " confirm
    if [ "$confirm" = "y" ]; then
        git remote remove origin
        echo "✅ 已删除旧的remote"
    else
        echo "❌ 取消操作"
        exit 1
    fi
fi

# 添加remote
echo ""
echo "🔗 添加远程仓库..."
git remote add origin $REMOTE_URL
if [ $? -eq 0 ]; then
    echo "✅ 远程仓库添加成功"
else
    echo "❌ 远程仓库添加失败"
    exit 1
fi

# 显示remote信息
echo ""
echo "📡 当前remote配置:"
git remote -v

# 推送提示
echo ""
echo "========================================="
echo "准备推送到GitHub"
echo "========================================="
echo ""
echo "⚠️  推送前请确认:"
echo "  1. 已在GitHub创建仓库: $REPO_NAME"
echo "  2. 仓库设置为Public或Private"
echo "  3. 准备好GitHub凭据（用户名+密码或Token）"
echo ""
read -p "确认推送？(y/n): " confirm

if [ "$confirm" != "y" ]; then
    echo "❌ 取消推送"
    exit 1
fi

# 推送到GitHub
echo ""
echo "🚀 开始推送..."
git branch -M master
git push -u origin master

if [ $? -eq 0 ]; then
    echo ""
    echo "========================================="
    echo "✅ 推送成功！"
    echo "========================================="
    echo ""
    echo "🎉 你的项目已经在GitHub上了！"
    echo ""
    echo "📍 仓库地址:"
    echo "   https://github.com/${GITHUB_USER}/${REPO_NAME}"
    echo ""
    echo "📊 推送统计:"
    git log --oneline | wc -l
    echo "   次提交已推送"
    echo ""
    echo "🔗 下一步建议:"
    echo "  1. 访问GitHub仓库查看"
    echo "  2. 添加仓库描述"
    echo "  3. 设置Topics标签（stm32, iot, embedded等）"
    echo "  4. 添加演示图片到demo/文件夹"
    echo ""
else
    echo ""
    echo "❌ 推送失败"
    echo ""
    echo "常见错误解决:"
    echo "  1. 仓库不存在 → 先在GitHub创建仓库"
    echo "  2. 认证失败 → 检查用户名密码或Token"
    echo "  3. 网络问题 → 检查网络连接"
    echo ""
    exit 1
fi
