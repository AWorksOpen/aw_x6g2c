#include "py/obj.h"
#include "imx1050_pin.h"

const mp_rom_map_elem_t machine_pin_board_pins_locals_dict_table[] = {
  { MP_ROM_QSTR(MP_QSTR_GPIO1_0), MP_ROM_INT(GPIO1_0) },

  { MP_ROM_QSTR(MP_QSTR_GPIO1_1), MP_ROM_INT(GPIO1_1) },

  { MP_ROM_QSTR(MP_QSTR_GPIO1_2), MP_ROM_INT(GPIO1_2) },

  { MP_ROM_QSTR(MP_QSTR_GPIO1_3), MP_ROM_INT(GPIO1_3) },

  { MP_ROM_QSTR(MP_QSTR_GPIO1_4), MP_ROM_INT(GPIO1_4) },

  { MP_ROM_QSTR(MP_QSTR_GPIO1_5), MP_ROM_INT(GPIO1_5) },

  { MP_ROM_QSTR(MP_QSTR_GPIO1_6), MP_ROM_INT(GPIO1_6) },

  { MP_ROM_QSTR(MP_QSTR_GPIO1_7), MP_ROM_INT(GPIO1_7) },

  { MP_ROM_QSTR(MP_QSTR_GPIO1_8), MP_ROM_INT(GPIO1_8) },

  { MP_ROM_QSTR(MP_QSTR_GPIO1_9), MP_ROM_INT(GPIO1_9) },

  { MP_ROM_QSTR(MP_QSTR_GPIO1_10), MP_ROM_INT(GPIO1_10) },

  { MP_ROM_QSTR(MP_QSTR_GPIO1_11), MP_ROM_INT(GPIO1_11) },

  { MP_ROM_QSTR(MP_QSTR_GPIO1_12), MP_ROM_INT(GPIO1_12) },

  { MP_ROM_QSTR(MP_QSTR_GPIO1_13), MP_ROM_INT(GPIO1_13) },

  { MP_ROM_QSTR(MP_QSTR_GPIO1_14), MP_ROM_INT(GPIO1_14) },

  { MP_ROM_QSTR(MP_QSTR_GPIO1_15), MP_ROM_INT(GPIO1_15) },

  { MP_ROM_QSTR(MP_QSTR_GPIO1_16), MP_ROM_INT(GPIO1_16) },

  { MP_ROM_QSTR(MP_QSTR_GPIO1_17), MP_ROM_INT(GPIO1_17) },

  { MP_ROM_QSTR(MP_QSTR_GPIO1_18), MP_ROM_INT(GPIO1_18) },

  { MP_ROM_QSTR(MP_QSTR_GPIO1_19), MP_ROM_INT(GPIO1_19) },

  { MP_ROM_QSTR(MP_QSTR_GPIO1_20), MP_ROM_INT(GPIO1_20) },

  { MP_ROM_QSTR(MP_QSTR_GPIO1_21), MP_ROM_INT(GPIO1_21) },

  { MP_ROM_QSTR(MP_QSTR_GPIO1_22), MP_ROM_INT(GPIO1_22) },

  { MP_ROM_QSTR(MP_QSTR_GPIO1_23), MP_ROM_INT(GPIO1_23) },

  { MP_ROM_QSTR(MP_QSTR_GPIO1_24), MP_ROM_INT(GPIO1_24) },

  { MP_ROM_QSTR(MP_QSTR_GPIO1_25), MP_ROM_INT(GPIO1_25) },

  { MP_ROM_QSTR(MP_QSTR_GPIO1_26), MP_ROM_INT(GPIO1_26) },

  { MP_ROM_QSTR(MP_QSTR_GPIO1_27), MP_ROM_INT(GPIO1_27) },

  { MP_ROM_QSTR(MP_QSTR_GPIO1_28), MP_ROM_INT(GPIO1_28) },

  { MP_ROM_QSTR(MP_QSTR_GPIO1_29), MP_ROM_INT(GPIO1_29) },

  { MP_ROM_QSTR(MP_QSTR_GPIO1_30), MP_ROM_INT(GPIO1_30) },

  { MP_ROM_QSTR(MP_QSTR_GPIO1_31), MP_ROM_INT(GPIO1_31) },

  { MP_ROM_QSTR(MP_QSTR_GPIO2_0), MP_ROM_INT(GPIO2_0) },

  { MP_ROM_QSTR(MP_QSTR_GPIO2_1), MP_ROM_INT(GPIO2_1) },

  { MP_ROM_QSTR(MP_QSTR_GPIO2_2), MP_ROM_INT(GPIO2_2) },

  { MP_ROM_QSTR(MP_QSTR_GPIO2_3), MP_ROM_INT(GPIO2_3) },

  { MP_ROM_QSTR(MP_QSTR_GPIO2_4), MP_ROM_INT(GPIO2_4) },

  { MP_ROM_QSTR(MP_QSTR_GPIO2_5), MP_ROM_INT(GPIO2_5) },

  { MP_ROM_QSTR(MP_QSTR_GPIO2_6), MP_ROM_INT(GPIO2_6) },

  { MP_ROM_QSTR(MP_QSTR_GPIO2_7), MP_ROM_INT(GPIO2_7) },

  { MP_ROM_QSTR(MP_QSTR_GPIO2_8), MP_ROM_INT(GPIO2_8) },

  { MP_ROM_QSTR(MP_QSTR_GPIO2_9), MP_ROM_INT(GPIO2_9) },

  { MP_ROM_QSTR(MP_QSTR_GPIO2_10), MP_ROM_INT(GPIO2_10) },

  { MP_ROM_QSTR(MP_QSTR_GPIO2_11), MP_ROM_INT(GPIO2_11) },

  { MP_ROM_QSTR(MP_QSTR_GPIO2_12), MP_ROM_INT(GPIO2_12) },

  { MP_ROM_QSTR(MP_QSTR_GPIO2_13), MP_ROM_INT(GPIO2_13) },

  { MP_ROM_QSTR(MP_QSTR_GPIO2_14), MP_ROM_INT(GPIO2_14) },

  { MP_ROM_QSTR(MP_QSTR_GPIO2_15), MP_ROM_INT(GPIO2_15) },

  { MP_ROM_QSTR(MP_QSTR_GPIO2_16), MP_ROM_INT(GPIO2_16) },

  { MP_ROM_QSTR(MP_QSTR_GPIO2_17), MP_ROM_INT(GPIO2_17) },

  { MP_ROM_QSTR(MP_QSTR_GPIO2_18), MP_ROM_INT(GPIO2_18) },

  { MP_ROM_QSTR(MP_QSTR_GPIO2_19), MP_ROM_INT(GPIO2_19) },

  { MP_ROM_QSTR(MP_QSTR_GPIO2_20), MP_ROM_INT(GPIO2_20) },

  { MP_ROM_QSTR(MP_QSTR_GPIO2_21), MP_ROM_INT(GPIO2_21) },

  { MP_ROM_QSTR(MP_QSTR_GPIO2_22), MP_ROM_INT(GPIO2_22) },

  { MP_ROM_QSTR(MP_QSTR_GPIO2_23), MP_ROM_INT(GPIO2_23) },

  { MP_ROM_QSTR(MP_QSTR_GPIO2_24), MP_ROM_INT(GPIO2_24) },

  { MP_ROM_QSTR(MP_QSTR_GPIO2_25), MP_ROM_INT(GPIO2_25) },

  { MP_ROM_QSTR(MP_QSTR_GPIO2_26), MP_ROM_INT(GPIO2_26) },

  { MP_ROM_QSTR(MP_QSTR_GPIO2_27), MP_ROM_INT(GPIO2_27) },

  { MP_ROM_QSTR(MP_QSTR_GPIO2_28), MP_ROM_INT(GPIO2_28) },

  { MP_ROM_QSTR(MP_QSTR_GPIO2_29), MP_ROM_INT(GPIO2_29) },

  { MP_ROM_QSTR(MP_QSTR_GPIO2_30), MP_ROM_INT(GPIO2_30) },

  { MP_ROM_QSTR(MP_QSTR_GPIO2_31), MP_ROM_INT(GPIO2_31) },

  { MP_ROM_QSTR(MP_QSTR_GPIO3_0), MP_ROM_INT(GPIO3_0) },

  { MP_ROM_QSTR(MP_QSTR_GPIO3_1), MP_ROM_INT(GPIO3_1) },

  { MP_ROM_QSTR(MP_QSTR_GPIO3_2), MP_ROM_INT(GPIO3_2) },

  { MP_ROM_QSTR(MP_QSTR_GPIO3_3), MP_ROM_INT(GPIO3_3) },

  { MP_ROM_QSTR(MP_QSTR_GPIO3_4), MP_ROM_INT(GPIO3_4) },

  { MP_ROM_QSTR(MP_QSTR_GPIO3_5), MP_ROM_INT(GPIO3_5) },

  { MP_ROM_QSTR(MP_QSTR_GPIO3_6), MP_ROM_INT(GPIO3_6) },

  { MP_ROM_QSTR(MP_QSTR_GPIO3_7), MP_ROM_INT(GPIO3_7) },

  { MP_ROM_QSTR(MP_QSTR_GPIO3_8), MP_ROM_INT(GPIO3_8) },

  { MP_ROM_QSTR(MP_QSTR_GPIO3_9), MP_ROM_INT(GPIO3_9) },

  { MP_ROM_QSTR(MP_QSTR_GPIO3_10), MP_ROM_INT(GPIO3_10) },

  { MP_ROM_QSTR(MP_QSTR_GPIO3_11), MP_ROM_INT(GPIO3_11) },

  { MP_ROM_QSTR(MP_QSTR_GPIO3_12), MP_ROM_INT(GPIO3_12) },

  { MP_ROM_QSTR(MP_QSTR_GPIO3_13), MP_ROM_INT(GPIO3_13) },

  { MP_ROM_QSTR(MP_QSTR_GPIO3_14), MP_ROM_INT(GPIO3_14) },

  { MP_ROM_QSTR(MP_QSTR_GPIO3_15), MP_ROM_INT(GPIO3_15) },

  { MP_ROM_QSTR(MP_QSTR_GPIO3_16), MP_ROM_INT(GPIO3_16) },

  { MP_ROM_QSTR(MP_QSTR_GPIO3_17), MP_ROM_INT(GPIO3_17) },

  { MP_ROM_QSTR(MP_QSTR_GPIO3_18), MP_ROM_INT(GPIO3_18) },

  { MP_ROM_QSTR(MP_QSTR_GPIO3_19), MP_ROM_INT(GPIO3_19) },

  { MP_ROM_QSTR(MP_QSTR_GPIO3_20), MP_ROM_INT(GPIO3_20) },

  { MP_ROM_QSTR(MP_QSTR_GPIO3_21), MP_ROM_INT(GPIO3_21) },

  { MP_ROM_QSTR(MP_QSTR_GPIO3_22), MP_ROM_INT(GPIO3_22) },

  { MP_ROM_QSTR(MP_QSTR_GPIO3_23), MP_ROM_INT(GPIO3_23) },

  { MP_ROM_QSTR(MP_QSTR_GPIO3_24), MP_ROM_INT(GPIO3_24) },

  { MP_ROM_QSTR(MP_QSTR_GPIO3_25), MP_ROM_INT(GPIO3_25) },

  { MP_ROM_QSTR(MP_QSTR_GPIO3_26), MP_ROM_INT(GPIO3_26) },

  { MP_ROM_QSTR(MP_QSTR_GPIO3_27), MP_ROM_INT(GPIO3_27) },

  { MP_ROM_QSTR(MP_QSTR_GPIO3_28), MP_ROM_INT(GPIO3_28) },

  { MP_ROM_QSTR(MP_QSTR_GPIO3_29), MP_ROM_INT(GPIO3_29) },

  { MP_ROM_QSTR(MP_QSTR_GPIO3_30), MP_ROM_INT(GPIO3_30) },

  { MP_ROM_QSTR(MP_QSTR_GPIO3_31), MP_ROM_INT(GPIO3_31) },

  { MP_ROM_QSTR(MP_QSTR_GPIO4_0), MP_ROM_INT(GPIO4_0) },

  { MP_ROM_QSTR(MP_QSTR_GPIO4_1), MP_ROM_INT(GPIO4_1) },

  { MP_ROM_QSTR(MP_QSTR_GPIO4_2), MP_ROM_INT(GPIO4_2) },

  { MP_ROM_QSTR(MP_QSTR_GPIO4_3), MP_ROM_INT(GPIO4_3) },

  { MP_ROM_QSTR(MP_QSTR_GPIO4_4), MP_ROM_INT(GPIO4_4) },

  { MP_ROM_QSTR(MP_QSTR_GPIO4_5), MP_ROM_INT(GPIO4_5) },

  { MP_ROM_QSTR(MP_QSTR_GPIO4_6), MP_ROM_INT(GPIO4_6) },

  { MP_ROM_QSTR(MP_QSTR_GPIO4_7), MP_ROM_INT(GPIO4_7) },

  { MP_ROM_QSTR(MP_QSTR_GPIO4_8), MP_ROM_INT(GPIO4_8) },

  { MP_ROM_QSTR(MP_QSTR_GPIO4_9), MP_ROM_INT(GPIO4_9) },

  { MP_ROM_QSTR(MP_QSTR_GPIO4_10), MP_ROM_INT(GPIO4_10) },

  { MP_ROM_QSTR(MP_QSTR_GPIO4_11), MP_ROM_INT(GPIO4_11) },

  { MP_ROM_QSTR(MP_QSTR_GPIO4_12), MP_ROM_INT(GPIO4_12) },

  { MP_ROM_QSTR(MP_QSTR_GPIO4_13), MP_ROM_INT(GPIO4_13) },

  { MP_ROM_QSTR(MP_QSTR_GPIO4_14), MP_ROM_INT(GPIO4_14) },

  { MP_ROM_QSTR(MP_QSTR_GPIO4_15), MP_ROM_INT(GPIO4_15) },

  { MP_ROM_QSTR(MP_QSTR_GPIO4_16), MP_ROM_INT(GPIO4_16) },

  { MP_ROM_QSTR(MP_QSTR_GPIO4_17), MP_ROM_INT(GPIO4_17) },

  { MP_ROM_QSTR(MP_QSTR_GPIO4_18), MP_ROM_INT(GPIO4_18) },

  { MP_ROM_QSTR(MP_QSTR_GPIO4_19), MP_ROM_INT(GPIO4_19) },

  { MP_ROM_QSTR(MP_QSTR_GPIO4_20), MP_ROM_INT(GPIO4_20) },

  { MP_ROM_QSTR(MP_QSTR_GPIO4_21), MP_ROM_INT(GPIO4_21) },

  { MP_ROM_QSTR(MP_QSTR_GPIO4_22), MP_ROM_INT(GPIO4_22) },

  { MP_ROM_QSTR(MP_QSTR_GPIO4_23), MP_ROM_INT(GPIO4_23) },

  { MP_ROM_QSTR(MP_QSTR_GPIO4_24), MP_ROM_INT(GPIO4_24) },

  { MP_ROM_QSTR(MP_QSTR_GPIO4_25), MP_ROM_INT(GPIO4_25) },

  { MP_ROM_QSTR(MP_QSTR_GPIO4_26), MP_ROM_INT(GPIO4_26) },

  { MP_ROM_QSTR(MP_QSTR_GPIO4_27), MP_ROM_INT(GPIO4_27) },

  { MP_ROM_QSTR(MP_QSTR_GPIO4_28), MP_ROM_INT(GPIO4_28) },

  { MP_ROM_QSTR(MP_QSTR_GPIO4_29), MP_ROM_INT(GPIO4_29) },

  { MP_ROM_QSTR(MP_QSTR_GPIO4_30), MP_ROM_INT(GPIO4_30) },

  { MP_ROM_QSTR(MP_QSTR_GPIO4_31), MP_ROM_INT(GPIO4_31) },

  { MP_ROM_QSTR(MP_QSTR_GPIO5_0), MP_ROM_INT(GPIO5_0) },

  { MP_ROM_QSTR(MP_QSTR_GPIO5_1), MP_ROM_INT(GPIO5_1) },

  { MP_ROM_QSTR(MP_QSTR_GPIO5_2), MP_ROM_INT(GPIO5_2) },


}; /*GPIO dict table*/
