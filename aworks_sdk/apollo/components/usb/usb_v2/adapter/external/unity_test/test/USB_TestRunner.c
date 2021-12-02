//- Copyright (c) 2010 James Grenning and Contributed to Unity Project
/* ==========================================
    Unity Project - A Test Framework for C
    Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
    [Released under MIT License. Please refer to license.txt for details]
========================================== */

#include "unity_fixture.h"

TEST_GROUP_RUNNER(USB_HOST_DEV)
{
//    RUN_TEST_CASE(UnityFixture, PointerSetting);
//    RUN_TEST_CASE(UnityFixture, ForceMallocFail);
//    RUN_TEST_CASE(UnityFixture, ReallocSmallerIsUnchanged);
//    RUN_TEST_CASE(UnityFixture, ReallocSameIsUnchanged);
//    RUN_TEST_CASE(UnityFixture, ReallocLargerNeeded);
//    RUN_TEST_CASE(UnityFixture, ReallocNullPointerIsLikeMalloc);
//    RUN_TEST_CASE(UnityFixture, ReallocSizeZeroFreesMemAndReturnsNullPointer);
//    RUN_TEST_CASE(UnityFixture, CallocFillsWithZero);
//    RUN_TEST_CASE(UnityFixture, PointerSet);
//    RUN_TEST_CASE(UnityFixture, FreeNULLSafety);
//    RUN_TEST_CASE(UnityFixture, ConcludeTestIncrementsFailCount);
}

/** USB主机接口测试*/
TEST_GROUP_RUNNER(USB_HOST){
    RUN_TEST_CASE(USB_HOST, USBH_Info_Get);
    RUN_TEST_CASE(USB_HOST, USBH_Create_Destory_Test);
    RUN_TEST_CASE(USB_HOST, USBH_Addr_Alloc_Free_Test);
    RUN_TEST_CASE(USB_HOST, USBH_FrameNum_Get);
    RUN_TEST_CASE(USB_HOST, USBH_EP_Enable);
    RUN_TEST_CASE(USB_HOST, USBH_EP_Disable);
    RUN_TEST_CASE(USB_HOST, USBH_Request);
    RUN_TEST_CASE(USB_HOST, USBH_Cancel);
}
//
//TEST_GROUP_RUNNER(LeakDetection)
//{
//    RUN_TEST_CASE(LeakDetection, DetectsLeak);
//    RUN_TEST_CASE(LeakDetection, BufferOverrunFoundDuringFree);
//    RUN_TEST_CASE(LeakDetection, BufferOverrunFoundDuringRealloc);
//    RUN_TEST_CASE(LeakDetection, BufferGuardWriteFoundDuringFree);
//    RUN_TEST_CASE(LeakDetection, BufferGuardWriteFoundDuringRealloc);
//    RUN_TEST_CASE(LeakDetection, PointerSettingMax);
//}
//
//TEST_GROUP_RUNNER(InternalMalloc)
//{
//    RUN_TEST_CASE(InternalMalloc, MallocPastBufferFails);
//    RUN_TEST_CASE(InternalMalloc, CallocPastBufferFails);
//    RUN_TEST_CASE(InternalMalloc, MallocThenReallocGrowsMemoryInPlace);
//    RUN_TEST_CASE(InternalMalloc, ReallocFailDoesNotFreeMem);
//}
