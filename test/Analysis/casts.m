// RUN: %clang_cc1 -analyze -analyzer-checker=core,alpha.core -analyzer-store=region -verify %s

// Test function pointer casts.  Currently we track function addresses using
// loc::FunctionVal.  Because casts can be arbitrary, do we need to model
// functions with regions?
typedef void* (*MyFuncTest1)(void);

MyFuncTest1 test1_aux(void);
void test1(void) {
  void *x;
  void* (*p)(void);
  p = ((void*) test1_aux());
  if (p != ((void*) 0)) x = (*p)();
}

// Test casts from void* to function pointers.  Same issue as above:
// should we eventually model function pointers using regions?
void* test2(void *p) {
  MyFuncTest1 fp = (MyFuncTest1) p;
  return (*fp)();
}

// <radar://10087620>
// A cast from int onjective C property reference to int.
typedef signed char BOOL;
@protocol NSObject  - (BOOL)isEqual:(id)object; @end
@interface NSObject <NSObject> {} - (id)init; @end
typedef enum {
  EEOne,
  EETwo
} RDR10087620Enum;
@interface RDR10087620 : NSObject {
  RDR10087620Enum   elem;
}
@property (readwrite, nonatomic) RDR10087620Enum elem;
@end

static void
adium_media_ready_cb(RDR10087620 *InObj)
{
  InObj.elem |= EEOne;
}
