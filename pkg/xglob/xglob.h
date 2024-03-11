typedef struct xglob_s xglob_ctx;
xglob_ctx* xglob_init(const char* pattern, const char* dir);
char* xglob_next(xglob_ctx*);
void xglob_free(xglob_ctx*);
