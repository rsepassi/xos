typedef struct MDB_env MDB_env;
typedef struct WrenVM WrenVM;
typedef struct uv_loop_s uv_loop_t;
typedef struct WrenHandle WrenHandle;

extern const char* wrensh_src_usage;
extern const char* wrensh_src_user;
extern const char* wrensh_src_meta;

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
  WrenHandle* wren_map_keys;

  Node* garbage;

  MDB_env* kv;
} Ctx;

WrenVM* setupWren(Ctx* ctx);
void* setupStdio(uv_loop_t* loop);
void cleanupStdio(void* ctx);
void cleanupUV(Ctx* ctx);
void cleanupGarbage(Ctx* ctx);
void cleanupWren(WrenVM* vm);
