# 🚀 如何推送到GitHub

## 📋 前置准备

### 1. 在GitHub创建仓库

1. 登录GitHub: https://github.com
2. 点击右上角 "+" → "New repository"
3. 填写信息：
   - **Repository name**: `smart_helmet_f407`
   - **Description**: `基于STM32F407的智能安全帽监测系统`
   - **Public/Private**: 根据需要选择
   - ⚠️ **不要勾选**: "Add a README file"
   - ⚠️ **不要勾选**: "Add .gitignore"
   - ⚠️ **不要勾选**: "Choose a license"
4. 点击 "Create repository"

### 2. 准备GitHub凭据

**方法1: 使用Personal Access Token（推荐）**

1. GitHub → Settings → Developer settings → Personal access tokens
2. Generate new token (classic)
3. 勾选 `repo` 权限
4. 生成Token并保存（只显示一次！）
5. 推送时用Token替代密码

**方法2: 使用用户名+密码**

- 用户名: 你的GitHub用户名
- 密码: 你的GitHub密码

---

## 🎯 推送方法（三选一）

### 方法1: 使用自动化脚本（最简单）⭐

#### Windows用户：

```cmd
# 双击运行或在CMD中执行
push_to_github.bat <你的GitHub用户名>

# 示例
push_to_github.bat zhangsan
```

#### Git Bash用户：

```bash
bash push_to_github.sh <你的GitHub用户名>

# 示例
bash push_to_github.sh zhangsan
```

---

### 方法2: 手动执行Git命令

```bash
# 1. 进入项目目录
cd /f/RT/smart_helmet_f407

# 2. 添加远程仓库（替换<你的用户名>）
git remote add origin https://github.com/<你的用户名>/smart_helmet_f407.git

# 3. 验证remote
git remote -v

# 4. 推送到GitHub
git branch -M master
git push -u origin master
```

**示例**（假设你的用户名是zhangsan）：
```bash
git remote add origin https://github.com/zhangsan/smart_helmet_f407.git
git push -u origin master
```

---

### 方法3: 使用SSH（适合频繁推送）

#### 前置设置（只需一次）：

```bash
# 1. 生成SSH密钥
ssh-keygen -t ed25519 -C "your_email@example.com"
# 一路回车，使用默认设置

# 2. 查看公钥
cat ~/.ssh/id_ed25519.pub

# 3. 复制公钥内容，添加到GitHub:
#    GitHub → Settings → SSH and GPG keys → New SSH key
```

#### 推送步骤：

```bash
# 1. 添加SSH远程仓库
git remote add origin git@github.com:<你的用户名>/smart_helmet_f407.git

# 2. 推送
git push -u origin master
```

---

## ❓ 常见问题

### Q1: 推送时提示"remote origin already exists"

**解决方案**：
```bash
# 删除已有的remote
git remote remove origin

# 重新添加
git remote add origin https://github.com/<你的用户名>/smart_helmet_f407.git
```

---

### Q2: 推送时提示"Authentication failed"

**原因**: GitHub已不支持密码认证

**解决方案**: 使用Personal Access Token
```bash
# 推送时:
Username: <你的GitHub用户名>
Password: <你的Personal Access Token>（不是GitHub密码！）
```

---

### Q3: 推送时提示"Repository not found"

**原因**: 仓库不存在或名称错误

**解决方案**:
1. 确认已在GitHub创建仓库
2. 检查仓库名称是否为 `smart_helmet_f407`
3. 检查用户名是否正确

---

### Q4: 推送超时或网络错误

**解决方案1**: 使用代理
```bash
git config --global http.proxy http://127.0.0.1:7890
git config --global https.proxy http://127.0.0.1:7890
```

**解决方案2**: 使用Gitee镜像（国内）
```bash
# 同时推送到GitHub和Gitee
git remote set-url --add --push origin https://gitee.com/<你的用户名>/smart_helmet_f407.git
```

---

## ✅ 推送成功后

### 1. 访问仓库

```
https://github.com/<你的用户名>/smart_helmet_f407
```

### 2. 完善仓库信息

#### 添加描述（About）

- 点击仓库右侧 ⚙️ 图标
- Description: `基于STM32F407VET6的工业级智能安全帽监测系统`
- Website: 可以留空或填个人网站

#### 添加Topics标签
点击 "Add topics"，添加以下标签：
- `stm32`
- `stm32f4`
- `iot`
- `embedded`
- `smart-helmet`
- `sensor`
- `mqtt`
- `freertos` (如果后续添加)

#### 设置主页
- Settings → Pages
- Source: Deploy from a branch
- Branch: master, / (root)
- Save

### 3. 添加演示资源

```bash
# 创建demo文件夹
mkdir demo
cd demo

# 添加演示图片（你需要手动复制图片）
# - hardware_overview.jpg  (硬件全貌)
# - sensor_connection.jpg  (传感器连接)
# - serial_output.png      (串口输出截图)
# - huawei_cloud_data.png  (华为云数据)

# 提交
git add demo/
git commit -m "docs: 添加演示图片"
git push
```

### 4. 更新README

在README.md中更新演示章节，替换占位符为实际图片：
```markdown
## 🎬 演示

### 硬件实物
![硬件全貌](demo/hardware_overview.jpg)
![传感器连接](demo/sensor_connection.jpg)

### 串口输出
![串口输出](demo/serial_output.png)

### 华为云数据
![华为云数据](demo/huawei_cloud_data.png)
```

---

## 🔗 后续更新推送

以后有新的修改，只需：

```bash
# 1. 查看修改
git status

# 2. 添加修改
git add .

# 3. 提交
git commit -m "描述你的修改"

# 4. 推送
git push
```

---

## 📞 需要帮助？

- GitHub文档: https://docs.github.com
- Git教程: https://git-scm.com/book/zh/v2
- 本项目文档: `GitHub推送指南.md`

---

**推送成功后，你的项目就在云端了！可以随时随地访问和展示！** 🎉
