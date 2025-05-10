# IAP
基于stm32F407+FreeRTOS+LWIP+HAL库实现的IAP系统<br/>
通讯端口为以太网和串口<br/>
通讯自定义协议,两字节命令、两字节总长度、256字节数据区、四字节保留、2字节CRC16校验<br/>
Flash划分区域包括boot区和APP区<br/>
为防止flash坏块，采用A/B冗余设计
上位机采用qt
