#ifndef __AW_CHECK_H
#define __AW_CHECK_H

#define __CHECK_PRINTF aw_kprintf
#define __CHECK_MAKESTR(x)  #x

#define ERR_CHECK( x )  if( ( x ) ) { \
    __CHECK_PRINTF("%s@%d::Err %s return \r\n",__func__,__LINE__, \
    __CHECK_MAKESTR(x) );\
    goto cleanup; };
	


#endif	/* __AW_BSP_ADC_H */
