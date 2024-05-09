foreign class KV {
  construct new(path) {}

  foreign get(key)
  foreign getp(prefix)
  foreign set(key, val)
}

class JSON {
  foreign static encode(obj)
  foreign static decode(str)
}

foreign class Watcher {
  construct new(path, fn) {}
  foreign cancel()
}

foreign class Trap {
  construct new(signal, fn) {}
  foreign cancel()
}

class AsyncValue {
  construct new() {
    _done = false
  }

  err=(err) {
    if (!(err is Null)) {
      _done = true
      _err = err
    }
  }
  val=(val) {
    _done = true
    _val = val
  }

  await() {
    while (!_done) Fiber.yield()
    if (!(_err is Null)) Fiber.abort(_err)
    return _val
  }
}

foreign class Timer {
  construct new() {}
  foreign lap()
  foreign read()
  foreign reset()
}

class X_ {
  construct new() {}

  async(fn) {
    var val = AsyncValue.new()
    var parent = Fiber.current
    var f = Fiber.new {
      val.val = fn.call()
      parent.transfer()
    }
    val.err = f.try()
    return val
  }

  await(vals) {
    var outs = []
    for (v in vals) {
      outs.add(v.await())
    }
    return outs
  }
}
var X = X_.new()

class ProcessBuilder {
  construct new(args) {
    _args = args
  }
  env(e) {
    _env = e
    return this
  }
  stdout(filename) {
    _stdout = filename
    return this
  }
  stderr(filename) {
    _stderr = filename
    return this
  }
  run() {
    return IO.run_wrap_(_args, _env, false, _stdout, _stderr)
  }
  runc() {
    return IO.run_wrap_(_args, _env, true, _stdout, _stderr)
  }
  test() {
    var out = IO.run_wrap_(_args, _env, true, "/dev/null", "/dev/null")
    return out == 0
  }
}

class Data {
  static Record(name, fields) {
    if (fields is String) {
      fields = fields.split(" ")
    }

    var class_str = "class %(name) {\n"

    // new
    if (fields.count <= 16) {
      var fields_init = fields.join(", ")
      var sets = fields.map { |x| "      \"%(x)\": %(x),\n" }
      class_str = class_str + "  construct new(%(fields_init)) {\n"
      class_str = class_str + "    _data = {\n"
      class_str = class_str + "%(sets.join(""))"
      class_str = class_str + "    }\n"
      class_str = class_str + "  }\n"
    }

    if (fields.count > 1) {
      // new from map
      class_str = class_str + "  construct new(map) {\n"
      class_str = class_str + "    _data = map\n"
      class_str = class_str + "  }\n"
    }

    // getters
    var getters = fields.map { |x| "  %(x) { _data[\"%(x)\"] }\n" }
    class_str = class_str + "%(getters.join(""))"

    // setters
    var setters = fields.map { |x| "  %(x)=(val) { _data[\"%(x)\"] = val }\n" }
    class_str = class_str + "%(setters.join(""))"

    // equality
    var eqstrs = fields.map { |x| "_data[\"%(x)\"] == other.%(x)" }
    class_str = class_str + "  ==(other) {\n"
    class_str = class_str + "    return other is %(name) && " + eqstrs.join(" && ") + "\n"
    class_str = class_str + "  }\n"
    class_str = class_str + "  !=(other) {\n    return !(this == other)\n  }\n"

    // toString
    class_str = class_str + "  toString {\n"
    class_str = class_str + "    return \"%(name)(\%(_data))\"\n"
    class_str = class_str + "  }\n"

    class_str = class_str + "}\n"

    Meta.eval(class_str)
    return Meta.compileExpression(name).call()
  }
}

class IO {
  foreign static arg(i)
  foreign static argc()
  foreign static args()
  foreign static env()
  foreign static env(name)

  static read() {
    read_(Fiber.current)
    return Fiber.yield()
  }

  static read(f) {
    read_(Fiber.current, f)
    return Fiber.yield()
  }

  static write(f, contents) {
    write_(Fiber.current, f, contents)
    return Fiber.yield()
  }

  static append(f, contents) {
    append_(Fiber.current, f, contents)
    return Fiber.yield()
  }

  static write(s) {
    if (!(s is String)) s = "%(s)"
    write_(Fiber.current, s)
    Fiber.yield()
  }

  static writeln(s) {
    write_(Fiber.current, "%(s)\n")
    Fiber.yield()
  }

  foreign static flush()

  foreign static cwd()
  foreign static chdir(dir)

  static Process(args) {
    return ProcessBuilder.new(args)
  }

  static Timer() {
    return Timer.new()
  }

  static run(argv) {
    return run_wrap_(argv, null, false, null, null)
  }

  static run(argv, env) {
    return run_wrap_(argv, env, false, null, null)
  }

  static runc(argv) {
    return run_wrap_(argv, null, true, "/dev/null", "/dev/null")
  }

  static runc(argv, env) {
    return run_wrap_(argv, env, true, "/dev/null", "/dev/null")
  }

  static exec(argv) {
    exec(argv, null)
  }
  static exec(argv, env) {
    argv = normalize_argv_(argv)
    env = normalize_env_(env)
    var exe = IO.run(["which", argv[0]]).trim()
    argv[0] = exe
    exec_(argv, env)
  }

  static sleep(n) {
    sleep_(Fiber.current, n)
    Fiber.yield()
  }

  static trap(sig, fn) {
    return Trap.new(sig, fn)
  }

  static watch(path, fn) {
    return Watcher.new(path, fn)
  }

  foreign static glob(pattern)
  foreign static glob(pattern, dir)

  foreign static exit(c)

  // private
  foreign static read_(f)
  foreign static read_(f, file)
  foreign static write_(f, s)
  foreign static write_(f, file, contents)
  foreign static append_(f, file, contents)
  foreign static sleep_(f, n)
  foreign static exec_(args, env)
  foreign static run_(f, args, env, rc, stdout, stderr)

  static run_wrap_(argv, env, rc, stdout, stderr) {
    argv = normalize_argv_(argv)
    env = normalize_env_(env)
    run_(Fiber.current, argv, env, rc, stdout, stderr)
    return Fiber.yield()
  }
  static normalize_argv_(argv) {
    if (argv is String) argv = argv.split(" ")
    return argv
  }
  static normalize_env_(env) {
    var envl = env
    if (env is Map) {
      envl = []
      for (x in env) {
        envl.add("%(x.key)=%(x.value)")
      }
    }
    return envl
  }
}
