// See LICENSE for license details.

#ifndef SIFIVE_GPIO_H
#define SIFIVE_GPIO_H

#define GPIO_value      (0x00)
#define GPIO_input_en   (0x04)
#define GPIO_output_en  (0x08)
#define GPIO_port       (0x0C)
#define GPIO_pullup_en  (0x10)
#define GPIO_drive      (0x14)
#define GPIO_rise_ie    (0x18)
#define GPIO_rise_ip    (0x1C)
#define GPIO_fall_ie    (0x20)
#define GPIO_fall_ip    (0x24)
#define GPIO_high_ie    (0x28)
#define GPIO_high_ip    (0x2C)
#define GPIO_low_ie     (0x30)
#define GPIO_low_ip     (0x34)
#define GPIO_iof_en     (0x38)
#define GPIO_iof_sel    (0x3C)
#define GPIO_out_xor    (0x40)
#endif
