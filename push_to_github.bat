@echo off
REM STM32智能安全帽项目 - GitHub推送脚本（Windows版）
REM 使用方法: push_to_github.bat <你的GitHub用户名>

echo =========================================
echo GitHub推送脚本 (Windows)
echo =========================================
echo.

REM 检查参数
if "%1"=="" (
    echo 错误：请提供GitHub用户名
    echo.
    echo 使用方法:
    echo   push_to_github.bat ^<你的GitHub用户名^>
    echo.
    echo 示例:
    echo   push_to_github.bat zhangsan
    echo.
    pause
    exit /b 1
)

set GITHUB_USER=%1
set REPO_NAME=smart_helmet_f407
set REMOTE_URL=https://github.com/%GITHUB_USER%/%REPO_NAME%.git

echo 配置信息:
echo   GitHub用户名: %GITHUB_USER%
echo   仓库名称: %REPO_NAME%
echo   远程地址: %REMOTE_URL%
echo.

REM 进入项目目录
cd /d F:\RT\smart_helmet_f407

REM 添加remote
echo 添加远程仓库...
git remote add origin %REMOTE_URL% 2>nul
if errorlevel 1 (
    echo 检测到已存在remote，尝试删除...
    git remote remove origin
    git remote add origin %REMOTE_URL%
)
echo 远程仓库添加成功
echo.

REM 显示remote信息
echo 当前remote配置:
git remote -v
echo.

echo =========================================
echo 准备推送到GitHub
echo =========================================
echo.
echo 推送前请确认:
echo   1. 已在GitHub创建仓库: %REPO_NAME%
echo   2. 仓库设置为Public或Private
echo   3. 准备好GitHub凭据
echo.
set /p confirm=确认推送？(y/n): 

if /i not "%confirm%"=="y" (
    echo 取消推送
    pause
    exit /b 1
)

REM 推送到GitHub
echo.
echo 开始推送...
git branch -M master
git push -u origin master

if errorlevel 1 (
    echo.
    echo =========================================
    echo 推送失败
    echo =========================================
    echo.
    echo 常见错误解决:
    echo   1. 仓库不存在 -^> 先在GitHub创建仓库
    echo   2. 认证失败 -^> 检查用户名密码或Token
    echo   3. 网络问题 -^> 检查网络连接
    echo.
    pause
    exit /b 1
) else (
    echo.
    echo =========================================
    echo 推送成功！
    echo =========================================
    echo.
    echo 你的项目已经在GitHub上了！
    echo.
    echo 仓库地址:
    echo   https://github.com/%GITHUB_USER%/%REPO_NAME%
    echo.
    echo 下一步建议:
    echo   1. 访问GitHub仓库查看
    echo   2. 添加仓库描述
    echo   3. 设置Topics标签
    echo   4. 添加演示图片
    echo.
    pause
)
