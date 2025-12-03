# 🛠️ DAPLink/CMSIS-DAP 调试配置指南

> **适用工具**: DAPLink, CMSIS-DAP, STLink V2/V3
> **IDE**: Keil MDK-ARM (μVision5)
> **芯片**: STM32F407VET6
> **最后更新**: 2025-12-03

---

## 📌 **为什么用DAP代替USB转TTL？**

### **优势对比：**

| 功能 | USB转TTL | DAPLink (SWO) |
|------|----------|---------------|
| **调试功能** | ❌ 无 | ✅ 单步调试、断点、变量查看 |
| **printf输出** | ✅ 有 | ✅ 有（通过ITM） |
| **占用UART** | ✅ 占用UART1 | ❌ 不占用任何UART |
| **输出速度** | 慢（115200bps） | 快（2Mbps+） |
| **硬件成本** | ~5元 | ~15元（DAPLink） |
| **与ASR-PRO冲突** | ✅ 冲突（共用UART1）| ❌ 不冲突 |

**结论：用DAP调试更专业、更灵活！**

---

## 🔌 **硬件连接**

### **DAPLink/STLink 与 STM32F407 连接：**

```
DAPLink/STLink              STM32F407VET6开发板
┌─────────────────┐         ┌──────────────────┐
│                 │         │                  │
│  SWDIO ●───────────────→●  PA13 (SWDIO)     │
│  SWCLK ●───────────────→●  PA14 (SWCLK)     │
│  GND   ●───────────────→●  GND              │
│  3.3V  ●───────────────→●  3.3V (可选供电)  │
│                 │         │                  │
│  [USB口]        │         │  [USB口]         │
│  连接电脑       │         │  供电用          │
└─────────────────┘         └──────────────────┘
         ↓                           ↓
     Keil调试器                  目标板供电

⚠️ 注意：
- SWDIO/SWCLK 不能接错！
- GND 必须连接（共地）
- 3.3V可选：如果STM32用USB供电就不需要
```

### **实物连接图：**

```
桌面布局：

[💻 电脑]
   │
   └─ USB线 ─→ [DAPLink调试器]
                   │
                   └─ 4根杜邦线 ─→ [STM32F407开发板]
                                     │
                                     └─ USB线供电

所有传感器照常连接到STM32，不受影响！
```

---

## ⚙️ **Keil MDK 配置步骤**

### **第一步：选择调试器**

1. 打开Keil MDK，加载项目：
   ```
   F:\RT\smart_helmet_f407\MDK-ARM\smart_helmet_f407.uvprojx
   ```

2. 点击菜单：**Project → Options for Target 'smart_helmet_f407'**
   - 快捷键：`Alt + F7`

3. 切换到 **Debug** 标签页

4. 在右侧"Use:"下拉菜单中选择：
   ```
   CMSIS-DAP Debugger   (如果用DAPLink)
   或
   ST-Link Debugger     (如果用STLink)
   ```

5. 点击右侧的 **Settings** 按钮

---

### **第二步：配置Debug Settings**

**在弹出的"CMSIS-DAP Configuration"窗口中：**

#### **1. Debug 标签页：**
```
□ Port: SW (不是JTAG！)
□ Max Clock: 10MHz (默认即可)
□ Connect: with Pre-reset (推荐)
□ Reset: System Reset (推荐)

✅ 确保能检测到芯片：
   SW Device → 应该显示 "ARM CoreSight SW-DP"
```

#### **2. Flash Download 标签页：**
```
✅ Download Function:
   ☑ Erase Full Chip (首次烧录时勾选)
   ☑ Program
   ☑ Verify (推荐)
   ☐ Reset and Run (如果想烧录后自动运行就勾选)

✅ Programming Algorithm:
   应该显示：
   STM32F4xx Flash  (0x08000000, 0x80000)

   如果没有，点击"Add"添加：
   Device: STM32F407xE
   Algorithm: STM32F4xx 512kB Flash
```

#### **3. Trace 标签页（⭐ 关键配置！）：**
```
✅ Core Clock: 168 MHz (STM32F407的主频)

✅ Trace Enable (勾选！)
   ☑ Use: Core Clock: 168 MHz
   ☑ Trace Port: SWO (Serial Wire Output)
   ☑ ITM Stimulus Port:
      ☑ Port 0 (必须勾选！printf输出在Port 0)
```

**配置示意图：**
```
┌────────────────────────────────────────────┐
│ Trace                                      │
├────────────────────────────────────────────┤
│  ☑ Trace Enable                            │
│     Core Clock: [168] MHz                  │
│     ○ Use:  ● Core Clock: 168 MHz          │
│            ○ Trace Clock: Prescaler: 1     │
│     ☑ Trace Port:  ● SWO                   │
│                    ○ TracePort             │
│                                            │
│  ITM Stimulus Ports:                       │
│  ☑ 0   ☐ 1   ☐ 2   ☐ 3   ☐ 4   ☐ 5       │
│  ☐ 6   ☐ 7   ☐ 8   ☐ 9   ☐ 10  ☐ 11      │
│  ...                                       │
└────────────────────────────────────────────┘
```

#### **4. 点击"OK"保存配置**

---

### **第三步：查看ITM输出窗口**

1. 点击Keil菜单：**View → Serial Windows → Debug (printf) Viewer**
   - 这个窗口会显示所有的printf输出！

2. 或者：**View → Trace → ITM**
   - 这是更详细的ITM跟踪窗口

---

### **第四步：编译并下载程序**

1. 编译项目：
   ```
   快捷键: F7
   或点击: Project → Build Target
   ```

2. 下载到芯片：
   ```
   快捷键: F8
   或点击: Flash → Download
   ```

3. 进入调试模式：
   ```
   快捷键: Ctrl + F5
   或点击: Debug → Start/Stop Debug Session
   ```

---

## 🎯 **测试ITM输出**

### **测试代码（已经在main.c中）：**

```c
#include <stdio.h>

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART1_UART_Init();
    MX_USART2_UART_Init();
    MX_USART3_UART_Init();
    MX_ADC1_Init();
    // ... 其他初始化

    printf("=================================\r\n");
    printf("  STM32智能安全帽系统启动\r\n");
    printf("  固件版本: v1.0.0\r\n");
    printf("  编译时间: %s %s\r\n", __DATE__, __TIME__);
    printf("=================================\r\n");

    // 初始化各个模块
    MAX30102_Init();
    MQ2_Init();
    GPS_Init();
    ESP01S_Init();
    ASR_Init();

    printf("系统初始化完成，开始运行...\r\n");

    while(1)
    {
        scheduler_run();  // 运行调度器
        HAL_Delay(1);
    }
}
```

### **预期输出（在ITM窗口中）：**

```
=================================
  STM32智能安全帽系统启动
  固件版本: v1.0.0
  编译时间: Dec  3 2025 10:30:45
=================================
MAX30102: Init Success
MQ2: Init Success, ADC=1234
GPS: Init Success
ESP01S: Init Success
ASR-PRO: Init Success
系统初始化完成，开始运行...
MAX30102: HR=75bpm, SpO2=98%
MQ2: Gas=52ppm
GPS: Waiting for fix...
...
```

---

## 🐛 **常见问题排查**

### **问题1：ITM窗口没有输出**

**可能原因1：Core Clock配置错误**
```
解决方法：
1. 打开Debug Settings → Trace标签页
2. 确认Core Clock = 168 MHz（STM32F407的主频）
3. 点击OK保存，重新启动调试
```

**可能原因2：ITM Port 0没有勾选**
```
解决方法：
1. 打开Debug Settings → Trace标签页
2. 在ITM Stimulus Ports中勾选"Port 0"
3. 点击OK保存
```

**可能原因3：没有开启Trace**
```
解决方法：
1. 打开Debug Settings → Trace标签页
2. 勾选"Trace Enable"
3. 选择"SWO"模式
```

**可能原因4：代码优化导致printf被删除**
```
解决方法：
在main.c中添加：
#pragma import(__use_no_semihosting)
```

---

### **问题2：无法连接调试器**

**症状：**
```
Error: Flash Download failed - "Cortex-M4"
或
Cannot connect to target
```

**解决方法：**

1. **检查硬件连接：**
   - SWDIO、SWCLK、GND是否接好？
   - 用万用表测量STM32的3.3V引脚是否有电压？

2. **检查Keil配置：**
   - Debug设置中Port选择"SW"（不是JTAG）
   - 尝试降低时钟频率：Max Clock改为1MHz

3. **复位芯片：**
   - 按STM32开发板上的复位键
   - 在Keil中点击"Connect" with "Pre-reset"

4. **检查驱动：**
   - DAPLink/STLink的USB驱动是否正确安装？
   - 在设备管理器中应该看到"CMSIS-DAP"或"STLink"设备

---

### **问题3：printf输出乱码**

**原因：** Core Clock频率配置与实际不符

**解决方法：**
```c
// 在main.c的SystemClock_Config()函数中检查：
RCC_OscInitStruct.PLL.PLLN = 336;
RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;  // 168MHz
```

确保Keil Trace配置中的Core Clock = 168MHz

---

### **问题4：进入调试模式后程序不运行**

**解决方法：**
1. 进入Debug模式后，点击"Run"按钮（F5）
2. 或者在Debug Settings中勾选"Reset and Run"

---

## 🎨 **Keil窗口布局建议**

### **推荐的调试窗口布局：**

```
┌─────────────────────────────────────────────────────────┐
│  Keil μVision5                                          │
├─────────────────────────────────────────────────────────┤
│  [工具栏]  Build | Download | Debug | Run | Stop       │
├──────────────────────────┬──────────────────────────────┤
│  代码编辑器窗口           │  Watch窗口                   │
│  (显示main.c)            │  (监视变量)                  │
│                          │  - heart_rate               │
│  int main(void) {        │  - spo2                     │
│    ...                   │  - mq2_value                │
│  }                       │  - gps_latitude             │
│                          ├──────────────────────────────┤
│                          │  Call Stack窗口              │
│                          │  (函数调用栈)                │
├──────────────────────────┼──────────────────────────────┤
│  Debug (printf) Viewer   │  Command窗口                 │
│  (ITM输出窗口)           │  (输入调试命令)              │
│                          │                              │
│  MAX30102: HR=75bpm      │  > load                      │
│  MQ2: Gas=52ppm          │  > reset                     │
│  GPS: Waiting...         │                              │
└──────────────────────────┴──────────────────────────────┘
```

### **如何调整窗口布局：**
1. 进入Debug模式（Ctrl+F5）
2. 点击View菜单
3. 打开需要的窗口：
   - **View → Serial Windows → Debug (printf) Viewer** (查看printf)
   - **View → Watch Windows → Watch 1** (监视变量)
   - **View → Call Stack Window** (查看函数调用)
4. 拖动窗口到合适位置，Keil会自动记住布局

---

## ✅ **配置完成检查清单**

```
硬件连接：
□ DAPLink/STLink已连接到电脑（USB）
□ SWDIO → PA13 ✓
□ SWCLK → PA14 ✓
□ GND → GND ✓
□ STM32板子已上电（USB或外部电源）

Keil配置：
□ 选择了正确的调试器（CMSIS-DAP或STLink）✓
□ Port选择"SW" ✓
□ Core Clock设置为168MHz ✓
□ Trace Enable已勾选 ✓
□ SWO模式已选择 ✓
□ ITM Stimulus Port 0已勾选 ✓
□ Flash Download配置正确 ✓

代码配置：
□ usart.c中已添加fputc函数（ITM重定向）✓
□ main.c中有printf语句 ✓
□ 项目编译无错误 ✓

测试：
□ 能够下载程序到芯片 ✓
□ 进入Debug模式成功 ✓
□ ITM窗口能看到printf输出 ✓
□ 可以设置断点并单步调试 ✓
```

---

## 🎓 **调试技巧**

### **1. 单步调试：**
```
F10: Step Over  (单步执行，不进入函数)
F11: Step Into  (单步执行，进入函数内部)
Shift+F11: Step Out (跳出当前函数)
F5: Run         (运行到下一个断点)
```

### **2. 设置断点：**
```
方法1: 在代码行号左侧单击鼠标左键
方法2: 光标定位到某行，按F9
```

### **3. 监视变量：**
```
1. 在代码中选中变量名
2. 右键 → Add 'variable_name' to...
3. 选择 → Watch 1
```

### **4. 实时查看传感器数据：**
```c
// 在Watch窗口中添加这些变量：
extern uint8_t heart_rate;
extern uint8_t spo2;
extern uint16_t mq2_value;
extern float gps_latitude;
extern float gps_longitude;
```

程序运行时，Watch窗口会实时更新这些值！

---

## 📚 **参考资料**

- [ARM CMSIS-DAP 官方文档](https://arm-software.github.io/CMSIS_5/DAP/html/index.html)
- [Keil ITM 使用指南](http://www.keil.com/support/man/docs/uv4/)
- [STM32F407 参考手册](https://www.st.com/resource/en/reference_manual/dm00031020.pdf)

---

**📅 文档创建**: 2025-12-03
**🔧 适用项目**: STM32智能安全帽
**📧 问题反馈**: 调试有问题随时告诉我！

---

**🎉 配置完成后，你就可以：**
- ✅ 使用printf输出调试信息（不占用UART1）
- ✅ 单步调试代码，查看变量
- ✅ 设置断点，追踪程序流程
- ✅ 实时监控传感器数据
- ✅ ASR-PRO可以独占UART1，不再冲突

**祝调试顺利！有问题随时找我！** 🚀
