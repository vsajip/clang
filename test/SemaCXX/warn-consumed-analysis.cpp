// RUN: %clang_cc1 -fsyntax-only -verify -Wconsumed -fcxx-exceptions -std=c++11 %s

// TODO: Switch to using macros for the expected warnings.

#define CALLABLE_WHEN(...)      __attribute__ ((callable_when(__VA_ARGS__)))
#define CONSUMABLE(state)       __attribute__ ((consumable(state)))
#define PARAM_TYPESTATE(state)  __attribute__ ((param_typestate(state)))
#define RETURN_TYPESTATE(state) __attribute__ ((return_typestate(state)))
#define SET_TYPESTATE(state)    __attribute__ ((set_typestate(state)))
#define TEST_TYPESTATE(state)   __attribute__ ((test_typestate(state)))

typedef decltype(nullptr) nullptr_t;

template <typename T>
class CONSUMABLE(unconsumed) ConsumableClass {
  T var;
  
public:
  ConsumableClass();
  ConsumableClass(nullptr_t p) RETURN_TYPESTATE(consumed);
  ConsumableClass(T val) RETURN_TYPESTATE(unconsumed);
  ConsumableClass(ConsumableClass<T> &other);
  ConsumableClass(ConsumableClass<T> &&other);
  
  ConsumableClass<T>& operator=(ConsumableClass<T>  &other);
  ConsumableClass<T>& operator=(ConsumableClass<T> &&other);
  ConsumableClass<T>& operator=(nullptr_t) SET_TYPESTATE(consumed);
  
  template <typename U>
  ConsumableClass<T>& operator=(ConsumableClass<U>  &other);
  
  template <typename U>
  ConsumableClass<T>& operator=(ConsumableClass<U> &&other);
  
  void operator()(int a) SET_TYPESTATE(consumed);
  void operator*() const CALLABLE_WHEN("unconsumed");
  void unconsumedCall() const CALLABLE_WHEN("unconsumed");
  void callableWhenUnknown() const CALLABLE_WHEN("unconsumed", "unknown");
  
  bool isValid() const TEST_TYPESTATE(unconsumed);
  operator bool() const TEST_TYPESTATE(unconsumed);
  bool operator!=(nullptr_t) const TEST_TYPESTATE(unconsumed);
  bool operator==(nullptr_t) const TEST_TYPESTATE(consumed);
  
  void constCall() const;
  void nonconstCall();
  
  void consume() SET_TYPESTATE(consumed);
  void unconsume() SET_TYPESTATE(unconsumed);
};

class CONSUMABLE(unconsumed) DestructorTester {
public:
  DestructorTester() RETURN_TYPESTATE(unconsumed);
  DestructorTester(int);
  
  void operator*() CALLABLE_WHEN("unconsumed");
  
  ~DestructorTester() CALLABLE_WHEN("consumed");
};

void baf0(const ConsumableClass<int>  var);
void baf1(const ConsumableClass<int> &var);
void baf2(const ConsumableClass<int> *var);

void baf3(ConsumableClass<int>   var);
void baf4(ConsumableClass<int>  &var);
void baf5(ConsumableClass<int>  *var);
void baf6(ConsumableClass<int> &&var);

ConsumableClass<int> returnsUnconsumed() {
  return ConsumableClass<int>(); // expected-warning {{return value not in expected state; expected 'unconsumed', observed 'consumed'}}
}

ConsumableClass<int> returnsConsumed() RETURN_TYPESTATE(consumed);
ConsumableClass<int> returnsConsumed() {
  return ConsumableClass<int>();
}

ConsumableClass<int> returnsUnknown() RETURN_TYPESTATE(unknown);

void testInitialization() {
  ConsumableClass<int> var0;
  ConsumableClass<int> var1 = ConsumableClass<int>();
  
  var0 = ConsumableClass<int>();
  
  *var0; // expected-warning {{invalid invocation of method 'operator*' on object 'var0' while it is in the 'consumed' state}}
  *var1; // expected-warning {{invalid invocation of method 'operator*' on object 'var1' while it is in the 'consumed' state}}
  
  if (var0.isValid()) {
    *var0;
    *var1;
    
  } else {
    *var0; // expected-warning {{invalid invocation of method 'operator*' on object 'var0' while it is in the 'consumed' state}}
  }
}

void testDestruction() {
  DestructorTester D0(42), D1(42);
  
  *D0;
  *D1;
  
  DestructorTester D2;
  *D2;
  
  D0.~DestructorTester(); // expected-warning {{invalid invocation of method '~DestructorTester' on object 'D0' while it is in the 'unconsumed' state}}
  
  return; // expected-warning {{invalid invocation of method '~DestructorTester' on object 'D0' while it is in the 'unconsumed' state}} \
             expected-warning {{invalid invocation of method '~DestructorTester' on object 'D1' while it is in the 'unconsumed' state}} \
             expected-warning {{invalid invocation of method '~DestructorTester' on object 'D2' while it is in the 'unconsumed' state}}
}

void testTempValue() {
  *ConsumableClass<int>(); // expected-warning {{invalid invocation of method 'operator*' on a temporary object while it is in the 'consumed' state}}
}

void testSimpleRValueRefs() {
  ConsumableClass<int> var0;
  ConsumableClass<int> var1(42);
  
  *var0; // expected-warning {{invalid invocation of method 'operator*' on object 'var0' while it is in the 'consumed' state}}
  *var1;
  
  var0 = static_cast<ConsumableClass<int>&&>(var1);
  
  *var0;
  *var1; // expected-warning {{invalid invocation of method 'operator*' on object 'var1' while it is in the 'consumed' state}}
}

void testIfStmt() {
  ConsumableClass<int> var;
  
  if (var.isValid()) {
    *var;
  } else {
    *var; // expected-warning {{invalid invocation of method 'operator*' on object 'var' while it is in the 'consumed' state}}
  }
  
  if (!var.isValid()) {
    *var; // expected-warning {{invalid invocation of method 'operator*' on object 'var' while it is in the 'consumed' state}}
  } else {
    *var;
  }
  
  if (var) {
    // Empty
  } else {
    *var; // expected-warning {{invalid invocation of method 'operator*' on object 'var' while it is in the 'consumed' state}}
  }
  
  if (var != nullptr) {
    // Empty
  } else {
    *var; // expected-warning {{invalid invocation of method 'operator*' on object 'var' while it is in the 'consumed' state}}
  }
  
  if (var == nullptr) {
    *var; // expected-warning {{invalid invocation of method 'operator*' on object 'var' while it is in the 'consumed' state}}
  } else {
    // Empty
  }
}

void testComplexConditionals0() {
  ConsumableClass<int> var0, var1, var2;
  
  if (var0 && var1) {
    *var0;
    *var1;
    
  } else {
    *var0; // expected-warning {{invalid invocation of method 'operator*' on object 'var0' while it is in the 'consumed' state}}
    *var1; // expected-warning {{invalid invocation of method 'operator*' on object 'var1' while it is in the 'consumed' state}}
  }
  
  if (var0 || var1) {
    *var0;
    *var1;
    
  } else {
    *var0; // expected-warning {{invalid invocation of method 'operator*' on object 'var0' while it is in the 'consumed' state}}
    *var1; // expected-warning {{invalid invocation of method 'operator*' on object 'var1' while it is in the 'consumed' state}}
  }
  
  if (var0 && !var1) {
    *var0;
    *var1;
    
  } else {
    *var0; // expected-warning {{invalid invocation of method 'operator*' on object 'var0' while it is in the 'consumed' state}}
    *var1; // expected-warning {{invalid invocation of method 'operator*' on object 'var1' while it is in the 'consumed' state}}
  }
  
  if (var0 || !var1) {
    *var0; // expected-warning {{invalid invocation of method 'operator*' on object 'var0' while it is in the 'consumed' state}}
    *var1; // expected-warning {{invalid invocation of method 'operator*' on object 'var1' while it is in the 'consumed' state}}
    
  } else {
    *var0;
    *var1;
  }
  
  if (!var0 && !var1) {
    *var0; // expected-warning {{invalid invocation of method 'operator*' on object 'var0' while it is in the 'consumed' state}}
    *var1; // expected-warning {{invalid invocation of method 'operator*' on object 'var1' while it is in the 'consumed' state}}
    
  } else {
    *var0;
    *var1;
  }
  
  if (!var0 || !var1) {
    *var0; // expected-warning {{invalid invocation of method 'operator*' on object 'var0' while it is in the 'consumed' state}}
    *var1; // expected-warning {{invalid invocation of method 'operator*' on object 'var1' while it is in the 'consumed' state}}
    
  } else {
    *var0;
    *var1;
  }
  
  if (!(var0 && var1)) {
    *var0; // expected-warning {{invalid invocation of method 'operator*' on object 'var0' while it is in the 'consumed' state}}
    *var1; // expected-warning {{invalid invocation of method 'operator*' on object 'var1' while it is in the 'consumed' state}}
    
  } else {
    *var0;
    *var1;
  }
  
  if (!(var0 || var1)) {
    *var0; // expected-warning {{invalid invocation of method 'operator*' on object 'var0' while it is in the 'consumed' state}}
    *var1; // expected-warning {{invalid invocation of method 'operator*' on object 'var1' while it is in the 'consumed' state}}
    
  } else {
    *var0;
    *var1;
  }
  
  if (var0 && var1 && var2) {
    *var0;
    *var1;
    *var2;
    
  } else {
    *var0; // expected-warning {{invalid invocation of method 'operator*' on object 'var0' while it is in the 'consumed' state}}
    *var1; // expected-warning {{invalid invocation of method 'operator*' on object 'var1' while it is in the 'consumed' state}}
    *var2; // expected-warning {{invalid invocation of method 'operator*' on object 'var2' while it is in the 'consumed' state}}
  }
  
#if 0
  // FIXME: Get this test to pass.
  if (var0 || var1 || var2) {
    *var0;
    *var1;
    *var2;
    
  } else {
    *var0; // expected-warning {{invalid invocation of method 'operator*' on object 'var0' while it is in the 'consumed' state}}
    *var1; // expected-warning {{invalid invocation of method 'operator*' on object 'var1' while it is in the 'consumed' state}}
    *var2; // expected-warning {{invalid invocation of method 'operator*' on object 'var2' while it is in the 'consumed' state}}
  }
#endif
}

void testComplexConditionals1() {
  ConsumableClass<int> var0, var1, var2;
  
  // Coerce all variables into the unknown state.
  baf4(var0);
  baf4(var1);
  baf4(var2);
  
  if (var0 && var1) {
    *var0;
    *var1;
    
  } else {
    *var0; // expected-warning {{invalid invocation of method 'operator*' on object 'var0' while it is in the 'unknown' state}}
    *var1; // expected-warning {{invalid invocation of method 'operator*' on object 'var1' while it is in the 'unknown' state}}
  }
  
  if (var0 || var1) {
    *var0; // expected-warning {{invalid invocation of method 'operator*' on object 'var0' while it is in the 'unknown' state}}
    *var1; // expected-warning {{invalid invocation of method 'operator*' on object 'var1' while it is in the 'unknown' state}}
    
  } else {
    *var0; // expected-warning {{invalid invocation of method 'operator*' on object 'var0' while it is in the 'consumed' state}}
    *var1; // expected-warning {{invalid invocation of method 'operator*' on object 'var1' while it is in the 'consumed' state}}
  }
  
  if (var0 && !var1) {
    *var0;
    *var1; // expected-warning {{invalid invocation of method 'operator*' on object 'var1' while it is in the 'consumed' state}}
    
  } else {
    *var0; // expected-warning {{invalid invocation of method 'operator*' on object 'var0' while it is in the 'unknown' state}}
    *var1; // expected-warning {{invalid invocation of method 'operator*' on object 'var1' while it is in the 'unknown' state}}
  }
  
  if (var0 || !var1) {
    *var0; // expected-warning {{invalid invocation of method 'operator*' on object 'var0' while it is in the 'unknown' state}}
    *var1; // expected-warning {{invalid invocation of method 'operator*' on object 'var1' while it is in the 'unknown' state}}
    
  } else {
    *var0; // expected-warning {{invalid invocation of method 'operator*' on object 'var0' while it is in the 'consumed' state}}
    *var1;
  }
  
  if (!var0 && !var1) {
    *var0; // expected-warning {{invalid invocation of method 'operator*' on object 'var0' while it is in the 'consumed' state}}
    *var1; // expected-warning {{invalid invocation of method 'operator*' on object 'var1' while it is in the 'consumed' state}}
    
  } else {
    *var0; // expected-warning {{invalid invocation of method 'operator*' on object 'var0' while it is in the 'unknown' state}}
    *var1; // expected-warning {{invalid invocation of method 'operator*' on object 'var1' while it is in the 'unknown' state}}
  }
  
  if (!(var0 || var1)) {
    *var0; // expected-warning {{invalid invocation of method 'operator*' on object 'var0' while it is in the 'consumed' state}}
    *var1; // expected-warning {{invalid invocation of method 'operator*' on object 'var1' while it is in the 'consumed' state}}
    
  } else {
    *var0; // expected-warning {{invalid invocation of method 'operator*' on object 'var0' while it is in the 'unknown' state}}
    *var1; // expected-warning {{invalid invocation of method 'operator*' on object 'var1' while it is in the 'unknown' state}}
  }
  
  if (!var0 || !var1) {
    *var0; // expected-warning {{invalid invocation of method 'operator*' on object 'var0' while it is in the 'unknown' state}}
    *var1; // expected-warning {{invalid invocation of method 'operator*' on object 'var1' while it is in the 'unknown' state}}
    
  } else {
    *var0;
    *var1;
  }
  
  if (!(var0 && var1)) {
    *var0; // expected-warning {{invalid invocation of method 'operator*' on object 'var0' while it is in the 'unknown' state}}
    *var1; // expected-warning {{invalid invocation of method 'operator*' on object 'var1' while it is in the 'unknown' state}}
    
  } else {
    *var0;
    *var1;
  }
  
  if (var0 && var1 && var2) {
    *var0;
    *var1;
    *var2;
    
  } else {
    *var0; // expected-warning {{invalid invocation of method 'operator*' on object 'var0' while it is in the 'unknown' state}}
    *var1; // expected-warning {{invalid invocation of method 'operator*' on object 'var1' while it is in the 'unknown' state}}
    *var2; // expected-warning {{invalid invocation of method 'operator*' on object 'var2' while it is in the 'unknown' state}}
  }
  
#if 0
  // FIXME: Get this test to pass.
  if (var0 || var1 || var2) {
    *var0; // expected-warning {{invalid invocation of method 'operator*' on object 'var0' while it is in the 'unknown' state}}
    *var1; // expected-warning {{invalid invocation of method 'operator*' on object 'var1' while it is in the 'unknown' state}}
    *var2; // expected-warning {{invalid invocation of method 'operator*' on object 'var2' while it is in the 'unknown' state}}
    
  } else {
    *var0; // expected-warning {{invalid invocation of method 'operator*' on object 'var0' while it is in the 'consumed' state}}
    *var1; // expected-warning {{invalid invocation of method 'operator*' on object 'var1' while it is in the 'consumed' state}}
    *var2; // expected-warning {{invalid invocation of method 'operator*' on object 'var2' while it is in the 'consumed' state}}
  }
#endif
}

void testStateChangeInBranch() {
  ConsumableClass<int> var;
  
  // Make var enter the 'unknown' state.
  baf4(var);
  
  if (!var) {
    var = ConsumableClass<int>(42);
  }
  
  *var;
}

void testFunctionParam(ConsumableClass<int> param) {
  
  if (param.isValid()) {
    *param;
  } else {
    *param;
  }
  
  param = nullptr;
  *param; // expected-warning {{invocation of method 'operator*' on object 'param' while it is in the 'consumed' state}}
}

void testParamReturnTypestateCallee(bool cond, ConsumableClass<int> &Param RETURN_TYPESTATE(unconsumed)) { // expected-warning {{parameter 'Param' not in expected state when the function returns: expected 'unconsumed', observed 'consumed'}}
  
  if (cond) {
    Param.consume();
    return; // expected-warning {{parameter 'Param' not in expected state when the function returns: expected 'unconsumed', observed 'consumed'}}
  }
  
  Param.consume();
}

void testParamReturnTypestateCaller() {
  ConsumableClass<int> var;
  
  testParamReturnTypestateCallee(true, var);
  
  *var;
}

void testParamTypestateCallee(ConsumableClass<int>  Param0 PARAM_TYPESTATE(consumed),
                              ConsumableClass<int> &Param1 PARAM_TYPESTATE(consumed)) {
  
  *Param0; // expected-warning {{invalid invocation of method 'operator*' on object 'Param0' while it is in the 'consumed' state}}
  *Param1; // expected-warning {{invalid invocation of method 'operator*' on object 'Param1' while it is in the 'consumed' state}}
}

void testParamTypestateCaller() {
  ConsumableClass<int> Var0, Var1(42);
  
  testParamTypestateCallee(Var0, Var1); // expected-warning {{argument not in expected state; expected 'consumed', observed 'unconsumed'}}
}

void baf3(ConsumableClass<int> var) {
  *var;
}

void baf4(ConsumableClass<int> &var) {
  *var;  // expected-warning {{invalid invocation of method 'operator*' on object 'var' while it is in the 'unknown' state}}
}

void baf6(ConsumableClass<int> &&var) {
  *var;
}

void testCallingConventions() {
  ConsumableClass<int> var(42);
  
  baf0(var);  
  *var;
  
  baf1(var);  
  *var;
  
  baf2(&var);  
  *var;
  
  baf4(var);  
  *var; // expected-warning {{invalid invocation of method 'operator*' on object 'var' while it is in the 'unknown' state}}
  
  var = ConsumableClass<int>(42);
  baf5(&var);  
  *var; // expected-warning {{invalid invocation of method 'operator*' on object 'var' while it is in the 'unknown' state}}
  
  var = ConsumableClass<int>(42);
  baf6(static_cast<ConsumableClass<int>&&>(var));  
  *var; // expected-warning {{invalid invocation of method 'operator*' on object 'var' while it is in the 'consumed' state}}
}

void testConstAndNonConstMemberFunctions() {
  ConsumableClass<int> var(42);
  
  var.constCall();
  *var;
  
  var.nonconstCall();
  *var;
}

void testFunctionParam0(ConsumableClass<int> param) {
  *param;
}

void testFunctionParam1(ConsumableClass<int> &param) {
  *param; // expected-warning {{invalid invocation of method 'operator*' on object 'param' while it is in the 'unknown' state}}
}

void testReturnStates() {
  ConsumableClass<int> var;
  
  var = returnsUnconsumed();
  *var;
  
  var = returnsConsumed();
  *var; // expected-warning {{invalid invocation of method 'operator*' on object 'var' while it is in the 'consumed' state}}
}

void testCallableWhen() {
  ConsumableClass<int> var(42);
  
  *var;
  
  baf4(var);
  
  var.callableWhenUnknown();
}

void testMoveAsignmentish() {
  ConsumableClass<int>  var0;
  ConsumableClass<long> var1(42);
  
  *var0; // expected-warning {{invalid invocation of method 'operator*' on object 'var0' while it is in the 'consumed' state}}
  *var1;
  
  var0 = static_cast<ConsumableClass<long>&&>(var1);
  
  *var0;
  *var1; // expected-warning {{invalid invocation of method 'operator*' on object 'var1' while it is in the 'consumed' state}}
  
  var1 = ConsumableClass<long>(42);
  var1 = nullptr;
  *var1; // expected-warning {{invalid invocation of method 'operator*' on object 'var1' while it is in the 'consumed' state}}
}

void testConditionalMerge() {
  ConsumableClass<int> var;
  
  if (var.isValid()) {
    // Empty
  }
  
  *var; // expected-warning {{invalid invocation of method 'operator*' on object 'var' while it is in the 'consumed' state}}
  
  if (var.isValid()) {
    // Empty
  } else {
    // Empty
  }
  
  *var; // expected-warning {{invalid invocation of method 'operator*' on object 'var' while it is in the 'consumed' state}}
}

void testSetTypestate() {
  ConsumableClass<int> var(42);
  
  *var;
  
  var.consume();
  
  *var; // expected-warning {{invalid invocation of method 'operator*' on object 'var' while it is in the 'consumed' state}}
  
  var.unconsume();
  
  *var;
}

void testConsumes0() {
  ConsumableClass<int> var(nullptr);
  
  *var; // expected-warning {{invalid invocation of method 'operator*' on object 'var' while it is in the 'consumed' state}}
}

void testConsumes1() {
  ConsumableClass<int> var(42);
  
  var.unconsumedCall();
  var(6);
  
  var.unconsumedCall(); // expected-warning {{invalid invocation of method 'unconsumedCall' on object 'var' while it is in the 'consumed' state}}
}

void testUnreachableBlock() {
  ConsumableClass<int> var(42);
  
  if (var) {
    *var;
  } else {
    *var;
  }
  
  *var;
}


void testForLoop1() {
  ConsumableClass<int> var0, var1(42);
  
  for (int i = 0; i < 10; ++i) { // expected-warning {{state of variable 'var1' must match at the entry and exit of loop}}
    *var0; // expected-warning {{invalid invocation of method 'operator*' on object 'var0' while it is in the 'consumed' state}}
    
    *var1;
    var1.consume();
    *var1; // expected-warning {{invalid invocation of method 'operator*' on object 'var1' while it is in the 'consumed' state}}
  }
  
  *var0; // expected-warning {{invalid invocation of method 'operator*' on object 'var0' while it is in the 'consumed' state}}
}

void testWhileLoop1() {
  int i = 10;
  
  ConsumableClass<int> var0, var1(42);
  
  while (i-- > 0) { // expected-warning {{state of variable 'var1' must match at the entry and exit of loop}}
    *var0; // expected-warning {{invalid invocation of method 'operator*' on object 'var0' while it is in the 'consumed' state}}
    
    *var1;
    var1.consume();
    *var1; // expected-warning {{invalid invocation of method 'operator*' on object 'var1' while it is in the 'consumed' state}}
  }
  
  *var0; // expected-warning {{invalid invocation of method 'operator*' on object 'var0' while it is in the 'consumed' state}}
}


namespace ContinueICETest {

bool cond1();
bool cond2();

static void foo1() {
  while (cond1()) {
    if (cond2())
      continue;
  }
}

static void foo2() {
  while (true) {
    if (false)
      continue;
  }
}

class runtime_error
{
public:
  virtual ~runtime_error();
};

void read(bool sf) {
    while (sf) {
        if(sf) throw runtime_error();
    }
}

} // end namespace ContinueICETest


namespace InitializerAssertionFailTest {

class CONSUMABLE(unconsumed) Status {
  int code;

public:
  Status() RETURN_TYPESTATE(consumed);
  Status(int c) RETURN_TYPESTATE(unconsumed);

  Status(const Status &other);
  //Status(Status &&other);

  Status& operator=(const Status &other) CALLABLE_WHEN("unknown", "consumed");
  //Status& operator=(Status &&other) CALLABLE_WHEN("unknown", "consumed");

  bool check()  const SET_TYPESTATE(consumed);
  void ignore() const SET_TYPESTATE(consumed);
  // Status& markAsChecked() { return *this; }

  void clear() CALLABLE_WHEN("unknown", "consumed") SET_TYPESTATE(consumed);

  ~Status() CALLABLE_WHEN("unknown", "consumed");
};


bool   cond();
Status doSomething();
void   handleStatus(const Status& s);
void   handleStatusPtr(const Status* s);

int a;


void test() {
  if (cond()) {
    Status s = doSomething();
    return;                     // Warning: Store it, but don't check.
  }
}

} // end namespace InitializerAssertionFailTest

