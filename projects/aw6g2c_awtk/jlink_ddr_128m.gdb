#connect to jlink jdbserver
target remote localhost:2331

#monitor long 0x800401e0 = 0x00000002

#reset target
monitor reset
monitor sleep 300

#select endian
monitor endian little

# Set JTAG speed to auto
monitor speed 3000

#initilized memery contror
#monitor long 0x53000000 = 0x00000000

#wait for moment¡§
monitor sleep 20

monitor long 0x020c4068=0xffffffff
monitor long 0x020c406c=0xffffffff
monitor long 0x020c4070=0xffffffff
monitor long 0x020c4074=0xffffffff
monitor long 0x020c4078=0xffffffff
monitor long 0x020c407c=0xffffffff
monitor long 0x020c4080=0xffffffff
monitor long 0x020c4084=0xffffffff

monitor long 0x020E04B4=0x000C0000
monitor long 0x020E04AC=0x00000000
monitor long 0x020E027C=0x00000030
monitor long 0x020E0250=0x00000030
monitor long 0x020E024C=0x00000030
monitor long 0x020E0490=0x00000030
monitor long 0x020E0288=0x00000030
monitor long 0x020E0270=0x00000000
monitor long 0x020E0260=0x00000030
monitor long 0x020E0264=0x00000030
monitor long 0x020E04A0=0x00000030
monitor long 0x020E0494=0x00020000
monitor long 0x020E0280=0x00000030
monitor long 0x020E0284=0x00000030
monitor long 0x020E04B0=0x00020000
monitor long 0x020E0498=0x00000030
monitor long 0x020E04A4=0x00000030
monitor long 0x020E0244=0x00000030
monitor long 0x020E0248=0x00000030

monitor long 0x021B001C=0x00008000
monitor long 0x021B0800=0xA1390003
monitor long 0x021B080C=0x001F0000
monitor long 0x021B083C=0x41380138
monitor long 0x021B0848=0x40404A46
monitor long 0x021B0850=0x40404C44
monitor long 0x021B081C=0x33333333
monitor long 0x021B0820=0x33333333
monitor long 0x021B082C=0xf3333333
monitor long 0x021B0830=0xf3333333
monitor long 0x021B08C0=0x00921012
monitor long 0x021B0858=0x00000F00
monitor long 0x021B08b8=0x00000800
monitor long 0x021B0004=0x0003003F
monitor long 0x021B0008=0x2D555050
monitor long 0x021B000C=0x4249A013
monitor long 0x021B0010=0xFFF71425
monitor long 0x021B0014=0x01FF00DB
monitor long 0x021B0018=0x00201740
monitor long 0x021B001C=0x00008000
monitor long 0x021B002C=0x000026D2
monitor long 0x021B0030=0x00491023
monitor long 0x021B0040=0x0000004F
monitor long 0x021B0000=0x84180000
monitor long 0x021B001C=0x02108032
monitor long 0x021B001C=0x00008033
monitor long 0x021B001C=0x00048031
monitor long 0x021B001C=0x1B208030
monitor long 0x021B001C=0x04008040
monitor long 0x021B0020=0x00000800
monitor long 0x021B0818=0x00000227
monitor long 0x021B0004=0x0003557F
monitor long 0x021B0404=0x00011006
monitor long 0x021B001C=0x00000000


monitor sleep 500

#set cpu to svc mode(on cpu reset)
monitor reg cpsr = 0xd3


# Set break points
#break _start
#continue

#debug begin
