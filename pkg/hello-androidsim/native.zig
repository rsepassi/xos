const jni = @cImport(@cInclude("jni.h"));

export fn Java_com_xos_hello_MainActivity_stringFromJNI(env: *jni.JNIEnv, this: jni.jobject) jni.jstring {
    _ = this;
    const hello = "Hello from Zig2!";
    return env.*.*.NewStringUTF.?(env, hello);
}
