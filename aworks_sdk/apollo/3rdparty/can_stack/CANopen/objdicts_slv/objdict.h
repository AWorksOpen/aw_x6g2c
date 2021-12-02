/************************************************************
* File: D:\PROJEKTE\CANopen_V5\Objdicts\DS401_3P\objdict.h 
* was automatically generated by ODBuilder
* from (c) SYS TEC electronic GmbH   D-07973 Greiz
* August-Bebel-Strasse 29 
*************************************************************
* !THE FILE MUST NOT BE CHANGED!
*************************************************************
* Date: 14.11.2002
* Time: 14:46:03
************************************************************/ 

#define OBD_DEFINE_MACRO
    #include "obdmacro.h"
#undef OBD_DEFINE_MACRO

OBD_BEGIN ()

    OBD_BEGIN_PART_GENERIC ()

        OBD_BEGIN_INDEX_ROM(0x1000, 0x01, NULL)
            OBD_SUBINDEX_RAM_VAR(0x1000, 0x00, 0x07, 0x01, tObdUnsigned32, Device_Type, 0xF0191)
        OBD_END_INDEX(0x1000)

        OBD_BEGIN_INDEX_ROM(0x1001, 0x01, NULL)
            OBD_SUBINDEX_RAM_VAR(0x1001, 0x00, 0x05, 0x01, tObdUnsigned8, Error_Register, 0x0)
        OBD_END_INDEX(0x1001)

        OBD_BEGIN_INDEX_ROM(0x1003, 0x05, EmcpCbPreDefinedErrorField)
            OBD_SUBINDEX_RAM_VAR(0x1003, 0x00, 0x05, 0x03, tObdUnsigned8, number_of_errors, 0x4)
            OBD_SUBINDEX_RAM_VAR(0x1003, 0x01, 0x07, 0x01, tObdUnsigned32, standard_error_field, 0x0)
            OBD_SUBINDEX_RAM_VAR(0x1003, 0x02, 0x07, 0x01, tObdUnsigned32, standard_error_field, 0x0)
            OBD_SUBINDEX_RAM_VAR(0x1003, 0x03, 0x07, 0x01, tObdUnsigned32, standard_error_field, 0x0)
            OBD_SUBINDEX_RAM_VAR(0x1003, 0x04, 0x07, 0x01, tObdUnsigned32, standard_error_field, 0x0)
        OBD_END_INDEX(0x1003)

        OBD_BEGIN_INDEX_ROM(0x1005, 0x01, PdoCbCfgSyncId)
            OBD_SUBINDEX_RAM_VAR(0x1005, 0x00, 0x07, 0x83, tObdUnsigned32, COBID_SYNC, 0x80)
        OBD_END_INDEX(0x1005)

        OBD_BEGIN_INDEX_ROM(0x1006, 0x01, PdoCbCfgSyncCycle)
            OBD_SUBINDEX_RAM_VAR(0x1006, 0x00, 0x07, 0x83, tObdUnsigned32, Communication_cycle_period, 0x0)
        OBD_END_INDEX(0x1006)

        OBD_BEGIN_INDEX_ROM(0x1007, 0x01, PdoCbCfgSyncWindow)
            OBD_SUBINDEX_RAM_VAR(0x1007, 0x00, 0x07, 0x83, tObdUnsigned32, Window_Length, 0x0)
        OBD_END_INDEX(0x1007)

        OBD_BEGIN_INDEX_ROM(0x1008, 0x01, NULL)
           OBD_SUBINDEX_ROM_VSTRING(0x1008, 0x00, 0x01, device_name,"CANopen Slave dynOD")
        OBD_END_INDEX(0x1008)

        OBD_BEGIN_INDEX_ROM(0x1009, 0x01, NULL)
           OBD_SUBINDEX_ROM_VSTRING(0x1009, 0x00, 0x01, hardware_version,"V1.00")
        OBD_END_INDEX(0x1009)

        OBD_BEGIN_INDEX_ROM(0x100A, 0x01, NULL)
           OBD_SUBINDEX_ROM_VSTRING(0x100A, 0x00, 0x01, software_version, DEFINED_STRING_VERSION)
        OBD_END_INDEX(0x100A)

        OBD_BEGIN_INDEX_ROM(0x100C, 0x01, NmtsCbConfigLifeGuarding)
            OBD_SUBINDEX_RAM_VAR(0x100C, 0x00, 0x06, 0x83, tObdUnsigned16, Guard_Time, 0x0)
        OBD_END_INDEX(0x100C)

        OBD_BEGIN_INDEX_ROM(0x100D, 0x01, NmtsCbConfigLifeGuarding)
            OBD_SUBINDEX_RAM_VAR(0x100D, 0x00, 0x05, 0x83, tObdUnsigned8, Life_Time_Factor, 0x0)
        OBD_END_INDEX(0x100D)

        OBD_BEGIN_INDEX_ROM(0x1010, 0x02, CcmCbStore)
            OBD_SUBINDEX_ROM_VAR(0x1010, 0x00, 0x05, 0x01, tObdUnsigned8, largest_supported_SubIndex, 0x1)
            OBD_SUBINDEX_RAM_VAR(0x1010, 0x01, 0x07, 0x03, tObdUnsigned32, save_all_parameters, 0x0)
        OBD_END_INDEX(0x1010)

        OBD_BEGIN_INDEX_ROM(0x1011, 0x02, CcmCbRestore)
            OBD_SUBINDEX_ROM_VAR(0x1011, 0x00, 0x05, 0x01, tObdUnsigned8, largest_supported_SubIndex, 0x1)
            OBD_SUBINDEX_RAM_VAR(0x1011, 0x01, 0x07, 0x03, tObdUnsigned32, restore_all_default_para, 0x0)
        OBD_END_INDEX(0x1011)

        OBD_BEGIN_INDEX_ROM(0x1014, 0x01, EmcpCbConfigEmergencyId)
            OBD_SUBINDEX_RAM_VAR(0x1014, 0x00, 0x07, 0x83, tObdUnsigned32, COBID_Emergency_message, 0x80000000)
        OBD_END_INDEX(0x1014)

        OBD_BEGIN_INDEX_ROM(0x1016, 0x02, HbcCbConfigHeartbeat)
            OBD_SUBINDEX_RAM_VAR(0x1016, 0x00, 0x05, 0x01, tObdUnsigned8, Number_Of_Entries, 0x1)
            OBD_SUBINDEX_RAM_VAR(0x1016, 0x01, 0x07, 0x83, tObdUnsigned32, Consumer_Heartbeat_Time_1, 0x0)
        OBD_END_INDEX(0x1016)

        OBD_BEGIN_INDEX_ROM(0x1017, 0x01, HbpCbConfigHeartbeat)
            OBD_SUBINDEX_RAM_VAR(0x1017, 0x00, 0x06, 0x83, tObdUnsigned16, Producer_Heartbeat_Time, 0x0)
        OBD_END_INDEX(0x1017)

        OBD_BEGIN_INDEX_ROM(0x1018, 0x05, NULL)
            OBD_SUBINDEX_ROM_VAR(0x1018, 0x00, 0x05, 0x01, tObdUnsigned8,  Number_Of_Entries, 0x4)
            OBD_SUBINDEX_RAM_VAR(0x1018, 0x01, 0x07, 0x01, tObdUnsigned32, VendorID, 0x3F)
            OBD_SUBINDEX_RAM_VAR(0x1018, 0x02, 0x07, 0x01, tObdUnsigned32, Product_code, 0x0)
            OBD_SUBINDEX_RAM_VAR(0x1018, 0x03, 0x07, 0x01, tObdUnsigned32, Revision_number, DEFINED_OBJ1018_VERSION)
            OBD_SUBINDEX_RAM_VAR(0x1018, 0x04, 0x07, 0x01, tObdUnsigned32, Serial_number, 0x1)
        OBD_END_INDEX(0x1018)

    OBD_END_PART ()

    OBD_BEGIN_PART_MANUFACTURER ()

    OBD_END_PART ()

    OBD_BEGIN_PART_DEVICE ()

    OBD_END_PART ()

OBD_END ()

#define OBD_UNDEFINE_MACRO
    #include "obdmacro.h"
#undef OBD_UNDEFINE_MACRO
