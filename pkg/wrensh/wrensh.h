typedef struct uv_loop_s uv_loop_t;
typedef struct WrenHandle WrenHandle;

typedef struct Node {
  void* data;
  struct Node* next;
} Node;

typedef struct {
  int argc;
  void* argv;

  uv_loop_t* loop;

  void* stdio;

  WrenHandle* wren_tx_val;
  WrenHandle* wren_tx_err;
  WrenHandle* wren_tx;
  WrenHandle* wren_call;
  WrenHandle* wren_call_val;
  WrenHandle* wren_call2_val;

  Node* garbage;
} Ctx;

void* setupStdio(uv_loop_t* loop);
void cleanupStdio(void* ctx);
void cleanupUV(Ctx* ctx);
void cleanupGarbage(Ctx* ctx);
